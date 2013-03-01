#include <vector>
#include <algorithm>

#include <allegro5/allegro5.h>

#ifndef ALLEGRO_WINDOWS
#include <sys/time.h>
#endif

#include "tgui.hpp"

#include "monster2.hpp"

struct TGUI {
	int focus;
	std::vector<TGUIWidget*> widgets;
};

static std::vector<TGUI*> tguiStack;
static TGUI* activeGUI = 0;
static long tguiLastUpdate;
static bool tguiMouseReleased = true;
static TGUIWidget* tguiCurrentParent = 0;
static TGUIWidget* tguiClickedWidget = 0;

static std::vector<int> keycodeBuffer;
static std::vector<int> unicharBuffer;

static bool ignore_hot_zone = false;

/*
 * The widget that was clicked on (or 0) and where it was clicked.
 */
TGUIWidget* tguiActiveWidget = 0;
TGUIPoint tguiActiveWidgetClickedPoint; // relative to widget
int tguiMouseButton;
TGUIPoint tguiMouseReleasePoint;

static int tguiScreenWidth = 0;
static int tguiScreenHeight = 0;
static int tgui_screen_offset_x = 0;
static int tgui_screen_offset_y = 0;
static float tgui_screen_ratio_x = 1.0;
static float tgui_screen_ratio_y = 1.0;

static ALLEGRO_EVENT_QUEUE *key_events;
static ALLEGRO_EVENT_QUEUE *mouse_events;
struct MSESTATE {
	int x, y, buttons;
};

MSESTATE tguiMouseState;

static std::vector<int> pressedHotkeys;

struct MouseEvent {
	int b, x, y, z;
};

static std::vector<MouseEvent *> mouseDownEvents;
static std::vector<MouseEvent *> mouseUpEvents;
static std::vector<MouseEvent *> mouseMoveEvents;

static bool event_queues_created = false;

static float x_scale = 1;
static float y_scale = 1;
static int tolerance = 0;

static int ignore = 0;
static int mouse_downs = 0;

static int rotation = 0;

static std::vector<TGUIWidget *> disabledWidgets;

#if defined ALLEGRO_ANDROID || defined ALLEGRO_IPHONE
#define ALLEGRO_EVENT_MOUSE_BUTTON_DOWN ALLEGRO_EVENT_TOUCH_BEGIN
#define ALLEGRO_EVENT_MOUSE_BUTTON_UP ALLEGRO_EVENT_TOUCH_END
#define ALLEGRO_EVENT_MOUSE_AXES ALLEGRO_EVENT_TOUCH_MOVE
#endif

#define KEEP_TIME 1.0

static void *queue_emptier(void *crap)
{
	int own_mouse_downs = 0;

	(void)crap;

	while (event_queues_created) {
		ALLEGRO_EVENT event;
		if (al_peek_next_event(key_events, &event)) {
			if (event.any.timestamp+KEEP_TIME < al_current_time()) {
				al_drop_next_event(key_events);
			}
			if (event.type == USER_KEY_DOWN || event.type == USER_KEY_UP || event.type == USER_KEY_CHAR) {
				al_unref_user_event((ALLEGRO_USER_EVENT *)&event);
			}
		}
		if (al_peek_next_event(mouse_events, &event)) {
			if (event.any.timestamp+KEEP_TIME < al_current_time()) {
				if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
					own_mouse_downs++;
				}
				if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
					if (own_mouse_downs > 0) {
						own_mouse_downs--;
					}
					else if (mouse_downs > 0) {
						mouse_downs--;
					}
				}
				al_drop_next_event(mouse_events);
			}
			if (event.type == USER_KEY_DOWN || event.type == USER_KEY_UP || event.type == USER_KEY_CHAR) {
				al_unref_user_event((ALLEGRO_USER_EVENT *)&event);
			}
		}
		al_rest(0.001);
	}

	return NULL;
}

static void tguiFindFocus()
{
	for (int i = 0; i < (int)activeGUI->widgets.size(); i++) {
		if (activeGUI->widgets[i]->getFocus()) {
			activeGUI->focus = i;
			return;
		}
	}
}


/*
 * Returns true if the shift keys are pressed, otherwise false.
 */
static bool tguiShiftsPressed(ALLEGRO_KEYBOARD_STATE *kbdstate, int shifts)
{
#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_ANDROID
	if (shifts & TGUI_KEYFLAG_SHIFT) {
		if (!al_key_down(kbdstate, ALLEGRO_KEY_LSHIFT) &&
				!al_key_down(kbdstate, ALLEGRO_KEY_RSHIFT))
			return false;
	}
	else {
		if (shifts & TGUI_KEYFLAG_LSHIFT) {
			if (!al_key_down(kbdstate, ALLEGRO_KEY_LSHIFT))
				return false;
		}
		if (shifts & TGUI_KEYFLAG_RSHIFT) {
			if (!al_key_down(kbdstate, ALLEGRO_KEY_RSHIFT))
				return false;
		}
	}
	if (shifts & TGUI_KEYFLAG_CONTROL) {
		if (!al_key_down(kbdstate, ALLEGRO_KEY_LCTRL) &&
				!al_key_down(kbdstate, ALLEGRO_KEY_RCTRL))
			return false;
	}
	else {
		if (shifts & TGUI_KEYFLAG_LCONTROL) {
			if (!al_key_down(kbdstate, ALLEGRO_KEY_LCTRL))
				return false;
		}
		if (shifts & TGUI_KEYFLAG_RCONTROL) {
			if (!al_key_down(kbdstate, ALLEGRO_KEY_RCTRL))
				return false;
		}
	}
	if (shifts & TGUI_KEYFLAG_ALT) {
		if (!al_key_down(kbdstate, ALLEGRO_KEY_ALT) &&
				!al_key_down(kbdstate, ALLEGRO_KEY_ALTGR))
			return false;
	}
	else {
		if (shifts & TGUI_KEYFLAG_LALT) {
			if (!al_key_down(kbdstate, ALLEGRO_KEY_ALT))
				return false;
		}
		if (shifts & TGUI_KEYFLAG_RALT) {
			if (!al_key_down(kbdstate, ALLEGRO_KEY_ALTGR))
				return false;
		}
	}

	return true;
#else
	return false;
#endif
}

