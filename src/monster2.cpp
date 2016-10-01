#include "monster2.hpp"

#ifdef ALLEGRO_ANDROID
#include "java.h"
#endif

#include "svg.hpp"

#ifdef STEAMWORKS
#include <steam/steam_api.h>
#endif

extern double my_last_shake_time;
extern ALLEGRO_FONT *my_load_ttf_font(const char *filename, int sz, int flags);

extern bool center_button_pressed;

int modifier_repeat_count[7] = { 0, };

bool cmdline_warped = false;
int cmdline_warp_x;
int cmdline_warp_y;
std::string cmdline_warp_area;

bool mouse_in_display = true;
bool forced_closed = false;

bool was_switched_out = false;

static int last_mouse_x = -1, last_mouse_y;
static int total_mouse_x, total_mouse_y;

#if defined ALLEGRO_ANDROID
static std::string old_music_name;
static std::string old_ambience_name;
static float old_music_volume;
static float old_ambience_volume;
#endif

#if defined ALLEGRO_ANDROID || defined ALLEGRO_IPHONE
bool switched_in = true;
#endif

#ifdef ALLEGRO_ANDROID
extern "C" {
}
#endif

TGUIWidget *mainWidget = NULL;
uint32_t runtime;
int runtime_ms = 0;
long runtime_start;

bool dont_draw_now = false;

bool break_main_loop = false;
static bool broke_with_manchooser = false;
bool quit_game = false;

bool timer_on = false;
int timer_time = 0;

bool battle_must_win;

Player *player = NULL;

bool battle_won = false;
bool battle_lost = false;

bool close_pressed = false;
bool close_pressed_for_configure = false;
	
char *saveFilename = NULL;

bool was_in_map = false;

bool fps_on = false;
int fps_frames;
double fps_counter;
int fps = 0;

#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
static int bright_dir = 1;
#endif
float bright_ticker = 0;

std::vector<std::pair<int, bool> > forced_milestones;

bool global_can_save = true;

bool tutorial_started = false;
bool gonna_fade_in_red = false;

void check_some_stuff_in_shooter(void);

int old_control_mode = -1;

bool prompt_for_close_on_next_flip = false;

bool menu_pressed = false;

#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
static void set_transform()
{
	int BB_W = al_get_display_width(display);
	int BB_H = al_get_display_height(display);

	glViewport(0, 0, BB_W, BB_H);
}

#ifdef ALLEGRO_IPHONE
static bool should_pause_game(void)
{
	return (area && !battle && !player_scripted && !in_pause && !in_map);
}

static float backup_music_volume = 1.0f;
static float backup_ambience_volume = 1.0f;
#endif
#endif

#ifdef ALLEGRO_ANDROID
void switch_in()
{
	switched_in = true;
	music_replayed = false;
}

