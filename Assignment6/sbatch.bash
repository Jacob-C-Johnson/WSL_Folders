#!/bin/bash
#SBATCH --job-name="MPI_ComputeTest" 
#SBATCH --output="ComputeTest.%j.%N.out"
#SBATCH --partition=compute
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=4
#SBATCH --cpus-per-task=1
#SBATCH --mem=16GB
#SBATCH --account=ccu108
#SBATCH --export=ALL
#SBATCH -t 02:00:00

# Exit on error
set -e

# Load required modules
module purge
module load cpu
module load slurm
module load gcc/10.2.0
module load openmpi/4.1.3
module load mpip/3.5

# Calculate total number of processes
P=$SLURM_NTASKS
echo "Running MPI with $P processes"

# Run the MPI program
mpirun -np $P ./gsum