/*
 * Returns true if a hotkey is pressed, otherwise false.
 */
static bool tguiHotkeyPressed(int hotkey)
{
#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_ANDROID
	int shifts = tguiGetHotkeyFlags(hotkey);
	int k = tguiGetHotkeyKey(hotkey);

	ALLEGRO_KEYBOARD_STATE kbdstate;

	al_get_keyboard_state(&kbdstate);

	if (tguiShiftsPressed(&kbdstate, shifts) && al_key_down(&kbdstate, k))
		return true;
#endif
	return false;
}

TGUIWidget::TGUIWidget()
{
	focus = false;
	allClear = true;
}

/*
 * Returns the widget that the pixel x, y fall on,
 * or 0 if none is found.
 */
static TGUIWidget* tguiFindPixelOwner(int x, int y)
{
	for (int i = (int)activeGUI->widgets.size()-1; i >= 0; i--) {
		TGUIWidget* widget = activeGUI->widgets[i];
		if (tguiPointOnWidget(widget, &x, &y)) {
			return widget;
		}
	}
	return 0;
}

/*
 * Returns true if the point x, y falls on a particular widget.
 */
bool tguiPointOnWidget(TGUIWidget* widget, int *x, int *y)
{
	if (
		(*x > widget->getX() - tolerance) &&
		(*x < widget->getX() + widget->getWidth() + tolerance) &&
		(*y > widget->getY() - tolerance) &&
		(*y < widget->getY() + widget->getHeight() + tolerance)) {
		if (*x < widget->getX())
			*x = widget->getX();
		else if (*x > widget->getX()+widget->getWidth())
			*x = widget->getX()+widget->getWidth()-1;
		if (*y < widget->getY())
			*y = widget->getY();
		else if (*y > widget->getY()+widget->getHeight())
			*y = widget->getY()+widget->getHeight()-1;
		return true;
	}
	return false;
}

unsigned long tguiCurrentTimeMillis()
{
#ifndef ALLEGRO_WINDOWS
	struct timeval tv;
	gettimeofday(&tv, 0);
	return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
#else
	return timeGetTime();
#endif
//#endif
}

bool tguiIsInitialized()
{
	return activeGUI != NULL;
}

/*
 * Delete the active widget set and allocates a blank one.
 * Must be called before any other GUI functions are called.
 */
void tguiInit()
{
	if (activeGUI) {
		tguiDeleteActive();
	}

	activeGUI = new TGUI;
	activeGUI->focus = 0;

	tguiLastUpdate = tguiCurrentTimeMillis();

	if (al_get_current_display()) {
		tguiScreenWidth = al_get_display_width(al_get_current_display());
		tguiScreenHeight = al_get_display_height(al_get_current_display());
	}
	else {
		tguiScreenWidth = 480;
		tguiScreenHeight = 320;
	}

	if (!event_queues_created) {

		mouse_events = al_create_event_queue();
		
		key_events = al_create_event_queue();
#if !defined ALLEGRO_IPHONE
		al_register_event_source(key_events, al_get_keyboard_event_source());
#else
		al_register_event_source(key_events, &user_event_source);
#endif
#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_ANDROID
		if (al_is_mouse_installed()) {
			al_register_event_source(mouse_events, al_get_mouse_event_source());
		}

#else
		al_register_event_source(mouse_events, al_get_touch_input_event_source());
#endif

		event_queues_created = true;

		al_run_detached_thread(queue_emptier, NULL);
	}
}

/*
 * Clean up. You should call tguiDeleteActive first if there
 * are any widgets you haven't cleared up yet.
 */
void tguiShutdown()
{
	if (activeGUI) {
		delete activeGUI;
	}
	tguiStack.clear();
	event_queues_created = false;
	al_destroy_event_queue(key_events);
	if (al_is_mouse_installed()) {
		al_unregister_event_source(mouse_events, al_get_mouse_event_source());
	}
	al_destroy_event_queue(mouse_events);
	al_rest(0.15);
	pressedHotkeys.clear();
}

static bool tgui_force_set_focus = false;

/*
 * Set the widget that has the keyboard focus.
 */
void tguiSetFocus(int widget)
{
	if (activeGUI->widgets.size() <= 0
		|| !activeGUI->widgets[widget]->acceptsFocus()) {
		return;
	}
	if (tguiIsDisabled(activeGUI->widgets[widget]))
		return;
	if (!tgui_force_set_focus) {
		if (activeGUI->widgets[widget] == tguiActiveWidget)
			return;
	}
	tgui_force_set_focus = false;
	activeGUI->widgets[widget]->setFocus(true);
	if (widget != activeGUI->focus
		&& activeGUI->focus >= 0 && activeGUI->focus < (int)activeGUI->widgets.size())
		activeGUI->widgets[activeGUI->focus]->setFocus(false);
	tguiActiveWidget = activeGUI->widgets[widget];
	activeGUI->focus = widget;
}

/*
 * Set the widget that has the keyboard focus.
 */
