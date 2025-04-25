#!/bin/bash
#SBATCH --job-name=Stencil2D_All
#SBATCH --output=results/stencil2d_all_%j.out
#SBATCH --mail-user=jjohns7@coastal.edu
#SBATCH --mail-type=BEGIN,END
#SBATCH --partition=compute
#SBATCH --time=00:30:00
#SBATCH --mem=128GB
#SBATCH --nodes=2               # max you’ll ever need
#SBATCH --ntasks-per-node=8     # max threads/node
#SBATCH --cpus-per-task=16
#SBATCH --account=ccu108
#SBATCH --export=ALL

set -e
module purge
module load cpu/0.17.3b
module load slurm/22.05.8
module load gpu/0.17.3b
module load gcc/10.2.0/
module load openmpi/4.1.3
module load mpip/3.5

# Parameters
#MATRIX_SIZES=(5000 10000 20000 40000)
#THREAD_COUNTS=(1 2 4 8 16)
# --time=12:30:00
# --partition=compute
# --time=12:30:00
# --mem=128GB
# --nodes=2               # max you’ll ever need
# --ntasks-per-node=8     # max threads/node
# --cpus-per-task=16

MATRIX_SIZES=(5000)
THREAD_COUNTS=(1)
IMPLEMENTATIONS=(stencil-2d stencil-2d-omp stencil-2d-pth stencil-2d-mpi stencil-2d-hybrid)
INPUT=input_matrix.bin
OUTPUT=output_matrix.bin
RESULTS=results/computation_times.csv
t=8  # number of iterations

# prepare
rm -rf frames/*
mkdir -p results
echo "impl,n,p,t,overall,computation,other" > $RESULTS

for impl in "${IMPLEMENTATIONS[@]}"; do
  for n in "${MATRIX_SIZES[@]}"; do
      # make the input matrix
      ./make-2d $INPUT $n $n
      
      # run the serial version only the number of matrix sizes
      if [[ "$impl" == "stencil-2d" ]]; then
        ./stencil-2d $t $INPUT $OUTPUT 0
      fi
    
    for p in "${THREAD_COUNTS[@]}"; do
      echo "=== $impl, n=$n, p=$p, t=$t ==="

        if [[ "$impl" == "stencil-2d" ]]; then
          # Already run above; skip in thread-count loop
          continue
      fi

      if [[ "$impl" == "stencil-2d-mpi" ]]; then
        mpiexec -n $p ./$impl $t $INPUT $OUTPUT 0 $p

      elif [[ "$impl" == "stencil-2d-hybrid" ]]; then
        # Choose MPI ranks and OpenMP threads
        if [[ $p -eq 1 ]]; then
          ntasks=1; threads=1
        elif [[ $p -eq 2 ]]; then
          ntasks=1; threads=2
        elif [[ $p -eq 4 ]]; then
          ntasks=2; threads=2
        elif [[ $p -eq 8 ]]; then
          ntasks=2; threads=4
        elif [[ $p -eq 16 ]]; then
          ntasks=4; threads=4
        else
          echo "Unsupported thread count $p for hybrid. Skipping."
          continue
        fi

        export OMP_NUM_THREADS=$threads
        mpiexec -n $ntasks ./$impl $t $INPUT $OUTPUT 0 $threads

      else
        # For OpenMP or Pthread (not MPI)
        export OMP_NUM_THREADS=$p
        ./$impl $t $INPUT $OUTPUT 0 $p
      fi

      # Extract timing information for the current run
      T_OVR=$(grep "Total time" results/stencil2d_all_${SLURM_JOB_ID}.out | tail -n1 | awk '{print $3}' | tr -d '\n')
      T_CMP=$(grep "Work time" results/stencil2d_all_${SLURM_JOB_ID}.out | tail -n1 | awk '{print $3}' | tr -d '\n')
      T_OTH=$(echo "$T_OVR - $T_CMP" | bc | tr -d '\n')

      # Append results to the CSV file
      echo "$impl,$n,$p,$t,$T_OVR,$T_CMP,$T_OTH" >> $RESULTS
    done

    if [[ "$impl" == "stencil-2d" ]]; then
      DEST=results/frames_${n}
      mkdir -p "$DEST"
      mv frames/* "$DEST"/
      echo "→ moved $(ls -1 "$DEST" | wc -l) frames to $DEST"
    fi
  done
done

echo "Done. All results in $RESULTS"