#if defined __APPLE__
#import <Foundation/Foundation.h>
#endif

#include "monster2.hpp"

#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID || defined ALLEGRO_RASPBERRYPI
#define OPENGLES
#endif

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

#ifdef ALLEGRO_ANDROID
#include "java.h"
#endif

/*
static bool screensaver_was_on = false;
int screensaver_idle_delay = 0;

void maybe_enable_screensaver()
{
#if defined __linux__ && !(defined ALLEGRO_ANDROID || defined ALLEGRO_RASPBERRYPI)
	system("gsettings set org.gnome.desktop.screensaver idle-activation-enabled true");
	system((std::string("gsettings set org.gnome.desktop.session idle-delay ") + my_itoa(screensaver_idle_delay)).c_str());
#endif
}
*/

uint32_t parse_version(const char *v)
{
	char buf1[100];
	char buf2[100];
	int i, j;

	for (i = 0; i < 99; i++) {
		if (!isdigit(v[i])) {
			break;
		}
		buf1[i] = v[i];
	}
	buf1[i++] = 0;

	for (j = 0; j < 99; j++) {
		if (!isdigit(v[i])) {
			break;
		}
		buf2[j] = v[i++];
	}
	buf2[j] = 0;

	printf("bufs=%s %s\n", buf1, buf2);

	return (atoi(buf1) << 16) | atoi(buf2);
}

#ifdef A5_D3D
LPDIRECT3DSURFACE9 big_depth_surface = NULL;

void init_big_depth_surface(void)
{
	LPDIRECT3DDEVICE9 dev = al_get_d3d_device(display);
	ALLEGRO_MONITOR_INFO mi;
	
	al_get_monitor_info(config.getAdapter(), &mi);
	int w = mi.x2 - mi.x1;
	int h = mi.y2 - mi.y1;
	int size = 1024;
	while (size < w || size < h)
		size = size * 2;

	dev->CreateDepthStencilSurface(
		size, size,
		D3DFMT_D16,
		D3DMULTISAMPLE_NONE, 0,
		true,
		&big_depth_surface,
		NULL
	);
	dev->SetDepthStencilSurface(big_depth_surface);
}

bool main_halted = false;
bool d3d_halted = false;
bool should_reset = false;

bool is_fs_toggle = false;

static void d3d_resource_release(void)
{
	if (!is_fs_toggle) {
		d3d_halted = true;
		should_reset = true;
		while (!main_halted) {
			al_rest(0.01);
		}
	}
}

static void d3d_resource_restore(void)
{
	if (!is_fs_toggle) {
		d3d_halted = false;
		while (main_halted) {
			al_rest(0.01);
		}
	}
}
#endif

int width_before_fullscreen = 0;
int height_before_fullscreen = 0;
int config_save_width = 0;
int config_save_height = 0;

bool have_mouse;

#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
bool do_pause_game = false;
#endif
ALLEGRO_EVENT_QUEUE *events;
ALLEGRO_EVENT_QUEUE *input_event_queue;
double next_shake = 0;

bool do_acknowledge_resize = false;

ALLEGRO_JOYSTICK *user_joystick = NULL;
bool is_intel_gpu_on_desktop_linux = false;
#ifdef ALLEGRO_ANDROID
bool is_android_lessthan_2_3;
#endif

bool achievement_show = false;
double achievement_time = 0;
MBITMAP *achievement_bmp;
#if defined A5_D3D //|| defined KINDLEFIRE
int PRESERVE_TEXTURE = 0;
int NO_PRESERVE_TEXTURE = ALLEGRO_NO_PRESERVE_TEXTURE;
#else
int PRESERVE_TEXTURE = ALLEGRO_NO_PRESERVE_TEXTURE;
int NO_PRESERVE_TEXTURE = ALLEGRO_NO_PRESERVE_TEXTURE;
#endif
bool reload_translation = false;
static std::string replayMusicName = "";

bool do_toggle_fullscreen = false;

// cheats
bool superpower = false, healall = false;

static bool pause_joystick_repeat_events = false;
bool pause_f_to_toggle_fullscreen = false;

bool use_digital_joystick = false;
int screen_offset_x, screen_offset_y;
float screen_ratio_x, screen_ratio_y;
#ifdef ALLEGRO_IPHONE
double allegro_iphone_shaken = DBL_MIN;
#endif
float initial_screen_scale = 0.0f;
bool sound_was_playing_at_program_start;
ALLEGRO_DISPLAY *display = 0;
ALLEGRO_DISPLAY *controller_display = 0;
ALLEGRO_COND *wait_cond;
ALLEGRO_MUTEX *wait_mutex;
ALLEGRO_MUTEX *joypad_mutex;
int exit_event_thread = 0;
ALLEGRO_SHADER *default_shader;
ALLEGRO_SHADER *cheap_shader;
ALLEGRO_SHADER *tinter;
ALLEGRO_SHADER *warp;
ALLEGRO_SHADER *shadow_shader;
ALLEGRO_SHADER *brighten;
ALLEGRO_SHADER *scale2x;
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
GuiAnims guiAnims;
XMLData *terrain;
std::vector<WaterData> waterData;
float screenScaleX = 2;
float screenScaleY = 2;
bool use_programmable_pipeline = true;

#ifdef ALLEGRO_IPHONE
bool create_airplay_mirror = false;
bool delete_airplay_mirror = false;
bool airplay_connected = false;
#endif

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
int click_x, click_y;
int event_mouse_x, event_mouse_y;
int current_mouse_x = -1, current_mouse_y = -1;
ALLEGRO_MUTEX *click_mutex;
ALLEGRO_MUTEX *input_mutex;
ALLEGRO_MUTEX *dpad_mutex;
ALLEGRO_MUTEX *touch_mutex;
float area_panned_x = 0;
float area_panned_y = 0;
static AnimationSet *eny_loader;
static AnimationSet *dot_loader;
static MBITMAP *bg_loader, *loading_loader, *bar_loader;
MBITMAP *corner_bmp;
bool had_battle = false;
MBITMAP *shadow_sheet;
MBITMAP *dpad_buttons;
bool onscreen_swipe_to_attack;
bool onscreen_drag_to_use;
bool reinstall_timer;
ALLEGRO_TIMER *draw_timer;
ALLEGRO_TIMER *logic_timer;
bool drew_shadow = false;
MBITMAP *batteryIcon;
bool global_draw_red = true;
bool red_off_press_on = false;
volatile bool loading_done = false;
MBITMAP *stomach_circle;

bool scr_tiny = false;
bool scr_smaller = false;
bool scr_small = false;
bool scr_medium = false;

char versionString[10];
int versionMajor = 1;
int versionMinor = 0;

bool switched_out = false;
ALLEGRO_MUTEX *switch_mutex;
ALLEGRO_COND *switch_cond;
uint32_t my_opengl_version;

MBITMAP *custom_mouse_cursor = NULL;
bool show_custom_mouse_cursor = true;

void destroy_fonts(void)
{
	m_destroy_font(game_font);
	m_destroy_font(medium_font);
	m_destroy_font(huge_font);
}

