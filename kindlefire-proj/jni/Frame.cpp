#include "monster2.hpp"

Frame *Frame::clone(int type, MBITMAP *bitmap)
{
	Frame *frame = new Frame(image->clone(type, bitmap), delay);
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

