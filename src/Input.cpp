#include "monster2.hpp"

#ifndef NO_JOYPAD
#if defined ALLEGRO_IPHONE || defined ALLEGRO_MACOSX
#include "joypad.hpp"
#endif
#endif

#ifdef ALLEGRO_ANDROID
#include "java.h"
#endif

extern "C" {
void lock_joypad_mutex();
void unlock_joypad_mutex();
}


TripleInput *tripleInput = NULL;

double drop_input_events_older_than = 0;

InputDescriptor EMPTY_INPUT_DESCRIPTOR = {
	false, false, false, false,
	false, false, false,
	DIRECTION_NONE
};

std::list<INPUT_EVENT> input_events;
int num_joystick_buttons = 0;
INPUT_EVENT EMPTY_INPUT_EVENT = { false, NONE, };
int joy_axes[2] = { 0, 0 };
bool next_input_event_ready = false;
ALLEGRO_MUTEX *input_event_mutex;
long joystick_initial_repeat_countdown[JOY_NUM_REPEATABLE] = { JOY_INITIAL_REPEAT_TIME, };
long joystick_repeat_countdown[JOY_NUM_REPEATABLE] = { JOY_REPEAT_TIME, };
bool joystick_repeat_started[JOY_NUM_REPEATABLE] = { false, };
INPUT_EVENT joystick_repeat_events[JOY_NUM_REPEATABLE] = { EMPTY_INPUT_EVENT, };

static INPUT_EVENT ie = EMPTY_INPUT_EVENT;

INPUT_EVENT get_next_input_event()
{
	if (next_input_event_ready) {
		al_lock_mutex(input_event_mutex);
		if (input_events.size() == 0) {
			ie = EMPTY_INPUT_EVENT;
		}
		else {
			ie = input_events.front();
			input_events.pop_front();
		}
		al_unlock_mutex(input_event_mutex);
		next_input_event_ready = false;
	}

	return ie;
}

void add_input_event(INPUT_EVENT ie)
{
	al_lock_mutex(input_event_mutex);

	ie.timestamp = al_get_time();
	input_events.push_back(ie);

	al_unlock_mutex(input_event_mutex);
}

void use_input_event()
{
	al_lock_mutex(input_event_mutex);

	ie = EMPTY_INPUT_EVENT;

	al_unlock_mutex(input_event_mutex);
}

void clear_input_events(double older_than)
{
	if (older_than < 0) {
		drop_input_events_older_than = al_get_time();
	}
	else {
		drop_input_events_older_than = older_than;
	}

	while (input_events.size() > 0) {
		INPUT_EVENT &ie = *input_events.begin();
		if (ie.timestamp >= drop_input_events_older_than) {
			break;
		}
		input_events.erase(input_events.begin());
	}

	for (int i = 0; i < JOY_NUM_REPEATABLE; i++) {
		joystick_initial_repeat_countdown[i] = JOY_INITIAL_REPEAT_TIME;
		joystick_repeat_countdown[i] = JOY_REPEAT_TIME;
		joystick_repeat_started[i] = false;
		joystick_repeat_events[i] = EMPTY_INPUT_EVENT;
	}

	Input *i = getInput();
	if (i) {
		i->reset();
	}

	clear_touches();

#ifndef NO_JOYPAD
#if defined ALLEGRO_IPHONE || defined ALLEGRO_MACOSX
	reset_joypad_state();
#endif
#endif

	released = true;
}

void waitForRelease(int what)
{
	Input *i = getInput();
	InputDescriptor id;
	while (true) {
		pump_events();
		id = i->getDescriptor();
		switch (what) {
			case 0:
				if (!id.left)
					goto done;
				break;
			case 1:
				if (!id.right)
					goto done;
				break;
			case 2:
				if (!id.up)
					goto done;
				break;
			case 3:
				if (!id.down)
					goto done;
				break;
			case 4:
				if (!id.button1)
					goto done;
				break;
			case 5:
				if (!id.button2)
					goto done;
				break;
			case 6:
				if (!id.button3)
					goto done;
				break;
		}
	}

done:;
}

