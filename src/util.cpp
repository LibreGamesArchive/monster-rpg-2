#include "monster2.hpp"

#ifdef ALLEGRO_WINDOWS
#define mkdir(a, b) _mkdir(a)
#endif

#include <sys/stat.h>

#ifdef ALLEGRO_ANDROID
#include "java.h"
#endif

int myArgc;
char **myArgv;
double last_shake_check;


#if defined ALLEGRO_IPHONE
/*
 * Return the path to user resources (save states, configuration)
 */
static char *userResourcePath()
{
	static char path[MAX_PATH];

	ALLEGRO_PATH *user_path = al_get_standard_path(ALLEGRO_USER_DOCUMENTS_PATH);
	sprintf(path, "%s/", al_path_cstr(user_path, '/'));
	al_destroy_path(user_path);
	return path;
}
#endif

// NOTE: a5 changed path from home/Library/Preferences to Documents before 5.0.0
const char *getUserResource(const char *fmt, ...)
{
	va_list ap;

#ifdef ALLEGRO_IPHONE
	char file[MAX_PATH];
	static char result[MAX_PATH];
	char old[MAX_PATH];

	// This stuff is for backwards compatibility when
	// saves and screenshots etc were stored in Library/Preferences
	sprintf(file, "%s/MoRPG2", userResourcePath());
	if (!al_filename_exists(file))
		mkdir(file, 0755);

	va_start(ap, fmt);
	vsnprintf(file, MAX_PATH, fmt, ap);
	va_end(ap);

	sprintf(old, "%s/Library/Preferences/%s", getenv("HOME"), file);
	sprintf(result, "%s/MoRPG2/%s", userResourcePath(), file);

	if (al_filename_exists(old)) {
		rename(old, result);
	}
#else
	char s1[MAX_PATH];
	char s2[MAX_PATH];
	static char result[MAX_PATH];

	ALLEGRO_PATH *user_path = al_get_standard_path(ALLEGRO_USER_SETTINGS_PATH);
	strcpy(s1, al_path_cstr(user_path, ALLEGRO_NATIVE_PATH_SEP));
	al_drop_path_tail(user_path);
	strcpy(s2, al_path_cstr(user_path, ALLEGRO_NATIVE_PATH_SEP));
	al_destroy_path(user_path);

	if (!al_filename_exists(s2))
		mkdir(s2, 0755);
	if (!al_filename_exists(s1))
		mkdir(s1, 0755);

	va_start(ap, fmt);
	vsnprintf(s2, MAX_PATH, fmt, ap);
	va_end(ap);

	sprintf(result, "%s/%s", s1, s2);
#endif
	
	return result;
}

/*
 * Get the path to the game resources. First checks for a
 * MONSTER_DATA environment variable that points to the resources,
 * then a system-wide resource directory then the directory
 * "data" from the current directory.
 */
#ifndef ALLEGRO_ANDROID
static char* resourcePath()
{
	char tmp[MAX_PATH];
	static char result[MAX_PATH];

	ALLEGRO_PATH *resource_path = al_get_standard_path(ALLEGRO_RESOURCES_PATH);
	strcpy(tmp, al_path_cstr(resource_path, ALLEGRO_NATIVE_PATH_SEP));
	al_destroy_path(resource_path);
	sprintf(result, "%s/data/", tmp);

	return result;
}
#endif

const char *getResource(const char *fmt, ...)
{
	va_list ap;
	static char name[MAX_PATH];

#ifndef ALLEGRO_ANDROID
	strcpy(name, resourcePath());
#else
	strcpy(name, "assets/data/");
#endif
	va_start(ap, fmt);
	vsnprintf(name+strlen(name), (sizeof(name)/sizeof(*name))-1, fmt, ap);
	va_end(ap);

	return name;
}

bool pointInBox(int px, int py, int x1, int y1, int x2, int y2)
{
	if (px >= x1 && px < x2 && py >= y1 && py < y2)
		return true;
	return false;
}


const char *my_itoa(int i)
{
	static char buf[20];
	sprintf(buf, "%d", i);
	return buf;
}

int countOccurances(const char *s, char c)
{
	const char *p = s;
	int count = 0;

	for (int i = 0; p[i]; i++) {
		if (p[i] == c)
			count++;
	}

	return count;
}

const char *findOccurance(const char *p, char c, int num)
{
	for (int i = 0; p[i]; i++) {
		if (p[i] == c) {
			num--;
			if (num == 0)
				return p+i;
		}
	}

	return NULL;
}


int check_arg(int argc, char **argv, const char *s)
{
	debug_message("argc=%d\n", argc);
	for (int i = 1; i < argc && argv[i]; i++) {
		if (!strcmp(argv[i], s))
			return i;
	}
	return -1;
}


// returns true to continue
void native_error(const char *msg, const char *msg2)
{
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID || defined ALLEGRO_RASPBERRYPI
	fprintf(stderr, "%s\n", msg);
	exit(1);
#elif defined EDITOR
	return;
#else

	const char *ss = msg2 ? strstr(msg2, "data/") : NULL;
	if (ss) {
		ss += 5;
	}

	prepareForScreenGrab1();
	m_clear(black);
	prepareForScreenGrab2();

	if (inited) {
		if (prompt(msg, "Continue anyway?", 1, 0, ss ? ss : "", NULL, true)) {
			set_target_backbuffer();
			m_clear(al_map_rgb_f(0, 0, 0));
			drawBufferToScreen();
			m_flip_display();
			return;
		}
		else {
			exit(1);
		}
	}

#if !defined(__linux__)
	char buf[1000];
	const char *crap = "Error";
	snprintf(buf, 1000, "%s Continue anyway?", msg);
#ifdef ALLEGRO_MACOSX
	int button = al_show_native_message_box(display, ss ? ss : crap, ":(", buf, NULL, ALLEGRO_MESSAGEBOX_YES_NO);
#else
	int button = al_show_native_message_box(display, crap, ss ? ss : ":(", buf, NULL, ALLEGRO_MESSAGEBOX_YES_NO);
#endif

	if (button == 1) return;
	else {
		exit(1);
	}
#else
	fprintf(stderr, "%s\n", msg);
	exit(1);
#endif
#endif
}


bool isVowel(char c)
{
	const char *vowels = "aeiouAEIOU";
	char const *ptr = vowels;
	bool ret = false;

	while (*ptr) {
		if (c == *ptr) {
			ret = true;
			break;
		}
		ptr++;
	}

	return ret;
}

double iphone_line_times[4] = { -9999, };
double iphone_shake_time = -9999;
static bool need_release = false;

bool iphone_line(IPHONE_LINE_DIR dir, double since)
{
	if (need_release) {
		if (released) {
			need_release = false;
		}
		else {
			iphone_clear_line(dir);
			return false;
		}
	}
	if (al_current_time()-iphone_line_times[dir] < since) {
		need_release = true;
		return true;
	}
	return false;
}

bool iphone_shaken(double since)
{
#if defined ALLEGRO_IPHONE
	if (use_dpad) {
		return false;
	}
#endif
#if defined ALLEGRO_ANDROID
	if (!on_title_screen && use_dpad) return false;
#endif

	last_shake_check = al_get_time();

	if (al_current_time()-iphone_shake_time < since) {
		return true;
	}
	return false;
}

void iphone_clear_line(IPHONE_LINE_DIR dir)
{
	iphone_line_times[dir] = -9999;
}

void iphone_clear_shaken(void)
{
	iphone_shake_time = -9999;
}
