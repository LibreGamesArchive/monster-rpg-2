#include "c.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PATH 5000

extern char **myArgv;
extern int myArgc;

typedef int bool;

enum BOOLEAN {
	false = 0,
	true
};

/*
 * Get the path to the game resources. First checks for a
 * MONSTER_DATA environment variable that points to the resources,
 * then a system-wide resource directory then the directory
 * "data" from the current directory.
 */
#ifndef ALLEGRO_ANDROID
#ifndef ALLEGRO_WINDOWS
static char* resourcePath()
{
	static char path[MAX_PATH];

#ifdef ALLEGRO_IPHONE
	strcpy(path, "data/");
	return path;
#endif

	char* env = getenv("MONSTER2_DATA");

	if (env) {
		strcpy(path, env);
		if (path[strlen(path)-1] != '/') {
			strncat(path, "/", (sizeof(path)/sizeof(*path))-1);
		}
	}
	else {
		char tmp[1000];
		strcpy(tmp, myArgv[0]);
		int i = strlen(tmp)-1;
		bool found = false;
		for (; i >= 0; i--) {
			if (tmp[i] == '/' || tmp[i] == '\\') {
				tmp[i] = 0;
				found = true;
				break;
			}
		}
		if (found) {
			sprintf(path, "%s/data/", tmp);
		}
		else {
			strcpy(path, "data/");
		}
	}

	return path;
}
#else
static char* resourcePath()
{
	static char path[MAX_PATH];

	char* env = getenv("MONSTER2_DATA");

	if (env) {
		strcpy(path, env);
		if (path[strlen(path)-1] != '\\' && path[strlen(path)-1] != '/') {
			strncat(path, "/", (sizeof(path)/sizeof(*path))-1);
		}
	}
	else {
		strcpy(path, "x/data/");
	}

	return path;
}
#endif

const char *getResource(const char *fmt, ...)
{
	va_list ap;
	static char name[MAX_PATH];

	strcpy(name, resourcePath());

	va_start(ap, fmt);
	vsnprintf(name+strlen(name), (sizeof(name)/sizeof(*name))-1, fmt, ap);
	return name;
}
#endif

#ifdef ALLEGRO_ANDROID
const char* getResource(const char* fmt, ...)
{
   va_list ap;
   static char res[512];
   static ALLEGRO_PATH *dir;
   static ALLEGRO_PATH *path;

   va_start(ap, fmt);
   memset(res, 0, 512);
   snprintf(res, 511, fmt, ap);

   if (!dir) {
      dir = al_get_standard_path(ALLEGRO_RESOURCES_PATH);
      al_append_path_component(dir, "unpack");
   }

   if (path)
      al_destroy_path(path);

   path = al_create_path(res);
   al_rebase_path(dir, path);
   return al_path_cstr(path, '/');
}
#endif

