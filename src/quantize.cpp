/* Based on explanation of algorithm in Allegro 4.4 source (quantize.c) */

// Palettes are 4x256 bytes (RGBA)

#include "monster2.hpp"

#include <map>
#include <vector>
#include <algorithm>

#include "quantize.hpp"

#ifdef ALLEGRO_ANDROID
extern "C" {
#include "allegro5/internal/aintern_android.h"
void glBindFramebufferOES(GLenum target, GLuint framebuffer);
}
#endif

// Number of most common colors to use
#define COMMON 220
// Number of most different colors to use
#define DIFFERENT (256-COMMON)

#define HASH_BITS 8
#define BUCKETS (1 << HASH_BITS)

struct Color {
	uint16_t pixel;
	int count;
};

struct Sorted {
	unsigned char start;
	uint16_t color;
};

/* Sort most to least used */
static int descending_sort(const void *a, const void *b)
{
	Color c1 = *((Color *)a);
	Color c2 = *((Color *)b);

	return c2.count - c1.count;
}

static int pixel_sort(const void *a, const void *b)
{
	Sorted s1 = *((Sorted *)a);
	Sorted s2 = *((Sorted *)b);

	return s1.color - s2.color;
}

/* Get 4x8 bit RGBA from 1x16 bit 4444 encoded pixel */
static void get_components(uint32_t color, int *components)
{
	components[0] = (color & 0xf000) >> 12;
	components[1] = (color & 0x0f00) >> 8;
	components[2] = (color & 0x00f0) >> 4;
	components[3] = (color & 0x000f);
	for (int j = 0; j < 4; j++) {
		components[j] = ((float)components[j] / 0xf) * 255;
	}
}

/*
 * Generate a decent 256 color palette from an image.
 */
void gen_palette(ALLEGRO_BITMAP *bitmap, unsigned char *palette)
{
	Color *map = new Color[65536];
	ALLEGRO_DEBUG("map=%p", map);
	memset(map, 0, sizeof(Color) * 65536);
	int sizes[BUCKETS] = { 0, };
	unsigned char *pos = palette;
	int components[4];
	int hash_mask = 0;

	for (int i = 0; i < HASH_BITS; i++) {
		hash_mask |= 1 << i;
	}

	ALLEGRO_DEBUG("in gen_palette, bitmap = %p", bitmap);

	ALLEGRO_LOCKED_REGION *lr = al_lock_bitmap(bitmap, ALLEGRO_PIXEL_FORMAT_RGBA_4444, ALLEGRO_LOCK_READONLY);

	ALLEGRO_DEBUG("lr = %p", lr);

	for (int y = 0; y < al_get_bitmap_height(bitmap); y++) {
		uint16_t *p = (uint16_t *)((unsigned char *)lr->data + y * lr->pitch);
		for (int x = 0; x < al_get_bitmap_width(bitmap); x++) {
			uint16_t pixel = *p++;
			int bucket = pixel & hash_mask;
			int start = bucket * (65536 / BUCKETS);
			int i;
			for (i = 0; i < sizes[bucket]; i++) {
				if (map[start+i].pixel == pixel) {
					break;
				}
			}
			if (i == sizes[bucket]) {
				sizes[bucket]++;
			}
			map[start+i].pixel = pixel;
			map[start+i].count++;
		}
	}
	
	al_unlock_bitmap(bitmap);

	int sz = sizes[0];
	for (int i = 1; i < BUCKETS; i++) {
		int start = i * (65536 / BUCKETS);
		for (int j = 0; j < sizes[i]; j++) {
			map[sz+j] = map[start+j];
		}
		sz += sizes[i];
	}

	if (sz > 256) {
		qsort(map, sz, sizeof(Color), descending_sort);
	}

	for (int i = 0; i < (sz <= 256 ? sz : COMMON); i++) {
		uint16_t c = map[i].pixel;
		get_components(c, components);
		for (int j = 0; j < 4; j++) {
			*pos++ = components[j];
		}
	}

	if (sz <= 256) {
		delete[] map;
		return;
	}

	for (int i = sz-1, count = 0; count < DIFFERENT; i--, count++) {
		uint16_t c = map[i].pixel;
		get_components(c, components);
		for (int j = 0; j < 4; j++) {
			*pos++ = components[j];
		}
	}

	delete[] map;
}

static int dist(int a, int b)
{
	if (a < b)
		return b - a + 1;
	else
		return (65535 - a) + b + 1;
}

