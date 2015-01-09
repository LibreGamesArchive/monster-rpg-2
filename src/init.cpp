#if defined __APPLE__
#import <Foundation/Foundation.h>
#endif

#include "monster2.hpp"

#ifdef ALLEGRO_IPHONE
extern "C" {
#include <allegro5/allegro_iphone_objc.h>
}
#endif

#ifdef ALLEGRO_ANDROID
#include "java.h"
#endif

#ifdef ALLEGRO_RASPBERRYPI
#include <bcm_host.h>
#endif

#include <object_filenames.hpp>

void create_tmpbuffer()
{
	int flags = al_get_new_bitmap_flags();
#if defined ALLEGRO_IPHONE || defined ALLEGRO_RASPBERRYPI
	int format = al_get_new_bitmap_format();
	al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_RGB_565);
#endif
	al_set_new_bitmap_flags(flags | ALLEGRO_NO_PRESERVE_TEXTURE);
	int w = al_get_display_width(display);
	int h = al_get_display_height(display);
	tmpbuffer = m_create_bitmap(
		w, h
	);
	al_set_new_bitmap_flags(flags);
#if defined ALLEGRO_IPHONE
	al_set_new_bitmap_format(format);
#endif
}

#ifdef ALLEGRO_ANDROID
static uint32_t parse_version(const char *v)
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

	return (atoi(buf1) << 16) | atoi(buf2);
}
#endif

#ifdef ALLEGRO_WINDOWS
LPDIRECT3DSURFACE9 big_depth_surface = NULL;

static int pot(int n)
{
	int p = 1;
	while (p < n) {
		p = p * 2;
	}
	return p;
}

