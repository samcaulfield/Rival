CC=gcc
CFLAGS=-std=c99

all: Rival

Rival: Rival.c
	$(CC) $(CFLAGS) Rival.c -o Rival

clean:
	rm Rival

