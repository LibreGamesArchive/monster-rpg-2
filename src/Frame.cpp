#include "monster2.hpp"

Frame *Frame::clone(int type, MBITMAP *clone_from, MBITMAP *clone_to, int col, int y, bool skip_draw)
{
	Frame *frame = new Frame(image->clone(type, clone_from, clone_to, col, y, skip_draw), delay);
	return frame;
}

Frame::Frame(Image *image, int delay) :
	image(image),
	delay(delay)
{
}

Frame::~Frame(void)
{
	delete image;
}

