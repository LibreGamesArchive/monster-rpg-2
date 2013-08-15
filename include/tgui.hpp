#ifndef TGUI_H
#define TGUI_H

#include <vector>

#include <allegro5/allegro5.h>

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

const int TGUI_KEYFLAG_LSHIFT = 1;
const int TGUI_KEYFLAG_RSHIFT = 2;
const int TGUI_KEYFLAG_SHIFT = 4; // left or right
const int TGUI_KEYFLAG_LCONTROL = 8;
const int TGUI_KEYFLAG_RCONTROL = 16;
const int TGUI_KEYFLAG_CONTROL = 32; // left or right
const int TGUI_KEYFLAG_LALT = 64;
const int TGUI_KEYFLAG_RALT = 128;
const int TGUI_KEYFLAG_ALT = 256; // left or right

const int TGUI_MOUSE_LEFT = 1;
const int TGUI_MOUSE_RIGHT = 2;
const int TGUI_MOUSE_MIDDLE = 4;

const int TGUI_CONTINUE = 0;
const int TGUI_RETURN = 1;

const int TGUI_BORDER = 25; // parent widgets must be at least this much on screen

// for SetLayout (FIXME: also add inverse)
const int TGUI_X = 1;
const int TGUI_Y = 2;

const int TGUI_MOUSE = 1;
const int TGUI_KEYBOARD = 2;

struct TGUIPoint {
	int x;
	int y;
};

struct TGUIRect {
	int x1;
	int y1;
	int x2;
	int y2;
};

class TGUIWidget {
public:
	int getX() { return x; }
	int getY() { return y; }
	int getRelativeX() { return relativeX; }
	int getRelativeY() { return relativeY; }
	int getWidth() { return width; }
	int getHeight() { return height; }
	TGUIWidget* getParent() { return parent; }
	std::vector<int>* getHotkeys() { return hotkeys; }
	bool getFocus() { return focus; }
	void setX(int newX) { x = newX; }
	void setY(int newY) { y = newY; }
	void setRelativeX(int rx) { relativeX = rx; }
	void setRelativeY(int ry) { relativeY = ry; }
	void setWidth(int w) { width = w; }
	void setHeight(int h) { height = h; }
	void setParent(TGUIWidget* p) { parent = p; }
	bool getAllClear() { return allClear; }
	void setAllClear(bool allClear) { this->allClear = allClear; }
	virtual void setFocus(bool fcs) { focus = fcs; }
	// pre_draw is called on widgets in the same order as draw, only before any draw call
	virtual void pre_draw() {}
	virtual void draw() {}
	virtual void post_draw() {}
	/*
	 * Return false to close the current GUI.
	 */
	virtual int update(int millis) { return TGUI_CONTINUE; }
	/*
	 * Mouse move event gives absolute mouse coordinates
	 */
	virtual void mouseMove(int mx, int my, int mz) {}
	/*
	 * The mouse was clicked on the widget.
	 * tguiActiveWidgetClickedPoint holds the position of the
	 * click relative to the widget. tguiMouseButton is the
	 * mouse button that was pressed.
	 */
	virtual void mouseDown(int mx, int my, int mb) {}
	virtual void mouseDownAbs(int mx, int my, int mb) {}
	/*
	 * Called when the mouse is released after being pressed
	 * on the widget. x and y are relative to the widget or
	 * < 0 if the mouse was released outside the widget.
	 * In any case, the position of the mouse release in
	 * screen coordinates is stored in tguiMouseReleasePoint.
	 */
	virtual void mouseUp(int x, int y, int b) {}
	/* Called for every widget on every mouse up event
	 * with absolute mouse coordinates
	 */
	virtual void mouseUpAbs(int x, int y, int b) {}
	/*
	 * Gets passed the result of readkey(), return false
	 * to push the key back into the key buffer
	 */
	virtual bool handleKey(int keycode, int unichar) { return false; }
	virtual bool handleHotkey(int hotkey) { return false; }
	virtual bool acceptsFocus() { return false; }
	virtual bool acceptsHover() { return false; }
	TGUIWidget();
	virtual ~TGUIWidget() {}

protected:
	int x;
	int y;
	int relativeX; // relative to parent (set automatically)
	int relativeY;
	int width;
	int height;
	TGUIWidget* parent;
	std::vector<int>* hotkeys;
	bool focus;
	bool allClear;
};

