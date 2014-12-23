#ifndef rvl_colour_h
#define rvl_colour_h

typedef enum {
        rvl_green, rvl_red, rvl_white
} rvl_colour;

char *rvl_colour_begin(rvl_colour colour);

char *rvl_colour_end();

#endif

