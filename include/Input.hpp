#ifndef INPUT_H
#define INPUT_H

#include "monster2.hpp"

#include "input_descriptor.h"

class Input {
public:
	void set(bool l, bool r, bool u, bool d, int set_sets = true);
	void set(bool b1, bool b2, bool b3, int set_sets = true);
	void set(bool l, bool r, bool u, bool d, bool b1, bool b2, bool b3, int set_sets = true);
	InputDescriptor getDescriptor();
	void setDirection(int direction);
	int getDirection() { return descriptor.direction; }
	bool isPlayerControlled() { return playerControlled; }
	void setTimeTillNextNotification(int t);
	unsigned long getTimeOfNextNotification();
	void waitForReleaseOr(int button_id, unsigned long wait_time);
	Input();
	virtual ~Input() {
		al_destroy_mutex(mutex);
	}
	virtual void reset() {}

	void dump() {
		printf("Input dump:\n");
		printf("%d %d %d %d\n",
			descriptor.left,
			descriptor.right,
			descriptor.up,
			descriptor.down);
		printf("%d %d %d\n",
			descriptor.button1,
			descriptor.button2,
			descriptor.button3);
		printf("%ld %d\n",
			timeOfNextNotification,
			orRelease);
		printf("now = %ld\n", tguiCurrentTimeMillis());
	}

protected:
	virtual void update() = 0; 

	InputDescriptor descriptor;
	InputDescriptor sets;
	bool playerControlled;
	int startDirection;
	unsigned long timeOfNextNotification;
	int orRelease;
	ALLEGRO_MUTEX *mutex;
};

class KeyboardInput : public Input {
	friend class TripleInput;
public:
	void handle_event(ALLEGRO_EVENT *event);
	virtual void reset();

	KeyboardInput() {
		playerControlled = true;
		left = right = up = down = button1 = button2 = button3 = false;
		set(false, false, false, false, false, false, false);
	}
	virtual ~KeyboardInput() {}
protected:
	void update();
	bool left, right, up, down, button1, button2, button3;
};

class GamepadInput : public Input {
	friend class TripleInput;
public:
	void reconfig() {
		if (config.getGamepadAvailable())
			joystick = al_get_joystick(0);
		else
			joystick = NULL;
		set(false, false, false, false, false, false, false);
	}

	void handle_event(ALLEGRO_EVENT *event);
	virtual void reset();

	GamepadInput() {
		playerControlled = true;
		reconfig();
		left = right = up = down = button1 = button2 = button3 = false;
	}
	virtual ~GamepadInput() {}
protected:
	void update();
	ALLEGRO_JOYSTICK *joystick;
	bool left, right, up, down, button1, button2, button3;
};

class TripleInput : public Input {
public:
	friend void Input::setDirection(int direction);

	void reconfig() {
		if (js)
			js->reconfig();
	}

	void handle_event(ALLEGRO_EVENT *event) {
		if (kb)
			kb->handle_event(event);
		if (js)
			js->handle_event(event);
	}

	virtual void reset();

	TripleInput();
	virtual ~TripleInput() {
		if (kb)
			delete kb;
		if (js)
			delete js;
		kb = 0;
		js = 0;
	}
private:
	void update();
	KeyboardInput *kb;
	GamepadInput *js;
};

class ScriptInput : public Input {
public:
    ScriptInput();
protected:
    void update();
};


/* new input stuff */
enum INPUT_EVENT_TYPE {
	NONE = 0,
	DOWN,
	UP
};

struct INPUT_EVENT {
	bool repeat;
	INPUT_EVENT_TYPE left;
	INPUT_EVENT_TYPE right;
	INPUT_EVENT_TYPE up;
	INPUT_EVENT_TYPE down;
	INPUT_EVENT_TYPE button1;
	INPUT_EVENT_TYPE button2;
	INPUT_EVENT_TYPE button3;
	double timestamp;
};

INPUT_EVENT get_next_input_event();
void add_input_event(INPUT_EVENT ie);
void use_input_event();
void clear_input_events(double older_than = -1.0);
void waitForRelease(int what);

const long JOY_INITIAL_REPEAT_TIME = 500;
const long JOY_REPEAT_TIME = 100;
enum {
	JOY_REPEAT_B1 = 0,
	JOY_REPEAT_B2,
	JOY_REPEAT_B3,
	JOY_REPEAT_AXIS0,
	JOY_REPEAT_AXIS1,
	JOY_NUM_REPEATABLE
};

extern InputDescriptor EMPTY_INPUT_DESCRIPTOR;

extern std::list<INPUT_EVENT> input_events;
extern int num_joystick_buttons;
extern INPUT_EVENT EMPTY_INPUT_EVENT;
extern int joy_axes[2];
extern bool next_input_event_ready;
extern ALLEGRO_MUTEX *input_event_mutex;
extern long joystick_initial_repeat_countdown[JOY_NUM_REPEATABLE];
extern long joystick_repeat_countdown[JOY_NUM_REPEATABLE];
extern bool joystick_repeat_started[JOY_NUM_REPEATABLE];
extern INPUT_EVENT joystick_repeat_events[JOY_NUM_REPEATABLE];
/*-------------*/

void gamepad2Keypresses(int step);
void initInput();
void destroyInput();
TripleInput *getInput();

extern TripleInput *tripleInput; // too lazy to change the name now

extern double drop_input_events_older_than;

extern "C" {
void joy_b1_down(bool skip_initial_event = false, bool long_delay = false);
void joy_b2_down(bool skip_initial_event = false, bool long_delay = false);
void joy_b3_down(bool skip_initial_event = false, bool long_delay = false);
void joy_b1_up();
void joy_b2_up();
void joy_b3_up();
void joy_l_down(bool skip_initial_event = false, bool long_delay = false);
void joy_r_down(bool skip_initial_event = false, bool long_delay = false);
void joy_u_down(bool skip_initial_event = false, bool long_delay = false);
void joy_d_down(bool skip_initial_event = false, bool long_delay = false);
void joy_l_up();
void joy_r_up();
void joy_u_up();
void joy_d_up();
}
#endif

