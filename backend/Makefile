CC     = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2

SRC = board.c moves.c eval.c ai.c main.c
OBJ = $(SRC:.c=.o)

# C targets
chess: $(OBJ)
	$(CC) $(CFLAGS) -o chess $(OBJ)

test_chess: board.c moves.c eval.c ai.c test_chess.c
	$(CC) $(CFLAGS) -o test_chess board.c moves.c eval.c ai.c test_chess.c

test_c: test_chess
	./test_chess

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Python targets
test_py:
	python -m unittest test_protocol.py -v

# Combined

test: test_c test_py

clean:
	rm -f *.o chess test_chess