void gen_paletted_image(ALLEGRO_BITMAP *bitmap, unsigned char *palette, unsigned char *image)
{
	unsigned char *pos = image;
	unsigned char *pos2 = palette;
	int components[4];

	unsigned char *lookup = new unsigned char[65536];
	uint16_t uint16_colors[256];

	for (int i = 0; i < 256; i++) {
		for (int j = 0; j < 4; j++) {
			components[j] = *pos2++;
		}
		uint16_t pixel = ((components[0] & 0xf0) << 8) | ((components[1] & 0xf0) << 4) |
			(components[2] & 0xf0) | ((components[3] & 0xf0) >> 4);
		uint16_colors[i] = pixel;
	}

	Sorted s[256];

	for (int i = 0; i < 256; i++) {
		s[i].start = i;
		s[i].color = uint16_colors[i];
	}

	qsort(&s, 256, sizeof(Sorted), pixel_sort);

	for (int i = 0; i < 256; i++) {
		int start, end;
		int pixel = s[i].color;
		if (i == 0)
			start = 0;
		else
			start = pixel - (pixel - s[i-1].color) / 2;
		if (i == 255)
			end = 65535;
		else {
			end = pixel + (s[i+1].color - pixel) / 2;
			if (end % 2 == 0) end++;
		}
		for (int j = start; j <= end; j++) {
			lookup[j] = s[i].start;
		}
	}

	ALLEGRO_LOCKED_REGION *lr = al_lock_bitmap(bitmap, ALLEGRO_PIXEL_FORMAT_RGBA_4444, ALLEGRO_LOCK_READONLY);

	for (int y = al_get_bitmap_height(bitmap)-1; y >= 0; y--) {
		uint16_t *p = (uint16_t *)((unsigned char *)lr->data + y * lr->pitch);
		for (int x = 0; x < al_get_bitmap_width(bitmap); x++) {
			uint16_t pixel = *p++;
			*pos++ = lookup[pixel];
		}
	}
	
	al_unlock_bitmap(bitmap);

	delete[] lookup;
}

bool upload_paletted_image(ALLEGRO_BITMAP *bitmap, void *data)
{
	ALLEGRO_BITMAP_EXTRA_OPENGL *ogl_bitmap = (ALLEGRO_BITMAP_EXTRA_OPENGL *)bitmap->extra;
	int w = bitmap->w;
	int h = bitmap->h;
	GLenum e;
	Paletted_Image_Data *imgdata = (Paletted_Image_Data *)data;
	bool deletebuf = false;

	glGenTextures(1, &ogl_bitmap->texture);
	e = glGetError();
	if (e) {
		ALLEGRO_ERROR("glGenTextures failed");
	}
	else {
		ALLEGRO_DEBUG("Created new OpenGL texture <paletted> %d", ogl_bitmap->texture);
	}

	GLint fbo;
#ifdef ALLEGRO_ANDROID
	fbo = _al_android_get_curr_fbo();
#else
	glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, &fbo);
#endif
#if defined ALLEGRO_ANDROID || defined ALLEGRO_IPHONE
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, 0);
#else
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
#endif
#ifdef ALLEGRO_ANDROID
	_al_android_set_curr_fbo(0);
#endif

	glBindTexture(GL_TEXTURE_2D, ogl_bitmap->texture);
	e = glGetError();
	if (e) {
		ALLEGRO_ERROR("glBindTexture failed");
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	unsigned char *tmpbuf;
	/* If size != actual texture size, create a temporary buffer that is */
	if (w != ogl_bitmap->true_w || h != ogl_bitmap->true_h) {
		tmpbuf = (unsigned char *)al_calloc(1, imgdata->size);
		memcpy(tmpbuf, imgdata->data, 256*4);
		for (int y = 0; y < h; y++) {
			unsigned char *src = imgdata->data + y * w + (256*4);
			unsigned char *dst = tmpbuf + y * ogl_bitmap->true_w + (256*4);
			memcpy(dst, src, w);
		}
		deletebuf = true;
	}
	else {
		tmpbuf = imgdata->data;
	}

	glCompressedTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_PALETTE8_RGBA8_OES,
		ogl_bitmap->true_w,
		ogl_bitmap->true_h,
		0,
		imgdata->size,
		tmpbuf
	);

	if (deletebuf) {
		al_free(tmpbuf);
	}
	
	e = glGetError();
	if (e) {
		ALLEGRO_ERROR("glCompressedTexImage2D failed <paletted> (%d)", e);
		glDeleteTextures(1, &ogl_bitmap->texture);
		ogl_bitmap->texture = 0;
		return false;
	}
      
#if defined ALLEGRO_ANDROID || defined ALLEGRO_IPHONE
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, fbo);
#else
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
#endif
#ifdef ALLEGRO_ANDROID
	_al_android_set_curr_fbo(fbo);
#endif
	
	ogl_bitmap->left = 0;
	ogl_bitmap->right = (float) w / ogl_bitmap->true_w;
	ogl_bitmap->top = (float) h / ogl_bitmap->true_h;
	ogl_bitmap->bottom = 0;

	return true;
}

