
#if defined __APPLE__
#import <Foundation/Foundation.h>
#endif

#include "monster2.hpp"

#define ASSERT ALLEGRO_ASSERT
#include <allegro5/internal/aintern_bitmap.h>
#include <allegro5/internal/aintern_display.h>
#ifdef ALLEGRO_IPHONE
#include <allegro5/platform/aintiphone.h>
extern "C" {
#include <allegro5/internal/aintern_iphone.h>
}
#endif

#ifdef A5_D3D
#include <allegro5/allegro_direct3d.h>
#include <allegro5/allegro_shader_hlsl.h>
#endif

#if defined ALLEGRO_IPHONE || defined ALLEGRO_MACOSX
#include "joypad.hpp"
#endif

#ifdef ALLEGRO_IPHONE
extern "C" {
#include <allegro5/allegro_iphone_objc.h>
}
#endif

void get_buffer_true_size(int *buffer_true_w, int *buffer_true_h)
{
#ifdef A5_OGL
	al_get_opengl_texture_size(buffer->bitmap, buffer_true_w, buffer_true_h);
#else
	al_get_d3d_texture_size(buffer->bitmap, buffer_true_w, buffer_true_h);
#endif
}

#ifdef A5_D3D
LPDIRECT3DSURFACE9 big_depth_surface = NULL;
ALLEGRO_MUTEX *d3d_resource_mutex;
bool d3d_device_lost = false;

static void shaders_lost(void)
{
#if 0
	al_get_direct3d_effect(default_shader)->OnLostDevice();
	al_get_direct3d_effect(cheap_shader)->OnLostDevice();
	al_get_direct3d_effect(tinter)->OnLostDevice();
	al_get_direct3d_effect(brighten)->OnLostDevice();
	al_get_direct3d_effect(warp)->OnLostDevice();
	al_get_direct3d_effect(shadow_shader)->OnLostDevice();
#endif
}

static void shaders_found(void)
{
#if 0
	al_get_direct3d_effect(default_shader)->OnResetDevice();
	al_get_direct3d_effect(cheap_shader)->OnResetDevice();
	al_get_direct3d_effect(tinter)->OnResetDevice();
	al_get_direct3d_effect(brighten)->OnResetDevice();
	al_get_direct3d_effect(warp)->OnResetDevice();
	al_get_direct3d_effect(shadow_shader)->OnResetDevice();
#endif
}

static void init_big_depth_surface(void)
{
	LPDIRECT3DDEVICE9 dev = al_get_d3d_device(display);
	dev->CreateDepthStencilSurface(
		1024, 1024,
		D3DFMT_D16,
		D3DMULTISAMPLE_NONE, 0,
		true,
		&big_depth_surface,
		NULL
	);
	dev->SetDepthStencilSurface(big_depth_surface);
}

static void d3d_resource_restore(void)
{
	init_big_depth_surface();
	shaders_found();
	shooter_restoring = true;
	d3d_device_lost = false;
}

static void d3d_resource_release(void)
{
	al_lock_mutex(d3d_resource_mutex);
	d3d_device_lost = true;
	big_depth_surface->Release();
	shaders_lost();
	al_unlock_mutex(d3d_resource_mutex);
}
#endif

#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
bool do_pause_game = false;
#endif

bool achievement_show = false;
double achievement_time = 0;
MBITMAP *achievement_bmp;
int PRESERVE_TEXTURE = ALLEGRO_NO_PRESERVE_TEXTURE;
bool reload_translation = false;
static std::string replayMusicName = "";

bool do_toggle_fullscreen = false;

// cheats
bool superpower = false, healall = false;

static bool pause_joystick_repeat_events = false;
bool pause_f_to_toggle_fullscreen = false;

#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
bool have_mouse = false;
#else
bool have_mouse = true;
#endif
bool use_digital_joystick = false;
int screen_offset_x, screen_offset_y;
float screen_ratio_x, screen_ratio_y;
#ifdef ALLEGRO_IPHONE
static double allegro_iphone_shaken = 0.0;
#endif
float initial_screen_scale = 0.0f;
bool sound_was_playing_at_program_start;
#ifndef ALLEGRO4
ALLEGRO_DISPLAY *display = 0;
ALLEGRO_DISPLAY *controller_display = 0;
ALLEGRO_COND *wait_cond;
ALLEGRO_MUTEX *wait_mutex;
ALLEGRO_MUTEX *joypad_mutex;
int exit_event_thread = 0;
ALLEGRO_SHADER *controller_shader;
ALLEGRO_SHADER *default_shader;
ALLEGRO_SHADER *cheap_shader;
ALLEGRO_SHADER *tinter;
ALLEGRO_SHADER *warp;
ALLEGRO_SHADER *shadow_shader;
ALLEGRO_SHADER *brighten;
ALLEGRO_SHADER *scale2x;
ALLEGRO_SHADER *scale2x_flipped;
ALLEGRO_SHADER *scale3x;
ALLEGRO_SHADER *scale3x_flipped;
ALLEGRO_SHADER *scale2x_linear;
ALLEGRO_SHADER *scale2x_linear_flipped;
ALLEGRO_SHADER *scale3x_linear;
ALLEGRO_SHADER *scale3x_linear_flipped;
#endif
MBITMAP *buffer = 0;
MBITMAP *overlay = 0;
MBITMAP *scaleXX_buffer = 0;
MBITMAP *screenshot = 0;
MBITMAP *tilemap = 0;
bool *tileTransparent = 0;
int numTiles = 0;
MCOLOR white, black, grey, blue;
MBITMAP *tile;
char start_cwd[1000];
XMLData *tilemap_data = NULL;
XMLData *trans_data = NULL;
AnimationSet *guiAnims = NULL;
XMLData *terrain;
std::vector<WaterData> waterData;
float screenScale = 2;
bool use_programmable_pipeline = true;

#ifdef ALLEGRO_IPHONE
bool create_airplay_mirror = false;
bool delete_airplay_mirror = false;
bool airplay_connected = false;
#endif

//MBITMAP *profileBg;
MBITMAP *cursor;
MFONT *huge_font;
MFONT *medium_font;
MBITMAP *stoneTexture;
MBITMAP *mushroom;
MBITMAP *webbed;
MBITMAP *orb_bmp;
MBITMAP *poison_bmp;
MBITMAP *poison_bmp_tmp;
MBITMAP *poison_bmp_tmp2;
MBITMAP *shakeAlternativeBitmap;

GameInfo gameInfo;

static bool safemode = false;

volatile int draw_counter = 0;
volatile int logic_counter = 0;

bool gfx_mode_set = false;

#ifdef WIZ
MBITMAP *tmp_texture;
unsigned char *tmp_buffer;
//float sintab[SIN_TAB_SIZE+1];
//float costab[COS_TAB_SIZE+1];
int startGFXDriver;
#endif

bool egl_workaround = false;
bool inited = false;

#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
bool use_dpad = false;
#else
bool use_dpad = true;
#endif

int dpad_type;
bool dpad_at_top = false;
bool released = true;
static int last_mouse_x = -1, last_mouse_y;
static int total_mouse_x, total_mouse_y;
int click_x, click_y;
int event_mouse_x, event_mouse_y;
int current_mouse_x = -1, current_mouse_y = -1;
ALLEGRO_MUTEX *click_mutex;
ALLEGRO_MUTEX *input_mutex;
ALLEGRO_MUTEX *dpad_mutex;
ALLEGRO_MUTEX *touch_mutex;
//ALLEGRO_MUTEX *orient_mutex;
int exit_minor_event_thread = 0;
float area_panned_x = 0;
float area_panned_y = 0;
static AnimationSet *eny_loader;
static AnimationSet *dot_loader;
static MBITMAP *bg_loader, *loading_loader, *bar_loader;
MBITMAP *corner_bmp;
bool had_battle = false;
MBITMAP *shadow_corners[4]; // top left then clockwise
MBITMAP *shadow_sides[4]; // top then clockwise
ALLEGRO_VERTEX triangle_lines[32];
MBITMAP *dpad_buttons;
bool onscreen_swipe_to_attack;
bool onscreen_drag_to_use;
//bool mouse_off = false;
bool reinstall_timer;
ALLEGRO_TIMER *draw_timer;
ALLEGRO_TIMER *logic_timer;
bool drew_shadow = false;
MBITMAP *batteryIcon;
bool global_draw_red = true;
bool red_off_press_on = false;
volatile bool loading_done = false;
MBITMAP *stomach_circle;

bool scr_small = false;
bool scr_tiny = false;

char versionString[10];
int versionMajor = 1;
int versionMinor = 0;

bool switched_out = false;
ALLEGRO_MUTEX *switch_mutex;
ALLEGRO_COND *switch_cond;
uint32_t my_opengl_version;

#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
static bool should_pause_game(void)
{
	return (area && !battle && !player_scripted && !in_pause && !in_map);
}
#endif

ScreenSize small_screen(void)
{
	if (scr_tiny) {
		return ScreenSize_Tiny;
	}
	else if (scr_small) {
		return ScreenSize_Smaller;
	}

	ALLEGRO_MONITOR_INFO mi;
	al_get_monitor_info(config.getAdapter(), &mi);
	int w = mi.x2 - mi.x1;
	int h = mi.y2 - mi.y1;
	if (w < 1024 || h < 768) {
		return ScreenSize_Small;
	}

	return ScreenSize_Normal;
}

bool is_ipad(void)
{
	static bool first = true;
	static bool value;
	
	if (first) {
		first = false;
#if (__IPHONE_OS_VERSION_MAX_ALLOWED >= 30200)
		if ([[UIDevice currentDevice] respondsToSelector: @selector(userInterfaceIdiom)])
			value = ([UIDevice currentDevice].userInterfaceIdiom == UIUserInterfaceIdiomPad);
		else
			value = false;
#else
		value = false;
#endif
	}
	
	return value;
}

#ifdef ALLEGRO_WINDOWS
static void print_windows_error(void)
{
#ifdef ALLEGRO_WINDOWS
	DWORD err = GetLastError();
	LPTSTR *buffer = NULL;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err, 0, (LPTSTR)&buffer, 0, NULL);
	printf("%s\n", buffer);
	LocalFree(buffer);
#endif
}
#endif

static int progress_percent = 0;

static void show_progress(int percent)
{
#if defined WIZ
	MBITMAP *old_target = m_get_target_bitmap();
	//m_set_target_bitmap(m_get_backbuffer());
	m_set_target_bitmap(buffer);

	m_clear(black);
	percent = 100 - percent;
	const int BAR_W = 200;
	const int BAR_H = 16;
	float p = percent / 100.0;
	int bar_width = p * BAR_W;
	if (bar_width > BAR_W)
		bar_width = BAR_W;
	else if (bar_width <= 0)
		bar_width = 1;
	int x = BW/2;
	int y = 20;
	al_draw_filled_rectangle((x-BAR_W/2)-6, (y-BAR_H/2)-5,
		(x+BAR_W/2)+5, (y+BAR_H/2)+5, white);
	al_draw_filled_rectangle((x-BAR_W/2)-4, (y-BAR_H/2)-3,
		(x+BAR_W/2)+3, (y+BAR_H/2)+3, black);
	if (p > 0) {
		MCOLOR colors[4];
		colors[0] = m_map_rgb(255, 0, 0);
		colors[1] = m_map_rgb(255, 155, 0);
		colors[2] = m_map_rgb(255, 255, 0);
		colors[3] = m_map_rgb(0, 255, 0);
		MCOLOR c = colors[(bar_width-1)/(BAR_W/4)];
		m_draw_rectangle((x-BAR_W/2)-1, y-BAR_H/2,
			(x-BAR_W/2)+bar_width, y+BAR_H/2, c,
			M_FILLED);
	}
	int w = harpy->getWidth();
	int h = harpy->getHeight();
	float scale = (BW+100)/(float)w;
	harpy->drawScaled(-50, 50, w*scale, h*scale, M_FLIP_HORIZONTAL);
	drawBufferToScreen();
	m_flip_display();

	m_set_target_bitmap(old_target);
#else
	progress_percent = percent;
#endif
}

static void get_inputs(int x, int y, bool *l, bool *r, bool *u, bool *d, bool *b1, bool *b2, bool *b3)
{
	*l = *r = *u = *d = *b1 = *b2 = *b3 = false;

#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_ANDROID
	return;
#else

	int xx = BW-BUTTON_SIZE-20;
	int xx2 = xx + BUTTON_SIZE;
	xx -= 5;
	xx2 += 5;

	if (x > xx & x < xx+BUTTON_SIZE) {
		if (dpad_at_top) {
			if (y < BUTTON_SIZE+10) {
				if (config.getSwapButtons())
					*b1 = true;
				else
					*b2 = true;
			}
		}
		else {
			if (y > BH-10-BUTTON_SIZE) {
				if (config.getSwapButtons())
					*b1 = true;
				else
					*b2 = true;
			}
		}
	}

	xx = BW-BUTTON_SIZE*2-25;
	xx2 = xx + BUTTON_SIZE;
	xx -= 5;
	xx2 += 5;

	if (x > xx & x < xx+BUTTON_SIZE) {
		if (dpad_at_top) {
			if (y < BUTTON_SIZE+10) {
				if (config.getSwapButtons())
					*b2 = true;
				else
					*b1 = true;
			}
		}
		else {
			if (y > BH-10-BUTTON_SIZE) {
				if (config.getSwapButtons())
					*b2 = true;
				else
					*b1 = true;
			}
		}
	}

	if (dpad_at_top) {
		y -= 5;
	}
	else {
		y -= BH-(BUTTON_SIZE*3)-5;
	}

	x -= 5;

	if (x < BUTTON_SIZE*3 && y < BUTTON_SIZE*3) {
		float angle = atan2(y-BUTTON_SIZE*1.5, x-BUTTON_SIZE*1.5);
		while (angle < 0) angle += (M_PI*2);
		const float fortyfive = M_PI/4;
		if (angle >= fortyfive && angle < (fortyfive*3)) {
			x = 1; y = 2;
		}
		else if (angle >= (fortyfive*3) && angle < (fortyfive*5)) {
			x = 0; y = 1;
		}
		else if (angle >= (fortyfive*5) && angle < (fortyfive*7)) {
			x = 1; y = 0;
		}
		else {
			x = 2; y = 1;
		}
	}

	switch (x) {
		case 0:
			switch (y) {
				case 1:
					*l = true;
					break;
			}
			break;
		case 1:
			switch (y) {
				case 0:
					*u = true;
					break;
				case 2:
					*d = true;
					break;
			}
			break;
		case 2:
			switch (y) {
				case 1:
					*r = true;
					break;
			}
			break;
	}
#endif
}

struct Touch {
	int touch_id;
	int x;
	int y;
	int onscreen_button;
};

const int MAX_TOUCHES = 10;

Touch touches[MAX_TOUCHES] = { { -1, -1, -1, - 1 }, };

volatile int curr_touches = 0;

const int MOUSE_DOWN = 1;
const int MOUSE_UP = 2;
const int MOUSE_AXES = 3;

void myTguiIgnore(int type)
{
	tguiIgnore(type);
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
	if (!(config.getDpadType() == DPAD_TOTAL_1 || config.getDpadType() == DPAD_TOTAL_2)) {
		al_lock_mutex(touch_mutex);
		getInput()->set(false, false, false, false, false, false, false);
		for (int i = 0; i < MAX_TOUCHES; i++) {
			touches[i].x = touches[i].y = -1;
		}
		curr_touches = 0;
		al_unlock_mutex(touch_mutex);
	}
	#endif
}

static int find_touch(int touch_id)
{
	for (int i = 0; i < MAX_TOUCHES; i++) {
		if (touches[i].touch_id == touch_id)
			return i;
	}
	
	return -1;
}

static void process_touch(int x, int y, int touch_id, int type) {
	if (have_mouse)
		return;
	
	al_lock_mutex(touch_mutex);
	if (type == MOUSE_DOWN) {
		if (curr_touches >= MAX_TOUCHES) {
			al_unlock_mutex(touch_mutex);
			return;
		}
		touches[curr_touches].x = x;
		touches[curr_touches].y = y;
		touches[curr_touches].touch_id = touch_id;
		touches[curr_touches].onscreen_button = -1;
		curr_touches++;
	}
	else if (type == MOUSE_UP) {
		if (curr_touches > 0) {
			int idx = find_touch(touch_id);
			for (; idx < MAX_TOUCHES-1; idx++) {
				touches[idx].x = touches[idx+1].x;
				touches[idx].y = touches[idx+1].y;
				touches[idx].touch_id = touches[idx+1].touch_id;
			}
			curr_touches--;
		}
	}
	else { // MOVE
		int idx = find_touch(touch_id);
		touches[idx].x = x;
		touches[idx].y = y;
	}
	al_unlock_mutex(touch_mutex);
}

static std::list<ZONE> zones;

std::list<ZONE>::iterator define_zone(int x1, int y1, int x2, int y2)
{
	std::pair<int, int> a;
	std::pair<int, int> b;

	a.first = x1;
	a.second = y1;

	b.first = x2;
	b.second = y2;

	ZONE z;

	z.first = a;
	z.second = b;

	return zones.insert(zones.begin(), z);
}

