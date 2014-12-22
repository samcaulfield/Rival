all: rival

rival: src/*.c
	gcc src/*.c -o rival -std=c99 -I include/
clean:
	rm rival

