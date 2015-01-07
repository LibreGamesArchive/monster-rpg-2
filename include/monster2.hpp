#ifndef M2_HPP
#define M2_HPP

#define _CRT_SECURE_NO_WARNINGS

#ifdef _MSC_VER
#define snprintf _snprintf
#define strcasecmp _stricmp
#endif

#define R2D(a) ((a)*180/M_PI)
#define D2R(a) ((a)*M_PI/180)

#include <cstdio>
#include <cstring>
#include <cstdarg>
#include <new>
#include <vector>
#include <string>
#include <sstream>
#include <cctype>
#include <iostream>
#include <fstream>
#include <ios>
#include <list>
#include <map>
#include <algorithm>


#include <math.h>

#define MS_CELL_SCENE 1
#define MS_ON_MOON 152
#define MS_GOT_ORB 166
#define MS_MET_HORSE 23
#define MS_FIRST_POISON 177
#define MS_FOREST_GOLD 179
#define MS_SWIPE_TO_ATTACK 190
#define MS_DRAG_TO_USE 191

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#if !defined ALLEGRO_ANDROID && !defined ALLEGRO_RASPBERRYPI
#include <allegro5/allegro_native_dialog.h>
#endif
//#include <allegro5/allegro_shader.h>
//#include <allegro5/allegro_shader_glsl.h>
#include <allegro5/allegro_memfile.h>
#ifdef ALLEGRO_ANDROID
#include <allegro5/allegro_physfs.h>
#include <physfs.h>
#endif

#include "user_events.h"

ALLEGRO_DEBUG_CHANNEL("morpg2")

#ifdef ALLEGRO_MACOSX
#include <objc/objc.h>
#ifndef __OBJC__
#define __OBJC__
#endif
#endif

#ifdef ALLEGRO_IPHONE
#include <allegro5/allegro_iphone.h>
#endif
#if defined ALLEGRO_ANDROID
#include <allegro5/allegro_android.h>
#endif

#ifndef ALLEGRO_WINDOWS
#define MAX_PATH 5000
#else
#include <allegro5/allegro_direct3d.h>
#undef _WIN32_IE
#define _WIN32_IE 0x400
#include <shlobj.h>
#endif

#if defined KCM_AUDIO
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#endif

#include <zlib.h>

#if !defined KCM_AUDIO
#include <bass.h>
#endif
#include "sound.hpp"

#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
#define ALPHA_FMT ALLEGRO_PIXEL_FORMAT_RGBA_4444
#elif defined A5_OGL
#define ALPHA_FMT ALLEGRO_PIXEL_FORMAT_ABGR_8888_LE
#else
#define ALPHA_FMT ALLEGRO_PIXEL_FORMAT_ARGB_8888
#endif


const int LETTER_INDEX = 3;

const int CURE_INDEX = 2;
const int HOLY_WATER_INDEX = 6;
const int HEAL_INDEX = 7;
const int ELIXIR_INDEX = 9;
const int JUICE_INDEX = 15;
const int CURE2_INDEX = 21;
const int CURE3_INDEX = 43;

const int M_OUTLINED = 0;
const int M_FILLED = 1;

const int MAX_SPELLS = 32;
/* MAX_SPELLS is huge, so the list scrolls on forever practically..
 * limit it for this game but keep it at 256 for save state compatibility ...edit: keep compatibility another way */
const int MAX_SPELLS_IN_THIS_GAME = 32;

#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
typedef ALLEGRO_FONT MFONT;

const int LOGIC_RATE = 60;

const int LOGIC_MILLIS = (1000/LOGIC_RATE);

#include "tgui.hpp"

extern "C" {
#include <lua5.2/lua.h>
#include <lua5.2/lauxlib.h>
#include <lua5.2/lualib.h>
#define lua_open luaL_newstate
}

#ifdef A5_D3D
#include <d3d9.h>
#endif

#if (defined ALLEGRO_WINDOWS) && !(defined A5_OGL)
#include <d3dx9.h>
#endif

#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
#define glUseProgramObjectARB glUseProgram
#define glGetUniformLocationARB glGetUniformLocation
#define glUniform1fARB glUniform1f
#define GLhandleARB GLuint
#define GL_FRAGMENT_SHADER_ARB GL_FRAGMENT_SHADER
#define glCreateShaderObjectARB glCreateShader
#define glShaderSourceARB glShaderSource
#define glCompileShaderARB glCompileShader
#define glCreateProgramObjectARB glCreateProgram
#define glAttachObjectARB glAttachShader
#define glLinkProgramARB glLinkProgram
#endif