void delete_zone(std::list<ZONE>::iterator it)
{
	zones.erase(it);
}

bool zone_defined(int x, int y)
{
	std::list<ZONE>::iterator it;
	for (it = zones.begin(); it != zones.end(); it++) {
		ZONE z = *it;
		std::pair<int, int> a = z.first;
		std::pair<int, int> b = z.second;

		if (x > a.first && y > a.second && x < b.first && y < b.second) {
			return true;
		}
	}

	return false;
}

static ALLEGRO_EVENT_QUEUE *events;
static ALLEGRO_EVENT_QUEUE *events_minor;

void register_display(ALLEGRO_DISPLAY *display)
{
	al_register_event_source(events, al_get_display_event_source(display));
	al_register_event_source(events_minor, al_get_display_event_source(display));
}

/* FIXME: Any new Effects have to be freed/restored here! */
static void *thread_proc(void *arg)
{
	bool f5_cheated = false, f6_cheated = false;
	double f5_time = -1, f6_time = -1;

	events = al_create_event_queue();
	
	ALLEGRO_EVENT event;
	
#if defined A5_D3D || defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
	al_register_event_source(events, al_get_display_event_source(display));
#endif

#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_ANDROID
	al_register_event_source(events, al_get_keyboard_event_source());
	al_register_event_source(events, al_get_joystick_event_source());
#endif
	
	draw_timer = al_create_timer(1.0/config.getTargetFPS());
	logic_timer = al_create_timer(1.0/LOGIC_RATE);
	
	al_start_timer(draw_timer);
	al_start_timer(logic_timer);
	
	al_register_event_source(events, (ALLEGRO_EVENT_SOURCE *)draw_timer);
	al_register_event_source(events, (ALLEGRO_EVENT_SOURCE *)logic_timer);
	
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
	al_register_event_source(events, al_get_touch_input_event_source());
#else
	al_register_event_source(events, al_get_mouse_event_source());
#endif

	double next_shake = al_current_time();
	
	while  (exit_event_thread != 1) {
		if (reinstall_timer) {
			al_destroy_timer(draw_timer);
			draw_timer = al_create_timer(1.0/config.getTargetFPS());
			al_start_timer(draw_timer);
			al_register_event_source(events, (ALLEGRO_EVENT_SOURCE *)draw_timer);
			reinstall_timer = false;
		}
		
		if (!f5_cheated && f5_time > 0 && al_get_time() > f5_time+5) {
			f5_cheated = true;
			superpower = !superpower;
			loadPlayDestroy("nooskewl.ogg");
		}
		if (!f6_cheated && f6_time > 0 && al_get_time() > f6_time+5) {
			f6_cheated = true;
			healall = true;
			loadPlayDestroy("Cure.ogg");
		}
		
		if (al_wait_for_event_timed(events, &event, 1.0f/LOGIC_RATE)) {
		
#if !defined ALLEGRO_IPHONE && #defined ALLEGRO_ANDROID
			al_lock_mutex(input_mutex);
			if (getInput())
				getInput()->handle_event(&event);
			al_unlock_mutex(input_mutex);
#endif

			if (event.type == ALLEGRO_EVENT_TIMER) {
				if (event.timer.source == draw_timer) {
					draw_counter++;
				}
				else if (event.timer.source == logic_timer) {
					logic_counter++;
					// Handle joystick repeat events
					al_lock_mutex(joypad_mutex);
					if (!pause_joystick_repeat_events) {
						for (int i = 0; i < JOY_NUM_REPEATABLE; i++) {
							if (joystick_repeat_started[i] == false) {
								continue;
							}
							if (joystick_initial_repeat_countdown[i] > 0) {
								joystick_initial_repeat_countdown[i] -= LOGIC_MILLIS;
							}
							else {
								joystick_repeat_countdown[i] -= LOGIC_MILLIS;
								if (joystick_repeat_countdown[i] <= 0) {
									joystick_repeat_countdown[i] = JOY_REPEAT_TIME;
									add_input_event(joystick_repeat_events[i]);
								}
							}
						}
					}
					al_unlock_mutex(joypad_mutex);
				}
				al_signal_cond(wait_cond);
			}
#if !defined ALLEGRO_IPHONE && #defined ALLEGRO_ANDROID
			else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
				if (event.keyboard.keycode == ALLEGRO_KEY_F) {
					if (!pause_f_to_toggle_fullscreen) {
						do_toggle_fullscreen = true;
					}
				}
				else if (event.keyboard.keycode == ALLEGRO_KEY_S) {
					if (!pause_f_to_toggle_fullscreen) {
						close_pressed_for_configure = true;
						close_pressed = true;
					}
				}
				else if (event.keyboard.keycode == ALLEGRO_KEY_F1) {
					int v = config.getMusicVolume();
					if (v <= 26) v = 0;
					else v = v - 26; 
					config.setMusicVolume(v);
					setMusicVolume(getMusicVolume());
					setAmbienceVolume(getAmbienceVolume());
				}
				else if (event.keyboard.keycode == ALLEGRO_KEY_F2) {
					int v = config.getMusicVolume();
					if (v >= 230) v = 255;
					else v = v + 26; 
					config.setMusicVolume(v);
					setMusicVolume(getMusicVolume());
					setAmbienceVolume(getAmbienceVolume());
				}
				else if (event.keyboard.keycode == ALLEGRO_KEY_F3) {
					int v = config.getSFXVolume();
					if (v <= 26) v = 0;
					else v = v - 26; 
					config.setSFXVolume(v);
				}
				else if (event.keyboard.keycode == ALLEGRO_KEY_F4) {
					int v = config.getSFXVolume();
					if (v >= 230) v = 255;
					else v = v + 26; 
					config.setSFXVolume(v);
				}
				else if (event.keyboard.keycode == ALLEGRO_KEY_F5) {
					f5_time = al_get_time();
				}
				else if (event.keyboard.keycode == ALLEGRO_KEY_F6) {
					f6_time = al_get_time();
				}
				else if (event.keyboard.keycode == ALLEGRO_KEY_F12) {
					reload_translation = true;
				}
			}
			else if (event.type == ALLEGRO_EVENT_KEY_CHAR) {
				INPUT_EVENT ie = EMPTY_INPUT_EVENT;
				int code = event.keyboard.keycode;
				if (code == ALLEGRO_KEY_LEFT || code == config.getKeyLeft()) {
					ie.left = DOWN;
					add_input_event(ie);
				}
				else if (code == ALLEGRO_KEY_RIGHT || code == config.getKeyRight()) {
					ie.right = DOWN;
					add_input_event(ie);
				}
				else if (code == ALLEGRO_KEY_UP || code == config.getKeyUp()) {
					ie.up = DOWN;
					add_input_event(ie);
				}
				else if (code == ALLEGRO_KEY_DOWN || code == config.getKeyDown()) {
					ie.down = DOWN;
					add_input_event(ie);
				}
				else if (code == ALLEGRO_KEY_ENTER || code == config.getKey1()) {
					ie.button1 = DOWN;
					add_input_event(ie);
				}
				else if (code == ALLEGRO_KEY_ESCAPE || code == config.getKey2()) {
					ie.button2 = DOWN;
					add_input_event(ie);
				}
				else if (code == ALLEGRO_KEY_V || code == config.getKey3()) {
					ie.button3 = DOWN;
					add_input_event(ie);
				}
			}
			else if (event.type == ALLEGRO_EVENT_KEY_UP) {
				INPUT_EVENT ie = EMPTY_INPUT_EVENT;
				int code = event.keyboard.keycode;
				if (code == ALLEGRO_KEY_LEFT || code == config.getKeyLeft()) {
					ie.left = UP;
					add_input_event(ie);
				}
				else if (code == ALLEGRO_KEY_RIGHT || code == config.getKeyRight()) {
					ie.right = UP;
					add_input_event(ie);
				}
				else if (code == ALLEGRO_KEY_UP || code == config.getKeyUp()) {
					ie.up = UP;
					add_input_event(ie);
				}
				else if (code == ALLEGRO_KEY_DOWN || code == config.getKeyDown()) {
					ie.down = UP;
					add_input_event(ie);
				}
				else if (code == ALLEGRO_KEY_ENTER || code == config.getKey1()) {
					dpad_panning = false;
					ie.button1 = UP;
					add_input_event(ie);
				}
				else if (code == ALLEGRO_KEY_ESCAPE || code == config.getKey2()) {
					ie.button2 = UP;
					add_input_event(ie);
				}
				else if (code == ALLEGRO_KEY_V || code == config.getKey3()) {
					ie.button3 = UP;
					add_input_event(ie);
				}
				else if (event.keyboard.keycode == ALLEGRO_KEY_F5) {
					f5_time = -1;
					f5_cheated = false;
				}
				else if (event.keyboard.keycode == ALLEGRO_KEY_F6) {
					f6_time = -1;
					f6_cheated = false;
				}
			}
			else if (event.type == ALLEGRO_EVENT_JOYSTICK_CONFIGURATION) {
				al_reconfigure_joysticks();
				if (al_get_num_joysticks() == 0) {
					num_joystick_buttons = 0;
					config.setGamepadAvailable(false);
				}
				else if (al_get_num_joysticks() > 0) {
					num_joystick_buttons = al_get_joystick_num_buttons(al_get_joystick(0));
					config.setGamepadAvailable(true);
				}
				getInput()->reconfig();
			}
			else if (event.type == ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN) {
				if (event.joystick.button == config.getJoyButton1()) {
					joy_b1_down();
				}
				else if (event.joystick.button == config.getJoyButton2()) {
					joy_b2_down();
				}
				else if (event.joystick.button == config.getJoyButton3()) {
					joy_b3_down();
				}
			}
			else if (event.type == ALLEGRO_EVENT_JOYSTICK_BUTTON_UP) {
				if (event.joystick.button == config.getJoyButton1()) {
					joy_b1_up();
				}
				else if (event.joystick.button == config.getJoyButton2()) {
					joy_b2_up();
				}
				else if (event.joystick.button == config.getJoyButton3()) {
					joy_b3_up();
				}
			}
			else if (event.type == ALLEGRO_EVENT_JOYSTICK_AXIS) {
				int axis = event.joystick.axis;
				float pos = event.joystick.pos;
				if (axis == 0) {
					if (joy_axes[0] == -1) {
						if (pos >= -0.5) {
							joy_axes[0] = 0;
							joy_l_up();
						}
					}
					else if (joy_axes[0] == 1) {
						if (pos <= 0.5) {
							joy_axes[0] = 0;
							joy_r_up();
						}
					}
					else {
						if (pos < -0.5) {
							joy_axes[0] = -1;
							joy_l_down();
						}
						else if (pos > 0.5) {
							joy_axes[0] = 1;
							joy_r_down();
						}
					}
				}
				else if (axis == 1) {
					if (joy_axes[1] == -1) {
						if (pos >= -0.5) {
							joy_axes[1] = 0;
							joy_u_up();
						}
					}
					else if (joy_axes[1] == 1) {
						if (pos <= 0.5) {
							joy_axes[1] = 0;
							joy_d_up();
						}
					}
					else {
						if (pos > 0.5) {
							joy_axes[1] = 1;
							joy_d_down();
						}
						else if (pos < -0.5) {
							joy_axes[1] = -1;
							joy_u_down();
						}
					}
				}
			}
#endif

#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
#define BEGIN ALLEGRO_EVENT_TOUCH_BEGIN
#define END ALLEGRO_EVENT_TOUCH_END
#define MOVE ALLEGRO_EVENT_TOUCH_MOVE
#else
#define BEGIN ALLEGRO_EVENT_MOUSE_BUTTON_DOWN
#define END ALLEGRO_EVENT_MOUSE_BUTTON_UP
#define MOVE ALLEGRO_EVENT_MOUSE_AXES
#endif

			Input *i = getInput();
#if defined ALLEGRO_IPHONE || defined ALLEGRO_MACOSX
			bool jp_conn = joypad_connected();
#else
			bool jp_conn = false;
#endif
			if (!jp_conn && i && i->isPlayerControlled() && 
			(event.type == BEGIN ||
			event.type == END ||
			(event.type == MOVE && !path_head))) {

				al_lock_mutex(input_mutex);

#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
				int this_x = event.touch.x;
				int this_y = event.touch.y;
				int touch_id = event.touch.id;
#else
				int this_x = event.mouse.x;
				int this_y = event.mouse.y;
				int touch_id = 1;
				#endif
				
				event_mouse_x = this_x;
				event_mouse_y = this_y;
				
				if (is_ipad() && !config.getMaintainAspectRatio())
					tguiConvertMousePosition(&this_x, &this_y, 0, 0, screen_ratio_x, screen_ratio_y);
				else
					tguiConvertMousePosition(&this_x, &this_y, screen_offset_x, screen_offset_y, 1, 1);
					
				void (*down[7])(void) = {
					joy_l_down, joy_r_down, joy_u_down, joy_d_down,
					joy_b1_down, joy_b2_down, joy_b3_down
				};
				void (*up[7])(void) = {
					joy_l_up, joy_r_up, joy_u_up, joy_d_up,
					joy_b1_up, joy_b2_up, joy_b3_up
				};

				bool state[7] = { false, };
				
				int type;
				if (event.type == BEGIN)
					type = MOUSE_DOWN;
				else if (event.type == END)
					type = MOUSE_UP;
				else
					type = MOUSE_AXES;
				
				al_lock_mutex(dpad_mutex);
					
				bool _l = false, _r = false, _u = false, _d = false, _b1 = false, _b2 = false, _b3 = false;
				for (int i = 0; i < curr_touches; i++) {
					bool __l = false, __r = false, __u = false, __d = false, __b1 = false, __b2 = false, __b3 = false;
					get_inputs(touches[i].x, touches[i].y,
						&__l, &__r, &__u, &__d, &__b1, &__b2, &__b3
					);
					_l = _l || __l;
					_r = _r || __r;
					_u = _u || __u;
					_d = _d || __d;
					_b1 = _b1 || __b1;
					_b2 = _b2 || __b2;
					_b3 = _b3 || __b3;
				}
				
				process_touch(this_x, this_y, touch_id, type);
				
				bool l = false, r = false, u = false, d = false, b1 = false, b2 = false, b3 = false;
				for (int i = 0; i < curr_touches; i++) {
					bool __l = false, __r = false, __u = false, __d = false, __b1 = false, __b2 = false, __b3 = false;
					get_inputs(touches[i].x, touches[i].y,
						&__l, &__r, &__u, &__d, &__b1, &__b2, &__b3
					);
					l = l || __l;
					r = r || __r;
					u = u || __u;
					d = d || __d;
					b1 = b1 || __b1;
					b2 = b2 || __b2;
					b3 = b3 || __b3;
				}
				
				al_unlock_mutex(dpad_mutex);
				
				bool on1[7] = { _l, _r, _u, _d, _b1, _b2, _b3 };
				bool on2[7] = { l, r, u, d, b1, b2, b3 };
				
				if (use_dpad && !have_mouse) {
					for (int i = 0; i < 7; i++) {
						if (on1[i] == false && on2[i] == true) {
							(*(down[i]))();
						}
						else if (on1[i] == true && on2[i] == false) {
							(*(up[i]))();
						}
						
						state[i] = on2[i];
					}
				}
					
				getInput()->set(state[0], state[1], state[2], state[3],
					state[4], state[5], state[6], true, false);
								
				if (event.type == BEGIN) {
					bool hot_corner_touched = false;
					bool player_in_corner = false;
					if (area) {
						Object *o = area->findObject(0);
						if (o && o->isMoving()) {
							if ((area->getFocusX()-area->getOriginX() < (TILE_SIZE*3)/2) && (area->getFocusY()-area->getOriginY() < (TILE_SIZE*3)/2)) {
								player_in_corner = true;
							}
						}
					}
					bool roam = (tguiCurrentTimeMillis() - roaming) < 500;
					if (!roam || !player_in_corner) {
						/* Hot top left corner */
						if (global_draw_red || red_off_press_on) {
							if (this_x < 16 && this_y < 16) {
								hot_corner_touched = true;
								if (al_current_time() > next_shake) {
									iphone_shake_time = al_current_time();
									next_shake = al_current_time()+0.5;
								}
							}
						}
					}
					if (use_dpad) {
						al_lock_mutex(dpad_mutex);
						if (!zone_defined(this_x, this_y)) {
							if (this_y < BH/3) {
								dpad_at_top = true;
							}
							else if (this_y > (BH*2)/3) {
								dpad_at_top = false;
							}
						}
						al_unlock_mutex(dpad_mutex);
					}
					if (!hot_corner_touched) {
						released = false;
						al_lock_mutex(click_mutex);
						click_x = this_x;
						click_y = this_y;
						al_unlock_mutex(click_mutex);
					}
				}
				else if (event.type == END) {
					al_lock_mutex(dpad_mutex);
					if (!(this_x < 16 && this_y < 16)) {
						released = true;
						if (have_mouse || !use_dpad) {
							total_mouse_x = 0;
							total_mouse_y = 0;
							last_mouse_x = -1;
						}
					}
					al_unlock_mutex(dpad_mutex);
				}
				else if (event.type == MOVE) {
					al_lock_mutex(click_mutex);
					current_mouse_x = this_x;
					current_mouse_y = this_y;
					al_unlock_mutex(click_mutex);
					al_lock_mutex(dpad_mutex);
					if ((have_mouse && !released) || !use_dpad) {
						if (last_mouse_x < 0) {
							last_mouse_x = this_x;
							last_mouse_y = this_y;
						}
						else {
							int dx = this_x - last_mouse_x;
							int dy = this_y - last_mouse_y;
							last_mouse_x = this_x;
							last_mouse_y = this_y;
							total_mouse_x += dx;
							total_mouse_y += dy;
							if (total_mouse_x < (-IPHONE_LINE_MIN)) {
								total_mouse_x = 0;
								total_mouse_y = 0;
								iphone_line_times[IPHONE_LINE_DIR_WEST] = al_current_time();
							}
							else if (total_mouse_x > IPHONE_LINE_MIN) {
								total_mouse_x = 0;
								total_mouse_y = 0;
								iphone_line_times[IPHONE_LINE_DIR_EAST] = al_current_time();
							}
							if (total_mouse_y < (-IPHONE_LINE_MIN)) {
								total_mouse_x = 0;
								total_mouse_y = 0;
								iphone_line_times[IPHONE_LINE_DIR_NORTH] = al_current_time();
							}
							else if (total_mouse_y > IPHONE_LINE_MIN) {
								total_mouse_x = 0;
								total_mouse_y = 0;
								iphone_line_times[IPHONE_LINE_DIR_SOUTH] = al_current_time();
							}
						}
					}
					
					al_unlock_mutex(dpad_mutex);
				}
				
				al_unlock_mutex(input_mutex);
			}
		}
	}
	
	al_unregister_event_source(events, (ALLEGRO_EVENT_SOURCE *)draw_timer);
	al_unregister_event_source(events, (ALLEGRO_EVENT_SOURCE *)logic_timer);
