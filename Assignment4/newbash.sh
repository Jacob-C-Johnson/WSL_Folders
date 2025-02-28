#!/bin/bash

#SBATCH --job-name="MatrixMultiplication"

#SBATCH --output="ComputeTest.%j.%N.out"

#SBATCH --partition=compute

#SBATCH --nodes=1

#SBATCH --ntasks-per-node=1

#SBATCH --cpus-per-task=4

#SBATCH --mem=16GB

#SBATCH --account=ccu108

#SBATCH --export=ALL

#SBATCH -t 00:30:00



module purge

module load cpu

module load slurm

module load gcc/10.2.0



# Move to submission directory

cd $SLURM_SUBMIT_DIR



# Ensure executables exist

if [[ ! -f ./make_matrix || ! -f ./matrix_vector ]]; then

    echo "Error: Executables not found!"

    exit 1

fi



# Serial experiments

i=0

for N in 256 512 1024 2048 4096 8192; do

    ((i++))

    echo "Running serial experiment $i with N=$N"

    ./make_matrix A $N $N

    ./make_matrix X $N $N

    ./matrix_vector A X Y1

    echo "-----------------------------------------"

done



# Parallel experiments

i=0

for N in 256 512 1024 2048 4096 8192; do

    for P in 1 2 4 8 16 32 64 128; do

        ((i++))

        echo "Running parallel experiment $i with N=$N and P=$P"

        ./make_matrix A $N $N

        ./make_matrix X $N $N

        ./pth_matrix_vector A X Y1 $P

        echo "-----------------------------------------"

    done

done


