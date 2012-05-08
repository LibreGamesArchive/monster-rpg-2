#include "monster2.hpp"

#define ASSERT ALLEGRO_ASSERT
#include <allegro5/internal/aintern_opengl.h>

#include "quantize.hpp"

ALLEGRO_BITMAP *cached_bitmap = NULL;
std::string cached_bitmap_filename = "";

int Image::getWidth(void)
{
	return m_get_bitmap_width(bitmap);
}


int Image::getHeight(void)
{
	return m_get_bitmap_height(bitmap);
}

MBITMAP *Image::getBitmap(void)
{
	return bitmap;
}


void Image::setTransparent(bool trans)
{
	//this->transparent = trans;
}


bool Image::getTransparent(void)
{
	return transparent;
}


void Image::set(MBITMAP *b)
{
	bitmap = b;
}

/* Load from file
 */
bool Image::load(const char *filename)
{
	bitmap = m_load_bitmap(filename);
	return bitmap;
}

class load_image_data : public RecreateData
{
public:
	std::string filename;
	int x1, y1, w, h;
};

static void load_image_callback(MBITMAP *bitmap, RecreateData *data)
{
	load_image_data *d = (load_image_data *)data;
// FIXME:
if (strstr(d->filename.c_str(), "eny-loader")) printf("eny bitmap=%p\n", bitmap);

	int flags = al_get_new_bitmap_flags();
	int format = al_get_new_bitmap_format();

	al_set_new_bitmap_flags(al_get_bitmap_flags(bitmap->bitmap));
	al_set_new_bitmap_format(al_get_bitmap_format(bitmap->bitmap));

	ALLEGRO_BITMAP *b;

	if (cached_bitmap && d->filename == cached_bitmap_filename) {
		b = cached_bitmap;
	}
	else {
		if (cached_bitmap) {
			al_destroy_bitmap(cached_bitmap);
		}
		cached_bitmap = my_load_bitmap(d->filename.c_str());
		cached_bitmap_filename = d->filename;
		b = cached_bitmap;
	}

	ALLEGRO_BITMAP *tmp = al_get_target_bitmap();

	#ifdef A5_OGL_XXX_DONT_USE

	int pitch = al_get_bitmap_width(bitmap->bitmap) * al_get_pixel_size(al_get_bitmap_format(bitmap->bitmap));

	unsigned char *buf = (unsigned char *)al_malloc(pitch * al_get_bitmap_height(bitmap->bitmap));

	ALLEGRO_LOCKED_REGION *lr = al_lock_bitmap_region(tmp, d->x1, d->y1, d->w, d->h, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READONLY);
	for (int y = 0; y < al_get_bitmap_height(bitmap->bitmap); y++) {
		unsigned char *dst = buf + pitch * y;
		unsigned char *src = ((unsigned char *)lr->data) + y * lr->pitch;
		memcpy(dst, src, pitch);
	}
	al_unlock_bitmap(tmp);

	ALLEGRO_BITMAP_EXTRA_OPENGL *e = (ALLEGRO_BITMAP_EXTRA_OPENGL *)bitmap->bitmap->extra;
	glDeleteTextures(1, &e->texture);
	e->texture = 0;

	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGBA,
		al_get_bitmap_width(bitmap->bitmap),
		al_get_bitmap_height(bitmap->bitmap),
		0,
		al_get_bitmap_format(bitmap->bitmap) == ALLEGRO_PIXEL_FORMAT_RGBA_4444 ? ALLEGRO_PIXEL_FORMAT_RGBA_4444 : ALLEGRO_PIXEL_FORMAT_ABGR_8888_LE,
		al_get_bitmap_format(bitmap->bitmap) == ALLEGRO_PIXEL_FORMAT_RGBA_4444 ? GL_UNSIGNED_SHORT_4_4_4_4 : GL_UNSIGNED_BYTE,
		buf
	);

	//_al_ogl_upload_bitmap_memory(bitmap->bitmap, al_get_bitmap_format(bitmap->bitmap), buf);

	al_free(buf);

	#else
	
	m_save_blender();

	ALLEGRO_DEBUG("bitmap=%p", bitmap);
	m_set_target_bitmap(bitmap);

	m_set_blender(M_ONE, M_ZERO, white);

	al_draw_bitmap_region(b, d->x1, d->y1, d->w, d->h, 0, 0, 0);

	al_set_target_bitmap(tmp);
	m_restore_blender();

	#endif

	al_set_new_bitmap_flags(flags);
	al_set_new_bitmap_format(format);
}

/* Copy from bitmap (coords are inclusive)
 */
