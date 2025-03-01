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
#SBATCH -t 02:00:00

module purge
module load cpu
module load slurm
module load gcc/10.2.0

# Create CSV file and add headers
echo "Experiment,Type,N,P,TotalTime,WorkTime" > results.csv

# Serial experiments
i=0
for N in 256; do
    ((i++))
    echo "running serial experiment $i with N=$N"
    ./make_matrix A $N $N 
    ./make_matrix X $N $N

    output=$(./matrix_matrix A X Y1)
    total_time=$(echo "$output" | grep "Total time" | awk '{print $3}')
    work_time=$(echo "$output" | grep "Work time" | awk '{print $3}')
    echo "$i,Serial,$N,1,$total_time,$work_time" >> results.csv
    echo "-----------------------------------------"
done


# A bash script to run your experiments on the HPC
# This script provides an output file name
# runs on the compute nodes of the cluster (see the user guide)
# uses a single node
# has a single process on that node
# but we have 4 cpus available to run this task
# uses the ccu108 account number (it gets charged to this account)
# copy all environment variables over
# -t is the amount of time at most to run this job hours:minutes:seconds
# time is important so it kills the program if it accidentally breaks and keeps running
# time is also important because this program will be queued but will not run right away 
# because other people are running things right now (its busy)
# so the time you put helps the schedulder know when to start your script

# submit this job using the command: $ sbatch sbatch.bash
# it will reutrn the job id

# to know how long the job has been running: $ squeue -u <username> 
# and you can keep calling that command to see when it's done

# alternatively, use: $ watch squeue -u <username>
# for it to automatically refresh every 2 seconds

# when it's done running, it will create a .out file
# this file contains all of the print output (instead of it going to the screen)
