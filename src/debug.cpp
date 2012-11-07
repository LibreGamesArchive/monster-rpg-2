#include "monster2.hpp"

void debug_message(const char *fmt, ...)
{
	va_list ap;
	char msg[1000];

	va_start(ap, fmt);
	vsnprintf(msg, (sizeof(msg)/sizeof(*msg))-1, fmt, ap);

//	printf("%s\n", msg);
	ALLEGRO_DEBUG("%s", msg);
}
