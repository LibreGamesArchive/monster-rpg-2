#include <allegro.h>
#ifdef ALLEGRO_WINDOWS
#include <winalleg.h>
#undef _WIN32_IE
#define _WIN32_IE 0x400
#include <shlobj.h>
#endif
#include <tgui.hpp>
#include <unistd.h>
#include <string>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstdio>

#ifdef MAX_PATH
#undef MAX_PATH
#endif
#define MAX_PATH 5000


/*
 * Return the path to user resources (save states, configuration)
 */
#ifndef ALLEGRO_WINDOWS
static const char *userResourcePath()
{
	static char path[MAX_PATH];

	char* env = getenv("HOME");

	if (env) {
		strcpy(path, env);
		if (path[strlen(path)-1] != '/') {
			strncat(path, "/.monster2/", (sizeof(path)/sizeof(*path))-1);
		}
		else {
			strncat(path, ".monster2/", (sizeof(path)/sizeof(*path))-1);
		}
	}
	else {
		strcpy(path, "save/");
	}

	return path;
}
#else
static const char *userResourcePath()
{
	static char path[MAX_PATH];

	bool success = SHGetSpecialFolderPath(0, path, CSIDL_APPDATA, false);

	if (success) {
		if (path[strlen(path)-1] != '/') {
			strncat(path, "/Monster2/", (sizeof(path)/sizeof(*path))-1);	
		}
		else {
			strncat(path, "Monster2/", (sizeof(path)/sizeof(*path))-1);
		}
	}
	else {
		strcpy(path, "save/");
	}

	return path;
}
#endif

std::string getUserResource(const std::string fmt, ...)
{
	va_list ap;
	static char name[MAX_PATH];

	strcpy(name, userResourcePath());

	va_start(ap, fmt);
	vsnprintf(name+strlen(name), (sizeof(name)/sizeof(*name))-1, fmt.c_str(), ap);
	return std::string(name);
}


int main(int argc, char **argv)
{
#ifdef ALLEGRO_WINDOWS
	LPTSTR cmdline = GetCommandLine();
	argc = 1;
	bool in_quote = false;
	for (int i = 0; cmdline[i]; i++) {
		if (in_quote && cmdline[i] == '"') {
			in_quote = false;
		}
		else if (!in_quote && cmdline[i] == ' ') {
			while  (cmdline[i+1] == ' ')
				i++;
			argc++;
		}
		else if (!in_quote && cmdline[i] == '"') {
			in_quote = true;
		}
	}
	argv = new char *[argc];
	char buf[256];
	int k = 0;
	for (int i = 0; i < argc; i++) {
		int j = 0;
		while  (1) {
			if (!in_quote && cmdline[k] == '"') {
				in_quote = true;
				k++;
			}
			else
				buf[j++] = cmdline[k++];
			if (in_quote && cmdline[k] == '"' || cmdline[k] == 0) {
				k++;
				buf[j] = 0;
				break;
			}
			else if (!in_quote && cmdline[k] == ' ' || cmdline[k] == 0) {
				while  (cmdline[k] == ' ' && cmdline[k+1] == ' ')
					k++;
				k++;
				buf[j] = 0;
				break;
			}
		}
		argv[i] = strdup(buf);
	}

#endif

#if defined ALLEGRO_MACOSX
	const int NUM_CHOICES = 2;
	std::string choices[] = {
		"opengl", "quickdraw"
	};
#elif defined ALLEGRO_WINDOWS
	const int NUM_CHOICES = 3;
	std::string choices[] = {
		"direct3d", "opengl", "directdraw"
	};
#elif defined ALLEGRO_LINUX
	const int NUM_CHOICES = 2;
	std::string choices[] = {
		"opengl", "x11"
	};
#endif

	int iselected = -1;
	bool was_configged = false;
	bool bchecked = false;
	bool mouse_down = false;

	std::string filename = getUserResource("launch_config");
	FILE *f = fopen(filename.c_str(), "rb");
	if (f) {
		int byte = fgetc(f);
		if (byte != 255) {
			iselected = byte;
			was_configged = true;
		}
		fclose(f);
	}

	allegro_init();

	bool quit = false;

	if (iselected == -1) {
		install_keyboard();
		install_mouse();

		if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, 512, 512, 0, 0)) {
			allegro_message("Error setting graphics mode for launcher.\n");
			return 1;
		}

		set_window_title("Choose a display driver");
	
		char buf[1000];
#ifdef ALLEGRO_MACOSX
		sprintf(buf, "%s/%s", argv[0], "Contents/Resources/x/data/launcher.dat");
#else
		replace_filename(buf, argv[0], "x/data/launcher.dat", 1000);
