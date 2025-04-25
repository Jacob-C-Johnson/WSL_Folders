#!/usr/bin/env bash
MATRIX_SIZES=(50)
THREAD_COUNTS=(1)
IMPLEMENTATIONS=( stencil-2d stencil-2d-omp stencil-2d-pth stencil-2d-mpi stencil-2d-hybrid )
t=8 # Number of iterations

for impl in "${IMPLEMENTATIONS[@]}"; do
  for n in "${MATRIX_SIZES[@]}"; do
    if [[ "$impl" == "stencil-2d" ]]; then
      # For serial, only need p=1
      sbatch --export=IMPL="$impl",N="$n",P="1",T="$t" worker.sh
    else
      for p in "${THREAD_COUNTS[@]}"; do
        sbatch --export=IMPL="$impl",N="$n",P="$p",T="$t" worker.sh
      done
    fi
  done
done