#if defined A5_D3D || defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
	al_unregister_event_source(events, (ALLEGRO_EVENT_SOURCE *)display);
#endif
	
	al_destroy_timer(draw_timer);
	al_destroy_timer(logic_timer);
	
	al_destroy_event_queue(events);
	
	exit_event_thread = 2;

	return NULL;
}

#if defined ALLEGOR_IPHONE || defined ALLEGRO_ANDROID
static float backup_music_volume = 1.0f;
static float backup_ambience_volume = 1.0f;
#endif

static void *thread_proc_minor(void *arg)
{
#if defined ALLEGRO_IPHONE || defined ALLEGRO_MACOSX
	POOL_BEGIN
#endif

#define deltaX (fabs(last_x - curr_x))
#define deltaY (fabs(last_y - curr_y))
#define deltaZ (fabs(last_z - curr_z	))

#define SHAKEN(threshold) \
  ((deltaX > threshold && deltaY > threshold) || \
	(deltaX > threshold && deltaZ > threshold) || \
	(deltaY > threshold && deltaZ > threshold))
	
	events_minor = al_create_event_queue();

	ALLEGRO_EVENT event;

#ifdef ALLEGRO_IPHONE
	double next_shake = al_current_time();
#endif

	al_register_event_source(events_minor, (ALLEGRO_EVENT_SOURCE *)display);

	while  (exit_minor_event_thread != 1) {
#if defined ALLEGRO_IPHONE || defined ALLEGRO_MACOSX
		POOL_BEGIN
#endif
		if (al_wait_for_event_timed(events_minor, &event, 1.0f/LOGIC_RATE)) {
#ifdef ALLEGRO_IPHONE
			if (event.type == ALLEGRO_EVENT_DISPLAY_CONNECTED) {
				create_airplay_mirror = true;
			}
			else if (event.type == ALLEGRO_EVENT_DISPLAY_DISCONNECTED) {
				delete_airplay_mirror = true;
			}
#endif

			if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
#ifdef ALLEGRO_IPHONE
				if (!sound_was_playing_at_program_start)
					iPodStop();
#endif
				close_pressed = true;
#ifdef ALLEGRO_IPHONE
				break;
#endif
			}
			if (event.type == ALLEGRO_EVENT_DISPLAY_HALT_DRAWING) {
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
#if defined ALLEGRO_IPHONE
				if (!isMultitaskingSupported()) {
					if (!sound_was_playing_at_program_start)
						iPodStop();
					close_pressed = true;
					break;
				}
#endif
				do_pause_game = should_pause_game();
				do_close(false);
				switched_out = true;
				close_pressed = true;
#endif
			}
			if (event.type == ALLEGRO_EVENT_DISPLAY_RESUME_DRAWING) {
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
				switched_out = false;
				close_pressed = false;
				al_signal_cond(switch_cond);
#endif
			}
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
			if (event.type == ALLEGRO_EVENT_DISPLAY_SWITCH_OUT)
			{
				do_pause_game = should_pause_game();
				if (do_pause_game || in_pause) {
					backup_music_volume = 0.5;
					backup_ambience_volume = 0.5;
				}
				else {
					backup_music_volume = getMusicVolume();
					backup_ambience_volume = getAmbienceVolume();
				}
				setMusicVolume(0.0);
				setAmbienceVolume(0.0);
			}
			else if (event.type == ALLEGRO_EVENT_DISPLAY_SWITCH_IN)
			{
				setMusicVolume(backup_music_volume);
				setAmbienceVolume(backup_ambience_volume);
			}
#endif
			if (event.type == ALLEGRO_EVENT_DISPLAY_LOST) {
				destroy_loaded_bitmaps = true;
			}
			else if (event.type == ALLEGRO_EVENT_DISPLAY_FOUND) {
				reload_loaded_bitmaps = true;
			}
		}
#ifdef ALLEGRO_IPHONE
		double shake = al_iphone_get_last_shake_time();
		if (shake > allegro_iphone_shaken) {
			allegro_iphone_shaken = shake;
			if (config.getShakeAction() == CFG_SHAKE_CHANGE_SONG) {
				iPodNext();
			}
			else if (al_current_time() > next_shake) {
				iphone_shake_time = al_current_time();
				next_shake = al_current_time()+0.5;
			}
		}
#endif
#if defined ALLEGRO_IPHONE || defined ALLEGRO_MACOSX
		POOL_END
#endif
	}

	al_unregister_event_source(events_minor, (ALLEGRO_EVENT_SOURCE *)display);

	al_destroy_event_queue(events_minor);
	
	exit_minor_event_thread = 2;

#if defined ALLEGRO_IPHONE || defined ALLEGRO_MACOSX
	POOL_END
#endif

	return NULL;
}

static void *loader_proc(void *arg)
{
#if defined ALLEGRO_IPHONE || defined ALLEGRO_MACOSX
	POOL_BEGIN
#endif

#ifdef A5_OGL
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
	al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_RGBA_4444);
#else
	al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_ABGR_8888_LE);
#endif
#else
	al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_ARGB_8888);
#endif

	//(void)arg;
	//m_set_target_bitmap(al_get_backbuffer(display));
	show_progress(8);

	MBITMAP *deter_display_access_bmp;
	int flags = al_get_new_bitmap_flags();
	al_set_new_bitmap_flags(flags | ALLEGRO_MEMORY_BITMAP);
	deter_display_access_bmp = m_create_bitmap(16, 16); // check
	al_set_new_display_flags(flags);
	m_set_target_bitmap(deter_display_access_bmp);

	guiAnims = new AnimationSet(getResource("gui.png"));
	if (!guiAnims) {
		if (!native_error("Failed to load gui"))
			return NULL;
	}
	debug_message("GUI graphics loaded.\n");
	

	cursor = m_load_bitmap(getResource("media/cursor.png"));
	if (!cursor) {
		return NULL;
	}

	//orb_bmp = m_load_alpha_bitmap(getResource("media/orb_bmp.png"), true);
	poison_bmp = m_load_alpha_bitmap(getResource("media/poison.png"), true);
	poison_bmp_tmp = m_create_alpha_bitmap( // check
		m_get_bitmap_width(poison_bmp)+10,
		m_get_bitmap_height(poison_bmp)+10);
	poison_bmp_tmp2 = m_create_alpha_bitmap( // check
		m_get_bitmap_width(poison_bmp)+10,
		m_get_bitmap_height(poison_bmp)+10);

	for (int i = 0; i < 16; i++) {
		triangle_lines[i*2].x = 0;
		triangle_lines[i*2].y = i;
		triangle_lines[i*2].z = 0;
		triangle_lines[i*2].color =
			al_map_rgb(255, 0, 0);
		triangle_lines[i*2+1].x = 16-i;
		triangle_lines[i*2+1].y = i;
		triangle_lines[i*2+1].z = 0;
		triangle_lines[i*2+1].color =
			al_map_rgb(255, 0, 0);
	}

	debug_message("Input initialized.\n");
	initInput();

	show_progress(16);

	gfx_mode_set = true;

	al_inhibit_screensaver(true);

	debug_message("Tilemap loaded\n");

	tile = m_create_bitmap(TILE_SIZE, TILE_SIZE); // check

	if (!tile) {
		if (!native_error("Failed to create tile"))
			return NULL;
	}

	// Set an icon
#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_MACOSX && !defined ALLEGRO_ANDROID
	MBITMAP *tmp_bmp = m_load_alpha_bitmap(getResource("staff.png"));
	al_set_display_icon(display, tmp_bmp->bitmap);
	m_destroy_bitmap(tmp_bmp);
#endif

	show_progress(24);

	debug_message("TGUI initialized.\n");


	initSound();

#ifdef ALLEGRO_IPHONE
	if (iPodIsPlaying()) {
		config.setMusicVolume(0);
		setMusicVolume(1);
	}
#endif

#ifdef ALLEGRO_IPHONE
	sound_was_playing_at_program_start = iPodIsPlaying();
#endif

	debug_message("Sound initialized\n");


	show_progress(40);

	// load terrain file

	terrain = new XMLData(getResource("terrain.xml"));

	debug_message("terrain data loaded\n");

	// load "Water" info (tiles that cover the player partially)

	XMLData *water_data = new XMLData(getResource("water.xml"));
	std::list<XMLData *> &nodes = water_data->getNodes();
	std::list<XMLData *>::iterator it;
	for (it = nodes.begin(); it != nodes.end(); it++) {
		XMLData *node = *it;
		std::string name = node->getName();
		XMLData *nodeDepth = node->find("depth");
		WaterData w;
		w.depth = atoi(nodeDepth->getValue().c_str());
		w.tilenum = atoi(name.c_str());
		waterData.push_back(w);
	}
	delete water_data;

	show_progress(48);
	debug_message("water data loaded\n");

	for (int i = 0; i < MAX_INVENTORY; i++) {
		inventory[i].index = -1;
		inventory[i].quantity = 0;
	}

	debug_message("inventory initialized\n");

	/*
	profileBg = m_load_bitmap(getResource("media/profile_bg.png"));
	if (!profileBg) {
		return NULL;
	}
	*/

	debug_message("Loading icons\n");

	// FIXME: return value
	loadIcons();

	show_progress(56);

	debug_message("Loading weapon animations\n");

	// FIXME: return value
	//initWeaponAnimations();


	show_progress(64);

	debug_message("Loading miscellaneous graphics\n");

	stoneTexture = m_load_bitmap(getResource("combat_media/stone.png"));
	
	mushroom = m_load_bitmap(getResource("combat_media/mushroom.png"));
	webbed = m_load_bitmap(getResource("combat_media/webbed.png"));


	show_progress(72);

	wait_cond = al_create_cond();
	wait_mutex = al_create_mutex();
	al_lock_mutex(wait_mutex);
	
	switch_cond = al_create_cond();
	switch_mutex = al_create_mutex();
	al_lock_mutex(switch_mutex);
	
	joypad_mutex = al_create_mutex_recursive();
	
	show_progress(100);


	al_run_detached_thread(thread_proc, NULL);
	m_rest(0.5);

	if (safemode) {
		notify("Desired mode could not be", "set. Using default", "(safe) mode...");
	}
	
#if defined ALLEGRO_IPHONE || defined ALLEGRO_MACOSX
	POOL_END
#endif

	loading_done = true;
	return NULL;
}

void destroyTilemap(void)
{
	if (tilemap)
		m_destroy_bitmap(tilemap);
	if (tileTransparent)
		delete[] tileTransparent;

	tilemap = 0;
	tileTransparent = 0;

	if (tilemap_data)
		delete tilemap_data;

	if (trans_data)
		delete trans_data;

	tilemap_data = 0;
	trans_data = 0;
}


bool loadTilemap(void)
{
	destroyTilemap();

	/*
	tilemap = m_load_alpha_bitmap(getResource("packed.png"));

	if (!native_error("Failed to load tilemap"))
			return false;
	}
	*/

	numTiles = (1024/16) * (1024/16);//(m_get_bitmap_width(tilemap)/TILE_SIZE) *
		//(m_get_bitmap_height(tilemap)/TILE_SIZE);

	//findAlphaTiles();

	tilemap_data = new XMLData(getResource("tilemap.png.xml"));
	trans_data = new XMLData(getResource("trans.xml"));

	return true;
}

void init_controller_shader(void)
{
	static const char *main_vertex_source =
	"attribute vec4 pos;\n"
	"attribute vec4 color;\n"
	"attribute vec2 texcoord;\n"
	"uniform mat4 projview_matrix;\n"
	"varying vec4 varying_color;\n"
	"varying vec2 varying_texcoord;\n"
	"void main()\n"
	"{\n"
	"   varying_color = color;\n"
	"   varying_texcoord = texcoord;\n"
#ifndef __linux__
	"   gl_PointSize = 1.0;\n"
#endif
	"   gl_Position = projview_matrix * pos;\n"
	"}\n";

	static const char *main_pixel_source =
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
	"precision mediump float;\n"
#endif
	"uniform bool use_tex;\n"
	"uniform sampler2D tex;\n"
	"uniform bool use_tex_matrix;\n"
	"uniform mat4 tex_matrix;\n"
	"varying lowp vec4 varying_color;\n"
	"varying vec2 varying_texcoord;\n"
	"void main()\n"
	"{\n"
	"  lowp vec4 tmp = varying_color;\n"
	"  vec4 coord = vec4(varying_texcoord, 0.0, 1.0);\n"
	"  if (use_tex_matrix) {\n"
	"     coord = coord * tex_matrix;\n"
	"     lowp vec4 sample = texture2D(tex, coord.st);\n"
	"     tmp *= sample;\n"
	"  }\n"
	"  else if (use_tex) {\n"
	"     lowp vec4 sample = texture2D(tex, coord.st);\n"
	"     tmp *= sample;\n"
	"  }\n"
	"  gl_FragColor = tmp;\n"
	"}\n";
	
	controller_shader = al_create_shader(ALLEGRO_SHADER_GLSL);

	al_attach_shader_source(
				controller_shader,
				ALLEGRO_VERTEX_SHADER,
				main_vertex_source
				);
	
	al_attach_shader_source(
				controller_shader,
				ALLEGRO_PIXEL_SHADER,
				main_pixel_source
				);

	al_link_shader(controller_shader);
	
	al_set_opengl_program_object(controller_display, al_get_opengl_program_object(controller_shader));

}

void destroy_controller_shader(void)
{
	al_destroy_shader(controller_shader);
}

