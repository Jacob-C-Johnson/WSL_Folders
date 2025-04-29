#!/bin/bash
#SBATCH --job-name="Stencil2D_Iteration_timer"
#SBATCH --output="results/stencil2d_%j.out"
#SBATCH --partition=compute
#SBATCH --nodes=1              # Number of nodes
#SBATCH --ntasks-per-node=1  # Number of tasks per node
#SBATCH --cpus-per-task=4    # Number of CPUs per task
#SBATCH --time=00:30:00
#SBATCH --mem=64GB
#SBATCH --account=ccu108
#SBATCH --export=ALL

./make-2d input_matrix.bin 40000 40000

./stencil-2d 7 input_matrix.bin output_matrix.bin 0

