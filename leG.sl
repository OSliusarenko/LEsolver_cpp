#!/bin/bash
#SBATCH -J "leG"
#SBATCH --nodes=1
#SBATCH --cpus-per-task=1
#SBATCH --mem-per-cpu=1G
#SBATCH -p large
#SBATCH -o /home/osliusarenko/leG.out

echo "Starting at $(date +%Y/%m/%d\ %T)"
g++ --version
echo "SCRATCH directory: "$SCRATCH_JOB
echo "USER directory: "$SCRATCH_USER
echo "TMP directory: "$TMP_JOB
echo "Node: "$HOSTNAME

rm a.out
g++ -std=c++11 -pthread le.cpp
srun a.out -n $TMP_JOB/xs.dat $TMP_JOB/vs.dat
mv $TMP_JOB/xs.dat /home/osliusarenko/xsG_1.dat
mv $TMP_JOB/vs.dat /home/osliusarenko/vsG_1.dat

echo "Finished at $(date +%Y/%m/%d\ %T)"

