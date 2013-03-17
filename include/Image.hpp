#ifndef IMAGE_HPP
#define IMAGE_HPP


#include "monster2.hpp"


class Image {
public:
	void displayConvert()
	{
		bitmap = m_make_display_bitmap(bitmap);
	}

	int getWidth();
	int getHeight();
	int getX();
	int getY();

	void setTransparent(bool trans);
	bool getTransparent();
	MBITMAP *getBitmap();

	void set(MBITMAP *b);
	bool load(MBITMAP *copy_from, int x1, int y1, int x2, int y2);
	Image *clone(int type, MBITMAP *bitmap);
	Image *clone(int type, MBITMAP *clone_from, MBITMAP *clone_to, int x, int y);
	//void refresh();
	
	void draw(int x, int y, int flags = 0);
	void draw_trans(int x, int y, int alpha);

	Image();
	Image(bool alpha);
	/* bitmap is destroyed
	 */
	~Image();

protected:
	void setBlender();
	void unsetBlender();

	MBITMAP *bitmap;
	bool transparent;
	bool alpha;

	MBITMAP *copy_from;
	int x1, y1, w, h;
};

extern ALLEGRO_BITMAP *cached_bitmap;
extern std::string cached_bitmap_filename;

#endif

