#ifndef WIDGETS_HPP
#define WIDGETS_HPP


/* These names have become confused. NORMAL is no shadow. DROP_SHADOW is +1+1. BORDER is 8 directions around. SQUARE_BORDER is a square drop shadow, 3 positions bottom right */
const int WGT_TEXT_NORMAL = 0;
const int WGT_TEXT_DROP_SHADOW = 1;
const int WGT_TEXT_BORDER = 2;
const int WGT_TEXT_SQUARE_BORDER = 3;

const int CONTEXT_MENU_W = 200;
const int CONTEXT_MENU_H = 172;

extern std::vector<MBITMAP *> icons;
class MManSelector;
extern MManSelector *manChooser;

extern MFONT *game_font;
extern MFONT *game_font_second_display;

void mDrawFrame(int x, int y, int w, int h, bool shadow = false);
void mTextout(MFONT *font, const char *text, int x, int y,
		MCOLOR text_color, MCOLOR shadow_color,
		int shadowType, bool center);
void mTextout_simple(const char *text, int x, int y, MCOLOR color);
void showPlayerInfo(int who);
void showPlayerInfo_ptr(Player *p);

struct MultiPoint {
	float x, y;
	bool west;
	void *data;
};

class MouseMonitor {
public:
	struct Touch {
		int x, y, id;
	};

	std::vector <Touch> touches;

private:

	ALLEGRO_EVENT_QUEUE *queue;

	int find_touch(int id)
	{
		for (size_t i = 0; i < touches.size(); i++) {
			if (touches[i].id == id) {
				return (int)i;
			}
		}
	
		return -1;
	}

public:
	MouseMonitor(void) {
		queue = al_create_event_queue();
		al_register_event_source(queue, al_get_touch_input_event_source());
	}
	
	~MouseMonitor(void) {
		al_destroy_event_queue(queue);
	}
	
	void update(void)
	{
		while (!al_event_queue_is_empty(queue)) {
			ALLEGRO_EVENT event;
			al_get_next_event(queue, &event);

			int x = event.touch.x;
			int y = event.touch.y;

			if (config.getMaintainAspectRatio() == ASPECT_FILL_SCREEN)
				tguiConvertMousePosition(&x, &y, 0, 0, screen_ratio_x, screen_ratio_y);
			else
				tguiConvertMousePosition(&x, &y, screen_offset_x, screen_offset_y, 1, 1);

			if (event.type == ALLEGRO_EVENT_TOUCH_BEGIN) {
				Touch t;
				t.x = x;
				t.y = y;
				t.id = event.touch.id;
				touches.push_back(t);
			}
			else if (event.type == ALLEGRO_EVENT_TOUCH_END) {
				int idx = find_touch(event.touch.id);
				if (idx >= 0) {
					touches.erase(touches.begin()+idx);
				}
			}
			else if (event.type == ALLEGRO_EVENT_TOUCH_MOVE) {
				int idx = find_touch(event.touch.id);
				touches[idx].x = x;
				touches[idx].y = y;
			}
		}
	}
};

class TemporaryTextWidget : public TGUIWidget {
public:
	bool isDisplayed(void)
	{
		return displaying;
	}

	void draw(void)
	{
		if (!displaying) return;
		mTextout(game_font, _t(text), x, (int)(y + yoffs),
			color, black,
			WGT_TEXT_SQUARE_BORDER, true);
	}

	void stop(void) 
	{
		displaying = false;
		yoffs = 0;
   		x = y = 0;
	}

	void start(std::string text, int cx, int cy, MCOLOR color)
	{
		x = cx;
		y = cy;
		displaying = true;
		yoffs = 0;
		strcpy(this->text, text.c_str());
		this->color = color;
	}

	bool acceptsFocus() { return false; }
	
	int update(int step)
	{
		if (!displaying)
			return TGUI_CONTINUE;
		yoffs -= 0.01f * step;
		if (yoffs < -10) {
			stop();
		}
		return TGUI_CONTINUE;
	}
	
	TemporaryTextWidget(void) {
		this->hotkeys = 0;
		this->x = 0;
		this->y = 0;
		this->width = 10;
		this->height = 10;
		stop();
	}
private:
	bool displaying;
	char text[100];
	MCOLOR color;
	float yoffs;
};


class MShadow : public TGUIWidget {
public:
	void draw(void);
	bool acceptsFocus() { return false; }
	
	int update(int step)
	{
		return TGUI_CONTINUE;
	}
	
