#include <allegro5/allegro.h>

#include "monster2.hpp"

#include "atlas.h"
#include "atlas_internal.h"

#include <stdio.h>

static ARECT *create_rect(int x, int y, int w, int h, int sheet)
{
	ARECT *r = new ARECT;
	r->x = x;
	r->y = y;
	r->w = w;
	r->h = h;
	r->sheet = sheet;
	return r;
}

static ABMP *create_bmp(MBITMAP *bmp, int id)
{
	ABMP *b = new ABMP;
	b->bitmap = bmp;
	b->id = id;
	return b;
}

static void insert_sheet(ATLAS *atlas, std::vector<ARECT *> &rect_list)
{
	int sheet = atlas->num_sheets;
	ARECT *rect = create_rect(0, 0, atlas->width, atlas->height, sheet);
	rect_list.push_back(rect);

	if (sheet == 0) {
		atlas->sheets = new MBITMAP*[1];
	}
	else {
		MBITMAP **tmp = atlas->sheets;
		atlas->sheets = new MBITMAP*[sheet+1];
		for (int i = 0; i < sheet; i++) {
			atlas->sheets[i] = tmp[i];
		}
		delete[] tmp;
	}

	atlas->sheets[sheet] = m_create_bitmap(atlas->width, atlas->height);

	al_set_target_bitmap(atlas->sheets[sheet]->bitmap);
	al_clear_to_color(al_map_rgba(0, 0, 0, 0));

	atlas->num_sheets++;
}

static void draw_bitmap_with_borders(MBITMAP *bmp, int x, int y)
{
	#define BMP_W al_get_bitmap_width(bmp->bitmap)
	#define BMP_H al_get_bitmap_height(bmp->bitmap)

	int sides[4][6] = {
		{ /* src */ 0, 0, BMP_W, 1, /* dest */ 0, -1 }, // top
		{ /* src */ 0, BMP_H-1, BMP_W, 1, /* dest */ 0, BMP_H }, // bottom
		{ /* src */ 0, 0, 1, BMP_H, /* dest */ -1, 0 }, // left
		{ /* src */ BMP_W-1, 0, 1, BMP_H, /* dest */ BMP_W, 0 } // right
	};

	int corners[4][4] = {
		{ /* src */ 0, 0, /* dest */ -1, -1 }, // top left
		{ /* src */ BMP_W-1, 0, /* dest */ BMP_W, -1 }, // top right
		{ /* src */ 0, BMP_H-1, /* dest */ -1, BMP_H }, // bottom left
		{ /* src */ BMP_W-1, BMP_H-1, /* dest */ BMP_W, BMP_H } // bottom right
	};

	#undef BMP_W
	#undef BMP_H

	// do sides
	for (int i = 0; i < 4; i++) {
		quick_draw(
			bmp->bitmap,
			sides[i][0],
			sides[i][1],
			sides[i][2],
			sides[i][3],
			x+sides[i][4],
			y+sides[i][5],
			0
		);
	}

	// do corners
	for (int i = 0; i < 4; i++) {
		quick_draw(
			bmp->bitmap,
			corners[i][0],
			corners[i][1],
			1,
			1,
			x+corners[i][2],
			y+corners[i][3],
			0
		);
	}

	quick_draw(bmp->bitmap, x, y, 0);
}

ATLAS *atlas_create(int width, int height, int flags, int border, bool destroy_bmps)
{
	ATLAS *atlas;

	atlas = new ATLAS;

	atlas->finished = false;

	atlas->width = width;
	atlas->height = height;
	atlas->flags = flags;
	atlas->border = (flags & ATLAS_REPEAT_EDGES) ? 1 : border;
	atlas->destroy_bmps = destroy_bmps;

	atlas->sheets = NULL;
	atlas->items = NULL;

	return atlas;
}

