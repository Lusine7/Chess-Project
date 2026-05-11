# Makefile for the Chess project. To compile the engine run 
# make or make chess, to run the tests run either make test 
# to test both the backend and frontend, or make test_c for just the backend
# and make test_py for just the frontend tests.
# Run make install to install Python dependencies (pygame).

CC     = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2

SRC = board.c moves.c eval.c ai.c main.c
OBJ = $(SRC:.c=.o)

# C targets
chess: $(OBJ)
	$(CC) $(CFLAGS) -o chess $(OBJ)

test_chess: board.c moves.c eval.c ai.c test_chess.c
	$(CC) $(CFLAGS) -o test_chess board.c moves.c eval.c ai.c test_chess.c

test_c: test_chess #test only the C backend
	./test_chess

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Python tests
test_py:
	cd ../frontend && python3 -m unittest test_protocol -v

# Install Python dependencies
install:
	python3 -m pip install pygame

# Combined tests

test: test_c test_py

clean:
	rm -f *.o chess test_chess