void load_fonts(void)
{
	int ttf_flags;

	ttf_flags = ALLEGRO_TTF_MONOCHROME;

	ALLEGRO_DEBUG("loading fonts");

#ifdef ALLEGRO_ANDROIDXXX
	ALLEGRO_PATH *res_dir = al_get_standard_path(ALLEGRO_RESOURCES_PATH);
	char boofer[1000];
	sprintf(boofer, "%s/unpack/DejaVuSans.ttf", al_path_cstr(res_dir, '/'));
	al_destroy_path(res_dir);
	al_set_standard_file_interface();
	game_font = al_load_ttf_font("/mnt/sdcard/removable_sdcard/DejaVuSans.ttf", 9, ttf_flags);
	al_android_set_apk_file_interface();
	if (!game_font) {
		native_error("Failed to load game_font.");
	}
#else
	game_font = al_load_ttf_font(getResource("DejaVuSans.ttf"), 9, ttf_flags);
	if (!game_font) {
		native_error("Failed to load game_font.");
	}
#endif

	medium_font = al_load_ttf_font(getResource("DejaVuSans.ttf"), 32, 0);
	if (!medium_font) {
		native_error("Failed to load medium_font.");
	}
	
	huge_font = m_load_font(getResource("huge_font.tga"));
	if (!huge_font) {
		native_error("Failed to load huge_font.");
	}

	ALLEGRO_DEBUG("done loading fonts");
	
	// NOTE: This has to be after display creation and loading of fonts
	load_translation(get_language_name(config.getLanguage()).c_str());
}

void get_buffer_true_size(int *buffer_true_w, int *buffer_true_h)
{
#ifdef A5_OGL
	al_get_opengl_texture_size(buffer->bitmap, buffer_true_w, buffer_true_h);
#else
	al_get_d3d_texture_size(buffer->bitmap, buffer_true_w, buffer_true_h);
#endif
}

static void create_shadows(MBITMAP *bmp, RecreateData *data)
{
	(void)data;
	MBITMAP *shadow_corners[4]; // top left then clockwise
	MBITMAP *shadow_sides[4]; // top then clockwise

	ALLEGRO_BITMAP *__old_target__ = al_get_target_bitmap();
	m_save_blender();
	m_set_target_bitmap(bmp);
	m_clear(al_map_rgba(0, 0, 0, 0));

	shadow_sides[0] = m_create_sub_bitmap(bmp, 16*0, 0, 16, SHADOW_CORNER_SIZE);
	shadow_sides[1] = m_create_sub_bitmap(bmp, 16*1, 0, SHADOW_CORNER_SIZE, 16);
	shadow_sides[2] = m_create_sub_bitmap(bmp, 16*2, 0, 16, SHADOW_CORNER_SIZE);
	shadow_sides[3] = m_create_sub_bitmap(bmp, 16*3, 0, SHADOW_CORNER_SIZE, 16);
	
	for (int i = 0; i < 4; i++) {
		shadow_corners[i] = m_create_sub_bitmap(bmp, i*SHADOW_CORNER_SIZE, 16, SHADOW_CORNER_SIZE, SHADOW_CORNER_SIZE);
	}

	m_set_target_bitmap(shadow_corners[2]);
#ifdef ALLEGRO_ANDROID
	al_lock_bitmap(shadow_corners[2]->bitmap->parent, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_WRITEONLY);
#else
	m_lock_bitmap(shadow_corners[2], ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_WRITEONLY);
#endif
	for (int yy = 0; yy < SHADOW_CORNER_SIZE; yy++) {
		for (int xx = 0; xx < SHADOW_CORNER_SIZE; xx++) {
			int dx = xx;
			int dy = yy;
 			float dist = (float)sqrt((float)dx*dx + dy*dy) + 1;
			if (dist > SHADOW_CORNER_SIZE) dist = SHADOW_CORNER_SIZE;
			float a = (1.0f-(dist/SHADOW_CORNER_SIZE))*255;
			m_put_pixel(xx, yy, m_map_rgba(0, 0, 0, a));
		}
	}
#ifdef ALLEGRO_ANDROID
	al_unlock_bitmap(shadow_corners[2]->bitmap->parent);
#else
	m_unlock_bitmap(shadow_corners[2]);
#endif
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

	// Can't draw bitmaps to themselves except MEMORY or locked ones, so lock it

	m_set_target_bitmap(bmp);

	m_draw_bitmap_to_self(shadow_corners[2], 0, 16, M_FLIP_HORIZONTAL|M_FLIP_VERTICAL);
	m_draw_bitmap_to_self(shadow_corners[2], 10, 16, M_FLIP_VERTICAL);
	m_draw_bitmap_to_self(shadow_corners[2], 30, 16, M_FLIP_HORIZONTAL);
	m_draw_bitmap_region_to_self(shadow_corners[0], SHADOW_CORNER_SIZE-1, 0, 1, SHADOW_CORNER_SIZE, 0, 0, 0);
	m_draw_bitmap_region_to_self(shadow_corners[0], 0, SHADOW_CORNER_SIZE-1, SHADOW_CORNER_SIZE, 1, 48, 0, 0);
	m_draw_bitmap_to_self(shadow_sides[0], 32, 0, M_FLIP_VERTICAL);
	m_draw_bitmap_to_self(shadow_sides[3], 16, 0, M_FLIP_HORIZONTAL);
	
	m_restore_blender();
	al_set_target_bitmap(__old_target__);

	ALLEGRO_DEBUG("created shadows in callback thing");

	for (int i = 0; i < 4; i++) {
		m_destroy_bitmap(shadow_corners[i]);
		m_destroy_bitmap(shadow_sides[i]);
	}
}

static void destroy_shadows(MBITMAP *bmp)
{
	al_destroy_bitmap(bmp->bitmap);
}

ScreenSize small_screen(void)
{
	if (scr_tiny) {
		return ScreenSize_Tiny;
	}
	else if (scr_smaller) {
		return ScreenSize_Smaller;
	}
	else if (scr_small) {
		return ScreenSize_Small;
	}
	else if (scr_medium) {
		return ScreenSize_Medium;
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
#elif defined ALLEGRO_ANDROID
		value = true;
#else
		value = false;
#endif
	}
	
	return value;
}

static int progress_percent = 0;

static void show_progress(int percent)
{
	progress_percent = percent;
}

void register_display(ALLEGRO_DISPLAY *display)
{
	al_register_event_source(events, al_get_display_event_source(display));
	al_register_event_source(input_event_queue, al_get_display_event_source(display));
}

void set_user_joystick(void)
{
	int nj = al_get_num_joysticks();
	user_joystick = NULL;
	for (int k = 0; k < nj; k++) {
		ALLEGRO_JOYSTICK *j;
		j = al_get_joystick(k);
		int nb = al_get_joystick_num_buttons(j);
		if (nb > 0) {
			user_joystick = j;
			break;
		}
	}
}
	
bool f5_cheated = false, f6_cheated = false;
double f5_time = -1, f6_time = -1;


