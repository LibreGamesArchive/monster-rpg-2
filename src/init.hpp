#ifndef INIT_HPP

#define INIT_HPP

#include <setjmp.h>

typedef std::pair< std::pair<int, int>, std::pair<int, int> > ZONE;
bool zone_defined(int x, int y);
void delete_zone(std::list<ZONE>::iterator it);
std::list<ZONE>::iterator define_zone(int x1, int y1, int x2, int y2);

void set_screen_params(void);

struct WaterData
{
	int tilenum;
	int depth;
};

bool init(int *argc, char **argv[]);
void destroy(void);
void destroyTilemap(void);
bool loadTilemap(void);
void close_button_callback(void);
void myTguiIgnore(int type);
void set_mouse_off(bool off);
bool get_mouse_off(void);
void dpad_off(bool count = true);
void dpad_on(bool count = true);
bool is_ipad(void);
void toggle_fullscreen(void);
bool is_ipad(void);


extern ALLEGRO_TIMER *logic_timer, *draw_timer;
extern float initial_screen_scale;
#ifndef ALLEGRO4
extern ALLEGRO_DISPLAY *display;
extern ALLEGRO_DISPLAY *controller_display;
extern ALLEGRO_COND *wait_cond;
extern ALLEGRO_MUTEX *wait_mutex;
extern int exit_event_thread;
//extern ALLEGRO_SHADER *controller_shader;
extern ALLEGRO_SHADER *default_shader;
extern ALLEGRO_SHADER *cheap_shader;
extern ALLEGRO_SHADER *tinter;
extern ALLEGRO_SHADER *warp;
extern ALLEGRO_SHADER *shadow_shader;
extern ALLEGRO_SHADER *brighten;
extern ALLEGRO_SHADER *scale2x;
extern ALLEGRO_SHADER *scale2x_flipped;
extern ALLEGRO_SHADER *scale3x;
extern ALLEGRO_SHADER *scale3x_flipped;
extern ALLEGRO_SHADER *scale2x_linear;
extern ALLEGRO_SHADER *scale2x_linear_flipped;
extern ALLEGRO_SHADER *scale3x_linear;
extern ALLEGRO_SHADER *scale3x_linear_flipped;
#endif
extern MBITMAP *buffer;
extern MBITMAP *overlay;
extern MBITMAP *scaleXX_buffer;
extern int shadow_buf_w, shadow_buf_h;
extern int shadow_buf_x, shadow_buf_y;
extern MBITMAP *screenshot;
extern MBITMAP *tilemap;
extern bool *tileTransparent;
extern int numTiles;
extern MCOLOR white, black, grey, blue;
extern MBITMAP *tile;
extern char start_cwd[1000];
extern XMLData *tilemap_data;
extern XMLData *trans_data;
extern XMLData *terrain;
extern std::vector<WaterData> waterData;

struct GuiAnims
{
	MBITMAP *bitmap;
	MBITMAP *corner_sub;
	MBITMAP *wide_sub;
	MBITMAP *tall_sub;
};
extern GuiAnims guiAnims;

extern GameInfo gameInfo;
//extern MBITMAP *profileBg;
extern MBITMAP *cursor;
extern MFONT *huge_font;
extern MFONT *medium_font;
extern MBITMAP *stoneTexture;
extern MBITMAP *mushroom;
extern MBITMAP *webbed;
extern MBITMAP *orb_bmp;
extern MBITMAP *poison_bmp;
extern MBITMAP *poison_bmp_tmp;
extern MBITMAP *poison_bmp_tmp2;

extern volatile int draw_counter;
extern volatile int logic_counter;

extern bool gfx_mode_set;

#ifdef WIZ
extern MBITMAP *tmp_texture;
extern unsigned char *tmp_buffer;
//extern float sintab[SIN_TAB_SIZE+1];
//extern float costab[COS_TAB_SIZE+1];
extern MBITMAP *screen;
extern int startGFXDriver;
void *thread_proc(ALLEGRO_THREAD *thread, void *arg);
extern ALLEGRO_THREAD *thread;
#endif