void init_big_depth_surface(void)
{
	LPDIRECT3DDEVICE9 dev = al_get_d3d_device(display);
	
	int w = al_get_display_width(display);
	int h = al_get_display_height(display);

	int max = (w > h) ? w : h;
	max = pot(max);
	dev->CreateDepthStencilSurface(
		max, max,
		D3DFMT_D24S8,
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

static void d3d_resource_release(ALLEGRO_DISPLAY *d)
{
	(void)d;
	if (!is_fs_toggle) {
		d3d_halted = true;
		should_reset = true;
		while (!main_halted) {
			al_rest(0.01);
		}
	}
}

static void d3d_resource_restore(ALLEGRO_DISPLAY *d)
{
	(void)d;
	if (!is_fs_toggle) {
		d3d_halted = false;
		while (main_halted) {
			al_rest(0.01);
		}
	}
}
#endif

ATLAS *object_atlas;
std::map<std::string, int> object_atlas_map;

int width_before_fullscreen = 0;
int height_before_fullscreen = 0;
int config_save_width = 0;
int config_save_height = 0;

#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
bool do_pause_game = false;
#endif
ALLEGRO_EVENT_QUEUE *events;
ALLEGRO_EVENT_QUEUE *input_event_queue;
double next_shake = 0;

bool do_acknowledge_resize = false;

ALLEGRO_JOYSTICK *user_joystick = NULL;
#ifdef ALLEGRO_ANDROID
bool is_android_lessthan_2_3 = false;
#endif

bool achievement_show = false;
double achievement_time = 0;
MBITMAP *achievement_bmp;
int PRESERVE_TEXTURE;
int NO_PRESERVE_TEXTURE;
bool reload_translation = false;
static std::string replayMusicName = "";

static bool have_mouse = false;

bool do_toggle_fullscreen = false;

// cheats
bool superpower = false, healall = false;

static bool pause_joystick_repeat_events = false;
bool pause_f_to_toggle_fullscreen = false;

bool use_digital_joystick = false;
int screen_offset_x, screen_offset_y;
float screen_ratio_x, screen_ratio_y;
#ifdef ALLEGRO_IPHONE
double allegro_iphone_shaken = 0;
#endif
bool sound_was_playing_at_program_start;
ALLEGRO_DISPLAY *display = 0;
ALLEGRO_COND *wait_cond;
ALLEGRO_MUTEX *wait_mutex;
ALLEGRO_MUTEX *ss_mutex;
int exit_event_thread = 0;
ALLEGRO_SHADER *tinter;
ALLEGRO_SHADER *warp;
ALLEGRO_SHADER *shadow_shader;
ALLEGRO_SHADER *brighten;
MBITMAP *screenshot = 0;
MBITMAP *tmpbuffer = 0;
MBITMAP *tilemap = 0;
bool *tileTransparent = 0;
int numTiles = 0;
MCOLOR white, black, grey, blue;
MBITMAP *tile;
char start_cwd[1000];
XMLData *tilemap_data = NULL;
XMLData *trans_data = NULL;
AnimationSet *guiAnims;
XMLData *terrain;
std::vector<WaterData> waterData;
float screenScaleX = 2;
float screenScaleY = 2;
bool use_programmable_pipeline = true;

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

bool use_dpad = false;

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
AnimationSet *shadow_sheet;
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

bool scr_smaller = false;
bool scr_small = false;
bool scr_medium = false;

bool switched_out = false;
uint32_t my_opengl_version;

MBITMAP *custom_cursor_bmp;
ALLEGRO_MOUSE_CURSOR *custom_mouse_cursor;
int custom_cursor_w, custom_cursor_h;

int ALPHA_FMT;

void destroy_fonts(void)
{
	m_destroy_font(game_font);
	m_destroy_font(medium_font);
	m_destroy_font(huge_font);
}

// Android is stupid and wants to compress ttfs unless you give them a different extension
ALLEGRO_FONT *my_load_ttf_font(const char *filename, int sz, int flags)
{
	ALLEGRO_FILE *f = al_fopen(filename, "rb");
	ALLEGRO_FONT *font = al_load_ttf_font_f(f, "", sz, flags);
	return font;
}

float textScaleX;
float textScaleY;

void load_fonts(void)
{
	int ttf_flags;

	ttf_flags = ALLEGRO_TTF_MONOCHROME;

	debug_message("loading fonts");

	if (screenScaleX > screenScaleY) {
		textScaleX = 2 * (screenScaleX/screenScaleY);
		textScaleY = 2;
	}
	else {
		textScaleX = 2;
		textScaleY = 2 * (screenScaleY/screenScaleX);
	}

	game_font = my_load_ttf_font(getResource("DejaVuSans.gif"), 9*(MIN(screenScaleX, screenScaleY)/2), ttf_flags);
	if (!game_font) {
		native_error("Failed to load game_font.");
	}
		
	medium_font = my_load_ttf_font(getResource("DejaVuSans.gif"), 32, ttf_flags);
	if (!medium_font) {
		native_error("Failed to load medium_font.");
	}

	huge_font = my_load_ttf_font(getResource("heavy.gif"), 24, ttf_flags);
	if (!huge_font) {
		native_error("Failed to load huge_font.");
	}

	debug_message("done loading fonts");
	
	// NOTE: This has to be after display creation and loading of fonts
	load_translation(get_language_name(config.getLanguage()).c_str());
}

ScreenSize small_screen(void)
{
	if (scr_smaller) {
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
#ifdef ALLEGRO_ANDROID
	use_dpad = false;
#endif

	int nj = al_get_num_joysticks();
	user_joystick = NULL;
	for (int k = 0; k < nj; k++) {
		ALLEGRO_JOYSTICK *j;
		j = al_get_joystick(k);
		int nb = al_get_joystick_num_buttons(j);
		if (nb > 0) {
			user_joystick = j;
#ifdef ALLEGRO_ANDROID
			use_dpad = true;
#endif
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

#if defined ALLEGRO_WINDOWS || defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
#ifdef defined ALLEGRO_WINDOWS
	if (al_get_display_flags(display) & ALLEGRO_DIRECT3D) {
#endif
		al_register_event_source(events, al_get_display_event_source(display));
#ifdef defined ALLEGRO_WINDOWS
	}
#endif
#endif

	al_register_event_source(events, (ALLEGRO_EVENT_SOURCE *)draw_timer);
	al_register_event_source(events, (ALLEGRO_EVENT_SOURCE *)logic_timer);
	
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

		al_wait_for_event(events, &event);
		if (event.type == ALLEGRO_EVENT_TIMER) {
			if (event.timer.source == draw_timer) {
				draw_counter++;
			}
			else if (event.timer.source == logic_timer) {
				logic_counter++;
				// Handle joystick repeat events
#if !defined ALLEGRO_IPHONE
				ALLEGRO_KEYBOARD_STATE state;
				my_get_keyboard_state(&state);

				bool joyb1 = false;
				bool joyb2 = false;
				bool joyb3 = false;
				if (config.getGamepadAvailable() && user_joystick != NULL) {
					ALLEGRO_JOYSTICK_STATE joystate;
					al_get_joystick_state(user_joystick, &joystate);
					if (joystate.button[config.getJoyButton1()]) {
						joyb1 = true;
					}
					if (joystate.button[config.getJoyButton2()]) {
						joyb2 = true;
					}
					if (joystate.button[config.getJoyButton3()]) {
						joyb3 = true;
					}
				}

				if (!joystick_repeat_started[JOY_REPEAT_B1]) {
					if (joyb1 || (is_modifier(config.getKey1()) && al_key_down(&state, config.getKey1()))) {
						joy_b1_down(true, modifier_repeat_count[0] == 0);
						modifier_repeat_count[0]++;
					}
				}
				if (!joystick_repeat_started[JOY_REPEAT_B2]) {
					if (joyb2 || (is_modifier(config.getKey2()) && al_key_down(&state, config.getKey2()))) {
						joy_b2_down(true, modifier_repeat_count[1] == 0);
						modifier_repeat_count[1]++;
					}
				}
				if (!joystick_repeat_started[JOY_REPEAT_B3]) {
					if (joyb3 || (is_modifier(config.getKey3()) && al_key_down(&state, config.getKey3()))) {
						joy_b3_down(true, modifier_repeat_count[2] == 0);
						modifier_repeat_count[2]++;
					}
				}
				if (!joystick_repeat_started[JOY_REPEAT_AXIS0]) {
					if (is_modifier(config.getKeyLeft()) && al_key_down(&state, config.getKeyLeft())) {
						joy_l_down(true, modifier_repeat_count[3] == 0);
						modifier_repeat_count[3]++;
					}
					if (is_modifier(config.getKeyRight()) && al_key_down(&state, config.getKeyRight())) {
						joy_r_down(true, modifier_repeat_count[4] == 0);
						modifier_repeat_count[4]++;
					}
				}
				if (!joystick_repeat_started[JOY_REPEAT_AXIS1]) {
					if (is_modifier(config.getKeyUp()) && al_key_down(&state, config.getKeyUp())) {
						joy_u_down(true, modifier_repeat_count[5] == 0);
						modifier_repeat_count[5]++;
					}
					if (is_modifier(config.getKeyDown()) && al_key_down(&state, config.getKeyDown())) {
						joy_d_down(true, modifier_repeat_count[6] == 0);
						modifier_repeat_count[6]++;
					}
				}
#endif

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
			}
			al_signal_cond(wait_cond);
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
	show_progress((75*curr)/total);
}

static void *loader_proc(void *arg)
{
#if defined ALLEGRO_IPHONE || defined ALLEGRO_MACOSX
	POOL_BEGIN
#endif

	al_set_physfs_file_interface();

#ifdef ALLEGRO_WINDOWS
	if (al_get_display_flags(display) & ALLEGRO_DIRECT3D) {
		al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_ARGB_8888);
	}
	else
#endif
	{
		al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_ABGR_8888_LE);
	}
	
	config.setUseOnlyMemoryBitmaps(true);

	show_progress(80);

	MBITMAP *deter_display_access_bmp;

	deter_display_access_bmp = m_create_bitmap(16, 16); // check
	m_set_target_bitmap(deter_display_access_bmp);

	poison_bmp = m_load_alpha_bitmap(getResource("media/poison.png"), true);
	poison_bmp_tmp = m_create_alpha_bitmap( // check
		m_get_bitmap_width(poison_bmp)+10,
		m_get_bitmap_height(poison_bmp)+10);
	poison_bmp_tmp2 = m_create_alpha_bitmap( // check
		m_get_bitmap_width(poison_bmp)+10,
		m_get_bitmap_height(poison_bmp)+10);

	initInput();
	debug_message("Input initialized.\n");

	show_progress(85);

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

	debug_message("water data loaded\n");

	for (int i = 0; i < MAX_INVENTORY; i++) {
		inventory[i].index = -1;
		inventory[i].quantity = 0;
	}

	debug_message("inventory initialized\n");

	show_progress(95);

	debug_message("Loading miscellaneous graphics\n");

	wait_cond = al_create_cond();
	wait_mutex = al_create_mutex();
	al_lock_mutex(wait_mutex);

	ss_mutex = al_create_mutex();
	
	m_push_target_bitmap();
	orb_bmp = m_create_alpha_bitmap(80, 80);
	m_set_target_bitmap(orb_bmp);
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
	m_pop_target_bitmap();

	// FIXME
	al_run_detached_thread(thread_proc, NULL);
	m_rest(0.5);

	if (safemode) {
		prepareForScreenGrab1();
		m_clear(black);
		prepareForScreenGrab2();
		notify("Desired mode could not be", "set. Using default", "(safe) mode...");
	}
	
#if defined ALLEGRO_IPHONE || defined ALLEGRO_MACOSX
	POOL_END
#endif

	config.setUseOnlyMemoryBitmaps(false);

	loading_done = true;

	al_set_target_bitmap(NULL);
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

#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_ANDROID && !defined ALLEGRO_RASPBERRYPI
#define LOWP ""
#else
#define LOWP "lowp"
#endif	

void init_shaders(void)
{
	if (al_get_display_flags(display) & ALLEGRO_OPENGL) {

	if (use_programmable_pipeline) {
		static const char *default_vertex_source =
		"attribute vec4 " ALLEGRO_SHADER_VAR_POS ";\n"
		"attribute vec4 " ALLEGRO_SHADER_VAR_COLOR ";\n"
		"attribute vec2 " ALLEGRO_SHADER_VAR_TEXCOORD ";\n"
		"uniform mat4 " ALLEGRO_SHADER_VAR_PROJVIEW_MATRIX ";\n"
		"uniform bool " ALLEGRO_SHADER_VAR_USE_TEX_MATRIX ";\n"
		"uniform mat4 " ALLEGRO_SHADER_VAR_TEX_MATRIX ";\n"
		"varying vec4 varying_color;\n"
		"varying vec2 varying_texcoord;\n"
		"void main()\n"
		"{\n"
		"   varying_color = " ALLEGRO_SHADER_VAR_COLOR ";\n"
		"   if (" ALLEGRO_SHADER_VAR_USE_TEX_MATRIX ") {\n"
		"     vec4 uv = " ALLEGRO_SHADER_VAR_TEX_MATRIX " * vec4(" ALLEGRO_SHADER_VAR_TEXCOORD ", 0, 1);\n"
		"     varying_texcoord = vec2(uv.x, uv.y);\n"
		"   }\n"
		"   else\n"
		"     varying_texcoord = " ALLEGRO_SHADER_VAR_TEXCOORD ";\n"
		"   gl_Position = " ALLEGRO_SHADER_VAR_PROJVIEW_MATRIX " * " ALLEGRO_SHADER_VAR_POS ";\n"
		"}\n";

		const char *tinter_pixel_source =
#if defined ALLEGRO_RASPBERRYPI
		"precision lowp float;\n"
#elif defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID || defined ALLEGRO_RASPBERRYPI
		"precision mediump float;\n"
#endif
		"uniform sampler2D " ALLEGRO_SHADER_VAR_TEX ";\n"
		"uniform bool " ALLEGRO_SHADER_VAR_USE_TEX ";\n"
		"varying vec4 varying_color;\n"
		"varying vec2 varying_texcoord;\n"
		"uniform float r;\n"
		"uniform float g;\n"
		"uniform float b;\n"
		"uniform float ratio;\n"
		"void main() {\n"
		"   float avg, dr, dg, db;\n"
		"   vec4 color = varying_color;\n"
		"   vec4 v = texture2D(" ALLEGRO_SHADER_VAR_TEX ", varying_texcoord);\n"
		"   if (" ALLEGRO_SHADER_VAR_USE_TEX ") {\n"
		"      color *= v;\n"
		"   }\n"
		"   avg = (color.r + color.g + color.b) / 3.0;\n"
		"   dr = avg * r;\n"
		"   dg = avg * g;\n"
		"   db = avg * b;\n"
		"   color.r = color.r - (ratio * (color.r - dr));\n"
		"   color.g = color.g - (ratio * (color.g - dg));\n"
		"   color.b = color.b - (ratio * (color.b - db));\n"
		"   gl_FragColor = color;\n"
		"}";
		
		const char *warp_pixel_source =
#if defined ALLEGRO_RASPBERRYPI
		"precision lowp float;\n"
#elif defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID || defined ALLEGRO_RASPBERRYPI
		"precision mediump float;\n"
#endif
		"uniform sampler2D " ALLEGRO_SHADER_VAR_TEX ";\n"
		"uniform bool " ALLEGRO_SHADER_VAR_USE_TEX ";\n"
		"varying vec4 varying_color;\n"
		"varying vec2 varying_texcoord;\n"
		"uniform float angle;\n"
		"uniform float tex_bot;\n"
		"uniform float alpha;\n"
		"void main() {\n"
		"   float mul;\n"
		"   if (angle > 3.14159) {\n"
		"      mul = 0.1 - (0.1 * (angle - 3.14159) / 3.14159);\n"
		"   }\n"
		"   else {\n"
		"      mul = 0.1 * angle / 3.14159;\n"
		"   }\n"
		"   float o = sin((angle+(varying_texcoord.x-0.5))*4.0)*mul + varying_texcoord.y;\n"
		"   if (o < 0.0 || o > tex_bot)\n"
		"      gl_FragColor = vec4(0, 0, 0, 1);\n"
		"   else {\n"
		"      vec4 c = texture2D(" ALLEGRO_SHADER_VAR_TEX ", vec2(varying_texcoord.x, o));\n"
		"      c.r *= alpha;\n"
		"      c.g *= alpha;\n"
		"      c.b *= alpha;\n"
		"      c.a = alpha;\n"
		"      gl_FragColor = c;\n"
		"   }\n"
		"}\n";
		
		const char *shadow_pixel_source =
#if defined ALLEGRO_RASPBERRYPI
		"precision lowp float;\n"
#elif defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID || defined ALLEGRO_RASPBERRYPI
		"precision mediump float;\n"
#endif
		"uniform float x1;\n"
		"uniform float y1;\n"
		"uniform float x2;\n"
		"uniform float y2;\n"
		"uniform float BH;\n"
		"uniform float BW;\n"
		"uniform float radius;\n"
		"void main() {\n"
		// rx, ry is the closest point to gl_FragCoord on the rectangle
		"   float rx, ry;\n"
		"   float dx, dy;\n"
		"   float frag_y = (BH-1.0) - gl_FragCoord.y;\n"
		"   rx = clamp(gl_FragCoord.x, x1, x2);\n"
		"   ry = clamp(frag_y, y1, y2);\n"
		"   dx = gl_FragCoord.x - rx;\n"
		"   dy = frag_y - ry;\n"
		"   float dist = clamp(sqrt(dx*dx + dy*dy), 0.0, radius);\n"
		"   float alpha = 1.0 - (dist / radius);\n"
		"   gl_FragColor = vec4(0.0, 0.0, 0.0, alpha);\n"
		"}\n";
		
		const char *brighten_pixel_source =
#if defined ALLEGRO_RASPBERRYPI
		"precision lowp float;\n"
#elif defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID || defined ALLEGRO_RASPBERRYPI
		"precision mediump float;\n"
#endif
		"uniform sampler2D " ALLEGRO_SHADER_VAR_TEX ";\n"
		"uniform bool " ALLEGRO_SHADER_VAR_USE_TEX ";\n"
		"varying vec4 varying_color;\n"
		"varying vec2 varying_texcoord;\n"
		"uniform float brightness;\n"
		"void main() {\n"
		"   vec4 coord = vec4(varying_texcoord, 0.0, 1.0);\n"
		"   vec4 color = varying_color;\n"
		"   if (" ALLEGRO_SHADER_VAR_USE_TEX ") {\n"
		"      color *= texture2D(" ALLEGRO_SHADER_VAR_TEX ", coord.st);\n"
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
		
		const char *shader_log;
		warp = al_create_shader(ALLEGRO_SHADER_GLSL);
		al_attach_shader_source(
			warp,
			ALLEGRO_VERTEX_SHADER,
			default_vertex_source
		);
		al_attach_shader_source(
			warp,
			ALLEGRO_PIXEL_SHADER,
			warp_pixel_source
		);
		al_build_shader(warp);
		if ((shader_log = al_get_shader_log(warp))[0] != 0) {
			printf("warp: %s\n", shader_log);
		}
		
		shadow_shader = al_create_shader(ALLEGRO_SHADER_GLSL);
		al_attach_shader_source(
			shadow_shader,
			ALLEGRO_VERTEX_SHADER,
			default_vertex_source
		);
		al_attach_shader_source(
			shadow_shader,
			ALLEGRO_PIXEL_SHADER,
			shadow_pixel_source
		);
		al_build_shader(shadow_shader);
		if ((shader_log = al_get_shader_log(shadow_shader))[0] != 0) {
			printf("shadow: %s\n", shader_log);
		}
		
		brighten = al_create_shader(ALLEGRO_SHADER_GLSL);
		al_attach_shader_source(
			brighten,
			ALLEGRO_VERTEX_SHADER,
			default_vertex_source
		);
		al_attach_shader_source(
			brighten,
			ALLEGRO_PIXEL_SHADER,
			brighten_pixel_source
		);
		al_build_shader(brighten);
		if ((shader_log = al_get_shader_log(brighten))[0] != 0) {
			printf("brighten: %s\n", shader_log);
		}
		
		tinter = al_create_shader(ALLEGRO_SHADER_GLSL);
		al_attach_shader_source(
			tinter,
			ALLEGRO_VERTEX_SHADER,
			default_vertex_source
		);
		al_attach_shader_source(
			tinter,
			ALLEGRO_PIXEL_SHADER,
			tinter_pixel_source
		);
		al_build_shader(tinter);
		if ((shader_log = al_get_shader_log(tinter))[0] != 0) {
			printf("tinter: %s\n", shader_log);
		}
	}

	}
#ifdef ALLEGRO_WINDOWS
	else {

	if (use_programmable_pipeline) {
		const char *default_vertex_source =
		"struct VS_INPUT\n"
		"{\n"
		"	float4 Position  : POSITION0;\n"
		"	float2 TexCoord  : TEXCOORD0;\n"
		"	float4 Color     : TEXCOORD1;\n"
		"};\n"
		"struct VS_OUTPUT\n"
		"{\n"
		"	float4 Position  : POSITION0;\n"
		"	float4 Color     : COLOR0;\n"
		"	float2 TexCoord  : TEXCOORD0;\n"
		"	float4 ClipPos   : TEXCOORD1;\n"
		"};\n"
		"float4x4 al_projview_matrix;\n"
		"VS_OUTPUT vs_main(VS_INPUT Input)\n"
		"{\n"
		"	VS_OUTPUT Output;\n"
		"	Output.Color = Input.Color;\n"
		"	Output.TexCoord = Input.TexCoord;\n"
		"	Output.Position = mul(Input.Position, al_projview_matrix);\n"
		"	Output.ClipPos = Output.Position;\n"
		"	return Output;\n"
		"}\n";
		
		const char *tinter_pixel_source =
		"texture al_tex;\n"
		"sampler2D s = sampler_state {\n"
		"	texture = <al_tex>;\n"
		"};\n"
		"bool al_use_tex;\n"
		"float r;\n"
		"float g;\n"
		"float b;\n"
		"float ratio;\n"
		"float4 ps_main(VS_OUTPUT Input) : COLOR0\n"
		"{\n"
		"   float avg, dr, dg, db;\n"
		"   float4 color = Input.Color;\n"
		"   if (" ALLEGRO_SHADER_VAR_USE_TEX ") {\n"
		"      color *= tex2D(s, Input.TexCoord);\n"
		"   }\n"
		"   avg = (color.r + color.g + color.b) / 3.0;\n"
		"   dr = avg * r;\n"
		"   dg = avg * g;\n"
		"   db = avg * b;\n"
		"   color.r = color.r - (ratio * (color.r - dr));\n"
		"   color.g = color.g - (ratio * (color.g - dg));\n"
		"   color.b = color.b - (ratio * (color.b - db));\n"
		"   return color;\n"
		"}";
		
		const char *warp_pixel_source =
		"texture al_tex;\n"
		"sampler2D s = sampler_state {\n"
		"	texture = <al_tex>;\n"
		"};\n"
		"float angle;\n"
		"float tex_bot;\n"
		"float alpha;\n"
		"float4 ps_main(VS_OUTPUT Input) : COLOR0\n"
		"{\n"
		"   float mul;\n"
		"   if (angle > 3.14159) {\n"
		"      mul = 0.1 - (0.1 * (angle - 3.14159) / 3.14159);\n"
		"   }\n"
		"   else {\n"
		"      mul = 0.1 * angle / 3.14159;\n"
		"   }\n"
		"   float o = sin((angle+(Input.TexCoord.x-0.5))*4.0)*mul + Input.TexCoord.y;\n"
		"   if (o < 0.0 || o > tex_bot)\n"
		"      return float4(0.0, 0.0, 0.0, 1.0);\n"
		"   else {\n"
		"      float4 c = tex2D(s, float2(Input.TexCoord.x, o));\n"
		"      c.r *= alpha;\n"
		"      c.g *= alpha;\n"
		"      c.b *= alpha;\n"
		"      c.a = alpha;\n"
		"      return c;\n"
		"   }\n"
		"}\n";
		
		const char *shadow_pixel_source =
		"float x1;\n"
		"float y1;\n"
		"float x2;\n"
		"float y2;\n"
		"float BH;\n"
		"float BW;\n"
		"float radius;\n"
		"float4 ps_main(VS_OUTPUT Input) : COLOR0\n"
		"{\n"
		// rx, ry is the closest point to gl_FragCoord on the rectangle
		"   float rx, ry;\n"
		"   float dx, dy;\n"
		"   float4 gl_FragCoord = Input.ClipPos / Input.ClipPos.w;\n"
		"   gl_FragCoord.x = (gl_FragCoord.x + 1.0) * (BW/2.0) + 0.5;\n"
		"   gl_FragCoord.y = BH - ((gl_FragCoord.y + 1.0) * (BH/2.0)) + 0.5;\n"
		"   rx = clamp(gl_FragCoord.x, x1, x2);\n"
		"   ry = clamp(gl_FragCoord.y, y1, y2);\n"
		"   dx = gl_FragCoord.x - rx;\n"
		"   dy = gl_FragCoord.y - ry;\n"
		"   float dist = clamp(sqrt(dx*dx + dy*dy), 0.0, radius);\n"
		"   float alpha = 1.0 - (dist / radius);\n"
		"   return float4(0.0, 0.0, 0.0, alpha);\n"
		"}\n";
		
		const char *brighten_pixel_source =
		"texture al_tex;\n"
		"sampler2D s = sampler_state {\n"
		"	texture = <al_tex>;\n"
		"};\n"
		"bool al_use_tex;\n"
		"float brightness;\n"
		"float4 ps_main(VS_OUTPUT Input) : COLOR0\n"
		"{\n"
		"   float4 coord = float4(Input.TexCoord, 0.0, 1.0);\n"
		"   float4 color = Input.Color;\n"
		"   if (" ALLEGRO_SHADER_VAR_USE_TEX ") {\n"
		"      color *= tex2D(s, coord.xy);\n"
		"   }\n"
		"   if (color.a == 0.0) {\n"
		"      return float4(0.0, 0.0, 0.0, 0.0);\n"
		"   }\n"
		"   else {\n"
		"      float src_factor = 1.0 - brightness;\n"
		"      color.r *= src_factor;\n"
		"      color.r = color.r + brightness;\n"
		"      color.g *= src_factor;\n"
		"      color.g = color.g + brightness;\n"
		"      color.b *= src_factor;\n"
		"      color.b = color.b + brightness;\n"
		"      return color;\n"
		"   }\n"
		"}";

		const char *shader_log;
		warp = al_create_shader(ALLEGRO_SHADER_HLSL);
		al_attach_shader_source(
			warp,
			ALLEGRO_VERTEX_SHADER,
			default_vertex_source
		);
		al_attach_shader_source(
			warp,
			ALLEGRO_PIXEL_SHADER,
			warp_pixel_source
		);
		al_build_shader(warp);
		if ((shader_log = al_get_shader_log(warp))[0] != 0) {
			printf("warp: %s\n", shader_log);
		}
		
		shadow_shader = al_create_shader(ALLEGRO_SHADER_HLSL);
		al_attach_shader_source(
			shadow_shader,
			ALLEGRO_VERTEX_SHADER,
			default_vertex_source
		);
		al_attach_shader_source(
			shadow_shader,
			ALLEGRO_PIXEL_SHADER,
			shadow_pixel_source
		);
		al_build_shader(shadow_shader);
		if ((shader_log = al_get_shader_log(shadow_shader))[0] != 0) {
			printf("shadow: %s\n", shader_log);
		}
		
		brighten = al_create_shader(ALLEGRO_SHADER_HLSL);
		al_attach_shader_source(
			brighten,
			ALLEGRO_VERTEX_SHADER,
			al_get_default_shader_source(ALLEGRO_SHADER_HLSL, ALLEGRO_VERTEX_SHADER)
		);
		al_attach_shader_source(
			brighten,
			ALLEGRO_PIXEL_SHADER,
			brighten_pixel_source
		);
		al_build_shader(brighten);
		if ((shader_log = al_get_shader_log(brighten))[0] != 0) {
			printf("brighten: %s\n", shader_log);
		}
		
		tinter = al_create_shader(ALLEGRO_SHADER_HLSL);
		al_attach_shader_source(
			tinter,
			ALLEGRO_VERTEX_SHADER,
			al_get_default_shader_source(ALLEGRO_SHADER_HLSL, ALLEGRO_VERTEX_SHADER)
		);
		al_attach_shader_source(
			tinter,
			ALLEGRO_PIXEL_SHADER,
			tinter_pixel_source
		);
		al_build_shader(tinter);
		if ((shader_log = al_get_shader_log(tinter))[0] != 0) {
			printf("tinter: %s\n", shader_log);
		}
	}

	}
#endif
}

void destroy_shaders(void)
{
	if (use_programmable_pipeline) {
		al_destroy_shader(tinter);
		al_destroy_shader(brighten);
		al_destroy_shader(warp);
		al_destroy_shader(shadow_shader);
	}
}

static void draw_loading_screen(MBITMAP *tmp, int percent, ScreenDescriptor *sd)
{
	m_set_target_bitmap(tmp);
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
	set_target_backbuffer();
	m_clear(black);
	m_draw_scaled_bitmap(tmp, 0, 0, BH, BW, 0, 0, BH*BH/BW, BH, 0);
	m_flip_display();
}

#ifndef ALLEGRO_ANDROID
bool isAndroidConsole()
{
	return false;
}
bool gamepadConnected()
{
	bool ret = false;

	if (al_is_joystick_installed() && al_get_num_joysticks() > 1) {
		ret = true;
	}

	return ret;
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

#if !defined ALLEGRO_ANDROID && !defined ALLEGRO_RASPBERRYPI
	if (check_arg(*argc, *argv, "-smaller") > 0)
		scr_smaller = true;
	else if (check_arg(*argc, *argv, "-small") > 0)
		scr_small = true;
	else if (check_arg(*argc, *argv, "-medium") > 0)
		scr_medium = true;

	if (check_arg(*argc, *argv, "-fixed-pipeline") > 0)
		use_fixed_pipeline = true;
#endif

	if (!al_init()) {
		printf("al_init failed.\n");
		exit(1);
	}

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

#if !defined ALLEGRO_IPHONE
	for (int i = 0; i < 10; i++) {
		if (al_install_keyboard()) {
			break;
		}
		al_rest(0.25);
	}
#endif

#if !defined ALLEGRO_IPHONE
	al_set_org_name("Nooskewl");
	al_set_app_name("Monster RPG 2");
#endif

	debug_message("after set_app/org_name");

	// must be before al_set_physfs_file_interface
	bool config_read = false;
	try {
		config_read = config.read();
	}
	catch (ReadError e) {
		(void)e;
	}

	debug_message("config read");

#if defined ALLEGRO_LINUX
	ALLEGRO_CONFIG *syscfg = al_get_system_config();
	al_set_config_value(syscfg, "graphics", "config_selection", "old");
#endif

	debug_message("set config value");

	input_event_mutex = al_create_mutex();

	debug_message("created input_event_mutex");

#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
	dpad_type = config.getDpadType();
#else
	dpad_type = DPAD_TOTAL_1;
#endif

	debug_message("After al_init\n");

	debug_message("After config.read\n");

	ScreenDescriptor *sd = config.getWantedGraphicsMode();

	// FIXME: check return
	debug_message("al_init success\n");


#if defined ALLEGRO_RASPBERRYPI
	if (!al_filename_exists("/dev/input/mouse0")) {
		have_mouse = false;
	}
	else
#endif

	have_mouse = al_install_mouse();

#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
	al_install_touch_input();
	al_set_mouse_emulation_mode(ALLEGRO_MOUSE_EMULATION_5_0_x);
#endif

	al_init_font_addon();
	al_init_ttf_addon();
	al_init_image_addon();
	al_init_primitives_addon();
	
	int flags = 0;

#ifdef ALLEGRO_WINDOWS
	if (check_arg(*argc, *argv, "-opengl") > 0) {
#endif
		flags |= ALLEGRO_OPENGL;
#ifdef ALLEGRO_WINDOWS
	}
#endif

	al_set_new_display_flags(flags);

/*
#if defined ALLEGRO_ANDROID
	al_set_new_display_option(ALLEGRO_DEPTH_SIZE, 16, ALLEGRO_REQUIRE);
	al_set_new_display_option(ALLEGRO_STENCIL_SIZE, 8, ALLEGRO_REQUIRE);
	al_set_new_display_option(ALLEGRO_COLOR_SIZE, 16, ALLEGRO_REQUIRE);
#else
*/
	al_set_new_display_option(ALLEGRO_DEPTH_SIZE, 24, ALLEGRO_SUGGEST);
	al_set_new_display_option(ALLEGRO_STENCIL_SIZE, 8, ALLEGRO_REQUIRE);
	al_set_new_display_option(ALLEGRO_COLOR_SIZE, 32, ALLEGRO_REQUIRE);
//#endif

#if !defined(ALLEGRO_IPHONE) && !defined(ALLEGRO_ANDROID)
	al_set_new_display_adapter(config.getAdapter());
	
	ALLEGRO_MONITOR_INFO mi;
	al_get_monitor_info(config.getAdapter(), &mi);
	int w = mi.x2 - mi.x1;
	int h = mi.y2 - mi.y1;
#ifdef ALLEGRO_RASPBERRYPI
	w = 1280;
	h = 720;
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
	(void)config_read;
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

	if (!use_fixed_pipeline) {
		al_set_new_display_flags(
			al_get_new_display_flags() |
			ALLEGRO_PROGRAMMABLE_PIPELINE
		);
	}

#if defined ALLEGRO_IPHONE
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
#endif

	if (!display) {
		config.write();
		exit(1);
	}

#ifdef ALLEGRO_WINDOWS
	if (al_get_display_flags(display) & ALLEGRO_DIRECT3D) {
		ALPHA_FMT = ALLEGRO_PIXEL_FORMAT_ARGB_8888;
	}
	else
#endif
	{
		ALPHA_FMT = ALLEGRO_PIXEL_FORMAT_ABGR_8888_LE;
	}

#if defined ALLEGRO_WINDOWS
	if (al_get_display_flags(display) & ALLEGRO_DIRECT3D) {
		PRESERVE_TEXTURE = 0;
		NO_PRESERVE_TEXTURE = ALLEGRO_NO_PRESERVE_TEXTURE;
	}
	else
#endif
	{
		PRESERVE_TEXTURE = ALLEGRO_NO_PRESERVE_TEXTURE;
		NO_PRESERVE_TEXTURE = ALLEGRO_NO_PRESERVE_TEXTURE;
	}
#ifdef ALLEGRO_MACOSX
	PHYSFS_init(myArgv[0]);
	PHYSFS_addToSearchPath("data.zip", 1);
#else
	ALLEGRO_PATH *exename = al_get_standard_path(ALLEGRO_EXENAME_PATH);
#ifndef ALLEGRO_ANDROID
	al_set_path_filename(exename, "data.zip");
	PHYSFS_init(myArgv[0]);
#else
	PHYSFS_init(al_path_cstr(exename, '/'));
#endif
	PHYSFS_addToSearchPath(al_path_cstr(exename, '/'), 1);
	al_destroy_path(exename);
#endif
	al_set_physfs_file_interface();

	// Set an icon
#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_MACOSX && !defined ALLEGRO_ANDROID
	int icon_format = al_get_new_bitmap_format();
	int icon_flags = al_get_new_bitmap_flags();
	al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_ABGR_8888_LE);
	al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
	ALLEGRO_BITMAP *tmp_bmp = al_load_bitmap(getResource("staff.png"));
	al_set_display_icon(display, tmp_bmp);
	al_destroy_bitmap(tmp_bmp);
	al_set_new_bitmap_format(icon_format);
	al_set_new_bitmap_flags(icon_flags);
#endif

#ifdef ALLEGRO_ANDROID
	al_rest(1.0);
#endif

#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
	sd->width = al_get_display_width(display);
	sd->height = al_get_display_height(display);
#endif
	
	if (al_get_display_flags(display) & ALLEGRO_OPENGL) {
		if (use_fixed_pipeline) {
			my_opengl_version = 0x01;
		}
		else {
			my_opengl_version = al_get_opengl_version();
		}
	}
	else {
		my_opengl_version = 0x0;
	}

	if (use_fixed_pipeline) {
		use_programmable_pipeline = false;
	}
	else {
		use_programmable_pipeline = al_get_display_flags(display) & ALLEGRO_PROGRAMMABLE_PIPELINE;
	}

	debug_message("initing shaders");

	init_shaders();

#ifdef ALLEGRO_IPHONE
	al_init_user_event_source(&user_event_source);
#endif

	draw_timer = al_create_timer(1.0/config.getTargetFPS());
	logic_timer = al_create_timer(1.0/LOGIC_RATE);
	
	al_start_timer(draw_timer);
	al_start_timer(logic_timer);
	
	tguiInit();
	tguiSetRotation(0);

	set_screen_params();
      
#ifdef ALLEGRO_ANDROID
	uint32_t vers1 = parse_version(al_android_get_os_version());
	uint32_t vers2 = parse_version("2.3");
	if (vers1 < vers2) {
		is_android_lessthan_2_3 = true;
	}
#endif

	debug_message("Calling initSound\n");

	initSound();

	al_set_new_bitmap_flags(PRESERVE_TEXTURE | ALLEGRO_CONVERT_BITMAP);

#if defined ALLEGRO_IPHONE || defined ALLEGRO_MACOSX
	authenticatePlayer();
#endif

	custom_cursor_bmp = m_load_bitmap(getResource("media/mouse_cursor.png"));
	custom_cursor_w = m_get_bitmap_width(custom_cursor_bmp);
	custom_cursor_h = m_get_bitmap_height(custom_cursor_bmp);
#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_ANDROID && !defined ALLEGRO_RASPBERRYPI
	if (have_mouse) {
		custom_mouse_cursor = al_create_mouse_cursor(custom_cursor_bmp->bitmap, 0, 0);
		al_set_mouse_cursor(display, custom_mouse_cursor);
	}
#endif
	show_mouse_cursor();

#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_ANDROID
	if (sd->fullscreen) {
		sd->fullscreen = false; // hack
		toggle_fullscreen();
	}
#endif

	if (have_mouse) {
		int dx, dy, dw, dh;
		get_screen_offset_size(&dx, &dy, &dw, &dh);
		int mousex = al_get_display_width(display)-custom_cursor_w-20-dx;
		int mousey = al_get_display_height(display)-custom_cursor_h-20-dy;
		m_set_mouse_xy(display, mousex, mousey);
	}

	input_event_queue = al_create_event_queue();
	al_register_event_source(input_event_queue, (ALLEGRO_EVENT_SOURCE *)display);
#ifndef ALLEGRO_IPHONE
	al_register_event_source(input_event_queue, al_get_keyboard_event_source());
	al_register_event_source(input_event_queue, al_get_joystick_event_source());
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
#ifdef ALLEGRO_WINDOWS
	if (al_get_display_flags(display) & ALLEGRO_DIRECT3D) {
		init_big_depth_surface();
		al_set_d3d_device_restore_callback(d3d_resource_restore);
		al_set_d3d_device_release_callback(d3d_resource_release);
	}
#endif
	m_clear(black);
	m_flip_display();
#endif

	debug_message("setting window title\n");

#ifdef DEMO
	al_set_window_title(display, "Monster RPG 2 Demo");
#else
	al_set_window_title(display, "Monster RPG 2");
#endif

	if (al_get_display_flags(display) & ALLEGRO_OPENGL) {
		al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_ABGR_8888_LE);
	}
	else {
		al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_ARGB_8888);
	}

	debug_message("creating screenshot buffer\n");
	
	{
		flags = al_get_new_bitmap_flags();
		al_set_new_bitmap_flags(flags | ALLEGRO_NO_PRESERVE_TEXTURE);
		screenshot = m_create_bitmap(BW/2, BH/2);
		al_set_new_bitmap_flags(flags);
	}
	
	debug_message("creating tmpbuffer\n");

	create_tmpbuffer();

	debug_message("initing shader variables\n");

	if (al_get_display_flags(display) & ALLEGRO_OPENGL) {
		al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_ABGR_8888_LE);
	}
	else {
		al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_ARGB_8888);
	}

	if (!screenshot) {
		native_error("Failed to create SS buffer.");
	}
	
	debug_message("loading some bitmaps\n");
	
	corner_bmp = m_load_bitmap(getResource("media/corner.png"));
	stomach_circle = m_load_bitmap(getResource("combat_media/stomach_circle.png"));
	if (!loadTilemap()) {
		return false;
	}

	eny_loader = new_AnimationSet(getResource("media/eny-loader.png"));
	dot_loader = new_AnimationSet(getResource("media/dot-loader.png"));
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

	debug_message("going into load loop\n");

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

#ifndef ALLEGRO_ANDROID
	al_inhibit_screensaver(true);
#endif

	al_set_new_bitmap_flags(ALLEGRO_CONVERT_BITMAP);

	AnimationSet *shadow_tmp = new_AnimationSet(getResource("media/shadows.png"));
	shadow_sheet = shadow_tmp->clone(CLONE_COPY_BORDERS);
	delete shadow_tmp;

	draw_loading_screen(tmp, 100, sd);

	m_destroy_bitmap(tmp);
	delete eny_loader;
	delete dot_loader;
	m_destroy_bitmap(bg_loader);
	m_destroy_bitmap(bar_loader);
	m_destroy_bitmap(loading_loader);

	load_translation_tags();
	load_fonts();
	
	AnimationSet *tmpAnimSet = new_AnimationSet(getResource("gui.png"));
	guiAnims = tmpAnimSet->clone(CLONE_COPY_BORDERS);
	delete tmpAnimSet;

	poison_bmp = m_make_alpha_display_bitmap(poison_bmp);
	poison_bmp_tmp = m_make_alpha_display_bitmap(poison_bmp_tmp);
	poison_bmp_tmp2 = m_make_alpha_display_bitmap(poison_bmp_tmp2);
	orb_bmp = m_make_alpha_display_bitmap(orb_bmp);

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
	loadIcons();

	al_set_new_bitmap_flags(0);
	object_atlas = atlas_create(1024, 1024, ATLAS_ALPHA_BORDER, 1, false);
	al_set_new_bitmap_flags(ALLEGRO_CONVERT_BITMAP);
	std::vector<AnimationSet *> tmp_animsets;
	for (int i = 0; object_filenames[i]; i++) {
		AnimationSet *a = new_AnimationSet(getResource(object_filenames[i]));
		atlas_add(object_atlas, a->getBitmap(), i);
		object_atlas_map[getResource(object_filenames[i])] = i;
		tmp_animsets.push_back(a);
	}
	atlas_finish(object_atlas);
	for (size_t i = 0; i < tmp_animsets.size(); i++) {
		delete tmp_animsets[i];
	}
	tmp_animsets.clear();
	
	al_set_new_bitmap_flags(PRESERVE_TEXTURE | ALLEGRO_CONVERT_BITMAP);

	inited = true;

	m_clear(black);
	m_flip_display(); // backup bitmaps (slow)
	m_clear(black);

	return true;
}


