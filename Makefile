CC=gcc
CFLAGS=-c -std=c99
OBJDIR=obj/
SRCDIR=src/

all: Rival

Rival: $(OBJDIR)Rival.o $(OBJDIR)Network.o
	$(CC) $(OBJDIR)Rival.o $(OBJDIR)Network.o -o Rival

$(OBJDIR)Rival.o: $(SRCDIR)Rival.c
	$(CC) $(CFLAGS) $(SRCDIR)Rival.c -o $(OBJDIR)Rival.o

$(OBJDIR)Network.o: $(SRCDIR)Network.c
	$(CC) $(CFLAGS) $(SRCDIR)Network.c -o $(OBJDIR)Network.o

clean:
	rm obj/*.o Rival

