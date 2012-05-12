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

#if 0
class load_image_data : public RecreateData
{
public:
	// for normal
	std::string filename;
	int x1, y1, w, h;

	// for clone
	MBITMAP *bitmap;
	int type;
};

static void load_image_callback(MBITMAP *bitmap, RecreateData *data)
{
	load_image_data *d = (load_image_data *)data;

	ALLEGRO_BITMAP *b;

	if (cached_bitmap && d->filename == cached_bitmap_filename) {
		b = cached_bitmap;
	}
	else {
		if (cached_bitmap) {
			al_destroy_bitmap(cached_bitmap);
		}
		cached_bitmap = my_load_bitmap(d->filename.c_str());
		//cached_bitmap = make_paletted(cached_bitmap);
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

	//bitmap->bitmap = make_paletted(bitmap->bitmap);
}

static void load_image_callback_clone(MBITMAP *bitmap, RecreateData *data)
{
	load_image_data *d = (load_image_data *)data;

	ALLEGRO_BITMAP *b;

	if (cached_bitmap && d->filename == cached_bitmap_filename) {
		b = cached_bitmap;
	}
	else {
		if (cached_bitmap) {
			al_destroy_bitmap(cached_bitmap);
		}
		cached_bitmap = my_load_bitmap(d->filename.c_str());
		//cached_bitmap = make_paletted(cached_bitmap);
		cached_bitmap_filename = d->filename;
		b = cached_bitmap;
	}

	ALLEGRO_BITMAP *tmp = al_get_target_bitmap();
	m_set_target_bitmap(bitmap);
	m_clear(m_map_rgba(0, 0, 0, 0));
	m_save_blender();
	m_set_blender(ALLEGRO_ONE, ALLEGRO_ZERO, white);
	m_draw_bitmap(d->bitmap, 0, 0, 0);
	m_restore_blender();
	if (d->type == CLONE_ENEMY || d->type == CLONE_PLAYER)
		add_blit(d->bitmap, 0, 0, white, 0.7, 0);

	al_set_target_bitmap(tmp);

	//bitmap->bitmap = make_paletted(bitmap->bitmap);
}
#endif

/* Copy from bitmap (coords are inclusive)
 */
bool Image::load(MBITMAP *copy_from, int x1, int y1, int x2, int y2)
{
	this->copy_from = copy_from;
	this->x1 = x1;
	this->y1 = y1;
	w = x2 - x1;
	h = y2 - y1;

	bitmap = m_create_sub_bitmap(copy_from, x1, y1, w, h);

	return true;
}

/*
void Image::refresh(void)
{
	ALLEGRO_DEBUG("in refresh 1 bitmap = %p", bitmap);
	m_destroy_bitmap(bitmap);
	ALLEGRO_DEBUG("in refresh 2");
	bitmap = m_create_sub_bitmap(copy_from, x1, y1, w, h);
	ALLEGRO_DEBUG("in refresh 3");
}
*/

void Image::draw(int x, int y, int flags)
{
	m_set_blender(M_ONE, M_INVERSE_ALPHA, white);
	m_draw_bitmap(bitmap, x, y, flags);
}

void Image::draw_trans(int x, int y, int alpha)
{
	m_draw_trans_bitmap(bitmap, x, y, alpha);
}

Image *Image::clone(int type, MBITMAP *copy)
{
	Image *img = new Image();
	img->transparent = transparent;
	img->alpha = alpha;
	img->copy_from = copy;
	img->x1 = x1;
	img->y1 = y1;
	img->w = w;
	img->h = h;

	img->bitmap = m_create_sub_bitmap(copy, x1, y1, w, h);

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

