#!/bin/bash
#SBATCH --job-name=Heatmap
#SBATCH --output=heatmap_%j.out
#SBATCH --partition=compute
#SBATCH --time=00:10:00
#SBATCH --mem=4G
#SBATCH --account=ccu108
#SBATCH --nodes=1   
#SBATCH --ntasks=1               # <— add this
#SBATCH --ntasks-per-node=1      # optional, but explicit

module purge
module load python/3.8-anaconda   

python3 heatmap.py