#ifndef A5_D3D
void init_shaders(void)
{
#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_ANDROID
#define LOWP ""
#else
#define LOWP "lowp"
#endif	
	
#if !defined NO_SHADERS
	if (use_programmable_pipeline) {
		
#ifdef A5_OGL
		static const char *main_vertex_source =
		"attribute vec4 pos;\n"
		"attribute vec4 color;\n"
		"attribute vec2 texcoord;\n"
		"uniform mat4 projview_matrix;\n"
		"varying vec4 varying_color;\n"
		"varying vec2 varying_texcoord;\n"
		"void main()\n"
		"{\n"
		"   varying_color = color;\n"
		"   varying_texcoord = texcoord;\n"
#ifndef __linux__
		"   gl_PointSize = 1.0;\n"
#endif
		"   gl_Position = projview_matrix * pos;\n"
		"}\n";
		
		static const char *warp_vertex_source =
		"attribute vec4 pos;\n"
		"attribute vec2 texcoord;\n"
		"uniform mat4 projview_matrix;\n"
		"varying vec2 varying_texcoord;\n"
		"void main()\n"
		"{\n"
		"   varying_texcoord = texcoord;\n"
#ifndef __linux__
		"   gl_PointSize = 1.0;\n"
#endif
		"   gl_Position = projview_matrix * pos;\n"
		"}\n";
		
		static const char *default_vertex_source =
		"attribute vec4 pos;\n"
		"attribute vec4 color;\n"
		"attribute vec2 texcoord;\n"
		"uniform mat4 projview_matrix;\n"
		"varying vec4 varying_color;\n"
		"varying vec2 varying_texcoord;\n"
		"void main()\n"
		"{\n"
		"   varying_color = color;\n"
		"   varying_texcoord = texcoord;\n"
#ifndef __linux__
		"   gl_PointSize = 1.0;\n"
#endif
		"   gl_Position = projview_matrix * pos;\n"
		"}\n";
		
#ifdef ALLEGRO_IPHONE
		static const char *scale2x_vertex_source =
		"attribute vec4 pos;\n"
		"attribute vec2 texcoord;\n"
		"uniform mat4 projview_matrix;\n"
		"uniform float hstep;\n"
		"uniform float vstep;\n"
		"varying vec2 varying_texcoord;\n"
		"varying vec2 tB;\n"
		"varying vec2 tD;\n"
		"varying vec2 tF;\n"
		"varying vec2 tH;\n"
		"void main()\n"
		"{\n"
		"   varying_texcoord = texcoord;\n"
		"   tB = vec2(texcoord.s,       texcoord.t+vstep);\n"
		"   tD = vec2(texcoord.s-hstep, texcoord.t);\n"
		"   tF = vec2(texcoord.s+hstep, texcoord.t);\n"
		"   tH = vec2(texcoord.s,       texcoord.t-vstep);\n"
#ifndef __linux__
		"   gl_PointSize = 1.0;\n"
#endif
		"   gl_Position = projview_matrix * pos;\n"
		"}\n";
		
		static const char *scale3x_vertex_source =
		"attribute vec4 pos;\n"
		"attribute vec2 texcoord;\n"
		"uniform mat4 projview_matrix;\n"
		"uniform float hstep;\n"
		"uniform float vstep;\n"
		"varying vec2 varying_texcoord;\n"
		"varying vec2 tA;\n"
		"varying vec2 tB;\n"
		"varying vec2 tC;\n"
		"varying vec2 tD;\n"
		"varying vec2 tF;\n"
		"varying vec2 tG;\n"
		"varying vec2 tH;\n"
		"varying vec2 tI;\n"
		"void main()\n"
		"{\n"
		"	tA = vec2(texcoord.s-hstep, texcoord.t+vstep);\n"
		"   tB = vec2(texcoord.s,       texcoord.t+vstep);\n"
		"	tC = vec2(texcoord.s+hstep, texcoord.t+vstep);\n"
		"   tD = vec2(texcoord.s-hstep, texcoord.t      );\n"
		"   tF = vec2(texcoord.s+hstep, texcoord.t      );\n"
		"	tG = vec2(texcoord.s-hstep, texcoord.t-vstep);\n"
		"   tH = vec2(texcoord.s,       texcoord.t-vstep);\n"
		"	tI = vec2(texcoord.s+hstep, texcoord.t-vstep);\n"
		"   varying_texcoord = texcoord;\n"
#ifndef __linux__
		"   gl_PointSize = 1.0;\n"
#endif
		"   gl_Position = projview_matrix * pos;\n"
		"}\n";
#endif
		
		static const char *main_pixel_source =
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
		"precision mediump float;\n"
#endif
		"uniform bool use_tex;\n"
		"uniform sampler2D tex;\n"
		"uniform bool use_tex_matrix;\n"
		"uniform mat4 tex_matrix;\n"
		"varying " LOWP " vec4 varying_color;\n"
		"varying vec2 varying_texcoord;\n"
		"void main()\n"
		"{\n"
		"  " LOWP " vec4 tmp = varying_color;\n"
		"  vec4 coord = vec4(varying_texcoord, 0.0, 1.0);\n"
		"  if (use_tex_matrix) {\n"
		"     coord = coord * tex_matrix;\n"
		"     " LOWP " vec4 sample = texture2D(tex, coord.st);\n"
		"     tmp *= sample;\n"
		"  }\n"
		"  else if (use_tex) {\n"
		"     " LOWP " vec4 sample = texture2D(tex, coord.st);\n"
		"     tmp *= sample;\n"
		"  }\n"
		"  gl_FragColor = tmp;\n"
		"}\n";
		
		static const char *cheap_pixel_source =
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
		"precision mediump float;\n"
#endif
		"uniform sampler2D tex;\n"
		"varying " LOWP " vec4 varying_color;\n"
		"varying vec2 varying_texcoord;\n"
		"void main()\n"
		"{\n"
		"  gl_FragColor = texture2D(tex, varying_texcoord) * varying_color;\n"
		"}\n";
		
		const char *tinter_pixel_source =
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
		"precision mediump float;\n"
#endif
		"uniform bool use_tex;\n"
		"uniform sampler2D tex;\n"
		"uniform bool use_tex_matrix;\n"
		"uniform mat4 tex_matrix;\n"
		"varying vec4 varying_color;\n"
		"varying vec2 varying_texcoord;\n"
		"uniform float r;\n"
		"uniform float g;\n"
		"uniform float b;\n"
		"uniform float ratio;\n"
		"void main() {\n"
		"   float avg, dr, dg, db;\n"
		"   vec4 color = varying_color;\n"
		"   vec4 coord = vec4(varying_texcoord, 0.0, 1.0);\n"
		"   if (use_tex_matrix) {\n"
		"      coord = coord * tex_matrix;\n"
		"   }\n"
		"   color *= texture2D(tex, coord.st);\n"
		"   avg = (color.r + color.g + color.b) / 3.0;\n"
		"   dr = avg * r;\n"
		"   dg = avg * g;\n"
		"   db = avg * b;\n"
		"   color.r = color.r - (ratio * (color.r - dr));\n"
		"   color.g = color.g - (ratio * (color.g - dg));\n"
		"   color.b = color.b - (ratio * (color.b - db));\n"
		"   gl_FragColor = color;\n"
		"}";
		
		const char *warp2_pixel_source =
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
		"precision mediump float;\n"
#endif
		"uniform sampler2D tex;\n"
		"varying vec2 varying_texcoord;\n"
		"uniform float angle;\n"
		"uniform float tex_bot;\n"
		"void main() {\n"
		"   float div;\n"
		"   div = angle / (3.14159*2.0);\n"
		"   if (div > 0.5)\n"
		"      div = 1.0 - div;\n"
		"   div = (0.5 - div) * 25.0 + 5.0;\n"
		"   float o = (sin((angle+(varying_texcoord.x-0.5))*4.0) / div) + varying_texcoord.y;\n"
		"   if (o < 0.0 || o > tex_bot)\n"
		"      gl_FragColor = vec4(0, 0, 0, 1);\n"
		"   else\n"
		"      gl_FragColor = texture2D(tex, vec2(varying_texcoord.x, o));\n"
		"}\n";
		
		const char *shadow_pixel_source =
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
		"precision mediump float;\n"
#endif
		"varying vec4 varying_color;\n"
		"varying vec2 varying_texcoord;\n"
		"uniform float x1;\n"
		"uniform float y1;\n"
		"uniform float x2;\n"
		"uniform float y2;\n"
		"uniform float BW;\n"
		"uniform float BH;\n"
		"void main() {\n"
		// rx, ry is the closest point to gl_FragCoord on the rectangle
		"   float rx, ry;\n"
		"   float dx, dy;\n"
		"   float frag_y = (160.0-1.0) - gl_FragCoord.y;\n"
		"   rx = clamp(gl_FragCoord.x, x1, x2);\n"
		"   ry = clamp(frag_y, y1, y2);\n"
		"   dx = gl_FragCoord.x - rx;\n"
		"   dy = frag_y - ry;\n"
		"   float dist = clamp(sqrt(dx*dx + dy*dy), 0.0, 10.0);\n"
		"   float alpha = 1.0 - (dist / 10.0);\n"
		"   gl_FragColor = vec4(0.0, 0.0, 0.0, alpha);\n"
		"}\n";
		
		const char *brighten_pixel_source =
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
		"precision mediump float;\n"
#endif
		"uniform sampler2D tex;\n"
		"varying vec4 varying_color;\n"
		"varying vec2 varying_texcoord;\n"
		"uniform float brightness;\n"
		"uniform bool use_tex;\n"
		"uniform bool use_tex_matrix;\n"
		"uniform mat4 tex_matrix;\n"
		"void main() {\n"
		"   vec4 coord = vec4(varying_texcoord, 0.0, 1.0);\n"
		"   if (use_tex_matrix) {\n"
		"      coord = coord * tex_matrix;\n"
		"   }\n"
		"   vec4 color = varying_color;\n"
		"   if (use_tex) {\n"
		"      color *= texture2D(tex, coord.st);\n"
		"   }\n"
		"   if (color.a == 0.0) {\n"
		"      gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);\n"
		"   }\n"
		"   else {\n"
		"      float src_factor = 1.0 - brightness;\n"
		"      color.r *= src_factor;\n"
		"      color.r = color.r + brightness;\n"
		"      color.g *= src_factor;\n"
		"      color.g = color.g + brightness;\n"
		"      color.b *= src_factor;\n"
		"      color.b = color.b + brightness;\n"
		"      gl_FragColor = color;\n"
		"   }\n"
		"}";
		
#ifdef ALLEGRO_IPHONE
		const char *scale2x_pixel_source =
		"precision mediump float;\n"
		"uniform sampler2D tex;\n"
		"varying vec2 varying_texcoord;\n"
		"uniform " LOWP " float offset_x;\n"
		"uniform " LOWP " float offset_y;\n"
		"varying vec2 tB;\n"
		"varying vec2 tD;\n"
		"varying vec2 tF;\n"
		"varying vec2 tH;\n"
		"void main() {\n"
		"	" LOWP " vec4 B = texture2D(tex, tB);\n"
		"	" LOWP " vec4 E = texture2D(tex, varying_texcoord);\n"
		"	" LOWP " vec4 H = texture2D(tex, tH);\n"
		"	" LOWP " vec4 D = texture2D(tex, tD);\n"
		"	" LOWP " vec4 F = texture2D(tex, tF);\n"
		"	if (B != H && D != F) {\n"
		"		" LOWP " float y = mod(gl_FragCoord.t-offset_x, 2.0);\n"
		"		" LOWP " float x = mod(gl_FragCoord.s-offset_y, 2.0);\n"
		"		if (x < 1.0 && y < 1.0) {\n"
		"			gl_FragColor = D == B ? D : E;\n"
		"		}\n"
		"		else if (x < 1.0 && y >= 1.0) {\n"
		"			gl_FragColor = D == H ? D : E;\n"
		"		}\n"
		"		else if (x >= 1.0 && y < 1.0) {\n"
		"			gl_FragColor = B == F ? F : E;\n"
		"		}\n"
		"		else{\n"
		"			gl_FragColor = H == F ? F : E;\n"
		"		}\n"
		/*
		 "		" LOWP " float x = mod(gl_FragCoord.t-offset_y, 2.0);\n"
		 "		" LOWP " float y = mod(gl_FragCoord.s-offset_x, 2.0);\n"
		 "		if (x < 1.0 && y < 1.0) {\n"
		 "			gl_FragColor = B == F ? F : E;\n"
		 "		}\n"
		 "		else if (x < 1.0 && y >= 1.0) {\n"
		 "			gl_FragColor = H == F ? F : E;\n"
		 "		}\n"
		 "		else if (x >= 1.0 && y < 1.0) {\n"
		 "			gl_FragColor = D == B ? D : E;\n"
		 "		}\n"
		 "		else{\n"
		 "			gl_FragColor = D == H ? D : E;\n"
		 "		}\n"
		 */
		"	}\n"
		"	else {\n"
		"		gl_FragColor = E;\n"
		"	}\n"
		"}\n";
		
		const char *scale2x_flipped_pixel_source =
		"precision mediump float;\n"
		"uniform sampler2D tex;\n"
		"varying vec2 varying_texcoord;\n"
		"uniform " LOWP " float offset_x;\n"
		"uniform " LOWP " float offset_y;\n"
		"varying vec2 tB;\n"
		"varying vec2 tD;\n"
		"varying vec2 tF;\n"
		"varying vec2 tH;\n"
		"void main() {\n"
		"	" LOWP " vec4 H = texture2D(tex, tB);\n"
		"	" LOWP " vec4 E = texture2D(tex, varying_texcoord);\n"
		"	" LOWP " vec4 B = texture2D(tex, tH);\n"
		"	" LOWP " vec4 F = texture2D(tex, tD);\n"
		"	" LOWP " vec4 D = texture2D(tex, tF);\n"
		"	if (B != H && D != F) {\n"
		"		" LOWP " float x = mod(gl_FragCoord.t-offset_y, 2.0);\n"
		"		" LOWP " float y = mod(gl_FragCoord.s-offset_x, 2.0);\n"
		"		if (x < 1.0 && y < 1.0) {\n"
		"			gl_FragColor = B == F ? F : E;\n"
		"		}\n"
		"		else if (x < 1.0 && y >= 1.0) {\n"
		"			gl_FragColor = H == F ? F : E;\n"
		"		}\n"
		"		else if (x >= 1.0 && y < 1.0) {\n"
		"			gl_FragColor = D == B ? D : E;\n"
		"		}\n"
		"		else{\n"
		"			gl_FragColor = D == H ? D : E;\n"
		"		}\n"
		"	}\n"
		"	else {\n"
		"		gl_FragColor = E;\n"
		"	}\n"
		"}\n";
		
		const char *scale3x_pixel_source =
		"precision mediump float;\n"
		"uniform sampler2D tex;\n"
		"uniform float offset_x;\n"
		"uniform float offset_y;\n"
		"varying vec2 varying_texcoord;\n"
		"varying vec2 tA;\n"
		"varying vec2 tB;\n"
		"varying vec2 tC;\n"
		"varying vec2 tD;\n"
		"varying vec2 tF;\n"
		"varying vec2 tG;\n"
		"varying vec2 tH;\n"
		"varying vec2 tI;\n"
		"void main() {\n"
		"	vec4 A = texture2D(tex, tA);\n"
		"	vec4 B = texture2D(tex, tB);\n"
		"	vec4 C = texture2D(tex, tC);\n"
		"	vec4 D = texture2D(tex, tD);\n"
		"	vec4 E = texture2D(tex, varying_texcoord);\n"
		"	vec4 F = texture2D(tex, tF);\n"
		"	vec4 G = texture2D(tex, tG);\n"
		"	vec4 H = texture2D(tex, tH);\n"
		"	vec4 I = texture2D(tex, tI);\n"
		"	if (B != H && D != F) {\n"
		"		float x = mod(gl_FragCoord.t-offset_y, 3.0);\n"
		"		float y = mod(gl_FragCoord.s-offset_x, 3.0);\n"
		"		if (y < 1.0) {\n"
		"			if (x < 1.0) {\n"
		"				gl_FragColor = H == F ? F : E;\n"
		"			}\n"
		"			else if (x < 2.0) {\n"
		"				gl_FragColor = (D == H && E != I) || (H == F && E != G) ? H : E;\n"
		"			}\n"
		"			else {\n"
		"				gl_FragColor = D == H ? D : E;\n"
		"			}\n"
		"		}\n"
		"		else if (y < 2.0) {\n"
		"			if (x < 1.0) {\n"
		"				gl_FragColor = (B == F && E != I) || (H == F && E != C) ? F : E;\n"
		"			}\n"
		"			else if (x < 2.0) {\n"
		"				gl_FragColor = E;\n"
		"			}\n"
		"			else {\n"
		"				gl_FragColor = (D == B && E != G) || (D == H && E != A) ? D : E;\n"
		"			}\n"
		"		}\n"
		"		else {\n"
		"			if (x < 1.0) {\n"
		"				gl_FragColor = B == F ? F : E;\n"
		"			}\n"
		"			else if (x < 2.0) {\n"
		"				gl_FragColor = (D == B && E != C) || (B == F && E != A) ? B : E;\n"
		"			}\n"
		"			else {\n"
		"				gl_FragColor = D == B ? D : E;\n"
		"			}\n"
		"		}\n"
		"	}\n"
		"	else {\n"
		"		gl_FragColor = E;\n"
		"	}\n"
		"}\n";
		
		const char *scale3x_flipped_pixel_source =
		"precision mediump float;\n"
		"uniform sampler2D tex;\n"
		"uniform float offset_x;\n"
		"uniform float offset_y;\n"
		"varying vec2 varying_texcoord;\n"
		"varying vec2 tA;\n"
		"varying vec2 tB;\n"
		"varying vec2 tC;\n"
		"varying vec2 tD;\n"
		"varying vec2 tF;\n"
		"varying vec2 tG;\n"
		"varying vec2 tH;\n"
		"varying vec2 tI;\n"
		"void main() {\n"
		"	vec4 A = texture2D(tex, tI);\n"
		"	vec4 B = texture2D(tex, tH);\n"
		"	vec4 C = texture2D(tex, tG);\n"
		"	vec4 D = texture2D(tex, tF);\n"
		"	vec4 E = texture2D(tex, varying_texcoord);\n"
		"	vec4 F = texture2D(tex, tD);\n"
		"	vec4 G = texture2D(tex, tC);\n"
		"	vec4 H = texture2D(tex, tB);\n"
		"	vec4 I = texture2D(tex, tA);\n"
		"	if (B != H && D != F) {\n"
		"		float x = mod(gl_FragCoord.t-offset_y, 3.0);\n"
		"		float y = mod(gl_FragCoord.s-offset_x, 3.0);\n"
		"		if (y < 1.0) {\n"
		"			if (x < 1.0) {\n"
		"				gl_FragColor = H == F ? F : E;\n"
		"			}\n"
		"			else if (x < 2.0) {\n"
		"				gl_FragColor = (D == H && E != I) || (H == F && E != G) ? H : E;\n"
		"			}\n"
		"			else {\n"
		"				gl_FragColor = D == H ? D : E;\n"
		"			}\n"
		"		}\n"
		"		else if (y < 2.0) {\n"
		"			if (x < 1.0) {\n"
		"				gl_FragColor = (B == F && E != I) || (H == F && E != C) ? F : E;\n"
		"			}\n"
		"			else if (x < 2.0) {\n"
		"				gl_FragColor = E;\n"
		"			}\n"
		"			else {\n"
		"				gl_FragColor = (D == B && E != G) || (D == H && E != A) ? D : E;\n"
		"			}\n"
		"		}\n"
		"		else {\n"
		"			if (x < 1.0) {\n"
		"				gl_FragColor = B == F ? F : E;\n"
		"			}\n"
		"			else if (x < 2.0) {\n"
		"				gl_FragColor = (D == B && E != C) || (B == F && E != A) ? B : E;\n"
		"			}\n"
		"			else {\n"
		"				gl_FragColor = D == B ? D : E;\n"
		"			}\n"
		"		}\n"
		"	}\n"
		"	else {\n"
		"		gl_FragColor = E;\n"
		"	}\n"
		"}\n";
		
		const char *scale2x_linear_pixel_source =
		"precision mediump float;\n"
		"uniform sampler2D tex;\n"
		"varying vec2 varying_texcoord;\n"
		"uniform float offset_x;\n"
		"uniform float offset_y;\n"
		"varying vec2 tB;\n"
		"varying vec2 tD;\n"
		"varying vec2 tF;\n"
		"varying vec2 tH;\n"
		"void main() {\n"
		"	vec4 B = texture2D(tex, tB);\n"
		"	vec4 E = texture2D(tex, varying_texcoord);\n"
		"	vec4 H = texture2D(tex, tH);\n"
		"	vec4 D = texture2D(tex, tD);\n"
		"	vec4 F = texture2D(tex, tF);\n"
		"	if (B != H && D != F) {\n"
		"		float x = mod(gl_FragCoord.s-offset_y, 2.0);\n"
		"		float y = mod(gl_FragCoord.t-offset_x, 2.0);\n"
		"		if (x >= 1.0 && y < 1.0) {\n"
		"			gl_FragColor = D == H ? D : E;\n"
		"		}\n"
		"		else if (x < 1.0 && y < 1.0) {\n"
		"			gl_FragColor = H == F ? F : E;\n"
		"		}\n"
		"		else if (x >= 1.0 && y >= 1.0) {\n"
		"			gl_FragColor = D == B ? D : E;\n"
		"		}\n"
		"		else{\n"
		"			gl_FragColor = B == F ? F : E;\n"
		"		}\n"
		"	}\n"
		"	else {\n"
		"		gl_FragColor = E;\n"
		"	}\n"
		"}\n";
		
		const char *scale2x_linear_flipped_pixel_source =
		"precision mediump float;\n"
		"uniform sampler2D tex;\n"
		"varying vec2 varying_texcoord;\n"
		"uniform float offset_x;\n"
		"uniform float offset_y;\n"
		"varying vec2 tB;\n"
		"varying vec2 tD;\n"
		"varying vec2 tF;\n"
		"varying vec2 tH;\n"
		"void main() {\n"
		"	vec4 H = texture2D(tex, tB);\n"
		"	vec4 E = texture2D(tex, varying_texcoord);\n"
		"	vec4 B = texture2D(tex, tH);\n"
		"	vec4 F = texture2D(tex, tF);\n"
		"	vec4 D = texture2D(tex, tD);\n"
		"	if (B != H && D != F) {\n"
		"		float x = mod(gl_FragCoord.s-offset_y, 2.0);\n"
		"		float y = mod(gl_FragCoord.t-offset_x, 2.0);\n"
		"		if (x >= 1.0 && y < 1.0) {\n"
		"			gl_FragColor = D == B ? D : E;\n"
		"		}\n"
		"		else if (x < 1.0 && y < 1.0) {\n"
		"			gl_FragColor = B == F ? F : E;\n"
		"		}\n"
		"		else if (x >= 1.0 && y >= 1.0) {\n"
		"			gl_FragColor = D == H ? D : E;\n"
		"		}\n"
		"		else{\n"
		"			gl_FragColor = H == F ? F : E;\n"
		"		}\n"
		"	}\n"
		"	else {\n"
		"		gl_FragColor = E;\n"
		"	}\n"
		"}\n";
		
		const char *scale3x_linear_pixel_source =
		"precision mediump float;\n"
		"uniform sampler2D tex;\n"
		"uniform float offset_x;\n"
		"uniform float offset_y;\n"
		"varying vec2 varying_texcoord;\n"
		"varying vec2 tA;\n"
		"varying vec2 tB;\n"
		"varying vec2 tC;\n"
		"varying vec2 tD;\n"
		"varying vec2 tF;\n"
		"varying vec2 tG;\n"
		"varying vec2 tH;\n"
		"varying vec2 tI;\n"
		"void main() {\n"
		"	vec4 A = texture2D(tex, tA);\n"
		"	vec4 B = texture2D(tex, tB);\n"
		"	vec4 C = texture2D(tex, tC);\n"
		"	vec4 D = texture2D(tex, tD);\n"
		"	vec4 E = texture2D(tex, varying_texcoord);\n"
		"	vec4 F = texture2D(tex, tF);\n"
		"	vec4 G = texture2D(tex, tG);\n"
		"	vec4 H = texture2D(tex, tH);\n"
		"	vec4 I = texture2D(tex, tI);\n"
		"	if (B != H && D != F) {\n"
		"		float x = mod(gl_FragCoord.s-offset_y, 3.0);\n"
		"		float y = mod(gl_FragCoord.t-offset_x, 3.0);\n"
		"		if (y < 1.0) {\n"
		"			if (x < 1.0) {\n"
		"				gl_FragColor = D == H ? D : E;\n"
		"			}\n"
		"			else if (x < 2.0) {\n"
		"				gl_FragColor = (D == H && E != I) || (H == F && E != G) ? H : E;\n"
		"			}\n"
		"			else {\n"
		"				gl_FragColor = H == F ? F : E;\n"
		"			}\n"
		"		}\n"
		"		else if (y < 2.0) {\n"
		"			if (x < 1.0) {\n"
		"				gl_FragColor = (D == B && E != G) || (D == H && E != A) ? D : E;\n"
		"			}\n"
		"			else if (x < 2.0) {\n"
		"				gl_FragColor = E;\n"
		"			}\n"
		"			else {\n"
		"				gl_FragColor = (B == F && E != I) || (H == F && E != C) ? F : E;\n"
		"			}\n"
		"		}\n"
		"		else {\n"
		"			if (x < 1.0) {\n"
		"				gl_FragColor = D == B ? D : E;\n"
		"			}\n"
		"			else if (x < 2.0) {\n"
		"				gl_FragColor = (D == B && E != C) || (B == F && E != A) ? B : E;\n"
		"			}\n"
		"			else {\n"
		"				gl_FragColor = B == F ? F : E;\n"
		"			}\n"
		"		}\n"
		"	}\n"
		"	else {\n"
		"		gl_FragColor = E;\n"
		"	}\n"
		"}\n";
		
		const char *scale3x_linear_flipped_pixel_source =
		"precision mediump float;\n"
		"uniform sampler2D tex;\n"
		"uniform float offset_x;\n"
		"uniform float offset_y;\n"
		"varying vec2 varying_texcoord;\n"
		"varying vec2 tA;\n"
		"varying vec2 tB;\n"
		"varying vec2 tC;\n"
		"varying vec2 tD;\n"
		"varying vec2 tF;\n"
		"varying vec2 tG;\n"
		"varying vec2 tH;\n"
		"varying vec2 tI;\n"
		"void main() {\n"
		"	vec4 A = texture2D(tex, tI);\n"
		"	vec4 B = texture2D(tex, tH);\n"
		"	vec4 C = texture2D(tex, tG);\n"
		"	vec4 D = texture2D(tex, tF);\n"
		"	vec4 E = texture2D(tex, varying_texcoord);\n"
		"	vec4 F = texture2D(tex, tD);\n"
		"	vec4 G = texture2D(tex, tC);\n"
		"	vec4 H = texture2D(tex, tB);\n"
		"	vec4 I = texture2D(tex, tA);\n"
		"	if (B != H && D != F) {\n"
		"		float x = mod(gl_FragCoord.s-offset_y, 3.0);\n"
		"		float y = mod(gl_FragCoord.t-offset_x, 3.0);\n"
		"		if (y < 1.0) {\n"
		"			if (x < 1.0) {\n"
		"				gl_FragColor = D == H ? D : E;\n"
		"			}\n"
		"			else if (x < 2.0) {\n"
		"				gl_FragColor = (D == H && E != I) || (H == F && E != G) ? H : E;\n"
		"			}\n"
		"			else {\n"
		"				gl_FragColor = H == F ? F : E;\n"
		"			}\n"
		"		}\n"
		"		else if (y < 2.0) {\n"
		"			if (x < 1.0) {\n"
		"				gl_FragColor = (D == B && E != G) || (D == H && E != A) ? D : E;\n"
		"			}\n"
		"			else if (x < 2.0) {\n"
		"				gl_FragColor = E;\n"
		"			}\n"
		"			else {\n"
		"				gl_FragColor = (B == F && E != I) || (H == F && E != C) ? F : E;\n"
		"			}\n"
		"		}\n"
		"		else {\n"
		"			if (x < 1.0) {\n"
		"				gl_FragColor = D == B ? D : E;\n"
		"			}\n"
		"			else if (x < 2.0) {\n"
		"				gl_FragColor = (D == B && E != C) || (B == F && E != A) ? B : E;\n"
		"			}\n"
		"			else {\n"
		"				gl_FragColor = B == F ? F : E;\n"
		"			}\n"
		"		}\n"
		"	}\n"
		"	else {\n"
		"		gl_FragColor = E;\n"
		"	}\n"
		"}\n";
#endif
#else
		static const char *main_vertex_source =
		"struct VS_INPUT\n"
		"{\n"
		"   float4 Position  : POSITION0;\n"
		"   float2 TexCoord  : TEXCOORD0;\n"
		"   float4 Color     : TEXCOORD1;\n"
		"};\n"
		"struct VS_OUTPUT\n"
		"{\n"
		"   float4 Position  : POSITION0;\n"
		"   float4 Color     : COLOR0;\n"
		"   float2 TexCoord  : TEXCOORD0;\n"
		"};\n"
		"\n"
		"float4x4 projview_matrix;\n"
		"\n"
		"VS_OUTPUT vs_main(VS_INPUT Input)\n"
		"{\n"
		"   VS_OUTPUT Output;\n"
		"   Output.Position = mul(Input.Position, projview_matrix);\n"
		"   Output.Color = Input.Color;\n"
		"   Output.TexCoord = Input.TexCoord;\n"
		"   return Output;\n"
		"}\n";
		
		static const char *warp_vertex_source =
		"struct VS_INPUT\n"
		"{\n"
		"   float4 Position  : POSITION0;\n"
		"   float2 TexCoord  : TEXCOORD0;\n"
		"};\n"
		"struct VS_OUTPUT\n"
		"{\n"
		"   float4 Position  : POSITION0;\n"
		"   float2 TexCoord  : TEXCOORD0;\n"
		"};\n"
		"\n"
		"float4x4 projview_matrix;\n"
		"\n"
		"VS_OUTPUT vs_main(VS_INPUT Input)\n"
		"{\n"
		"   VS_OUTPUT Output;\n"
		"   Output.Position = mul(Input.Position, projview_matrix);\n"
		"   Output.TexCoord = Input.TexCoord;\n"
		"   return Output;\n"
		"}\n";
		
		static const char *default_vertex_source =
		"struct VS_INPUT\n"
		"{\n"
		"   float4 Position  : POSITION0;\n"
		"   float2 TexCoord  : TEXCOORD0;\n"
		"   float4 Color     : TEXCOORD1;\n"
		"};\n"
		"struct VS_OUTPUT\n"
		"{\n"
		"   float4 Position  : POSITION0;\n"
		"   float4 Color     : COLOR0;\n"
		"   float2 TexCoord  : TEXCOORD0;\n"
		"   float4 ClipPos   : TEXCOORD1;\n"
		"};\n"
		"\n"
		"float4x4 projview_matrix;\n"
		"\n"
		"VS_OUTPUT vs_main(VS_INPUT Input)\n"
		"{\n"
		"   VS_OUTPUT Output;\n"
		"   Output.Position = mul(Input.Position, projview_matrix);\n"
		"   Output.ClipPos = Output.Position;\n"
		"   Output.Color = Input.Color;\n"
		"   Output.TexCoord = Input.TexCoord;\n"
		"   return Output;\n"
		"}\n";
		
		static const char *main_pixel_source =
		"bool use_tex;\n"
		"texture tex;\n"
		"bool use_tex_matrix;\n"
		"float4x4 tex_matrix;\n"
		"sampler2D s = sampler_state {\n"
		"   texture = <tex>;\n"
		"};\n"
		"float4 ps_main(VS_OUTPUT Input) : COLOR0\n"
		"{\n"
		"   float4 tmp = Input.Color;\n"
		"   if (use_tex) {\n"
		"      float4 coord = float4(Input.TexCoord.x, Input.TexCoord.y, 0.0f, 1.0f);\n"
		"      if (use_tex_matrix) {\n"
		"         coord = mul(tex_matrix, coord);\n"
		"      }\n"
		"      float4 sample = tex2D(s, coord.xy);\n"
		"      tmp *= sample;\n"
		"   }\n"
		"   return tmp;\n"
		"}\n";
		
		static const char *cheap_pixel_source =
		"texture tex;\n"
		"sampler2D s = sampler_state {\n"
		"   texture = <tex>;\n"
		"};\n"
		"float4 ps_main(VS_OUTPUT Input) : COLOR0\n"
		"{\n"
		"   return tex2D(s, Input.TexCoord.xy) * Input.Color;\n"
		"}\n";
		
		static const char *tinter_pixel_source =
		"bool use_tex;\n"
		"texture tex;\n"
		"bool use_tex_matrix;\n"
		"float4x4 tex_matrix;\n"
		"float r;\n"
		"float g;\n"
		"float b;\n"
		"float ratio;\n"
		"sampler2D s = sampler_state {\n"
		"   texture = <tex>;\n"
		"};\n"
		"float4 ps_main(VS_OUTPUT Input) : COLOR0\n"
		"{\n"
		"   float avg, dr, dg, db;\n"
		"   float4 color = Input.Color;\n"
		"   if (use_tex) {\n"
		"      float4 coord = float4(Input.TexCoord.x, Input.TexCoord.y, 0.0f, 1.0f);\n"
		"      if (use_tex_matrix) {\n"
		"         coord = mul(tex_matrix, coord);\n"
		"      }\n"
		"      color *= tex2D(s, coord.xy);\n"
		"      avg = (color.r + color.g + color.b) / 3.0f;\n"
		"      dr = avg * r;\n"
		"      dg = avg * g;\n"
		"      db = avg * b;\n"
		"      color.r = color.r - (ratio * (color.r - dr));\n"
		"      color.g = color.g - (ratio * (color.g - dg));\n"
		"      color.b = color.b - (ratio * (color.b - db));\n"
		"   }\n"
		"   return color;\n"
		"}\n";
		
		// spiral transition
#if 0
		static const char *warp_pixel_source =
		"float angle;\n"
		"float cx;\n"
		"float cy;\n"
		"texture tex;\n"
		"sampler2D s = sampler_state {\n"
		"   texture = <tex>;\n"
		"};\n"
		"float4 ps_main(VS_OUTPUT Input) : COLOR0\n"
		"{\n"
		"   float x_offs = Input.TexCoord.x - cx;\n"
		"   float y_offs = Input.TexCoord.y - cy;\n"
		"   float dist = sqrt(x_offs*x_offs + y_offs*y_offs);\n"
		"   float curr_a = atan2(y_offs, x_offs);\n"
		"   float wave;\n"
		"   if (angle <= 3.14159) {\n"
		"      wave = angle;\n"
		"   }\n"
		"   else {\n"
		"      wave = (3.14159*2.0) - angle;\n"
		"   }\n"
		"   wave = dist/0.707*wave*4;\n"
		"   float v1 = cos(curr_a + angle) * dist + cx;\n"
		"   float v2 = sin(curr_a + (angle * wave)) * dist + cy;\n"
		"   return tex2D(s, float2(v1, v2));\n"
		"}\n";
#endif
		
		const char *warp2_pixel_source =
		"texture tex;\n"
		"sampler2D s = sampler_state {\n"
		"   texture = <tex>;\n"
		"};\n"
		"float angle;\n"
		"float tex_bot;\n"
		"float4 ps_main(VS_OUTPUT Input) : COLOR0\n"
		"{\n"
		"   float div;\n"
		"   div = angle / (3.14159*2.0);\n"
		"   if (div > 0.5)\n"
		"      div = 1.0 - div;\n"
		"   div = (0.5 - div) * 25.0 + 5.0;\n"
		"   float o = (sin((angle+(Input.TexCoord.x-0.5))*4.0) / div) + Input.TexCoord.y;\n"
		"   if (o < 0.0 || o > tex_bot)\n"
		"      return float4(0, 0, 0, 1);\n"
		"   else\n"
		"      return tex2D(s, float2(Input.TexCoord.x, o));\n"
		"}\n";
		
		const char *shadow_pixel_source =
		"float x1;\n"
		"float y1;\n"
		"float x2;\n"
		"float y2;\n"
		"float BW;\n"
		"float BH;\n"
		"float4 ps_main(VS_OUTPUT Input) : COLOR0\n"
		"{\n"
		"   float4 homo = Input.ClipPos / Input.ClipPos.w;\n"
		"   float2 gl_FragCoord;\n"
		"   gl_FragCoord.x = (homo.x + 1.0) * (BW/2.0);\n"
		"   gl_FragCoord.y = (homo.y + 1.0) * (BH/2.0);\n"
		"   gl_FragCoord.y = (BH-1.0) - gl_FragCoord.y;\n"
		"   float rx, ry;\n"
		"   float dx, dy;\n"
		"   rx = clamp(gl_FragCoord.x, x1, x2);\n"
		"   ry = clamp(gl_FragCoord.y, y1, y2);\n"
		"   dx = gl_FragCoord.x - rx;\n"
		"   dy = gl_FragCoord.y - ry;\n"
		"   float dist = clamp(sqrt(dx*dx + dy*dy), 0.0, 10.0);\n"
		"   float alpha = 1.0 - (dist / 10.0);\n"
		"   return float4(0.0, 0.0, 0.0, alpha);\n"
		"}\n";
		
		static const char *brighten_pixel_source =
		"bool use_tex;\n"
		"texture tex;\n"
		"bool use_tex_matrix;\n"
		"float4x4 tex_matrix;\n"
		"float brightness;\n"
		"sampler2D s = sampler_state {\n"
		"   texture = <tex>;\n"
		"};\n"
		"float4 ps_main(VS_OUTPUT Input) : COLOR0\n"
		"{\n"
		"   float4 coord = float4(Input.TexCoord.x, Input.TexCoord.y, 0.0f, 1.0f);\n"
		"   if (use_tex_matrix) {\n"
		"      coord = mul(tex_matrix, coord);\n"
		"   }\n"
		"   float4 color = Input.Color;\n"
		"   if (use_tex) {\n"
		"      color *= tex2D(s, coord.xy);\n"
		"   }\n"
		"   if (color.a == 0.0f) {\n"
		"      return float4(0.0f, 0.0f, 0.0f, 0.0f);\n"
		"   }\n"
		"   else {\n"
		"      float src_factor = 1.0f - brightness;\n"
		"      color.r *= src_factor;\n"
		"      color.r = color.r + brightness;\n"
		"      color.g *= src_factor;\n"
		"      color.g = color.g + brightness;\n"
		"      color.b *= src_factor;\n"
		"      color.b = color.b + brightness;\n"
		"      return color;\n"
		"   }\n"
		"}\n";
#endif
		
#ifdef A5_D3D
		default_shader = al_create_shader(ALLEGRO_SHADER_HLSL);
		cheap_shader = al_create_shader(ALLEGRO_SHADER_HLSL);
		tinter = al_create_shader(ALLEGRO_SHADER_HLSL);
		warp = al_create_shader(ALLEGRO_SHADER_HLSL);
		shadow_shader = al_create_shader(ALLEGRO_SHADER_HLSL);
		brighten = al_create_shader(ALLEGRO_SHADER_HLSL);
#else
		default_shader = al_create_shader(ALLEGRO_SHADER_GLSL);
		cheap_shader = al_create_shader(ALLEGRO_SHADER_GLSL);
		tinter = al_create_shader(ALLEGRO_SHADER_GLSL);
		warp = al_create_shader(ALLEGRO_SHADER_GLSL);
		shadow_shader = al_create_shader(ALLEGRO_SHADER_GLSL);
		brighten = al_create_shader(ALLEGRO_SHADER_GLSL);
#ifdef ALLEGRO_IPHONE
		scale2x = al_create_shader(ALLEGRO_SHADER_GLSL);
		scale2x_flipped = al_create_shader(ALLEGRO_SHADER_GLSL);
		scale3x = al_create_shader(ALLEGRO_SHADER_GLSL);
		scale3x_flipped = al_create_shader(ALLEGRO_SHADER_GLSL);
		scale2x_linear = al_create_shader(ALLEGRO_SHADER_GLSL);
		scale2x_linear_flipped = al_create_shader(ALLEGRO_SHADER_GLSL);
		scale3x_linear = al_create_shader(ALLEGRO_SHADER_GLSL);
		scale3x_linear_flipped = al_create_shader(ALLEGRO_SHADER_GLSL);
#endif
#endif
		
		al_attach_shader_source(
					default_shader,
					ALLEGRO_VERTEX_SHADER,
					main_vertex_source
					);
		
		al_attach_shader_source(
					cheap_shader,
					ALLEGRO_VERTEX_SHADER,
					main_vertex_source
					);
		
		al_attach_shader_source(
					tinter,
					ALLEGRO_VERTEX_SHADER,
					default_vertex_source
					);
		
		al_attach_shader_source(
					warp,
					ALLEGRO_VERTEX_SHADER,
					warp_vertex_source
					);
		
		al_attach_shader_source(
					shadow_shader,
					ALLEGRO_VERTEX_SHADER,
					default_vertex_source
					);
		
		al_attach_shader_source(
					brighten,
					ALLEGRO_VERTEX_SHADER,
					default_vertex_source
					);
		
#ifdef ALLEGRO_IPHONE
		al_attach_shader_source(
					scale2x,
					ALLEGRO_VERTEX_SHADER,
					scale2x_vertex_source
					);
		
		al_attach_shader_source(
					scale2x_flipped,
					ALLEGRO_VERTEX_SHADER,
					scale2x_vertex_source
					);
		
		al_attach_shader_source(
					scale3x,
					ALLEGRO_VERTEX_SHADER,
					scale3x_vertex_source
					);
		
		al_attach_shader_source(
					scale3x_flipped,
					ALLEGRO_VERTEX_SHADER,
					scale3x_vertex_source
					);
		
		al_attach_shader_source(
					scale2x_linear,
					ALLEGRO_VERTEX_SHADER,
					scale2x_vertex_source
					);
		
		al_attach_shader_source(
					scale2x_linear_flipped,
					ALLEGRO_VERTEX_SHADER,
					scale2x_vertex_source
					);
		
		al_attach_shader_source(
					scale3x_linear,
					ALLEGRO_VERTEX_SHADER,
					scale3x_vertex_source
					);
		
		al_attach_shader_source(
					scale3x_linear_flipped,
					ALLEGRO_VERTEX_SHADER,
					scale3x_vertex_source
					);
#endif
		
		al_attach_shader_source(
					default_shader,
					ALLEGRO_PIXEL_SHADER,
					main_pixel_source
					);
		
		al_attach_shader_source(
					cheap_shader,
					ALLEGRO_PIXEL_SHADER,
					cheap_pixel_source
					);
		
		al_attach_shader_source(
					tinter,
					ALLEGRO_PIXEL_SHADER,
					tinter_pixel_source
					);
		
		al_attach_shader_source(
					warp,
					ALLEGRO_PIXEL_SHADER,
					warp2_pixel_source
					);
		
		al_attach_shader_source(
					shadow_shader,
					ALLEGRO_PIXEL_SHADER,
					shadow_pixel_source
					);
		
		al_attach_shader_source(
					brighten,
					ALLEGRO_PIXEL_SHADER,
					brighten_pixel_source
					);
		
#ifdef ALLEGRO_IPHONE
		al_attach_shader_source(
					scale2x,
					ALLEGRO_PIXEL_SHADER,
					scale2x_pixel_source
					);
		
		al_attach_shader_source(
					scale2x_flipped,
					ALLEGRO_PIXEL_SHADER,
					scale2x_flipped_pixel_source
					);
		
		al_attach_shader_source(
					scale3x,
					ALLEGRO_PIXEL_SHADER,
					scale3x_pixel_source
					);
		
		al_attach_shader_source(
					scale3x_flipped,
					ALLEGRO_PIXEL_SHADER,
					scale3x_flipped_pixel_source
					);
		
		al_attach_shader_source(
					scale2x_linear,
					ALLEGRO_PIXEL_SHADER,
					scale2x_linear_pixel_source
					);
		
		al_attach_shader_source(
					scale2x_linear_flipped,
					ALLEGRO_PIXEL_SHADER,
					scale2x_linear_flipped_pixel_source
					);
		
		al_attach_shader_source(
					scale3x_linear,
					ALLEGRO_PIXEL_SHADER,
					scale3x_linear_pixel_source
					);
		
		al_attach_shader_source(
					scale3x_linear_flipped,
					ALLEGRO_PIXEL_SHADER,
					scale3x_linear_flipped_pixel_source
					);
#endif
		
		const char *shader_log;
		
		al_link_shader(default_shader);
		if ((shader_log = al_get_shader_log(default_shader))[0] != 0) {
			printf("1. %s\n", shader_log);
		}
		al_link_shader(cheap_shader);
		if ((shader_log = al_get_shader_log(cheap_shader))[0] != 0) {
			printf("2. %s\n", shader_log);
		}
		al_link_shader(tinter);
		if ((shader_log = al_get_shader_log(tinter))[0] != 0) {
			printf("3. %s\n", shader_log);
		}
		al_link_shader(warp);
		if ((shader_log = al_get_shader_log(warp))[0] != 0) {
			printf("4. %s\n", shader_log);
		}
		al_link_shader(shadow_shader);
		if ((shader_log = al_get_shader_log(shadow_shader))[0] != 0) {
			printf("5. %s\n", shader_log);
		}
		al_link_shader(brighten);
		if ((shader_log = al_get_shader_log(brighten))[0] != 0) {
			printf("6. %s\n", shader_log);
		}
#ifdef ALLEGRO_IPHONE
		al_link_shader(scale2x);
		if ((shader_log = al_get_shader_log(scale2x))[0] != 0) {
			printf("7. %s\n", shader_log);
		}
		al_link_shader(scale2x_flipped);
		if ((shader_log = al_get_shader_log(scale2x_flipped))[0] != 0) {
			printf("8. %s\n", shader_log);
		}
		al_link_shader(scale3x);
		al_link_shader(scale3x_flipped);
		al_link_shader(scale2x_linear);
		al_link_shader(scale2x_linear_flipped);
		al_link_shader(scale3x_linear);
		al_link_shader(scale3x_linear_flipped);
#endif
		
#ifdef A5_OGL
		al_set_opengl_program_object(display, al_get_opengl_program_object(default_shader));
#else
		al_set_direct3d_effect(display, al_get_direct3d_effect(default_shader));
#endif
		
	}
#endif

#ifdef A5_OGL
      glDisable(GL_DITHER);
   if (!use_programmable_pipeline) {
      glShadeModel(GL_FLAT);
      glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
   }
#endif


}

