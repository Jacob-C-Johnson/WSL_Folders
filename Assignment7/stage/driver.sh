#!/usr/bin/env bash
MATRIX_SIZES=(5000 10000 20000 40000)
THREAD_COUNTS=(1 2 4 8 16)
IMPLEMENTATIONS=( stencil-2d stencil-2d-omp stencil-2d-pth stencil-2d-mpi stencil-2d-hybrid )
t=8 # Number of iterations

for impl in "${IMPLEMENTATIONS[@]}"; do
  for n in "${MATRIX_SIZES[@]}"; do
    for p in "${THREAD_COUNTS[@]}"; do
      sbatch --export=IMPL="$impl",N="$n",P="$p",T="$t" worker.sh
    done
  done
done