extern TGUIWidget* tguiActiveWidget;
extern TGUIPoint tguiActiveWidgetClickedPoint; // relative to widget

extern bool tguiPointOnWidget(TGUIWidget* widget, int *x, int *y);
extern unsigned long tguiCurrentTimeMillis();
extern void tguiInit();
extern bool tguiIsInitialized();
extern void tguiShutdown();
extern void tguiSetFocus(int widget);
extern void tguiSetFocus(TGUIWidget* widget);
extern TGUIWidget* tguiGetFocus();
extern void tguiFocusPrevious();
extern void tguiFocusNext();
extern void tguiDeleteActive();
extern void tguiTranslateAll(int x, int y);
extern void tguiTranslateWidget(TGUIWidget* parent, int x, int y);
extern void tguiAddWidget(TGUIWidget* widget);
extern TGUIWidget* tguiUpdate();
extern void tguiHandleKeys();
extern void tguiWakeUp();
extern void tguiDraw();
extern void tguiDrawRect(int x1, int y1, int x2, int y2);
extern void tguiPush();
extern bool tguiPop();
extern int tguiCreateHotkey(int flags, int key);
extern int tguiGetHotkeyFlags(int hotkey);
extern int tguiGetHotkeyKey(int hotkey);
extern void tguiLowerChildren(TGUIWidget* parent);
extern void tguiRaiseChildren(TGUIWidget* parent);
extern void tguiRaiseSingleWidget(TGUIWidget* widget);
extern void tguiLowerSingleWidget(TGUIWidget* widget);
extern void tguiRaiseWidget(TGUIWidget* widget);
extern void tguiLowerWidget(TGUIWidget* widget);
extern void tguiDeleteWidget(TGUIWidget* widget);
extern void tguiSetParent(TGUIWidget* parent);
extern bool tguiWidgetIsActive(TGUIWidget* widget);
extern void tguiCenterWidget(TGUIWidget* widget, int x, int y);
extern TGUIWidget* tguiGetActiveWidget();
extern void tguiMakeFresh();
extern bool tguiWidgetIsChildOf(TGUIWidget* widget, TGUIWidget* parent);
extern int tguiGetMouseX();
extern int tguiGetMouseY();
extern int tguiGetMouseButtons();
extern void tguiClearKeybuffer();
extern void tguiClearMouseEvents();
extern void tguiSetScale(float x_scale, float y_scale);
extern void tguiSetTolerance(int pixels);
void tguiSetLayout(int x, int y);
void tguiIgnore(int type);
void tguiSetRotation(int angle_in_degrees);
void tguiConvertMousePosition(int *x, int *y, int ox, int oy, float rx, float ry);
void tguiDisableChildren(TGUIWidget *parent);
void tguiEnableChildren(TGUIWidget *parent);
void tguiDisableAllWidgets();
void tguiEnableAllWidgets();
bool tguiIsDisabled(TGUIWidget *widget);
void tguiSetScreenSize(int width, int height);
void tguiSetScreenParameters(int offset_x, int offset_y,
	float ratio_x, float ratio_y);
void tguiSetScreenSize(int width, int height);
void tguiEnableHotZone(bool enable); // monster 2 stuff

inline bool checkBoxCollision(int x1, int y1, int x2, int y2, int x3, int y3,
	int x4, int y4)
{
	if ((y2 < y3) || (y1 > y4) || (x2 < x3) || (x1 > x4))
		return false;
	return true;
}


#endif
