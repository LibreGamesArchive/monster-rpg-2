#ifndef TEXATLAS_H
#define TEXATLAS_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ATLAS ATLAS;
typedef struct ATLAS_ITEM ATLAS_ITEM;

/* Main api */
ATLAS *atlas_create(int width, int height, int border, bool destroy_bmps);
bool atlas_add(ATLAS *atlas, ALLEGRO_BITMAP *bitmap, int id);
int atlas_finish(ATLAS *atlas);
void atlas_destroy(ATLAS *atlas);

/* Structure accessors */
  /* ATLAS */
int atlas_get_width(ATLAS *atlas);
int atlas_get_height(ATLAS *atlas);
int atlas_get_border(ATLAS *atlas);
int atlas_get_destroy_bmps(ATLAS *atlas);
int atlas_get_num_items(ATLAS *atlas);
ATLAS_ITEM *atlas_get_item_by_id(ATLAS *atlas, int id);
ATLAS_ITEM *atlas_get_item_by_index(ATLAS *atlas, int index);
int atlas_get_num_sheets(ATLAS *atlas);
ALLEGRO_BITMAP *atlas_get_sheet(ATLAS *atlas, int index);
  /* ATLAS_ITEM */
ALLEGRO_BITMAP *atlas_get_item_sub_bitmap(ATLAS_ITEM *item);
int atlas_get_item_sheet(ATLAS_ITEM *item);
int atlas_get_item_x(ATLAS_ITEM *item);
int atlas_get_item_y(ATLAS_ITEM *item);
int atlas_get_item_id(ATLAS_ITEM *item);

#ifdef __cplusplus
}
#endif

#endif