bool Image::load(std::string copy_from, int x1, int y1, int x2, int y2)
{
	int w = x2 - x1;
	int h = y2 - y1;

	load_image_data *d = new load_image_data;
	d->filename = copy_from;
	d->x1 = x1;
	d->y1 = y1;
	d->w = w;
	d->h = h;
	
	if (alpha)
		bitmap = m_create_alpha_bitmap(w, h, load_image_callback, d); // check
	else
		bitmap = m_create_bitmap(w, h, load_image_callback, d); // check

//#ifdef ALLEGRO_ANDROID
#if 1
	// Convert to an 8 bit paletted texture
	ALLEGRO_DEBUG("Converting to paletted texture initial not clone");
	w = m_get_bitmap_width(bitmap);
	h = m_get_bitmap_height(bitmap);
	int true_w, true_h;
	al_get_opengl_texture_size(bitmap->bitmap, &true_w, &true_h);
	int sz = (256*4) + (true_w * true_h);
	unsigned char *imgdata = new unsigned char[sz];
	ALLEGRO_DEBUG("Trying palette stuff 2");
	uint16_t *exact = new uint16_t[256];
	ALLEGRO_DEBUG("Trying palette stuff 3");
	gen_palette(bitmap->bitmap, imgdata, exact);
	ALLEGRO_DEBUG("Trying palette stuff 4");
	gen_paletted_image(bitmap->bitmap, imgdata+(256*4), exact);
	ALLEGRO_DEBUG("Trying palette stuff 5");
	delete[] exact;
	ALLEGRO_DEBUG("Trying palette stuff 6");
	Paletted_Image_Data p;
	p.data = imgdata;
	p.size = sz;
	ALLEGRO_DEBUG("Trying palette stuff 7");
	ALLEGRO_BITMAP *pal = al_create_custom_bitmap(w, h, upload_paletted_image, &p);
	ALLEGRO_DEBUG("Trying palette stuff 8");
	ALLEGRO_DEBUG("pal=%p", pal);
	delete[] imgdata;

	al_destroy_bitmap(bitmap->bitmap);
	bitmap->bitmap = pal;
#endif

	return true;
}


void Image::draw(int x, int y, int flags)
{
	m_set_blender(M_ONE, M_INVERSE_ALPHA, white);
	m_draw_bitmap(bitmap, x, y, flags);
}

void Image::draw_trans(int x, int y, int alpha)
{
	m_draw_trans_bitmap(bitmap, x, y, alpha);
}

Image *Image::clone(int type)
{
	Image *img = new Image();
	img->transparent = transparent;
	img->alpha = alpha;

	/* This can be a generic clone by just using
	 * al_clone_bitmap, but right now we only need
	 * the white highlight
	 */

	/*
	img->bitmap = m_create_bitmap(
		m_get_bitmap_width(bitmap),
		m_get_bitmap_height(bitmap)
	);

	ALLEGRO_BITMAP *target = al_get_target_bitmap();
	m_set_target_bitmap(img->bitmap);
	m_clear(al_map_rgba_f(0, 0, 0 ,0));
	m_save_blender();
	m_set_blender(M_ONE, M_ZERO, white);
	m_draw_bitmap(bitmap, 0, 0, 0);
	m_restore_blender();
	*/

	
	// FIXME
	img->bitmap = m_clone_bitmap(bitmap);
	ALLEGRO_BITMAP *target = al_get_target_bitmap();
	m_set_target_bitmap(img->bitmap);
	if (type == CLONE_ENEMY || type == CLONE_PLAYER)
		add_blit(bitmap, 0, 0, white, 0.7, 0);
	al_set_target_bitmap(target);

//#ifdef ALLEGRO_ANDROID
#if 1
	// Convert to an 8 bit paletted texture
	ALLEGRO_DEBUG("Converting to paletted texture");
	int w = m_get_bitmap_width(img->bitmap);
	int h = m_get_bitmap_height(img->bitmap);
	int true_w, true_h;
	al_get_opengl_texture_size(img->bitmap->bitmap, &true_w, &true_h);
	int sz = (256*4) + (true_w * true_h);
	unsigned char *imgdata = new unsigned char[sz];
	ALLEGRO_DEBUG("Trying palette stuff 2");
	uint16_t *exact = new uint16_t[256];
	ALLEGRO_DEBUG("Trying palette stuff 3");
	gen_palette(img->bitmap->bitmap, imgdata, exact);
	ALLEGRO_DEBUG("Trying palette stuff 4");
	gen_paletted_image(img->bitmap->bitmap, imgdata+(256*4), exact);
	ALLEGRO_DEBUG("Trying palette stuff 5");
	delete[] exact;
	ALLEGRO_DEBUG("Trying palette stuff 6");
	Paletted_Image_Data p;
	p.data = imgdata;
	p.size = sz;
	ALLEGRO_DEBUG("Trying palette stuff 7");
	ALLEGRO_BITMAP *pal = al_create_custom_bitmap(w, h, upload_paletted_image, &p);
	ALLEGRO_DEBUG("Trying palette stuff 8");
	ALLEGRO_DEBUG("pal=%p", pal);
	delete[] imgdata;

	al_destroy_bitmap(img->bitmap->bitmap);
	img->bitmap->bitmap = pal;
#endif

	return img;
}

Image::Image(void)
{
	transparent = true;
	alpha = false;
}

Image::Image(bool alpha)
{
	transparent = true;
	this->alpha = alpha;
}

Image::~Image(void)
{
	m_destroy_bitmap(bitmap);
}

