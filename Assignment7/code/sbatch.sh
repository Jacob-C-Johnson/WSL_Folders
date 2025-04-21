#!/bin/bash
#SBATCH --job-name="Stencil2D_Experiments"
#SBATCH --output="results/stencil2d_%j.out"
#SBATCH --partition=compute
#SBATCH --time=04:30:00
#SBATCH --mem=64GB
#SBATCH --account=ccu108
#SBATCH --export=ALL

# Exit on error
set -e

# Load required modules
module purge
module load gcc/10.2.0
module load openmpi/4.1.3

# Parameters
MATRIX_SIZES=(5000 10000 20000 40000)
THREAD_COUNTS=(1 2 4 8 16)
IMPLEMENTATIONS=("stencil-2d" "stencil-2d-omp" "stencil-2d-pth" "stencil-2d-mpi" "stencil-2d-hybrid")
INPUT_FILE="input_matrix.bin"
OUTPUT_FILE="output_matrix.bin"
FRAMES_DIR="frames"
RESULTS_DIR="results"
T_COMPUTATION_FILE="computation_times.csv"

# Ensure results directory exists
mkdir -p $RESULTS_DIR

# Step 1: Determine `t` for n=40k and p=1
echo "Determining t for n=40k and p=1..."
n=40000
p=1
t=100  # Start with an initial guess for t
while true; do
    ./make-2d $INPUT_FILE $n $n
    start_time=$(date +%s)
    ./stencil-2d $t $INPUT_FILE $OUTPUT_FILE 0
    end_time=$(date +%s)
    elapsed=$((end_time - start_time))
    if [ $elapsed -ge 240 ]; then  # 4 minutes = 240 seconds
        break
    fi
    t=$((t + 50))  # Increment t and try again
done
echo "Determined t=$t for n=40k and p=1."

# Step 2: Run experiments for all combinations of n and p
echo "Running experiments..."
for impl in "${IMPLEMENTATIONS[@]}"; do
    for n in "${MATRIX_SIZES[@]}"; do
        for p in "${THREAD_COUNTS[@]}"; do
            echo "Running $impl with n=$n, p=$p, t=$t..."
            
            # Generate input matrix
            ./make-2d $INPUT_FILE $n $n
            
            # Adjust SLURM parameters dynamically
            if [ "$p" -le 8 ]; then
                NODES=1
                TASKS_PER_NODE=$p
            else
                NODES=2
                TASKS_PER_NODE=$((p / 2))
            fi
            
            # Run the implementation
            if [[ "$impl" == "stencil-2d-mpi" || "$impl" == "stencil-2d-hybrid" ]]; then
                mpirun -np $p ./$impl $t $INPUT_FILE $OUTPUT_FILE 0 $p
            else
                ./$impl $t $INPUT_FILE $OUTPUT_FILE 0 $p
            fi
            
            # Collect measurements
            T_OVERALL=$(grep "Total time" results/stencil2d_${SLURM_JOB_ID}.out | awk '{print $3}')
            T_COMPUTATION=$(grep "Work time" results/stencil2d_${SLURM_JOB_ID}.out | awk '{print $3}')
            T_OTHER=$(echo "$T_OVERALL - $T_COMPUTATION" | bc)
            
            # Save results
            echo "$impl,$n,$p,$t,$T_OVERALL,$T_COMPUTATION,$T_OTHER" >> $RESULTS_DIR/$T_COMPUTATION_FILE
        done
    done
done

echo "All experiments completed. Results saved in $RESULTS_DIR/$T_COMPUTATION_FILE."