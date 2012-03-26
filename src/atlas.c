#include <allegro5/allegro.h>
#include <allegro5/internal/aintern_list.h>

#include "atlas.h"
#include "atlas_internal.h"

#include <stdio.h>

typedef struct {
	int x, y, w, h;
	int sheet;
} ARECT;

typedef struct {
	ALLEGRO_BITMAP *bitmap;
	int id;
} ABMP;

static void my_free(void *value, void *unused)
{
	(void)unused;
	al_free(value);
}

static ARECT *create_rect(int x, int y, int w, int h, int sheet)
{
	ARECT *r = (ARECT *)al_malloc(sizeof(ARECT));
	r->x = x;
	r->y = y;
	r->w = w;
	r->h = h;
	r->sheet = sheet;
	return r;
}

static ABMP *create_bmp(ALLEGRO_BITMAP *bmp, int id)
{
	ABMP *b = (ABMP *)al_malloc(sizeof(ABMP));
	b->bitmap = bmp;
	b->id = id;
	return b;
}

static void insert_sheet(ATLAS *atlas, _AL_LIST *rect_list)
{
	int sheet = atlas->num_sheets;
	ARECT *rect = create_rect(0, 0, atlas->width, atlas->height, sheet);
	_al_list_item_set_dtor(_al_list_push_back(rect_list, rect), my_free);

	if (sheet == 0) {
		atlas->sheets = (ALLEGRO_BITMAP **)al_malloc(
			sizeof(ALLEGRO_BITMAP *));
	}
	else {
		atlas->sheets = (ALLEGRO_BITMAP **)al_realloc(
			atlas->sheets,
			sizeof(ALLEGRO_BITMAP *) * (sheet+1)
		);
	}

	int flags = al_get_new_bitmap_flags();
	al_set_new_bitmap_flags(flags & ~ALLEGRO_NO_PRESERVE_TEXTURE);
	atlas->sheets[sheet] = al_create_bitmap(atlas->width, atlas->height);
	al_set_new_bitmap_flags(flags);

	al_set_target_bitmap(atlas->sheets[sheet]);
	al_clear_to_color(al_map_rgba(0, 0, 0, 0));

	atlas->num_sheets++;
}

ATLAS *atlas_create(int width, int height, int border, bool destroy_bmps)
{
	ATLAS *atlas;

	atlas = (ATLAS *)al_malloc(sizeof(ATLAS));

	atlas->finished = false;

	atlas->width = width;
	atlas->height = height;
	atlas->border = border;
	atlas->destroy_bmps = destroy_bmps;

	atlas->bmp_list = _al_list_create();
	
	return atlas;
}

bool atlas_add(ATLAS *atlas, ALLEGRO_BITMAP *bitmap, int id)
{
	/* Add bitmaps from largest to smallest */
	int add_w = al_get_bitmap_width(bitmap);
	int add_h = al_get_bitmap_height(bitmap);
	//int add_area = add_w * add_h;
	int add_max = add_w > add_h ? add_w : add_h;


	_AL_LIST_ITEM *item = _al_list_front(atlas->bmp_list);

	while (item) {
		ABMP *b = (ABMP *)_al_list_item_data(item);
		ALLEGRO_BITMAP *bmp = b->bitmap;
		int this_w = al_get_bitmap_width(bmp);
		int this_h = al_get_bitmap_height(bmp);
		//int this_area = this_w * this_h;
		int this_max = this_w > this_h ? this_w : this_h;

		if (this_max < add_max)
			break;
		
		item = _al_list_next(atlas->bmp_list, item);
	}

	ABMP *b = create_bmp(bitmap, id);

	if (item) {
		_al_list_insert_before(
			atlas->bmp_list,
			item,
			b
		);
	}
	else {
		_al_list_push_back(atlas->bmp_list, b);
	}

	return true;
}

