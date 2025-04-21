#!/bin/bash
#SBATCH --job-name="Stencil2D_Iteration_timer"
#SBATCH --output="results/stencil2d_%j.out"
#SBATCH --partition=compute
#SBATCH --time=00:30:00
#SBATCH --mem=64GB
#SBATCH --account=ccu108
#SBATCH --export=ALL

./make-2d input_matrix.bin 40000 40000

./stencil-2d 10 input_matrix.bin output_matrix.bin 0

