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
}


bool Image::getTransparent(void)
{
	return transparent;
}


void Image::set(MBITMAP *b)
{
	bitmap = b;
}

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

