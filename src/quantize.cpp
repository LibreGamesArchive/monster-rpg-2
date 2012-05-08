/* Based on explanation of algorithm in Allegro 4.4 source (quantize.c) */

// Palettes are 4x256 bytes (RGBA)

#include "monster2.hpp"

#include <map>
#include <vector>
#include <algorithm>

#include "quantize.hpp"

// Number of most common colors to use
#define COMMON 192
// Number of most different colors to use
#define DIFFERENT (256-COMMON)

struct Color {
	uint16_t color;
	int count;
	int difference;
};

static bool cmp(const Color &a, const Color &b)
{
	return a.count <= b.count;
}

static bool cmp_difference(const Color &a, const Color &b)
{
	return a.difference >= b.difference;
}

static void get_components(Color *c, int *components)
{
	components[0] = (c->color & 0xf000) >> 12;
	components[1] = (c->color & 0x0f00) >> 8;
	components[2] = (c->color & 0x00f0) >> 4;
	components[3] = (c->color & 0x000f);
	for (int j = 0; j < 4; j++) {
		components[j] = ((float)components[j] / 0xf) * 255;
	}
}

void gen_palette(ALLEGRO_BITMAP *bitmap, unsigned char *palette, uint16_t *opt_exact)
{
	std::map<uint16_t, int> count;

	ALLEGRO_LOCKED_REGION *lr = al_lock_bitmap(bitmap, ALLEGRO_PIXEL_FORMAT_RGBA_4444, ALLEGRO_LOCK_READONLY);

	for (int y = 0; y < al_get_bitmap_height(bitmap); y++) {
		uint16_t *p = (uint16_t *)((unsigned char *)lr->data + y * lr->pitch);
		for (int x = 0; x < al_get_bitmap_width(bitmap); x++) {
			uint16_t pixel = *p++;
			count[pixel]++;
		}
	}
	
	al_unlock_bitmap(bitmap);

	std::vector<Color> v;

	std::map<uint16_t, int>::iterator it;
	for (it = count.begin(); it != count.end(); it++) {
		std::pair<uint16_t, int> p = *it;
		Color c;
		c.color = p.first;
		c.count = p.second;
		v.push_back(c);
	}

	count.clear();

	unsigned char *pos = palette;
	int components[4];
	int components2[4];
	
	if (v.size() > 256) {
		std::sort(v.begin(), v.end(), cmp);
	}

	ALLEGRO_DEBUG("HERE 1");

	// FIXME
	int loops = 0;

	for (int i = 0; i < (v.size() <= 256 ? v.size() : COMMON); i++) {
		Color &c = v[i];
		get_components(&c, components);
		for (int j = 0; j < 4; j++) {
			*pos++ = components[j];
		}
		loops++;
		ALLEGRO_DEBUG("loops = %d\n", loops);
		if (opt_exact) {
			*opt_exact++ = c.color;
		}
	}

	if (v.size() <= 256) {
		v.clear();
	for (int i = 0; i < 256; i++) {
		int idx = i * 4;
		ALLEGRO_DEBUG("pal[%d] %d %d %d %d", i, palette[idx], palette[idx+1], palette[idx+2], palette[idx+3]);
	}
		return;
	}

	ALLEGRO_DEBUG("HERE 2");

	for (int i = COMMON; i < v.size(); i++) {
		Color &c = v[i];
		c.difference = INT_MAX;
		get_components(&c, components);
		for (int j = 0; j < COMMON; j++) {
			Color &c2 = v[j];
			get_components(&c2, components2);
			int diff = 0;
			for (int k = 0; k < 4; k++) {
				diff += abs(components[k] - components2[k]);
			}
			if (diff < c.difference) {
				c.difference = diff;
			}
		}
	}

	ALLEGRO_DEBUG("HERE 3");

	v.erase(v.begin(), v.begin() + COMMON);

	std::sort(v.begin(), v.end(), cmp_difference);

	ALLEGRO_DEBUG("HERE 4");
	
	for (int i = 0; i < 256-COMMON && i < v.size(); i++) {
		ALLEGRO_DEBUG("BEGIN LOOP");
		Color &c = v[i];
		get_components(&c, components);
		for (int j = 0; j < 4; j++) {
			*pos++ = components[j];
		}
		loops++;
		ALLEGRO_DEBUG("loops = %d\n", loops);
		if (opt_exact) {
			*opt_exact++ = c.color;
		}
		ALLEGRO_DEBUG("END LOOP");
	}
	
	v.clear();

	for (int i = 0; i < 256; i++) {
		int idx = i * 4;
		ALLEGRO_DEBUG("pal[%d] %d %d %d %d", i, palette[idx], palette[idx+1], palette[idx+2], palette[idx+3]);
	}

	ALLEGRO_DEBUG("DONE");
}

