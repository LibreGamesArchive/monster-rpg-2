#ifndef IMAGE_HPP
#define IMAGE_HPP


#include "monster2.hpp"


class Image {
public:
	void displayConvert(void)
	{
		bitmap = m_make_display_bitmap(bitmap);
	}

	int getWidth(void);
	int getHeight(void);

	void setTransparent(bool trans);
	bool getTransparent(void);
	MBITMAP *getBitmap(void);

	void set(MBITMAP *b);
	/* Load from file
	 */
	bool load(const char *filename);
	/* Copy from bitmap (coords are inclusive)
	 */
	bool load(std::string filename, int x1, int y1, int x2, int y2);

	void draw(int x, int y, int flags = 0);
	void draw_trans(int x, int y, int alpha);

	Image(void);
	Image(bool alpha);
	/* bitmap is destroyed
	 */
	~Image(void);

protected:
	void setBlender(void);
	void unsetBlender(void);

	MBITMAP *bitmap;
	bool transparent;
	int oldSrcBlend, oldDstBlend;
	MCOLOR oldBlendColor;
	bool alpha;
};

#endif

