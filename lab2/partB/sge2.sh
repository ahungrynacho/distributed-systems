#!/bin/bash
#$ -cwd
#$ -j y
#$ -S /bin/bash
#$ -pe openmpi 2
#$ -o sge2.out

module load sge
module load openmpi
mpirun -np $NSLOTS ./partB aniketsh_input.txt the b1 
mpirun -np $NSLOTS ./partB aniketsh_input.txt the b2 