	MShadow(void) {
		x = (BW*2)/3;
		y = BH-50;
		width = SHADOW_CORNER_SIZE;
		height = 50;
		hotkeys = NULL;

		/*
		for (int i = 0; i < SHADOW_CORNER_SIZE; i++) {
			float a = (float)((1.0f-(float)sin(((float)i/(SHADOW_CORNER_SIZE-1))*(M_PI/2))) * 255);
			a /= 1.6f;
			verts[i*2].x = (BW*2)/3+i + 0.5;
			verts[i*2].y = BH-50 + 0.5;
			verts[i*2].z = 0;
			verts[i*2].color = m_map_rgba(0, 0, 0, a);
			verts[i*2+1].x = (BW*2)/3+i + 0.5;
			verts[i*2+1].y = BH + 0.5;
			verts[i*2+1].z = 0;
			verts[i*2+1].color = m_map_rgba(0, 0, 0, a);
		}
		*/

		verts[0].x = BW*2/3;
		verts[0].y = BH;
		verts[0].z = 0;
		verts[0].color = m_map_rgba(0, 0, 0, 255);
		verts[1].x = BW*2/3;
		verts[1].y = BH-50;
		verts[1].z = 0;
		verts[1].color = m_map_rgba(0, 0, 0, 255);
		verts[2].x = BW*2/3 + SHADOW_CORNER_SIZE;
		verts[2].y = BH;
		verts[2].z = 0;
		verts[2].color = m_map_rgba(0, 0, 0, 0);
		verts[3].x = BW*2/3 + SHADOW_CORNER_SIZE;
		verts[3].y = BH-50;
		verts[3].z = 0;
		verts[3].color = m_map_rgba(0, 0, 0, 0);
	}

private:
	ALLEGRO_VERTEX verts[4];
};


class FakeWidget : public TGUIWidget {
public:
	void draw(void) {
		if (use_dpad && this == tguiActiveWidget && draw_outline) {
			int n = (unsigned)tguiCurrentTimeMillis() % 1000;
			float p;
			if (n < 500) p = n / 500.0f;
			else p = (500-(n-500)) / 500.0f;
			int a = p*255;
			m_draw_rectangle(x+0.5f, y+0.5f, x+width+0.5f, y+height+0.5f,
				m_map_rgba(0xff*a/255, 0xd8*a/255, 0, a), 0);
		}
	}

	void mouseDown(int x, int y, int b)
	{
		holdStart = tguiCurrentTimeMillis();
	}

	void mouseUp(int x, int y, int b)
	{
		if (x >= 0 && y >= 0) {
			clicked = true;
		}
		holdStart = 0;
	}

	unsigned long getHoldStart(void)
	{
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
		return holdStart;
#else
		if (b3_pressed) {
			b3_pressed = false;
			return tguiCurrentTimeMillis() - 1000UL;
		}
		else
			return 0;
#endif
	}

	void reset(void) 
	{
		mouseUp(-1, -1, 0);
	}

	bool acceptsFocus() { return accFocus; }
	