void gen_paletted_image(ALLEGRO_BITMAP *bitmap, unsigned char *image, uint16_t *exact)
{
	unsigned char *pos = image;

	ALLEGRO_LOCKED_REGION *lr = al_lock_bitmap(bitmap, ALLEGRO_PIXEL_FORMAT_RGBA_4444, ALLEGRO_LOCK_READONLY);

	for (int y = al_get_bitmap_height(bitmap)-1; y >= 0; y--) {
		uint16_t *p = (uint16_t *)((unsigned char *)lr->data + y * lr->pitch);
		for (int x = 0; x < al_get_bitmap_width(bitmap); x++) {
			uint16_t pixel = *p++;
			for (int i = 0; i < 256; i++) {
				if (pixel == exact[i]) {
					*pos = i;
					break;
				}
			}
			pos++;
		}
	}
	
	al_unlock_bitmap(bitmap);
}

bool upload_paletted_image(ALLEGRO_BITMAP *bitmap, void *data)
{
	ALLEGRO_BITMAP_EXTRA_OPENGL *ogl_bitmap = (ALLEGRO_BITMAP_EXTRA_OPENGL *)bitmap->extra;
	int w = bitmap->w;
	int h = bitmap->h;
	GLenum e;
	Paletted_Image_Data *imgdata = (Paletted_Image_Data *)data;
	bool deletebuf = false;

	// FIXME
	GLint num;
	glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS, &num);
	ALLEGRO_DEBUG("num=%d", num);
	GLint f[num];
	glGetIntegerv(GL_COMPRESSED_TEXTURE_FORMATS, f);
	for (int i = 0; i < num; i++) {
		ALLEGRO_DEBUG("f[%d]=%d", i, f[i]);
	}

	ALLEGRO_DEBUG("imgdata size=%d data=%p", imgdata->size, imgdata->data);

	if (ogl_bitmap->texture == 0) {
		glGenTextures(1, &ogl_bitmap->texture);
		e = glGetError();
		if (e) {
			ALLEGRO_ERROR("glGenTextures failed");
			printf("glGenTextures failed\n");
		}
		else {
			ALLEGRO_DEBUG("Created new OpenGL texture <paletted> %d", ogl_bitmap->texture);
			printf("Created new OpenGL texture <paletted> %d\n", ogl_bitmap->texture);
		}
	}

	ALLEGRO_DEBUG("AH1");

	glBindTexture(GL_TEXTURE_2D, ogl_bitmap->texture);
	ALLEGRO_DEBUG("AH2");
	e = glGetError();
	if (e) {
		ALLEGRO_ERROR("glBindTexture failed");
		printf("glBindTexture failed\n");
	}

	ALLEGRO_DEBUG("Bound texture");
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	unsigned char *tmpbuf;
	if (w != ogl_bitmap->true_w || h != ogl_bitmap->true_h) {
		ALLEGRO_DEBUG("size != true size");
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

	ALLEGRO_DEBUG("true_w=%d true_h=%d", ogl_bitmap->true_w, ogl_bitmap->true_h);
	
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
		printf("glCompressedTexImage2D failed <paletted> (%d)\n", e);
		glDeleteTextures(1, &ogl_bitmap->texture);
		ogl_bitmap->texture = 0;
		return false;
	}
	
	ALLEGRO_DEBUG("glCompressed....");

	ogl_bitmap->left = 0;
	ogl_bitmap->right = (float) w / ogl_bitmap->true_w;
	ogl_bitmap->top = (float) h / ogl_bitmap->true_h;
	ogl_bitmap->bottom = 0;

	return true;
}