/* FIXME: Any new Effects have to be freed/restored here! */
static void *thread_proc(void *arg)
{
	events = al_create_event_queue();
	
	ALLEGRO_EVENT event;

#if defined A5_D3D || defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
	al_register_event_source(events, al_get_display_event_source(display));
#endif

	draw_timer = al_create_timer(1.0/config.getTargetFPS());
	logic_timer = al_create_timer(1.0/LOGIC_RATE);
	
	al_start_timer(draw_timer);
	al_start_timer(logic_timer);
	
	al_register_event_source(events, (ALLEGRO_EVENT_SOURCE *)draw_timer);
	al_register_event_source(events, (ALLEGRO_EVENT_SOURCE *)logic_timer);
	
#if defined __linux__ && !(defined ALLEGRO_ANDROID || defined ALLEGRO_RASPBERRYPI)
	double next_screensaver_disable = al_current_time() + 45;
#endif
	
	while  (exit_event_thread != 1) {
#if defined __linux__ && !(defined ALLEGRO_ANDROID || defined ALLEGRO_RASPBERRYPI)
		if (al_get_time() > next_screensaver_disable) {
			system("xscreensaver-command -deactivate >/dev/null 2>/dev/null");
			system("gnome-screensaver-command -d >/dev/null 2>/dev/null");
			next_screensaver_disable = al_get_time() + 45;
		}
#endif
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

#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_ANDROID
			if (event.type == ALLEGRO_EVENT_MOUSE_ENTER_DISPLAY) {
				show_custom_mouse_cursor = true;
			}
			else if (event.type == ALLEGRO_EVENT_MOUSE_LEAVE_DISPLAY) {
				show_custom_mouse_cursor = false;
			}
			else
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
		}
	}
	
	al_destroy_timer(draw_timer);
	al_destroy_timer(logic_timer);
	
	al_destroy_event_queue(events);
	
	exit_event_thread = 2;

	return NULL;
}

static void load_samples_cb(int curr, int total)
{
	show_progress((50*curr)/total);
}

static void *loader_proc(void *arg)
{
#if defined ALLEGRO_IPHONE || defined ALLEGRO_MACOSX
	POOL_BEGIN
#endif

#ifdef ALLEGRO_ANDROID
	al_android_set_apk_file_interface();
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
	
	config.setUseOnlyMemoryBitmaps(true);

	show_progress(55);

	MBITMAP *deter_display_access_bmp;
	deter_display_access_bmp = m_create_bitmap(16, 16); // check
	m_set_target_bitmap(deter_display_access_bmp);

	guiAnims.bitmap = m_load_bitmap(getResource("gui.png"));

	poison_bmp = m_load_alpha_bitmap(getResource("media/poison.png"), true);
	poison_bmp_tmp = m_create_alpha_bitmap( // check
		m_get_bitmap_width(poison_bmp)+10,
		m_get_bitmap_height(poison_bmp)+10);
	poison_bmp_tmp2 = m_create_alpha_bitmap( // check
		m_get_bitmap_width(poison_bmp)+10,
		m_get_bitmap_height(poison_bmp)+10);

	initInput();
	debug_message("Input initialized.\n");

	show_progress(60);

	// Set an icon
#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_MACOSX && !defined ALLEGRO_ANDROID
	MBITMAP *tmp_bmp = m_load_alpha_bitmap(getResource("staff.png"));
	al_set_display_icon(display, tmp_bmp->bitmap);
	m_destroy_bitmap(tmp_bmp);
#endif

	show_progress(65);

#ifdef ALLEGRO_IPHONE
	if (iPodIsPlaying()) {
		config.setMusicVolume(0);
		setMusicVolume(1);
	}
#endif

#ifdef ALLEGRO_IPHONE
	sound_was_playing_at_program_start = iPodIsPlaying();
#endif

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

	show_progress(75);
	debug_message("water data loaded\n");

	for (int i = 0; i < MAX_INVENTORY; i++) {
		inventory[i].index = -1;
		inventory[i].quantity = 0;
	}

	debug_message("inventory initialized\n");

	show_progress(80);

	debug_message("Loading miscellaneous graphics\n");

	show_progress(90);

	wait_cond = al_create_cond();
	wait_mutex = al_create_mutex();
	al_lock_mutex(wait_mutex);
	
	switch_cond = al_create_cond();
	switch_mutex = al_create_mutex();
	
	joypad_mutex = al_create_mutex_recursive();

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

	// FIXME
	al_run_detached_thread(thread_proc, NULL);
	m_rest(0.5);

	if (safemode) {
		notify("Desired mode could not be", "set. Using default", "(safe) mode...");
	}
	
#if defined ALLEGRO_IPHONE || defined ALLEGRO_MACOSX
	POOL_END
#endif

	config.setUseOnlyMemoryBitmaps(false);

	loading_done = true;

	m_destroy_bitmap(deter_display_access_bmp);
	
	show_progress(95);

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

	numTiles = (1024/16) * (1024/16);

	tilemap_data = new XMLData(getResource("tilemap.png.xml"));
	trans_data = new XMLData(getResource("trans.xml"));

	return true;
}

#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_ANDROID
#define LOWP ""
#else
#define LOWP "lowp"
#endif	