#ifdef A5_OGL
#include <allegro5/allegro_opengl.h>
#endif

#if defined ALLEGRO_IPHONE || defined ALLEGRO_MACOSX
#define POOL_BEGIN NSAutoreleasePool *___p = [[NSAutoreleasePool alloc] init];
#define POOL_END [___p drain];
#endif



#ifdef ALLEGRO_MSVC
#define strdup _strdup
#endif


const int BW = 240;
const int BH = 160;
const int TILE_SIZE = 16;
const int TILE_LAYERS = 4;
const int WT = BW/TILE_SIZE;
const int HT = BH/TILE_SIZE;
const int BUTTON_SIZE = 25;


#ifndef uint
#define uint unsigned int
#endif


// FIXME: move this later
struct ScreenDescriptor {
	int width;
	int height;
	bool fullscreen;
};


struct MBITMAP {
	ALLEGRO_BITMAP *bitmap;
};

typedef ALLEGRO_COLOR MCOLOR;

struct MPoint {
	float x, y;
};

extern TGUIWidget *mainWidget;

extern bool fps_on;
extern int fps_frames;
extern double fps_counter;
extern int fps;


class Battle;

#ifndef MIN
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) > (b) ? (b) : (a))
#endif


enum CloneType
{
	CLONE_PLAYER = 1,
	CLONE_ENEMY,
	CLONE_FULL,
	CLONE_OBJECT,
	CLONE_COPY_BORDERS,
	CLONE_NOT_A_CLONE
};


#include "input_descriptor.h"


#include "translate.hpp"

#include "c.h"
#include "shooter.hpp"
#include "redundant.hpp"
#include "3d.hpp"
#include "Error.hpp"
#include "debug.hpp"
#include "io.hpp"
#include "xml.hpp"
#include "util.hpp"
#include "graphics.hpp"
#include "GameInfo.hpp"
#include "script.hpp"
#include "Image.hpp"
#include "Frame.hpp"
#include "Animation.hpp"
#include "AnimationSet.hpp"
#include "Tile.hpp"
#include "Area.hpp"
#ifdef ALLEGRO_IPHONE
#include "iphone.h"
#endif
#if defined ALLEGRO_IPHONE || defined ALLEGRO_MACOSX
#include "gamecenter.h"
#endif
#include "atlas.h"
#include "init.hpp"
#include "Configuration.hpp"
#include "Input.hpp"
#include "Object.hpp"
#include "classes.hpp"
#include "equipment.hpp"
#include "CombatInfo.hpp"
#include "Spells.hpp"
#include "inventory.hpp"
#include "CombatInfo.hpp"
#include "CombatStatus.hpp"
#if !defined NOSOUND
#include "lander.hpp"
#endif
#include "CombatActions.hpp"
#include "CombatPlayer.hpp"
#include "CombatEnemy.hpp"
#include "battle.hpp"
#include "Player.hpp"
#include "pause.hpp"
#include "GenericEffect.hpp"
#include "Items.hpp"

#ifndef NOSOUND // HACK: move code out of widgets.hpp (playPreloadedSample etc)
#include "widgets.hpp"
#endif

#include "my_load_bitmap.h"

extern uint32_t runtime;
extern int runtime_ms;
extern bool break_main_loop;
extern bool quit_game;
extern long runtime_start;

extern bool timer_on;
extern int timer_time;
extern bool battle_must_win;
extern bool tutorial_started;

extern Player *player;
extern bool battle_won;
extern bool battle_lost;
extern bool close_pressed;
extern bool close_pressed_for_configure;

extern char *saveFilename;

extern bool was_in_map;

extern bool global_can_save;

void do_close(bool quit = true);
void do_close_exit_game();

extern std::vector<std::pair<int, bool> > forced_milestones;
extern bool gonna_fade_in_red;

void update_orientation();
bool is_close_pressed(bool pump_events_only = false);
#define pump_events() is_close_pressed(true)

void connect_second_display();
extern int old_control_mode;
void connect_airplay_controls(bool really_airplay = false);
void disconnect_airplay_controls(bool really_airplay = false);
void clear_touches();

void main_draw();

extern bool dont_draw_now;

extern bool mouse_in_display;

extern bool prompt_for_close_on_next_flip;

#ifdef ALLEGRO_ANDROID
extern bool halted;
extern bool switched_in;
#endif

extern bool cmdline_warped;
extern int cmdline_warp_x;
extern int cmdline_warp_y;
extern std::string cmdline_warp_area;

bool is_modifier(int c);
extern int modifier_repeat_count[7];

void get_mouse_pos_in_buffer_coords(int *this_x, int *this_y);

#endif
