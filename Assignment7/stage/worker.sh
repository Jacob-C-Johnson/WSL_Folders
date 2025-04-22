#!/bin/bash
#SBATCH --job-name=Stencil2D
#SBATCH --output=results/stencil2d_%j.out
#SBATCH --partition=compute
#SBATCH --time=12:30:00
#SBATCH --mem=64G
#SBATCH --nodes=2               # max you’ll ever need
#SBATCH --ntasks-per-node=8     # max threads per node
#SBATCH --export=ALL

set -e
module purge; module load gcc/10.2.0 openmpi/4.1.3

# read in from sbatch --export
impl=$IMPL; n=$N; p=$P; t=$T

echo "Running $impl n=$n p=$p t=$t"
./make-2d input_matrix.bin $n $n

if [[ "$impl" == stencil-2d-mpi || "$impl" == stencil-2d-hybrid ]]; then
  mpirun -np $p ./$impl $t input_matrix.bin output_matrix.bin 0 $p
else
  ./$impl $t input_matrix.bin output_matrix.bin 0 $p
fi

T_OVERALL=$(grep "Total time" results/stencil2d_${SLURM_JOB_ID}.out | awk '{print $3}')
T_COMP=$(grep "Work time"  results/stencil2d_${SLURM_JOB_ID}.out | awk '{print $3}')
T_OTHER=$(echo "$T_OVERALL - $T_COMP" | bc)
echo "$impl,$n,$p,$t,$T_OVERALL,$T_COMP,$T_OTHER" \
  >> results/computation_times.csv
