#!/bin/bash
#SBATCH --job-name=Stencil2D_All
#SBATCH --output=results/stencil2d_all_%j.out
#SBATCH --mail-user=jjohns7@coastal.edu
#SBATCH --mail-type=BEGIN,END
#SBATCH --partition=compute
#SBATCH --time=00:25:00
#SBATCH --mem=128GB
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=4
#SBATCH --cpus-per-task=8
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

# Prepare
mkdir -p results
mkdir -p matrices

impl=$IMPL; n=$N; p=$P; t=$T
INPUT=matrices/input_${impl}_${n}_${p}.bin
OUTPUT=matrices/output_${impl}_${n}_${p}.bin
RESULTS=results/computation_times.csv

echo "Running $impl n=$n p=$p t=$t"
./make-2d $INPUT $n $n

if [[ "$impl" == "stencil-2d" ]]; then
  # Serial version
  ./stencil-2d $t $INPUT $OUTPUT 0
else
  if [[ "$impl" == "stencil-2d-mpi" ]]; then
    # MPI version
    srun --ntasks=$p --cpus-per-task=1 ./$impl $t $INPUT $OUTPUT 0 $p
  elif [[ "$impl" == "stencil-2d-hybrid" ]]; then
    # Hybrid version (split MPI and OpenMP)
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
      echo "Unsupported p=$p for hybrid, skipping"
      exit 1
    fi
    export OMP_NUM_THREADS=$threads
    srun --ntasks=$ntasks --cpus-per-task=$threads ./$impl $t $INPUT $OUTPUT 0 $threads
  else
    # OpenMP or Pthreads version
    export OMP_NUM_THREADS=$p
    srun --ntasks=1 --cpus-per-task=$p ./$impl $t $INPUT $OUTPUT 0 $p
  fi
fi

# Extract timing information
T_OVR=$(grep "Total time" results/stencil2d_all_${SLURM_JOB_ID}.out | tail -n1 | awk '{print $3}' | tr -d '\n')
T_CMP=$(grep "Work time" results/stencil2d_all_${SLURM_JOB_ID}.out | tail -n1 | awk '{print $3}' | tr -d '\n')
T_OTH=$(echo "$T_OVR - $T_CMP" | bc | tr -d '\n')

# Append results
echo "$impl,$n,$p,$t,$T_OVR,$T_CMP,$T_OTH" >> $RESULTS
