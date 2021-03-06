#include <stdlib.h>

#include "rvl_item.h"

void rvl_item_free(rvl_item *item)
{
	free(item);
}

char *rvl_item_type_name(rvl_item_type type)
{
	switch (type) {
	case rvl_item_stone: return "Stone";
	case rvl_item_wood: return "Wood";
	case rvl_item_wooden_club: return "Wooden Club";
	}
}

rvl_item *rvl_item_new(rvl_item_type type)
{
	rvl_item *item = malloc(sizeof *item);
	if (item)
		item->type = type;
	return item;
}