void destroy()
{
	debug_message("Destroying resources...\n");

	tguiShutdown();

	m_destroy_bitmap(custom_cursor_bmp);

	exit_event_thread = 1;
	while (exit_event_thread != 2) {
		m_rest(0.001);
	}
	al_destroy_cond(wait_cond);
	al_destroy_mutex(wait_mutex);

	al_destroy_mutex(ss_mutex);
	
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

	m_destroy_bitmap(tile);
	destroyIcons();
	m_destroy_bitmap(stoneTexture);
	m_destroy_bitmap(mushroom);
	m_destroy_bitmap(webbed);
	m_destroy_bitmap(dpad_buttons);
	m_destroy_bitmap(batteryIcon);
	m_destroy_bitmap(achievement_bmp);
	delete terrain;
	m_destroy_bitmap(cursor);
	delete guiAnims;
	m_destroy_bitmap(orb_bmp);
	m_destroy_bitmap(poison_bmp);
	m_destroy_bitmap(poison_bmp_tmp);
	m_destroy_bitmap(poison_bmp_tmp2);
	m_destroy_bitmap(stomach_circle);
	if (screenshot) {
		m_destroy_bitmap(screenshot);
	}

	m_destroy_bitmap(corner_bmp);

	delete shadow_sheet;

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

	atlas_destroy(object_atlas);

	destroy_shaders();

#ifdef ALLEGRO_WINDOWS
	if (al_get_display_flags(display) & ALLEGRO_DIRECT3D) {
		al_get_d3d_device(display)->SetDepthStencilSurface(NULL);
		big_depth_surface->Release();
	}
#endif
	
	m_destroy_bitmap(tmpbuffer);

	if (custom_mouse_cursor) {
		al_destroy_mouse_cursor(custom_mouse_cursor);
	}

	if (saveFilename)
		free(saveFilename);

	cleanup_astar();
	al_destroy_mutex(click_mutex);
	al_destroy_mutex(input_mutex);
	al_destroy_mutex(dpad_mutex);
	al_destroy_mutex(touch_mutex);
#ifdef ALLEGRO_IPHONE
	shutdownIpod();
#endif

	al_destroy_mutex(input_event_mutex);

	destroySound();

	destroy_translation();

	PHYSFS_deinit();

	inited = false;
}

