CC = g++
CFLAGS = -ggdb -Wall -std=c++11 -pthread
PROGRAM = partB.out
SRC = Implementation.cpp

all: $(SRC)
	$(CC) -o $(PROGRAM) $(SRC) $(CFLAGS)

run: $(PROGRAM)
	# <input image file name> <output image file name> <# threads> <chunk size>
	./$(PROGRAM) aniketsh_tc1.pgm output_tc1.pgm 3 100 
	./$(PROGRAM) aniketsh_tc2.pgm output_tc2.pgm 5 50
	./$(PROGRAM) aniketsh_tc3.pgm output_tc3.pgm 1 87
	./$(PROGRAM) aniketsh_tc4.pgm output_tc4.pgm 50 5
	./$(PROGRAM) aniketsh_tc5.pgm output_tc5.pgm 7 29
mem:
	valgrind --leak-check=full -v ./$(PROGRAM) 5
clean:
	rm $(PROGRAM)