void Input::set(bool l, bool r, bool u, bool d, int set_sets)
{
	al_lock_mutex(mutex);
	
	if (!l && !r && !u && !d) {
		startDirection = DIRECTION_NONE;
		descriptor.left = false;
		descriptor.right = false;
		descriptor.up = false;
		descriptor.down = false;
	}
	else {
		bool startDirectionStillPressed = false;
		if ((l && (startDirection == DIRECTION_WEST)) ||
				((r && startDirection == DIRECTION_EAST)) ||
				((u && startDirection == DIRECTION_NORTH)) ||
				((d && startDirection == DIRECTION_SOUTH))) {
			startDirectionStillPressed = true;
		}
		else {
			startDirection = DIRECTION_NONE;
		}
		if (startDirectionStillPressed)
			descriptor.direction = startDirection;
		else {
			if (l)
				startDirection = DIRECTION_WEST;
			else if (r)
				startDirection = DIRECTION_EAST;
			else if (u)
				startDirection = DIRECTION_NORTH;
			else if (d)
				startDirection = DIRECTION_SOUTH;
			descriptor.direction = startDirection;
		}
		descriptor.left = l;
		descriptor.right = r;
		descriptor.up = u;
		descriptor.down = d;
	}

	int button_states[7] = {
		l, r, u, d,
		1, 1, 1
	};
	
	if (orRelease >= 0) {
		if (!button_states[orRelease]) {
			timeOfNextNotification = tguiCurrentTimeMillis();
			orRelease = -1;
		}
	}

	if (set_sets) {
		sets.left = l;
		sets.right = r;
		sets.up = u;
		sets.down = d;
	}
	
	al_unlock_mutex(mutex);
}

void Input::set(bool b1, bool b2, bool b3, int set_sets)
{
	al_lock_mutex(mutex);

	descriptor.button1 = b1;
	descriptor.button2 = b2;
	descriptor.button3 = b3;

	int button_states[7] = {
		1, 1, 1, 1,
		b1, b2, b3
	};
	
	if (orRelease >= 0) {
		if (!button_states[orRelease]) {
			timeOfNextNotification = tguiCurrentTimeMillis();
			orRelease = -1;
		}
	}

	if (set_sets) {
		sets.button1 = b1;
		sets.button2 = b2;
		sets.button3 = b3;
	}
	
	al_unlock_mutex(mutex);
}

void Input::set(bool l, bool r, bool u, bool d, bool b1, bool b2, bool b3, int set_sets)
{
	al_lock_mutex(mutex);
	set(l, r, u, d, set_sets);
	set(b1, b2, b3, set_sets);
	al_unlock_mutex(mutex);
}

InputDescriptor Input::getDescriptor()
{
	al_lock_mutex(mutex);
	
	update();

	if (tguiCurrentTimeMillis() < timeOfNextNotification || orRelease != -1) {
		descriptor.left =
		descriptor.right =
		descriptor.up =
		descriptor.down =
		descriptor.button1 =
		descriptor.button2 =
		descriptor.button3 = false;
	}
	else {
		orRelease = -1;
	}
	
	InputDescriptor i = descriptor;

	al_unlock_mutex(mutex);
	
	return i;
}

void Input::setDirection(int direction)
{
	descriptor.direction = direction;

	TripleInput *ti = dynamic_cast<TripleInput *>(this);
	if (ti) {
		if (ti->kb)
			ti->kb->setDirection(direction);
		if (ti->js)
			ti->js->setDirection(direction);
	}
}
	
void Input::waitForReleaseOr(int button_id, unsigned long wait_time)
{
	lock_joypad_mutex();

	orRelease = button_id;
	timeOfNextNotification = tguiCurrentTimeMillis() + wait_time;

	unlock_joypad_mutex();
}

void Input::setTimeTillNextNotification(int t)
{
	timeOfNextNotification = tguiCurrentTimeMillis() + t;
	orRelease = -1;
}


unsigned long Input::getTimeOfNextNotification()
{
	return timeOfNextNotification;
}



Input::Input()
{
	descriptor.left = false;
	descriptor.right = false;
	descriptor.up = false;
	descriptor.down = false;
	descriptor.button1 = false;
	descriptor.button2 = false;
	descriptor.button3 = false;
	sets = descriptor;
	descriptor.direction = DIRECTION_SOUTH;
	startDirection = DIRECTION_NONE;
	timeOfNextNotification = tguiCurrentTimeMillis();
	orRelease = -1;
	mutex = al_create_mutex_recursive();
}

