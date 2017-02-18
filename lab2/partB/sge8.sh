#!/bin/bash
#$ -cwd
#$ -j y
#$ -S /bin/bash
#$ -pe openmpi 8
#$ -o sge8.out

module load sge
module load openmpi
mpirun -np $NSLOTS ./partB aniketsh_input.txt the b1 
mpirun -np $NSLOTS ./partB aniketsh_input.txt the b2 
