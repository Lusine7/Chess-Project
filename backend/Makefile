CC     = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2

SRC = board.c moves.c
OBJ = $(SRC:.c=.o)

chess: $(OBJ)
	$(CC) $(CFLAGS) -o chess $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o chess