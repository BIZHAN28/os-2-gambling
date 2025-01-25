CC = gcc
CFLAGS = -Wall -g -Iinclude
LDFLAGS =

SRC = src/cache.c src/file_io.c src/main.c
OBJ = $(SRC:.c=.o)
OUT = lab2

all: $(OUT)

$(OUT): $(OBJ)
	$(CC) $(LDFLAGS) -o $(OUT) $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(OUT)
