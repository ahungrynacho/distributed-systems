MPI = mpic++
CC = g++
FLAGS = -ggdb -std=c++11
PROGRAM = partB
SRC0 = ImplementationB.cpp
SRC1 = aniketsh_mpi_partB_skel.cpp

all: $(SRC0)
	$(MPI) $(FLAGS) $(SRC0) -o $(PROGRAM)

run: $(PROGRAM)
	mpirun -np 2 ./$(PROGRAM) aniketsh_input.txt the b1
	mpirun -np 2 ./$(PROGRAM) aniketsh_input.txt the b2
	mpirun -np 4 ./$(PROGRAM) aniketsh_input.txt the b1
	mpirun -np 4 ./$(PROGRAM) aniketsh_input.txt the b2
	mpirun -np 8 ./$(PROGRAM) aniketsh_input.txt the b1
	mpirun -np 8 ./$(PROGRAM) aniketsh_input.txt the b2
	mpirun -np 16 ./$(PROGRAM) aniketsh_input.txt the b1
	mpirun -np 16 ./$(PROGRAM) aniketsh_input.txt the b2

clean:
	rm -f $(PROGRAM)