void init2_shaders(void)
{
	
	if (use_programmable_pipeline) {
		int buffer_true_w, buffer_true_h;
		get_buffer_true_size(&buffer_true_w, &buffer_true_h);
#ifdef ALLEGRO_IPHONE
		al_set_shader_float(scale2x, "hstep", 1.0/buffer_true_w);
		al_set_shader_float(scale2x, "vstep", 1.0/buffer_true_h);
		al_set_shader_float(scale2x, "offset_x", 1);
		al_set_shader_float(scale2x, "offset_y", 0);
		al_set_shader_float(scale2x_flipped, "hstep", 1.0/buffer_true_w);
		al_set_shader_float(scale2x_flipped, "vstep", 1.0/buffer_true_h);
		al_set_shader_float(scale2x_flipped, "offset_x", 1);
		al_set_shader_float(scale2x_flipped, "offset_y", 0);
		al_set_shader_float(scale3x, "hstep", 1.0/buffer_true_w);
		al_set_shader_float(scale3x, "vstep", 1.0/buffer_true_h);
		al_set_shader_float(scale3x, "offset_x", 0);
		al_set_shader_float(scale3x, "offset_y", 0);
		al_set_shader_float(scale3x_flipped, "hstep", 1.0/buffer_true_w);
		al_set_shader_float(scale3x_flipped, "vstep", 1.0/buffer_true_h);
		al_set_shader_float(scale3x_flipped, "offset_x", 0);
		al_set_shader_float(scale3x_flipped, "offset_y", 0);
		al_set_shader_float(scale2x_linear, "hstep", 1.0/buffer_true_w);
		al_set_shader_float(scale2x_linear, "vstep", 1.0/buffer_true_h);
		al_set_shader_float(scale2x_linear, "offset_x", 0);
		al_set_shader_float(scale2x_linear, "offset_y", 1);
		al_set_shader_float(scale2x_linear_flipped, "hstep", 1.0/buffer_true_w);
		al_set_shader_float(scale2x_linear_flipped, "vstep", 1.0/buffer_true_h);
		al_set_shader_float(scale2x_linear_flipped, "offset_x", 0);
		al_set_shader_float(scale2x_linear_flipped, "offset_y", 1);
		al_set_shader_float(scale3x_linear, "hstep", 1.0/buffer_true_w);
		al_set_shader_float(scale3x_linear, "vstep", 1.0/buffer_true_h);
		al_set_shader_float(scale3x_linear, "offset_x", 0);
		al_set_shader_float(scale3x_linear, "offset_y", 0);
		al_set_shader_float(scale3x_linear_flipped, "hstep", 1.0/buffer_true_w);
		al_set_shader_float(scale3x_linear_flipped, "vstep", 1.0/buffer_true_h);
		al_set_shader_float(scale3x_linear_flipped, "offset_x", 0);
		al_set_shader_float(scale3x_linear_flipped, "offset_y", 0);
		if (scaleXX_buffer) {
			al_set_shader_sampler(scale2x, "tex", scaleXX_buffer->bitmap, 0);
			al_set_shader_sampler(scale2x_flipped, "tex", scaleXX_buffer->bitmap, 0);
			al_set_shader_sampler(scale3x, "tex", scaleXX_buffer->bitmap, 0);
			al_set_shader_sampler(scale3x_flipped, "tex", scaleXX_buffer->bitmap, 0);
		}
#endif
		
		
		al_set_shader_bool(default_shader, "use_tex_matrix", false);
		al_set_shader_bool(tinter, "use_tex_matrix", false);
		al_set_shader_bool(brighten, "use_tex_matrix", false);
		
#ifndef A5_OGL
		al_set_shader_float(warp, "cx", ((float)BW/buffer_true_w)/2);
		al_set_shader_float(warp, "cy", ((float)BH/buffer_true_h)/2);
#endif
	}	
}