void KeyboardInput::reset()
{
	descriptor.left =
	descriptor.right =
	descriptor.up =
	descriptor.down =
	descriptor.button1 =
	descriptor.button2 =
	descriptor.button3 = false;
	sets = descriptor;
	timeOfNextNotification = tguiCurrentTimeMillis();
	orRelease = -1;

	left =
	right =
	up =
	down =
	button1 =
	button2 =
	button3 = false;
}

void KeyboardInput::update()
{
}

void KeyboardInput::handle_event(ALLEGRO_EVENT *event)
{
	if (event->type != ALLEGRO_EVENT_KEY_DOWN && event->type != ALLEGRO_EVENT_KEY_UP && event->type != USER_KEY_DOWN && event->type != USER_KEY_UP)
		return;
	
	int keycode = event->keyboard.keycode;
	bool onoff = event->type == ALLEGRO_EVENT_KEY_DOWN || event->type == USER_KEY_DOWN;

	if (keycode == config.getKey1()) {
		 button1 = onoff;
	}
	else if (keycode == config.getKey2()) {
		 button2 = onoff;
	}
	else if (keycode == config.getKey3()) {
		 button3 = onoff;
	}
	else if (keycode == config.getKeyLeft()) {
		 left = onoff;
	}
	else if (keycode == config.getKeyRight()) {
		 right = onoff;
	}
	else if (keycode == config.getKeyUp()) {
		 up = onoff;
	}
	else if (keycode == config.getKeyDown()) {
		 down = onoff;
	}

	if (!isOuya()) {
		/* X does weird crap with my macbook keyboard. Holding space
		 * makes left and right return home/end. This fixes it.
		 */
		if (keycode == ALLEGRO_KEY_HOME) {
			if (onoff) {
				right = false;
				left = true;
			}
			else {
				left = false;
			}
		}
		else if (keycode == ALLEGRO_KEY_END) {
			if (onoff) {
				left = false;
				right = true;
			}
			else {
				right = false;
			}
		}
	}

	set(left, right, up, down, button1, button2, button3);
}

void GamepadInput::reset()
{
	descriptor.left =
	descriptor.right =
	descriptor.up =
	descriptor.down =
	descriptor.button1 =
	descriptor.button2 =
	descriptor.button3 = false;
	sets = descriptor;
	timeOfNextNotification = tguiCurrentTimeMillis();
	orRelease = -1;

	left =
	right =
	up =
	down =
	button1 =
	button2 =
	button3 = false;
}

void GamepadInput::update()
{
}

void GamepadInput::handle_event(ALLEGRO_EVENT *event)
{
	if (!config.getGamepadAvailable())
		return;

	if (user_joystick == NULL || event->joystick.id != user_joystick)
		return;

	if (event->type == ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN) {
		if (event->joystick.button == config.getJoyButton1()) {
			button1 = true;
		}
		else if (event->joystick.button == config.getJoyButton2()) {
			button2 = true;
		}
		else if (event->joystick.button == config.getJoyButton3()) {
			button3 = true;
		}
	}
	else if (event->type == ALLEGRO_EVENT_JOYSTICK_BUTTON_UP) {
		if (event->joystick.button == config.getJoyButton1()) {
			button1 = false;
		}
		else if (event->joystick.button == config.getJoyButton2()) {
			button2 = false;
		}
		else if (event->joystick.button == config.getJoyButton3()) {
			button3 = false;
		}
	}
	else if (event->type == ALLEGRO_EVENT_JOYSTICK_AXIS) {
		float pos = event->joystick.pos;
		if (event->joystick.axis == 0) {
			if (pos < -0.35) {
				left = true;
				right = false;
			}
			else if (pos > 0.35) {
				right = true;
				left = false;
			}
			else {
				left = right = false;
			}
		}
		else if (event->joystick.axis == 1) {
			if (pos < -0.35) {
				up = true;
				down = false;
			}
			else if (pos > 0.35) {
				down = true;
				up = false;
			}
			else {
				up = down = false;
			}
		}
	}

	set(left, right, up, down, button1, button2, button3);
}

void TripleInput::reset()
{
	descriptor.left =
	descriptor.right =
	descriptor.up =
	descriptor.down =
	descriptor.button1 =
	descriptor.button2 =
	descriptor.button3 = false;
	sets = descriptor;
	timeOfNextNotification = tguiCurrentTimeMillis();
	orRelease = -1;

	if (kb) kb->reset();
	if (js) js->reset();
}

