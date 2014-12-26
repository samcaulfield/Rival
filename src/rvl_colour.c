#include "rvl_colour.h"

char *rvl_colour_begin(rvl_colour colour)
{
	switch (colour) {
	case rvl_green:
		return "[32m";
	case rvl_red:
		return "[31m";
	case rvl_white:
		return "[97m";
	}
}

char *rvl_colour_end()
{
	return "[97m";
}

