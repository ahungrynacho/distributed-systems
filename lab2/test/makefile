MPI = mpic++
FLAGS = -ggdb -std=c++11
EXECS = change
SRC0 = reduce_avg.c
SRC1 = avg.c
SRC2 = change.cpp

all: $(SRC2)
	$(MPI) $(FLAGS) $(SRC2) -o change

reduce_avg: $(SRC0)
	$(MPI) $(FLAGS) $(SRC0) -o reduce_avg

avg: $(SRC1)
	$(MPI) $(FLAGS) $(SRC1) -o avg


run-reduce: reduce_avg
	mpirun -np 4 ./reduce_avg 5

run-avg: avg
	mpirun -np 4 ./avg 5

run-change: change
	mpirun -np 4 ./change

clean:
	rm -f $(PROGRAM)
