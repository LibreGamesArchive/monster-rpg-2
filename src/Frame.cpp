#include "monster2.hpp"

Frame *Frame::clone(int type)
{
	Frame *frame = new Frame(image->clone(type), delay);
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

