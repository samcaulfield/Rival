#ifndef RENDERING_H
#define RENDERING_H

void draw(int width, int length, struct Player me, struct Player rival);

void addMessage(char *message);

void drawHelpText(struct Player me, bool myTurn, int movesLeft);

#endif