void tguiSetFocus(TGUIWidget* widget)
{
	if (!widget || !widget->acceptsFocus() || activeGUI->widgets.size() <= 0) {
		tguiActiveWidget = NULL;
		return;
	}
	if (tguiIsDisabled(widget))
		return;
	if (!tgui_force_set_focus && widget == tguiActiveWidget) {
		return;
	}
	tgui_force_set_focus = false;

//	TGUIWidget *oldActive = tguiActiveWidget;

	for (size_t i = 0; i < activeGUI->widgets.size(); i++) {
		TGUIWidget* w = activeGUI->widgets[i];
		if (w == widget) {
			w->setFocus(true);
			if (activeGUI->focus < (int)activeGUI->widgets.size() &&
				w != activeGUI->widgets[activeGUI->focus] &&
				activeGUI->focus >= 0)
				activeGUI->widgets[activeGUI->focus]->setFocus(false);
//			if (oldActive) {
//				oldActive->setFocus(false);
//			}
			activeGUI->focus = (int)i;
			tguiActiveWidget = w;
			return;
		}
	}

	
	tguiActiveWidget = NULL;
}

TGUIWidget* tguiGetFocus()
{
	return tguiActiveWidget;
}

/*
 * Move the focus to the first widget before this currently
 * focused widget that accepts focus.
 */
void tguiFocusPrevious()
{
	int focus = activeGUI->focus-1;

	if (focus >= 0) {
		while (focus >= 0) {
			if (activeGUI->widgets[focus]->acceptsFocus()) {
				if (activeGUI->widgets[focus]->getParent() ==
						activeGUI->widgets[activeGUI->focus]->getParent()) {
					activeGUI->widgets[activeGUI->focus]->setFocus(false);
					activeGUI->widgets[focus]->setFocus(true);
					activeGUI->focus = focus;
					tguiActiveWidget = activeGUI->widgets[focus];
					return;
				}
			}
			focus--;
		}
	}

	// If there is no previous widget, find the last and focus it

	for (focus = (int)activeGUI->widgets.size()-1; focus >= 0; focus--) {
		if (activeGUI->widgets[focus]->acceptsFocus()) {
			if (activeGUI->widgets[focus]->getParent() ==
					activeGUI->widgets[activeGUI->focus]->getParent()) {
				activeGUI->widgets[activeGUI->focus]->setFocus(false);
				activeGUI->widgets[focus]->setFocus(true);
				activeGUI->focus = focus;
				tguiActiveWidget = activeGUI->widgets[focus];
				return;
			}
		}
	}
}

/*
 * Move the focus to the next widget that accepts focus.
 */
void tguiFocusNext()
{
	int focus = activeGUI->focus+1;

	while (focus < (int)activeGUI->widgets.size()) {
		if (activeGUI->widgets[focus]->acceptsFocus()) {
			if (activeGUI->widgets[focus]->getParent() ==
					activeGUI->widgets[activeGUI->focus]->getParent()) {
				activeGUI->widgets[activeGUI->focus]->setFocus(false);
				activeGUI->widgets[focus]->setFocus(true);
				activeGUI->focus = focus;
				tguiActiveWidget = activeGUI->widgets[focus];
				return;
			}
		}
		focus++;
	}

	// If there is no next widget, find the first and focus it

	for (focus = 0; focus < (int)activeGUI->widgets.size(); focus++) {
		if (activeGUI->widgets[focus]->acceptsFocus()) {
			if (activeGUI->widgets[focus]->getParent() ==
					activeGUI->widgets[activeGUI->focus]->getParent()) {
				activeGUI->widgets[activeGUI->focus]->setFocus(false);
				activeGUI->widgets[focus]->setFocus(true);
				activeGUI->focus = focus;
				tguiActiveWidget = activeGUI->widgets[focus];
				return;
			}
		}
	}
}

/*
 * Delete the active set of widgets. You must call tguiInit or
 * tguiPop after this if you are going to use the GUI again.
 */
void tguiDeleteActive()
{
	delete activeGUI;
	activeGUI = 0;
	tguiActiveWidget = 0;
	tguiCurrentParent = 0;
}

/*
 * Move the entire GUI x pixels on the x axis and y on the y axis,
 * with positive x values moving towards the right and positive
 * y values moving down on the screen.
 */
void tguiTranslateAll(int x, int y)
{
	for (size_t i = 0; i < activeGUI->widgets.size(); i++) {
		TGUIWidget* w = activeGUI->widgets[i];
		w->setX(w->getX() + x);
		w->setY(w->getY() + y);
	}
}

/*
 * Like tguiTranslateAll, but only translates a widget and its
 * children.
 */
void tguiTranslateWidget(TGUIWidget* parent, int x, int y)
{
	for (size_t i = 0; i < activeGUI->widgets.size(); i++) {
		TGUIWidget* w = activeGUI->widgets[i];
		if ((parent == w) || (w->getParent() == parent)) {
			int newx;
			int newy;
			if (w->getParent() == parent) {
				newx = w->getParent()->getX() + w->getRelativeX();
				newy = w->getParent()->getY() + w->getRelativeY();
			}
			else if (w->getParent()) {
				if (x < 0)
					newx = MAX(w->getParent()->getX(), w->getX()+x);
				else
					newx = MIN(w->getParent()->getX()+w->getParent()->getWidth()-
							w->getWidth(),
							w->getX()+x);
				if (y < 0)
					newy = MAX(w->getParent()->getY(), w->getY()+y);
				else
					newy = MIN(w->getParent()->getY()+
							w->getParent()->getHeight()-
							w->getHeight(),
							w->getY()+y);
			}
			else {
				newx = w->getX() + x;
				newy = w->getY() + y;
			}
			w->setX(newx);
			w->setY(newy);
		}
	}
}

/*
 * Add a widget to the GUI.
 */
void tguiAddWidget(TGUIWidget* widget)
{
	widget->setParent(tguiCurrentParent);
	if (tguiCurrentParent) {
		widget->setRelativeX(widget->getX() - tguiCurrentParent->getX());
		widget->setRelativeY(widget->getY() - tguiCurrentParent->getY());
	}
	widget->setFocus(false);
	activeGUI->widgets.push_back(widget);
}


