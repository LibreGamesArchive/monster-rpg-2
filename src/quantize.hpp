#ifndef QUANTIZE_HPP
#define QUANTIZE_HPP

#include <allegro5/allegro.h>
#include <allegro5/allegro_opengl.h>
#define ASSERT ALLEGRO_ASSERT
#include <allegro5/internal/aintern_opengl.h>

struct Paletted_Image_Data
{
	unsigned char *data;
	int size;
};

void gen_palette(ALLEGRO_BITMAP *bitmap, unsigned char *palette, uint16_t *opt_exact);
void gen_paletted_image(ALLEGRO_BITMAP *bitmap, unsigned char *image, uint16_t *exact);
bool upload_paletted_image(ALLEGRO_BITMAP *bitmap, void *data);

#endif
