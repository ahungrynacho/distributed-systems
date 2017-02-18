#!/bin/bash
#
#$ -cwd
#$ -j y
#$ -S /bin/bash
#$ -pe openmpi 4
#$ -o output.out
#
#

module load sge
module load openmpi
mpirun -np $NSLOTS ./$program aniketsh_tc1.pgm output_tc1.pgm
mpirun -np $NSLOTS ./$program aniketsh_tc2.pgm output_tc2.pgm
mpirun -np $NSLOTS ./$program aniketsh_tc3.pgm output_tc3.pgm
mpirun -np $NSLOTS ./$program aniketsh_tc4.pgm output_tc4.pgm
mpirun -np $NSLOTS ./$program aniketsh_tc5.pgm output_tc5.pgm