void init_shaders(void)
{
	if (use_programmable_pipeline) {
		static const char *main_vertex_source =
		"attribute vec4 pos;\n"
		"attribute vec4 color;\n"
		"attribute vec2 texcoord;\n"
		"uniform mat4 projview_matrix;\n"
		"uniform mat4 tex_matrix;\n"
                "uniform bool use_tex_matrix;\n"
		"varying vec4 varying_color;\n"
		"varying vec2 varying_texcoord;\n"
		"void main()\n"
		"{\n"
	 	"   varying_color = color;\n"
		"   if (use_tex_matrix) {\n"
		"     vec4 uv = tex_matrix * vec4(texcoord, 0, 1);\n"
		"     varying_texcoord = vec2(uv.x, uv.y);\n"
		"   }\n"
		"   else\n"
		"     varying_texcoord = texcoord;\n"
#ifndef __linux__
		"   gl_PointSize = 1.0;\n"
#endif
		"   gl_Position = projview_matrix * pos;\n"
		"}\n";
		
		static const char *warp_vertex_source =
		"attribute vec4 pos;\n"
		"attribute vec2 texcoord;\n"
		"uniform mat4 projview_matrix;\n"
		"uniform mat4 tex_matrix;\n"
                "uniform bool use_tex_matrix;\n"
		"varying vec2 varying_texcoord;\n"
		"void main()\n"
		"{\n"
		"   if (use_tex_matrix) {\n"
		"     vec4 uv = tex_matrix * vec4(texcoord, 0, 1);\n"
		"     varying_texcoord = vec2(uv.x, uv.y);\n"
		"   }\n"
		"   else\n"
		"     varying_texcoord = texcoord;\n"
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
		"uniform mat4 tex_matrix;\n"
                "uniform bool use_tex_matrix;\n"
		"varying vec4 varying_color;\n"
		"varying vec2 varying_texcoord;\n"
		"void main()\n"
		"{\n"
		"   varying_color = color;\n"
		"   if (use_tex_matrix) {\n"
		"     vec4 uv = tex_matrix * vec4(texcoord, 0, 1);\n"
		"     varying_texcoord = vec2(uv.x, uv.y);\n"
		"   }\n"
		"   else\n"
		"     varying_texcoord = texcoord;\n"
#ifndef __linux__
		"   gl_PointSize = 1.0;\n"
#endif
		"   gl_Position = projview_matrix * pos;\n"
		"}\n";
		
		static const char *scale2x_vertex_source =
		"attribute vec4 pos;\n"
		"attribute vec2 texcoord;\n"
		"uniform mat4 projview_matrix;\n"
		"uniform mat4 tex_matrix;\n"
                "uniform bool use_tex_matrix;\n"
		"uniform float hstep;\n"
		"uniform float vstep;\n"
		"varying vec2 varying_texcoord;\n"
		"varying vec2 tB;\n"
		"varying vec2 tD;\n"
		"varying vec2 tF;\n"
		"varying vec2 tH;\n"
		"void main()\n"
		"{\n"
		"   if (use_tex_matrix) {\n"
		"     vec4 uv = tex_matrix * vec4(texcoord, 0, 1);\n"
		"     varying_texcoord = vec2(uv.x, uv.y);\n"
		"   }\n"
		"   else\n"
		"     varying_texcoord = texcoord;\n"
		"   tB = vec2(texcoord.s,       texcoord.t+vstep);\n"
		"   tD = vec2(texcoord.s-hstep, texcoord.t);\n"
		"   tF = vec2(texcoord.s+hstep, texcoord.t);\n"
		"   tH = vec2(texcoord.s,       texcoord.t-vstep);\n"
#ifndef __linux__
		"   gl_PointSize = 1.0;\n"
#endif
		"   gl_Position = projview_matrix * pos;\n"
		"}\n";
		
		static const char *main_pixel_source =
#if defined OPENGLES
		"precision mediump float;\n"
#endif
		"uniform bool use_tex;\n"
		"uniform sampler2D tex;\n"
		"varying " LOWP " vec4 varying_color;\n"
		"varying vec2 varying_texcoord;\n"
		"void main()\n"
		"{\n"
		"  " LOWP " vec4 tmp = varying_color;\n"
		"  if (use_tex) {\n"
		"     vec4 coord = vec4(varying_texcoord, 0.0, 1.0);\n"
		"     " LOWP " vec4 sample = texture2D(tex, coord.st);\n"
		"     tmp *= sample;\n"
                "  }\n"
		"  gl_FragColor = tmp;\n"
		"}\n";
		
		static const char *cheap_pixel_source =
#if defined OPENGLES
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
#if defined OPENGLES
		"precision mediump float;\n"
#endif
		"uniform bool use_tex;\n"
		"uniform sampler2D tex;\n"
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
#if defined OPENGLES
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
#if defined OPENGLES
		"precision mediump float;\n"
#endif
		"varying vec4 varying_color;\n"
		"varying vec2 varying_texcoord;\n"
		"uniform float x1;\n"
		"uniform float y1;\n"
		"uniform float x2;\n"
		"uniform float y2;\n"
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
#if defined OPENGLES
		"precision mediump float;\n"
#endif
		"uniform sampler2D tex;\n"
		"varying vec4 varying_color;\n"
		"varying vec2 varying_texcoord;\n"
		"uniform float brightness;\n"
		"uniform bool use_tex;\n"
		"void main() {\n"
		"   vec4 coord = vec4(varying_texcoord, 0.0, 1.0);\n"
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
		
		const char *scale2x_pixel_source =
#if defined OPENGLES
		"precision mediump float;\n"
#endif
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
		"	if ((B.r != H.r || B.g != H.g || B.b != H.b || B.a != H.a) &&\n"
		"           (D.r != F.r || D.g != F.g || D.b != F.b || D.a != F.a)) {\n"
		"		" LOWP " float y = mod(gl_FragCoord.t-offset_x, 2.0);\n"
		"		" LOWP " float x = mod(gl_FragCoord.s-offset_y, 2.0);\n"
		"		if (x < 1.0 && y < 1.0) {\n"
		"			if (D.r == B.r && D.g == B.g && D.b == B.b && D.a == B.a) {\n"
		"				gl_FragColor = D;\n"
		"			}\n"
		"			else {\n"
		"				gl_FragColor = E;\n"
		"			}\n"
		"		}\n"
		"		else if (x < 1.0 && y >= 1.0) {\n"
		"			if (D.r == H.r && D.g == H.g && D.b == H.b && D.a == H.a) {\n"
		"				gl_FragColor = D;\n"
		"			}\n"
		"			else {\n"
		"				gl_FragColor = E;\n"
		"			}\n"
		"		}\n"
		"		else if (x >= 1.0 && y < 1.0) {\n"
		"			if (B.r == F.r && B.g == F.g && B.b == F.b && B.a == F.a) {\n"
		"				gl_FragColor = F;\n"
		"			}\n"
		"			else {\n"
		"				gl_FragColor = E;\n"
		"			}\n"
		"		}\n"
		"		else{\n"
		"			if (H.r == F.r && H.g == F.g && H.b == F.b && H.a == F.a) {\n"
		"				gl_FragColor = F;\n"
		"			}\n"
		"			else {\n"
		"				gl_FragColor = E;\n"
		"			}\n"
		"		}\n"
		"	}\n"
		"	else {\n"
		"		gl_FragColor = E;\n"
		"	}\n"
		"}\n";
		
		default_shader = al_create_shader(ALLEGRO_SHADER_GLSL);
		cheap_shader = al_create_shader(ALLEGRO_SHADER_GLSL);
		tinter = al_create_shader(ALLEGRO_SHADER_GLSL);
		warp = al_create_shader(ALLEGRO_SHADER_GLSL);
		shadow_shader = al_create_shader(ALLEGRO_SHADER_GLSL);
		brighten = al_create_shader(ALLEGRO_SHADER_GLSL);
		scale2x = al_create_shader(ALLEGRO_SHADER_GLSL);

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

#ifndef A5_D3D
		al_attach_shader_source(
					scale2x,
					ALLEGRO_VERTEX_SHADER,
					scale2x_vertex_source
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

#ifndef A5_D3D
		al_attach_shader_source(
					scale2x,
					ALLEGRO_PIXEL_SHADER,
					scale2x_pixel_source
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
#ifndef A5_D3D
		al_link_shader(scale2x);
		if ((shader_log = al_get_shader_log(scale2x))[0] != 0) {
			printf("7. %s\n", shader_log);
		}
#endif
		
#ifdef A5_OGL
		al_set_opengl_program_object(display, al_get_opengl_program_object(default_shader));
		ALLEGRO_TRANSFORM tr;
		al_identity_transform(&tr);
		al_use_transform(&tr);
		al_ortho_transform(&tr, 0, al_get_display_width(display), al_get_display_height(display), 0, -1, 1);
		al_set_projection_transform(display, &tr);
#else
#ifndef A5_D3D
		al_set_direct3d_effect(display, al_get_direct3d_effect(default_shader));
#endif

#endif
		
	}
}

void init2_shaders(void)
{
	if (use_programmable_pipeline) {
		int buffer_true_w, buffer_true_h;
		get_buffer_true_size(&buffer_true_w, &buffer_true_h);

		al_set_shader_float(scale2x, "hstep", 1.0/buffer_true_w);
		al_set_shader_float(scale2x, "vstep", 1.0/buffer_true_h);
		al_set_shader_float(scale2x, "offset_x", 1);
		al_set_shader_float(scale2x, "offset_y", 0);
		al_set_shader_sampler(scale2x, "tex", scaleXX_buffer->bitmap, 0);
		
		al_set_shader_bool(default_shader, "use_tex_matrix", false);
		al_set_shader_bool(cheap_shader, "use_tex_matrix", false);
		al_set_shader_bool(tinter, "use_tex_matrix", false);
		al_set_shader_bool(warp, "use_tex_matrix", false);
		al_set_shader_bool(shadow_shader, "use_tex_matrix", false);
		al_set_shader_bool(brighten, "use_tex_matrix", false);
		al_set_shader_bool(scale2x, "use_tex_matrix", false);
		
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
		al_destroy_shader(scale2x);
	}	
}

static void draw_loading_screen(MBITMAP *tmp, int percent, ScreenDescriptor *sd)
{
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
	m_draw_scaled_bitmap(tmp, 0, 0, BH, BW, 0, 0, (float)sd->height/BW*BH, sd->height, 0);
	m_flip_display();
}

void create_buffers(void)
{
	if (buffer)
		m_destroy_bitmap(buffer);
	if (overlay)
		m_destroy_bitmap(overlay);
	int flags = al_get_new_bitmap_flags();
	if (config.getFilterType() == FILTER_LINEAR) {
		al_set_new_bitmap_flags(flags | ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR | NO_PRESERVE_TEXTURE);
	}
	else
		al_set_new_bitmap_flags(flags | NO_PRESERVE_TEXTURE);
	buffer = m_create_bitmap(BW, BH); // check
	overlay = m_create_bitmap(BW, BH); // check
	al_set_new_bitmap_flags(flags);
}

// FIXME FIXME FIXME FIXME FIXME
#ifdef ALLEGRO_ANDROID
int my_crap_atexit(void (*crap)(void))
{
	return 0;
}

void android_assert_handler(char const *expr,
	char const *file, int line, char const *func)
{
	ALLEGRO_DEBUG("BOOYA %s %s:%d", func, file, line);
}
#endif

#ifdef ALLEGRO_RASPBERRYPI
static void _al_raspberrypi_get_screen_info(int *dx, int *dy,
   int *screen_width, int *screen_height)
{
   graphics_get_display_size(0 /* LCD */, (uint32_t *)screen_width, (uint32_t *)screen_height);

   /* On TV-out the visible area (area used by X and console)
    * is different from that reported by the bcm functions. We
    * have to 1) read fbwidth and fbheight from /proc/cmdline
    * and also overscan parameters from /boot/config.txt so our
    * displays are the same size and overlap perfectly.
    */
   *dx = 0;
   *dy = 0;
   FILE *cmdline = fopen("/proc/cmdline", "r");
   if (cmdline) {
      char line[1000];
      int i;
      for (i = 0; i < 999; i++) {
         int c = fgetc(cmdline);
         if (c == EOF) break;
         line[i] = c;
      }
      line[i] = 0;
      const char *p = strstr(line, "fbwidth=");
      if (p) {
         const char *p2 = strstr(line, "fbheight=");
         if (p2) {
            p += strlen("fbwidth=");
            p2 += strlen("fbheight=");
            int w = atoi(p);
            int h = atoi(p2);
            ALLEGRO_CONFIG *cfg = al_load_config_file("/boot/config.txt");
            if (cfg) {
               const char *disable_overscan =
                  al_get_config_value(
                     cfg, "", "disable_overscan"
                  );
               // If overscan parameters are disabled don't process
               if (!disable_overscan ||
                   (disable_overscan &&
                    (!strcasecmp(disable_overscan, "false") ||
                     atoi(disable_overscan) == 0))) {
                  const char *left = al_get_config_value(
                     cfg, "", "overscan_left");
                  const char *right = al_get_config_value(
                     cfg, "", "overscan_right");
                  const char *top = al_get_config_value(
                     cfg, "", "overscan_top");
                  const char *bottom = al_get_config_value(
                     cfg, "", "overscan_bottom");
                  int xx = left ? atoi(left) : 0;
                  xx -= right ? atoi(right) : 0;
                  int yy = top ? atoi(top) : 0;
                  yy -= bottom ? atoi(bottom) : 0;
                  *dx = (*screen_width - w + xx) / 2;
                  *dy = (*screen_height - h + yy) / 2;
                  *screen_width = w;
                  *screen_height = h;
               }
               else {
                  *dx = (*screen_width - w) / 2;
                  *dy = (*screen_height - h) / 2;
                  *screen_width = w;
                  *screen_height = h;
               }
               al_destroy_config(cfg);
            }
            else {
               printf("Couldn't open /boot/config.txt\n");
            }
         }
      }
      fclose(cmdline);
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
	
	bool use_fixed_pipeline = false;

#ifndef ALLEGRO_ANDROID
	if (check_arg(*argc, *argv, "-tiny") > 0)
		scr_tiny = true;
	else if (check_arg(*argc, *argv, "-smaller") > 0)
		scr_smaller = true;
	else if (check_arg(*argc, *argv, "-small") > 0)
		scr_small = true;
	else if (check_arg(*argc, *argv, "-medium") > 0)
		scr_medium = true;

	if (check_arg(*argc, *argv, "-fixed-pipeline") > 0)
		use_fixed_pipeline = true;
#endif

#ifdef ALLEGRO_ANDROID
	al_install_system(ALLEGRO_VERSION_INT, NULL);
#else
	if (!al_init()) {
		printf("al_init failed.\n");
		exit(1);
	}
#endif

#if !defined ALLEGRO_IPHONE
	al_install_keyboard();
#endif

#ifdef ALLEGRO_ANDROID
	al_register_assert_handler(android_assert_handler);
#endif

#if !defined ALLEGRO_IPHONE
	al_set_org_name("Nooskewl");
	al_set_app_name("Monster RPG 2");
#endif

	debug_message("after set_app/org_name");

	// must be before al_android_set_apk_file_interface
	bool config_read = false;
	try {
		config_read = config.read();
	}
	catch (ReadError e) {
		(void)e;
	}

	debug_message("config read");

#ifdef ALLEGRO_ANDROID
	al_android_set_apk_file_interface();
#endif

	debug_message("apk interface set");

#if defined ALLEGRO_LINUX
	ALLEGRO_CONFIG *syscfg = al_get_system_config();
	al_set_config_value(syscfg, "graphics", "config_selection", "old");
#endif

	debug_message("set config value");

	input_event_mutex = al_create_mutex();

	debug_message("created input_event_mutex");

#ifdef ALLEGRO_IPHONE
	config.setAutoRotation(config.getAutoRotation());
#endif

	debug_message("setAutoRotation");

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


	have_mouse = al_install_mouse();
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
	have_mouse = false;
#endif
	
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
	al_install_touch_input();
	al_set_mouse_emulation_mode(ALLEGRO_MOUSE_EMULATION_5_0_x);
#endif

	ALLEGRO_FILE *f = al_fopen(getResource("Version"), "rb");
	if (f) {
		versionMajor = al_fread32le(f);
		versionMinor = al_fread32le(f);
		sprintf(versionString, "%d.%d", versionMajor, versionMinor);
		al_fclose(f);
	}
	else {
		strcpy(versionString, "-1");
	}

	al_init_font_addon();
	al_init_ttf_addon();
	al_init_image_addon();
	al_init_primitives_addon();
	
	int flags = 0;

#if defined A5_OGL
	flags |= ALLEGRO_OPENGL;
#endif

	al_set_new_display_flags(flags);

#ifdef ALLEGRO_ANDROID
	al_set_new_display_option(ALLEGRO_DEPTH_SIZE, 24, ALLEGRO_REQUIRE);
	al_set_new_display_option(ALLEGRO_STENCIL_SIZE, 0, ALLEGRO_REQUIRE);
	al_set_new_display_option(ALLEGRO_RED_SIZE, 8, ALLEGRO_REQUIRE);
	al_set_new_display_option(ALLEGRO_GREEN_SIZE, 8, ALLEGRO_REQUIRE);
	al_set_new_display_option(ALLEGRO_BLUE_SIZE, 8, ALLEGRO_REQUIRE);
	al_set_new_display_option(ALLEGRO_ALPHA_SIZE, 0, ALLEGRO_REQUIRE);
#else
#ifdef __linux__
	al_set_new_display_option(ALLEGRO_DEPTH_SIZE, 24, ALLEGRO_SUGGEST);
#else
	al_set_new_display_option(ALLEGRO_DEPTH_SIZE, 24, ALLEGRO_SUGGEST);
#endif
	al_set_new_display_option(ALLEGRO_COLOR_SIZE, 32, ALLEGRO_REQUIRE);
#endif

#if !defined(ALLEGRO_IPHONE) && !defined(ALLEGRO_ANDROID)
	al_set_new_display_adapter(config.getAdapter());
	
	ALLEGRO_MONITOR_INFO mi;
	al_get_monitor_info(config.getAdapter(), &mi);
	int w = mi.x2 - mi.x1;
	int h = mi.y2 - mi.y1;
#ifdef ALLEGRO_RASPBERRYPI
	sd->width = w;
	sd->height = h;
	config_save_width = w;
	config_save_height = h;
#else
	if (config_read) {
		config_save_width = sd->width;
		config_save_height = sd->height;
	}
	else {
		int i = 1;
		config_save_width = 240;
		config_save_height = 160;
		const float factor = 0.8f;
		while (1) {
			int this_w = 240*i;
			int this_h = 160*i;
			if (this_w > (w*0.8f) || this_h > (h*0.8f)) {
				break;
			}
			config_save_width = this_w;
			config_save_height = this_h;
			sd->width = this_w;
			sd->height = this_h;
			i++;
		}
	}
#endif
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
	else if (scr_sz == ScreenSize_Medium) {
		sd->width = 960;
		sd->height = 640;
	}
#else
	#ifndef ALLEGRO_ANDROID
	// FIXME: do this for android when supporting multihead
	al_set_new_display_adapter(0);
	#endif

	sd->width = 1;
	sd->height = 1;

	config_save_width = 1;
	config_save_height = 1;
#endif

	if (config.getWaitForVsync())
		al_set_new_display_option(ALLEGRO_VSYNC, 1, ALLEGRO_SUGGEST);


	click_mutex = al_create_mutex();
	input_mutex = al_create_mutex();
	dpad_mutex = al_create_mutex();
	touch_mutex = al_create_mutex();

	// Android because it's very slow switching back in on some devices
#if defined A5_D3D || defined ALLEGRO_ANDROID || defined ALLEGRO_RASPBERRYPI
	use_fixed_pipeline = true;
#endif

	if (!use_fixed_pipeline) {
		al_set_new_display_flags(
			al_get_new_display_flags() |
			ALLEGRO_USE_PROGRAMMABLE_PIPELINE
		);
	}

#if defined ALLEGRO_IPHONE || defined KINDLEFIRE
	al_set_new_display_option(ALLEGRO_SUPPORTED_ORIENTATIONS, ALLEGRO_DISPLAY_ORIENTATION_LANDSCAPE, ALLEGRO_REQUIRE);
#elif defined ALLEGRO_ANDROID
	al_set_new_display_option(ALLEGRO_SUPPORTED_ORIENTATIONS, ALLEGRO_DISPLAY_ORIENTATION_270_DEGREES, ALLEGRO_REQUIRE);
#endif

#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
	al_set_new_display_flags(al_get_new_display_flags() | ALLEGRO_FULLSCREEN_WINDOW);
#endif

#ifdef EDITOR
	display = al_create_display(800, 500);
#else

	display = al_create_display(sd->width, sd->height);
#if !defined ALLEGRO_ANDROID && !defined ALLEGRO_IPHONE
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
#endif

#if defined ALLEGRO_ANDROID
	if (!display) {
		const int N = 7;
		const int S = 5;
		int totry[N][S] = {
			{ 24, 0, 5, 6, 5 },
			{ 24, 8, 5, 6, 5 },
			{ 24, 8, 8, 8, 8 },
			{ 16, 0, 5, 6, 5 },
			{ 16, 0, 8, 8, 8 },
			{ 16, 8, 5, 6, 5 },
			{ 16, 8, 8, 8, 8 }
		};
		for (int i = 0; i < N; i++) {
			al_set_new_display_option(ALLEGRO_DEPTH_SIZE, totry[i][0], ALLEGRO_REQUIRE);
			al_set_new_display_option(ALLEGRO_STENCIL_SIZE, totry[i][1], ALLEGRO_REQUIRE);
			al_set_new_display_option(ALLEGRO_RED_SIZE, totry[i][2], ALLEGRO_REQUIRE);
			al_set_new_display_option(ALLEGRO_GREEN_SIZE, totry[i][3], ALLEGRO_REQUIRE);
			al_set_new_display_option(ALLEGRO_BLUE_SIZE, totry[i][4], ALLEGRO_REQUIRE);
			al_set_new_display_option(ALLEGRO_ALPHA_SIZE, 0, ALLEGRO_REQUIRE);
			display = al_create_display(sd->width, sd->height);
			if (display) {
				break;
			}
		}
	}
#endif

#endif

	if (!display) {
		native_error("Failed to set gfx mode.");
	}

	al_rest(1.0);
	
#ifdef ALLEGRO_IPHONE
	al_init_user_event_source(&user_event_source);
#endif

	tguiInit();
	tguiSetRotation(0);

	al_hide_mouse_cursor(display);
	custom_mouse_cursor = m_load_bitmap(getResource("media/mouse_cursor.png"));

	if (have_mouse) {
		al_set_mouse_xy(display, al_get_display_width(display)-al_get_bitmap_width(custom_mouse_cursor->bitmap)-20, al_get_display_height(display)-al_get_bitmap_height(custom_mouse_cursor->bitmap)-20);
	}

	set_screen_params();
      
#ifdef A5_OGL
	glDisable(GL_DITHER);
#if defined __linux__ && !defined ALLEGRO_ANDROID
	is_intel_gpu_on_desktop_linux = strstr((const char *)glGetString(GL_RENDERER), "Intel");
#endif
#endif

#ifdef ALLEGRO_ANDROID
	uint32_t vers1 = parse_version(al_android_get_os_version());
	uint32_t vers2 = parse_version("2.3");
	if (vers1 < vers2) {
		is_android_lessthan_2_3 = true;
	}
#endif
	
	initSound();

	al_set_new_bitmap_flags(PRESERVE_TEXTURE | ALLEGRO_CONVERT_BITMAP);

#if defined ALLEGRO_IPHONE || defined ALLEGRO_MACOSX
	init_joypad();
#endif

#if defined ALLEGRO_IPHONE || defined ALLEGRO_MACOSX
	authenticatePlayer();
#endif

#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_ANDROID
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
		my_opengl_version = al_get_opengl_version();
	}
#else
	my_opengl_version = 0x0;
#endif

	if (use_fixed_pipeline) {
		use_programmable_pipeline = false;
	}
	else {
		use_programmable_pipeline = al_get_display_flags(display) & ALLEGRO_USE_PROGRAMMABLE_PIPELINE;
	}

	ALLEGRO_DEBUG("initing shaders");

	init_shaders();

	ALLEGRO_DEBUG("done initing shaders");

	if (al_install_joystick()) {
		set_user_joystick();
		if (user_joystick != NULL) {
			config.setGamepadAvailable(true);
		}
		else {
			config.setGamepadAvailable(false);
		}
	}
	else {
		config.setGamepadAvailable(false);
	}

	input_event_queue = al_create_event_queue();
	al_register_event_source(input_event_queue, (ALLEGRO_EVENT_SOURCE *)display);
#ifndef ALLEGRO_IPHONE
	al_register_event_source(input_event_queue, al_get_keyboard_event_source());
#ifndef ALLEGRO_ANDROID
	al_register_event_source(input_event_queue, al_get_joystick_event_source());
#endif
#endif
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
	al_register_event_source(input_event_queue, al_get_touch_input_event_source());
#else
	if (have_mouse) {
		al_register_event_source(input_event_queue, al_get_mouse_event_source());
	}
#endif

	debug_message("GFX mode set.\n");

	black = m_map_rgb(0, 0, 0);
	white = m_map_rgb(255, 255, 255);
	grey = m_map_rgb(200, 200, 200);
	blue = m_map_rgb(30, 0, 150);

#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_ANDROID
#ifdef A5_D3D
	init_big_depth_surface();
	al_d3d_set_restore_callback(d3d_resource_restore);
	al_d3d_set_release_callback(d3d_resource_release);
#endif

	m_clear(black);
	m_flip_display();
#endif
	
	al_set_window_title(display, "Monster RPG 2");

	m_set_blender(M_ONE, M_INVERSE_ALPHA, white);

#ifdef A5_OGL
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
	al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_RGBA_4444);
#else
	al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_ABGR_8888_LE);
#endif
#else
	al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_ARGB_8888);
