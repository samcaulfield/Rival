#include <stdlib.h>

#include "rvl_item.h"

void rvl_item_free(rvl_item *item)
{
        free(item);
}

char *rvl_item_type_name(rvl_item_type type)
{
        switch (type) {
        case rvl_wood: return "Wood";
        }
}

rvl_item *rvl_item_new(rvl_item_type type)
{
        rvl_item *item = (rvl_item *) malloc(sizeof(rvl_item));
        if (item)
                item->type = type;
        return item;
}

