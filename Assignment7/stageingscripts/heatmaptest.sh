#!/bin/bash
#SBATCH --job-name=Heatmap
#SBATCH --output=heatmap_%j.out
#SBATCH --partition=compute
#SBATCH --time=00:10:00
#SBATCH --mem=4G
#SBATCH --account=ccu108
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --ntasks-per-node=1

module purge
module load cpu/0.15.4 gcc/10.2.0 python3

# Create venv on first run (will live under ./env)
if [ ! -d env ]; then
  python3 -m venv env
  source env/bin/activate
  pip install --upgrade pip
  pip install matplotlib imageio numpy
else
  source env/bin/activate
fi

cd ../code
python3 heatmap.py