void destroy_shaders(void)
{
	if (use_programmable_pipeline) {
		al_destroy_shader(default_shader);
		al_destroy_shader(cheap_shader);
		al_destroy_shader(tinter);
		al_destroy_shader(brighten);
		al_destroy_shader(warp);
		al_destroy_shader(shadow_shader);
#ifdef ALLEGRO_IPHONE
		al_destroy_shader(scale2x);
		al_destroy_shader(scale2x_flipped);
		al_destroy_shader(scale3x);
		al_destroy_shader(scale3x_flipped);
		al_destroy_shader(scale2x_linear);
		al_destroy_shader(scale2x_linear_flipped);
		al_destroy_shader(scale3x_linear);
		al_destroy_shader(scale3x_linear_flipped);
#endif
	}	
}
#endif

bool init(int *argc, char **argv[])
{
	myArgc = *argc;
	myArgv = *argv;

#ifndef ALLEGRO_WINDOWS
//FIXME:
#else
#ifdef EDITOR
	_getcwd(start_cwd, 999);
#endif
#endif

	srand((int)time(NULL));

	if (check_arg(*argc, *argv, "-tiny") > 0)
		scr_tiny = true;
	else if (check_arg(*argc, *argv, "-small") > 0)
		scr_small = true;

	bool use_fixed_pipeline = false;
	if (check_arg(*argc, *argv, "-fixed-pipeline") > 0)
		use_fixed_pipeline = true;

	al_init();

#if !defined ALLEGRO_WINDOWS
	ALLEGRO_CONFIG *syscfg = al_get_system_config();
	al_set_config_value(syscfg, "graphics", "config_selection", "old");
#endif

	input_event_mutex = al_create_mutex();

#if !defined ALLEGRO_IPHONE
	al_set_org_name("Nooskewl");
#ifdef LITE
	al_set_app_name("Monster RPG 2 Lite");
#else
	al_set_app_name("Monster RPG 2");
#endif
#endif

	try {
		config.read();
	}
	catch (ReadError e) {
	}

#ifdef ALLEGRO_IPHONE
	config.setAutoRotation(config.getAutoRotation());
#endif

	load_translation_tags();
	load_translation(get_language_name(config.getLanguage()).c_str());
	
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
	dpad_type = config.getDpadType();
	use_dpad = false;
#else
	dpad_type = DPAD_TOTAL_1;
#endif

	debug_message("After al_init\n");

	debug_message("After config.read\n");

	ScreenDescriptor *sd = config.getWantedGraphicsMode();

	// FIXME: check return
	debug_message("al_init success\n");


	al_install_mouse();
#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_ANDROID
	al_install_keyboard();
#else
	al_install_touch_input();
	al_set_mouse_emulation_mode(ALLEGRO_MOUSE_EMULATION_5_0_x);
#endif

	gzFile f = gzopen(getResource("Version"), "rb");
	if (f) {
		versionMajor = (int)igetl(f);
		versionMinor = (int)igetl(f);
		sprintf(versionString, "%d.%d", versionMajor, versionMinor);
		gzclose(f);
	}
	else {
		strcpy(versionString, "-1");
	}

	al_init_font_addon();
	al_init_ttf_addon();
	al_init_image_addon();
	al_init_primitives_addon();
	
   	PHYSFS_init(myArgv[0]);
	PHYSFS_addToSearchPath(getResource("tiles.zip"), 1);

	int flags = 0;

#if defined A5_OGL
	flags |= ALLEGRO_OPENGL;
#endif

	al_set_new_display_flags(flags);

#if !defined(IPHONE) && !defined(ALLEGRO_ANDROID)
	al_set_new_display_option(ALLEGRO_DEPTH_SIZE, 16, ALLEGRO_REQUIRE);
	al_set_new_display_option(ALLEGRO_COLOR_SIZE, 16, ALLEGRO_REQUIRE);
	al_set_new_display_adapter(config.getAdapter());
	
	// set screenScale *for loading screen only*
	ScreenSize scr_sz = small_screen();
	if (scr_sz == ScreenSize_Tiny) {
		sd->width = 240;
		sd->height = 160;
	}
	else if (scr_sz == ScreenSize_Smaller) {
		sd->width = 480;
		sd->height = 320;
	}
	else if (scr_sz == ScreenSize_Small) {
		sd->width = 720;
		sd->height = 480;
	}
	else {
		sd->width = 960;
		sd->height = 640;
	}

	set_screen_params();
#else
	al_set_new_display_option(ALLEGRO_DEPTH_SIZE, 16, ALLEGRO_REQUIRE);
	al_set_new_display_option(ALLEGRO_COLOR_SIZE, 16, ALLEGRO_REQUIRE);
	al_set_new_display_adapter(0);
#endif

	if (config.getWaitForVsync())
		al_set_new_display_option(ALLEGRO_VSYNC, 1, ALLEGRO_SUGGEST);


#ifdef ALLEGRO_IPHONE
	initial_screen_scale = al_iphone_get_screen_scale();

	if (initial_screen_scale == 2.0f) {
		if (config.getFilterType() == FILTER_NONE) {
			al_iphone_override_screen_scale(2.0);
		}
		else if (config.getFilterType() == FILTER_LINEAR) {
			al_iphone_override_screen_scale(2.0);
		}
		else if (config.getFilterType() == FILTER_SCALE2X || config.getFilterType() == FILTER_SCALE2X_LINEAR) {
			al_iphone_override_screen_scale(1.0f);
		}
		else if (config.getFilterType() == FILTER_SCALE3X || config.getFilterType() == FILTER_SCALE3X_LINEAR) {
			al_iphone_override_screen_scale(1.5f);
		}
		else if (config.getFilterType() == FILTER_HALFSCALE) {
			al_iphone_override_screen_scale(1.0f);
		}
		else if (config.getFilterType() == FILTER_SCALEDOWN) {
			al_iphone_override_screen_scale(0.5);
		}
		screenScale *= al_iphone_get_screen_scale();
	}
	
	if (is_ipad()) {
		if (config.getFilterType() == FILTER_SCALE2X) {
			al_iphone_override_screen_scale(0.5);
			screenScale = 2;
			sd->width = 768 / 2;
			sd->height = 1024 / 2;
		}
		else if (config.getFilterType() == FILTER_SCALE2X_LINEAR) {
			screenScale = 4;
			sd->width = 768;
			sd->height = 1024;
		}
		else if (config.getFilterType() == FILTER_SCALE3X_LINEAR) {
			screenScale = 4;
			sd->width = 768;
			sd->height = 1024;
		}
		else if (config.getFilterType() == FILTER_SCALE3X) {
			al_iphone_override_screen_scale(0.75);
			screenScale = 3;
			sd->width = 768 * 0.75;
			sd->height = 1024 * 0.75;
		}
		else if (config.getFilterType() == FILTER_HALFSCALE) {
			al_iphone_override_screen_scale(0.5);
			screenScale = 2;
			sd->width = 768 / 2;
			sd->height = 1024 / 2;			
		}
		else if (config.getFilterType() == FILTER_SCALEDOWN) {
			al_iphone_override_screen_scale(0.25);
			screenScale = 1;
			sd->width = 768 / 4;
			sd->height = 1024 / 4;
		}
		else {
			screenScale = 4;
			sd->width = 768;
			sd->height = 1024;
		}
	}
	else {
		sd->width = 160 * screenScale;
		sd->height = 240 * screenScale;
	}
#endif
	click_mutex = al_create_mutex();
	input_mutex = al_create_mutex();
	dpad_mutex = al_create_mutex();
	touch_mutex = al_create_mutex();
	//orient_mutex = al_create_mutex();

	tguiInit();

#ifdef A5_D3D
	use_fixed_pipeline = true;
#endif

	if (!use_fixed_pipeline) {
		al_set_new_display_flags(al_get_new_display_flags() | ALLEGRO_USE_PROGRAMMABLE_PIPELINE);
	}
	
#if defined ALLEGRO_IPHONE
	al_set_new_display_option(ALLEGRO_SUPPORTED_ORIENTATIONS, ALLEGRO_DISPLAY_ORIENTATION_LANDSCAPE, ALLEGRO_REQUIRE);
#endif	

#ifdef EDITOR
	display = al_create_display(800, 500);
#else
	display = al_create_display(sd->width, sd->height);
	if (!display) {
		safemode = true;
		flags &= ~ALLEGRO_FULLSCREEN;
		flags |= ALLEGRO_WINDOWED;
		al_set_new_display_flags(flags);
		display = al_create_display(720, 480);
		if (!display) {
			display = al_create_display(480, 320);
		}
	}
	
	//al_rest(3);
	
#endif
	if (!display) {
		if (!native_error("Failed to set gfx mode"))
			return false;
	}

#ifdef ALLEGRO_IPHONE
   NSAutoreleasePool *p = [[NSAutoreleasePool alloc] init];

   NSString *reqSysVer = @"3.2";
   NSString *currSysVer = [[UIDevice currentDevice] systemVersion];
   BOOL osVersionSupported = ([currSysVer compare:reqSysVer options:NSNumericSearch] != NSOrderedAscending);
   if (osVersionSupported) {
   	// FIXME!
   	//PRESERVE_TEXTURE = ALLEGRO_PRESERVE_TEXTURE;
   }

   [p drain];
#endif

	al_set_new_bitmap_flags(PRESERVE_TEXTURE);

#if defined ALLEGRO_IPHONE || defined ALLEGRO_MACOSX
	init_joypad();
#endif

#if defined ALLEGRO_IPHONE && !defined LITE
	authenticatePlayer();
#endif

#ifdef ALLEGRO_MACOSX
	if (sd->fullscreen) {
		sd->fullscreen = false; // hack
		toggle_fullscreen();
	}
#endif

#ifdef A5_OGL
	if (use_fixed_pipeline) {
		my_opengl_version = 0x01;
	}
	else {
		if (!al_have_opengl_extension("GL_EXT_framebuffer_object")
		 && !al_have_opengl_extension("GL_ARB_framebuffer_object")) {
			if (!native_error("Fragment shaders not supported"))
				return false;
		}

		my_opengl_version = al_get_opengl_version();
	}
#else
	//my_opengl_version = 0x02000000;
	my_opengl_version = 0x0;

	#ifdef A5_D3D
	/*
	al_get_d3d_device(display)->SetRenderState(D3DRS_ALPHAREF, 1);
	al_get_d3d_device(display)->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	al_get_d3d_device(display)->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
	*/
	#endif
#endif

	if (use_fixed_pipeline) {
		use_programmable_pipeline = false;
	}
	else {
		use_programmable_pipeline = al_get_display_flags(display) & ALLEGRO_USE_PROGRAMMABLE_PIPELINE;
	}

	#ifndef A5_D3D
	init_shaders();
	#endif

	if (al_install_joystick()) {
		if (al_get_num_joysticks() > 0)
			config.setGamepadAvailable(true);
		else
			config.setGamepadAvailable(false);
	}
	else {
		config.setGamepadAvailable(false);
	}

	if (config.getGamepadAvailable()) {
//		al_get_joystick(0); // needed?
	}

	al_run_detached_thread(thread_proc_minor, NULL);

	debug_message("GFX mode set.\n");

	black = m_map_rgb(0, 0, 0);
	white = m_map_rgb(255, 255, 255);
	grey = m_map_rgb(200, 200, 200);
	blue = m_map_rgb(30, 0, 150);




#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_ANDROID
#ifdef A5_D3D
	d3d_resource_mutex = al_create_mutex();
	init_big_depth_surface();
	al_d3d_set_restore_callback(d3d_resource_restore);
	al_d3d_set_release_callback(d3d_resource_release);
#endif

	m_clear(black);
	m_flip_display();
#endif
	
#ifdef LITE
	al_set_window_title(display, "Monster RPG 2 Lite");
#else
	al_set_window_title(display, "Monster RPG 2");
#endif

	m_set_blender(M_ONE, M_INVERSE_ALPHA, white);

#ifdef A5_OGL
#if defined ALLEGOR_IPHONE || defined ALLEGRO_ANDROID
	al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_RGBA_4444);