/*
 * Update the GUI.
 * Passes all input to the relevant widgets.
 * Must be called frequently after a GUI is created.
 * Returns 0 normally, or if a widget requests,
 * it returns that widget. It's up to the caller to
 * query the widget as to why it wanted to return.
 */
TGUIWidget* tguiUpdate()
{
	if (show_item_info_on_flip >= 0) {
		return NULL;
	}

	long currTime = tguiCurrentTimeMillis();
	long elapsed = currTime - tguiLastUpdate;
	if (elapsed > 50) {
		elapsed = 50;
	}
	tguiLastUpdate = currTime;

#if defined ALLEGRO_ANDROID || defined ALLEGRO_IPHONE
#define EV event.touch
#define BUTTON id
#else
#define EV event.mouse
#define BUTTON button
#endif

	while (!al_event_queue_is_empty(mouse_events)) {
		ALLEGRO_EVENT event;
		al_get_next_event(mouse_events, &event);
		if (event.any.timestamp < drop_input_events_older_than) {
			continue;
		}
		if (!(ignore & TGUI_MOUSE)) {
			if (event.type == ALLEGRO_EVENT_MOUSE_AXES) {
				tguiMouseState.x = EV.x;
				tguiMouseState.y = EV.y;
				MouseEvent *e = new MouseEvent;
				e->x = EV.x;
				e->y = EV.y;
#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_ANDROID
				e->z = EV.z;
#endif
				tguiConvertMousePosition(&e->x, &e->y, tgui_screen_offset_x, tgui_screen_offset_y, tgui_screen_ratio_x, tgui_screen_ratio_y);
				e->b = 0;
				mouseMoveEvents.push_back(e);
			}
			else if (mouse_downs > 0 && event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
				mouse_downs--;
				MouseEvent *e = new MouseEvent;
				e->x = EV.x;
				e->y = EV.y;
				tguiConvertMousePosition(&e->x, &e->y, tgui_screen_offset_x, tgui_screen_offset_y, tgui_screen_ratio_x, tgui_screen_ratio_y);
				e->b = EV.BUTTON;
				mouseUpEvents.push_back(e);
				tguiMouseState.buttons &= (~EV.BUTTON);
			}
			if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
				mouse_downs++;
				int ex = EV.x;
				int ey = EV.y;
				tguiConvertMousePosition(&ex, &ey, tgui_screen_offset_x, tgui_screen_offset_y, tgui_screen_ratio_x, tgui_screen_ratio_y);
				EV.x = ex;
				EV.y = ey;
				/* This check is Monster 2 specific! */
				if (ignore_hot_zone || !(EV.x < 16 && EV.y < 16)) {
					MouseEvent *e = new MouseEvent;
					e->x = EV.x;
					e->y = EV.y;
					e->b = EV.BUTTON;
					mouseDownEvents.push_back(e);
					tguiMouseState.buttons |= EV.BUTTON;
				}
			}
		}
	}

	bool allClear = false;
	Input *i;
	i = getInput();
	if (i) {
		InputDescriptor id;
		id = i->getDescriptor();
		if (
		!id.left &&
		!id.right &&
		!id.up &&
		!id.down &&
		!id.button1 &&
		!id.button2 &&
		!id.button3) {
			allClear = true;
		}
	}

	if (allClear) {
		for (size_t i = 0; i < activeGUI->widgets.size(); i++) {
			TGUIWidget* widget = activeGUI->widgets[i];
			widget->setAllClear(true);
		}
	}

	if (tguiActiveWidget) {
		ALLEGRO_EVENT event;
		while (!al_event_queue_is_empty(key_events)) {
			al_get_next_event(key_events, &event);
			if (event.any.timestamp < drop_input_events_older_than) {
				continue;
			}
			if (!(ignore & TGUI_KEYBOARD) && !tguiIsDisabled(tguiActiveWidget)) {
				if (event.type == ALLEGRO_EVENT_KEY_DOWN || event.type == USER_KEY_DOWN) {
					ALLEGRO_KEYBOARD_EVENT *key = &event.keyboard;
					keycodeBuffer.push_back(key->keycode);
					unicharBuffer.push_back(key->unichar);
				}
			}
			if (event.type == USER_KEY_DOWN || event.type == USER_KEY_UP || event.type == USER_KEY_CHAR) {
				al_unref_user_event((ALLEGRO_USER_EVENT *)&event);
			}
		}
		if (tguiActiveWidget->getAllClear()) {
			for (unsigned int i = 0; i < keycodeBuffer.size(); i++) {
				int keycode = keycodeBuffer[i];
				int unichar = unicharBuffer[i];
				if (!tguiActiveWidget->handleKey(keycode, unichar)) {
				}
			}
		}
		keycodeBuffer.clear();
                unicharBuffer.clear();
	}
	else {
		ALLEGRO_EVENT event;
		while (!al_event_queue_is_empty(key_events)) {
			al_get_next_event(key_events, &event);
			if (event.any.timestamp < drop_input_events_older_than) {
				continue;
			}
			if ((event.type == ALLEGRO_EVENT_KEY_DOWN || event.type == USER_KEY_DOWN)  && event.keyboard.keycode == ALLEGRO_KEY_TAB) {
				int focus = 0;
				while (focus < (int)activeGUI->widgets.size()) {
					if (activeGUI->widgets[focus]->acceptsFocus()) {
						activeGUI->widgets[focus]->setFocus(true);
						activeGUI->focus = focus;
						tguiActiveWidget = activeGUI->widgets[focus];
						break;
					}
					focus++;
				}
			}
			if (event.type == USER_KEY_DOWN || event.type == USER_KEY_UP || event.type == USER_KEY_CHAR) {
				al_unref_user_event((ALLEGRO_USER_EVENT *)&event);
			}
		}
	}

	for (size_t i = 0; i < activeGUI->widgets.size(); i++) {
		TGUIWidget* widget = activeGUI->widgets[i];
		if (!widget->getAllClear()) {
			continue;
		}
		if (tguiIsDisabled(widget))
			continue;
		unsigned int retVal = widget->update((int)elapsed);
		switch (retVal) {
			case TGUI_CONTINUE:
				break;
			case TGUI_RETURN: {
				widget->setAllClear(false); // could help
				return widget;
			}
		}
		std::vector<int>* hotkeys = widget->getHotkeys();
		if (hotkeys) {
			for (unsigned int j = 0; j < hotkeys->size(); j++) {
				int hotkey = (*hotkeys)[j];
				bool found = false;
				for (unsigned int i = 0;
						i < pressedHotkeys.size();
						i++) {
					if (hotkey == pressedHotkeys[i]) {
						found = true;
						break;
					}
				}
				if (!found) {
					if (tguiHotkeyPressed(hotkey)) {
						if (widget->handleHotkey(hotkey))
							pressedHotkeys.push_back(hotkey);
					}
				}
			}
		}
	}

	std::vector<int>::iterator it;

	for (it = pressedHotkeys.begin(); it != pressedHotkeys.end();) {
		if (!tguiHotkeyPressed(*it)) {
			it = pressedHotkeys.erase(it);
			continue;
		}
		it++;
	}

	for (int i = 0; i < (int)mouseMoveEvents.size(); i++) {
		int saved_mouse_x = mouseMoveEvents[0]->x;
		int saved_mouse_y = mouseMoveEvents[0]->y;
		int saved_mouse_z = mouseMoveEvents[0]->z;
		delete mouseMoveEvents[0];
		mouseMoveEvents.erase(mouseMoveEvents.begin());
		if (!(ignore & TGUI_MOUSE)) {
			for (int i = 0; i < (int)activeGUI->widgets.size(); i++) {
				if (activeGUI->widgets[i]->getAllClear() && !tguiIsDisabled(activeGUI->widgets[i])) {
					activeGUI->widgets[i]->mouseMove(saved_mouse_x,
						saved_mouse_y, saved_mouse_z);
				}
			}
		}
	}

	for (int i = 0; i < (int)mouseDownEvents.size(); i++) {
		int saved_mouse_b = mouseDownEvents[0]->b;
		int saved_mouse_x = mouseDownEvents[0]->x;
		int saved_mouse_y = mouseDownEvents[0]->y;
		delete mouseDownEvents[0];
		mouseDownEvents.erase(mouseDownEvents.begin());
		if (!(ignore & TGUI_MOUSE)) {
			std::vector<TGUIWidget *> tmpVec = activeGUI->widgets;
			for (int i = 0; i < (int)tmpVec.size(); i++) {
				TGUIWidget *widget = tmpVec[i];
				if (!widget->getAllClear()) {
					continue;
				}
				if (!tguiIsDisabled(activeGUI->widgets[i])) {
					widget->mouseDownAbs(
						saved_mouse_x, 
						saved_mouse_y,
						saved_mouse_b
					);
				}
			}
			TGUIWidget* widget =
				tguiFindPixelOwner(saved_mouse_x, saved_mouse_y);
			if (widget && widget->getAllClear()) {
				if (!tguiIsDisabled(widget)) {
					if (widget && widget != tguiClickedWidget) {
						tguiSetFocus(widget);
						tguiActiveWidgetClickedPoint.x =
							saved_mouse_x - widget->getX();
						tguiActiveWidgetClickedPoint.y =
							saved_mouse_y - widget->getY();
						tguiMouseButton = saved_mouse_b;
						widget->mouseDown(saved_mouse_x - widget->getX(),
							saved_mouse_y - widget->getY(),
							saved_mouse_b);
						tguiClickedWidget = widget;
					}
				}
			}
			tguiMouseReleased = false;
		}
	}

	for (int i = 0; i < (int)mouseUpEvents.size(); i++) {
		int saved_mouse_b = mouseUpEvents[0]->b;
		int saved_mouse_x = mouseUpEvents[0]->x;
		int saved_mouse_y = mouseUpEvents[0]->y;
		delete mouseUpEvents[0];
		mouseUpEvents.erase(mouseUpEvents.begin());
		if (!(ignore & TGUI_MOUSE)) {
			std::vector<TGUIWidget *> tmpVec = activeGUI->widgets;
			for (int i = 0; i < (int)tmpVec.size(); i++) {
				TGUIWidget *widget = tmpVec[i];
				if (activeGUI->widgets[i]->getAllClear() && !tguiIsDisabled(activeGUI->widgets[i])) {
					widget->mouseUpAbs(
						saved_mouse_x, 
						saved_mouse_y,
						saved_mouse_b
					);
				}
			}
			if (tguiClickedWidget) {
				std::vector<TGUIWidget *>::iterator it =
					std::find(
								 activeGUI->widgets.begin(),
								 activeGUI->widgets.end(),
								 tguiClickedWidget);
				if (it == activeGUI->widgets.end())
					tguiClickedWidget = NULL;
			}
			if (tguiClickedWidget && !tguiIsDisabled(tguiClickedWidget) && tguiClickedWidget->getAllClear()) {
				tguiMouseReleasePoint.x = saved_mouse_x;
				tguiMouseReleasePoint.y = saved_mouse_y;
				int relativeX;
				int relativeY;
				if (tguiPointOnWidget(tguiClickedWidget, &saved_mouse_x,
							&saved_mouse_y)) {
					relativeX = saved_mouse_x - tguiClickedWidget->getX();
					relativeY = saved_mouse_y - tguiClickedWidget->getY();
				}
				else {
					/* If mouse is released when it is no longer
					 * on the widget, pass mouseUp the negated
					 * absolute coordinates of the release
					 */
					relativeX = -saved_mouse_x;
					relativeY = -saved_mouse_y;
				}
				tguiClickedWidget->mouseUp(relativeX, relativeY, saved_mouse_b);
				tguiClickedWidget = 0;
			}
			tguiMouseReleased = true;
		}
	}

	return 0;
}

