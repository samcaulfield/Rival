#ifndef rvl_item_h
#define rvl_item_h

typedef enum {
	rvl_item_stone, rvl_item_wood, rvl_item_wooden_club
} rvl_item_type;

typedef struct {
	rvl_item_type type;
} rvl_item;

void rvl_item_free(rvl_item *item);

char *rvl_item_type_name(rvl_item_type type);

rvl_item *rvl_item_new(rvl_item_type type);

#endif

