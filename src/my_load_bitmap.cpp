#include "monster2.hpp"
#include "svg.hpp"

#ifdef ALLEGRO_ANDROID
#include "java.h"
#endif

ALLEGRO_BITMAP *my_load_bitmap(
	const char *filename, bool ok_to_fail)
{
	return al_load_bitmap(filename);
}