	int update(int step)
	{
		if (was_down && use_dpad && getInput()->getDescriptor().button1)
			return TGUI_CONTINUE;
		else
			was_down = false;

		if (tguiActiveWidget == this && use_dpad) {
			INPUT_EVENT ie = get_next_input_event();
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
			InputDescriptor id = getInput()->getDescriptor();
#endif

#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_ANDROID
			if (ie.button3 == DOWN) {
				b3_pressed = true;
			}
			else 
#endif

#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
			if (!id.button1 && holdStart != 0) {
#else
			if (!ie.button1 == DOWN && holdStart != 0) {
#endif
				use_input_event();
				if (holdStart+250 > tguiCurrentTimeMillis()) {
					clicked = true;
				}
				holdStart = 0;
			}
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
			else if (id.button1 && holdStart == 0) {
#else
			else if (ie.button1 == DOWN && holdStart == 0) {
#endif
				use_input_event();
				holdStart = tguiCurrentTimeMillis();
			}

			else if (ie.left == DOWN || ie.up == DOWN) {
				use_input_event();
				playPreloadedSample("blip.ogg");
				tguiFocusPrevious();
			}
			else if (ie.right == DOWN || ie.down == DOWN) {
				use_input_event();
				playPreloadedSample("blip.ogg");
				tguiFocusNext();
			}
		}
		if (clicked) {
			clicked = false;
			return TGUI_RETURN;
		}
		return TGUI_CONTINUE;
	}

	virtual void setFocus(bool fcs)
	{
		TGUIWidget::setFocus(fcs);

#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
		was_down = use_dpad && getInput()->getDescriptor().button1;
#else
		was_down = false;
#endif
	}

	FakeWidget(int x, int y, int w, int h, bool accFocus = true, bool draw_outline = false) {
		this->hotkeys = 0;
		this->x = x;
		this->y = y;
		this->width = w;
		this->height = h;
		clicked = false;
		holdStart = 0;
		this->accFocus = accFocus;
		this->draw_outline = draw_outline;
		b3_pressed = false;
	}
private:
	bool clicked;
	unsigned long holdStart;
	bool accFocus;
	bool draw_outline;
	bool b3_pressed;
	bool was_down;
};


class MTab : public TGUIWidget {
public:
	void draw(void)
	{
		if (selected) {
			m_draw_bitmap(bmp, x, y, 0);
		}
		MCOLOR color = grey;
		if (use_dpad && this == tguiActiveWidget)
			color = m_map_rgb(255, 255, 0);
		mTextout_simple(_t(text.c_str()), x+width/2-m_text_length(game_font, _t(text.c_str()))/2, y+3, color);
	}

	void mouseUp(int x, int y, int b)
	{
		if (x >= 0 && y >= 0) {
			clicked = true;
		}
	}

	bool acceptsFocus() { return true; }
	
	int update(int step)
	{
		if (use_dpad && this == tguiActiveWidget) {
			INPUT_EVENT ie = get_next_input_event();
			if (ie.button1 == DOWN) {
				use_input_event();
				playPreloadedSample("select.ogg");
				clicked = true;
			}
			else if (ie.left == DOWN || ie.up == DOWN) {
				use_input_event();
				playPreloadedSample("blip.ogg");
				tguiFocusPrevious();
			}
			else if (ie.right == DOWN || ie.down == DOWN) {
				use_input_event();
				playPreloadedSample("blip.ogg");
				tguiFocusNext();
			}
		}

		if (clicked) {
			clicked = false;
			return TGUI_RETURN;
		}
		return TGUI_CONTINUE;
	}

	void setSelected(bool s)
	{
		selected = s;
	}
	
	MTab(std::string text, int x, int y) {
		this->hotkeys = 0;
		this->x = x;
		this->y = y;
		this->width = m_text_length(game_font, _t(text.c_str())) + 6;
		this->height = 15;
		this->text = text;
		clicked = false;
		int flags = al_get_new_bitmap_flags();
		al_set_new_bitmap_flags(flags & ~ALLEGRO_NO_PRESERVE_TEXTURE);
		bmp = m_create_bitmap(width, 14); // check
		al_set_new_bitmap_flags(flags);
		m_push_target_bitmap();
		m_set_target_bitmap(bmp);
		mDrawFrame(2, 2, width-4, 20);
		m_pop_target_bitmap();
		selected = false;
	}

	virtual ~MTab()
	{
		m_destroy_bitmap(bmp);
	}
private:
	bool clicked;
	MBITMAP *bmp;
	std::string text;
	bool selected;
};


extern Player *player;

class MDragNDropForm : public TGUIWidget {
public:
	void mouseDown(int x, int y, int b) {
		if (who >= 0) return;

		int slot = y / (BH/4);
		if (party[slot]) {
			who = slot;
			icon = m_load_bitmap(getResource("media/%s_profile.png", party[who]->getName().c_str()));
		}
		else {
			who = -1;
		}
	}

	void mouseUpAbs(int x, int y, int b)
	{
		bool clicked_on = x >= this->x && y >= this->y &&
			x < (this->x+this->width) && y < (this->y+this->height);

		if (clicked_on) {
			x -= this->x;
			y -= this->y;
		}
		else {
			x = y = -1;
		}

		if (x >= 0 && y >= 0 && who >= 0) {
			int dest_slot = y / (BH/4);

			if (who == dest_slot) {
				CombatFormation f = party[who]->getFormation();
				if (f == FORMATION_FRONT) {
					f = FORMATION_BACK;
				}
				else {
					f = FORMATION_FRONT;
				}
				party[who]->setFormation(f);
			}
			else {
				if (who == heroSpot)
					heroSpot = dest_slot;
				else if (dest_slot == heroSpot)
					heroSpot = who;
				Player *p = party[who];
				party[who] = party[dest_slot];
				party[dest_slot] = p;
				player = party[heroSpot];
			}

			who = -1;
			m_destroy_bitmap(icon);
		}
		else {
			if (who >= 0) {
				m_destroy_bitmap(icon);
				who = -1;
			}
		}
	}

	void draw(void) {
		if (who >= 0) {
			// FIXME: / 2 here for current_mouse_x should use a SCALE_X/SCALE_Y constant 
			ALLEGRO_MOUSE_STATE state;
			m_get_mouse_state(&state);
			int w = m_get_bitmap_width(icon);
			int h = m_get_bitmap_height(icon);
			al_draw_tinted_scaled_bitmap(icon->bitmap, al_map_rgba_f(0.75f, 0.75f, 0.75f, 0.75f), 0, 0, w, h,
				state.x-w, state.y-h, w*2, h*2, 0);
		}
	}

	bool acceptsFocus() { return false; }
	
	int update(int step)
	{
		return TGUI_CONTINUE;
	}
	
	MDragNDropForm(void) {
		this->hotkeys = 0;
		this->x = 0;
		this->y = 0;
		this->width = BW/4;
		this->height = BH;
		who = -1;
	}
private:
	int who;
	MBITMAP *icon;
};

enum MTableJustify {
	JUSTIFY_LEFT = 0,
	JUSTIFY_CENTER,
	JUSTIFY_RIGHT
};

struct MTableData {
	std::string text;
	MTableJustify justify;
	MCOLOR color;
	int width;
	int height;
};

class MTable : public TGUIWidget {
public:
	void setData(std::vector<std::vector<MTableData> > newData) {
		data = newData;
	}

	void draw(void)
	{
		int curr = x;
		for (int xx = 0; xx < columns-1; xx++) {
			curr += widths[xx];
			m_draw_line(curr, y, curr, y+total_height, line_color);
		}
		curr = y;
		for (int yy = 0; yy < rows-1; yy++) {
			curr += heights[yy];
			m_draw_line(x, curr, x+total_width, curr, line_color);
		}

		for (int r = 0; r < rows; r++) {
			for (int c = 0; c < columns; c++) {
				std::string& text = data[c][r].text;
				MTableJustify j = data[c][r].justify;
				MCOLOR color = data[c][r].color;

				int dx = 0;
				for (int i = 0; i < c; i++) {
					dx += widths[i];
				};
				int dy = 0;
				for (int i = 0; i < r; i++) {
					dy += heights[i];
				}
				dy += heights[r]/2-m_text_height(game_font)/2;

				dx += x;
				dy += y;

				switch (j) {
				case JUSTIFY_LEFT:
					mTextout_simple(text.c_str(),
						dx+3, dy, color);
					break;
				case JUSTIFY_CENTER:
					mTextout_simple(text.c_str(),
						dx+widths[c]/2-m_text_length(game_font, text.c_str())/2, dy, color);
					break;
				case JUSTIFY_RIGHT:
					mTextout_simple(text.c_str(),
						dx+widths[c]-m_text_length(game_font, text.c_str())-3, dy, color);
					break;
				}
			}
		}
	}

	int update(int millis)
	{
		return TGUI_CONTINUE;
	}

	bool acceptsFocus(void)
	{
		return false;
	}

	MTable(int x, int y,
		std::vector< std::vector< MTableData > > data,
		MCOLOR line_color) : data(data)
	{
		this->x = x;
		this->y = y;
		this->hotkeys = 0;

		this->line_color = line_color;

		columns = (int)data.size();
		rows = (int)data[0].size();

		total_width = 0;
		total_height = 0;

		for (int xx = 0; xx < columns; xx++) {
			int max = 0;
			for (int yy = 0; yy < rows; yy++) {
				if (data[xx][yy].width > max)
					max = data[xx][yy].width;
			}
			widths.push_back(max);
			total_width += max;
		}

		for (int yy = 0; yy < rows; yy++) {
			int max = 0;
			for (int xx = 0; xx < columns; xx++) {
				if (data[xx][yy].height > max)
					max = data[xx][yy].height;
			}
			heights.push_back(max);
			total_height += max;
		}

		this->width = total_width;
		this->height = total_height;
	}

	virtual ~MTable(void) {}
protected:
	std::vector<std::vector<MTableData> > data;
	MCOLOR line_color;
	std::vector<int> widths;
	std::vector<int> heights;
	int rows;
	int columns;
	int total_width;
	int total_height;
};



class MToggleList : public TGUIWidget {
public:
	void draw(void);
	int update(int millis);
	bool acceptsFocus(void);
	void mouseMove(int x, int y);
	void mouseUpAbs(int x, int y, int b);
	void mouseDownAbs(int x, int y, int b);
	int getSelected(void);
	void setItems(std::vector<std::string> items);
	void setFocus(bool f);
	void reset(void) { top = selected = cursor = 0; }
	int getTop(void) { return top; }
	void setTop(int t) { top = selected = cursor = t; }
	std::vector<bool> getToggled(void);
	void setToggled(std::vector<bool> t);

	MToggleList(int x, int y, int width, int height, bool onoff = true);
	virtual ~MToggleList(void);
protected:
	int top;
	int cursor;
	int selected;
	int up_selected;
	std::vector<std::string> items;
	bool down;
	int downX;
	int downY;
	bool dragging;
	MBITMAP *dragBmp;
	bool started;
	bool just_focused;
	MBITMAP *up_arrow;
	int rows;
	bool clicked;
	int down2X;
	int down2Y;
	bool scrolling;
	int scroll_offs;
	int first_finger_x; // x, y of first finger pressed at any time
	int first_finger_y;
	// Getting insane with all the variables here...
	int initial_down_x; // x, y of first finger pressed at the moment of first press
	int initial_down_y;
	void (*drop_callback)(int n);
	int drop_x;
	int drop_y;
	int downCount;
	std::vector<bool> toggled;
	bool onoff;
	int last_clicked;
	bool moved;
};



class MScrollingList : public TGUIWidget {
public:
	void draw(void);
	int update(int millis);
	bool acceptsFocus(void);
	void mouseMove(int x, int y);
	void mouseUpAbs(int x, int y, int b);
	void mouseDownAbs(int x, int y, int b);
	int getSelected(void);
	void setItems(std::vector<std::string> items);
	void setFocus(bool f);
	void reset(void) { top = selected = 0; }
	int getTop(void) { return top; }
	void setTop(int t) { top = selected = t; }

	MScrollingList(int x, int y, int width, int height, void (*drop_callback)(int n), int drop_x, int drop_y, void (*hold_callback)(int n, const void *data), const void *hold_data, bool do_prompt = true);
	virtual ~MScrollingList(void);
protected:
	int top;
	int selected;
	int up_selected;
	std::vector<std::string> items;
	bool down;
	int downX;
	int downY;
	bool dragging;
	MBITMAP *dragBmp;
	bool started;
	bool just_focused;
	MBITMAP *up_arrow;
	int rows;
	bool clicked;
	int down2X;
	int down2Y;
	bool scrolling;
	int scroll_offs;
	int first_finger_x; // x, y of first finger pressed at any time
	int first_finger_y;
	// Getting insane with all the variables here...
	int initial_down_x; // x, y of first finger pressed at the moment of first press
	int initial_down_y;
	void (*drop_callback)(int n);
	int drop_x;
	int drop_y;
	int downCount;
	void (*hold_callback)(int n, const void *data);
	const void *hold_data;
	long holdTime;
	bool do_prompt;
	bool was_down;
};



class MSpeechDialog : public TGUIWidget {
public:
	static const int MIN_HEIGHT = 21;
	static const int TRANSITION_IN_LENGTH = 300;
	static const int SCROLL_LENGTH = 300;
	void draw(void);
	int update(int millis);
	bool acceptsFocus(void);
	void mouseUp(int x, int y, int b);

	MSpeechDialog(int x, int y, int width, int height,
		std::string text);
	virtual ~MSpeechDialog(void);
protected:
	void realDrawText(int section, int xo, int yo);
	void drawText(void);
	bool transitionInDone;
	int drawHeight;
	int count;
	int currentSection;
	int numSections;
	bool scrollingSection;
	std::string untranslated_text;
	std::string text;
	bool clicked;
	int xx, yy, w, h;
};



class MCorner : public TGUIWidget {
public:
	void pre_draw(void)
	{
		m_draw_tinted_bitmap(bmp, al_map_rgb_f(blue.r+0.1f, blue.g+0.1f, blue.b+0.1f), x, y, 0);
	}

	bool acceptsFocus(void) { return false; }

	MCorner(int x, int y)
	{
		this->x = x; this->y = y;
		bmp = m_load_bitmap(getResource("media/corner.png"));
		hotkeys = 0;
	}

	virtual ~MCorner(void)
	{
		m_destroy_bitmap(bmp);
	}
private:
	MBITMAP *bmp;
};


class MIcon : public TGUIWidget {
public:
	void mouseDown(int x, int y, int b)
	{
		mouse_is_down = true;
		if (return_on_mouse_down) {
			playPreloadedSample("select.ogg");
			should_return = true;
		}
		else if (accFocus) {
			down = true;
		}
	}

	void mouseUp(int x, int y, int b)
	{
		mouse_is_down = false;
		if (x >= 0 && y >= 0 && accFocus && !return_on_mouse_down) {
			should_return = true;
			playPreloadedSample("select.ogg");
		}
	}
	
	void mouseMove(int x, int y) {
		if (!(x >= this->x && y >= this->y && x < this->x+width && y < this->y+height))
			mouse_is_down = false;
	}
	
	void mouseUpAbs(int x, int y, int b)
	{
		down = false;
	}

	void set_left_widget(TGUIWidget *w)
	{
		left_widget = w;
	}

	void set_right_widget(TGUIWidget *w)
	{
		right_widget = w;
	}

	void pre_draw(void);
	void draw(void);
	int update(int millis);
	bool acceptsFocus(void) { return accFocus; }
	void setBitmap(std::string filename) {
		m_destroy_bitmap(bitmap);
		bitmap = m_load_bitmap(filename.c_str());
	}
	MIcon(int x, int y, std::string filename, MCOLOR tint,
		bool accFocus = false, const char *name = NULL, bool show_name = false, bool alpha_image = false, bool show_focus = true, bool return_on_mouse_down = false, bool generate_repeat_presses = false);
	virtual ~MIcon(void);
private:
	MBITMAP *bitmap;
	MCOLOR tint;
	bool accFocus;
	bool down;
	bool should_return;
	const char *name;
	bool show_name;
	bool show_focus;
	bool return_on_mouse_down;
	double lastDown;
	bool generate_repeat_presses;
	bool mouse_is_down;
	TGUIWidget *left_widget, *right_widget;
};


class MFrame : public TGUIWidget {
public:
	void pre_draw(void);
	int update(int millis);
	MFrame(int x, int y, int width, int height, bool shadow = false);
	virtual ~MFrame(void);
private:
	bool shadow;
};


class MFrame_NormalDraw : public TGUIWidget {
public:
	void draw(void);
	int update(int millis);
	MFrame_NormalDraw(int x, int y, int width, int height, bool shadow = false);
	virtual ~MFrame_NormalDraw(void);
private:
	bool shadow;
};


class MSplitFrame : public MFrame {
public:
	bool acceptsFocus(void) { return false; }
	void pre_draw(void);
	int update(int millis);
	MSplitFrame(int x, int y, int width, int height, std::vector<int> splits);
	virtual ~MSplitFrame(void);
private:
	std::vector<int> splits;
};


class MLabel : public TGUIWidget {
public:
	void draw(void);
	int update(int millis);
	MLabel(int x, int y, std::string text, MCOLOR color);
	virtual ~MLabel(void);
protected:
	char text[100];
	MCOLOR color;
};


class MTextButton : public TGUIWidget {
public:
	void draw(void);
	int update(int millis);
	bool acceptsFocus(void);
	void mouseUp(int x, int y, int b);
	void set_left_widget(TGUIWidget *w)
	{
		left_widget = w;
	}

	MTextButton(int x, int y, std::string text, bool disabled = false, TGUIWidget *left_widget = NULL, TGUIWidget *right_widget = NULL);
	virtual ~MTextButton(void);
protected:
	char text[100];
	bool clicked;
	int shadow_type;
	bool disabled;
	TGUIWidget *left_widget, *right_widget;
};


class MTextButtonFullShadow : public MTextButton {
public:
	MTextButtonFullShadow(int x, int y, std::string text);
	virtual ~MTextButtonFullShadow(void);
};


class MToggle : public TGUIWidget {
public:
	void draw(void);
	int update(int millis);
	bool acceptsFocus(void);

	void setSelected(int s);
	int getSelected(void);

	MToggle(int x, int y, std::vector<std::string>& options);
	virtual ~MToggle(void);
protected:
	std::vector<std::string>& options;
	int selected;
};


class MSlider : public TGUIWidget {
public:
	void draw(void);
	int update(int millis);
	bool acceptsFocus(void);

	void setValue(int v);
	int getValue(void);

	MSlider(int x, int y);
	virtual ~MSlider(void);
protected:
	int value;
};


class MCheckbox : public TGUIWidget {
public:
	void draw(void);
	int update(int millis);
	bool acceptsFocus(void);

	void setChecked(bool c);
	bool getChecked(void);

	MCheckbox(int x, int y, std::string text);
	virtual ~MCheckbox(void);
protected:
	std::string text;
	bool checked;
};


class MShop : public TGUIWidget {
public:
	void draw(void);
	int update(int millis);
	bool acceptsFocus(void);
	int getRow(void);
	void setRow(int row);
	int getCol(void);
	void setCol(int col);
	void mouseUp(int x, int y, int b);
	
	MShop(int x, int y, int nItems, int *things, int *costs);
	virtual ~MShop(void);
protected:
	int nItems;
	int *things;
	int *costs;
	int top;
	int row, col;
	MBITMAP *up;
	AnimationSet *partyAnims[MAX_PARTY];
	bool up_clicked, down_clicked;
	bool clicked;
};


class MDoubleToggle : public TGUIWidget {
public:
	void draw(void);
	int update(int millis);
	bool acceptsFocus(void);
	int getSelected(void);
	void setSelected(int sel);
	void mouseUp(int x, int y, int b);
	
	MDoubleToggle(int x, int y, std::string text1,
		std::string text2);
	virtual ~MDoubleToggle(void);
protected:
	std::string text1;
	std::string text2;
	int selected;
};


class MSingleToggle : public TGUIWidget {
public:
	void draw(void);
	int update(int millis);
	bool acceptsFocus(void);
	int getSelected(void);
	void setSelected(int sel);
	void mouseDown(int x, int y, int b);
	
	MSingleToggle(int x, int y, std::vector<std::string> options, bool megashadow = false);
	virtual ~MSingleToggle(void);
protected:
	std::vector<std::string> options;
	int selected;
	bool megashadow;
};


class MRectangle : public TGUIWidget {
public:
	void pre_draw(void);
	int update(int millis);
	MRectangle(int x, int y, int w, int h,
		MCOLOR color, int flags);
	virtual ~MRectangle(void);
protected:
	MCOLOR color;
	int flags;
};


class MLevelUpHeader : public TGUIWidget {
public:
	void draw(void);
	int update(int millis);
	MLevelUpHeader(int *points, std::string spellCaption, bool bonus = false);
	virtual ~MLevelUpHeader(void);
protected:
	std::string spellCaption;
	int *points;
	bool bonus;
};


class MStats : public TGUIWidget {
public:
	void draw(void);
	int update(int millis);
	void setSelected(int index) { who = index; if (!party[who]) next(); }
	void mouseUp(int x, int y, int b);
	bool acceptsFocus(void) { return true; }
	MStats(int y, int h, int who, bool can_change);
	virtual ~MStats(void);
protected:
	void next(void);

	int who;
	bool can_change;
	MBITMAP *arrow;
	bool go_left;
	bool go_right;
};


class MStateSelector : public TGUIWidget {
public:
	void draw(void);
	int update(int millis);
	MStateSelector(void);
	virtual ~MStateSelector(void);
protected:
	std::vector< std::vector<MBITMAP *> > bitmaps;
};


class MParty : public TGUIWidget {
public:
	bool acceptsFocus(void) { return false; }
	void draw(void);
	int update(int millis);
	MParty(void);
	virtual ~MParty(void);
protected:
};


class MPartySelector : public TGUIWidget {
public:
	void setSelected(int s) { index = s; if (index != MAX_PARTY && !party[index]) next(); }
	int getSelected(void) { return index; }
	void grow(int dir) { growing = dir; }
	int getEquipIndex(void) { return equipIndex; }

	bool acceptsFocus(void);
	void draw(void);
	int update(int millis);
	void mouseDown(int x, int y, int b);
	void mouseUp(int x, int y, int b);
	void setFocus(bool f);

	MPartySelector(int y, int index, bool show_trash);
	virtual ~MPartySelector();
protected:
	void next(void);

	int index;
	bool show_trash;
	MBITMAP *arrow;
	bool left_half_clicked;
	bool right_half_clicked;
	//bool center_clicked;
	bool just_focused;
	bool started;
	int growing;
	int grow_count;
	int downCount;
	int downX;
	int downY;
	bool down;
	bool dragging;
	int equipIndex;
	MBITMAP *dragBmp;
	bool done;
};

class MMultiChooser : public TGUIWidget {
public:
	std::vector<int> &getSelected(void);
	void setSelected(std::vector<int> sel);
	bool acceptsFocus(void);
	void mouseDownAbs(int x, int y, int b);
	void mouseUp(int x, int y, int b);
	int getTapped(void);
	void setTapped(bool tapped);
	void setInset(bool i);
	bool getInset(void) { return inset; }

	void draw(void);
	int update(int millis);
	MMultiChooser(std::vector<MultiPoint> points, bool can_multi);
	virtual ~MMultiChooser(void);
protected:
	std::vector<MultiPoint> points;
	std::vector<int> current;
	MBITMAP *arrow;
	bool can_multi;
	bool call_callback;
	int closest;
	bool inset;
};



struct MMan {
	//Player *p;
	int x, y;
	bool go;
	bool used;
	bool dead;
};


class MManSelector : public TGUIWidget {
public:
	virtual void setFocus(bool fcs)
	{
		TGUIWidget::setFocus(fcs);

#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
		was_down = getInput()->getDescriptor().button1;
#else
		was_down = false;
#endif
	}

	bool acceptsFocus(void);
	void mark(int index, bool used, bool dead);
	void mouseDown(int x, int y, int b);
	void mouseUp(int x, int y, int b);

	void draw(void);
	int update(int millis);
	MManSelector(std::vector<MMan> mans);
	virtual ~MManSelector(void);
protected:
	std::vector<MMan> mans;
	MBITMAP *arrow;
	int pos;
	AnimationSet *go;
	bool clicked;
	float alpha;
	float alpha_inc;
	int holdTime;
	int holdx, holdy;
	int holdi, holdj;
	bool need_release;
	bool was_down;
};


class MItemSelector : public TGUIWidget {
public:
	void reset(void);
	int getSelected(void);
	void setSelected(int s);
	void grow(int dir) {
		growing = dir;
		selected = 0;
		pressed = -1;
		top = 0;
		if (dir == -1)
			rows-=2;
	}
	void setTop(int t);

	void mouseUpAbs(int x, int y, int b);
	void mouseDownAbs(int x, int y, int b);
	void mouseMove(int x, int y);

	void setFocus(bool f);
	bool acceptsFocus(void);
	void draw(void);
	int update(int millis);
	void setInventory(Inventory *i) { inventory = i; }
	void setRaiseOnFocus(bool r) { raiseOnFocus = r; }

	void setShop(void) { isShop = true; }
	void getDropLocation(int *dx, int *dy) {
		if (dx) *dx = drop_x;
		if (dy) *dy = drop_y;
	}

	MItemSelector(int y1, int y2, int top, int selected, bool canArrange);
	virtual ~MItemSelector(void);
protected:
	bool itemsBelow(void);

	int top;
	int selected;
	int rows;
	int y2;
	int pressed;
	MBITMAP *arrow;
	bool canArrange;
	bool clicked;
	MBITMAP *up_arrow;
	bool just_focused;
	bool started;
	int growing;
	int grow_count;
	int start_height;
	int start_y;
	bool down;
	int downX;
	int downY;
	int downCount;
	bool dragging;
	MBITMAP *dragBmp;
	int scrollCount;
	bool scrolling;
	int scroll_offs;
	int first_finger_x; // x, y of first finger pressed at any time
	int first_finger_y;
	// Getting insane with all the variables here...
	int initial_down_x; // x, y of first finger pressed at the moment of first press
	int initial_down_y;
	int down2X;
	int down2Y;
	bool isShop;
	int drop_x;
	int drop_y;
	Inventory *inventory;
	bool raiseOnFocus;
	bool proceed1, proceed2;
	bool maybe_scrolling;
	bool was_down;
};


class MSpellSelector : public TGUIWidget {
public:
	int getSelected(void);
	void setSelected(int s);
	void setTop(int t);
	void mouseUpAbs(int x, int y, int b);
	void mouseDownAbs(int x, int y, int b);
	void setFocus(bool f);
	void mouseMove(int x, int y);

	bool acceptsFocus(void);
	void draw(void);
	int update(int millis);
	MSpellSelector(int y1, int y2, int top, int selected, bool canArrange, MPartySelector *partySelector, CombatantInfo playerInfo);
	virtual ~MSpellSelector(void);
protected:
	bool spellsBelow(void);

	int top;
	int selected;
	int rows;
	int y2;
	int pressed;
	MBITMAP *arrow;
	bool canArrange;
	MPartySelector *partySelector;
	CombatantInfo playerInfo;
	bool clicked;
	MBITMAP *up_arrow;
	bool just_focused;
	bool started;
	int who;
	bool down;
	int downX;
	int downY;
	bool dragging;
	MBITMAP *dragBmp;
	int scrollCount;
	bool scrolling;
	int scroll_offs;
	int first_finger_x; // x, y of first finger pressed at any time
	int first_finger_y;
	// Getting insane with all the variables here...
	int initial_down_x; // x, y of first finger pressed at the moment of first press
	int initial_down_y;
	int down2X;
	int down2Y;
	bool changed;
	bool maybe_scrolling;
};


struct BmpAndPos {
	int x, y;
	MBITMAP *bitmap;
	std::string point1_name;
	std::string point2_name;
};


struct MapPoint {
	int x, y;
	std::string display_name;
	std::string internal_name;
	MapPoint *links[4];
	bool mapped;
	std::string dest_area;
	int dest_x;
	int dest_y;
	int dest_dir;
	int index;
};



class MMap : public TGUIWidget {
public:
	void auto_save(int millis, bool force);
	bool isTransitioning(void);
	std::string getSelected(void);
	void setSelected(std::string s);
	void flash(void);

	bool acceptsFocus(void);
	void draw(void);
	int update(int millis);
	void mouseUp(int x, int y, int b);
	void mouseDown(int x, int y, int b);

	MMap(std::string start, std::string prefix);
	virtual ~MMap(void);
protected:
	void load_map_data(void);
	void getLines(MapPoint *p);
	MapPoint *findPoint(std::string name);
	void getIdealPoint(int x, int y, int *dx, int *dy);
	BmpAndPos *findDots(std::string s1, std::string s2);

	MBITMAP *map_bmp;
	std::map<std::string, MBITMAP *> bitmaps;
	MBITMAP *down_arrow;
	std::vector<MapPoint> points;
	std::vector<BmpAndPos> lines;
	int selected;
	std::string start;
	bool transitioning;
	int top_x, top_y;
	float startx, starty;
	float destx, desty;
	float percent_moved;
	int count;
	std::string prev;
	std::string prefix;
	bool clicked;
	bool shouldFlash;
	int mem_save_counter;
	int ss_save_counter;
	int l_pressed, r_pressed, u_pressed, d_pressed;
	bool mouse_down;
	int downX, downY;
	int offset_x, offset_y;
};



extern MSpeechDialog *speechDialog;
void doDialogue(std::string text, bool top = false, int rows = 4, int offset = 10, bool bottom = false);
void notify(std::string msg1, std::string msg2, std::string msg3);
bool prompt(std::string msg1, std::string msg2, bool shake_choice, bool choice, std::string bottom_msg = "", bool *cancelled = NULL);
int triple_prompt(std::string msg1, std::string msg2, std::string msg3,
	std::string b1text, std::string b2text, std::string b3text, int shake_action,
	bool called_from_is_close_pressed = false);


void loadIcons(MBITMAP *bmp, RecreateData *data);
void destroyIcons(void);

extern MBITMAP *icon_bmp;

#endif