#else
	al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_ABGR_8888_LE);
#endif
#else
	al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_ARGB_8888);
#endif

	flags = al_get_new_bitmap_flags();
	buffer = m_create_bitmap(BW, BH); // check
	overlay = m_create_bitmap(BW, BH); // check

	if (config.getFilterType() == FILTER_SCALE2X_LINEAR) {
		scaleXX_buffer = m_create_bitmap(BW*2, BH*2); // check
	}
	else if (config.getFilterType() == FILTER_SCALE3X_LINEAR) {
		scaleXX_buffer = m_create_bitmap(BW*3, BH*3); // check
	}
	else {
		scaleXX_buffer = NULL;
	}
	
	screenshot = m_create_bitmap(BW/2, BH/2); // check

	al_set_new_bitmap_flags(flags);

#if defined ALLEGRO_IPHONE
	screen_offset_x = 0;
	screen_offset_y = 0;
	screen_ratio_x = 1.0;
	screen_ratio_y = 1.0;
	if (is_ipad()) {
		ALLEGRO_MONITOR_INFO mi;
		al_get_monitor_info(0, &mi);
		int scr_w = mi.y2-mi.y1;
		int scr_h = mi.x2-mi.x1;
		float mul;
		if (config.getFilterType() == FILTER_SCALE2X || config.getFilterType() == FILTER_HALFSCALE
		) {
			mul = 2;
		}
		else if (config.getFilterType() == FILTER_SCALE3X) {
			mul = 1.0/0.75;
		}
		else if (config.getFilterType() == FILTER_SCALEDOWN) {
			mul = 4;
		}
		else {
			mul = 1;
		}
		screen_offset_x = (scr_w - (BW*screenScale*mul)) / (2*mul);
		screen_offset_y = (scr_h - (BH*screenScale*mul)) / (2*mul);
		screen_ratio_x = (float)scr_w / (BW*screenScale*mul);
		screen_ratio_y = (float)scr_h / (BH*screenScale*mul);
	}
#endif

#ifndef A5_D3D
	init2_shaders();
#endif

#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
	al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_RGBA_4444);
#elif defined A5_OGL
	al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_ABGR_8888_LE);
#else
	al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_ARGB_8888);
#endif

	if (!buffer) {
		if (!native_error("Failed to create buffer"))
			return false;
	}

	config.setFilterType(config.getFilterType());
	
	
	if (!screenshot) {
		if (!native_error("Failed to create screenshot"))
			return false;
	}

	
#ifdef WIZ
	if (startGFXDriver == 0) {
		buffer->memory += (buffer->h-1)*buffer->pitch;
		buffer->pitch = -buffer->pitch;
	}
	harpy = new AnimationSet(getResource("combat_media/Harpy.png"));
#endif



	eny_loader = new AnimationSet(getResource("media/eny-loader.png"));
	dot_loader = new AnimationSet(getResource("media/dot-loader.png"));
	bg_loader = m_load_bitmap(getResource("media/bg-loader.png"));
	bar_loader = m_load_bitmap(getResource("media/bar-loader.png"));
	loading_loader = m_load_bitmap(getResource("media/loading-loader.png"));
	MBITMAP *tmp = m_create_bitmap(BH, BW); // check
	corner_bmp = m_load_bitmap(getResource("media/corner.png"));

	/* ON IPHONE WE LOAD EVERYTHING AS MEMORY BITMAP FIRST THEN
	 * CONVERT THEM TO DISPLAY BITMAPS SO WE CAN SHOW A SMOOTH
	 * PROGRESS BAR */
	
	tguiSetRotation(0);

#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
	tguiSetScreenSize(BH*screenScale, BW*screenScale);
	config.setMaintainAspectRatio(config.getMaintainAspectRatio());
#elif !defined EDITOR
	tguiSetScreenSize(BW*screenScale, BH*screenScale);
	config.setMaintainAspectRatio(config.getMaintainAspectRatio());
#endif

#ifndef EDITOR
	tguiSetScale(screenScale, screenScale);

	tguiSetTolerance(3);