/* Return number of bitmaps added to atlas */
int atlas_finish(ATLAS *atlas)
{
	assert(_al_list_size(atlas->bmp_list) > 0);

	ALLEGRO_STATE orig_state;
	al_store_state(&orig_state,
		ALLEGRO_STATE_BLENDER |
		ALLEGRO_STATE_TARGET_BITMAP);
	al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);

	atlas->num_items = 0;
	atlas->num_sheets = 0;

	_AL_LIST *rect_list = _al_list_create();
	insert_sheet(atlas, rect_list);

	_AL_LIST_ITEM *item = _al_list_front(atlas->bmp_list);
	int count = 0;

	do {
		ABMP *b = (ABMP *)_al_list_item_data(item);
		ALLEGRO_BITMAP *bmp = b->bitmap;
		int bmp_w = al_get_bitmap_width(bmp);
		int bmp_h = al_get_bitmap_height(bmp);
		int req_w = bmp_w + (atlas->border * 2);
		int req_h = bmp_h + (atlas->border * 2);
		if (req_w > atlas->width || req_h > atlas->height) {
			item = _al_list_next(atlas->bmp_list, item);
			continue;
		}
		bool found = false;
		_AL_LIST_ITEM *rect_item = _al_list_front(rect_list);
		while (rect_item) {
			ARECT *rect = (ARECT *)_al_list_item_data(rect_item);
			if (rect->w >= req_w && rect->h >= req_h) {
				found = true;
				ATLAS_ITEM *atlas_item =
					(ATLAS_ITEM *)
					al_malloc(sizeof(ATLAS_ITEM));
				al_set_target_bitmap(atlas->sheets[rect->sheet]);
				al_draw_bitmap(
					bmp,
					rect->x+atlas->border,
					rect->y+atlas->border,
					0
				);
				atlas_item->sub = al_create_sub_bitmap(
					atlas->sheets[rect->sheet],
					rect->x+atlas->border,
					rect->y+atlas->border,
					bmp_w, bmp_h
				);
				atlas_item->sheet = rect->sheet;
				atlas_item->x = rect->x+atlas->border;
				atlas_item->y = rect->y+atlas->border;
				atlas_item->id = b->id;
				if (atlas->num_items == 0) {
					atlas->items = 
					(ATLAS_ITEM **)al_malloc(
						sizeof(ATLAS_ITEM *)
					);
				}
				else {
					atlas->items = 
					(ATLAS_ITEM **)al_realloc(
						atlas->items,
						sizeof(ATLAS_ITEM *) *
						(atlas->num_items+1)
					);
				}
				atlas->items[atlas->num_items] = atlas_item;
				atlas->num_items++;
				// Four possibilities
				if (rect->w == req_w && rect->h == req_h) {
					// exact match, remove rect
					_al_list_erase(rect_list, rect_item);
				}
				else if (rect->w == req_w) {
					// width match, change rect
					rect->y += req_h;
					rect->h -= req_h;
				}
				else if (rect->h == req_h) {
					// height match, change rect
					rect->x += req_w;
					rect->w -= req_w;
				}
				else {
					// no match, change 1 rect, add 1 rect
					ARECT *r2 = create_rect(
						rect->x + req_w,
						rect->y,
						rect->w - req_w,
						rect->h,
						rect->sheet
					);
					_al_list_item_set_dtor(
						_al_list_insert_after(
							rect_list,
							rect_item,
							r2
						),
						my_free
					);
					rect->y += req_h;
					rect->h -= req_h;
					rect->w = req_w;
				}
				break;
			}
			rect_item = _al_list_next(rect_list, rect_item);
		}
		if (!found) {
			insert_sheet(atlas, rect_list);
			continue;
		}
		item = _al_list_next(atlas->bmp_list, item);
		count++;
	} while (item);

	_al_list_destroy(rect_list);

	al_restore_state(&orig_state);

	return count;
}

void atlas_destroy(ATLAS *atlas)
{
	int i;

	for (i = 0; i < atlas->num_sheets; i++) {
		al_destroy_bitmap(atlas->sheets[i]);
	}
	al_free(atlas->sheets);
	for (i = 0; i < atlas->num_items; i++) {
		al_free(atlas->items[i]);
	}
	al_free(atlas->items);

	if (atlas->destroy_bmps) {
		_AL_LIST_ITEM *item;
		while ((item = _al_list_front(atlas->bmp_list)) != NULL) {
			ABMP *b = (ABMP *)_al_list_item_data(item);
			al_destroy_bitmap(b->bitmap);
			al_free(b);
			_al_list_pop_front(atlas->bmp_list);
		}
	}

	_al_list_destroy(atlas->bmp_list);

	al_free(atlas);
}