#endif

	create_buffers();

	flags = al_get_new_bitmap_flags();
	al_set_new_bitmap_flags(flags | ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);
	screenshot = m_create_bitmap(BW/2, BH/2); // check
	al_set_new_bitmap_flags(flags);

	if (use_programmable_pipeline) {
		int flags = al_get_new_bitmap_flags();
		al_set_new_bitmap_flags(flags | NO_PRESERVE_TEXTURE);
		scaleXX_buffer = m_create_bitmap(BW*2, BH*2); // check
		al_set_new_bitmap_flags(flags);
	}

	init2_shaders();
	
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
	al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_RGBA_4444);
#elif defined A5_OGL
	al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_ABGR_8888_LE);
#else
	al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_ARGB_8888);
#endif
	
	if (!buffer) {
		native_error("Failed to create buffer.");
	}

	config.setFilterType(config.getFilterType());
	
	
	if (!screenshot) {
		native_error("Failed to create SS buffer.");
	}
	
	corner_bmp = m_load_bitmap(getResource("media/corner.png"));
	stomach_circle = m_load_bitmap(getResource("combat_media/stomach_circle.png"));
	if (!loadTilemap()) {
		return false;
	}

	eny_loader = new AnimationSet(getResource("media/eny-loader.png"));
	dot_loader = new AnimationSet(getResource("media/dot-loader.png"));
	bg_loader = m_load_bitmap(getResource("media/bg-loader.png"));
	bar_loader = m_load_bitmap(getResource("media/bar-loader.png"));
	loading_loader = m_load_bitmap(getResource("media/loading-loader.png"));

	int flogs = al_get_new_bitmap_flags();
	al_set_new_bitmap_flags(flogs | NO_PRESERVE_TEXTURE);
	MBITMAP *tmp = m_create_bitmap(BH, BW); // check
	al_set_new_bitmap_flags(flogs);

	/* ON IPHONE WE LOAD EVERYTHING AS MEMORY BITMAP FIRST THEN
	 * CONVERT THEM TO DISPLAY BITMAPS SO WE CAN SHOW A SMOOTH
	 * PROGRESS BAR */
	
	bool done_loading_samples = false;
	double start = al_get_time();

	if (cached_bitmap) {
		al_destroy_bitmap(cached_bitmap);
		cached_bitmap = NULL;
		cached_bitmap_filename = "";
	}

	while (1) {
		if (!done_loading_samples) {
			done_loading_samples = loadSamples(load_samples_cb);
			if (done_loading_samples) {
				al_run_detached_thread(loader_proc, NULL);
			}
		}
		
 		int percent = progress_percent;
		draw_loading_screen(tmp, percent, sd);
		if (percent >= 95) {
 			break;
 		}

		al_rest(0.016);

		double now = al_get_time();
		double elapsed = now - start;
		start = now;
		int millis = elapsed * 1000;
		eny_loader->update(millis);
		dot_loader->update(millis);
	}

	while (!loading_done) {
		m_rest(0.001);
	}

	// Must be first thing after thread end or before thread start
	if (cached_bitmap) {
		al_destroy_bitmap(cached_bitmap);
		cached_bitmap = NULL;
		cached_bitmap_filename = "";
	}

