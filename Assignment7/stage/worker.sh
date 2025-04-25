#!/bin/bash
#SBATCH --job-name=Stencil2D_All
#SBATCH --output=results/stencil2d_all_%j.out
#SBATCH --mail-user=jjohns7@coastal.edu
#SBATCH --mail-type=BEGIN,END
#SBATCH --partition=compute
#SBATCH --time=00:25:00
#SBATCH --mem=128GB
#SBATCH --nodes=1               # max you’ll ever need
#SBATCH --ntasks-per-node=4     # max threads/node
#SBATCH --cpus-per-task=8
#SBATCH --account=ccu108
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

T_OVR=$(grep "Total time" results/stencil2d_all_${SLURM_JOB_ID}.out | tail -n1 | awk '{print $3}' | tr -d '\n')
T_CMP=$(grep "Work time" results/stencil2d_all_${SLURM_JOB_ID}.out | tail -n1 | awk '{print $3}' | tr -d '\n')
T_OTH=$(echo "$T_OVR - $T_CMP" | bc | tr -d '\n')
echo "$impl,$n,$p,$t,$T_OVR,$T_CMP,$T_OTH" >> $RESULTS