/*
 * Must be called if you stop making calls to tguiUpdate
 * for some time.
 */
void tguiWakeUp()
{
	tguiLastUpdate = tguiCurrentTimeMillis();
}

/*
 * Draw all widgets.
 */
void tguiDraw()
{
	tguiDrawRect(0, 0, tguiScreenWidth, tguiScreenHeight);
}

/*
 * Draw every widget that falls into a rectangle.
 */
void tguiDrawRect(int x1, int y1, int x2, int y2)
{
	for (size_t i = 0; i < activeGUI->widgets.size(); i++) {
		TGUIWidget* widget = activeGUI->widgets[i];
		/*
		int wx1 = widget->getX();
		int wy1 = widget->getY();
		int wx2 = widget->getX() + widget->getWidth() - 1;
		int wy2 = widget->getY() + widget->getHeight() - 1;
		if (checkBoxCollision(wx1, wy1, wx2, wy2, x1, y1, x2, y2)) {
		*/
			widget->pre_draw();
		//}
	}
	for (size_t i = 0; i < activeGUI->widgets.size(); i++) {
		TGUIWidget* widget = activeGUI->widgets[i];
		/*
		int wx1 = widget->getX();
		int wy1 = widget->getY();
		int wx2 = widget->getX() + widget->getWidth() - 1;
		int wy2 = widget->getY() + widget->getHeight() - 1;
		if (checkBoxCollision(wx1, wy1, wx2, wy2, x1, y1, x2, y2)) {
		*/
			widget->draw();
		//}
	}
	for (size_t i = 0; i < activeGUI->widgets.size(); i++) {
		TGUIWidget* widget = activeGUI->widgets[i];
		/*
		int wx1 = widget->getX();
		int wy1 = widget->getY();
		int wx2 = widget->getX() + widget->getWidth() - 1;
		int wy2 = widget->getY() + widget->getHeight() - 1;
		if (checkBoxCollision(wx1, wy1, wx2, wy2, x1, y1, x2, y2)) {
		*/
			widget->post_draw();
		//}
	}
}