TripleInput::TripleInput() :
	Input()
{
	playerControlled = true;
#if !defined ALLEGRO_IPHONE
	js = new GamepadInput();
#else
	js = NULL;
#endif

	kb = new KeyboardInput();
 }

void TripleInput::update()
{
	al_lock_mutex(mutex);
	
#if !defined ALLEGRO_IPHONE
	kb->update();
	js->update();
	
	InputDescriptor id1 = kb->getDescriptor();

	InputDescriptor id2;
	if (js)
		id2 = js->getDescriptor();
	else
		id2.left = id2.right = id2.up = id2.down =
			id2.button1 = id2.button2 = id2.button3 = 
			false;

	InputDescriptor id3;
#ifndef NO_JOYPAD
#if defined ALLEGRO_MACOSX
	if (joypad_connected())
		id3 = get_joypad_state();
	else
#endif
#endif
		id3.left = id3.right = id3.up = id3.down =
			id3.button1 = id3.button2 = id3.button3 =
			false;

	set(
		sets.left || id1.left || id2.left || id3.left,
		sets.right || id1.right || id2.right || id3.right,
		sets.up || id1.up || id2.up || id3.up,
		sets.down || id1.down || id2.down || id3.down,
		sets.button1 || id1.button1 || id2.button1 || id3.button1,
		sets.button2 || id1.button2 || id2.button2 || id3.button2,
		sets.button3 || id1.button3 || id2.button3 || id3.button3,
		false
	);
#elif defined ALLEGRO_IPHONE
	kb->update();
	
	InputDescriptor id1 = kb->getDescriptor();

	InputDescriptor id3;
#ifndef NO_JOYPAD
	if (joypad_connected()) {
		id3 = get_joypad_state();
	}
	else
#endif
	{
		id3.left = id3.right = id3.up = id3.down = id3.button1 = id3.button2 = id3.button3 = false;
	}

	InputDescriptor id4;
#if defined WITH_60BEAT
	get_sb_state(&id4.left, &id4.right, &id4.up, &id4.down, &id4.button1, &id4.button2, &id4.button3);
#else
	id4.left = id4.right = id4.up = id4.down = id4.button1 = id4.button2 = id4.button3 = false;
#endif
		
	set(
	    sets.left || id1.left || id3.left || id4.left,
	    sets.right || id1.right || id3.right || id4.right,
	    sets.up || id1.up || id3.up || id4.up,
	    sets.down || id1.down || id3.down || id4.down,
	    sets.button1 || id1.button1 || id3.button1 || id4.button1,
	    sets.button2 || id1.button2 || id3.button2 || id4.button2,
	    sets.button3 || id1.button3 || id3.button3 || id4.button3,
	    false
	    );
#endif

	al_unlock_mutex(mutex);
}

void initInput()
{
	tripleInput = new TripleInput();
}


void destroyInput()
{
	delete tripleInput;
	tripleInput = NULL;
}


TripleInput *getInput()
{
	return tripleInput;
}

ScriptInput::ScriptInput()
{ playerControlled = false; }

void ScriptInput::update()
{}

void joy_b1_down(bool skip_initial_event, bool long_delay)
{
	INPUT_EVENT ie = EMPTY_INPUT_EVENT;
	ie.button1 = DOWN;
	if (!skip_initial_event) {
		add_input_event(ie);
	}
	joystick_repeat_started[JOY_REPEAT_B1] = true;
	joystick_repeat_events[JOY_REPEAT_B1] = ie;
	if (skip_initial_event) {
		joystick_initial_repeat_countdown[JOY_REPEAT_B1] = long_delay ? JOY_INITIAL_REPEAT_TIME : JOY_REPEAT_TIME;
	}
	else {
		joystick_initial_repeat_countdown[JOY_REPEAT_B1] = JOY_INITIAL_REPEAT_TIME;
	}
	joystick_repeat_countdown[JOY_REPEAT_B1] = JOY_REPEAT_TIME;

	blueblock_times[4] = al_get_time();
}