#if !defined(ALLEGRO_ANDROID)
	al_inhibit_screensaver(true);
#endif

#if defined ALLEGRO_IPHONE
	sb_start();
#endif

	al_set_new_bitmap_flags(ALLEGRO_CONVERT_BITMAP);

	shadow_sheet = m_create_alpha_bitmap(4*16, 2*16, create_shadows, NULL, destroy_shadows);
	draw_loading_screen(tmp, 100, sd);

	m_destroy_bitmap(tmp);
	delete eny_loader;
	delete dot_loader;
	m_destroy_bitmap(bg_loader);
	m_destroy_bitmap(bar_loader);
	m_destroy_bitmap(loading_loader);

	load_translation_tags();
	load_fonts();

	poison_bmp = m_make_alpha_display_bitmap(poison_bmp);
	poison_bmp_tmp = m_make_alpha_display_bitmap(poison_bmp_tmp);
	poison_bmp_tmp2 = m_make_alpha_display_bitmap(poison_bmp_tmp2);
	orb_bmp = m_make_alpha_display_bitmap(orb_bmp);
	guiAnims.bitmap = m_make_display_bitmap(guiAnims.bitmap);
	guiAnims.corner_sub = m_create_sub_bitmap(guiAnims.bitmap, 0, 0, 3, 3);
	guiAnims.wide_sub = m_create_sub_bitmap(guiAnims.bitmap, 0, 3, 32, 3);
	guiAnims.tall_sub = m_create_sub_bitmap(guiAnims.bitmap, 0, 6, 3, 32);

	al_set_new_bitmap_flags(PRESERVE_TEXTURE | ALLEGRO_CONVERT_BITMAP);
	
	cursor = m_load_bitmap(getResource("media/cursor.png"));
	tile = m_create_bitmap(TILE_SIZE, TILE_SIZE); // check
	stoneTexture = m_load_bitmap(getResource("combat_media/stone.png"));
	mushroom = m_load_bitmap(getResource("combat_media/mushroom.png"));
	webbed = m_load_bitmap(getResource("combat_media/webbed.png"));
	dpad_buttons = m_load_bitmap(getResource("media/buttons.png"));
	batteryIcon = m_load_bitmap(getResource("media/battery_icon.png"));
	achievement_bmp = m_load_bitmap(getResource("media/trophy.png"));

	debug_message("Loading icons\n");
	icon_bmp = m_load_bitmap_redraw(getResource("media/icons.png"), loadIcons, NULL);

	m_set_target_bitmap(buffer);
	
	inited = true;

	m_clear(black);

