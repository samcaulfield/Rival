CC=gcc
CFLAGS=-c -std=c99
OBJDIR=obj/
SRCDIR=src/

all: Rival

Rival: $(OBJDIR)Rival.o $(OBJDIR)Network.o $(OBJDIR)Rendering.o \
$(OBJDIR)rvlScene.o $(OBJDIR)rvlLinkedList.o
	$(CC) $(OBJDIR)Rival.o $(OBJDIR)Network.o $(OBJDIR)Rendering.o \
	$(OBJDIR)rvlScene.o $(OBJDIR)rvlLinkedList.o -o Rival

$(OBJDIR)Rival.o: $(SRCDIR)Rival.c
	$(CC) $(CFLAGS) $(SRCDIR)Rival.c -o $(OBJDIR)Rival.o

$(OBJDIR)Network.o: $(SRCDIR)Network.c
	$(CC) $(CFLAGS) $(SRCDIR)Network.c -o $(OBJDIR)Network.o

$(OBJDIR)rvlScene.o: $(SRCDIR)rvlScene.c
	$(CC) $(CFLAGS) $(SRCDIR)rvlScene.c -o $(OBJDIR)rvlScene.o

$(OBJDIR)Rendering.o: $(SRCDIR)Rendering.c
	$(CC) $(CFLAGS) $(SRCDIR)Rendering.c -o $(OBJDIR)Rendering.o


$(OBJDIR)rvlLinkedList.o: $(SRCDIR)rvlLinkedList.c
	$(CC) $(CFLAGS) $(SRCDIR)rvlLinkedList.c -o $(OBJDIR)rvlLinkedList.o

clean:
	rm obj/*.o Rival

