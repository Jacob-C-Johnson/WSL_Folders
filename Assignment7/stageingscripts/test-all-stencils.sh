#!/bin/bash

# Set variables
INPUT_FILE="input_matrix.bin"
SERIAL_OUTPUT="serial_output.bin"
PARALLEL_OUTPUT="parallel_output.bin"
ROWS=100
COLS=100
ITERATIONS=100
NUM_PROCESSES=4
NUM_THREADS=4

# Step 1: Generate input matrix
echo "Generating input matrix..."
./make-2d $INPUT_FILE $ROWS $COLS

# Step 2: Run serial implementation
echo "Running serial implementation..."
./stencil-2d $ITERATIONS $INPUT_FILE $SERIAL_OUTPUT

# Step 3: Test all parallel implementations
IMPLEMENTATIONS=("stencil-2d-hybrid" "stencil-2d-mpi" "stencil-2d-omp" "stencil-2d-pth")

for IMPL in "${IMPLEMENTATIONS[@]}"; do
    echo "Testing $IMPL..."

    # Run the implementation
    if [ "$IMPL" == "stencil-2d-hybrid" ]; then
        mpirun -np $NUM_PROCESSES ./$IMPL $ITERATIONS $INPUT_FILE $PARALLEL_OUTPUT 1 $NUM_THREADS
    elif [ "$IMPL" == "stencil-2d-mpi" ]; then
        mpirun -np $NUM_PROCESSES ./$IMPL $ITERATIONS $INPUT_FILE $PARALLEL_OUTPUT 1 $NUM_THREADS
    elif [ "$IMPL" == "stencil-2d-omp" ]; then
        ./$IMPL $ITERATIONS $INPUT_FILE $PARALLEL_OUTPUT 1 $NUM_THREADS
    elif [ "$IMPL" == "stencil-2d-pth" ]; then
        ./$IMPL $ITERATIONS $INPUT_FILE $PARALLEL_OUTPUT 1 $NUM_THREADS
    fi

    # Compare outputs
    echo "Comparing outputs for $IMPL..."
    ./compare-outs $SERIAL_OUTPUT $PARALLEL_OUTPUT
    COMPARE_RESULT=$?

    # Print result
    if [ $COMPARE_RESULT -eq 0 ]; then
        echo "$IMPL: Test passed!"
    else
        echo "$IMPL: Test failed!"
    fi
    echo "----------------------------------------------------"
    echo
    rm -f $PARALLEL_OUTPUT
done

# Step 4: Clean up files
echo "Cleaning up..."
rm -f $INPUT_FILE $SERIAL_OUTPUT $PARALLEL_OUTPUT

echo "All tests completed."