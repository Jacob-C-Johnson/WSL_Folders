#!/bin/bash

for P in 2 4 8 16; do
    if [ $P -le 4 ]; then
        NODES=1
        TASKS_PER_NODE=$P
    else
        NODES=2
        TASKS_PER_NODE=$((P / 2))
    fi

    sed -i "s/^#SBATCH --nodes=.*/#SBATCH --nodes=$NODES/" sbatch.bash
    sed -i "s/^#SBATCH --ntasks-per-node=.*/#SBATCH --ntasks-per-node=$TASKS_PER_NODE/" sbatch.bash

    echo "Submitting job with $P processes ($NODES nodes, $TASKS_PER_NODE tasks per node)"
    sbatch sbatch.bash
done