/*
 * Push the current GUI on a stack.
 */
void tguiPush()
{
	if (activeGUI && activeGUI->focus >= 0 &&
	activeGUI->focus < (int)activeGUI->widgets.size() &&
	activeGUI->widgets[activeGUI->focus]) {
		activeGUI->widgets[activeGUI->focus]->setFocus(false);
	}
	tguiStack.push_back(activeGUI);
	activeGUI = new TGUI;
	activeGUI->focus = 0;
	tguiActiveWidget = 0;
}

/*
 * Make the last GUI pushed onto the stack with tguiPush
 * active and remove it from the stack. You should have
 * deleted the active GUI with tguiDeleteActive before
 * calling this if you created a new GUI after calling
 * tguiPush.
 */
bool tguiPop()
{
	if (tguiStack.size() <= 0)
		return false;
	delete activeGUI;
	activeGUI = tguiStack[tguiStack.size()-1];
	std::vector<TGUI*>::iterator it = tguiStack.begin() + tguiStack.size() - 1;
	tguiStack.erase(it);
	tguiSetFocus(activeGUI->focus);
	return true;
}

/*
 * Create a hotkey that can contain keyshift flags as well
 * as regular keys, that can be used by the GUI widgets.
 */
int tguiCreateHotkey(int flags, int key)
{
	return ((flags & 0xffff) << 16) | (key & 0xffff);
}

int tguiGetHotkeyFlags(int hotkey)
{
	return (hotkey >> 16) & 0xffff;
}

int tguiGetHotkeyKey(int hotkey)
{
	return (hotkey & 0xffff);
}

/*
 * Move all the widgets in a group forward to the front
 * of the display (end of vector).
 */
void tguiRaiseChildren(TGUIWidget* parent)
{
	std::vector<TGUIWidget*> targetWidgets;

	for (size_t i = 0; i < activeGUI->widgets.size(); i++) {
		TGUIWidget* widget = activeGUI->widgets[i];
		if (widget->getParent() == parent) {
			targetWidgets.push_back(widget);
		}
	}

	int currIndex = 0;
	size_t i;

	for (i = 0; i < activeGUI->widgets.size(); i++) {
		TGUIWidget* widget = activeGUI->widgets[i];
		if (widget->getParent() != parent)
			activeGUI->widgets[currIndex++] = widget;
	}

	for (i = 0; i < targetWidgets.size(); i++) {
		activeGUI->widgets[currIndex++] = targetWidgets[i];
	}
	
	targetWidgets.clear();

	tguiFindFocus();
}

/*
 * Move all the widgets in a group forward to the front
 * of the display (end of vector).
 */
void tguiLowerChildren(TGUIWidget* parent)
{
	std::vector<TGUIWidget*> lowerWidgets;
	std::vector<TGUIWidget*> upperWidgets;

	for (size_t i = 0; i < activeGUI->widgets.size(); i++) {
		TGUIWidget* widget = activeGUI->widgets[i];
		if (widget->getParent() == parent) {
			lowerWidgets.push_back(widget);
		}
		else {
			upperWidgets.push_back(widget);
		}
	}

	int currIndex = 0;
	unsigned int i;

	for (i = 0; i < lowerWidgets.size(); i++) {
		activeGUI->widgets[currIndex++] = lowerWidgets[i];
	}
	
	for (i = 0; i < upperWidgets.size(); i++) {
		activeGUI->widgets[currIndex++] = upperWidgets[i];
	}
	
	lowerWidgets.clear();
	upperWidgets.clear();

	tguiFindFocus();
}

