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
#ifndef ALLEGRO_WINDOWS
static char* resourcePath()
{
	static char path[MAX_PATH];

#ifdef IPHONE
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
#ifdef ALLEGRO4
		replace_filename(path, myArgv[0], "/data/", MAX_PATH);
#else
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
#endif
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

