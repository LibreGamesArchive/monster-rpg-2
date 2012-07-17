#include "monster2.hpp"

void debug_message(const char *fmt, ...)
{
//	if (!config.showDebugMessages())
//		return;

	va_list ap;
	char msg[1000];

	va_start(ap, fmt);
	vsnprintf(msg, (sizeof(msg)/sizeof(*msg))-1, fmt, ap);

	//FILE *f = fopen("/mnt/sd/m2wiz/m2_log.txt", "a");
	/*
	FILE *f = fopen("m2_log.txt", "a");
	if (f) {
		fprintf(f, "%s", msg);
		fclose(f);
	}
	*/

//	printf("%s\n", msg);
	ALLEGRO_DEBUG("%s", msg);
}
