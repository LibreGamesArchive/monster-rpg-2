#include "monster2.hpp"

Frame::Frame(Image *image, int delay) :
	image(image),
	delay(delay)
{
}

Frame::~Frame(void)
{
	delete image;
}

