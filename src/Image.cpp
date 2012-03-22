#include "monster2.hpp"


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
	this->transparent = trans;
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


/* Copy from bitmap (coords are inclusive)
 */
bool Image::load(MBITMAP *copy_from, int x1, int y1, int x2, int y2, bool is_pvr)
{
	int w = x2 - x1;
	int h = y2 - y1;
	
	if (is_pvr) {
		bitmap = al_create_sub_bitmap(copy_from, x1, y1, w, h);
	}
	else {
		if (alpha)
			bitmap = m_create_alpha_bitmap(w, h);
		else
			bitmap = m_create_bitmap(w, h);

		if (!bitmap)
			return false;

		MBITMAP *tmp = m_get_target_bitmap();
			m_save_blender();

		m_set_target_bitmap(bitmap);

		m_set_blender(M_ONE, M_ZERO, white);

		m_draw_bitmap_region(copy_from, x1, y1, w, h, 0, 0, 0);

		m_set_target_bitmap(tmp);
			m_restore_blender();
	}

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

