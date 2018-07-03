#!/bin/bash
#SBATCH -J "leA9600e"
#SBATCH --nodes=1
#SBATCH --cpus-per-task=32
#SBATCH --mem-per-cpu=1G
#SBATCH -p extra
#SBATCH -o /home/osliusarenko/leA9600e.out

echo "Starting at $(date +%Y/%m/%d\ %T)"
g++ --version
echo "SCRATCH directory: "$SCRATCH_JOB
echo "USER directory: "$SCRATCH_USER
echo "TMP directory: "$TMP_JOB
echo "Node: "$HOSTNAME

rm a.out
g++ -std=c++11 -pthread le.cpp
srun a.out -n $TMP_JOB/xs.dat $TMP_JOB/vs.dat
mv $TMP_JOB/xs.dat /home/osliusarenko/xsA0.5_9600e.dat
mv $TMP_JOB/vs.dat /home/osliusarenko/vsA0.5_9600e.dat

echo "Finished at $(date +%Y/%m/%d\ %T)"

