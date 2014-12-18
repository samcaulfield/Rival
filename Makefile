CC=gcc
CFLAGS=-c -std=c99
OBJDIR=obj/
SRCDIR=src/

all: Rival

Rival: $(OBJDIR)Rival.o $(OBJDIR)Network.o $(OBJDIR)Rendering.o \
$(OBJDIR)Scene.o $(OBJDIR)rvlLinkedList.o
	$(CC) $(OBJDIR)Rival.o $(OBJDIR)Network.o $(OBJDIR)Rendering.o \
	$(OBJDIR)Scene.o $(OBJDIR)rvlLinkedList.o -o Rival

$(OBJDIR)Rival.o: $(SRCDIR)Rival.c
	$(CC) $(CFLAGS) $(SRCDIR)Rival.c -o $(OBJDIR)Rival.o

$(OBJDIR)Network.o: $(SRCDIR)Network.c
	$(CC) $(CFLAGS) $(SRCDIR)Network.c -o $(OBJDIR)Network.o

$(OBJDIR)Scene.o: $(SRCDIR)Scene.c
	$(CC) $(CFLAGS) $(SRCDIR)Scene.c -o $(OBJDIR)Scene.o

$(OBJDIR)Rendering.o: $(SRCDIR)Rendering.c
	$(CC) $(CFLAGS) $(SRCDIR)Rendering.c -o $(OBJDIR)Rendering.o


$(OBJDIR)rvlLinkedList.o: $(SRCDIR)rvlLinkedList.c
	$(CC) $(CFLAGS) $(SRCDIR)rvlLinkedList.c -o $(OBJDIR)rvlLinkedList.o

clean:
	rm obj/*.o Rival