/*
 * Move all the widgets in a group forward to the front
 * of the display (end of vector).
 */
void tguiRaiseSingleWidget(TGUIWidget* widgetToRaise)
{
	std::vector<TGUIWidget*> targetWidgets;

	for (size_t i = 0; i < activeGUI->widgets.size(); i++) {
		TGUIWidget* widget = activeGUI->widgets[i];
		if (widget == widgetToRaise) {
			targetWidgets.push_back(widget);
		}
	}

	int currIndex = 0;
	size_t i;

	for (i = 0; i < activeGUI->widgets.size(); i++) {
		TGUIWidget* widget = activeGUI->widgets[i];
		if (widget != widgetToRaise) {
			activeGUI->widgets[currIndex++] = widget;
		}
	}

	for (i = 0; i < targetWidgets.size(); i++) {
		activeGUI->widgets[currIndex++] = targetWidgets[i];
	}
	
	targetWidgets.clear();

	tguiFindFocus();
}

/*
 * Move all the widgets in a group forward to the front
 * of the display (end of vector).
 */
void tguiLowerSingleWidget(TGUIWidget* widgetToLower)
{
	std::vector<TGUIWidget*> lowerWidgets;
	std::vector<TGUIWidget*> upperWidgets;

	for (size_t i = 0; i < activeGUI->widgets.size(); i++) {
		TGUIWidget* widget = activeGUI->widgets[i];
		if (widget == widgetToLower) {
			lowerWidgets.push_back(widget);
		}
		else {
			upperWidgets.push_back(widget);
		}
	}

	int currIndex = 0;
	unsigned int i;

	for (i = 0; i < lowerWidgets.size(); i++) {
		activeGUI->widgets[currIndex++] = lowerWidgets[i];
	}
	
	for (i = 0; i < upperWidgets.size(); i++) {
		activeGUI->widgets[currIndex++] = upperWidgets[i];
	}
	
	lowerWidgets.clear();
	upperWidgets.clear();

	tguiFindFocus();
}

/*
 * Move all the widgets in a group forward to the front
 * of the display (end of vector).
 */
static void tguiLowerSingleWidgetBelow(TGUIWidget* widgetToLower,
		TGUIWidget* widgetAbove)
{
	std::vector<TGUIWidget*> widgets;

	for (size_t i = 0; i < activeGUI->widgets.size(); i++) {
		TGUIWidget* widget = activeGUI->widgets[i];
		if (widget == widgetAbove) {
			widgets.push_back(widgetToLower);
			widgets.push_back(widgetAbove);
		}
		else if (widget != widgetToLower) {
			widgets.push_back(widget);
		}
	}

	activeGUI->widgets.clear();

	for (int i = 0; i < (int)widgets.size(); i++) {
		activeGUI->widgets.push_back(widgets[i]);
	}

	widgets.clear();

	tguiFindFocus();
}

/*
 * Move all the widgets in a group forward to the front
 * of the display (end of vector).
 */
void tguiRaiseWidget(TGUIWidget* widgetToRaise)
{
	std::vector<TGUIWidget*> targetWidgets;

	for (size_t i = 0; i < activeGUI->widgets.size(); i++) {
		TGUIWidget* widget = activeGUI->widgets[i];
		if ((widget == widgetToRaise) ||
				(widget->getParent() == widgetToRaise)) {
			targetWidgets.push_back(widget);
		}
	}

	int currIndex = 0;
	size_t i;

	for (i = 0; i < activeGUI->widgets.size(); i++) {
		TGUIWidget* widget = activeGUI->widgets[i];
		if ((widget != widgetToRaise) &&
				(widget->getParent() != widgetToRaise))
			activeGUI->widgets[currIndex++] = widget;
	}

	for (i = 0; i < targetWidgets.size(); i++) {
		activeGUI->widgets[currIndex++] = targetWidgets[i];
	}
	
	targetWidgets.clear();

	tguiFindFocus();
}

/*
 * Move all the widgets in a group forward to the front
 * of the display (end of vector).
 */
void tguiLowerWidget(TGUIWidget* widgetToLower)
{
	std::vector<TGUIWidget*> lowerWidgets;
	std::vector<TGUIWidget*> upperWidgets;

	for (size_t i = 0; i < activeGUI->widgets.size(); i++) {
		TGUIWidget* widget = activeGUI->widgets[i];
		if ((widget == widgetToLower) ||
				(widget->getParent() == widgetToLower)) {
			lowerWidgets.push_back(widget);
		}
		else {
			upperWidgets.push_back(widget);
		}
	}

	int currIndex = 0;
	unsigned int i;

	for (i = 0; i < lowerWidgets.size(); i++) {
		activeGUI->widgets[currIndex++] = lowerWidgets[i];
	}
	
	for (i = 0; i < upperWidgets.size(); i++) {
		activeGUI->widgets[currIndex++] = upperWidgets[i];
	}
	
	lowerWidgets.clear();
	upperWidgets.clear();

	tguiFindFocus();
}

/*
 * Remove a widget and all of it's children.
 * You should call tguiSetFocus after this.
 */
void tguiDeleteWidget(TGUIWidget* widgetToDelete)
{
	std::vector<TGUIWidget*> widgetsToKeep;
	size_t i;

	for (i = 0; i < activeGUI->widgets.size(); i++) {
		TGUIWidget* widget = activeGUI->widgets[i];
		if ((widget != widgetToDelete) &&
				!tguiWidgetIsChildOf(widget, widgetToDelete)) {
			widgetsToKeep.push_back(widget);
		}
		else {
			if (widget == tguiActiveWidget)
				tguiActiveWidget = NULL;
		}
	}

	activeGUI->widgets.clear();

	for (int i = 0; i < (int)widgetsToKeep.size(); i++) {
		activeGUI->widgets.push_back(widgetsToKeep[i]);
	}

	widgetsToKeep.clear();

	//tguiMakeFresh();
	tguiSetFocus(tguiActiveWidget);
}