void joy_b2_down(bool skip_initial_event, bool long_delay)
{
	INPUT_EVENT ie = EMPTY_INPUT_EVENT;
	ie.button2 = DOWN;
	if (!skip_initial_event) {
		add_input_event(ie);
	}
	joystick_repeat_started[JOY_REPEAT_B2] = true;
	joystick_repeat_events[JOY_REPEAT_B2] = ie;
	if (skip_initial_event) {
		joystick_initial_repeat_countdown[JOY_REPEAT_B2] = long_delay ? JOY_INITIAL_REPEAT_TIME : JOY_REPEAT_TIME;
	}
	else {
		joystick_initial_repeat_countdown[JOY_REPEAT_B2] = JOY_INITIAL_REPEAT_TIME;
	}
	joystick_repeat_countdown[JOY_REPEAT_B2] = JOY_REPEAT_TIME;

	blueblock_times[5] = al_get_time();
}

void joy_b3_down(bool skip_initial_event, bool long_delay)
{
	INPUT_EVENT ie = EMPTY_INPUT_EVENT;
	ie.button3 = DOWN;
	if (!skip_initial_event) {
		add_input_event(ie);
	}
	joystick_repeat_started[JOY_REPEAT_B3] = true;
	joystick_repeat_events[JOY_REPEAT_B3] = ie;
	if (skip_initial_event) {
		joystick_initial_repeat_countdown[JOY_REPEAT_B3] = long_delay ? JOY_INITIAL_REPEAT_TIME : JOY_REPEAT_TIME;
	}
	else {
		joystick_initial_repeat_countdown[JOY_REPEAT_B3] = JOY_INITIAL_REPEAT_TIME;
	}
	joystick_repeat_countdown[JOY_REPEAT_B3] = JOY_REPEAT_TIME;

	blueblock_times[6] = al_get_time();
}

void joy_b1_up()
{
	INPUT_EVENT ie = EMPTY_INPUT_EVENT;
	ie.button1 = UP;
	add_input_event(ie);
	joystick_repeat_started[JOY_REPEAT_B1] = false;
	joystick_initial_repeat_countdown[JOY_REPEAT_B1] = JOY_INITIAL_REPEAT_TIME;
	joystick_repeat_countdown[JOY_REPEAT_B1] = JOY_REPEAT_TIME;
	dpad_panning = false;
}

void joy_b2_up()
{
	INPUT_EVENT ie = EMPTY_INPUT_EVENT;
	ie.button2 = UP;
	add_input_event(ie);
	joystick_repeat_started[JOY_REPEAT_B2] = false;
	joystick_initial_repeat_countdown[JOY_REPEAT_B2] = JOY_INITIAL_REPEAT_TIME;
	joystick_repeat_countdown[JOY_REPEAT_B2] = JOY_REPEAT_TIME;
}

void joy_b3_up()
{
	INPUT_EVENT ie = EMPTY_INPUT_EVENT;
	ie.button3 = UP;
	add_input_event(ie);
	joystick_repeat_started[JOY_REPEAT_B3] = false;
	joystick_initial_repeat_countdown[JOY_REPEAT_B3] = JOY_INITIAL_REPEAT_TIME;
	joystick_repeat_countdown[JOY_REPEAT_B3] = JOY_REPEAT_TIME;
}

void joy_l_down(bool skip_initial_event, bool long_delay)
{
	INPUT_EVENT ie = EMPTY_INPUT_EVENT;
	ie.left = DOWN;
	if (!skip_initial_event) {
		add_input_event(ie);
	}
	joystick_repeat_started[JOY_REPEAT_AXIS0] = true;
	joystick_repeat_events[JOY_REPEAT_AXIS0] = ie;
	if (skip_initial_event) {
		joystick_initial_repeat_countdown[JOY_REPEAT_AXIS0] = long_delay ? JOY_INITIAL_REPEAT_TIME : JOY_REPEAT_TIME;
	}
	else {
		joystick_initial_repeat_countdown[JOY_REPEAT_AXIS0] = JOY_INITIAL_REPEAT_TIME;
	}
	joystick_repeat_countdown[JOY_REPEAT_AXIS0] = JOY_REPEAT_TIME;

	blueblock_times[0] = al_get_time();
}

