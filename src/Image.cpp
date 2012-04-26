#include "monster2.hpp"

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

void load_image_callback(MBITMAP *bitmap, RecreateData *data)
{
	load_image_data *d = (load_image_data *)data;

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
	m_save_blender();

	m_set_target_bitmap(bitmap);

	m_set_blender(M_ONE, M_ZERO, white);

	al_draw_bitmap_region(b, d->x1, d->y1, d->w, d->h, 0, 0, 0);

	al_set_target_bitmap(tmp);
	m_restore_blender();

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

	if (!bitmap)
		return false;

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

	img->bitmap = m_create_bitmap(
		m_get_bitmap_width(bitmap),
		m_get_bitmap_height(bitmap)
	);

	ALLEGRO_BITMAP *target = al_get_target_bitmap();
	m_set_target_bitmap(img->bitmap);
	m_clear(al_map_rgba_f(0, 0, 0 ,0));
	m_draw_bitmap(bitmap, 0, 0, 0);
	if (type == CLONE_ENEMY || type == CLONE_PLAYER)
		add_blit(bitmap, 0, 0, white, 0.7, 0);
	al_set_target_bitmap(target);

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