/*
 * Sets the parent widget. Every call to tguiAddWidget after
 * this call will add a widget with this parent.
 */
void tguiSetParent(TGUIWidget* parent)
{
	tguiCurrentParent = parent;
}

/*
 * Returns true if a widget is in the active GUI, otherwise false
 */
bool tguiWidgetIsActive(TGUIWidget* widget)
{
	for (size_t i = 0; i < activeGUI->widgets.size(); i++) {
		if (activeGUI->widgets[i] == widget)
			return true;
	}
	return false;
}

/*
 * Center a widget on x,y
 */
void tguiCenterWidget(TGUIWidget* widget, int x, int y)
{
	widget->setX(x - (widget->getWidth() / 2));
	widget->setY(y - (widget->getHeight() / 2));
}

TGUIWidget* tguiGetActiveWidget()
{
	return tguiActiveWidget;
}

void tguiMakeFresh()
{
	tguiActiveWidget = 0;
	activeGUI->focus = 0;
}

bool tguiWidgetIsChildOf(TGUIWidget* widget, TGUIWidget* parent)
{
	if (parent == 0) {
		return widget->getParent() == 0;
	}
	else {
		while (widget != 0) {
			if (widget->getParent() == parent) {
				return true;
			}
			widget = widget->getParent();
		}
		return false;
	}
}

int tguiGetMouseX()
{
	return tguiMouseState.x;
}

int tguiGetMouseY()
{
	return tguiMouseState.y;
}

int tguiGetMouseButtons()
{
	return tguiMouseState.buttons;
}

void tguiClearKeybuffer()
{
/*
	keycodeBuffer.clear();
	unicharBuffer.clear();
*/
}

void tguiSetScale(float xscale, float yscale)
{
	x_scale = xscale;
	y_scale = yscale;
}

void tguiSetTolerance(int pixels)
{
	tolerance = pixels;
}

void tguiClearMouseEvents()
{
/*
	for (size_t i = 0; i < mouseMoveEvents.size(); i++) {
		delete mouseMoveEvents[i];
	}
	mouseMoveEvents.clear();
	for (size_t i = 0; i < mouseUpEvents.size(); i++) {
		delete mouseUpEvents[i];
	}
	mouseUpEvents.clear();
	for (size_t i = 0; i < mouseDownEvents.size(); i++) {
		delete mouseDownEvents[i];
	}
	mouseDownEvents.clear();
	al_flush_event_queue(mouse_events);
*/
}

void tguiIgnore(int type)
{
	ignore = type;
	//tguiClearMouseEvents();
}

void tguiSetRotation(int angle_in_degrees)
{
	rotation = angle_in_degrees;
}

void tguiConvertMousePosition(int *x, int *y, int ox, int oy, float rx, float ry)
{
	int in_x = *x;
	int in_y = *y;

	if (ox != 0 || oy != 0) {
		in_x -= tgui_screen_offset_x;
		in_y -= tgui_screen_offset_y;
	}
	else {
		in_x /= rx;
		in_y /= ry;
	}

	switch (rotation) {
		case 0:
			*x = in_x;
			*y = in_y;
			break;
		case 90:
			*x = (tguiScreenHeight-1) - in_y;
			*y = in_x;
			break;
		case 180:
			*x = (tguiScreenWidth-1) - in_x;
			*y = (tguiScreenHeight-1) - in_y;
			break;
		case 270:
			*x = in_y;
			*y = (tguiScreenWidth-1) - in_x;
			break;
	}

	*x = *x / x_scale;
	*y = *y / y_scale;
}

void tguiDisableChildren(TGUIWidget *parent)
{
	for (int i = 0; i < (int)activeGUI->widgets.size(); i++) {
		if (activeGUI->widgets[i]->getParent() == parent) {
			disabledWidgets.push_back(activeGUI->widgets[i]);
		}
	}
}
void tguiDisableAllWidgets()
{
	for (int i = 0; i < (int)activeGUI->widgets.size(); i++) {
		if (!tguiIsDisabled(activeGUI->widgets[i])) {
			disabledWidgets.push_back(activeGUI->widgets[i]);
		}
	}
}

void tguiEnableChildren(TGUIWidget *parent)
{
	std::vector<TGUIWidget *>::iterator it;
	for (it = disabledWidgets.begin(); it != disabledWidgets.end();) {
		TGUIWidget *this_widget = *it;
		if (this_widget->getParent() == parent) {
			it = disabledWidgets.erase(it);
		}
		else {
			it++;
		}
	}
}

void tguiEnableAllWidgets()
{
	disabledWidgets.clear();
}

bool tguiIsDisabled(TGUIWidget *widget)
{
	for (int i = 0; i < (int)disabledWidgets.size(); i++) {
		if (widget == disabledWidgets[i]) {
			return true;
		}
	}
	return false;
}

void tguiSetScreenSize(int width, int height)
{
	tguiScreenWidth = width;
	tguiScreenHeight = height;
}

void tguiSetScreenParameters(int offset_x, int offset_y,
	float ratio_x, float ratio_y)
{
	tgui_screen_offset_x = offset_x;
	tgui_screen_offset_y = offset_y;
	tgui_screen_ratio_x = ratio_x;
	tgui_screen_ratio_y = ratio_y;
}

void tguiEnableHotZone(bool enable)
{
	ignore_hot_zone = (enable == false);
}