#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
static int dpad_count = 1;
#endif

void dpad_off(bool count)
{
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
	if (gamepadConnected()) {
		return;
	}

	if (count)
		dpad_count++;
	if (dpad_count > 0 || !count) {
		al_lock_mutex(dpad_mutex);
		use_dpad = (dpad_type == DPAD_TOTAL_1 || dpad_type == DPAD_TOTAL_2);
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
		al_unlock_mutex(dpad_mutex);
	}
#endif
}

void dpad_on(bool count)
{
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
	if (gamepadConnected()) {
		return;
	}

	if (count)
		dpad_count--;
	if (dpad_count <= 0 || !count) {
		al_lock_mutex(dpad_mutex);
		use_dpad = dpad_type != DPAD_NONE;
		if (use_dpad)
			myTguiIgnore(TGUI_MOUSE);
		else
			myTguiIgnore(0);
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
		float ratio_x, ratio_y;
		if (config.getMaintainAspectRatio() == ASPECT_INTEGER) {
			ratio_x = sd->width / BW;
			if (ratio_x > (sd->height / BH)) {
				ratio_x = sd->height / BH;
			}
			ratio_y = ratio_x;
		}
		else if (config.getMaintainAspectRatio() == ASPECT_MAINTAIN_RATIO) {
			ratio_x = (float)sd->width / BW;
			if (ratio_x > ((float)sd->height / BH)) {
				ratio_x = (float)sd->height / BH;
			}
			ratio_y = ratio_x;
		}
		else {
			ratio_x = sd->width / (float)BW;
			ratio_y = sd->height / (float)BH;
		}
		screenScaleX = ratio_x;
		screenScaleY = ratio_y;
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
		tguiSetScale(screenScaleX, screenScaleY);
		tguiSetTolerance(3);
	}

	set_target_backbuffer();

	int dx, dy;
	int dw, dh;

	get_screen_offset_size(&dx, &dy, &dw, &dh);

	ALLEGRO_TRANSFORM t;
	al_identity_transform(&t);
	al_scale_transform(&t, screenScaleX, screenScaleY);
	al_translate_transform(&t, dx, dy);
	al_use_transform(&t);

	al_set_clipping_rectangle(dx, dy, dw, dh);
}

