#ifndef TEXATLAS_INTERNAL_H
#define TEXATLAS_INTERNAL_H

#include <vector>

#include "atlas.h"

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

typedef struct {
	int x, y, w, h;
	int sheet;
} ARECT;

typedef struct {
	MBITMAP *bitmap;
	int id;
} ABMP;

struct ATLAS {
	bool finished;
	int width;
	int height;
	int flags;
	int border;
	bool destroy_bmps;
	int num_sheets;
	MBITMAP **sheets;
	int num_items;
	ATLAS_ITEM **items;
	std::vector <ABMP *> bmp_list;
};

#ifdef __cplusplus
}
#endif

#endif