#ifdef ALLEGRO_ANDROID
	ALLEGRO_PATH *apkname = al_get_standard_path(ALLEGRO_EXENAME_PATH);
	debug_message("exename='%s'", al_path_cstr(apkname, '/'));
	if (!PHYSFS_init(al_path_cstr(apkname, '/'))) {
		return false;
	}
	PHYSFS_addToSearchPath(al_path_cstr(apkname, '/'), 1);
	al_destroy_path(apkname);
#else
	PHYSFS_init(*argv[0]);
#endif

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

	destroyTilemap();
	
	if (cached_bitmap) {
		al_destroy_bitmap(cached_bitmap);
		cached_bitmap = NULL;
		cached_bitmap_filename = "";
	}

	m_destroy_bitmap(buffer);
	m_destroy_bitmap(overlay);
	if (scaleXX_buffer) {
		m_destroy_bitmap(scaleXX_buffer);
	}

	m_destroy_bitmap(tile);
	destroyIcons();
	m_destroy_bitmap(stoneTexture);
	m_destroy_bitmap(mushroom);
	m_destroy_bitmap(webbed);
	m_destroy_bitmap(dpad_buttons);
	m_destroy_bitmap(batteryIcon);
	delete terrain;
	m_destroy_bitmap(cursor);
	m_destroy_bitmap(guiAnims.corner_sub);
	m_destroy_bitmap(guiAnims.wide_sub);
	m_destroy_bitmap(guiAnims.tall_sub);
	m_destroy_bitmap(guiAnims.bitmap);
	m_destroy_bitmap(orb_bmp);
	m_destroy_bitmap(poison_bmp);
	m_destroy_bitmap(poison_bmp_tmp);
	m_destroy_bitmap(poison_bmp_tmp2);
	m_destroy_bitmap(stomach_circle);
	if (screenshot) {
		m_destroy_bitmap(screenshot);
	}

	m_destroy_bitmap(corner_bmp);

	m_destroy_bitmap(shadow_sheet);

	destroy_fonts();

	for (int i = 0; i < MAX_PARTY; i++) {
		if (party[i]) {
			Object *o = party[i]->getObject();
			if (o)
				delete o;
			delete party[i];
			party[i] = NULL;
		}
	}

	if (area)
		delete area;

	#if !defined A5_D3D
	destroy_shaders();
	#endif

	#ifdef A5_D3D
	big_depth_surface->Release();
	#endif

	al_destroy_display(display);

	if (saveFilename)
		free(saveFilename);

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
#if defined ALLEGRO_IPHONE
		if (!joypad_connected() && !is_sb_connected()) {
#elif defined ALLEGRO_ANDROID
		if (!zeemote_connected) {
#else
		if (true) {
#endif
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
		}
		al_unlock_mutex(dpad_mutex);
	}
#endif
}

