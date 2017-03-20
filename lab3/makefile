CC = g++
FLAGS = -std=c++11 -ggdb -pthread -fopenmp
PROGRAM = Implementation.o
SRC = Implementation.cpp
chunkSize = 70
a1 = a1
a2 = a2

all: $(SRC)
	$(CC) $(FLAGS) $(SRC) -o $(PROGRAM)

run:
	./Implementation.o aniketsh_tc1.pgm output_tc1_a1.pgm $(chunkSize) $(a1)
	./Implementation.o aniketsh_tc2.pgm output_tc2_a1.pgm $(chunkSize) $(a1)
	./Implementation.o aniketsh_tc3.pgm output_tc3_a1.pgm $(chunkSize) $(a1)
	./Implementation.o aniketsh_tc4.pgm output_tc4_a1.pgm $(chunkSize) $(a1)
	./Implementation.o aniketsh_tc5.pgm output_tc5_a1.pgm $(chunkSize) $(a1)
	./Implementation.o aniketsh_tc1.pgm output_tc1_a2.pgm $(chunkSize) $(a2)
	./Implementation.o aniketsh_tc2.pgm output_tc2_a2.pgm $(chunkSize) $(a2)
	./Implementation.o aniketsh_tc3.pgm output_tc3_a2.pgm $(chunkSize) $(a2)
	./Implementation.o aniketsh_tc4.pgm output_tc4_a2.pgm $(chunkSize) $(a2)
	./Implementation.o aniketsh_tc5.pgm output_tc5_a2.pgm $(chunkSize) $(a2)

clean:
	rm -f output_tc*
	rm -f Implementation.o
