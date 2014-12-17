CC=gcc
CFLAGS=-std=c99

all: Rival

Rival: src/Rival.c
	$(CC) $(CFLAGS) src/Rival.c -o Rival

clean:
	rm Rival

