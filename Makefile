all: Rival

Rival: src/*.c
	gcc src/*.c -o Rival -std=c99
clean:
	rm Rival