bool atlas_add(ATLAS *atlas, MBITMAP *bitmap, int id)
{
	/* Add bitmaps from largest to smallest */
	int add_w = al_get_bitmap_width(bitmap->bitmap);
	int add_h = al_get_bitmap_height(bitmap->bitmap);
	//int add_area = add_w * add_h;
	int add_max = add_w > add_h ? add_w : add_h;


	std::vector<ABMP *>::iterator it;
	for (it = atlas->bmp_list.begin(); it != atlas->bmp_list.end(); it++) {
		ABMP *b = *it;
		MBITMAP *bmp = b->bitmap;
		int this_w = al_get_bitmap_width(bmp->bitmap);
		int this_h = al_get_bitmap_height(bmp->bitmap);
		//int this_area = this_w * this_h;
		int this_max = this_w > this_h ? this_w : this_h;

		if (this_max < add_max)
			break;
	}

	ABMP *b = create_bmp(bitmap, id);

	if (it != atlas->bmp_list.end()) {
		atlas->bmp_list.insert(it, b);
	}
	else {
		atlas->bmp_list.push_back(b);
	}

	return true;
}

/* Return number of bitmaps added to atlas */
int atlas_finish(ATLAS *atlas)
{
	ALLEGRO_STATE orig_state;
	al_store_state(&orig_state,
		ALLEGRO_STATE_BLENDER |
		ALLEGRO_STATE_TARGET_BITMAP);
	al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);

	atlas->num_items = 0;
	atlas->num_sheets = 0;

	std::vector<ARECT *> rect_list;
	insert_sheet(atlas, rect_list);

	int count = 0;
	
	if (atlas->bmp_list.size() == 0) {
		goto end;
	}

	for (size_t item = 0; item < atlas->bmp_list.size();) {
		ABMP *b = atlas->bmp_list[item];
		MBITMAP *bmp = b->bitmap;

		int bmp_w = al_get_bitmap_width(bmp->bitmap);
		int bmp_h = al_get_bitmap_height(bmp->bitmap);
		int req_w = bmp_w + (atlas->border * 2);
		int req_h = bmp_h + (atlas->border * 2);
		if (req_w > atlas->width || req_h > atlas->height) {
			item++;
			continue;
		}
		bool found = false;
		for (size_t rect_i = 0; rect_i < rect_list.size();) {	
			ARECT *rect = rect_list[rect_i];
			if (rect->w >= req_w && rect->h >= req_h) {
				found = true;
				ATLAS_ITEM *atlas_item = new ATLAS_ITEM;
				al_set_target_bitmap(atlas->sheets[rect->sheet]->bitmap);

				if (atlas->flags & ATLAS_REPEAT_EDGES) {
					draw_bitmap_with_borders(
						bmp,
						rect->x+1,
						rect->y+1
					);
				}
				else {
					quick_draw(
						bmp->bitmap,
						rect->x+atlas->border,
						rect->y+atlas->border,
						0
					);
				}


				atlas_item->sub = m_create_sub_bitmap(
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
						new ATLAS_ITEM *[1];
				}
				else {
					ATLAS_ITEM ** tmp = atlas->items;
					atlas->items =
					new ATLAS_ITEM *[atlas->num_items+1];
					for (
						int i = 0;
						i < atlas->num_items;
						i++
					)
					{
						atlas->items[i] =
							tmp[i];
					}
					delete[] tmp;
				}
				atlas->items[atlas->num_items] = atlas_item;
				atlas->num_items++;
				// Four possibilities
				if (rect->w == req_w && rect->h == req_h) {
					// exact match, remove rect
					delete rect_list[rect_i];
					rect_list.erase(rect_list.begin()+rect_i);
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
					rect_list.insert(rect_list.begin()+rect_i+1, r2);
					rect->y += req_h;
					rect->h -= req_h;
					rect->w = req_w;
				}
				break;
			}
			rect_i++;
		}
		if (!found) {
			insert_sheet(atlas, rect_list);
			continue;
		}
		item++;
		count++;
	}

end:

	for (size_t i = 0; i < rect_list.size(); i++) {
		delete rect_list[i];
	}

	al_restore_state(&orig_state);

	return count;
}

void atlas_destroy(ATLAS *atlas)
{
	int i;

	for (size_t i = 0; i < atlas->bmp_list.size(); i++) {
		ABMP *b = atlas->bmp_list[i];
		if (atlas->destroy_bmps) {
			m_destroy_bitmap(b->bitmap);
		}
		delete b;
	}

	for (i = 0; i < atlas->num_sheets; i++) {
		m_destroy_bitmap(atlas->sheets[i]);
	}
	delete[] atlas->sheets;
	for (i = 0; i < atlas->num_items; i++) {
		m_destroy_bitmap(atlas->items[i]->sub);
		delete atlas->items[i];
	}

	delete[] atlas->items;

	delete atlas;
}