void dpad_on(bool count)
{
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
	if (count)
		dpad_count--;
	if (dpad_count <= 0 || !count) {
		al_lock_mutex(dpad_mutex);
		use_dpad = dpad_type != DPAD_NONE;
#if defined ALLEGRO_IPHONE
		if (!joypad_connected() && !is_sb_connected()) {
#elif defined ALLEGRO_ANDROID
		if (!zeemote_connected) {
#else
		if (true) {
#endif
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
	sd->width = al_get_display_width(display);
	sd->height = al_get_display_height(display);
	if (config.getMaintainAspectRatio() == ASPECT_FILL_SCREEN) {
		screenScaleX = (float)sd->width / BW;
		screenScaleY = (float)sd->height / BH;
	}
	else {
		float ratio;
		if (config.getMaintainAspectRatio() == ASPECT_INTEGER) {
			ratio = sd->width / BW;
			if (ratio > (sd->height / BH)) {
				ratio = sd->height / BH;
			}
		}
		else if (config.getMaintainAspectRatio() == ASPECT_MAINTAIN_RATIO) {
			ratio = (float)sd->width / BW;
			if (ratio > ((float)sd->height / BH)) {
				ratio = (float)sd->height / BH;
			}
		}
		screenScaleX = ratio;
		screenScaleY = ratio;
	}
	screen_offset_x = (sd->width - (screenScaleX*BW)) / 2;
	screen_offset_y = (sd->height - (screenScaleY*BH)) / 2;
	screen_ratio_x = sd->width / (screenScaleX*BW);
	screen_ratio_y = sd->height / (screenScaleY*BH);

	if (tguiIsInitialized()) {
		tguiSetScreenSize(screenScaleX*BW, screenScaleY*BH);

		if (config.getMaintainAspectRatio() != ASPECT_FILL_SCREEN) {
			tguiSetScreenParameters(screen_offset_x, screen_offset_y, 1.0f, 1.0f);
		}
		else {
			tguiSetScreenParameters(0, 0, screen_ratio_x, screen_ratio_y);
		}
#ifndef EDITOR
		tguiSetScale(screenScaleX, screenScaleY);
		tguiSetTolerance(3);
#endif
	}
}

void toggle_fullscreen(void)
{
#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_ANDROID
	show_custom_mouse_cursor = true;
#endif

	pause_joystick_repeat_events = true;
	ScreenDescriptor *sd = config.getWantedGraphicsMode();

	if (!sd->fullscreen) {
		width_before_fullscreen = sd->width;
		height_before_fullscreen = sd->height;
	}

	sd->fullscreen = !sd->fullscreen;
#ifdef A5_D3D
	is_fs_toggle = true;
	bool depth_surface_inited = big_depth_surface != NULL;
	if (depth_surface_inited) {
		big_depth_surface->Release();
	}
	_destroy_loaded_bitmaps();
#endif
	al_set_display_flag(display, ALLEGRO_FULLSCREEN_WINDOW, config.getWantedGraphicsMode()->fullscreen);
#ifdef A5_D3D
	_reload_loaded_bitmaps();
	_reload_loaded_bitmaps_delayed();
#endif
	set_screen_params();
#ifdef A5_D3D
	if (depth_surface_inited) {
		init_big_depth_surface();
	}
	shooter_restoring = true;
	is_fs_toggle = false;
#endif
	pause_joystick_repeat_events = false;

#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_ANDROID
	if (sd->fullscreen) {
		al_hide_mouse_cursor(display);
	}
	if (in_shooter) {
		al_set_mouse_xy(display, al_get_display_width(display)/2, al_get_display_height(display)/2);
	}
	else {
		al_set_mouse_xy(display, al_get_display_width(display)-al_get_bitmap_width(custom_mouse_cursor->bitmap)-20, al_get_display_height(display)-al_get_bitmap_height(custom_mouse_cursor->bitmap)-20);
	}
#endif
}

bool imperfect_aspect(void)
{
	if (
		((al_get_display_width(display)/BW) != ((float)al_get_display_width(display)/BW)) ||
		((al_get_display_height(display)/BH) != ((float)al_get_display_height(display)/BH))) {
		return true;
	}
	return false;
}

void connect_external_controls(void)
{
	connect_airplay_controls();
}

void disconnect_external_controls(void)
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