#endif

	config.setUseOnlyMemoryBitmaps(true);
	al_run_detached_thread(loader_proc, NULL);

	long start = -1;
	int draw = 1000/30;
	while (1) {
		long now = tguiCurrentTimeMillis();
		if (start < 0) start = now;
		int elapsed = (int)(now - start);
		start = now;
		eny_loader->update(elapsed);
		dot_loader->update(elapsed);
		draw -= elapsed;

 		int percent = progress_percent;
 		if (draw < 0 || percent == 100) {
 			draw = 1000/30;
			m_set_target_bitmap(tmp);
 			m_set_blender(M_ONE, M_INVERSE_ALPHA, white);
 			m_draw_bitmap(bg_loader, 0, 0, 0);
			m_draw_bitmap(loading_loader, 33, 10, 0);
 			int bx = 38;
 			int by = 34;
 			int bw = m_get_bitmap_width(bar_loader);
 			int bh = m_get_bitmap_height(bar_loader);
			al_draw_filled_rectangle(bx-1, by-1, bx+bw+1, by+bh+1, white);
			al_draw_filled_rectangle(bx, by, bx+bw, by+bh, black);
			m_draw_bitmap_region(bar_loader, 0, 0, bw*(percent/100.0f), bh, bx, by, 0);
			eny_loader->draw(BH/2-eny_loader->getWidth()/2-25, BW-eny_loader->getHeight()-35, 0);
			dot_loader->draw(118, 24, 0);
			al_set_target_backbuffer(display);
			m_clear(black);
#if !defined IPHONE && !defined ALLEGRO_ANDROID
			float scale = screenScale / 2.0f;
			if (config.getMaintainAspectRatio()) {
				m_draw_scaled_bitmap(tmp, 0, 0, BH, BW, screen_offset_x, screen_offset_y, BH*scale, BW*scale, 0);
			}
			else {
				float rx, ry;
				if (sd->fullscreen) {
					rx = screen_ratio_x;
					ry = screen_ratio_y;
				}
				else {
					rx = ry = 1;
				}
				m_draw_scaled_bitmap(tmp, 0, 0, BH, BW, 0, 0, BH*scale*rx, BW*scale*ry, 0);
			}
#else
			if (is_ipad()) {
				m_draw_scaled_bitmap(tmp, 0, 0, BH, BW, 0, 0, sd->width*BH/BW, sd->width, 0);
			}
			else {
				m_draw_scaled_bitmap(tmp, 0, 0, BH, BW, screen_offset_x, screen_offset_y, BH*screenScale*BH/BW, BW*screenScale*BH/BW, 0);
			}
#endif
 			m_flip_display();
 		}
 		if (percent == 100) {
			m_rest(1);
 			break;
 		}
	}

	m_destroy_bitmap(tmp);
   
	while (!loading_done) {
		m_rest(0.001);
	}

	config.setUseOnlyMemoryBitmaps(false);

	stomach_circle = m_load_bitmap(getResource("combat_media/stomach_circle.png"));
	
	if (!loadTilemap()) {
		return false;
	}

	int bflags = al_get_new_bitmap_flags();

	int ttf_flags;
	
	if (config.getFilterType() == FILTER_SCALE2X) {
		ttf_flags = 0;
	}
	else {
		ttf_flags = ALLEGRO_TTF_MONOCHROME;
	}

	al_set_new_bitmap_flags(bflags & ~ALLEGRO_NO_PRESERVE_TEXTURE);

	game_font = al_load_ttf_font(getResource("DejaVuSans.ttf"), 9, ttf_flags);
	if (!game_font) {
		if (!native_error("Failed to load game_font"))
			return false;
	}

	medium_font = al_load_ttf_font(getResource("DejaVuSans.ttf"), 32, 0);
	if (!game_font) {
		if (!native_error("Failed to load medium_font"))
			return false;
	}
	
	huge_font = m_load_font(getResource("huge_font.png"));
	if (!huge_font) {
		if (!native_error("Failed to load huge_font"))
			return false;
	}

	/* RELOAD EVERYTHING AS DISPLAY BITMAPS */
	guiAnims->displayConvert();

	cursor = m_make_display_bitmap(cursor);
	
	m_push_target_bitmap();
	m_save_blender();
	orb_bmp = m_create_alpha_bitmap(80, 80);
	m_set_target_bitmap(orb_bmp);
	m_set_blender(M_ONE, M_ZERO, white);
	for (int yy = 0; yy < 80; yy++) {
		for (int xx = 0; xx < 80; xx++) {
			float ax = xx - 40; // actual x
			float ay = yy - 40;
			float dist = (float)sqrt(ax*ax + ay*ay);
			MCOLOR c;
			c.r = c.g = c.b = 0;
			if (dist >= 39)
				c.a = 1;
			else {
				if (dist > 39-12) {
					float v = dist - (39-12);
					c.a = (float)sin((v/12.0f)*(float)(M_PI/2));
				}
				else
					c.a = 0;
			}
			m_put_pixel(xx, yy, c);
		}
	}
	m_restore_blender();
	m_pop_target_bitmap();

	poison_bmp = m_make_alpha_display_bitmap(poison_bmp);
	poison_bmp_tmp = m_make_alpha_display_bitmap(poison_bmp_tmp);
	poison_bmp_tmp2 = m_make_alpha_display_bitmap(poison_bmp_tmp2);


	ALLEGRO_BITMAP *__old_target__ = al_get_target_bitmap();
	m_save_blender();
	shadow_sides[0] = m_create_alpha_bitmap(16, SHADOW_CORNER_SIZE); // check
	shadow_sides[1] = m_create_alpha_bitmap(SHADOW_CORNER_SIZE, 16); // check
	shadow_sides[2] = m_create_alpha_bitmap(16, SHADOW_CORNER_SIZE); // check
	shadow_sides[3] = m_create_alpha_bitmap(SHADOW_CORNER_SIZE, 16); // check
	for (int i = 0; i < 4; i++) {
		shadow_corners[i] = m_create_alpha_bitmap(SHADOW_CORNER_SIZE, SHADOW_CORNER_SIZE); // check
		m_set_target_bitmap(shadow_corners[i]);
		m_clear(m_map_rgba(0, 0, 0, 0));
		m_set_target_bitmap(shadow_sides[i]);
		m_clear(m_map_rgba(0, 0, 0, 0));
	}
	for (int yy = 0; yy < SHADOW_CORNER_SIZE; yy++) {
		for (int xx = 0; xx < SHADOW_CORNER_SIZE; xx++) {
			int dx = xx;
			int dy = yy;
 			float dist = (float)sqrt((float)dx*dx + dy*dy) + 1;
			if (dist > SHADOW_CORNER_SIZE) dist = SHADOW_CORNER_SIZE;
			float a = (1.0f-(dist/SHADOW_CORNER_SIZE))*255; //(1.0-sin(((float)dist/(SHADOW_CORNER_SIZE-1))*(M_PI/2))) * 255;
			m_set_target_bitmap(shadow_corners[2]);
			m_put_pixel(xx, yy, m_map_rgba(0, 0, 0, a));
		}
	}
	_blend_color = white;

	if (use_programmable_pipeline) {
		al_set_separate_blender(
			ALLEGRO_ADD,
			ALLEGRO_ONE,
			ALLEGRO_ZERO,
			ALLEGRO_ADD,
			ALLEGRO_ONE,
			ALLEGRO_ONE
		);
	}
	else {
		al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
	}

	m_set_target_bitmap(shadow_corners[0]);
	m_draw_bitmap(shadow_corners[2], 0, 0, M_FLIP_HORIZONTAL|M_FLIP_VERTICAL);
	m_set_target_bitmap(shadow_corners[1]);
	m_draw_bitmap(shadow_corners[2], 0, 0, M_FLIP_VERTICAL);
	m_set_target_bitmap(shadow_corners[3]);
	m_draw_bitmap(shadow_corners[2], 0, 0, M_FLIP_HORIZONTAL);
	m_set_target_bitmap(shadow_sides[0]);
	m_draw_bitmap_region(shadow_corners[0], SHADOW_CORNER_SIZE-1, 0, 1, SHADOW_CORNER_SIZE, 0, 0, 0);
	m_set_target_bitmap(shadow_sides[3]);
	m_draw_bitmap_region(shadow_corners[0], 0, SHADOW_CORNER_SIZE-1, SHADOW_CORNER_SIZE, 1, 0, 0, 0);
	m_set_target_bitmap(shadow_sides[2]);
	m_draw_bitmap(shadow_sides[0], 0, 0, M_FLIP_VERTICAL);
	m_set_target_bitmap(shadow_sides[1]);
	m_draw_bitmap(shadow_sides[3], 0, 0, M_FLIP_HORIZONTAL);
	al_set_target_bitmap(__old_target__);
	m_restore_blender();

	tile = m_make_display_bitmap(tile);

	//profileBg = m_make_display_bitmap(profileBg);
	
	for (int i = 0; i < (int)icons.size(); i++) {
		icons[i] = m_make_display_bitmap(icons[i]);
	}

	stoneTexture = m_make_display_bitmap(stoneTexture);
	
	mushroom = m_make_display_bitmap(mushroom);
	webbed = m_make_display_bitmap(webbed);

	al_set_new_display_flags(bflags);
	
	dpad_buttons = m_load_bitmap(getResource("media/buttons.png"));
	batteryIcon = m_load_bitmap(getResource("media/battery_icon.png"));
	achievement_bmp = m_load_bitmap(getResource("media/trophy.png"));

	m_set_target_bitmap(buffer);
	

	inited = true;

	m_clear(black);

	return true;
}


void destroy(void)
{
	debug_message("Destroying resources...\n");

	tguiShutdown();

	exit_event_thread = 1;
	while (exit_event_thread != 2) {
		m_rest(0.001);
	}
	exit_minor_event_thread = 1;
	while (exit_minor_event_thread != 2) {
		m_rest(0.001);
	}
	al_destroy_cond(wait_cond);
	al_destroy_mutex(wait_mutex);
	
	al_destroy_cond(switch_cond);
	al_destroy_mutex(switch_mutex);
	al_destroy_mutex(joypad_mutex);

	if (party[heroSpot] && party[heroSpot]->getObject()
			&& party[heroSpot]->getObject()->getInput()
			&& party[heroSpot]->getObject()->getInput() == tripleInput) {
		al_lock_mutex(input_mutex);
		destroyInput();
		party[heroSpot]->getObject()->setInput(NULL);
		al_unlock_mutex(input_mutex);
	}
	else
		destroyInput();

	debug_message("Destroy 1\n");
	destroyTilemap();
	debug_message("Destroy 2\n");

	m_destroy_bitmap(buffer);
	m_destroy_bitmap(overlay);
	if (scaleXX_buffer) {
		m_destroy_bitmap(scaleXX_buffer);
	}

	debug_message("Destroy 3\n");
	m_destroy_bitmap(tile);
	debug_message("Destroy 4\n");
	debug_message("Destroy 5\n");
	destroyIcons();
	debug_message("Destroy 6\n");
	//destroyWeaponAnimations();
	debug_message("Destroy 7\n");
	m_destroy_bitmap(stoneTexture);
	debug_message("Destroy 8\n");
	m_destroy_bitmap(mushroom);
	debug_message("Destroy 9\n");
	m_destroy_bitmap(webbed);
	m_destroy_bitmap(dpad_buttons);
	m_destroy_bitmap(batteryIcon);
	debug_message("Destroy 10\n");
	delete terrain;
	debug_message("Destroy 11\n");
	debug_message("Destroy 12\n");
	//m_destroy_bitmap(profileBg);
	debug_message("Destroy 13\n");
	m_destroy_bitmap(cursor);
	debug_message("Destroy 14\n");
	delete guiAnims;
	debug_message("Destroy 15\n");
	m_destroy_bitmap(orb_bmp);
	m_destroy_bitmap(poison_bmp);
	m_destroy_bitmap(poison_bmp_tmp);
	m_destroy_bitmap(poison_bmp_tmp2);
	m_destroy_bitmap(stomach_circle);
	if (screenshot) {
		m_destroy_bitmap(screenshot);
	}

	delete eny_loader;
	delete dot_loader;
	m_destroy_bitmap(bg_loader);
	m_destroy_bitmap(bar_loader);
	m_destroy_bitmap(loading_loader);
	m_destroy_bitmap(corner_bmp);

	for (int i = 0; i < 4; i++) {
		m_destroy_bitmap(shadow_corners[i]);
		m_destroy_bitmap(shadow_sides[i]);
	}

	m_destroy_font(game_font);
	m_destroy_font(huge_font);
	m_destroy_font(medium_font);

	for (int i = 0; i < MAX_PARTY; i++) {
		if (party[i]) {
			Object *o = party[i]->getObject();
			if (o)
				delete o;
			delete party[i];
			party[i] = NULL;
		}
	}
	debug_message("Destroy 16\n");

	if (area)
		delete area;
	debug_message("Destroy 17\n");

	#ifndef A5_D3D
	destroy_shaders();
	#endif

	#ifdef A5_D3D
	big_depth_surface->Release();
	al_destroy_mutex(d3d_resource_mutex);
	#endif

	al_destroy_display(display);

	debug_message("Destroy 18\n");
	if (saveFilename)
		free(saveFilename);
	debug_message("Destroy 19\n");

	cleanup_astar();
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
	al_destroy_mutex(click_mutex);
	al_destroy_mutex(input_mutex);
	al_destroy_mutex(dpad_mutex);
	al_destroy_mutex(touch_mutex);
#ifdef ALLEGRO_IPHONE
	shutdownIpod();
#endif
#endif

	al_destroy_mutex(input_event_mutex);

	destroySound();

	al_shutdown_ttf_addon();

	destroy_translation();
	
	destroy_string_bitmaps();

#ifdef ALLEGRO_WINDOWS
	al_uninstall_system();
#endif

	inited = false;
}

#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
static int dpad_count = 1;
#endif

void dpad_off(bool count)
{
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
	if (count)
		dpad_count++;
	if (dpad_count > 0 || !count) {
		al_lock_mutex(dpad_mutex);
		use_dpad = (dpad_type == DPAD_TOTAL_1 || dpad_type == DPAD_TOTAL_2);
		if (!joypad_connected()) {
			if (use_dpad)
				myTguiIgnore(TGUI_MOUSE);
			else {
				myTguiIgnore(0);
			}
			joystick_repeat_started[JOY_REPEAT_AXIS0] = false;
			joystick_initial_repeat_countdown[JOY_REPEAT_AXIS0] = JOY_INITIAL_REPEAT_TIME;
			joystick_repeat_countdown[JOY_REPEAT_AXIS0] = JOY_REPEAT_TIME;
			joystick_repeat_started[JOY_REPEAT_AXIS1] = false;
			joystick_initial_repeat_countdown[JOY_REPEAT_AXIS1] = JOY_INITIAL_REPEAT_TIME;
			joystick_repeat_countdown[JOY_REPEAT_AXIS1] = JOY_REPEAT_TIME;
			joystick_repeat_started[JOY_REPEAT_B1] = false;
			joystick_initial_repeat_countdown[JOY_REPEAT_B1] = JOY_INITIAL_REPEAT_TIME;
			joystick_repeat_countdown[JOY_REPEAT_B1] = JOY_REPEAT_TIME;
			joystick_repeat_started[JOY_REPEAT_B2] = false;
			joystick_initial_repeat_countdown[JOY_REPEAT_B2] = JOY_INITIAL_REPEAT_TIME;
			joystick_repeat_countdown[JOY_REPEAT_B2] = JOY_REPEAT_TIME;
			joystick_repeat_started[JOY_REPEAT_B3] = false;
			joystick_initial_repeat_countdown[JOY_REPEAT_B3] = JOY_INITIAL_REPEAT_TIME;
			joystick_repeat_countdown[JOY_REPEAT_B3] = JOY_REPEAT_TIME;
			clear_input_events();
		}
		al_unlock_mutex(dpad_mutex);
	}
#endif
}

void dpad_on(bool count)
{
#if defned ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
	if (count)
		dpad_count--;
	if (dpad_count <= 0 || !count) {
		al_lock_mutex(dpad_mutex);
		use_dpad = dpad_type != DPAD_NONE;
		if (!joypad_connected()) {
			if (use_dpad)
				myTguiIgnore(TGUI_MOUSE);
			else
				myTguiIgnore(0);
		}
		al_unlock_mutex(dpad_mutex);
	}
#endif
}

void set_screen_params(void)
{
	ScreenDescriptor *sd = config.getWantedGraphicsMode();
	if (sd->fullscreen) {
#if !defined(IPHONE) && !defined(ALLEGRO_MACOSX)
		al_set_new_display_flags(al_get_new_display_flags() | ALLEGRO_FULLSCREEN_WINDOW);
#endif
		static ALLEGRO_MONITOR_INFO mi;
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
		static bool gotten = false;
		if (!gotten) {
			al_get_monitor_info(config.getAdapter(), &mi);
		}
#else
		al_get_monitor_info(config.getAdapter(), &mi);
#endif
		int w = mi.x2 - mi.x1;
		int h = mi.y2 - mi.y1;
		//printf("w=%d h=%d ssp\n", w, h);
		sd->real_width = w;
		sd->real_height = h;
		screenScale = (int)w / (int)BW;
		if (screenScale > ((int)h / (int)BH))
			screenScale = (int)h /(int)BH;
		if (config.getMaintainAspectRatio()) {
			screen_ratio_x = screen_ratio_y = 0;
			screen_offset_x = (w - screenScale*BW)/2;
			screen_offset_y = (h - screenScale*BH)/2;
		}
		else {
			screen_offset_x = screen_offset_y = 0;
			screen_ratio_x = w / (BW*screenScale);
			screen_ratio_y = h / (BH*screenScale);
		}
	}
	else {
		ScreenSize scr_sz = small_screen();

		if (scr_sz == ScreenSize_Tiny) {
			sd->real_width = 240;
			sd->real_height = 160;
			screenScale = 1;
		}
		else if (scr_sz == ScreenSize_Smaller) {
			sd->real_width = 480;
			sd->real_height = 320;
			screenScale = 2;
		}
		else if (scr_sz == ScreenSize_Small) {
			sd->real_width = 720;
			sd->real_height = 480;
			screenScale = 3;
		}
		else {
			sd->real_width = 960;
			sd->real_height = 640;
			screenScale = 4;
		}

		screen_ratio_x = screen_ratio_y = 1.0f;
		screen_offset_x = screen_offset_y = 0;
	}
}

void toggle_fullscreen(void)
{
	pause_joystick_repeat_events = true;
	ScreenDescriptor *sd = config.getWantedGraphicsMode();
	sd->fullscreen = !sd->fullscreen;
	al_set_display_flag(display, ALLEGRO_FULLSCREEN_WINDOW, config.getWantedGraphicsMode()->fullscreen);
	set_screen_params();
	tguiSetScreenSize(BW*screenScale, BH*screenScale);
	//if (config.getWantedGraphicsMode()->fullscreen) {
		config.setMaintainAspectRatio(config.getMaintainAspectRatio());
	//}
	tguiSetScale(screenScale, screenScale);
	pause_joystick_repeat_events = false;
}


extern "C" {
void connect_joypad(void);
void disconnect_joypad(void);
void lock_joypad_mutex(void);
void unlock_joypad_mutex(void);

void connect_joypad(void)
{
	connect_airplay_controls();
}

void disconnect_joypad(void)
{
	disconnect_airplay_controls();
}

void lock_joypad_mutex(void)
{
	al_lock_mutex(joypad_mutex);
}
void unlock_joypad_mutex(void)
{
	al_unlock_mutex(joypad_mutex);
}
}