void switch_out()
{
	old_music_name = musicName;
	old_ambience_name = ambienceName;
	old_music_volume = getMusicVolume();
	old_ambience_volume = getAmbienceVolume();
	playMusic("");
	playAmbience("");
	switched_in = false;
}
#endif

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

	if (x > xx && x < xx+BUTTON_SIZE) {
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

	if (x > xx && x < xx+BUTTON_SIZE) {
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
		if (y > BUTTON_SIZE*3+5)
			return;

		y -= 5;
	}
	else {
		if (y < (BH-(BUTTON_SIZE*3+5)))
			return;

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
	else {
		return;
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
};

std::vector<Touch> touches;

const int MOUSE_DOWN = 1;
const int MOUSE_UP = 2;
const int MOUSE_AXES = 3;

void myTguiIgnore(int type)
{
	tguiIgnore(type);
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
	if (!(config.getDpadType() == DPAD_TOTAL_1 || config.getDpadType() == DPAD_TOTAL_2)) {
		al_lock_mutex(touch_mutex);
		// FIXME: THIS OK REMOVED??? getInput()->set(false, false, false, false, false, false, false, true);
		touches.clear();
		al_unlock_mutex(touch_mutex);
	}
#endif
}

static int find_touch(int touch_id)
{
	for (size_t i = 0; i < touches.size(); i++) {
		if (touches[i].touch_id == touch_id)
			return (int)i;
	}
	
	return -1;
}

static void process_touch(int x, int y, int touch_id, int type)
{
#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_ANDROID
	return;
#endif
	
	al_lock_mutex(touch_mutex);
	if (type == MOUSE_DOWN) {
		Touch t;
		t.x = x;
		t.y = y;
		t.touch_id = touch_id;
		touches.push_back(t);
	}
	else if (type == MOUSE_UP) {
		int idx = find_touch(touch_id);
		if (idx >= 0) {
			touches.erase(touches.begin() + idx);
		}
	}
	else { // MOVE
		int idx = find_touch(touch_id);
		if (idx >= 0) {
			touches[idx].x = x;
			touches[idx].y = y;
		}
	}
	al_unlock_mutex(touch_mutex);
}

void clear_touches()
{
	al_lock_mutex(touch_mutex);
	touches.clear();
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

bool is_modifier(int c)
{
	switch (c) {
		case ALLEGRO_KEY_LSHIFT:
		case ALLEGRO_KEY_RSHIFT:
		case ALLEGRO_KEY_LCTRL:
		case ALLEGRO_KEY_RCTRL:
		case ALLEGRO_KEY_ALT:
		case ALLEGRO_KEY_ALTGR:
		case ALLEGRO_KEY_LWIN:
		case ALLEGRO_KEY_RWIN:
			return true;
	}

	return false;
}

static bool is_input_event(ALLEGRO_EVENT *e)
{
	if (
	e->type == ALLEGRO_EVENT_KEY_CHAR ||
	e->type == USER_KEY_CHAR ||
	e->type == ALLEGRO_EVENT_KEY_DOWN ||
	e->type == USER_KEY_DOWN ||
	e->type == ALLEGRO_EVENT_KEY_UP ||
	e->type == USER_KEY_UP ||
	e->type == ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN ||
	e->type == ALLEGRO_EVENT_JOYSTICK_BUTTON_UP ||
	e->type == ALLEGRO_EVENT_JOYSTICK_AXIS ||
	e->type == ALLEGRO_EVENT_TOUCH_BEGIN ||
	e->type == ALLEGRO_EVENT_TOUCH_END ||
	e->type == ALLEGRO_EVENT_TOUCH_MOVE ||
	e->type == ALLEGRO_EVENT_TOUCH_CANCEL ||
	e->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN ||
	e->type == ALLEGRO_EVENT_MOUSE_BUTTON_UP ||
	e->type == ALLEGRO_EVENT_MOUSE_AXES
	)
	{
		return true;
	}

	return false;
}

void get_mouse_pos_in_buffer_coords(int *this_x, int *this_y)
{
	if (config.getMaintainAspectRatio() == ASPECT_FILL_SCREEN)
		tguiConvertMousePosition(this_x, this_y, 0, 0, screen_ratio_x, screen_ratio_y);
	else
		tguiConvertMousePosition(this_x, this_y, screen_offset_x, screen_offset_y, 1, 1);
}

// called from everywhere
bool is_close_pressed(bool pump_events_only)
{
#ifdef STEAMWORKS
SteamAPI_RunCallbacks();
#endif

	/* This is a bit of a hack, to make sure "input events" don't
	 * stack up forever in places that don't use them.
	 */
	get_next_input_event();

	/* On OUYA the menu button press/release events come at the same time so we need a workaround. */
	menu_pressed = false;

#ifdef ALLEGRO_ANDROID	
top:
#endif

	// random tasks
	while (!al_event_queue_is_empty(input_event_queue)) {

		ALLEGRO_EVENT event;
		al_get_next_event(input_event_queue, &event);

#ifdef ALLEGRO_ANDROID
		if (gamepadConnected() && (
			event.type == ALLEGRO_EVENT_TOUCH_BEGIN ||
			event.type == ALLEGRO_EVENT_TOUCH_END ||
			event.type == ALLEGRO_EVENT_TOUCH_MOVE
		)) {
			continue;
		}
#endif
		if (is_input_event(&event) && event.any.timestamp < drop_input_events_older_than) {
			continue;
		}

#ifdef ALLEGRO_ANDROID
		if (event.type == ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN) {
			if (event.joystick.button == 6) {
				event.type = ALLEGRO_EVENT_JOYSTICK_AXIS;
				event.joystick.stick = 0;
				event.joystick.axis = 0;
				event.joystick.pos = -1;
			}
			else if (event.joystick.button == 7) {
				event.type = ALLEGRO_EVENT_JOYSTICK_AXIS;
				event.joystick.stick = 0;
				event.joystick.axis = 0;
				event.joystick.pos = 1;
			}
			else if (event.joystick.button == 8) {
				event.type = ALLEGRO_EVENT_JOYSTICK_AXIS;
				event.joystick.stick = 0;
				event.joystick.axis = 1;
				event.joystick.pos = -1;
			}
			else if (event.joystick.button == 9) {
				event.type = ALLEGRO_EVENT_JOYSTICK_AXIS;
				event.joystick.stick = 0;
				event.joystick.axis = 1;
				event.joystick.pos = 1;
			}
			else if (event.joystick.button == 10) {
				menu_pressed = true;
			}
		}
		else if (event.type == ALLEGRO_EVENT_JOYSTICK_BUTTON_UP) {
			if (event.joystick.button == 6) {
				event.type = ALLEGRO_EVENT_JOYSTICK_AXIS;
				event.joystick.stick = 0;
				event.joystick.axis = 0;
				event.joystick.pos = 0;
			}
			else if (event.joystick.button == 7) {
				event.type = ALLEGRO_EVENT_JOYSTICK_AXIS;
				event.joystick.stick = 0;
				event.joystick.axis = 0;
				event.joystick.pos = 0;
			}
			else if (event.joystick.button == 8) {
				event.type = ALLEGRO_EVENT_JOYSTICK_AXIS;
				event.joystick.stick = 0;
				event.joystick.axis = 1;
				event.joystick.pos = 0;
			}
			else if (event.joystick.button == 9) {
				event.type = ALLEGRO_EVENT_JOYSTICK_AXIS;
				event.joystick.stick = 0;
				event.joystick.axis = 1;
				event.joystick.pos = 0;
			}
		}
#endif

		al_lock_mutex(input_mutex);
		if (getInput())
			getInput()->handle_event(&event);
		al_unlock_mutex(input_mutex);
				
		if (event.type == ALLEGRO_EVENT_MOUSE_ENTER_DISPLAY) {
			mouse_in_display = true;
			al_set_mouse_cursor(display, custom_mouse_cursor);
		}
		else if (event.type == ALLEGRO_EVENT_MOUSE_LEAVE_DISPLAY) {
			mouse_in_display = false;
			al_set_system_mouse_cursor(display, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
		}
		else if (event.type == ALLEGRO_EVENT_KEY_CHAR || event.type == USER_KEY_CHAR) {
			INPUT_EVENT ie = EMPTY_INPUT_EVENT;
			int code = event.keyboard.keycode;
			if (code == config.getKeyLeft()) {
				ie.left = DOWN;
				add_input_event(ie);
			}
			else if (code == config.getKeyRight()) {
				ie.right = DOWN;
				add_input_event(ie);
			}
			else if (code == config.getKeyUp()) {
				ie.up = DOWN;
				add_input_event(ie);
			}
			else if (code == config.getKeyDown()) {
				ie.down = DOWN;
				add_input_event(ie);
			}
			else if (code == config.getKey1()) {
				ie.button1 = DOWN;
				add_input_event(ie);
			}
			else if (code == config.getKey2()) {
				ie.button2 = DOWN;
				add_input_event(ie);
			}
			else if (code == config.getKey3()) {
				ie.button3 = DOWN;
				add_input_event(ie);
			}
#ifdef ALLEGRO_ANDROID
			else if (event.keyboard.keycode == ALLEGRO_KEY_BACK) {
				ie.button2 = DOWN;
				add_input_event(ie);
			}
#endif
		}
		else if (event.type == ALLEGRO_EVENT_KEY_UP || event.type == USER_KEY_UP) {
			if (is_modifier(event.keyboard.keycode)) {
				if (event.keyboard.keycode == config.getKey1()) {
					if (area && !battle && !in_pause && config.getAlwaysCenter() == PAN_HYBRID) {
						area_panned_x = floor(area_panned_x);
						area_panned_y = floor(area_panned_y);
						area->center_view = true;
						center_button_pressed = true;
					}
					joy_b1_up();
					modifier_repeat_count[0] = 0;
				}
				else if (event.keyboard.keycode == config.getKey2()) {
					joy_b2_up();
					modifier_repeat_count[1] = 0;
				}
				else if (event.keyboard.keycode == config.getKey3()) {
					joy_b3_up();
					modifier_repeat_count[2] = 0;
				}
				else if (event.keyboard.keycode == config.getKeyLeft()) {
					joy_l_up();
					modifier_repeat_count[3] = 0;
				}
				else if (event.keyboard.keycode == config.getKeyRight()) {
					joy_r_up();
					modifier_repeat_count[4] = 0;
				}
				else if (event.keyboard.keycode == config.getKeyUp()) {
					joy_u_up();
					modifier_repeat_count[5] = 0;
				}
				else if (event.keyboard.keycode == config.getKeyDown()) {
					joy_d_up();
					modifier_repeat_count[6] = 0;
				}
			}
			else {
				INPUT_EVENT ie = EMPTY_INPUT_EVENT;
				int code = event.keyboard.keycode;
				if (code == config.getKeyLeft()) {
					ie.left = UP;
					add_input_event(ie);
				}
				else if (code == config.getKeyRight()) {
					ie.right = UP;
					add_input_event(ie);
				}
				else if (code == config.getKeyUp()) {
					ie.up = UP;
					add_input_event(ie);
				}
				else if (code == config.getKeyDown()) {
					ie.down = UP;
					add_input_event(ie);
				}
				else if (code == config.getKey1()) {
					dpad_panning = false;
					if (area && !battle && !in_pause && config.getAlwaysCenter() == PAN_HYBRID) {
						area_panned_x = floor(area_panned_x);
						area_panned_y = floor(area_panned_y);
						area->center_view = true;
						center_button_pressed = true;
					}
					ie.button1 = UP;
					add_input_event(ie);
				}
				else if (code == config.getKey2()) {
					ie.button2 = UP;
					add_input_event(ie);
				}
				else if (code == config.getKey3()) {
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
#ifdef ALLEGRO_ANDROID
				else if (code == ALLEGRO_KEY_BACK) {
					ie.button2 = UP;
					add_input_event(ie);
				}
#endif
			}
		}

#if !defined ALLEGRO_IPHONE
		else if (event.type == ALLEGRO_EVENT_JOYSTICK_CONFIGURATION) {
			al_reconfigure_joysticks();
			int nj = al_get_num_joysticks();
			if (nj == 0) {
				num_joystick_buttons = 0;
				config.setGamepadAvailable(false);
			}
			else if (nj > 0) {
				set_user_joystick();
				if (user_joystick != NULL) {
					num_joystick_buttons = al_get_joystick_num_buttons(user_joystick);
					config.setGamepadAvailable(true);
				}
				else {
					num_joystick_buttons = 0;
					config.setGamepadAvailable(false);
				}
			}
			getInput()->reconfig();
		}
		else if (event.type == ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN) {
			if (event.joystick.id == user_joystick) {
				if (event.joystick.button == config.getJoyButton1()) {
					joy_b1_down();
				}
				else if (event.joystick.button == config.getJoyButton2()) {
					joy_b2_down();
				}
				else if (event.joystick.button == config.getJoyButton3()) {
					joy_b3_down();
				}
				else if (event.joystick.button == config.getJoyButtonMusicDown()) {
					int v = config.getMusicVolume();
					if (v <= 26) v = 0;
					else v = v - 26; 
					config.setMusicVolume(v);
					setMusicVolume(getMusicVolume());
					setAmbienceVolume(getAmbienceVolume());
				}
				else if (event.joystick.button == config.getJoyButtonMusicUp()) {
					int v = config.getMusicVolume();
					if (v >= 230) v = 255;
					else v = v + 26; 
					config.setMusicVolume(v);
					setMusicVolume(getMusicVolume());
					setAmbienceVolume(getAmbienceVolume());
				}
				else if (event.joystick.button == config.getJoyButtonSFXDown()) {
					int v = config.getSFXVolume();
					if (v <= 26) v = 0;
					else v = v - 26; 
					config.setSFXVolume(v);
					if (boost) {
						setStreamVolume(
							boost,
							boost_volume
						);
					}
				}
				else if (event.joystick.button == config.getJoyButtonSFXUp()) {
					int v = config.getSFXVolume();
					if (v >= 230) v = 255;
					else v = v + 26; 
					config.setSFXVolume(v);
					if (boost) {
						setStreamVolume(
							boost,
							boost_volume
						);
					}
				}
			}
		}
		else if (event.type == ALLEGRO_EVENT_JOYSTICK_BUTTON_UP) {
			if (event.joystick.id == user_joystick) {
				if (event.joystick.button == config.getJoyButton1()) {
					if (area && !battle && !in_pause && config.getAlwaysCenter() == PAN_HYBRID) {
						area_panned_x = floor(area_panned_x);
						area_panned_y = floor(area_panned_y);
						area->center_view = true;
						center_button_pressed = true;
					}
					joy_b1_up();
				}
				else if (event.joystick.button == config.getJoyButton2()) {
					joy_b2_up();
				}
				else if (event.joystick.button == config.getJoyButton3()) {
					joy_b3_up();
				}
			}
		}
		else if (event.type == ALLEGRO_EVENT_JOYSTICK_AXIS) {
			if (event.joystick.id == user_joystick) {
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

		else if (getInput() && getInput()->isPlayerControlled() && 
		(event.type == BEGIN ||
		event.type == END ||
		event.type == ALLEGRO_EVENT_TOUCH_CANCEL ||
		(event.type == MOVE && !path_head))) {
			al_lock_mutex(input_mutex);

			if (event.type == ALLEGRO_EVENT_TOUCH_CANCEL) {
				event.type = ALLEGRO_EVENT_TOUCH_END;
			}

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

			get_mouse_pos_in_buffer_coords(&this_x, &this_y);

			if (use_dpad) {
				void (*down[7])(bool, bool) = {
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

				for (size_t i = 0; i < touches.size(); i++) {
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

				for (size_t i = 0; i < touches.size(); i++) {
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

				for (int i = 0; i < 7; i++) {
					if (on1[i] == false && on2[i] == true) {
						(*(down[i]))(false, false);
					}
					else if (on1[i] == true && on2[i] == false) {
						if (i == 4 /* up */ && area && !battle && !in_pause && config.getAlwaysCenter() == PAN_HYBRID) {
							area_panned_x = floor(area_panned_x);
							area_panned_y = floor(area_panned_y);
							area->center_view = true;
							center_button_pressed = true;
						}

						(*(up[i]))();
					}
					
					state[i] = on2[i];
				}
					
				getInput()->set(state[0], state[1], state[2], state[3],
					state[4], state[5], state[6], true);
			}
							
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
							if (al_current_time() > next_shake && !on_title_screen) {
								iphone_shake_time = al_current_time();
								next_shake = al_current_time()+0.25;
							}
						}
					}
				}
				if (use_dpad) {
					al_lock_mutex(dpad_mutex);
					if (!zone_defined(this_x, this_y)) {
						if (this_y < BH/3 && this_x > (BW/2-25) && this_x < (BW/2+25)) {
							dpad_at_top = true;
						}
						else if (this_y > (BH*2)/3 && this_x > (BW/2-25) && this_x < (BW/2+25)) {
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
					if (!use_dpad) {
						total_mouse_x = 0;
						total_mouse_y = 0;
						last_mouse_x = -1;
					}
				}
				al_unlock_mutex(dpad_mutex);
			}
			else if (event.type == MOVE && !released) {
				al_lock_mutex(click_mutex);
				current_mouse_x = this_x;
				current_mouse_y = this_y;
				al_unlock_mutex(click_mutex);
				al_lock_mutex(dpad_mutex);
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
				if (!use_dpad) {
#else
				if (!released) {
#endif
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
#if defined ALLEGRO_ANDROID
		else if (event.type == ALLEGRO_EVENT_DISPLAY_RESIZE) {
			do_acknowledge_resize = true;
		}
#endif
#if defined ALLEGRO_IPHONE
		else if (event.type == ALLEGRO_EVENT_DISPLAY_SWITCH_OUT)
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
			was_switched_out = true;
			do_close(false);
		}
		else if (event.type == ALLEGRO_EVENT_DISPLAY_SWITCH_IN)
		{
			setMusicVolume(backup_music_volume);
			setAmbienceVolume(backup_ambience_volume);
			switchiOSKeyboardIn();
		}
#endif
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
		else if (event.type == ALLEGRO_EVENT_DISPLAY_ORIENTATION) {
			set_transform();
			/*
			int x, y, w, h;
			get_screen_offset_size(&x, &y, &w, &h);
			al_set_clipping_rectangle(x, y, w, h);
			*/
		}
#endif

#ifdef ALLEGRO_ANDROID
		else if (event.type == ALLEGRO_EVENT_DISPLAY_SWITCH_IN) {
			switch_in();
		}
		else if (event.type == ALLEGRO_EVENT_DISPLAY_SWITCH_OUT) {
			switch_out();
		}
#endif

#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
		else if (event.type == ALLEGRO_EVENT_DISPLAY_HALT_DRAWING) {
			if (in_shooter && shooter_paused) {
				break_shooter_pause = true;
			}
			save_auto_save_to_disk();
#if defined ALLEGRO_ANDROID
			int cx, cy, cw, ch;
			al_get_clipping_rectangle(&cx, &cy, &cw, &ch);
			
			int dx, dy, dw, dh;
			get_screen_offset_size(&dx, &dy, &dw, &dh);
			m_destroy_bitmap(tmpbuffer);

			_destroy_loaded_bitmaps();
			destroy_fonts();
			destroyIcons();
			destroy_shaders();
#endif
			config.write();

			al_stop_timer(logic_timer);
			al_stop_timer(draw_timer);
			// halt
			
			al_acknowledge_drawing_halt(display);

			while (true) {
				ALLEGRO_EVENT event;
				al_wait_for_event(input_event_queue, &event);
				if (event.type == ALLEGRO_EVENT_DISPLAY_RESUME_DRAWING) {
					break;
				}
				else if (event.type == ALLEGRO_EVENT_DISPLAY_SWITCH_IN) {
					switch_in();
				}
				else if (event.type == ALLEGRO_EVENT_DISPLAY_SWITCH_OUT) {
					switch_out();
				}
				else if (event.type == ALLEGRO_EVENT_JOYSTICK_CONFIGURATION) {
					al_reconfigure_joysticks();
					int nj = al_get_num_joysticks();
					if (nj == 0) {
						num_joystick_buttons = 0;
						config.setGamepadAvailable(false);
					}
					else if (nj > 0) {
						set_user_joystick();
						if (user_joystick != NULL) {
							num_joystick_buttons = al_get_joystick_num_buttons(user_joystick);
							config.setGamepadAvailable(true);
						}
						else {
							num_joystick_buttons = 0;
							config.setGamepadAvailable(false);
						}
					}
					getInput()->reconfig();
				}
				if (event.type == USER_KEY_DOWN || event.type == USER_KEY_UP || event.type == USER_KEY_CHAR) {
					al_unref_user_event((ALLEGRO_USER_EVENT *)&event);
				}
			}

			// resume
			al_acknowledge_drawing_resume(display);
#ifdef ALLEGRO_ANDROID
			glDisable(GL_DITHER);

			init_shaders();

			_reload_loaded_bitmaps();
			_reload_loaded_bitmaps_delayed();
	
			set_screen_params();

			load_fonts();
			loadIcons();
			if (in_shooter) {
				shooter_restoring = true;
			}

			int dx2, dy2, dw2, dh2;
			get_screen_offset_size(&dx2, &dy2, &dw2, &dh2);
			ALLEGRO_BITMAP *old_target = al_get_target_bitmap();
			set_target_backbuffer();
			ALLEGRO_TRANSFORM t;
			al_identity_transform(&t);
			al_scale_transform(&t, screenScaleX, screenScaleY);
			al_translate_transform(&t, dx2, dy2);
			al_use_transform(&t);
			al_set_target_bitmap(old_target);
			al_set_clipping_rectangle(cx, cy, cw, ch);

			create_tmpbuffer();
			old_target = al_get_target_bitmap();
			m_set_target_bitmap(tmpbuffer);
			m_clear(black);
			al_set_target_bitmap(old_target);
#endif
			al_start_timer(logic_timer);
			al_start_timer(draw_timer);
		}
		else {
		}

		if (ALLEGRO_EVENT_TYPE_IS_USER(event.type)) {
			al_unref_user_event((ALLEGRO_USER_EVENT *)&event);
		}
#endif

		if (pump_events_only) {
			continue;
		}

		if (!getting_input_config && (event.type == ALLEGRO_EVENT_KEY_DOWN || event.type == USER_KEY_DOWN)) {
			if (is_modifier(event.keyboard.keycode)) {
				if (event.keyboard.keycode == config.getKey1()) {
					joy_b1_down();
				}
				else if (event.keyboard.keycode == config.getKey2()) {
					joy_b2_down();
				}
				else if (event.keyboard.keycode == config.getKey3()) {
					joy_b3_down();
				}
				else if (event.keyboard.keycode == config.getKeyLeft()) {
					joy_l_down();
				}
				else if (event.keyboard.keycode == config.getKeyRight()) {
					joy_r_down();
				}
				else if (event.keyboard.keycode == config.getKeyUp()) {
					joy_u_down();
				}
				else if (event.keyboard.keycode == config.getKeyDown()) {
					joy_d_down();
				}
			}
#if !defined ALLEGRO_ANDROID && !defined ALLEGRO_IPHONE && !defined ALLEGRO_RASPBERRYPI
			else if (event.keyboard.keycode == config.getKeyFullscreen()) {
				if (!pause_f_to_toggle_fullscreen && !transitioning) {
					do_toggle_fullscreen = true;
				}
			}
#endif
			else if (event.keyboard.keycode == config.getKeySettings() && !shooter_paused) {
				if (!pause_f_to_toggle_fullscreen) {
					close_pressed_for_configure = true;
				}
			}
			else if (event.keyboard.keycode == config.getKeyMusicDown()) {
				int v = config.getMusicVolume();
				if (v <= 26) v = 0;
				else v = v - 26; 
				config.setMusicVolume(v);
				setMusicVolume(getMusicVolume());
				setAmbienceVolume(getAmbienceVolume());
			}
			else if (event.keyboard.keycode == config.getKeyMusicUp()) {
				int v = config.getMusicVolume();
				if (v >= 230) v = 255;
				else v = v + 26; 
				config.setMusicVolume(v);
				setMusicVolume(getMusicVolume());
				setAmbienceVolume(getAmbienceVolume());
			}
			else if (event.keyboard.keycode == config.getKeySFXDown()) {
				int v = config.getSFXVolume();
				if (v <= 26) v = 0;
				else v = v - 26; 
				config.setSFXVolume(v);
				if (boost) {
					setStreamVolume(
						boost,
						boost_volume
					);
				}
			}
			else if (event.keyboard.keycode == config.getKeySFXUp()) {
				int v = config.getSFXVolume();
				if (v >= 230) v = 255;
				else v = v + 26; 
				config.setSFXVolume(v);
				if (boost) {
					setStreamVolume(
						boost,
						boost_volume
					);
				}
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
			
		if ((((event.type == ALLEGRO_EVENT_KEY_DOWN || event.type == USER_KEY_DOWN) && event.keyboard.keycode == config.getKeyQuit() && !getting_input_config) || event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) && !shooter_paused) {
#ifdef ALLEGRO_IPHONE
			if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
				forced_closed = true;
			}
			else {
				forced_closed = false;
			}
			close_pressed = true;
			break;
#else
			close_pressed = true;
#endif
		}
	}

	if (pump_events_only) {
		return false;
	}

#ifdef ALLEGRO_ANDROID
	if (switched_in && !music_replayed) {
		music_replayed = true;
		playMusic(old_music_name, old_music_volume, true);
		playAmbience(old_ambience_name, old_ambience_volume);
	}
#endif

	if (do_acknowledge_resize) {
		al_acknowledge_resize(display);
		do_acknowledge_resize = false;
		int x, y, w, h;
		get_screen_offset_size(&x, &y, &w, &h);
		al_set_clipping_rectangle(x, y, w, h);
		// FIXME: Kindle Fire only?
		set_screen_params();
	}

	if (reload_translation) {
		load_translation(get_language_name(config.getLanguage()).c_str());
		reload_translation = false;
		playPreloadedSample("blip.ogg");
	}
#ifdef ALLEGRO_WINDOWS
	if ((al_get_display_flags(display) & ALLEGRO_DIRECT3D) && should_reset) {
		al_get_d3d_device(display)->SetDepthStencilSurface(NULL);
		big_depth_surface->Release();
		_destroy_loaded_bitmaps();
		al_stop_timer(logic_timer);
		al_stop_timer(draw_timer);
		main_halted = true;
		while (d3d_halted) {
			m_rest(0.01);
		}
		init_big_depth_surface();
		_reload_loaded_bitmaps();
		_reload_loaded_bitmaps_delayed();
		ALLEGRO_BITMAP *old_target = al_get_target_bitmap();
		al_set_target_bitmap(tmpbuffer->bitmap);
		m_clear(black);
		al_set_target_bitmap(old_target);
		if (in_shooter) {
			shooter_restoring = true;
		}
		al_start_timer(logic_timer);
		al_start_timer(draw_timer);
		main_halted = false;
		should_reset = false;
	}
#endif
	
	if (do_toggle_fullscreen) {
		do_toggle_fullscreen = false;
		set_target_backbuffer();
		toggle_fullscreen();
	}

#ifdef ALLEGRO_ANDROID
	if (!switched_in) {
		al_rest(0.01);
		goto top;
	}
#endif

	return close_pressed;
}

void do_close_exit_game()
{
	if (saveFilename) saveTime(saveFilename);
	config.write();
	set_target_backbuffer();
	m_clear(al_map_rgb(0, 0, 0));
	m_flip_display();
	m_clear(al_map_rgb(0, 0, 0));
	m_flip_display();
	hide_mouse_cursor(); // for Pi
	destroy();
	exit(0);
}

void do_close(bool quit)
{
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
	if (area && !shouldDoMap) {
		area->auto_save_game_to_memory(0, true, false);
	}
	if (forced_closed) {
		forced_closed = false;
		save_auto_save_to_disk();
		config.write();
		if (quit) {
			destroy();
			exit(0);
		}
	}
#else
	if (area && !shouldDoMap) {
		area->auto_save_game_to_memory(0, true, false);
	}
	if (close_pressed_for_configure) {
		close_pressed_for_configure = false;
		close_pressed = false;
		config_menu();
	}
#endif
	else if (!was_switched_out) {
		if (on_title_screen) {
			do_close_exit_game();
		}
		else {
			close_pressed = false;
			prompt_for_close_on_next_flip = true;
			prepareForScreenGrab1();
		}
	}
	was_switched_out = false;
}


static bool playerCanLevel(std::string name)
{
	if (name == "Guard")
		return false;

	return true;
}

void main_draw()
{
	/* draw the Area */
	if (battle) {
		battle->draw();
	}
	else if (area) {
		if (gonna_fade_in_red)
			m_clear(m_map_rgb(255, 0, 0));
		else {
			area->draw();
		}
	}
	if (timer_on) {
		int minutes = (timer_time/1000) / 60;
		int seconds = (timer_time/1000) % 60;
		char text[10];
		sprintf(text, "%d:%02d", minutes, seconds);
		int tw = m_text_length(huge_font, "5:55") + 10;
		int th = m_text_height(huge_font);
		mTextout(huge_font, text, BW-(tw/2)-10, th/2+5,
			white, black,
			WGT_TEXT_NORMAL, true);
	}
	// Draw the GUI
	if (!manChooser || battle)
		tguiDraw();

	drawBufferToScreen();
}

static void run()
{
	//levelUp(party[heroSpot], 10);

	// Fix because Eny used to be only CLASS_WARRIOR, some save
	// states are missing CLASS_ENY
	party[heroSpot]->getInfo().characterClass |= CLASS_ENY;

	runtime_start = tguiCurrentTimeMillis();
	timer_on = false;
	timer_time = 0;

	clear_input_events();

	while (!break_main_loop) {
		// apply healall cheat
		if (healall) {
			healall = false;
			if (battle) {
				for (int i = 0; i < 4; i++) {
					CombatPlayer *p = battle->findPlayer(i);
					if (p) {
						CombatantInfo &i = p->getInfo();
						i.abilities.hp = i.abilities.maxhp;
						i.abilities.mp = i.abilities.maxmp;
					}
				}
			}
			else {
				for (int i = 0; i < 4; i++) {
					Player *p = party[i];
					if (p) {
						CombatantInfo &i = p->getInfo();
						i.abilities.hp = i.abilities.maxhp;
						i.abilities.mp = i.abilities.maxmp;
					}
				}
			}
		}

		al_wait_cond(wait_cond, wait_mutex);
		// Logic
		int tmp_counter = logic_counter;
		logic_counter = 0;
		if (tmp_counter > 10)
			tmp_counter = 1;
		while  (tmp_counter > 0) {
			next_input_event_ready = true;

			tmp_counter--;
			if (is_close_pressed()) {
				do_close();
				close_pressed = false;
			}

#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
			if (do_pause_game)
			{
				do_pause_game = false;
				bool ret = pause(true, false);
				if (!ret) {
					prepareForScreenGrab1();
					main_draw();
					prepareForScreenGrab2();
					fadeOut(black);
					return;
				}
			}
			
			if (party[heroSpot]) {
				Object *o = party[heroSpot]->getObject();
				if (o) {
					o->faceInputsLikeSprite();
				}
			}
				
			const float MAX_BRIGHT = 0.8f;
			bright_ticker += MAX_BRIGHT/1000.0*bright_dir*LOGIC_MILLIS; // go from normal to white in 1000ms
			if (bright_dir == 1 && bright_ticker >= MAX_BRIGHT) {
				bright_ticker = MAX_BRIGHT;
				bright_dir = -bright_dir;
			}
			else if (bright_dir == -1 && bright_ticker <= 0) {
				bright_ticker = 0;
				bright_dir = -bright_dir;
			}
#endif
			
			long now = tguiCurrentTimeMillis();
			runtime_ms += now - runtime_start;
			runtime_start = now;
			while (runtime_ms >= 1000) {
				runtime++;
				runtime_ms -= 1000;
			}
			if (!battle && party[heroSpot] && party[heroSpot]->getName() == "Eny") {
				// seems like the best spot for this
				bool poisoned = false;
				for (int i = 0; i < MAX_PARTY; i++) {
					Player *p = party[i];
					if (!p)
						continue;
					CombatantInfo &info = p->getInfo();
					if (info.abilities.hp < 1)
						continue;
					if (info.condition == CONDITION_POISONED) {
						poisoned = true;
						break;
					}
				}
				party[heroSpot]->getObject()->setPoisoned(poisoned);
			}
			if (timer_on) {
				timer_time -= LOGIC_MILLIS;
				if (timer_time < 0) {
					timer_time = 0;
					if (saveFilename) saveTime(saveFilename);

					if (speechDialog) {
						dpad_on();
						delete speechDialog;
						speechDialog = NULL;
					}

					if (!battle) {
						prepareForScreenGrab1();
						main_draw();
						prepareForScreenGrab2();
						fadeOut(m_map_rgb(255, 0, 0));
					}

					prepareForScreenGrab1();
					m_clear(m_map_rgb(255, 0, 0));
					drawBufferToScreen(false);
					prepareForScreenGrab2();

					if (battle) {
						delete battle;
						battle = NULL;
					}

					anotherDoDialogue("You were not quick enough to stop the Golems.\n", false, true, false);

					return;
				}
			}
			// update gui
			mainWidget = tguiUpdate();
			if (manChooser && mainWidget == manChooser) {
				tguiDeleteWidget(manChooser);
			}
			if (speechDialog && mainWidget == speechDialog) {
				tguiDeleteWidget(speechDialog);
				delete speechDialog;
				speechDialog = NULL;
				dpad_on();
				// FIXME: make sure this works
				// dont want tapping away a dialog to make
				// you move once it's closed
				// HERE
				//tguiMakeFresh();
			}
			// update battle
			if (battle) {
			
				std::vector<int> levels;
				for (int i = 0; i < MAX_PARTY; i++) {
					if (party[i]) {
						levels.push_back(getLevel(party[i]->getInfo().experience));
					}
					else {
						levels.push_back(-1);
					}
				}
				BattleResult result = battle->update(LOGIC_MILLIS);
				if (result != BATTLE_CONTINUE) {
					getInput()->waitForReleaseOr(4, 1000000);
					clear_input_events();

					had_battle = true;
					astar_stop();
					if (!timer_on && !manChooser &&
					result != BATTLE_ENEMY_WIN) {
						area->startMusic();
						setMusicVolume(1);
						setAmbienceVolume(1);
					}
					if (result == BATTLE_ENEMY_WIN) {
						battle_won = false;
						battle_lost = true;
						dont_draw_now = true;
					}
					else if (result == BATTLE_PLAYER_RUN) {
						battle_won = false;
						battle_lost = false;
					}
					else if (result == BATTLE_PLAYER_WIN) {
						battle_won = true;
						battle_lost = false;
						for (int i = 0; i < MAX_PARTY; i++) {
							if (party[i] && playerCanLevel(party[i]->getName())) {
								int newLevel = getLevel(party[i]->getInfo().experience);
								for (int l = levels[i]; l < newLevel; l++) {
									while (levelUp(party[i])) {
										if (break_main_loop) {
											break;
										}
									}
								}
							}
						}
					}
					delete battle;
					battle = NULL;
					
					if (party[heroSpot]) {
						Object *o = party[heroSpot]->getObject();
						if (o) {
							o->faceInputsLikeSprite();
						}
					}
				
					if (result == BATTLE_ENEMY_WIN && battle_must_win) {
						if (saveFilename) saveTime(saveFilename);
						return;
					}
					else if (result == BATTLE_ENEMY_WIN) {
						for (int i = 0; i < MAX_PARTY; i++) {
							Player *p = party[i];
							if (p) {
								if (p->getInfo().abilities.hp <= 0)
									p->getInfo().abilities.hp = 1;
							}
						}
					}
					logic_counter = draw_counter = 0;
				}
				levels.clear();
			}
			else if (area) {
				Area *oldArea = area;
				std::string oldAreaName = area->getName();
				area->update(LOGIC_MILLIS);
				
				/* FIXME: Hardcoded (esp. "5") */
				if (area->getName() == "tutorial") {
					if (getNumberFromScript(area->getLuaState(), "stage") == 5) {
						tutorial_started = false;
						return;
					}
				}
				
				// players could be dead
				bool all_dead = true;
				for (int i = 0; i < MAX_PARTY; i++) {
					if (party[i] &&
					party[i]->getInfo().abilities.hp > 0) {
						all_dead = false;
						break;
					}
				}

				bool was = was_in_map;
				was_in_map = false;

				if (all_dead) {
					//FIXME
					if (saveFilename) saveTime(saveFilename);
					return;
				}

				bool did_change_areas = false;

				if (!was && (area->getName() != oldAreaName)) {
					delete oldArea;
					oldArea = NULL;
					area->getObjects()[0]->stop();
					did_change_areas = true;
				}
				
				if (was || area->getName() != oldAreaName)
					dont_draw_now = true;

				if (break_main_loop) {
					if (manChooser) {
						broke_with_manchooser = true;
					}
					break_main_loop = false;
					return;
				}

				if (was || (area->getName() != oldAreaName)) {
					if (oldArea)
						delete oldArea;
					area->getObjects()[0]->stop();
					did_change_areas = true;
				}

				if (did_change_areas) {
					area_panned_x = -BW/2;
					area_panned_y = -BH/2;
				}

				// pause
				if (area && party[heroSpot] && party[heroSpot]->getName() == "Eny" && party[heroSpot]->getObject()->getInput()->isPlayerControlled() &&
						!speechDialog && !path_head) {
					InputDescriptor ie = getInput()->getDescriptor();
					if (area->getName() != "tutorial") {
						if (ie.button2 || iphone_shaken(0.1) || menu_pressed) {
							menu_pressed = false;
							waitForRelease(4);
							iphone_clear_shaken();
							int posx, posy;
							party[heroSpot]->getObject()->getPosition(&posx, &posy);
							bool can_save = true;
							prepareForScreenGrab1();
							main_draw();
							prepareForScreenGrab2();
							fadeOut(black);
							bool ret = pause(can_save);
							if (!ret) {
								return;
							}
							
							prepareForScreenGrab1();
							m_clear(black);
							area->draw();
							drawBufferToScreen(false);
							prepareForScreenGrab2();
							fadeIn(black);

							if (party[heroSpot]) {
								Object *o = party[heroSpot]->getObject();
								if (o) {
									o->faceInputsLikeSprite();
								}
							}
				
							runtime_start = tguiCurrentTimeMillis();
						}
					}
				}
			}
		}

		if (break_main_loop) {
			if (manChooser) {
				broke_with_manchooser = true;
			}
			break_main_loop = false;
			return;
		}

		if (draw_counter > 0 && !dont_draw_now) {
			draw_counter = 0;

			set_target_backbuffer();
			main_draw();

			m_flip_display();
		}
		
		dont_draw_now = false;

		if (!battle && party[heroSpot] && party[heroSpot]->getName() == "Eny" && party[heroSpot]->getObject()->getPoisoned() && !gameInfo.milestones[MS_FIRST_POISON]) {
			gameInfo.milestones[MS_FIRST_POISON] = true;
			prepareForScreenGrab1();
			main_draw();
			prepareForScreenGrab2();
			anotherDoDialogue("Oh, no! Someone is poisoned. They'll lose health every turn until they're healed.\n", false, true);
		}

#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_ANDROID
		ALLEGRO_KEYBOARD_STATE state;
		my_get_keyboard_state(&state);
#endif
	}

	if (break_main_loop) {
		if (manChooser) {
			broke_with_manchooser = true;
		}
	}

	break_main_loop = false;
}

#ifndef EDITOR
int main(int argc, char *argv[])
{
#ifndef ALLEGRO_ANDROID
	int n;
	if ((n = check_arg(argc, argv, "-adapter")) != -1) {
		config.setAdapter(atoi(argv[n+1]));
	}
#endif

	if (!init(&argc, &argv)) {
		printf("An error occurred during initialization.\n");
		remove(getUserResource("launch_config"));
		return 1;
	}

#ifndef ALLEGRO_ANDROID
	int c = argc;
	char **p = argv;
	while ((n = check_arg(c, p, "-ms")) != -1) {
		int num = atoi(p[n+1]);
		bool value = atoi(p[n+2]);
		std::pair<int, bool> x;
		x.first = num;
		x.second = value;
		forced_milestones.push_back(x);
		c -= n+2;
		p = &p[n+2];
	}

	if ((n = check_arg(argc, argv, "-warp")) != -1) {
		cmdline_warped = true;
		cmdline_warp_area = argv[n+1];
		cmdline_warp_x = atoi(argv[n+2]);
		cmdline_warp_y = atoi(argv[n+3]);
	}
#endif

#ifdef DEBUG_XXX
	#include "savestate.h"
	//#ifdef OVERWRITE_SAVE
	FILE *f = fopen(getUserResource("auto9.save"), "wb");
	fwrite(savedata, save_state_size, 1, f);
	fclose(f);
	//#endif
#endif

#ifdef ALLEGRO_IPHONE
	initiOSKeyboard();
	al_register_event_source(input_event_queue, &user_event_source);
#endif

	int dx, dy, dw, dh;
	get_screen_offset_size(&dx, &dy, &dw, &dh);

	float wanted = dw * 0.75f;
#ifdef WITH_SVG
	const float svg_w = 362;
	float scale = wanted / svg_w;
	MBITMAP *nooskewl = new_mbitmap(load_svg(getResource("media/nooskewl.svg"), scale));
#else
	int flags = al_get_new_bitmap_flags();
	
	int linear = 0;
#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_ANDROID && !defined ALLEGRO_RASPBERRYPI
#ifdef ALLEGRO_WINDOWS
	if (al_get_display_flags(display) & ALLEGRO_DIRECT3D) {
#endif
		linear = ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR;
#ifdef ALLEGRO_WINDOWS
	}
#endif
#endif
	al_set_new_bitmap_flags(linear);
	al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);
	MBITMAP *tmp = m_load_bitmap(getResource("media/nooskewl.png"));
	al_set_new_bitmap_flags(flags);
	MBITMAP *nooskewl = m_create_bitmap(
		wanted,
		((float)wanted/m_get_bitmap_width(tmp))*m_get_bitmap_height(tmp)
	);
	m_push_target_bitmap();
	m_set_target_bitmap(nooskewl);
	m_clear(m_map_rgba(0, 0, 0, 0));
	quick_draw(
		tmp->bitmap,
		0, 0,
		m_get_bitmap_width(tmp), m_get_bitmap_height(tmp),
		0, 0,
		m_get_bitmap_width(nooskewl), m_get_bitmap_height(nooskewl),
		0
	);
	m_pop_target_bitmap();
	m_destroy_bitmap(tmp);
#endif
	
#ifndef ALLEGRO_ANDROID
	if ((n = check_arg(argc, argv, "-stick")) != -1) {
		int stick = atoi(argv[n+1]);
		config.setStick(stick);
	}
	if ((n = check_arg(argc, argv, "-axis")) != -1) {
		int axis = atoi(argv[n+1]);
		config.setAxis(axis);
	}
	if ((n = check_arg(argc, argv, "-360")) != -1) {
		config.setXbox360(true);
	}
#endif

	bool fps_save = fps_on;
	fps_on = false;

	prepareForScreenGrab1();
	m_clear(m_map_rgb(0x00, 0x00, 0x00));
	m_draw_bitmap_identity_view(
		nooskewl,
		dx+dw/2-m_get_bitmap_width(nooskewl)/2,
		dy+dh/2-m_get_bitmap_height(nooskewl)/2,
		0
	);
	drawBufferToScreen(false);
	prepareForScreenGrab2();
	bool cancelled = transitionIn(true, false);

	if (!cancelled) {
		m_clear(m_map_rgb(0x00, 0x00, 0x00));
		m_draw_bitmap_identity_view(
			nooskewl,
			dx+dw/2-m_get_bitmap_width(nooskewl)/2,
			dy+dh/2-m_get_bitmap_height(nooskewl)/2,
			0
		);
		transitioning = true; // hide controls on touchscreen & mouse?
		drawBufferToScreen(false);
		transitioning = false;
		m_flip_display();
					
		m_rest(1.5);
		prepareForScreenGrab1();
		m_clear(m_map_rgb(0x00, 0x00, 0x00));
		m_draw_bitmap_identity_view(
			nooskewl,
			dx+dw/2-m_get_bitmap_width(nooskewl)/2,
			dy+dh/2-m_get_bitmap_height(nooskewl)/2,
			0
		);
		drawBufferToScreen(false);
		prepareForScreenGrab2();
		transitionOut(false);
	}
	else {
		m_rest(1);
	}
	m_destroy_bitmap(nooskewl);

	set_target_backbuffer();
	
	fps_on = fps_save;

#ifndef ALLEGRO_ANDROID
	if (check_arg(argc, argv, "-volcano") != -1) {
		playMusic("volcano.ogg");
		volcano_scene();
	}
	if (check_arg(argc, argv, "-lander") != -1) {
		do_lander();
	}
	if (check_arg(argc, argv, "-archery") != -1) {
		archery(false);
	}
	if (check_arg(argc, argv, "-shooter") != -1) {
		shooter(false);
	}
	if (check_arg(argc, argv, "-credits") != -1) {
		credits();
	}
#endif

	fps_counter = al_get_time();
	fps_frames = 0;

	while (!quit_game) {
		playAmbience("");
		playMusic("title.ogg");

		debug_message("After playmusic/ambience\n");
		
		debug_message("After getinputupdate\n");
		
		int choice = 0;
		
#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_ANDROID
		if (al_filename_exists(getUserResource("start_in_config"))) {
			remove(getUserResource("start_in_config"));
			bool result = config_menu(true);
			if (result == true)
				choice = 0xDEAD;
		}
		if (choice != 0xDEAD)
#endif
		choice = title_menu();

		m_push_target_bitmap();
		set_target_backbuffer();
		m_clear(m_map_rgb(0, 0, 0));
		m_pop_target_bitmap();

		int remap[4] = { 0, 1, 3, 4 };
		if (choice != 0xDEAD && choice != 0xBEEF) {
			choice = remap[choice];
		}

		debug_message("after title_menu\n");

		m_rest(0.25);
		
		if (choice == 0) {
			try {
				if (!loadGame(getUserResource("auto0.save"))) {
					Area *oldArea = area;
					area->update(1);
					if (area != oldArea) {
						delete oldArea;
						was_in_map = false;
					}
					else {
						prepareForScreenGrab1();
						area->draw();
						drawBufferToScreen(false);
						prepareForScreenGrab2();
						transitionIn();
					}
				}
				player = party[heroSpot];
			}
			catch (ReadError e) {
				(void)e;
				prepareForScreenGrab1();
				m_clear(black);
				prepareForScreenGrab2();
				notify("No auto-save", "yet recorded...", "");
				continue;
			}
		}
		else if (choice == 1) {
			int num;
			bool exists;
			bool isAuto;

			choose_savestate(&num, &exists, &isAuto);

			set_target_backbuffer();
			m_clear(black);
			drawBufferToScreen(false);
			m_flip_display();

			if (num < 0) {
				continue;
			}
			else if (exists) {
				try {
					debug_message("loading game\n");
					if (!loadGame(getUserResource("%s%d.save", isAuto ? "auto" : "", num))) {
						Area *oldArea = area;
						area->update(1);
						if (area != oldArea) {
							delete oldArea;
							was_in_map = false;
						}
						else {
							prepareForScreenGrab1();
							area->draw();
							drawBufferToScreen(false);
							prepareForScreenGrab2();
							transitionIn();
						}
					}
					player = party[heroSpot];
					debug_message("loaded\n");
				}
				catch (ReadError e) {
					(void)e;
					prepareForScreenGrab1();
					m_clear(black);
					prepareForScreenGrab2();
					notify("Error loading...", "", "");
					continue;
				}
			}
			else {
				debug_message("creating player\n");
				player = new Player("Eny"); // auto put in party
				player->setObject(new Object());
				player->getObject()->setPerson(true);
				player->getObject()->setAnimationSet(std::string(getResource("objects/Eny.png")));
				al_lock_mutex(input_mutex);
				player->getObject()->setInput(getInput());
				al_unlock_mutex(input_mutex);
				heroSpot = 0;
				debug_message("calling startNewGame\n");
				party[heroSpot] = player;
				startNewGame();
				if (saveFilename) free(saveFilename);
				saveFilename = NULL;
				runtime = 0;
				gold = 0;
				runtime_ms = 0;
				runtime_start = 0;
				debug_message("started new game\n");
			}
		}
		else if (choice == 3) {
			player = new Player("Eny"); // auto put in party
			player->setObject(new Object());
			player->getObject()->setPerson(true);
			player->getObject()->setAnimationSet(getResource("objects/Eny.png"));
			al_lock_mutex(input_mutex);
			player->getObject()->setInput(getInput());
			al_unlock_mutex(input_mutex);
			heroSpot = 0;
			party[heroSpot] = player;
			tutorial_started = true;
			startNewGame("tutorial");
		}
		else {
			config.write();
			break;
		}

		run();

		save_auto_save_to_disk();

		if (manChooser) {
			if (broke_with_manchooser) {
				broke_with_manchooser = false;
				tguiDeleteWidget(manChooser);
				destroy_guards();
				pop_players();
			}
			delete manChooser;
			manChooser = NULL;
			dpad_on();
		}
		if (player_scripted) {
			CDeScriptifyPlayer(NULL);
		}
		al_lock_mutex(input_mutex);
		astar_stop();
		al_unlock_mutex(input_mutex);


		if (area) {
			delete area;
			area = NULL;
		}

		if (battle) {
			delete battle;
			battle = NULL;
		}
		if (speechDialog) {
			dpad_on();
			tguiDeleteWidget(speechDialog);
			delete speechDialog;
			speechDialog = NULL;
		}

		al_lock_mutex(input_mutex);
		for (int i = 0; i < MAX_PARTY; i++) {
			if (party[i]) {
				Object *o = party[i]->getObject();
				if (o)
					delete o;
				delete party[i];
				party[i] = NULL;
			}
		}
		player = NULL;
		resetIds();
		initInput();
		al_unlock_mutex(input_mutex);
	}

#ifndef ALLEGRO_WINDOWS
	al_unlock_mutex(wait_mutex);
#endif

	destroy();

	debug_message("done\n");

	debug_message("RETURNING FROM MAIN\n");

	return 0;
}
#endif