#endif

		DATAFILE *dat = load_datafile(buf);

		BITMAP *bg = (BITMAP *)find_datafile_object(dat, "bg")->dat;
		BITMAP *checked = (BITMAP *)find_datafile_object(dat, "checked")->dat;
		BITMAP *unchecked = (BITMAP *)find_datafile_object(dat, "unchecked")->dat;
		BITMAP *selected = (BITMAP *)find_datafile_object(dat, "selected")->dat;
		BITMAP *unselected = (BITMAP *)find_datafile_object(dat, "unselected")->dat;
		BITMAP *remember = (BITMAP *)find_datafile_object(dat, "remember")->dat;

		BITMAP *choice_bmps[NUM_CHOICES];
		for (int i = 0; i < NUM_CHOICES; i++) {
			choice_bmps[i] = (BITMAP *)find_datafile_object(dat, choices[i].c_str())->dat;
		}

		iselected = 0;
		show_mouse(screen);

		bool redraw = true;

		while (true) {
			if (key[KEY_ESC]) {
				quit = true;
				break;
			}
			if (!mouse_down && (mouse_b & 1)) {
				mouse_down = true;
				for (int i = 0; i < 3; i++) {
				}
			}
			else if (mouse_down & !(mouse_b & 1)) {
				mouse_down = false;
				redraw = true;
				int mx = mouse_x;
				int my = mouse_y;
				for (int i = 0; i < NUM_CHOICES; i++) {
					if (mx > 50 && mx < 100 && my > 128+55*i && my < 178+55*i) {
						iselected = i;
					}
				}
				if (mx > 50 && mx < 100 && my > 300 && my < 350) {
					bchecked = !bchecked;
				}
				else if (mx> 280 && mx < 313 && my > 432 && my < 457) {
					break;
				}
				else if (mx > 346 && mx < 438 && my > 432 && my < 457) {
					quit = 1;
					break;
				}
			}
			if (redraw) {
				scare_mouse();
				redraw = false;
				acquire_screen();
				blit(bg, screen, 0, 0, 0, 0, bg->w, bg->h);
				for (int i = 0; i < NUM_CHOICES; i++) {
					int x = 50;
					int y = 128 + 55*i;
					if (i == iselected) {
						draw_sprite(screen, selected, x, y);
					}
					else {
						draw_sprite(screen, unselected, x, y);
					}
					draw_sprite(screen, choice_bmps[i], x+60, y);
				}
				if (bchecked) {
					draw_sprite(screen, checked, 50, 300);
				}
				else {
					draw_sprite(screen, unchecked, 50, 300);
				}
				draw_sprite(screen, remember, 110, 300);
				release_screen();
				unscare_mouse();
			}
			rest(1);
		}

		show_mouse(NULL);
		
		// Destroy data
		unload_datafile(dat);
		set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
		allegro_exit();
	}

	// write config
	f = fopen(getUserResource("launch_config").c_str(), "wb");
	if (f) {
		if (bchecked || was_configged) {
			fputc((uint8_t)iselected, f);
		}
		else {
			fputc(255, f);
		}
		fclose(f);
	}

	if (quit)
		return 0;

	// Launch
	char cmd[1000];
	char tmp[1000];

#if defined ALLEGRO_WINDOWS
	sprintf(tmp, "x\\monster2-%s.exe", choices[iselected].c_str());
	replace_filename(cmd, argv[0], tmp, 1000);
#elif defined ALLEGRO_MACOSX
	sprintf(cmd, "%s/Contents/Resources/x/monster2-%s", argv[0], choices[iselected].c_str());
#else
	sprintf(tmp, "x/monster2-%s", choices[iselected].c_str());
	replace_filename(cmd, argv[0], tmp, 1000);
#endif

#ifdef ALLEGRO_WINDOWS
	char full[1000];
	char tmp2[1000];
	strcpy(full, cmd);
	for (int i = 1; i < argc; i++) {
		sprintf(tmp2, "%s %s", full, argv[i]);
		strcpy(full, tmp2);
	}

	STARTUPINFO startupInfo = {0};
	startupInfo.cb = sizeof(startupInfo);

	PROCESS_INFORMATION processInformation;
	
	// Try to start the process
	BOOL result = ::CreateProcess(
	  NULL,
	  full,
	  NULL,
	  NULL,
	  FALSE,
	  NORMAL_PRIORITY_CLASS,
	  NULL,
	  NULL,
	  &startupInfo,
	  &processInformation
	);

	for (int i = 0; i < argc; i++) {
		delete argv[i];
	}
	delete[] argv;
#else
	char *args[argc+1];

	args[0] = cmd;
	for (int i = 1; i < argc; i++) {
		args[i] = argv[i];
	}
	args[argc] = NULL;

	if (fork() == 0)
		execvp(cmd, args);
#endif
}
END_OF_MAIN()