void get_screen_offset_size(int *dx, int *dy, int *dw, int *dh)
{
	ScreenDescriptor *sd = config.getWantedGraphicsMode();
	
	if (config.getMaintainAspectRatio() == ASPECT_FILL_SCREEN) {
		*dx = *dy = 0;
		*dw = sd->width;
		*dh = sd->height;
	}
	else {
		*dx = screen_offset_x;
		*dy = screen_offset_y;
		*dw = screenScaleX*BW;
		*dh = screenScaleY*BH;
	}
}

void toggle_fullscreen()
{
#if defined ALLEGRO_RASPBERRYPI || defined ALLEGRO_ANDROID || defined ALLEGRO_IPHONE
	return;
#endif

	pause_joystick_repeat_events = true;

	bool redo_tmpbuffer = tmpbuffer != NULL;
	ALLEGRO_BITMAP *tmp = NULL;
	int dx, dy, dw, dh;
	get_screen_offset_size(&dx, &dy, &dw, &dh);
	if (redo_tmpbuffer) {
		// redo tmpbuffer
		ALLEGRO_BITMAP *old_target = al_get_target_bitmap();
		int old_flags = al_get_new_bitmap_flags();
		al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
		tmp = my_al_create_bitmap(dw, dh);
		al_set_new_bitmap_flags(old_flags);
		al_set_target_bitmap(tmp);
		al_draw_bitmap(tmpbuffer->bitmap, -dx, -dy, 0);
		al_set_target_bitmap(old_target);
		m_destroy_bitmap(tmpbuffer);
	}
		
	ScreenDescriptor *sd = config.getWantedGraphicsMode();

	if (!sd->fullscreen) {
		width_before_fullscreen = sd->width;
		height_before_fullscreen = sd->height;
	}

	set_target_backbuffer();
	al_clear_to_color(black);
	al_flip_display();

	sd->fullscreen = !sd->fullscreen;
#ifdef ALLEGRO_WINDOWS
	bool inited_depth_surface = false;
	if (al_get_display_flags(display) & ALLEGRO_DIRECT3D) {
		is_fs_toggle = true;
		inited_depth_surface = big_depth_surface != NULL;
		if (inited_depth_surface) {
			al_get_d3d_device(display)->SetDepthStencilSurface(NULL);
			big_depth_surface->Release();
		}
		_destroy_loaded_bitmaps();
	}
#endif

	if (inited) {
		destroy_fonts();
		destroyIcons();
	}

	al_set_display_flag(display, ALLEGRO_FULLSCREEN_WINDOW, config.getWantedGraphicsMode()->fullscreen);

	al_rest(0.1);

#ifdef ALLEGRO_WINDOWS
	if (al_get_display_flags(display) & ALLEGRO_DIRECT3D) {
		if (inited_depth_surface) {
			init_big_depth_surface();
		}
		_reload_loaded_bitmaps();
		_reload_loaded_bitmaps_delayed();
	}
#endif
	set_target_backbuffer();
	set_screen_params();
	if (inited) {
		load_fonts();
		loadIcons();
	}

#ifdef ALLEGRO_WINDOWS
	if (al_get_display_flags(display) & ALLEGRO_DIRECT3D) {
		if (in_shooter) {
			shooter_restoring = true;
		}
		is_fs_toggle = false;
	}
#endif
	pause_joystick_repeat_events = false;

#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_ANDROID
	if (have_mouse) {
#if !defined ALLEGRO_RASPBERRYPI
		al_set_mouse_cursor(display, custom_mouse_cursor);
#endif
		if (in_shooter || in_archery) {
			m_set_mouse_xy(display, al_get_display_width(display)/2, al_get_display_height(display)/2);
		}
		else {
			int dx, dy, dw, dh;
			get_screen_offset_size(&dx, &dy, &dw, &dh);
			int mousex = al_get_display_width(display)-custom_cursor_w-20-dx;
			int mousey = al_get_display_height(display)-custom_cursor_h-20-dy;
			m_set_mouse_xy(display, mousex, mousey);
		}
	}
#endif

	if (redo_tmpbuffer) {
		int dx2, dy2, dw2, dh2;
		get_screen_offset_size(&dx2, &dy2, &dw2, &dh2);
		create_tmpbuffer();
		ALLEGRO_BITMAP *old_target = al_get_target_bitmap();
		m_set_target_bitmap(tmpbuffer);
		al_draw_scaled_bitmap(tmp, 0, 0, dw, dh, dx2, dy2, dw2, dh2, 0);
		al_set_target_bitmap(old_target);
		al_destroy_bitmap(tmp);
	}
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

static bool cursor_hidden = false;

void show_mouse_cursor()
{
#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_ANDROID && !defined ALLEGRO_RASPBERRYPI
	if (have_mouse) {
		al_show_mouse_cursor(display);
	}
#endif
	cursor_hidden = false;
}

void hide_mouse_cursor()
{
#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_ANDROID && !defined ALLEGRO_RASPBERRYPI
	if (have_mouse) {
		al_hide_mouse_cursor(display);
	}
#endif	
	cursor_hidden = true;
}

bool is_cursor_hidden()
{
	return cursor_hidden;
}
