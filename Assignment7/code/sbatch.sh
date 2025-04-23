#!/bin/bash
#SBATCH --job-name=Stencil2D_All
#SBATCH --output=results/stencil2d_all_%j.out
#SBATCH --mail-user=jjohns7@coastal.edu
#SBATCH --mail-type=BEGIN,END
#SBATCH --partition=compute
#SBATCH --time=00:30:00
#SBATCH --mem=128GB
#SBATCH --nodes=1               # max you’ll ever need
#SBATCH --ntasks-per-node=1     # max threads/node
#SBATCH --cpus-per-task=1
#SBATCH --account=ccu108
#SBATCH --export=ALL

set -e
module purge
module load gcc/10.2.0 
module load cpu/0.15.4 
module load openmpi/4.1.3
module load mpip/3.4.1

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
t=8   # your tuned iteration count

# prepare
mkdir -p results
echo "impl,n,p,t,overall,computation,other" > $RESULTS

for impl in "${IMPLEMENTATIONS[@]}"; do
  for n in "${MATRIX_SIZES[@]}"; do
    for p in "${THREAD_COUNTS[@]}"; do
      echo "=== $impl, n=$n, p=$p, t=$t ==="
      ./make-2d $INPUT $n $n

      if [[ "$impl" == "stencil-2d-mpi" || "$impl" == "stencil-2d-hybrid" ]]; then
        srun -n$p ./$impl $t $INPUT $OUTPUT 0 $p
      else
        srun -n1 --cpus-per-task=$p ./$impl $t $INPUT $OUTPUT 0 $p
      fi

      T_OVR=$(grep "Total time" results/stencil2d_all_${SLURM_JOB_ID}.out \
               | awk '{print $3}')
      T_CMP=$(grep "Work time"  results/stencil2d_all_${SLURM_JOB_ID}.out \
               | awk '{print $3}')
      T_OTH=$(echo "$T_OVR - $T_CMP" | bc)

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