extern bool egl_workaround;
extern bool inited;

extern bool use_dpad;
extern int dpad_type;
extern bool dpad_at_top;
extern bool released;
extern int click_x, click_y;
extern int event_mouse_x, event_mouse_y;
extern int current_mouse_x, current_mouse_y;
extern ALLEGRO_MUTEX *click_mutex;
extern ALLEGRO_MUTEX *input_mutex;
extern ALLEGRO_MUTEX *dpad_mutex;
extern ALLEGRO_MUTEX *orient_mutex;
extern float bright_ticker;
//extern int area_panned_x;
//extern int area_panned_y;
extern float area_panned_x;
extern float area_panned_y;
extern MBITMAP *corner_bmp;
extern bool had_battle;
extern MBITMAP *shadow_sheet;
const int SHADOW_CORNER_SIZE = 10;
extern MBITMAP *shakeAlternativeBitmap;
extern ALLEGRO_VERTEX triangle_lines[32];
extern MBITMAP *dpad_buttons;
extern bool onscreen_swipe_to_attack;
extern bool onscreen_drag_to_use;
extern bool reinstall_timer;
extern bool drew_shadow;
extern MBITMAP *batteryIcon;
extern bool global_draw_red;
extern bool red_off_press_on;
extern volatile bool loading_done;
extern MBITMAP *stomach_circle;
extern float screenScaleX;
extern float screenScaleY;

extern bool switched_out;
extern ALLEGRO_COND *switch_cond;
extern ALLEGRO_MUTEX *switch_mutex;

extern char versionString[10];
extern uint32_t my_opengl_version;
extern bool use_programmable_pipeline;
extern int screen_offset_x, screen_offset_y;
extern float screen_ratio_x, screen_ratio_y;
extern bool pause_f_to_toggle_fullscreen;
extern bool do_toggle_fullscreen;
#ifdef A5_D3D
extern bool d3d_device_lost;
extern LPDIRECT3DSURFACE9 big_depth_surface;
#endif
extern bool use_digital_joystick;

enum ScreenSize {
	ScreenSize_Normal = 0,
	ScreenSize_Tiny,
	ScreenSize_Smaller,
	ScreenSize_Small
};

ScreenSize small_screen(void);
void get_buffer_true_size(int *buffer_true_w, int *buffer_true_h);
// cheats
extern bool superpower, healall;

extern bool have_mouse;
extern bool reload_translation;

extern int PRESERVE_TEXTURE;
extern bool achievement_show;
extern double achievement_time;
extern MBITMAP *achievement_bmp;

void init_shaders(void);
void destroy_shaders(void);

#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
extern bool do_pause_game;
void init_shaders();
void destroy_shaders();
#endif
#ifdef ALLEGRO_IPHONE
void check_orientation(void);
extern bool create_airplay_mirror;
extern bool delete_airplay_mirror;
extern bool airplay_connected;
#endif
void init2_shaders();

void register_display(ALLEGRO_DISPLAY *display);
//void init_controller_shader(void);
//void destroy_controller_shader(void);

extern volatile int curr_touches;

extern int versionMajor;
extern int versionMinor;

extern bool do_acknowledge_resize;
	
extern ALLEGRO_EVENT_QUEUE *events_minor;
#ifdef ALLEGRO_IPHONE
extern double next_shake;
#endif

extern bool sound_was_playing_at_program_start;
#ifdef ALLEGRO_IPHONE
extern double allegro_iphone_shaken;
#endif

extern ALLEGRO_JOYSTICK *user_joystick;
extern bool is_intel_gpu_on_desktop_linux;

extern bool main_halted;
extern bool d3d_halted;
extern bool should_reset;
extern bool is_fs_toggle;
void init_big_depth_surface(void);

bool imperfect_aspect(void);
void create_buffers(void);

extern "C" {
void connect_external_controls(void);
void disconnect_external_controls(void);
void lock_joypad_mutex(void);
void unlock_joypad_mutex(void);
}

#endif

