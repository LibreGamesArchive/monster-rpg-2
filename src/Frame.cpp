#include "monster2.hpp"

Frame *Frame::clone(void)
{
	Frame *frame = new Frame(image->clone(), delay);
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

