CC     = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2

SRC = board.c moves.c eval.c ai.c main.c
OBJ = $(SRC:.c=.o)

chess: $(OBJ)
	$(CC) $(CFLAGS) -o chess $(OBJ)

test_chess: board.c moves.c eval.c ai.c test_chess.c
	$(CC) $(CFLAGS) -o test_chess board.c moves.c eval.c ai.c test_chess.c

test: test_chess
	./test_chess

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o chess test_chess