void joy_r_down(bool skip_initial_event, bool long_delay)
{
	INPUT_EVENT ie = EMPTY_INPUT_EVENT;
	ie.right = DOWN;
	if (!skip_initial_event) {
		add_input_event(ie);
	}
	joystick_repeat_started[JOY_REPEAT_AXIS0] = true;
	joystick_repeat_events[JOY_REPEAT_AXIS0] = ie;
	if (skip_initial_event) {
		joystick_initial_repeat_countdown[JOY_REPEAT_AXIS0] = long_delay ? JOY_INITIAL_REPEAT_TIME : JOY_REPEAT_TIME;
	}
	else {
		joystick_initial_repeat_countdown[JOY_REPEAT_AXIS0] = JOY_INITIAL_REPEAT_TIME;
	}
	joystick_repeat_countdown[JOY_REPEAT_AXIS0] = JOY_REPEAT_TIME;

	blueblock_times[1] = al_get_time();
}

void joy_u_down(bool skip_initial_event, bool long_delay)
{
	INPUT_EVENT ie = EMPTY_INPUT_EVENT;
	ie.up = DOWN;
	if (!skip_initial_event) {
		add_input_event(ie);
	}
	joystick_repeat_started[JOY_REPEAT_AXIS1] = true;
	joystick_repeat_events[JOY_REPEAT_AXIS1] = ie;
	if (skip_initial_event) {
		joystick_initial_repeat_countdown[JOY_REPEAT_AXIS1] = long_delay ? JOY_INITIAL_REPEAT_TIME : JOY_REPEAT_TIME;
	}
	else {
		joystick_initial_repeat_countdown[JOY_REPEAT_AXIS1] = JOY_INITIAL_REPEAT_TIME;
	}
	joystick_repeat_countdown[JOY_REPEAT_AXIS1] = JOY_REPEAT_TIME;

	blueblock_times[2] = al_get_time();
}

void joy_d_down(bool skip_initial_event, bool long_delay)
{
	INPUT_EVENT ie = EMPTY_INPUT_EVENT;
	ie.down = DOWN;
	if (!skip_initial_event) {
		add_input_event(ie);
	}
	joystick_repeat_started[JOY_REPEAT_AXIS1] = true;
	joystick_repeat_events[JOY_REPEAT_AXIS1] = ie;
	if (skip_initial_event) {
		joystick_initial_repeat_countdown[JOY_REPEAT_AXIS1] = long_delay ? JOY_INITIAL_REPEAT_TIME : JOY_REPEAT_TIME;
	}
	else {
		joystick_initial_repeat_countdown[JOY_REPEAT_AXIS1] = JOY_INITIAL_REPEAT_TIME;
	}
	joystick_repeat_countdown[JOY_REPEAT_AXIS1] = JOY_REPEAT_TIME;

	blueblock_times[3] = al_get_time();
}

void joy_l_up()
{
	INPUT_EVENT ie = EMPTY_INPUT_EVENT;
	ie.left = UP;
	add_input_event(ie);
	joystick_repeat_started[JOY_REPEAT_AXIS0] = false;
	joystick_initial_repeat_countdown[JOY_REPEAT_AXIS0] = JOY_INITIAL_REPEAT_TIME;
	joystick_repeat_countdown[JOY_REPEAT_AXIS0] = JOY_REPEAT_TIME;
}

void joy_r_up()
{
	INPUT_EVENT ie = EMPTY_INPUT_EVENT;
	ie.right = UP;
	add_input_event(ie);
	joystick_repeat_started[JOY_REPEAT_AXIS0] = false;
	joystick_initial_repeat_countdown[JOY_REPEAT_AXIS0] = JOY_INITIAL_REPEAT_TIME;
	joystick_repeat_countdown[JOY_REPEAT_AXIS0] = JOY_REPEAT_TIME;
}

void joy_u_up()
{
	INPUT_EVENT ie = EMPTY_INPUT_EVENT;
	ie.up = UP;
	add_input_event(ie);
	joystick_repeat_started[JOY_REPEAT_AXIS1] = false;
	joystick_initial_repeat_countdown[JOY_REPEAT_AXIS1] = JOY_INITIAL_REPEAT_TIME;
	joystick_repeat_countdown[JOY_REPEAT_AXIS1] = JOY_REPEAT_TIME;
}

void joy_d_up()
{
	INPUT_EVENT ie = EMPTY_INPUT_EVENT;
	ie.down = UP;
	add_input_event(ie);
	joystick_repeat_started[JOY_REPEAT_AXIS1] = false;
	joystick_initial_repeat_countdown[JOY_REPEAT_AXIS1] = JOY_INITIAL_REPEAT_TIME;
	joystick_repeat_countdown[JOY_REPEAT_AXIS1] = JOY_REPEAT_TIME;
}
