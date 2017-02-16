MPI = mpic++
CC = g++
FLAGS = -ggdb -std=c++11
PROGRAM = partA
SRC0 = ImplementationA.cpp
SRC1 = no_mpi.cpp
SRC2 = aniketsh_mpi_partA_skel.cpp

all: $(SRC0)
	$(MPI) $(FLAGS) $(SRC0) -o $(PROGRAM)

run: $(PROGRAM)
	mpirun -np 4 ./$(PROGRAM) aniketsh_tc1.pgm output_tc1.pgm
	mpirun -np 4 ./$(PROGRAM) aniketsh_tc2.pgm output_tc2.pgm
	mpirun -np 4 ./$(PROGRAM) aniketsh_tc3.pgm output_tc3.pgm
	mpirun -np 4 ./$(PROGRAM) aniketsh_tc4.pgm output_tc4.pgm
	mpirun -np 4 ./$(PROGRAM) aniketsh_tc5.pgm output_tc5.pgm

run-mpi: $(PROGRAM)
	mpirun -np 4 ./$(PROGRAM) aniketsh_tc1.pgm output_tc1.pgm

clean:
	rm -f $(PROGRAM)
