CC = gcc
CFLAGS = -ggdb -Wall -std=c11 -pthread
PROGRAM = partA.out
SRC = Implementation.c

all: $(SRC)
	$(CC) -o $(PROGRAM) $(SRC) $(CFLAGS)

run: $(PROGRAM)
	./$(PROGRAM) 5
mem:
	valgrind --leak-check=full -v ./$(PROGRAM) 5
clean:
	rm $(PROGRAM)
