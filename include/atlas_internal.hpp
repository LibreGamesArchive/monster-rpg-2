#ifndef TEXATLAS_INTERNAL_H
#define TEXATLAS_INTERNAL_H

#include <allegro5/internal/aintern_list.h>
#include "atlas.hpp"

#ifdef __cplusplus
extern "C" {
#endif

struct ATLAS_ITEM {
	MBITMAP *sub;
	int sheet;
	int x;
	int y;
	int id;
};

struct ATLAS {
	bool finished;
	int width;
	int height;
	int border;
	bool destroy_bmps;
	int num_sheets;
	MBITMAP **sheets;
	int num_items;
	ATLAS_ITEM **items;
	_AL_LIST *bmp_list;
};

#ifdef __cplusplus
}
#endif

#endif
