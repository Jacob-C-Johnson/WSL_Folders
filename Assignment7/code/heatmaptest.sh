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
module load cpu/0.15.4 gcc/10.2.0 python/3.8.5
module load matplotlib/3.3.4 imageio/2.15.0   # adjust versions to what `module avail` shows 

python3 heatmap.py