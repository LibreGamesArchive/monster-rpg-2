#include "monster2.hpp"
#include <algorithm>

#ifdef ALLEGRO_ANDROID
#include "java.h"
#endif

extern bool joypad_connected(void);

#ifdef EDITOR
MFONT *big_font;
#endif
MFONT *game_font;
MFONT *game_font_second_display;
MSpeechDialog *speechDialog = NULL;


std::vector<MBITMAP *> icons;
MManSelector *manChooser = NULL;
MBITMAP *icon_bmp;

bool getting_input_config = false;

int show_item_info_on_flip = -1;

void loadIcons(MBITMAP *bmp, RecreateData *data)
{
	(void)data;

	#define N 40
	int pos[N*4] = {
		0, 9, 9, 9,
		0, 18, 9, 9,
		0, 27, 9, 9,
		0, 36, 9, 9,
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
		11, 90, 9, 9,
		11, 99, 9, 9,
#else
		0, 45, 9, 9,
		0, 54, 9, 9,
#endif		
		0, 63, 9, 9,
		0, 72, 9, 9,
		0, 81, 9, 9,
		0, 90, 9, 9,
		0, 99, 9, 9,
		0, 108, 9, 9,
		0, 117, 9, 9,
		0, 126, 9, 9,
		0, 135, 9, 9,
		0, 144, 9, 9,
		0, 153, 9, 9,
		0, 162, 9, 9,
		0, 171, 9, 9,
		0, 180, 9, 9,
		0, 189, 9, 9,
		0, 198, 9, 9,
		0, 207, 9, 9,
		0, 216, 9, 9,
		0, 225, 9, 9,
		0, 234, 9, 9,
		0, 243, 9, 9,
		0, 0, 11, 9,
		11, 0, 9, 9,
		11, 9, 9, 9,
		11, 18, 9, 9,
		11, 27, 9, 9,
		11, 36, 9, 9,
		11, 45, 9, 9,
		11, 54, 9, 9,
		11, 63, 9, 9,
		11, 72, 9, 9,
		11, 81, 9, 9,
		0, 45, 9, 9,
		0, 54, 9, 9
	};

	for (size_t i = 0; i < icons.size(); i++) {
		m_destroy_bitmap(icons[i]);
	}
	icons.clear();

	for (int i = 0; i < N; i++) {
		icons.push_back(
			m_create_sub_bitmap(
				bmp,
				pos[i*4+0], pos[i*4+1],
				pos[i*4+2], pos[i*4+3]
			)
		);
	}
}

void destroyIcons(void)
{
	for (size_t i = 0; i < icons.size(); i++) {
		m_destroy_bitmap(icons[i]);
	}
	m_destroy_bitmap(icon_bmp);

	icons.clear();
}

// prints only one word
static void mTextout_real(MFONT *font, const char *text, int x, int y,
		MCOLOR text_color, MCOLOR shadow_color,
		int shadowType, bool center)
{
	float inc = 1.0f;

	const char *ctext;
	char buf[100];

	// Show symbols
	if (text[0] == '{') {
		char num[4] = { 0, };
		for (int i = 0; i < 3; i++) {
			num[i] = text[i+1];
		}
		int index = atoi(num);
		m_save_blender();
		m_set_blender(M_ONE, M_INVERSE_ALPHA, white);
		m_draw_bitmap(icons[index], x, y-2, 0);
		m_restore_blender();
		x += m_text_height(font)+2;
		strcpy(buf, text+5);
		ctext = buf;
	}
	else {
		ctext = text;
	}
	
	void (*to)(const MFONT *, const char *text, int x, int y, MCOLOR color);
	
	to = m_textout;

	bool held = al_is_bitmap_drawing_held();
	al_hold_bitmap_drawing(true);

	switch (shadowType) {
		case WGT_TEXT_NORMAL:
			break;
		case WGT_TEXT_DROP_SHADOW:
			to(font, ctext, x+inc, y+inc, shadow_color);
			break;
		case WGT_TEXT_BORDER:
			to(font, ctext, x-inc, y, shadow_color);
			to(font, ctext, x-inc, y-inc, shadow_color);
			to(font, ctext, x, y-inc, shadow_color);
			to(font, ctext, x+inc, y-inc, shadow_color);
			to(font, ctext, x+inc, y, shadow_color);
			to(font, ctext, x+inc, y+inc, shadow_color);
			to(font, ctext, x, y+inc, shadow_color);
			to(font, ctext, x-inc, y+inc, shadow_color);
			break;
		case WGT_TEXT_SQUARE_BORDER:
			to(font, ctext, x+inc, y, shadow_color);
			to(font, ctext, x, y+inc, shadow_color);
			to(font, ctext, x+inc, y+inc, shadow_color);
			break;
	}
	
	to(font, ctext, x, y, text_color);

	al_hold_bitmap_drawing(held);
}




void mTextout(MFONT *font, const char *text, int x, int y,
		MCOLOR text_color, MCOLOR shadow_color,
		int shadowType, bool center)
{
	int words = 0;
	int i = 0;

	while (1) {
 		while ((text[i] == ' ' || text[i] == '\t' || text[i] == '\n') && text[i] != 0)
			i++;
		if (text[i] == 0)
			break;
		words++;
 		while (!(text[i] == ' ' || text[i] == '\t' || text[i] == '\n') && text[i] != 0)
			i++;
	}

	std::stringstream ss(text);
	std::string word;

	if (center) {
		x -= m_text_length(font, text)/2;
		y -= m_text_height(font)/2;
	}

	for (int i = 0; i < words; i++) {
		ss >> word;
		mTextout_real(font, word.c_str(), x, y,
			text_color, shadow_color,
			shadowType, center);
		x += m_text_length(font, word.c_str());
		x += m_text_length(font, " ");
	}
}

void mTextout_simple(const char *text, int x, int y, MCOLOR color)
{
	mTextout(game_font, text, x, y,
		color, black,
		WGT_TEXT_DROP_SHADOW, false);
}


void mDrawFrame(int x, int y, int w, int h, bool shadow)
{
	h = h - 1; // hack for compatibility

	m_save_blender();

	m_set_blender(M_ONE, M_INVERSE_ALPHA, white);

	if (use_programmable_pipeline && shadow) {
		int dx, dy, dw, dh;
		get_screen_offset_size(&dx, &dy, &dw, &dh);
		al_set_shader(display, shadow_shader);
		al_set_shader_float(shadow_shader, "x1", dx+x*screenScaleX);
		al_set_shader_float(shadow_shader, "y1", dy+y*screenScaleY);
		al_set_shader_float(shadow_shader, "x2", dx+(x+w)*screenScaleX);
		al_set_shader_float(shadow_shader, "y2", dy+(y+h)*screenScaleY);
		al_set_shader_float(shadow_shader, "BW", al_get_display_width(display));
		al_set_shader_float(shadow_shader, "BH", al_get_display_height(display));
		al_set_shader_float(shadow_shader, "radius", screenScaleX*10);
		al_use_shader(shadow_shader, true);
		m_draw_rectangle(x-10, y-10, x+w+10, y+h+10, black, M_FILLED);
		al_use_shader(shadow_shader, false);
		al_set_shader(display, default_shader);
	}


	m_set_blender(M_ONE, M_ZERO, white);

	al_hold_bitmap_drawing(true);

	MBITMAP *b = guiAnims.wide_sub;
	int sprite_w = m_get_bitmap_width(b);
	int sprite_h = m_get_bitmap_height(b);
	al_draw_scaled_bitmap(b->bitmap, 0, 0, sprite_w, sprite_h, x, y-sprite_h, w, sprite_h, 0);
	al_draw_scaled_bitmap(b->bitmap, 0, 0, sprite_w, sprite_h, x, y+h, w, sprite_h, ALLEGRO_FLIP_VERTICAL);

	b = guiAnims.tall_sub;
	sprite_w = m_get_bitmap_width(b);
	sprite_h = m_get_bitmap_height(b);
	al_draw_scaled_bitmap(b->bitmap, 0, 0, sprite_w, sprite_h, x-sprite_w, y, sprite_w, h, 0);
	al_draw_scaled_bitmap(b->bitmap, 0, 0, sprite_w, sprite_h, x+w, y, sprite_w, h, ALLEGRO_FLIP_HORIZONTAL);

	b = guiAnims.corner_sub;
	sprite_w = m_get_bitmap_width(b);
	sprite_h = m_get_bitmap_height(b);
	m_draw_bitmap(b, x-sprite_w, y-sprite_h, 0);
	m_draw_bitmap(b, x+w, y-sprite_h, M_FLIP_HORIZONTAL);
	m_draw_bitmap(b, x+w, y+h, M_FLIP_HORIZONTAL | M_FLIP_VERTICAL);
	m_draw_bitmap(b, x-sprite_w, y+h, M_FLIP_VERTICAL);

	al_hold_bitmap_drawing(false);


	MCOLOR color = blue;

	m_draw_rectangle(x, y, x+w, y+h, color, M_FILLED);

	m_set_blender(M_ONE, M_INVERSE_ALPHA,
		m_map_rgb_f(blue.r+0.1, blue.g+0.1, blue.b+0.1));

	int xxx, yyy, www, hhh; // ya
	al_get_clipping_rectangle(&xxx, &yyy, &www, &hhh);
	
	m_set_clip(x, y, x+w, y+h);

	m_draw_bitmap(corner_bmp, x+w-m_get_bitmap_width(corner_bmp), y, 0);
	
	al_set_clipping_rectangle(xxx, yyy, www, hhh);

	if (!use_programmable_pipeline && shadow) {
		m_draw_scaled_bitmap(
			shadow_sheet,
			0, 0,
			1, SHADOW_CORNER_SIZE,
			x-2, y-2-SHADOW_CORNER_SIZE,
			w+4, SHADOW_CORNER_SIZE,
			0
		);
		#ifdef A5_OGL
		int true_w, true_h;
		al_get_opengl_texture_size(shadow_sheet->bitmap, &true_w, &true_h);
		#endif
		m_draw_scaled_bitmap(
			shadow_sheet,
		#ifdef A5_OGL
			32 + (0.5 / true_w), 0,
			0.5, SHADOW_CORNER_SIZE,
		#else
			32, 0,
			1, SHADOW_CORNER_SIZE,
		#endif
			x-2, y+h+2,
			w+4, SHADOW_CORNER_SIZE,
			0
		);
		m_draw_scaled_bitmap(
			shadow_sheet,
			16, 0,
			SHADOW_CORNER_SIZE, 1,
			x+w+2, y-2,
			SHADOW_CORNER_SIZE, h+4,
			0
		);
		m_draw_scaled_bitmap(
			shadow_sheet,
			48, 0,
			SHADOW_CORNER_SIZE, 1,
			x-2-SHADOW_CORNER_SIZE, y-2,
			SHADOW_CORNER_SIZE, h+4,
			0
		);

		al_draw_bitmap_region(shadow_sheet->bitmap, 0, 16, SHADOW_CORNER_SIZE, SHADOW_CORNER_SIZE, x-2-SHADOW_CORNER_SIZE, y-2-SHADOW_CORNER_SIZE, 0);
		al_draw_bitmap_region(shadow_sheet->bitmap, SHADOW_CORNER_SIZE, 16, SHADOW_CORNER_SIZE, SHADOW_CORNER_SIZE, x+w+2, y-2-SHADOW_CORNER_SIZE, 0);
		al_draw_bitmap_region(shadow_sheet->bitmap, SHADOW_CORNER_SIZE*2, 16, SHADOW_CORNER_SIZE, SHADOW_CORNER_SIZE, x+w+2, y+h+2, 0);
		al_draw_bitmap_region(shadow_sheet->bitmap, SHADOW_CORNER_SIZE*3, 16, SHADOW_CORNER_SIZE, SHADOW_CORNER_SIZE, x-2-SHADOW_CORNER_SIZE, y+h+2, 0);
	}

	m_restore_blender();
}


static void notify(void (*draw_callback)(int x, int y, int w, int h, void *data), void *data)
{
	if (!inited) return;
	
	dpad_off();
	
	bool delayed = false;

	int dx, dy, dw, dh;
	get_screen_offset_size(&dx, &dy, &dw, &dh);
	int flags = al_get_new_bitmap_flags();
	al_set_new_bitmap_flags(flags & ~ALLEGRO_NO_PRESERVE_TEXTURE);
	MBITMAP *tmp = m_create_bitmap(dw, dh);
	al_set_new_bitmap_flags(flags);
	m_draw_scaled_backbuffer(dx, dy, dw, dh, 0, 0, dw, dh, tmp);

	int w = 230;
	int h = 100;
	int x = (BW-w)/2;
	int y = (BH-h)/2;
	int th = m_text_height(game_font);
	int tw = m_text_length(game_font, _t("OK"));

	tguiPush();
	FakeWidget *w1 = new FakeWidget(BW/2-tw/2, (y+h-15)-th/2-2, tw, 16);
	MRectangle *fullscreenRect = new MRectangle(0, 0, BW, BH,
		m_map_rgba(0, 0, 0, 0), 0);
	tguiSetParent(0);
	tguiAddWidget(fullscreenRect);
	tguiSetParent(fullscreenRect);
	tguiAddWidget(w1);
	tguiSetFocus(w1);

	double started = al_get_time();
	bool updating = false;

	while (1) {
		al_wait_cond(wait_cond, wait_mutex);
		int tmp_counter = logic_counter;
		logic_counter = 0;
		if (tmp_counter > 10)
			tmp_counter = 1;
		while (tmp_counter > 0) {
			next_input_event_ready = true;

			tmp_counter--;
			if (is_close_pressed()) {
				do_close();
				close_pressed = false;
			}
			if (break_main_loop) {
				goto done;
			}

			if (!updating) {
				if (al_get_time() > started+0.5) {
					clear_input_events();
					updating = true;
				}
			}
			else {
				INPUT_EVENT ie = get_next_input_event();
				if (iphone_shaken(0.1) || ie.button1 == DOWN || ie.button2 == DOWN) {
					use_input_event();
					playPreloadedSample("select.ogg");
					al_rest(0.25);
					goto done;
				}
				if (tguiUpdate() == w1) {
					playPreloadedSample("select.ogg");
					goto done;
				}
			}
		}

		if (draw_counter > 0) {
			draw_counter = 0;

			al_set_target_backbuffer(display);
			m_draw_bitmap_identity_view(tmp, dx, dy, 0);
			// Draw frame
			mDrawFrame(x, y, w, h, true);
			draw_callback(x, y, w, h, data);
			// Draw "button"
			mTextout(game_font, _t("OK"), BW/2, y+h-15,
				grey, black,
				WGT_TEXT_DROP_SHADOW, true);
			// Draw "cursor"
			int tick = (unsigned)tguiCurrentTimeMillis() % 1000;
			if (tick < 800) {
				int size = m_get_bitmap_width(cursor);
				int rx = BW/2-m_text_length(game_font, _t("OK"))/2-size-2;
				int ry = y+h-15-7;
				m_draw_bitmap(cursor, rx, ry, 0);
			}
			drawBufferToScreen();
			m_flip_display();
			
			if (!delayed) {
				delayed = true;
				m_rest(0.25);
			}
		}
	}
done:
	
	dpad_on();
	
	tguiDeleteWidget(fullscreenRect);
	delete fullscreenRect;
	delete w1;

	tguiPop();

	m_destroy_bitmap(tmp);
	
	waitForRelease(4);
	waitForRelease(5);
	clear_input_events();

	return;
}

static void _drawSimpleStatus_real(Player *p, int x, int y, CombatantInfo info)
{
	MBITMAP *icon = p->getIcon();
	
	m_set_blender(M_ONE, M_INVERSE_ALPHA, white);
	if (info.condition == CONDITION_POISONED) {
		al_draw_filled_rectangle(x-1, y-1, x+1+al_get_bitmap_width(icon->bitmap), y+1+al_get_bitmap_height(icon->bitmap), al_map_rgb_f(1.0f, 0.0f, 1.0f));
	}
	m_draw_bitmap(icon, x, y, 0);
	int w = m_get_bitmap_width(icon);
	
	mTextout(game_font, _t(p->getName().c_str()), x+w+2, y,
				grey, black,
				WGT_TEXT_DROP_SHADOW, false);
	
	char text[100];
	sprintf(text, _t("Level %d"), getLevel(info.experience));
	mTextout(game_font, text, x+w+2, y+9,
				grey, black,
				WGT_TEXT_DROP_SHADOW, false);
	sprintf(text, _t("HP %d/%d"), (info.abilities.hp > 0) ?
			  info.abilities.hp : 0, info.abilities.maxhp);
	mTextout(game_font, text, x+w+2, y+18,
				grey, black,
				WGT_TEXT_DROP_SHADOW, false);
	sprintf(text, _t("MP %d/%d"), info.abilities.mp, info.abilities.maxmp);
	mTextout(game_font, text, x+w+2, y+27,
				grey, black,
				WGT_TEXT_DROP_SHADOW, false);
}

static void drawSimpleStatus_real(int partyMember, int x, int y, CombatantInfo info)
{
	Player *p = party[partyMember];
	_drawSimpleStatus_real(p, x, y, info);
}

static void _drawSimpleStatus(Player *p, int x, int y)
{
	CombatantInfo info = p->getInfo();
	_drawSimpleStatus_real(p, x, y, info);
}

static void drawSimpleStatus(int partyMember, int x, int y)
{
	Player *p = party[partyMember];
	_drawSimpleStatus(p, x, y);
}

static void playerMiniInfoDraw_battle(int x, int y, int w, int h, void *data)
{
	int who = (int)(int64_t)data;
	CombatantInfo info = battle->findPlayer(who)->getInfo();
	drawSimpleStatus_real(who, x+5, y+5, info);
	char text[100];
	if (info.abilities.hp <= 0) {
		strcpy(text, _t("Unconscious"));
	}
	else {
		std::string cond = getConditionName(info.condition);
		sprintf(text, _t("Status: %s"), _t(cond == "" ? "Normal" : cond.c_str()));
	}
	mTextout(game_font, text, x+5, y+41,
		grey, black,
		WGT_TEXT_DROP_SHADOW, false);
}

static void playerMiniInfoDraw_nobattle(int x, int y, int w, int h, void *data)
{
	Player *p = (Player *)data;
	CombatantInfo &info = p->getInfo();
	_drawSimpleStatus_real(p, x+5, y+5, info);
	char text[100];
	if (info.abilities.hp <= 0) {
		strcpy(text, _t("Unconscious"));
	}
	else {
		std::string cond = getConditionName(info.condition);
		sprintf(text, _t("Status: %s"), _t(cond == "" ? "Normal" : cond.c_str()));
	}
	mTextout(game_font, text, x+5, y+40,
		grey, black,
		WGT_TEXT_DROP_SHADOW, false);
}

void showPlayerInfo_number(int who)
{
	notify(playerMiniInfoDraw_battle, (void *)(int64_t)who);
}

void showPlayerInfo_ptr(Player *p)
{
	notify(playerMiniInfoDraw_nobattle, p);
}

void notify(std::string msg1, std::string msg2, std::string msg3)
{
	if (!inited) return;

	dpad_off();

	int dx, dy, dw, dh;
	get_screen_offset_size(&dx, &dy, &dw, &dh);
	int flags = al_get_new_bitmap_flags();
	al_set_new_bitmap_flags(flags & ~ALLEGRO_NO_PRESERVE_TEXTURE);
	MBITMAP *tmp = m_create_bitmap(dw, dh);
	al_set_new_bitmap_flags(flags);
	m_draw_scaled_backbuffer(dx, dy, dw, dh, 0, 0, dw, dh, tmp);
	
	int w = 200;
	int h = 65;
	int x = (BW-w)/2;
	int y = (BH-h)/2;
	int th = m_text_height(game_font);

	tguiClearMouseEvents();

	tguiPush();

	MRectangle *fullscreenRect = new MRectangle(0, 0, BW, BH,
		m_map_rgba(0, 0, 0, 0), 0);
	FakeWidget *w1 = new FakeWidget(BW/2-6, y+53-th/2-2, 12, 12);
	tguiSetParent(0);
	tguiAddWidget(fullscreenRect);
	tguiSetParent(fullscreenRect);
	tguiAddWidget(w1);
	tguiSetFocus(w1);

	clear_input_events();

	while (1) {
		al_wait_cond(wait_cond, wait_mutex);
		int tmp_counter = logic_counter;
		logic_counter = 0;
		if (tmp_counter > 10)
			tmp_counter = 1;
		while (tmp_counter > 0) {
			next_input_event_ready = true;

			tmp_counter--;
			if (is_close_pressed()) {
				do_close();
				close_pressed = false;
			}
			if (break_main_loop) {
				goto done;
			}

			INPUT_EVENT ie = get_next_input_event();
			if (iphone_shaken(0.1) || ie.button1 == DOWN || ie.button2 == DOWN) {
				use_input_event();
				playPreloadedSample("select.ogg");
				goto done;
			}
			if (tguiUpdate() == w1) {
				playPreloadedSample("select.ogg");
				goto done;
			}
		}

		if (draw_counter > 0) {
			draw_counter = 0;
			al_set_target_backbuffer(display);
			m_clear(m_map_rgb(0, 0, 0));
			m_draw_bitmap_identity_view(tmp, dx, dy, 0);
			// Draw frame
			mDrawFrame(x, y, w, h, true);
			// Draw messages
			mTextout(game_font, _t(msg1.c_str()), BW/2, y+5+m_text_height(game_font)/2,
				grey, black,
				WGT_TEXT_DROP_SHADOW, true);
			mTextout(game_font, _t(msg2.c_str()), BW/2, y+5+m_text_height(game_font)/2+m_text_height(game_font),
				grey, black,
				WGT_TEXT_DROP_SHADOW, true);
			mTextout(game_font, _t(msg3.c_str()), BW/2, y+5+m_text_height(game_font)/2+m_text_height(game_font)*2,
				grey, black,
				WGT_TEXT_DROP_SHADOW, true);
			// Draw "button"
			mTextout(game_font, _t("OK"), BW/2, y+5+48,
				grey, black,
				WGT_TEXT_DROP_SHADOW, true);
			// Draw "cursor"
			int tick = (unsigned)tguiCurrentTimeMillis() % 1000;
			if (tick < 800) {
				int size = m_get_bitmap_width(cursor);
				int rx = BW/2-m_text_length(game_font, _t("OK"))/2-size-2;
				int ry = y+5+48-7;
				m_draw_bitmap(cursor, rx, ry, 0);
			}
			drawBufferToScreen();
			m_flip_display();
		}
	}
done:
	
	tguiDeleteWidget(fullscreenRect);
	delete fullscreenRect;
	delete w1;

	tguiPop();
	
	dpad_on();
	
	m_destroy_bitmap(tmp);
	
	if (use_dpad) {
		waitForRelease(4);
	}

	return;
}



int triple_prompt(std::string msg1, std::string msg2, std::string msg3,
	std::string b1text, std::string b2text, std::string b3text, int shake_action,
	bool called_from_is_close_pressed)
{
	dpad_off();
	
	int dx, dy, dw, dh;
	get_screen_offset_size(&dx, &dy, &dw, &dh);
	int flags = al_get_new_bitmap_flags();
	al_set_new_bitmap_flags(flags & ~ALLEGRO_NO_PRESERVE_TEXTURE);
	MBITMAP *tmp = m_create_bitmap(dw, dh);
	al_set_new_bitmap_flags(flags);
	m_draw_scaled_backbuffer(dx, dy, dw, dh, 0, 0, dw, dh, tmp);
	
	int w = 230;
	int h = 100;
	int x = (BW-w)/2;
	int y = (BH-h)/2;
	int choice = 2;

	tguiClearMouseEvents();
	
	int x1, x2, x3, xinc;
	xinc = (w-20)/3;
	x1 = (BW-w)/2+xinc/2+10;
	x2 = x1 + xinc;
	x3 = x2 + xinc;
	int ty = y+h-15;
	int my1, my2, my3;
	my1 = y + 20;
	my2 = my1 + m_text_height(game_font) + 3;
	my3 = my2 + m_text_height(game_font) + 3;

	int l1 = m_text_length(game_font, _t(b1text.c_str()));
	int l2 = m_text_length(game_font, _t(b2text.c_str()));
	int l3 = m_text_length(game_font, _t(b3text.c_str()));
	int th = m_text_height(game_font);

	tguiPush();
	MRectangle *fullscreenRect = new MRectangle(0, 0, BW, BH,
		m_map_rgba(0, 0, 0, 0), 0);
	FakeWidget *w1 = new FakeWidget(x1-l1/2, ty-th/2-2, l1, 16);
	FakeWidget *w2 = new FakeWidget(x2-l2/2, ty-th/2-2, l2, 16);
	FakeWidget *w3 = new FakeWidget(x3-l3/2, ty-th/2-2, l3, 16);
	tguiSetParent(0);
	tguiAddWidget(fullscreenRect);
	tguiSetParent(fullscreenRect);
	tguiAddWidget(w1);
	tguiAddWidget(w2);
	tguiAddWidget(w3);
	tguiSetFocus(w3);

	clear_input_events();

	while (1) {
		al_wait_cond(wait_cond, wait_mutex);
		int tmp_counter = logic_counter;
		logic_counter = 0;
		if (tmp_counter > 10)
			tmp_counter = 1;
		while (tmp_counter > 0) {
			next_input_event_ready = true;

			tmp_counter--;
			if (is_close_pressed(called_from_is_close_pressed)) {
				do_close();
				close_pressed = false;
			}
			if (break_main_loop) {
				goto done;
			}
			TGUIWidget *widget = tguiUpdate();
			if (widget == w1) {
				playPreloadedSample("select.ogg");
				choice = 0;
				goto done;
			}
			else if (widget == w2) {
				playPreloadedSample("select.ogg");
				choice = 1;
				goto done;
			}
			else if (widget == w3) {
				playPreloadedSample("select.ogg");
				choice = 2;
				goto done;
			}
			INPUT_EVENT ie = { 0, };
			ie = get_next_input_event();
			if (iphone_shaken(0.1) || ie.button2 == DOWN) {
				use_input_event();
				iphone_clear_shaken();
				playPreloadedSample("select.ogg");
				choice = shake_action;
				m_rest(0.25);
				goto done;
			}
		}

		if (draw_counter > 0) {
			draw_counter = 0;
			al_set_target_backbuffer(display);
			m_clear(m_map_rgb(0, 0, 0));
			m_draw_bitmap_identity_view(tmp, dx, dy, 0);
			// Draw frame
			mDrawFrame(x, y, w, h, true);
			// Draw messages
			mTextout(game_font, _t(msg1.c_str()), BW/2, my1,
				grey, black,
				WGT_TEXT_DROP_SHADOW, true);
			mTextout(game_font, _t(msg2.c_str()), BW/2, my2,
				grey, black,
				WGT_TEXT_DROP_SHADOW, true);
			mTextout(game_font, _t(msg3.c_str()), BW/2, my3,
				grey, black,
				WGT_TEXT_DROP_SHADOW, true);
			// Draw "buttons"
			mTextout(game_font, _t(b1text.c_str()), x1, ty,
				grey, black,
				WGT_TEXT_DROP_SHADOW, true);
			mTextout(game_font, _t(b2text.c_str()), x2, ty,
				grey, black,
				WGT_TEXT_DROP_SHADOW, true);
			mTextout(game_font, _t(b3text.c_str()), x3, ty,
				grey, black,
				WGT_TEXT_DROP_SHADOW, true);
			// Draw "cursor"
			if (w1->getFocus()) choice = 0;
			if (w2->getFocus()) choice = 1;
			if (w3->getFocus()) choice = 2;
			int tick = (unsigned)tguiCurrentTimeMillis() % 1000;
			if (tick < 800) {
				int size = m_get_bitmap_width(cursor);
				int rx = 0;
				if (choice == 0)
					rx = x1 - size-2-m_text_length(game_font, _t(b1text.c_str()))/2;
				else if (choice == 1)
					rx = x2 - size-2-m_text_length(game_font, _t(b2text.c_str()))/2;
				else if (choice == 2)
					rx = x3 - size-2-m_text_length(game_font, _t(b3text.c_str()))/2;
				int ry = ty-7;
				m_draw_bitmap(cursor, rx, ry, 0);
			}
			drawBufferToScreen();
			m_flip_display();
		}
		m_rest(0.01);
	}
done:
	tguiDeleteWidget(fullscreenRect);
	delete fullscreenRect;
	delete w1;
	delete w2;
	delete w3;

	tguiPop();

	dpad_on();
	
	m_destroy_bitmap(tmp);
		
	waitForRelease(4);

	return choice;
}



bool prompt(std::string msg1, std::string msg2, bool shake_choice, bool choice, std::string bottom_msg, bool *cancelled, bool wide)
{
	dpad_off();

	int dx, dy, dw, dh;
	get_screen_offset_size(&dx, &dy, &dw, &dh);
	int flags = al_get_new_bitmap_flags();
	al_set_new_bitmap_flags(flags & ~ALLEGRO_NO_PRESERVE_TEXTURE);
	MBITMAP *tmp = m_create_bitmap(dw, dh);
	al_set_new_bitmap_flags(flags);
	m_draw_scaled_backbuffer(dx, dy, dw, dh, 0, 0, dw, dh, tmp);
	
	int w;
	if (wide) {
		w = 230;
	}
	else {
		w = 160;
	}
	int h = 50;
	int x = (BW-w)/2;
	int y = (BH-h)/2;

	tguiClearMouseEvents();
		
	const char *s1, *s2;
#if defined ALLEGRO_IPHONE || defined ALLEGRO_MACOSX
	s1 = "No";
	s2 = "Yes";
#else
	s1 = "Yes";
	s2 = "No";
#endif
	int len_1, len_2;
	len_1 = m_text_length(game_font, _t(s1));
	len_2 = m_text_length(game_font, _t(s2));
	int th = m_text_height(game_font);

	tguiPush();

	MRectangle *fullscreenRect = new MRectangle(0, 0, BW, BH,
		m_map_rgba(0, 0, 0, 0), 0);
	FakeWidget *w1 = new FakeWidget(BW/2-w/4-(len_1/2), (y+43)-th/2-2,
		len_1, 16);
	FakeWidget *w2 = new FakeWidget(BW/2+w/4-(len_2/2), (y+43)-th/2-2,
		len_2, 16);
	tguiSetParent(0);
	tguiAddWidget(fullscreenRect);
	tguiSetParent(fullscreenRect);
	tguiAddWidget(w1);
	tguiAddWidget(w2);

	#if defined ALLEGRO_IPHONE || defined ALLEGRO_MACOSX
	if (choice)
		tguiSetFocus(w2);
	else
		tguiSetFocus(w1);
	#else
	if (choice)
		tguiSetFocus(w1);
	else
		tguiSetFocus(w2);
	#endif

	clear_input_events();

	for (;;) {
		al_wait_cond(wait_cond, wait_mutex);
		// Logic
		int tmp_counter = logic_counter;
		logic_counter = 0;
		if (tmp_counter > 10)
			tmp_counter = 1;
		while (tmp_counter > 0) {
			next_input_event_ready = true;

			tmp_counter--;
			if (msg1 != "Really exit?") {
				if (is_close_pressed()) {
					do_close();
					close_pressed = false;
				}
				if (break_main_loop) {
					goto done;
				}
			}
			TGUIWidget *widget = tguiUpdate();
			if (widget == w1) {
				playPreloadedSample("select.ogg");
				#if defined ALLEGRO_IPHONE || defined ALLEGRO_MACOSX
				choice = false;
				#else
				choice = true;
				#endif
				goto done;
			}
			else if (widget == w2) {
				playPreloadedSample("select.ogg");
				#if defined ALLEGRO_IPHONE || defined ALLEGRO_MACOSX
				choice = true;
				#else
				choice = false;
				#endif
				goto done;
			}
		}

		if (draw_counter) {
			draw_counter = 0;
			al_set_target_backbuffer(display);
			m_clear(m_map_rgb(0, 0, 0));
			m_draw_bitmap_identity_view(tmp, dx, dy, 0);
			// Draw frame
			mDrawFrame(x, y, w, h, true);
			// Draw messages
			mTextout(game_font, _t(msg1.c_str()), BW/2, y+5+m_text_height(game_font)/2,
				grey, black,
				WGT_TEXT_DROP_SHADOW, true);
			mTextout(game_font, _t(msg2.c_str()), BW/2, y+5+m_text_height(game_font)/2+m_text_height(game_font),
				grey, black,
				WGT_TEXT_DROP_SHADOW, true);
			// Draw "buttons"
			mTextout(game_font, _t(s1), BW/2-w/4, y+5+38,
				grey, black,
				WGT_TEXT_DROP_SHADOW, true);
			mTextout(game_font, _t(s2), BW/2+w/4, y+5+38,
				grey, black,
				WGT_TEXT_DROP_SHADOW, true);
			mTextout(game_font, _t(bottom_msg.c_str()), BW/2, BH-10,
				grey, black,
				WGT_TEXT_BORDER, true);
			// Draw "cursor"
			int tick = (unsigned)tguiCurrentTimeMillis() % 1000;
			if (tick < 800) {
				int size = m_get_bitmap_width(cursor);
				int rx = 0;
#if defined ALLEGRO_IPHONE || defined ALLEGRO_MACOSX
				if (w1->getFocus()) {
					rx = BW/2-w/4-m_text_length(game_font, _t("No"))/2-size-2;
				}
				else if (w2->getFocus()) {
					rx = BW/2+w/4-m_text_length(game_font, _t("Yes"))/2-size-2;
				}
#else
				bool sel = tguiActiveWidget == w1 ? true : false;
				if (sel) {
					rx = BW/2-w/4-m_text_length(game_font, _t("Yes"))/2-size-2;
				}
				else {
					rx = BW/2+w/4-m_text_length(game_font, _t("No"))/2-size-2;
				}
#endif
				int ry = y+5+38-7;
				m_draw_bitmap(cursor, rx, ry, 0);
			}
			drawBufferToScreen();
			m_flip_display();
		}
	}
done:
	tguiDeleteWidget(fullscreenRect);
	delete fullscreenRect;
	delete w1;
	delete w2;

	tguiPop();

	dpad_on();

	m_destroy_bitmap(tmp);

	if (use_dpad) {
		waitForRelease(4);
	}

	return choice;
}


int config_input(int type)
{
	dpad_off();

	tguiPush();

	MFrame *frame = new MFrame(0, 0, BW, BH);

	MInputGetter *getters[15];
	int num_getters;
	
	if (type == MInputGetter::TYPE_KB) {
		num_getters = 15;
		getters[0] = new MInputGetter(type, 1, 5, 170, _t("Action Key"), config.getKey1());
		getters[1] = new MInputGetter(type, 1, 15, 170, _t("Back Key"), config.getKey2());
		getters[2] = new MInputGetter(type, 1, 25, 170, _t("View Key"), config.getKey3());
		getters[3] = new MInputGetter(type, 1, 35, 170, _t("Left Key"), config.getKeyLeft());
		getters[4] = new MInputGetter(type, 1, 45, 170, _t("Right Key"), config.getKeyRight());
		getters[5] = new MInputGetter(type, 1, 55, 170, _t("Up Key"), config.getKeyUp());
		getters[6] = new MInputGetter(type, 1, 65, 170, _t("Down Key"), config.getKeyDown());
		getters[7] = new MInputGetter(type, 1, 75, 170, _t("Settings Key"), config.getKeySettings());
		getters[8] = new MInputGetter(type, 1, 85, 170, _t("Fullscreen Key"), config.getKeyFullscreen());
		getters[9] = new MInputGetter(type, 1, 95, 170, _t("SFX Up Key"), config.getKeySFXUp());
		getters[10] = new MInputGetter(type, 1, 105, 170, _t("SFX Down Key"), config.getKeySFXDown());
		getters[11] = new MInputGetter(type, 1, 115, 170, _t("Music Up Key"), config.getKeyMusicUp());
		getters[12] = new MInputGetter(type, 1, 125, 170, _t("Music Down Key"), config.getKeyMusicDown());
		getters[13] = new MInputGetter(type, 1, 135, 170, _t("Quit Key"), config.getKeyQuit());
		getters[14] = new MInputGetter(type, 1, 145, 170, _t("Sort Items Key"), config.getKeySortItems());
	}
	else {
		num_getters= 3;
		getters[0] = new MInputGetter(type, 1, 5, 170, _t("Action Button"), config.getJoyButton1());
		getters[1] = new MInputGetter(type, 1, 15, 170, _t("Back Button"), config.getJoyButton2());
		getters[2] = new MInputGetter(type, 1, 25, 170, _t("View Button"), config.getJoyButton3());
	}

	MTextButton *apply = new MTextButton(172, 125, "Apply");

#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_ANDROID
	MTextButton *other = new MTextButton(172, 145, type == MInputGetter::TYPE_KB ? "Gamepad" : "Keyboard");
#endif

	int ret = 0;

	tguiSetParent(0);
	tguiAddWidget(frame);
	tguiSetParent(frame);

	for (int i = 0; i < num_getters; i++) {
		tguiAddWidget(getters[i]);
	}

	tguiAddWidget(apply);
#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_ANDROID
	tguiAddWidget(other);
#endif

	tguiSetFocus(getters[0]);

	al_set_target_backbuffer(display);
	tguiDraw();
	drawBufferToScreen();
	fadeIn(black);

	clear_input_events();

	for (;;) {
		al_wait_cond(wait_cond, wait_mutex);
		// Logic
		int tmp_counter = logic_counter;
		logic_counter = 0;
		if (tmp_counter > 10)
			tmp_counter = 1;
		while (tmp_counter > 0) {
			next_input_event_ready = true;

			tmp_counter--;
			if (is_close_pressed()) {
				do_close();
				close_pressed = false;
			}
			if (break_main_loop) {
				goto done;
			}

			TGUIWidget *widget = tguiUpdate();
			if (widget) {
				if (widget == apply) {
					bool dup = false;
					for (int i = 0; i < num_getters; i++) {
						for (int j = i+1; j < num_getters; j++) {
							if (getters[i]->getValue() == 0) {
								continue;
							}
							if (getters[i]->getValue() == getters[j]->getValue()) {
								dup = true;
								break;
							}
						}
						if (dup) {
							break;
						}
					}
					if (dup) {
						al_set_target_backbuffer(display);
						tguiDraw();
						drawBufferToScreen();
						notify("Duplicate values", "Please correct", "");
					}
					else {
						if (type == MInputGetter::TYPE_KB) {
							config.setKey1(getters[0]->getValue());
							config.setKey2(getters[1]->getValue());
							config.setKey3(getters[2]->getValue());
							config.setKeyLeft(getters[3]->getValue());
							config.setKeyRight(getters[4]->getValue());
							config.setKeyUp(getters[5]->getValue());
							config.setKeyDown(getters[6]->getValue());
							config.setKeySettings(getters[7]->getValue());
							config.setKeyFullscreen(getters[8]->getValue());
							config.setKeySFXUp(getters[9]->getValue());
							config.setKeySFXDown(getters[10]->getValue());
							config.setKeyMusicUp(getters[11]->getValue());
							config.setKeyMusicDown(getters[12]->getValue());
							config.setKeyQuit(getters[13]->getValue());
							config.setKeySortItems(getters[14]->getValue());
						}
						else {
							config.setJoyButton1(getters[0]->getValue());
							config.setJoyButton2(getters[1]->getValue());
							config.setJoyButton3(getters[2]->getValue());
						}
						al_set_target_backbuffer(display);
						tguiDraw();
						drawBufferToScreen();
						notify("", "Done", "");
					}
				}
#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_ANDROID
				else if (widget == other) {
					if (type == MInputGetter::TYPE_KB) {
						ret = MInputGetter::TYPE_GAMEPAD;
					}
					else {
						ret = MInputGetter::TYPE_KB;
					}
					goto done;
				}
#endif
				else {
					goto done;
				}
			}

			INPUT_EVENT ie = get_next_input_event();

			if (ie.button2 == DOWN || iphone_shaken(0.1)) {
				if (ie.button2 == DOWN) {
					use_input_event();
				}
				goto done;
			}
		}

		if (draw_counter) {
			draw_counter = 0;
			al_set_target_backbuffer(display);
			tguiDraw();
			drawBufferToScreen();
			m_flip_display();
		}
	}
done:
	getting_input_config = false;

	tguiDeleteWidget(frame);

	delete frame;

	for (int i = 0; i < num_getters; i++) {
		delete getters[i];
	}

	delete apply;
#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_ANDROID
	delete other;
#endif

	tguiPop();

	dpad_on();

	return ret;
}



// this got ugly...
void doDialogue(std::string text, bool top, int rows, int offset, bool bottom)
{
	if (speechDialog)
		return;
	
	dpad_off();

	int x, y, w, h;

	try {
		Input *i = NULL;
		if (player_scripted && player) {
			i = player->getObject()->getInput();
		}
		else {
			i = getInput();
		}
	
		InputDescriptor ie = i->getDescriptor();

		if (!bottom && (!party[heroSpot] || (party[heroSpot]->getName() != "Eny"))) {
			top = true;
		}
		else {
			if (!(party[heroSpot] && party[heroSpot]->getObject())) {
				bottom = true;
			}
		}

		if (top) {
			y = offset;
		}
		else if (bottom) {
			y = BH-offset-54;
		}
		else {
			int oy = area->getFocusY() - area->getOriginY();
			if (!area->center_view && oy < BH/2+5) {
				y = BH-offset-54;
			}
			else if (!area->center_view && oy > BH/2-5) {
				y = offset;
			}
			else {
				if (ie.direction == DIRECTION_NORTH) {
					y = BH-offset-54;
				}
				else {
					y = offset;
				}
			}
		}
		x = 16;
		w = BW-TILE_SIZE*2;
		h = 10+(11*rows);
		speechDialog = new MSpeechDialog(x, y, w, h, text);
		tguiSetParent(0);
		tguiAddWidget(speechDialog);
		tguiSetFocus(speechDialog);
	}
	catch (...) {
	}
}


void MShadow::draw(void)
{
	al_draw_prim(verts, NULL, NULL, 0, 4, ALLEGRO_PRIM_TRIANGLE_STRIP);
}


void MSpeechDialog::mouseUp(int x, int y, int b)
{
	if (x >= 0 && y >= 0) {
		clicked = true;
	}
}

void MSpeechDialog::realDrawText(int section, int xo, int yo)
{
	const char *p;
	int words = 0;

	if (section == 0)
		p = text.c_str();
	else
		p = findOccurance(text.c_str(), '\n', section)+1;

	for (int i = 0; p[i]; i++) {
		if (p[i] == ' ')
			words++;
		else if (p[i] == '\n') {
			words++;
			break;
		}
	}

	std::string pS(p);
	std::stringstream ss(pS);

	int cx = xx+xo;
	int cy = yy+yo;
	const int SPACE_SIZE = 7;

	al_hold_bitmap_drawing(true);
	
	for (int i = 0; i < words; i++) {
		std::string word;
		ss >> word;
		if (word == "\\") {
			cx = xx+xo;
			cy += m_text_height(game_font)+1;
			continue;
		}
		int endx = cx + m_text_length(game_font, word.c_str());
		if (endx > xx+w-10) {
			cx = xx+xo;
			cy += m_text_height(game_font)+1;
		}
		mTextout(game_font, word.c_str(), cx, cy, white, black,
			WGT_TEXT_DROP_SHADOW, false);
		cx += m_text_length(game_font, word.c_str()) + SPACE_SIZE;
	}
	
	al_hold_bitmap_drawing(false);
}



void MSpeechDialog::drawText(void)
{
	if (scrollingSection) {
		float ratio = (float)count/SCROLL_LENGTH;
		if (ratio >= 1.0f) {
			ratio = 1.0f;
			scrollingSection = false;
		}
		int offset = (int)(h-(ratio*h));
		int xxx, yyy, www, hhh;
		al_get_clipping_rectangle(&xxx, &yyy, &www, &hhh);
		m_set_clip(xx+5, yy+3, xx+w-10, yy+h-10);
		realDrawText(currentSection-1, 10, 6-(h-offset));
		realDrawText(currentSection, 10, 6+offset);
		al_set_clipping_rectangle(xxx, yyy, www, hhh);
	}
	else {
		realDrawText(currentSection, 10, 6);
	}
}


void MSpeechDialog::draw()
{
	text = std::string(_t(untranslated_text.c_str()));
	numSections = countOccurances(text.c_str(), '\n');

	if (drawHeight < h) {
		mDrawFrame(xx, yy, w, drawHeight, true);
	}
	else {
		mDrawFrame(xx, yy, w, h, true);
		drawText();
	}
}


int MSpeechDialog::update(int millis)
{
	count += millis;

	if (!transitionInDone) {
		float ratio = (float)count/TRANSITION_IN_LENGTH;
		if (ratio >= 1.0f) {
			ratio = 1.0f;
			transitionInDone = true;
		}
		drawHeight = (int)(ratio*((h-MIN_HEIGHT)))+MIN_HEIGHT;
	}


	INPUT_EVENT ie = { 0, };
	
	ie = get_next_input_event();
	
	if (this == tguiActiveWidget && (ie.button1 == DOWN || clicked)) {
		use_input_event();
		
		Input *i = getInput();
		if (i) {
			i->waitForReleaseOr(4, 5000);
		}
		
		clicked = false;
		playPreloadedSample("select.ogg");
		currentSection++;
		if (currentSection >= numSections)
			return TGUI_RETURN;
		scrollingSection = true;
		count = 0;
	}

	return TGUI_CONTINUE;
}


bool MSpeechDialog::acceptsFocus(void)
{
	return true;
}


MSpeechDialog::MSpeechDialog(int x, int y, int width, int height,
	std::string text)
{
	this->x = 0;
	this->y = 0;
	this->width = BW;
	this->height = BH;

	xx = x;
	yy = y;
	w = width;
	h = height;
	this->hotkeys = 0;
	untranslated_text = text;
	transitionInDone = false;
	drawHeight = MIN_HEIGHT;
	count = 0;
	currentSection = 0;
	scrollingSection = false;
	numSections = countOccurances(untranslated_text.c_str(), '\n');
	clicked = false;
}


MSpeechDialog::~MSpeechDialog(void)
{
}


void MSplitFrame::pre_draw()
{
	MFrame::pre_draw();

	for (unsigned int i = 0; i < splits.size(); i++) {
		m_draw_line(x-2, y+splits[i], x+width+2, y+splits[i], black);
	}
}


int MSplitFrame::update(int millis)
{
	return TGUI_CONTINUE;
}


MSplitFrame::MSplitFrame(int x, int y, int width, int height, std::vector<int> splits) :
	MFrame(x, y, width, height)
{
	this->splits = splits;
}


MSplitFrame::~MSplitFrame(void)
{
}


void MFrame::pre_draw()
{
	mDrawFrame(x, y, width, height, shadow);
}


int MFrame::update(int millis)
{
	return TGUI_CONTINUE;
}


MFrame::MFrame(int x, int y, int width, int height, bool shadow)
{
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
	this->hotkeys = 0;
	this->shadow = shadow;
}


MFrame::~MFrame(void)
{
}


void MFrame_NormalDraw::draw()
{
	mDrawFrame(x, y, width, height, shadow);
}


int MFrame_NormalDraw::update(int millis)
{
	return TGUI_CONTINUE;
}


MFrame_NormalDraw::MFrame_NormalDraw(int x, int y, int width, int height, bool shadow)
{
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
	this->hotkeys = 0;
	this->shadow = shadow;
}


MFrame_NormalDraw::~MFrame_NormalDraw(void)
{
}


void MLabel::draw()
{
	mTextout(game_font, _t(text), x, y,
		color, black,
		WGT_TEXT_DROP_SHADOW, false);
}


int MLabel::update(int millis)
{
	return TGUI_CONTINUE;
}


MLabel::MLabel(int x, int y, std::string text, MCOLOR color)
{
	this->x = x;
	this->y = y;
	strcpy(this->text, text.c_str());
	this->width = m_text_length(game_font, _t(this->text));
	this->height = m_text_height(game_font);
	this->hotkeys = 0;
	this->color = color;
}


MLabel::~MLabel(void)
{
}


void MTextButton::mouseUp(int x, int y, int b)
{
	if (disabled)
		return;

	if (x >= 0 && y >= 0) {
		clicked = true;
	}
}


void MTextButton::draw()
{
	if (disabled) {
		mTextout(game_font, _t(text), x+1, y,
			m_map_rgb(110, 100, 130), black,
			WGT_TEXT_NORMAL, false);
		return;
	}

	MCOLOR color;

	// Draw cursor
	if (this == tguiActiveWidget) {
		int tick = (unsigned)tguiCurrentTimeMillis() % 1000;
		if (tick < 800) {
			m_draw_bitmap(cursor, x, y-1, 0);
		}
	}

	// draw text

	if (this == tguiActiveWidget) {
		color = white;
	}
	else {
		color = grey;
	}

	mTextout(game_font, _t(text), x+m_get_bitmap_width(cursor)+1, y,
		color, black,
		shadow_type, false);
}


int MTextButton::update(int millis)
{
	width = m_text_length(game_font, _t(text)) +
		m_text_height(game_font) + 2;

	INPUT_EVENT ie;
	if (this != tguiActiveWidget) {
		ie = EMPTY_INPUT_EVENT;
	}
	else {
		ie = get_next_input_event();
	}

	if ((!disabled && (ie.button1 == DOWN)) || clicked) {
		use_input_event();
		clicked = false;
		playPreloadedSample("select.ogg");
		return TGUI_RETURN;
	}
	else if (ie.left == DOWN) {
		use_input_event();
		playPreloadedSample("blip.ogg");
		if (left_widget)
			tguiSetFocus(left_widget);
		else
			tguiFocusPrevious();
	}
	else if (ie.up == DOWN) {
		use_input_event();
		playPreloadedSample("blip.ogg");
		tguiFocusPrevious();
	}
	else if (ie.right == DOWN) {
		use_input_event();
		playPreloadedSample("blip.ogg");
		if (right_widget)
			tguiSetFocus(right_widget);
		else
			tguiFocusNext();
	}
	else if (ie.down == DOWN) {
		use_input_event();
		playPreloadedSample("blip.ogg");
		tguiFocusNext();
	}

	return TGUI_CONTINUE;
}


bool MTextButton::acceptsFocus()
{
	return disabled == false;
}



MTextButton::MTextButton(int x, int y, std::string text, bool disabled, TGUIWidget *left_widget, TGUIWidget *right_widget)
{
	this->x = x;
	this->y = y;
	strcpy(this->text, text.c_str());
	this->width = m_text_length(game_font, _t(this->text)) +
		m_text_height(game_font) + 2;
	this->height = m_text_height(game_font);
	this->hotkeys = 0;
	clicked = false;

	shadow_type = WGT_TEXT_DROP_SHADOW;

	this->disabled = disabled;

	this->left_widget = left_widget;
	this->right_widget = right_widget;
}


MTextButton::~MTextButton(void)
{
}


int MInputGetter::getValue()
{
	return value;
}

void MInputGetter::setValue(int v)
{
	value = v;
}

void MInputGetter::draw()
{
	MCOLOR color;

	// Draw cursor
	if (this == tguiActiveWidget) {
		int tick = (unsigned)tguiCurrentTimeMillis() % 1000;
		if (tick < 800) {
			m_draw_bitmap(cursor, x, y, 0);
		}
	}

	// draw text

	if (mode == GETTING) {
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
		if (type == TYPE_KB) {
			al_draw_textf(game_font, al_map_rgb_f(0, 1, 0), x+(width-(m_get_bitmap_width(cursor)+1))/2+m_get_bitmap_width(cursor)+1, y-2, ALLEGRO_ALIGN_CENTRE, "%s", _t("Press key or tap to cancel"));
		}
		else {
			al_draw_textf(game_font, al_map_rgb_f(0, 1, 0), x+(width-(m_get_bitmap_width(cursor)+1))/2+m_get_bitmap_width(cursor)+1, y-2, ALLEGRO_ALIGN_CENTRE, "%s", _t("Press button or tap to cancel"));
		}
#else
		if (type == TYPE_KB) {
			al_draw_textf(game_font, al_map_rgb_f(0, 1, 0), x+(width-(m_get_bitmap_width(cursor)+1))/2+m_get_bitmap_width(cursor)+1, y-2, ALLEGRO_ALIGN_CENTRE, "%s", _t("Press key or click to cancel"));
		}
		else {
			al_draw_textf(game_font, al_map_rgb_f(0, 1, 0), x+(width-(m_get_bitmap_width(cursor)+1))/2+m_get_bitmap_width(cursor)+1, y-2, ALLEGRO_ALIGN_CENTRE, "%s", _t("Press button or click to cancel"));
		}
#endif
	}
	else {
		if (this == tguiActiveWidget) {
			color = white;
		}
		else {
			color = grey;
		}

		mTextout(game_font, _t(text.c_str()), x+m_get_bitmap_width(cursor)+1, y,
			color, black,
			WGT_TEXT_DROP_SHADOW, false);

		al_draw_textf(game_font, color, x+width, y-2, ALLEGRO_ALIGN_RIGHT, "%s",
			type == TYPE_KB ? keycode_to_keyname(value) : my_itoa(value));
	}
}

int MInputGetter::update(int millis)
{
	if (this == tguiActiveWidget) {
		if (mode == GETTING) {
			if (!released_b1) {
				InputDescriptor id = getInput()->getDescriptor();
				if (!id.button1) {
					released_b1 = true;
				}
			}
			else {
				ALLEGRO_KEYBOARD_STATE state;
#ifdef ALLEGRO_IPHONE
				memcpy(&state, &icade_keyboard_state, sizeof state);
#else
				al_get_keyboard_state(&state);
#endif
				if (type == TYPE_KB) {
					for (int i = 0; i < ALLEGRO_KEY_MAX; i++) {
						if (al_key_down(&state, i)) {
							if (i == ALLEGRO_KEY_BACKSPACE) {
								value = start_value;
							}
							else {
								value = i;
							}
							mode = NORMAL;
							getting_input_config = false;
							do {
#ifdef ALLEGRO_IPHONE
				memcpy(&state, &icade_keyboard_state, sizeof state);
#else
				al_get_keyboard_state(&state);
#endif
							} while (al_key_down(&state, value));
							clear_input_events();
							break;
						}
					}
				}
				else {
					if (al_key_down(&state, ALLEGRO_KEY_BACKSPACE)) {
						value = start_value;
						mode = NORMAL;
						getting_input_config = false;
					}
					else {
						if (al_is_joystick_installed() && user_joystick) {
							int nb = al_get_joystick_num_buttons(user_joystick);
							ALLEGRO_JOYSTICK_STATE state;
							al_get_joystick_state(user_joystick, &state);
							for (int i = 0; i < nb; i++) {
								if (state.button[i]) {
									value = i;
									mode = NORMAL;
									getting_input_config = false;
									do {
										al_get_joystick_state(user_joystick, &state);
									} while (state.button[i]);
									clear_input_events();
									break;
								}
							}
						}
						else {
							mode = NORMAL;
							getting_input_config = false;
						}
					}
				}
			}
		}
		else {
			INPUT_EVENT ie;
			ie = get_next_input_event();

			if (ie.left == DOWN) {
				use_input_event();
				playPreloadedSample("blip.ogg");
				tguiFocusPrevious();
			}
			else if (ie.up == DOWN) {
				use_input_event();
				playPreloadedSample("blip.ogg");
				tguiFocusPrevious();
			}
			else if (ie.right == DOWN) {
				use_input_event();
				playPreloadedSample("blip.ogg");
				tguiFocusNext();
			}
			else if (ie.down == DOWN) {
				use_input_event();
				playPreloadedSample("blip.ogg");
				tguiFocusNext();
			}
			else if (ie.button1 == DOWN) {
				use_input_event();
				playPreloadedSample("select.ogg");
				getting_input_config = true;
				mode = GETTING;
				released_b1 = false;
			}
		}
	}

	return TGUI_CONTINUE;
}


bool MInputGetter::acceptsFocus()
{
	return true;
}

void MInputGetter::mouseDown(int mx, int my, int mb)
{
	mode = NORMAL;
}

MInputGetter::MInputGetter(int type, int x, int y, int w, std::string text, int start_value)
{
	this->x = x;
	this->y = y;
	this->text = text;
	this->width = w;
	this->height = m_text_height(game_font);
	this->hotkeys = 0;
	this->type = type;
	this->value = start_value;
	this->start_value = start_value;
	this->mode = NORMAL;
}

MInputGetter::~MInputGetter(void)
{
}


MTextButtonFullShadow::MTextButtonFullShadow(int x, int y, std::string text) :
	MTextButton(x, y, text, false)
{
	shadow_type = WGT_TEXT_SQUARE_BORDER;
}


MTextButtonFullShadow::~MTextButtonFullShadow(void)
{
}



void MToggle::draw()
{
	MCOLOR color;

	if (tguiActiveWidget == this)
		color = m_map_rgb(255, 255, 0);
	else
		color = m_map_rgb(220, 220, 220);

	mTextout(game_font, _t(options[selected].c_str()), x, y,
		color, black, WGT_TEXT_NORMAL, false);
}


int MToggle::update(int millis)
{
	if (tguiActiveWidget == this) {
		INPUT_EVENT ie = get_next_input_event();

		if (ie.up == DOWN) {
			use_input_event();
			playPreloadedSample("blip.ogg");
			tguiFocusPrevious();
		}
		else if (ie.down == DOWN) {
			use_input_event();
			playPreloadedSample("blip.ogg");
			tguiFocusNext();
		}
		else if (ie.left == DOWN) {
			use_input_event();
			if (selected > 0) {
				playPreloadedSample("blip.ogg");
				selected--;
			}
		}
		else if (ie.right == DOWN) {
			use_input_event();
			if ((int)selected < (int)(options.size()-1)) {
				playPreloadedSample("blip.ogg");
				selected++;
			}
		}
	}

	return TGUI_CONTINUE;
}


bool MToggle::acceptsFocus()
{
	return true;
}


int MToggle::getSelected(void)
{
	return selected;
}

void MToggle::setSelected(int s)
{
	selected = s;
}



MToggle::MToggle(int x, int y, std::vector<std::string>& options) :
	options(options)
{
	this->x = x;
	this->y = y;
	this->width = 1;
	this->height = 1;
	this->hotkeys = 0;

	selected = 0;
}


MToggle::~MToggle(void)
{
}

void MSlider::draw()
{
	MCOLOR color;

	if (this == tguiActiveWidget) {
		color = m_map_rgb(255, 216, 0);
	}
	else {
		color = m_map_rgb(220, 220, 220);
	}

	m_draw_line(x, y, x+100, y, color);

	int rx = x+value;

	m_draw_rectangle(rx-2, y-4, rx+2, y+4, color, M_FILLED);
}


int MSlider::update(int millis)
{
	if (tguiActiveWidget == this) {
		INPUT_EVENT ie = get_next_input_event();

		if (ie.up == DOWN) {
			use_input_event();
			playPreloadedSample("blip.ogg");
			tguiFocusPrevious();
		}
		else if (ie.down == DOWN) {
			use_input_event();
			playPreloadedSample("blip.ogg");
			tguiFocusNext();
		}
		else if (ie.left == DOWN) {
			use_input_event();
			if (value > 0) {
				playPreloadedSample("blip.ogg");
				value--;
			}
		}
		else if (ie.right == DOWN) {
			use_input_event();
			if ((int)value < 100) {
				playPreloadedSample("blip.ogg");
				value++;
			}
		}
	}

	return TGUI_CONTINUE;
}


bool MSlider::acceptsFocus()
{
	return true;
}


int MSlider::getValue(void)
{
	return value;
}

void MSlider::setValue(int v)
{
	value = v;
}



MSlider::MSlider(int x, int y)
{
	this->x = x;
	this->y = y;
	this->width = 1;
	this->height = 1;
	this->hotkeys = 0;

	value = 0;
}


MSlider::~MSlider(void)
{
}


void MCheckbox::draw()
{
	MCOLOR color;

	if (tguiActiveWidget == this)
		color = m_map_rgb(255, 216, 0);
	else
		color = m_map_rgb(220, 220, 220);

	mTextout(game_font, _t(text.c_str()), x, y,
		color, black, WGT_TEXT_NORMAL, false);
}


int MCheckbox::update(int millis)
{
	if (tguiActiveWidget == this) {
		INPUT_EVENT ie = get_next_input_event();

		if (ie.up == DOWN || ie.left == DOWN) {
			use_input_event();
			playPreloadedSample("blip.ogg");
			tguiFocusPrevious();
		}
		else if (ie.down == DOWN || ie.right == DOWN) {
			use_input_event();
			playPreloadedSample("blip.ogg");
			tguiFocusNext();
		}
		else if (ie.button1 == DOWN) {
			use_input_event();
			playPreloadedSample("blip.ogg");
			setChecked(!checked);
		}
	}

	return TGUI_CONTINUE;
}


bool MCheckbox::acceptsFocus()
{
	return true;
}


bool MCheckbox::getChecked(void)
{
	return checked;
}

void MCheckbox::setChecked(bool c)
{
	if (c) {
		text = "{029} " + text.substr(6);
	}
	else {
		text = "{028} " + text.substr(6);
	}
	checked = c;
}



MCheckbox::MCheckbox(int x, int y, std::string text)
{
	this->x = x;
	this->y = y;
	this->width = 1;
	this->height = 1;
	this->hotkeys = 0;

	this->text = "{028} " + text;

	checked = false;
}


MCheckbox::~MCheckbox(void)
{
}



void MLevelUpHeader::draw()
{
	mDrawFrame(3, 3, BW-6, 12);
	mDrawFrame(3, 19, BW-6, 12);

	mTextout_simple(bonus ? _t("Bonus") : _t("Level Up"), 6, 4, grey);

	char p[20];
	sprintf(p, _t("%d points"), *points);
	int dx = BW-6-m_text_length(game_font, _t(p));
	mTextout_simple(p, dx, 4, grey);

	if (!bonus)
		mTextout_simple(spellCaption.c_str(), 6, 20, grey);
}


int MLevelUpHeader::update(int millis)
{
	return TGUI_CONTINUE;
}


MLevelUpHeader::MLevelUpHeader(int *points, std::string spellCaption, bool bonus)
{
	this->x = 0;
	this->y = 0;
	this->width = BW;
	this->height = 20; // close enough?
	this->hotkeys = 0;
	this->points = points;
	this->spellCaption = spellCaption;
	this->bonus = bonus;
}


MLevelUpHeader::~MLevelUpHeader(void)
{
}


void MRectangle::pre_draw()
{
	m_draw_rectangle(x, y, width, height, color, flags);
}


int MRectangle::update(int millis)
{
	return TGUI_CONTINUE;
}


MRectangle::MRectangle(int x, int y, int width, int height,
	MCOLOR color, int flags)
{
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
	this->hotkeys = 0;
	this->color = color;
	this->flags = flags;
}


MRectangle::~MRectangle(void)
{
}


void MStats::mouseUp(int x, int y, int b)
{
	if (x >= 0 && y >= 0) {
		if (x < 20) {
			go_left = true;
		}
		else if (x > (BW-20)) {
			go_right = true;
		}
	}
}


void MStats::draw()
{
	mDrawFrame(x, y, width, height);

	Player *p = party[who];
	CombatantInfo &info = p->getInfo();

	MBITMAP *icon = p->getIcon();
	if (info.condition == CONDITION_POISONED) {
		al_draw_filled_rectangle(x+1, y+1, x+3+al_get_bitmap_width(icon->bitmap), y+3+al_get_bitmap_height(icon->bitmap), al_map_rgb_f(1.0f, 0.0f, 1.0f));
	}
	m_draw_bitmap(icon, x+2, y+2, 0);

	char text[100];

	strcpy(text, _t("Exp:"));
	mTextout_simple(text, 40, y+3, grey);

	sprintf(text, "%d", info.experience);
	mTextout_simple(text, 76, y+3, grey);

	strcpy(text, _t("Need:"));
	mTextout_simple(text, 40, y+3+m_text_height(game_font), grey);

	int level = getLevel(info.experience);
	int next = getExperience(level+1) - info.experience;
	sprintf(text, "%d", next);
	mTextout_simple(text, 76, y+3+m_text_height(game_font), grey);

	if (info.abilities.hp <= 0) {
		strcpy(text, _t("Unconscious"));
		mTextout_simple(text, 40, y+3+m_text_height(game_font)*2, grey);
	}
	else {
		sprintf(text, "%s", getConditionName(info.condition).c_str());
		if (std::string(text) == "")
			strcpy(text, "Normal");
		mTextout_simple(_t(text), 40, y+3+m_text_height(game_font)*2, grey);
	}

	mTextout_simple(_t("HP:"), x+2, y+38, grey);
	sprintf(text, "%d/%d", info.abilities.hp >= 0 ? info.abilities.hp : 0, info.abilities.maxhp);
	mTextout_simple(text, x+50, y+38, grey);
	mTextout_simple(_t("MP:"), x+2, y+50, grey);
	sprintf(text, "%d/%d", info.abilities.mp, info.abilities.maxmp);
	mTextout_simple(text, x+50, y+50, grey);

	std::string equip_labels[] = {
		"LHand:",
		"RHand:",
		"HArmor:",
		"CArmor:",
		"FArmor:"
	};

	for (int i = 0; i < 5; i++) {
		int _y = y+38+((i+2)*12);
		mTextout_simple(_t(equip_labels[i].c_str()), x+2, _y, grey);
		int index = ((int *)&info.equipment)[i];
		if (index < 0) {
			strcpy(text, "0");
		}
		else {
			int n;
			if (i < 2) {
				n = weaponDamage(items[index].id);
			}
			else {
				n = armorDefense((ItemType)(ITEM_TYPE_HEAD_ARMOR+(i-2)), items[index].id);
			}
			sprintf(text, "%d", n);
		}
		mTextout_simple(text, x+50, _y, grey);
	}
	
	std::string ability_labels[] = {
		"<unused>",
		"MaxHP:",
		"Attack:",
		"Defense:",
		"Speed:",
		"<unused>",
		"MaxMP:",
		"M.Def.:",
		"Luck:"
	};

	int yy = y+6;

	for (int i = 0; i < 9; i++) {
		if (ability_labels[i] == "<unused>") continue;
		mTextout_simple(_t(ability_labels[i].c_str()), x+width/2+5, yy, grey);
		int n = ((int *)&info.abilities)[i];
		if (i == 0 && n < 0)
			sprintf(text, "0");
		else
			sprintf(text, "%d", n);
		mTextout_simple(text, x+width/2+70, yy, grey);
		yy += 12;
	}


	if (can_change) {
		if ((unsigned)tguiCurrentTimeMillis() % 1000 < 500) {
			m_draw_bitmap(arrow, 0, y+20, 0);
			m_draw_bitmap(arrow, BW-m_get_bitmap_width(arrow), y+20, 
				M_FLIP_HORIZONTAL);
		}
	}
}

void MStats::next(void)
{
	do {
		who++;
		if (who >= MAX_PARTY)
			who = 0;
	} while (!party[who]);
}

int MStats::update(int millis)
{
	if (this == tguiActiveWidget) {
		INPUT_EVENT ie = get_next_input_event();
		if (ie.button1 == DOWN || ie.button2 == DOWN || iphone_shaken(0.1)) {
			use_input_event();
			iphone_clear_shaken();
			playPreloadedSample("select.ogg");
			return TGUI_RETURN;
		}
		else if (can_change && (ie.left == DOWN || go_left)) {
			use_input_event();
			go_left = false;
			playPreloadedSample("blip.ogg");
			do {
				who--;
				if (who < 0)
					who = MAX_PARTY-1;
			} while (!party[who]);
		}
		else if (can_change && (ie.right == DOWN || go_right)) {
			use_input_event();
			go_right = false;
			playPreloadedSample("blip.ogg");
			next();
		}
	}

	return TGUI_CONTINUE;
}


MStats::MStats(int y, int height, int who, bool can_change)
{
	this->x = 3;
	this->y = y;
	this->width = BW-6;
	this->height = height;
	this->hotkeys = 0;
	this->who = who;
	this->can_change = can_change;
	arrow = m_load_bitmap(getResource("media/arrow.png"));
	go_left = false;
	go_right = false;

	if (!party[who])
		next();
}


MStats::~MStats(void)
{
	m_destroy_bitmap(arrow);
}


static void drawSimpleEquipment(int partyMember, int x, int y, int height)
{
	char text[100];
	CombatantInfo &info = party[partyMember]->getInfo();
	int extra = (height-56)/4.0;

	sprintf(text, "%s%s", getItemIcon(info.equipment.lhand).c_str(), _t(getItemName(info.equipment.lhand).c_str()));
	mTextout(game_font, text, x, y,
		grey, black,
		WGT_TEXT_DROP_SHADOW, false);
	if (info.equipment.lhand >= 0 && weapons[items[info.equipment.lhand].id].ammo) {
		int o = 2 + m_text_length(game_font, text);
		sprintf(text, "(%d)", info.equipment.lquantity);
		mTextout(game_font, text,
			x+o,
			y, grey, black,
			WGT_TEXT_DROP_SHADOW, false);
	}
	sprintf(text, "%s%s", getItemIcon(info.equipment.rhand).c_str(), _t(getItemName(info.equipment.rhand).c_str()));
	mTextout(game_font, text, x, y+m_text_height(game_font)+extra,
		grey, black,
		WGT_TEXT_DROP_SHADOW, false);
	if (info.equipment.rhand >= 0 && weapons[items[info.equipment.rhand].id].ammo) {
		int o = 2 + m_text_length(game_font, text);
		sprintf(text, "(%d)", info.equipment.rquantity);
		mTextout(game_font, text,
			x+o,
			y+m_text_height(game_font)+extra,
			grey, black,
			WGT_TEXT_DROP_SHADOW, false);
	}
	sprintf(text, "%s%s", getItemIcon(info.equipment.harmor).c_str(), _t(getItemName(info.equipment.harmor).c_str()));
	mTextout(game_font, text, x, y+m_text_height(game_font)*2+extra*2,
		grey, black,
		WGT_TEXT_DROP_SHADOW, false);
	sprintf(text, "%s%s", getItemIcon(info.equipment.carmor).c_str(), _t(getItemName(info.equipment.carmor).c_str()));
	mTextout(game_font, text, x, y+m_text_height(game_font)*3+extra*3,
		grey, black,
		WGT_TEXT_DROP_SHADOW, false);
	sprintf(text, "%s%s", getItemIcon(info.equipment.farmor).c_str(), _t(getItemName(info.equipment.farmor).c_str()));
	mTextout(game_font, text, x, y+m_text_height(game_font)*4+extra*4,
		grey, black,
		WGT_TEXT_DROP_SHADOW, false);
}

void MMap::flash(void) { shouldFlash = true; }

void MMap::mouseDown(int xx, int yy, int b)
{
	mouse_down = true;
	ALLEGRO_MOUSE_STATE s;
	m_get_mouse_state(&s);
	downX = s.x;
	downY = s.y;
}


void MMap::mouseUp(int x, int y, int b)
{
	mouse_down = false;

	if (x >= 0 && y >= 0 && !transitioning) {
		MapPoint *p = &points[selected];
		int px = p->x-top_x-offset_x;
		int py = p->y-top_y-offset_y;
		int dx = x - px;
		int dy = y - py;
		if (sqrtf(dx*dx + dy*dy) < 10) {
			clicked = true;
			return;
		}
		// test surrounding points
		for (int i = 0; i < 4; i++) {
			p = points[selected].links[i];
			if (!p)
				continue;
			int px = p->x-top_x-offset_x;
			int py = p->y-top_y-offset_y;
			int dx = x - px;
			int dy = y - py;
			if (sqrtf(dx*dx + dy*dy) < 10) {
				switch (i) {
					case DIRECTION_NORTH:
						u_pressed = true;
						break;
					case DIRECTION_EAST:
						r_pressed = true;
						break;
					case DIRECTION_SOUTH:
						d_pressed = true;
						break;
					default:
						l_pressed = true;
						break;
				}
				return;
			}
		}
	}
}

static int crap_map_quadrant_global_count = 0;

void MMap::draw()
{
	m_set_blender(M_ONE, M_ZERO, white);

	if (transitioning) {
		m_draw_bitmap_region(map_bmp,
			top_x+offset_x, top_y+offset_y, BW, BH,
			0, 0, 0);
		m_set_blender(M_ONE, M_INVERSE_ALPHA, white);
	}
	else {
		m_draw_bitmap(map_bmp, -top_x-offset_x, -top_y-offset_y, 0);
		m_set_blender(M_ONE, M_INVERSE_ALPHA, white);

		MapPoint *p = &points[selected];
		MBITMAP *place_icon = bitmaps[points[selected].internal_name];
		m_draw_bitmap(place_icon, p->x-top_x-offset_x-m_get_bitmap_width(place_icon)/2,
			p->y-top_y-offset_y-m_get_bitmap_height(place_icon)/2, 0);

		// draw surrounding points
		for (int i = 0; i < 4; i++) {
			MapPoint *p2 = points[selected].links[i];
			if (!p2)
				continue;
			place_icon = bitmaps[points[selected].links[i]->internal_name];
			m_draw_bitmap(place_icon,
				p2->x-top_x-offset_x-m_get_bitmap_width(place_icon)/2,
				p2->y-top_y-offset_y-m_get_bitmap_height(place_icon)/2, 0);
			if ((unsigned)tguiCurrentTimeMillis() % 600 < 400) {
				BmpAndPos *b = findDots(p->internal_name,
					p2->internal_name);
				if (b) {
					m_draw_bitmap(b->bitmap,
						b->x-top_x-offset_x, b->y-top_y-offset_y, 0);
				}
			}
		}

		if (!transitioning) {
			// draw arrow

			int ay = ((unsigned)tguiCurrentTimeMillis() % 500) / 50;

			p = &points[selected];
			m_draw_bitmap(down_arrow,
				p->x-top_x-offset_x-m_get_bitmap_width(down_arrow)/2,
				p->y-top_y-offset_y-m_get_bitmap_height(down_arrow)-ay, 0);
		}

		mTextout(game_font, _t(points[selected].display_name.c_str()), 5, BH-m_text_height(game_font)-5, m_map_rgb(255, 255, 0), black,
			WGT_TEXT_BORDER, false);
	}

}

int MMap::update(int millis)
{
	crap_map_quadrant_global_count += millis;

	if (this != tguiActiveWidget) {
		return TGUI_CONTINUE;
	}

	if (transitioning) {
		offset_x = offset_y = 0;
		count += millis;
		if (count > 1000) {
			transitioning = false;
			top_x = destx;
			top_y = desty;
		}
		else {
			percent_moved = count / 1000.0f;
			top_x = ((destx - startx) * percent_moved) + startx;
			top_y = ((desty - starty) * percent_moved) + starty;
		}
		return TGUI_CONTINUE;
	}

	if (mouse_down) {
		ALLEGRO_MOUSE_STATE s;
		m_get_mouse_state(&s);
		int dx = s.x - downX;
		int dy = s.y - downY;
		downX = s.x;
		downY = s.y;
		offset_x -= dx;
		offset_y -= dy;
	}

	if (top_x+offset_x < 0) {
		offset_x -= top_x+offset_x;
	}
	else if (top_x+offset_x >= m_get_bitmap_width(map_bmp)-BW) {
		offset_x = m_get_bitmap_width(map_bmp)-BW-top_x;
	}
	if (top_y+offset_y < 0) {
		offset_y -= top_y+offset_y;
	}
	else if (top_y+offset_y >= m_get_bitmap_height(map_bmp)-BH) {
		offset_y = m_get_bitmap_height(map_bmp)-BH-top_y;
	}

	INPUT_EVENT ie = get_next_input_event();

	MapPoint *newSelected = NULL;

	if ((ie.left == DOWN || l_pressed) && points[selected].links[(int)DIRECTION_WEST]) {
		use_input_event();
		l_pressed = false;
		newSelected = points[selected].links[(int)DIRECTION_WEST];
	}
	else if ((ie.right == DOWN || r_pressed) && points[selected].links[(int)DIRECTION_EAST]) {
		use_input_event();
		r_pressed = false;
		newSelected = points[selected].links[(int)DIRECTION_EAST];
	}
	else if ((ie.up == DOWN || u_pressed) && points[selected].links[(int)DIRECTION_NORTH]) {
		use_input_event();
		u_pressed = false;
		newSelected = points[selected].links[(int)DIRECTION_NORTH];
	}
	else if ((ie.down == DOWN || d_pressed) && points[selected].links[(int)DIRECTION_SOUTH]) {
		use_input_event();
		d_pressed = false;
		newSelected = points[selected].links[(int)DIRECTION_SOUTH];
	}

	if (newSelected) {
		for (int i = 0; i < (int)points.size(); i++) {
			if (&points[i] == newSelected) {
				selected = i;
			}
		}
		transitioning = true;
		startx = top_x+offset_x;
		starty = top_y+offset_y;
		int tmpx, tmpy;
		getIdealPoint(points[selected].x, points[selected].y,
			&tmpx, &tmpy);
		destx = tmpx;
		desty = tmpy;
		percent_moved = 0;
		count = 0;
	}

	if (shouldFlash) {
		if (ie.button1 == DOWN || clicked) {
			use_input_event();
			clicked = false;
			playPreloadedSample("select.ogg");
			return TGUI_RETURN;
		}
	}

	if (ie.button1 == DOWN || clicked) {
		use_input_event();
		clicked = false;
		playPreloadedSample("select.ogg");
		main_draw();
		fadeOut(black);

		/* Scroll in a portrait of the keep if entering it */
		if (points[selected].dest_area == "Keep_outer") {
			playMusic("keep.ogg");
			MBITMAP *bmp = m_load_bitmap(getResource("media/keep.png"));
			m_save_blender();
			int w = m_get_bitmap_width(bmp);
			int h = m_get_bitmap_height(bmp);
			const int start_x = (w-BW)/2;
			const int start_y = 0;
			//int target_x = 0;
			int target_y = h-BH*2;
			m_draw_bitmap(bmp, -start_x, start_y, 0);
			const int TOTAL = 5000;
			long start = tguiCurrentTimeMillis();
			long end = start + TOTAL;
			long now = start;
			int elapsed = 0;
			int totalElapsed = 0;
			dpad_off();
		
			while (true) {
				if (is_close_pressed()) {
					do_close();
					close_pressed = false;
				}
				if (break_main_loop) {
					break;
				}
				float p = (float)MIN(totalElapsed, TOTAL) / TOTAL;
				int curr_w = p * w/2 + w/2;
				int curr_h = p * BH + BH;
				int curr_x = start_x - (p * start_x);
				int curr_y = p * target_y;

				al_set_target_backbuffer(display);
				/*
				 * For some unknown reason m_draw_scaled_bitmap
				 * sets an alpha blender, and I don't want to
				 * find out why.
				 */
				m_set_blender(M_ONE, M_ZERO, white);
				m_draw_scaled_bitmap(bmp, curr_x, curr_y, curr_w, curr_h, 0, 0, BW, BH, 0);
				m_set_blender(M_ONE, M_INVERSE_ALPHA, white);
				drawBufferToScreen();
				if  (now >= end) {
					break;
				}
				m_flip_display();

				now = tguiCurrentTimeMillis();
				elapsed = now-start;
				start = now;
				totalElapsed += elapsed;
			}
			dpad_on();
			m_restore_blender();
			m_destroy_bitmap(bmp);
			m_rest(2);
			fadeOut(black);
		}


		Object *o = party[heroSpot]->getObject();
		o->setPosition(points[selected].dest_x, points[selected].dest_y);
		setObjectDirection(o, points[selected].dest_dir);
		startArea(points[selected].dest_area);
		ALLEGRO_BITMAP *oldTarget = al_get_target_bitmap();
		al_set_target_backbuffer(display);
		m_clear(black);
		al_set_target_bitmap(oldTarget);

		waitForRelease(4);
		clear_input_events();

		area->update(1);
		al_set_target_backbuffer(display);
		area->draw();
		drawBufferToScreen();
		transitionIn();
		return TGUI_RETURN;
	}
	if (!shouldFlash && (ie.button2 == DOWN || iphone_shaken(0.1))) {
		use_input_event();
		iphone_clear_shaken();
		//playPreloadedSample("select.ogg");
		bool ret = pause(true, true, points[selected].internal_name);
		runtime_start = tguiCurrentTimeMillis();
		if (!ret) {
			//quit_game = true;
			break_main_loop = true;
			return TGUI_RETURN;
		}
	}


	return TGUI_CONTINUE;
}


std::string MMap::getSelected(void)
{
	return points[selected].internal_name;
}


void MMap::setSelected(std::string s)
{
	for (selected = 0; selected < (int)points.size(); selected++) {
		if (points[selected].internal_name == s)
			break;
	}
	debug_message("selected > num\n");
}


bool MMap::acceptsFocus(void)
{
	return true;
}

void MMap::getLines(MapPoint *p)
{
	if (p->mapped)
		return;
	
	p->mapped = true;
	
	for (int i = 0; i < 4; i++) {
		if (p->links[i] && !p->links[i]->mapped) {
			BmpAndPos b;
			b.point1_name = p->internal_name;
			b.point2_name = p->links[i]->internal_name;
			XMLData *dat;
			b.bitmap = m_load_bitmap(getResource("media/dots-%s_to_%s.png", 
				b.point1_name.c_str(), b.point2_name.c_str()), false, true);
			if (!b.bitmap) {
				b.bitmap = m_load_bitmap(getResource("media/dots-%s_to_%s.png", 
					b.point2_name.c_str(), b.point1_name.c_str()));
				dat = new XMLData(getResource("media/dots-%s_to_%s.png.xml",
					b.point2_name.c_str(), b.point1_name.c_str()));
			}
			else {
				dat = new XMLData(getResource("media/dots-%s_to_%s.png.xml",
					b.point1_name.c_str(), b.point2_name.c_str()));
			}
			XMLData *xnode = dat->find("x");
			b.x = atoi(xnode->getValue().c_str());
			XMLData *ynode = dat->find("y");
			b.y = atoi(ynode->getValue().c_str());
			lines.push_back(b);
			getLines(p->links[i]);
			delete dat;
		}
	}
}


MapPoint *MMap::findPoint(std::string name)
{
	for (int i = 0; i < (int)points.size(); i++) {
		if (points[i].internal_name == name)
			return &points[i];
	}

	return NULL;
}


BmpAndPos *MMap::findDots(std::string s1, std::string s2)
{
	for (int i = 0; i < (int)lines.size(); i++) {
		if ((s1 == lines[i].point1_name && s2 == lines[i].point2_name)
			||
			(s2 == lines[i].point1_name && s1 == lines[i].point2_name)) {
			return &lines[i];
		}
	}

	return NULL;
}


void MMap::load_map_data(void)
{
	lua_State *luaState = lua_open();

	openLuaLibs(luaState);

	registerCFunctions(luaState);

	runGlobalScript(luaState);

	unsigned char *bytes;
	int file_size;

	debug_message("Loading global script...\n");
	bytes = slurp_file(getResource("scripts/global.%s", getScriptExtension().c_str()), &file_size);
	if (!bytes) native_error("Load error.", "scripts/global.lua.");
	if (luaL_loadbuffer(luaState, (char *)bytes, file_size, "chunk")) {
		dumpLuaStack(luaState);
		throw ReadError();
	}
	delete[] bytes;

	debug_message("Running global script...\n");
	if (lua_pcall(luaState, 0, 0, 0)) {
		dumpLuaStack(luaState);
		lua_close(luaState);
		throw ScriptError();
	}

	// Make the previous area name available
	lua_pushstring(luaState, prev.c_str());
	lua_setglobal(luaState, "prev");

	debug_message("Loading map script...\n");
	bytes = slurp_file(getResource("%s.%s", prefix.c_str(), getScriptExtension().c_str()), &file_size);
	if (!bytes) native_error("Load error.", (prefix + ".lua").c_str());
	if (luaL_loadbuffer(luaState, (char *)bytes, file_size, "chunk")) {
		dumpLuaStack(luaState);
		throw ReadError();
	}
	delete[] bytes;

	debug_message("Running map script...\n");
	if (lua_pcall(luaState, 0, 0, 0)) {
		dumpLuaStack(luaState);
		lua_close(luaState);
		throw ScriptError();
	}

	lua_getglobal(luaState, prev.c_str());
	lua_setglobal(luaState, "prev_table");

	callLua(luaState, "post_cfg", ">");


	lua_getglobal(luaState, "num_areas");
	int num_areas = (int)lua_tonumber(luaState, -1);
	lua_pop(luaState, 1);

	for (int i = 0; i < num_areas; i++) {
		MapPoint p;
		callLua(luaState, "get_area_table_name", "i>s", i+1);
		const char *areaname = lua_tostring(luaState, -1);
		p.internal_name = std::string(areaname);
		if (std::string(areaname) == "keep")
			bitmaps[std::string(areaname)] = m_load_bitmap(getResource(
				"media/keep_icon.png"));
		else
			bitmaps[std::string(areaname)] = m_load_bitmap(getResource(
				"media/%s.png", areaname));
		lua_pop(luaState, 1);
		lua_getglobal(luaState, areaname);
		
		lua_pushstring(luaState, "name");
		lua_gettable(luaState, -2);
		p.display_name = std::string(lua_tostring(luaState, -1));
		lua_pop(luaState, 1);

		lua_pushstring(luaState, "x");
		lua_gettable(luaState, -2);
		p.x = (int)lua_tonumber(luaState, -1);
		lua_pop(luaState, 1);

		lua_pushstring(luaState, "y");
		lua_gettable(luaState, -2);
		p.y = (int)lua_tonumber(luaState, -1);
		lua_pop(luaState, 1);

		lua_pushstring(luaState, "milestone");
		lua_gettable(luaState, -2);
		bool milestone = (bool)lua_toboolean(luaState, -1);
		lua_pop(luaState, 1);

		lua_pushstring(luaState, "dest_area");
		lua_gettable(luaState, -2);
		p.dest_area = std::string(lua_tostring(luaState, -1));
		lua_pop(luaState, 1);

		lua_pushstring(luaState, "dest_x");
		lua_gettable(luaState, -2);
		p.dest_x = (int)lua_tonumber(luaState, -1);
		lua_pop(luaState, 1);

		lua_pushstring(luaState, "dest_y");
		lua_gettable(luaState, -2);
		p.dest_y = (int)lua_tonumber(luaState, -1);
		lua_pop(luaState, 1);

		lua_pushstring(luaState, "dest_dir");
		lua_gettable(luaState, -2);
		p.dest_dir = ((int)(lua_tonumber(luaState, -1)-1));
		lua_pop(luaState, 1);


		if (milestone) {
			p.mapped = false;
			p.index = i;
			points.push_back(p);
		}
		else {
		}

		lua_pop(luaState, 1);
	}

	for (int i = 0; i < (int)points.size(); i++) {
		callLua(luaState, "get_area_table_name", "i>s", points[i].index+1);
		char areaname[1000];
		strcpy(areaname, lua_tostring(luaState, -1));
		lua_pop(luaState, 1);
		lua_getglobal(luaState, areaname);

		lua_pushstring(luaState, "left");
		lua_gettable(luaState, -2);
		if (lua_isnil(luaState, -1)) {
			points[i].links[(int)DIRECTION_WEST] = NULL;
		}
		else {
			const char *left = lua_tostring(luaState, -1);
			points[i].links[(int)DIRECTION_WEST] = findPoint(std::string(left));
		}
		// FIXME:!
		lua_pop(luaState, 1);

		lua_pushstring(luaState, "right");
		lua_gettable(luaState, -2);
		if (lua_isnil(luaState, -1)) {
			points[i].links[(int)DIRECTION_EAST] = NULL;
		}
		else {
			const char *right = lua_tostring(luaState, -1);
			points[i].links[(int)DIRECTION_EAST] = findPoint(std::string(right));
		}
		lua_pop(luaState, 1);


		lua_pushstring(luaState, "up");
		lua_gettable(luaState, -2);
		if (lua_isnil(luaState, -1)) {
			points[i].links[(int)DIRECTION_NORTH] = NULL;
		}
		else {
			const char *up = lua_tostring(luaState, -1);
			points[i].links[(int)DIRECTION_NORTH] = findPoint(std::string(up));
		}
		lua_pop(luaState, 1);


		lua_pushstring(luaState, "down");
		lua_gettable(luaState, -2);
		if (lua_isnil(luaState, -1)) {
			points[i].links[(int)DIRECTION_SOUTH] = NULL;
		}
		else {
			const char *down = lua_tostring(luaState, -1);
			points[i].links[(int)DIRECTION_SOUTH] = findPoint(std::string(down));
		}
		lua_pop(luaState, 1);

		lua_pop(luaState, 1);
	}

	setSelected(start);

	lua_close(luaState);

	getLines(&points[selected]);
}


void MMap::getIdealPoint(int x, int y, int *dx, int *dy)
{
	if (x <= BW/2) {
		*dx = 0;
	}
	else if (x >= m_get_bitmap_width(map_bmp)-BW/2) {
		*dx = m_get_bitmap_width(map_bmp)-BW;
	}
	else {
		*dx = x - BW/2;
	}
	if (y <= BH/2) {
		*dy = 0;
	}
	else if (y >= m_get_bitmap_height(map_bmp)-BH/2) {
		*dy = m_get_bitmap_height(map_bmp)-BH;
	}
	else {
		*dy = y - BH/2;
	}
}


bool MMap::isTransitioning(void)
{
	return transitioning;
}


MMap::MMap(std::string start, std::string prefix)
{
	this->x = 0;
	this->y = 0;
	this->width = BW;
	this->height = BH;
	this->hotkeys = 0;
	this->start = start;
	this->prefix = prefix;

	this->prev = start;

	map_bmp = m_load_bitmap(getResource("media/%s.png", prefix.c_str()));
	down_arrow = m_load_bitmap(getResource("media/down_arrow.png"));

	load_map_data();

	getIdealPoint(points[selected].x, points[selected].y, &top_x, &top_y);

	transitioning = false;

	clicked = false;

	shouldFlash = false;

	l_pressed =
	r_pressed =
	u_pressed =
	d_pressed = false;

	mouse_down = false;
	downX = -1;
	downY = -1;
	offset_x = 0;
	offset_y = 0;

	Input *i = getInput();
	if (i) i->set(false, false, false, false, false, false, false);
}


MMap::~MMap(void)
{
	m_destroy_bitmap(map_bmp);
	m_destroy_bitmap(down_arrow);

	for (int i = 0; i < (int)lines.size(); i++) {
		m_destroy_bitmap(lines[i].bitmap);
	}
	lines.clear();

	points.clear();

	std::map<std::string, MBITMAP *>::iterator it;
	for (it = bitmaps.begin(); it != bitmaps.end(); it++) {
		MBITMAP *b = it->second;
		if (b) {
			m_destroy_bitmap(b);
		}
	}
}

bool MSpellSelector::spellsBelow(void)
{
	if (!battle) return true;
	int i = top + rows*2;
	for (; i < MAX_SPELLS_IN_THIS_GAME; i++) {
		if (playerInfo.spells[i] != "")
			return true;
	}
	return false;
}


void MSpellSelector::setFocus(bool f)
{
	if (!f) {
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
		if (dragging) {
			dragging = false;
			m_destroy_bitmap(dragBmp);
			dragBmp = NULL;
			first_finger_x = -1;
			first_finger_y = -1;
		}
#endif
	}
	TGUIWidget::setFocus(f);
}

void MSpellSelector::mouseMove(int xx, int yy, int zz)
{
#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_ANDROID
	ALLEGRO_MOUSE_STATE state;
	al_get_mouse_state(&state);
	if (xx >= x && yy >= y && xx < x+width && yy < y+height) {
		int diff = state.z - last_z;
		if (abs(diff) > 0) {
			scrollwheel_dir = -diff;
		}
	}
	last_z = state.z;
#endif

	if (first_finger_x < 0) return;

	int tmpx = xx - first_finger_x;
	int tmpy = yy - first_finger_y;

	if (abs(tmpx) <= 20 && abs(tmpy) <= 20) {
		first_finger_x = xx;
		first_finger_y = yy;
	}

	if (scrolling) {
		int dx = (xx - down2X);
		int dy = (yy - down2Y);

		/* Make sure we have the right finger */
		if (abs(dx) <= 20 && abs(dy) <= 20) {
			scroll_offs += dy*2;
			down2X = xx;
			down2Y = yy;
		}
	}
}

void MSpellSelector::mouseDownAbs(int xx, int yy, int b)
{
	if (scrolling)
		return;
	
	if (dragging)
		return;

	bool clicked_on = xx >= this->x && yy >= this->y &&
		xx < (this->x+this->width) && yy < (this->y+this->height);

	if (clicked_on) {
		xx -= this->x;
		yy -= this->y;
	}
	else {
		return;
	}

#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_ANDROID
	if (b == 2) {
		pressed = -1;
		return;
	}
#endif

	if (down || maybe_scrolling) {
		if (!scrolling && !dragging) {
			down2X = xx+x;
			down2Y = yy+y;
			scrolling = true;
			scroll_offs = 0;
			down = false;
			maybe_scrolling = false;
		}
		return;
	}

	int aw = m_get_bitmap_width(up_arrow)/2;
	int ah = m_get_bitmap_height(up_arrow)/2;
			
	const int arrow_radius = 10;
	int up_x = x + width - aw;
	int up_y = y + ah;
	int udx = (xx+x) - up_x;
	int udy = (yy+y) - up_y;
	int down_x = up_x;
	int down_y = y + height - ah;
	int ddx = (xx+x) - down_x;
	int ddy = (yy+y) - down_y;
	if (sqrtf(udx*udx+udy*udy) < arrow_radius) {
		if (top > 0) {
			playPreloadedSample("blip.ogg");
			top -= 2;
		}
	}
	else if (spellsBelow() && sqrtf(ddx*ddx+ddy*ddy) < arrow_radius) {
		if (top+(rows*2) < MAX_SPELLS_IN_THIS_GAME) {
			playPreloadedSample("blip.ogg");
			top += 2;
		}
	}
	else {
		int rows_down = (yy-3) / 15;
		int col = xx / (BW/3);
		if (col > 1) col = 1;
		int n = top + (rows_down*2) + col;
		if (n >= MAX_SPELLS_IN_THIS_GAME) {
			n = top + ((rows_down-1)*2) + col;
		}
		CombatantInfo info;
		if (partySelector) {
			Player *player = party[partySelector->getSelected()];
			info = player->getInfo();
		}
		else {
			info = playerInfo;
		}
		if (info.spells[n] != "") {
			if (selected == n)
				changed = false;
			else
				changed = true;
			selected = n;
			down = true;
			downX = xx+x;
			downY = yy+y;
			first_finger_x = downX;
			first_finger_y = downY;
			initial_down_x = downX;
			initial_down_y = downY;
		}
		else if (!use_dpad) {
			maybe_scrolling = true;
			selected = -1;
		}
	}
}

void MSpellSelector::mouseUpAbs(int xx, int yy, int b)
{
	maybe_scrolling = false;

	bool clicked_on = xx >= this->x && yy >= this->y &&
		xx < (this->x+this->width) && yy < (this->y+this->height);

	if (clicked_on) {
		xx -= this->x;
		yy -= this->y;
	}
	else {
		xx = yy = -1;
	}

	if (scrolling) {
		scrolling = false;
		down = false;
		goto done;
	}

	if (!down) goto done;
	down = false;

	if (selected < 0)
		return;

	if (xx >= 0 && yy >= 0) {
		if (dragging) {
			int rows_down = (yy-3) / 15;
			int col = xx / (BW/3);
			if (col > 1) col = 1;
			int n = top + rows_down*2 + col;
			if (n >= MAX_SPELLS_IN_THIS_GAME)
				n = top + (rows_down-1)*2 + col;
			pressed = n;
			clicked = true;
			was_dragged = true;
		}
		else {
			clicked = true;
		}
	}

done:
	if (dragging) {
		dragging = false;
		m_destroy_bitmap(dragBmp);
		dragBmp = NULL;
		first_finger_x = -1;
		first_finger_y = -1;
	}
}

void MSpellSelector::draw()
{
	mDrawFrame(3, y, BW-6, height);
	
	int dx;
	int dy = y+3;
	
	CombatantInfo info;
	if (partySelector) {
		Player *player = party[partySelector->getSelected()];
		if (player) {
			info = player->getInfo();
		}
		else {
			memset(&info.abilities, 0, sizeof(CombatantAbilities));
			memset(&info.equipment, 0, sizeof(CombatantEquipment));
			for (int i = 0; i < MAX_SPELLS_IN_THIS_GAME; i++) {
				info.spells[i] = "";
			}
			info.experience = 0;
			info.characterClass = 0;
			info.condition = (CombatCondition)0;
		}
	}
	else {
		info = playerInfo;
	}

	m_set_clip(0, dy-4, BW, dy+height-6);

	// draw names
	for (int i = top; i < top+(rows+1)*2 && i < MAX_SPELLS_IN_THIS_GAME; i++) {
		MCOLOR color;
		std::string name = info.spells[i];
		if (name == "") {
			goto loop;
		}
		if ((i % 2) == 0) {
			dx = 8;
		}
		else {
			dx = BW/3+5;
		}
		if (this == tguiActiveWidget && pressed == i && canArrange) {
			color = m_map_rgb(255, 255, 0);
		}
		else {
			color = grey;
		}
		mTextout(game_font, (std::string("{008}") + std::string(_t(name.c_str()))).c_str(), dx, dy,
			color, black,
			WGT_TEXT_DROP_SHADOW, false);
loop:
		if ((i % 2) == 1)
			dy += 15;
	}
	
	m_set_clip(0, 0, BW, BH);
	
	//draw cursor
	if (this == tguiActiveWidget && ((unsigned)tguiCurrentTimeMillis() % 300) < 150) {
		dx = (selected % 2) == 0 ? 3 : (BW/3-10);
		dy = ((selected - top) / 2) * 15 + y;

		int bmph = m_get_bitmap_height(arrow);

		if (this == tguiActiveWidget && selected >= 0) {
			if (dy+bmph/2 > y && dy+bmph/2 < y+height && top <= selected) {
				m_draw_bitmap(arrow, dx, dy, M_FLIP_HORIZONTAL);
			}
		}
	}
	
	
	if (dragging) {
		ALLEGRO_MOUSE_STATE state;
		m_get_mouse_state(&state);
		m_save_blender();
		m_set_blender(M_ONE, M_INVERSE_ALPHA, al_map_rgba(128, 128, 128, 128));
		int w = m_get_bitmap_width(dragBmp);
		int h = m_get_bitmap_height(dragBmp);
		m_draw_scaled_bitmap(dragBmp, 0, 0, w, h,
			state.x-w, state.y-h,
			w*2, h*2, 0, 255);
		m_restore_blender();
	}

	if (use_dpad && this != tguiActiveWidget)
		return;
	

	// draw cost & remaining MP
	int fx = BW/3*2+20;
	int fy = y;
	int fw = BW-fx-3;
	int fh = 45;

	mDrawFrame(fx, fy, fw, fh);

	mTextout(game_font, _t("Cost:"), fx+3, fy+2,
		grey, black,
		WGT_TEXT_DROP_SHADOW, false);
	
	char cost[10];
	if (selected >= 0 && info.spells[selected] != "") {
		sprintf(cost, "%d", getSpellCost(info.spells[selected]));
	}
	else {
		strcpy(cost, "-");
	}
		mTextout(game_font, cost, fx+3, fy+2+9,
		grey, black,
		WGT_TEXT_DROP_SHADOW, false);

	mTextout(game_font, _t("Remain:"), fx+3, fy+2+18,
		grey, black,
		WGT_TEXT_DROP_SHADOW, false);
	
	char remain[10];
	sprintf(remain, "%d", info.abilities.mp);
		mTextout(game_font, remain, fx+3, fy+2+27,
		grey, black,
		WGT_TEXT_DROP_SHADOW, false);
	

	// draw arrows
	if (top > 0) {
		dx = x + width - m_get_bitmap_width(up_arrow);
		dy = y;
		m_draw_bitmap(up_arrow, dx, dy, 0);
	}
	if (spellsBelow() && top+(rows*2) < MAX_SPELLS_IN_THIS_GAME) {
		dx = x + width - m_get_bitmap_width(up_arrow);
		dy = y + height - m_get_bitmap_height(up_arrow);
		m_draw_bitmap(up_arrow, dx, dy, M_FLIP_VERTICAL);
	}
}


int MSpellSelector::update(int millis)
{
	if (partySelector) {
		int w = partySelector->getSelected();
		if (w != who) {
			top = selected = 0;
			who = w;
		}
	}

	if (scrolling) {
		int d = scroll_offs / 12;
		if (d) {
			int n = top + d*2;
			if (n < 0) n = 0;
			else if (n+rows*2 >= MAX_SPELLS_IN_THIS_GAME) n = MAX_SPELLS_IN_THIS_GAME - rows*2;
			top = /*selected =*/ n;
		}
		scroll_offs %= 12;
		return TGUI_CONTINUE;
	}

	if (!dragging && down && canArrange) {
		ALLEGRO_MOUSE_STATE state;
		m_get_mouse_state(&state);
		CombatantInfo info;
		if (partySelector) {
			Player *player = party[partySelector->getSelected()];
			info = player->getInfo();
		}
		else {
			info = playerInfo;
		}
		if ((abs(initial_down_x-first_finger_x) >= 10 || abs(initial_down_y-first_finger_y) >= 10) && selected >= 0 && selected < MAX_SPELLS_IN_THIS_GAME && info.spells[selected] != "" && canArrange) {
			dragging = true;
			char s[100];
			if (info.spells[selected] == "")
				strcpy(s, "<empty>");
			else
				strcpy(s, _t(info.spells[selected].c_str()));
			int w = m_text_length(game_font, _t(s))+1;
			m_push_target_bitmap();
			dragBmp = m_create_alpha_bitmap(w, m_text_height(game_font)+4); // check
			m_set_target_bitmap(dragBmp);
			m_clear(m_map_rgba(0, 0, 0, 0));
			mTextout(game_font, s, 0, 2,
				grey, black,
				WGT_TEXT_DROP_SHADOW, false);
			m_pop_target_bitmap();
		}
	}
	else if (dragging) {
		if (this == tguiActiveWidget && iphone_shaken(0.1)) {
			iphone_clear_shaken();
			down = false;
			dragging = false;
			m_destroy_bitmap(dragBmp);
			dragBmp = NULL;
			first_finger_x = -1;
			first_finger_y = -1;
		}
		else {
			ALLEGRO_MOUSE_STATE state;
			m_get_mouse_state(&state);
			int aw = m_get_bitmap_width(up_arrow)/2;
			int ah = m_get_bitmap_height(up_arrow)/2;
			
			const int arrow_radius = 10;
			int up_x = x + width - aw;
			int up_y = y + ah;
			int udx = state.x - up_x;
			int udy = state.y - up_y;
			int down_x = up_x;
			int down_y = y + height - ah;
			int ddx = state.x - down_x;
			int ddy = state.y - down_y;
			if (sqrtf(udx*udx+udy*udy) < arrow_radius) {
				if (top > 0) {
					scrollCount += millis;
					if (scrollCount > 300) {
						playPreloadedSample("blip.ogg");
						top -= 2;
						//if (selected > 2)
						//	selected -= 2;
						scrollCount = 0;
					}
				}
			}
			else if (spellsBelow() && sqrtf(ddx*ddx+ddy*ddy) < arrow_radius) {
				if (top+(rows*2) < MAX_SPELLS_IN_THIS_GAME) {
					scrollCount += millis;
					if (scrollCount > 300) {
						playPreloadedSample("blip.ogg");
						top += 2;
						scrollCount = 0;
					}
				}
			}
			else
				scrollCount = 0;
		}
	}

	INPUT_EVENT ie;
	if (this != tguiActiveWidget) {
		ie = EMPTY_INPUT_EVENT;
	}
	else {
		ie = get_next_input_event();
	}

	bool play_sound = true;

	if (ie.left == DOWN) {
		use_input_event();
		playPreloadedSample("blip.ogg");
		if ((selected % 2) == 1) {
			selected--;
		}
	}
	else if (ie.right == DOWN) {
		use_input_event();
		playPreloadedSample("blip.ogg");
		if ((selected % 2) == 0) {
			selected++;
		}
	}
	else if (ie.up == DOWN || scrollwheel_dir < 0) {
		use_input_event();
		playPreloadedSample("blip.ogg");
		if (scrollwheel_dir < 0) {
			scrollwheel_dir++;
			if (top >= 2)
				top -= 2;
		}
		else if (selected > 1) {
			selected -= 2;
			if (selected < top) {
				top -= 2;
			}
		}
		else {
			tguiFocusPrevious();
		}
	}
	else if (ie.down == DOWN || scrollwheel_dir > 0) {
		use_input_event();
		playPreloadedSample("blip.ogg");
		if (scrollwheel_dir > 0) {
			scrollwheel_dir--;
			if ((top+(rows*2)) < MAX_SPELLS_IN_THIS_GAME)
				top += 2;
		}
		else if (selected < (MAX_SPELLS_IN_THIS_GAME-2)) {
			selected += 2;
			if (((selected - top) / 2) >= rows) {
				top += 2;
			}
		}
		else {
			tguiFocusNext();
		}
	}
	else if (ie.button1 == DOWN || clicked) {
		use_input_event();
		int tmp = pressed;
		if (tmp < 0) {
			CombatantInfo info;
			if (partySelector) {
				Player *player = party[partySelector->getSelected()];
				info = player->getInfo();
			}
			else {
				info = playerInfo;
			}
			if (info.spells[selected] != "") {
				pressed = selected;
				playPreloadedSample("select.ogg");
				play_sound = false;
			}
			else {
				down = false;
			}
		}
		if (tmp >= 0 || (!canArrange && !clicked)) {
			if (play_sound) {
				playPreloadedSample("select.ogg");
			}
			// Use
			if (pressed == selected) {
				clicked = false;
				pressed = -1;
				return TGUI_RETURN;
			}
			// Arrange
			else {
				if (canArrange) {
					if (!clicked || was_dragged) {
						if (was_dragged) {
							was_dragged = false;
							int tmp = pressed;
							pressed = selected;
							selected = tmp;
						}
						CombatantInfo *info;
						if (partySelector) {
							Player *player = party[partySelector->getSelected()];
							info = &player->getInfo();
						}
						else {
							info = &playerInfo;
						}
						std::string tmp;
						tmp = info->spells[pressed];
						info->spells[pressed] = info->spells[selected];
						info->spells[selected] = tmp;
						pressed = -1;
					}
					else if (clicked) {
						pressed = selected;
					}
					else {
						pressed = -1;
					}
				}
				else {
					pressed = -1;
				}
			}
		}
		clicked = false;
	}
	else if (this == tguiActiveWidget && (ie.button2 == DOWN || (!dragging && iphone_shaken(0.1)))) {
		use_input_event();
		iphone_clear_shaken();
		playPreloadedSample("select.ogg");
		if (pressed < 0) {
			selected = -1;
			return TGUI_RETURN;
		}
		else {
			pressed = -1;
		}
	}

	return TGUI_CONTINUE;
}


int MSpellSelector::getSelected(void)
{
	return selected;
}


void MSpellSelector::setSelected(int s)
{
	selected = s;
}


void MSpellSelector::setTop(int t)
{
	top = t;
}


bool MSpellSelector::acceptsFocus(void)
{
	return true;
}


MSpellSelector::MSpellSelector(int y1, int y2, int top, int selected,
	bool canArrange, MPartySelector *partySelector, CombatantInfo playerInfo)
{
	this->x = 0;
	this->y = y1;
	this->width = BW;
	this->height = y2-y1;
	this->hotkeys = 0;
	this->y2 = y2;
	this->top = top;
	this->selected = selected;
	this->canArrange = canArrange;
	this->partySelector = partySelector;
	this->playerInfo = playerInfo;
	rows = height / 15;
	pressed = -1;
	arrow = m_load_bitmap(getResource("media/arrow.png"));
	up_arrow = m_load_bitmap(getResource("media/up.png"));
	clicked = false;
	down = false;
	downX = -1;
	downY = -1;
	dragging = false;
	scrollCount = 0;
	down2X = -1;
	down2Y = -1;
	scrolling = false;
	scroll_offs = 0;
	first_finger_x = -1;
	first_finger_y = -1;
	if (partySelector) {
		who = partySelector->getSelected();
	}
	dragBmp = NULL;
	maybe_scrolling = false;
	was_dragged = false;

#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_ANDROID
	ALLEGRO_MOUSE_STATE state;
	al_get_mouse_state(&state);
	last_z = state.z;
#endif
	scrollwheel_dir = 0;
}


MSpellSelector::~MSpellSelector(void)
{
	m_destroy_bitmap(arrow);
	m_destroy_bitmap(up_arrow);
}

void MToggleList::setFocus(bool f)
{
	if (!f) {
		if (dragging) {
			dragging = false;
			m_destroy_bitmap(dragBmp);
			dragBmp = NULL;
			first_finger_x = -1;
			first_finger_y = -1;
		}
	}
	TGUIWidget::setFocus(f);
}

void MToggleList::mouseMove(int xx, int yy, int zz)
{
	if (first_finger_x < 0) return;

	int tmpx = xx - first_finger_x;
	int tmpy = yy - first_finger_y;

	if (abs(tmpx) <= 20 && abs(tmpy) <= 20) {
		first_finger_x = xx;
		first_finger_y = yy;
	}

	if (scrolling) {
		int dx = (xx - down2X);
		int dy = (yy - down2Y);

		/* Make sure we have the right finger */
		if (abs(dx) <= 20 && abs(dy) <= 20) {
			scroll_offs += dy*2;
			down2X = xx;
			down2Y = yy;
		}
	}
	else if (down) {
		bool clicked_on = xx >= this->x && yy >= this->y &&
			xx < (this->x+this->width) && yy < (this->y+this->height);

		if (!clicked_on) {
			return;
		}

		int rows_down = (yy-y) / 15;
		if (rows_down < rows) {
			int n = top + rows_down;
			int tmpy = yy - initial_down_y;
			if (n < (int)items.size()/* && abs(tmpy) > 6*/) {
				if (abs(tmpy) >= 3 && !moved)
					last_clicked = -1;
				selected = n;
				clicked = true;
				moved = true;
			}
		}
	}
}

void MToggleList::mouseDownAbs(int xx, int yy, int b)
{
	if (scrolling)
		return;

	bool clicked_on = xx >= this->x && yy >= this->y &&
		xx < (this->x+this->width) && yy < (this->y+this->height);

	if (down) {
		if (!scrolling) {
			down2X = xx;
			down2Y = yy;
			scrolling = true;
			scroll_offs = 0;
			selected = -1;
			last_clicked = -1;
			up_selected = -2;
			down = false;
		}
		return;
	}

	if (clicked_on) {
		xx -= this->x;
		yy -= this->y;
	}
	else {
		return;
	}

	downCount = 0;

	int aw = m_get_bitmap_width(up_arrow)/2;
	int ah = m_get_bitmap_height(up_arrow)/2;
			
	const int arrow_radius = 14;
	int up_x = x + width - aw/2;
	int up_y = y + ah/2;
	int udx = (xx+x) - up_x;
	int udy = (yy+y) - up_y;
	int down_x = up_x;
	int down_y = y + height - ah/2;
	int ddx = (xx+x) - down_x;
	int ddy = (yy+y) - down_y;
	if (sqrtf(udx*udx+udy*udy) < arrow_radius) {
		if (top > 0) {
			playPreloadedSample("blip.ogg");
			top--;
			if (selected > 0)
				selected--;
		}
	}
	else if (sqrtf(ddx*ddx+ddy*ddy) < arrow_radius) {
		if (top+rows < (int)items.size()) {
			playPreloadedSample("blip.ogg");
			top++;
			if (selected < (int)items.size()) {
				selected++;
			}
		}
	}
	else if (xx < width-30) {
		int rows_down = yy / 15;
		if (rows_down < rows) {
			int n = top + rows_down;
			if (n < (int)items.size()) {
				downX = xx+x;
				downY = yy+y;
				first_finger_x = downX;
				first_finger_y = downY;
				initial_down_x = downX;
				initial_down_y = downY;
				selected = n;
				last_clicked = n;
				up_selected = -2;
			}
		}
	}
	down = true;
}

void MToggleList::mouseUpAbs(int xx, int yy, int b)
{
	bool clicked_on = xx >= this->x && yy >= this->y &&
		xx < (this->x+this->width) && yy < (this->y+this->height);

	if (clicked_on) {
		xx -= this->x;
		yy -= this->y;
	}
	else {
		xx = yy = -1;
	}

	if (scrolling) {
		scrolling = false;
		down = false;
		last_clicked = -1;
		selected = -1;
		up_selected = -2;
		return;
	}

	if (!down) return;
	down = false;

	if (xx >= 0 && yy >= 0 && !moved) {
		if (!dragging) {
			int rows_down = yy / 15;
			if (rows_down < rows) {
				int n = top + rows_down;
				if (n < (int)items.size()) {
					up_selected = n;
					if (up_selected == selected) {
						clicked = true;
						return;
					}
				}
			}
		}
	}
	last_clicked = -1;
	selected = -1;
	up_selected = -2;
	moved = false;
}

void MToggleList::draw()
{
	int yy = y;

	for (int i = 0; i < rows; i++) {
		int r = i + top;
		if (r >= (int)items.size())
			break;
		
		char buf[1000];
		int count = 0;

		if (strlen(items[r].c_str()) > 1) {
			while (1) {
				strncpy(buf, _t(items[r].c_str()), count+1);
				buf[count+1] = 0;
				if (m_text_length(game_font, buf) > width-30)
					break;
				count++;
				if (count == (int)strlen(items[r].c_str()))
					break;
			}
		}
		else {
			count = strlen(items[r].c_str());
		}

		strncpy(buf, _t(items[r].c_str()), count);
		buf[count] = 0;

		if (use_dpad && cursor == r) {
			m_draw_rectangle(0, yy-2, BW, yy+m_text_height(game_font)+2, m_map_rgb(30, 60, 30), M_FILLED);
		}
		if (toggled[r])
			mTextout(game_font, buf, x, yy,
				m_map_rgb(255, 255, 0),
				black,
				WGT_TEXT_DROP_SHADOW, false);
		else
			mTextout(game_font, buf, x, yy,
				grey,
				black,
				WGT_TEXT_DROP_SHADOW, false);
		yy += 15;
	}
	
	if (dragging) {
		ALLEGRO_MOUSE_STATE state;
		m_get_mouse_state(&state);
		m_save_blender();
		m_set_blender(M_ONE, M_INVERSE_ALPHA, al_map_rgba(128, 128, 128, 128));
		int w = m_get_bitmap_width(dragBmp);
		int h = m_get_bitmap_height(dragBmp);
		m_draw_scaled_bitmap(dragBmp, 0, 0, w, h,
			state.x-w, state.y-h,
			w*2, h*2, 0, 255);
		m_restore_blender();
	}

	int dx, dy;
	/* draw arrows */
	if (top > 0) {
		dx = x + width - m_get_bitmap_width(up_arrow);
		dy = y;
		m_draw_bitmap(up_arrow, dx, dy, 0);
	}
	if (top+rows < (int)items.size()) {
		dx = x + width - m_get_bitmap_width(up_arrow);
		dy = y + height - m_get_bitmap_height(up_arrow);
		m_draw_bitmap(up_arrow, dx, dy, M_FLIP_VERTICAL);
	}
}
					
int MToggleList::update(int millis)
{
	if (scrolling) {
		int d = scroll_offs / 15;
		if (d) {
			int n = top + d;
			if (n < 0) n = 0;
			else if (n+rows > (int)items.size()) n = items.size() - rows;
			top /*= selected*/ = n;
		}
		scroll_offs %= 15;
		return TGUI_CONTINUE;
	}

	if (use_dpad && this == tguiActiveWidget) {
		INPUT_EVENT ie = get_next_input_event();
		if (ie.left == DOWN) {
			use_input_event();
			playPreloadedSample("blip.ogg");
			tguiFocusPrevious();
		}
		else if (ie.right == DOWN) {
			use_input_event();
			playPreloadedSample("blip.ogg");
			tguiFocusNext();
		}
		else if (ie.up == DOWN) {
			use_input_event();
			if (cursor > 0) {
				playPreloadedSample("blip.ogg");
				cursor--;
				if (cursor < top)
					top = cursor;
			}
		}
		else if (ie.down == DOWN) {
			use_input_event();
			if (cursor < (int)items.size()-1) {
				playPreloadedSample("blip.ogg");
				cursor++;
				if (top+rows <= cursor)
					top++;
			}
		}
		else if (ie.button1 == DOWN) {
			use_input_event();
			playPreloadedSample("select.ogg");
			toggled[cursor] = !toggled[cursor];
		}
	}
	else {
		if (clicked && ((last_clicked != selected) || (selected == up_selected))) {
			clicked = false;
			if (last_clicked != selected)
				last_clicked = selected;
			playPreloadedSample("select.ogg");
			toggled[selected] = !toggled[selected];
			selected = -1;
			up_selected = -2;
		}
	}

	return TGUI_CONTINUE;
}

void MToggleList::setItems(std::vector<std::string> items)
{
	this->items = items;

	toggled.clear();
	for (int i = 0; i < (int)items.size(); i++) {
		toggled.push_back(onoff);
	}
}

std::vector<bool> MToggleList::getToggled(void)
{
	return toggled;
}

void MToggleList::setToggled(std::vector<bool> t)
{
	for (int i = 0; i < (int)toggled.size(); i++) {
		toggled[i] = t[i];
	}
}

int MToggleList::getSelected(void)
{
	return selected;
}

bool MToggleList::acceptsFocus(void)
{
	return true;
}


MToggleList::MToggleList(int x, int y, int w, int h, bool onoff)
{
	last_clicked = -1;
	cursor = 0;
	this->x = x;
	this->y = y;
	this->width = w;
	this->height = h;
	this->hotkeys = 0;
	this->selected = -2;
	this->top = 0;
	this->onoff = onoff;
	rows = height / 15;
	up_arrow = m_load_bitmap(getResource("media/up.png"));
	clicked = false;
	down = false;
	downX = -1;
	downY = -1;
	dragging = false;
	down2X = -1;
	down2Y = -1;
	scrolling = false;
	scroll_offs = 0;
	up_selected = -1;
	first_finger_x = -1;
	first_finger_y = -1;
	drop_callback = NULL;
	drop_x = drop_y = 0;
	dragBmp = NULL;
	moved = false;
}



MToggleList::~MToggleList(void)
{
	m_destroy_bitmap(up_arrow);
}


void MScrollingList::setFocus(bool f)
{
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
	was_down = getInput()->getDescriptor().button1;
#else
	was_down = false;
#endif

	if (!f) {
		if (dragging) {
			dragging = false;
			m_destroy_bitmap(dragBmp);
			dragBmp = NULL;
			first_finger_x = -1;
			first_finger_y = -1;
		}
	}
	TGUIWidget::setFocus(f);
}

void MScrollingList::mouseMove(int xx, int yy, int zz)
{
#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_ANDROID
	ALLEGRO_MOUSE_STATE state;
	al_get_mouse_state(&state);
	if (xx >= x && yy >= y && xx < x+width && yy < y+height) {
		int diff = state.z - last_z;
		if (abs(diff) > 0) {
			scrollwheel_dir = -diff;
		}
	}
	last_z = state.z;
#endif

	if (first_finger_x < 0) return;

	int tmpx = xx - first_finger_x;
	int tmpy = yy - first_finger_y;

	if (abs(tmpx) <= 20 && abs(tmpy) <= 20) {
		first_finger_x = xx;
		first_finger_y = yy;
	}

	if (scrolling) {
		int dx = (xx - down2X);
		int dy = (yy - down2Y);

		/* Make sure we have the right finger */
		if (abs(dx) <= 20 && abs(dy) <= 20) {
			scroll_offs += dy*2;
			down2X = xx;
			down2Y = yy;
		}
	}
}

void MScrollingList::mouseDownAbs(int xx, int yy, int b)
{
	if (scrolling)
		return;

	bool clicked_on = xx >= this->x && yy >= this->y &&
		xx < (this->x+this->width) && yy < (this->y+this->height);

	if (clicked_on) {
		xx -= this->x;
		yy -= this->y;
	}
	else {
		return;
	}

	if (down) {
		if (!scrolling && !dragging && rows < (int)items.size()) {
			down2X = xx+x;
			down2Y = yy+y;
			scrolling = true;
			scroll_offs = 0;
			down = false;
		}
		return;
	}
	downCount = 0;

	int aw = m_get_bitmap_width(up_arrow)/2;
	int ah = m_get_bitmap_height(up_arrow)/2;
			
	const int arrow_radius = 14;
	int up_x = x + width - aw/2;
	int up_y = y + ah/2;
	int udx = (xx+x) - up_x;
	int udy = (yy+y) - up_y;
	int down_x = up_x;
	int down_y = y + height - ah/2;
	int ddx = (xx+x) - down_x;
	int ddy = (yy+y) - down_y;
	if (sqrtf(udx*udx+udy*udy) < arrow_radius) {
		if (top > 0) {
			playPreloadedSample("blip.ogg");
			top--;
			if (selected > 0)
				selected--;
		}
	}
	else if (sqrtf(ddx*ddx+ddy*ddy) < arrow_radius) {
		if (top+rows < (int)items.size()) {
			playPreloadedSample("blip.ogg");
			top++;
			if (selected < (int)items.size()) {
				selected++;
			}
		}
	}
	else if (xx < width-30) {
		int rows_down = yy / 15;
		if (rows_down < rows) {
			int n = top + rows_down;
			if (n < (int)items.size()) {
				down = true;
				downX = xx+x;
				downY = yy+y;
				first_finger_x = downX;
				first_finger_y = downY;
				initial_down_x = downX;
				initial_down_y = downY;
				selected = n;
			}
		}
	}
}

void MScrollingList::mouseUpAbs(int xx, int yy, int b)
{
	int mx = xx;
	int my = yy;

	bool clicked_on = xx >= this->x && yy >= this->y &&
		xx < (this->x+this->width) && yy < (this->y+this->height);

	if (clicked_on) {
		xx -= this->x;
		yy -= this->y;
	}
	else {
		xx = yy = -1;
	}

	if (scrolling) {
		scrolling = false;
		down = false;
		return;
	}

	if (!down) return;
	down = false;

	if (xx >= 0 && yy >= 0) {
		if (!dragging) {
			int rows_down = yy / 15;
			if (rows_down < rows) {
				int n = top + rows_down;
				if (n < (int)items.size()) {
					up_selected = n;
					clicked = true;
				}
			}
		}
	}
	else {
		int dx = drop_x - mx;
		int dy = drop_y - my;
		al_set_target_backbuffer(display);
		m_clear(black);
		tguiDraw();
		hide_mouse_cursor();
		drawBufferToScreen();
		show_mouse_cursor();
		if (drop_callback && sqrt((float)dx*dx + dy*dy) < 10) {
			drop_callback(selected);
		}
		else {
			clicked = true;
		}
	}

	if (dragging) {
		dragging = false;
		m_destroy_bitmap(dragBmp);
		dragBmp = NULL;
		first_finger_x = -1;
		first_finger_y = -1;
	}
}

void MScrollingList::post_draw()
{
	if (dragging) {
		ALLEGRO_MOUSE_STATE state;
		m_get_mouse_state(&state);
		m_save_blender();
		m_set_blender(M_ONE, M_INVERSE_ALPHA, al_map_rgba(128, 128, 128, 128));
		int w = m_get_bitmap_width(dragBmp);
		int h = m_get_bitmap_height(dragBmp);
		m_draw_scaled_bitmap(dragBmp, 0, 0, w, h,
			state.x-w, state.y-h,
			w*2, h*2, 0, 255);
		m_restore_blender();
	}
}

void MScrollingList::draw()
{
	int yy = y;

	int cx, cy, cw, ch;
	al_get_clipping_rectangle(&cx, &cy, &cw, &ch);
	m_set_clip(x, y-1, width-30+x, 15*rows+1+y-1);

	bool held = al_is_bitmap_drawing_held();
	al_hold_bitmap_drawing(true);

	for (int i = 0; i < rows; i++) {
		int r = i + top;
		if (r >= (int)items.size())
			break;

		if (r == selected) {
			if (this == tguiActiveWidget)
				mTextout_simple(_t(items[r].c_str()), x, yy, m_map_rgb(255, 255, 0));
			else
				mTextout_simple(_t(items[r].c_str()), x, yy, grey);
		}
		else
			mTextout_simple(_t(items[r].c_str()), x, yy, grey);
		yy += 15;
	}

	al_hold_bitmap_drawing(held);
	
	al_set_clipping_rectangle(cx, cy, cw, ch);
	
	int dx, dy;
	/* draw arrows */
	if (top > 0) {
		dx = x + width - m_get_bitmap_width(up_arrow);
		dy = y;
		m_draw_bitmap(up_arrow, dx, dy, 0);
	}
	if (top+rows < (int)items.size()) {
		dx = x + width - m_get_bitmap_width(up_arrow);
		dy = y + height - m_get_bitmap_height(up_arrow);
		m_draw_bitmap(up_arrow, dx, dy, M_FLIP_VERTICAL);
	}
}
					
int MScrollingList::update(int millis)
{
	if (was_down && getInput()->getDescriptor().button1)
		return TGUI_CONTINUE;
	else
		was_down = false;

	if (scrolling) {
		int d = scroll_offs / 15;
		if (d) {
			int n = top + d;
			if (n < 0) n = 0;
			else if (n+rows > (int)items.size()) n = items.size() - rows;
			top = selected = n;
		}
		scroll_offs %= 15;
		return TGUI_CONTINUE;
	}

	if (!dragging && down) {
		
		if (abs(initial_down_x-first_finger_x) >= 10 || abs(initial_down_y-first_finger_y) >= 10) {
			dragging = true;
			char s[100];
			sprintf(s, "%s", _t(items[selected].c_str()));
			int w = m_text_length(game_font, _t(s))+1;
			m_push_target_bitmap();
			dragBmp = m_create_alpha_bitmap(w, m_text_height(game_font)+4); // check
			m_set_target_bitmap(dragBmp);
			m_clear(m_map_rgba(0, 0, 0, 0));
			mTextout(game_font, s, 0, 2,
				grey, black,
				WGT_TEXT_DROP_SHADOW, false);
			m_pop_target_bitmap();
		}
		else {
			if (selected >= 0 && selected < (int)items.size()) {
				downCount += millis;
				if (downCount >= 600 && hold_callback) {
					playPreloadedSample("select.ogg");
					al_set_target_backbuffer(display);
					m_clear(black);
					tguiDraw();
					hide_mouse_cursor();
					drawBufferToScreen();
					show_mouse_cursor();
					hold_callback(selected, hold_data);
					down = false;
					downX = -1;
					downY = -1;
					downCount = 0;
					clicked = false;
				}
			}
		}
	}
	else if (dragging) {
		if (this == tguiActiveWidget && iphone_shaken(0.1)) {
			iphone_clear_shaken();
			down = false;
			dragging = false;
			m_destroy_bitmap(dragBmp);
			dragBmp = NULL;
			first_finger_x = -1;
			first_finger_y = -1;
		}
	}

	if (scrollwheel_dir < 0) {
		scrollwheel_dir++;
		if (top > 0) {
			top--;
		}
		playPreloadedSample("blip.ogg");
	}
	if (scrollwheel_dir > 0) {
		scrollwheel_dir--;
		if (top+rows < (int)items.size()) {
			top++;
		}
		playPreloadedSample("blip.ogg");
	}

	if (this == tguiActiveWidget) {
		INPUT_EVENT ie = get_next_input_event();
		InputDescriptor id = getInput()->getDescriptor();
		if (!id.button1 && holdTime != -1) {
			use_input_event();
			if ((unsigned long)holdTime+250 > tguiCurrentTimeMillis()) {
				clicked = true;
			}
			holdTime = -1;
		}
		else if (id.button1 && holdTime == -1) {
			use_input_event();
			holdTime = tguiCurrentTimeMillis();
		}
		else if (id.button1) {
			int elapsed = tguiCurrentTimeMillis() - holdTime;
			if (hold_callback && elapsed > 600) {
				playPreloadedSample("select.ogg");
				al_set_target_backbuffer(display);
				m_clear(black);
				tguiDraw();
				hide_mouse_cursor();
				drawBufferToScreen();
				show_mouse_cursor();
				hold_callback(selected, hold_data);
				holdTime = -1;
			}
		}
		if (ie.left == DOWN) {
			use_input_event();
			playPreloadedSample("blip.ogg");
			tguiFocusPrevious();
		}
		else if (ie.right == DOWN) {
			use_input_event();
			playPreloadedSample("blip.ogg");
			tguiFocusNext();
		}
		else if (ie.up == DOWN) {
			use_input_event();
			if (selected) {
				selected--;
				if (selected < top)
					top = selected;
			}
			else
				tguiFocusPrevious();
			playPreloadedSample("blip.ogg");
		}
		else if (ie.down == DOWN) {
			use_input_event();
			if (selected < (int)items.size()-1) {
				selected++;
				if (top+rows <= selected)
					top++;
			}
			else
				tguiFocusNext();
			playPreloadedSample("blip.ogg");
		}
		else if (ie.button3 == DOWN && hold_callback) {
			use_input_event();
			playPreloadedSample("select.ogg");
			al_set_target_backbuffer(display);
			m_clear(black);
			tguiDraw();
			hide_mouse_cursor();
			drawBufferToScreen();
			show_mouse_cursor();
			hold_callback(selected, hold_data);
		}
	}

	if (clicked) {
		clicked = false;
		if (selected == up_selected && selected < (int)items.size()) {
			playPreloadedSample("select.ogg");
			up_selected = -1;
			return TGUI_RETURN;
		}
		else {
			playPreloadedSample("select.ogg");
			al_set_target_backbuffer(display);
			m_clear(black);
			tguiDraw();
			hide_mouse_cursor();
			drawBufferToScreen();
			show_mouse_cursor();
			if (!do_prompt || prompt("Run this game?", "", 0, 1)) {
				return TGUI_RETURN;
			}
			al_set_target_backbuffer(display);
			m_clear(black);
			tguiDraw();
			hide_mouse_cursor();
			drawBufferToScreen();
			show_mouse_cursor();
			if (prompt("Delete this game?", "", 0, 0)) {
				if (drop_callback) {
					al_set_target_backbuffer(display);
					m_clear(black);
					tguiDraw();
					hide_mouse_cursor();
					drawBufferToScreen();
					show_mouse_cursor();
					drop_callback(selected);
					// delete
					if (items.size() <= 0) {
						selected = -1;
						tguiFocusPrevious();
					}
					else if (selected)
						selected--;
					else
						selected++;
				}
			}
		}
	}
	else if (this == tguiActiveWidget && (!dragging && iphone_shaken(0.1))) {
		iphone_clear_shaken();
		playPreloadedSample("select.ogg");
		selected = -1;
		return TGUI_RETURN;
	}

	return TGUI_CONTINUE;
}

void MScrollingList::setItems(std::vector<std::string> items)
{
	this->items = items;
}

int MScrollingList::getSelected(void)
{
	return selected;
}

bool MScrollingList::acceptsFocus(void)
{
	return items.size() > 0;
}


MScrollingList::MScrollingList(int x, int y, int w, int h, void (*drop_callback)(int n), int drop_x, int drop_y, void (*hold_callback)(int n, const void *data), const void *hold_data, bool do_prompt) :
	hold_data(hold_data)
{
	this->x = x;
	this->y = y;
	this->width = w;
	this->height = h;
	this->hotkeys = 0;
	this->selected = -1;
	this->top = 0;
	rows = height / 15;
	up_arrow = m_load_bitmap(getResource("media/up.png"));
	clicked = false;
	down = false;
	downX = -1;
	downY = -1;
	dragging = false;
	down2X = -1;
	down2Y = -1;
	scrolling = false;
	scroll_offs = 0;
	up_selected = -1;
	first_finger_x = -1;
	first_finger_y = -1;
	this->drop_callback = drop_callback;
	this->drop_x = drop_x;
	this->drop_y = drop_y;
	this->hold_callback = hold_callback;
	dragBmp = NULL;
	holdTime = -1;
	this->do_prompt = do_prompt;

#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_ANDROID
	ALLEGRO_MOUSE_STATE state;
	al_get_mouse_state(&state);
	last_z = state.z;
#endif
	scrollwheel_dir = 0;
}



MScrollingList::~MScrollingList(void)
{
	m_destroy_bitmap(up_arrow);
}

void MItemSelector::getDropLocation(int *dx, int *dy)
{
	if (dx) *dx = drop_x;
	if (dy) *dy = drop_y;
}

bool MItemSelector::itemsBelow(void)
{
	if (!battle) return true;
	int i = top + rows*2;
	for (; i < MAX_INVENTORY; i++) {
		if (inventory[i].index >= 0)
			return true;
	}
	return false;
}

void MItemSelector::setFocus(bool f)
{
	if (!f) {
		if (dragging) {
			dragging = false;
			m_destroy_bitmap(dragBmp);
			dragBmp = NULL;
			first_finger_x = -1;
			first_finger_y = -1;
		}
	}
	TGUIWidget::setFocus(f);
}

void MItemSelector::mouseMove(int xx, int yy, int zz)
{
#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_ANDROID
	ALLEGRO_MOUSE_STATE state;
	al_get_mouse_state(&state);
	if (xx >= x && yy >= y && xx < x+width && yy < y+height) {
		int diff = state.z - last_z;
		if (abs(diff) > 0) {
			scrollwheel_dir = -diff;
		}
	}
	last_z = state.z;
#endif

	if (first_finger_x < 0) return;

	int tmpx = xx - first_finger_x;
	int tmpy = yy - first_finger_y;

	if (abs(tmpx) <= 20 && abs(tmpy) <= 20) {
		first_finger_x = xx;
		first_finger_y = yy;
	}

	if (scrolling) {
		int dx = (xx - down2X);
		int dy = (yy - down2Y);

		/* Make sure we have the right finger */
		if (abs(dx) <= 20 && abs(dy) <= 20) {
			scroll_offs += dy;
			down2X = xx;
			down2Y = yy;
		}
	}
}

void MItemSelector::mouseDownAbs(int xx, int yy, int b)
{
	bool clicked_on = xx >= this->x && yy >= this->y &&
		xx < (this->x+this->width) && yy < (this->y+this->height);

	if (clicked_on) {
		xx -= this->x;
		yy -= this->y;
	}
	else {
		return;
	}

	tguiRaiseWidget(this);

	if (growing) return;

	if (scrolling)
		return;
	
	if (dragging)
		return;

#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_ANDROID
	if (b == 2) {
		pressed = -1;
		return;
	}
#endif

	if (down || maybe_scrolling) {
		if (!scrolling && !dragging) {
			down2X = xx+x;
			down2Y = yy+y;
			scrolling = true;
			scroll_offs = 0;
			down = false;
			maybe_scrolling = false;
		}
		return;
	}

	downCount = 0;

	int aw = m_get_bitmap_width(up_arrow)/2;
	int ah = m_get_bitmap_height(up_arrow)/2;
			
	const int arrow_radius = 10;
	int up_x = x + width - aw;
	int up_y = y + ah;
	int udx = (xx+x) - up_x;
	int udy = (yy+y) - up_y;
	int down_x = up_x;
	int down_y = y + height - ah;
	int ddx = (xx+x) - down_x;
	int ddy = (yy+y) - down_y;
	if (sqrtf(udx*udx+udy*udy) < arrow_radius) {
		if (top > 0) {
			playPreloadedSample("blip.ogg");
			top -= 2;
			selected -= 2;
		}
	}
	else if (itemsBelow() && sqrtf(ddx*ddx+ddy*ddy) < arrow_radius) {
		if (top+(rows*2) < MAX_INVENTORY) {
			playPreloadedSample("blip.ogg");
			top += 2;
		}
	}
	else {
		int rows_down = (yy-3) / 15;
		int col = xx / (BW/2);
		int n = top + (rows_down*2) + col;
		if (n >= MAX_INVENTORY) {
			n = top + ((rows_down-1)*2) + col;
		}
		if (inventory[n].index >= 0) {
			selected = n;
		}
		else {
			selected = -1;
		}
		maybe_scrolling = true;
		down = true;
		downX = xx+x;
		downY = yy+y;
		first_finger_x = downX;
		first_finger_y = downY;
		initial_down_x = downX;
		initial_down_y = downY;
	}
}

void MItemSelector::mouseUpAbs(int xx, int yy, int b)
{
	maybe_scrolling = false;

	int upx = xx;
	int upy = yy;

	bool clicked_on = xx >= this->x && yy >= this->y &&
		xx < (this->x+this->width) && yy < (this->y+this->height);

	if (clicked_on) {
		xx -= this->x;
		yy -= this->y;
	}
	else {
		xx = yy = -1;
	}

	if (growing) goto done;

	if (scrolling) {
		scrolling = false;
		down = false;
		goto done;
	}

	if (!down) goto done;
	down = false;

	if (selected < 0)
		return;

	if (xx >= 0 && yy >= 0) {
		if (dragging) {
			if (!isShop) {
				int rows_down = (yy-3) / 15;
				int col = xx / (BW/2);
				int n = top + (rows_down*2) + col;
				if (n >= MAX_INVENTORY)
					n = top + (rows_down-1)*2 + col;
				pressed = n;
				if (pressed == selected) {
					pressed = -1;
					down = false;
				}
				else {
					was_dragged = true;
					clicked = true;
				}
			}
			else {
				pressed = -1;
				down = false;
			}
		}
		else {
			clicked = true;
		}
	}
	else {
		drop_x = upx;
		drop_y = upy;
		clicked = true;
	}

done:
	if (dragging) {
		dragging = false;
		m_destroy_bitmap(dragBmp);
		dragBmp = NULL;
		first_finger_x = -1;
		first_finger_y = -1;
	}
}

void MItemSelector::draw()
{
	mDrawFrame(3, y, BW-6, height);
	int dx;
	int dy = y+3;
	
	m_set_clip(0, dy-4, BW, dy+height-6);

	for (int i = top; i < top+(rows+1)*2 && i < MAX_INVENTORY; i++) {
		Inventory *inv = &inventory[i];
		char s[100];
		MCOLOR color;
		if (inv->index < 0) {
			goto loop;
		}
		if ((i % 2) == 0) {
			dx = 23;
		}
		else {
			dx = BW/2+20;
		}
		sprintf(s, "%s%s", getItemIcon(inv->index).c_str(), _t(getItemName(inv->index).c_str()));
		if (pressed == i) {
			color = m_map_rgb(255, 255, 0);
		}
		else {
			color = grey;
		}
		mTextout(game_font, s, dx, dy,
			color, black,
			WGT_TEXT_DROP_SHADOW, false);
			sprintf(s, "%d", inv->quantity);
			mTextout(game_font, s, dx-20, dy,
				color, black,
				WGT_TEXT_DROP_SHADOW, false);
loop:
		if ((i % 2) == 1)
			dy += 15;
	}

	m_set_clip(0, 0, BW, BH);


	//draw cursor
	if (this == tguiActiveWidget && ((unsigned)tguiCurrentTimeMillis() % 300) < 150) {
		dx = (selected % 2) == 0 ? 3 : (BW/2);
		dx += m_get_bitmap_width(arrow);
		dy = ((selected - top) / 2) * 15 + y;

		int bmph = m_get_bitmap_height(arrow);

		if ((!use_dpad || this == tguiActiveWidget) && selected >= 0) {
			if (dy+bmph/2 > y && dy+bmph/2 < y+height && top <= selected) {
				m_draw_bitmap(arrow, dx, dy, M_FLIP_HORIZONTAL);
			}
		}
	}

	if (dragging) {
		ALLEGRO_MOUSE_STATE state;
		m_get_mouse_state(&state);
		m_save_blender();
		m_set_blender(M_ONE, M_INVERSE_ALPHA, al_map_rgba(128, 128, 128, 128));
		int w = m_get_bitmap_width(dragBmp);
		int h = m_get_bitmap_height(dragBmp);
		m_draw_scaled_bitmap(dragBmp, 0, 0, w, h,
			state.x-w, state.y-h,
			w*2, h*2, 0, 255);
		m_restore_blender();
	}

#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_ANDROID
	//if (this != tguiActiveWidget)
		//return;
#endif

	// draw arrows
	if (top > 0) {
		dx = x + width - m_get_bitmap_width(up_arrow);
		dy = y;
		m_draw_bitmap(up_arrow, dx, dy, 0);
	}
	if (itemsBelow() && top+(rows*2) < MAX_INVENTORY) {
		dx = x + width - m_get_bitmap_width(up_arrow);
		dy = y + height - m_get_bitmap_height(up_arrow);
		m_draw_bitmap(up_arrow, dx, dy, M_FLIP_VERTICAL);
	}
}
					
void MItemSelector::reset(void)
{
	pressed = -1;
	down = false;
	downX = -1;
	downY = -1;
	downCount = 0;
	dragging = false;
	clicked = false;
	drop_x = drop_y = -1;
	if (dragBmp) {
		m_destroy_bitmap(dragBmp);
		dragBmp = NULL;
	}
}

int MItemSelector::update(int millis)
{
	//printf("scrolling=%d dragging=%d\n", scrolling, dragging);
	if (scrolling) {
		int d = scroll_offs / 12;
		if (d) {
			int n = top + d*2;
			if (n < 0) n = 0;
			else if (n+rows*2 >= MAX_INVENTORY) n = MAX_INVENTORY - rows*2;
			top = /*selected =*/ n;
		}
		scroll_offs %= 12;
		return TGUI_CONTINUE;
	}

	if (clicked && (drop_x >= 0 || drop_y >= 0)) {
		playPreloadedSample("select.ogg");
		clicked = false;
		return TGUI_RETURN;
	}

	if (!dragging && down) {
		ALLEGRO_MOUSE_STATE state;
		m_get_mouse_state(&state);
		if ((abs(initial_down_x-first_finger_x) >= 10 || abs(initial_down_y-first_finger_y) >= 10) && selected >= 0 && selected < MAX_INVENTORY && inventory[selected].index >= 0 && (canArrange || isShop)) {
			dragging = true;
			Inventory *inv = &inventory[selected];
			char s[100];
			if (inv->index < 0)
				strcpy(s, _t("<empty>"));
			else
				sprintf(s, "%d %s", inv->quantity, _t(getItemName(inv->index).c_str()));
			int w = m_text_length(game_font, _t(s))+1;
			m_push_target_bitmap();
			dragBmp = m_create_alpha_bitmap(w, m_text_height(game_font)+4); // check
			m_set_target_bitmap(dragBmp);
			m_clear(m_map_rgba(0, 0, 0, 0));
			mTextout(game_font, s, 0, 2,
				grey, black,
				WGT_TEXT_DROP_SHADOW, false);
			m_pop_target_bitmap();
		}
		else if (selected >= 0) {
			if (inventory[selected].index >= 0) {
				downCount += millis;
				if (downCount >= 600) {
					playPreloadedSample("select.ogg");
					int index = inventory[selected].index;
					reset();
					show_item_info_on_flip = index;
					maybe_scrolling = false;
				}
			}
		}
	}
	else if (dragging) {
		if (this == tguiActiveWidget && iphone_shaken(0.1)) {
			iphone_clear_shaken();
			down = false;
			dragging = false;
			m_destroy_bitmap(dragBmp);
			dragBmp = NULL;
			first_finger_x = -1;
			first_finger_y = -1;
			if (area && area->getName() == "tutorial") {
				selected = -1;
				return TGUI_RETURN;
			}
		}
		else {
			ALLEGRO_MOUSE_STATE state;
			m_get_mouse_state(&state);
			int aw = m_get_bitmap_width(up_arrow)/2;
			int ah = m_get_bitmap_height(up_arrow)/2;
			
			const int arrow_radius = 10;
			int up_x = x + width - aw;
			int up_y = y + ah;
			int udx = state.x - up_x;
			int udy = state.y - up_y;
			int down_x = up_x;
			int down_y = y + height - ah;
			int ddx = state.x - down_x;
			int ddy = state.y - down_y;
			if (itemsBelow() && sqrtf(udx*udx+udy*udy) < arrow_radius) {
				if (top > 0) {
					scrollCount += millis;
					if (scrollCount > 300) {
						playPreloadedSample("blip.ogg");
						top -= 2;
						scrollCount = 0;
					}
				}
			}
			else if (sqrtf(ddx*ddx+ddy*ddy) < arrow_radius) {
				if (top+(rows*2) < MAX_INVENTORY) {
					scrollCount += millis;
					if (scrollCount > 300) {
						playPreloadedSample("blip.ogg");
						top += 2;
						scrollCount = 0;
					}
				}
			}
			else
				scrollCount = 0;
		}
	}

	if (growing) {
		grow_count += millis;
		if (grow_count >= 1000) {
			if (growing < 0) {
				height = start_height-20;
				y = start_y+20;
			}
			else {
				height = start_height;
				y = start_y;
				rows += 2;
			}
			growing = 0;
			grow_count = 0;
		}
		else {
			float extra = grow_count/1000.0;
			if (growing > 0) {
				y = (start_y+20) - extra*20;
			}
			else {
				y = start_y + extra*20;
			}
			height = start_height - (y - start_y);
		}
	}

#ifndef ALLEGRO_IPHONE // FIXME: fix when iphone gets keyboard support
	if (canArrange) {
		ALLEGRO_KEYBOARD_STATE st;
		al_get_keyboard_state(&st);
		if (al_key_down(&st, config.getKeySortItems())) {
			playPreloadedSample("select.ogg");
			sortInventory();
			downCount = 0;
			pressed = -1;
			al_rest(1);
		}
	}
#endif

	INPUT_EVENT ie;
	InputDescriptor _id;
	if (this != tguiActiveWidget) {
		ie = EMPTY_INPUT_EVENT;
		_id = EMPTY_INPUT_DESCRIPTOR;
	}
	else {
		ie = get_next_input_event();
		_id = getInput()->getDescriptor();
	}
	
	if (ie.left == DOWN) {
		use_input_event();
		if ((selected % 2) == 1) {
			playPreloadedSample("blip.ogg");
			selected--;
		}
		else {
			playPreloadedSample("blip.ogg");
			tguiFocusPrevious();
		}
	}
	else if (ie.right == DOWN) {
		use_input_event();
		if ((selected % 2) == 0) {
			playPreloadedSample("blip.ogg");
			selected++;
		}
		else {
			playPreloadedSample("blip.ogg");
			tguiFocusNext();
		}
	}
	else if (ie.up == DOWN || scrollwheel_dir < 0) {
		use_input_event();
		if (scrollwheel_dir < 0) {
			scrollwheel_dir++;
			if (top >= 2) {
				top -= 2;
			}
			playPreloadedSample("blip.ogg");
		}
		else if (selected > 1) {
			playPreloadedSample("blip.ogg");
			selected -= 2;
			if (selected < top) {
				top -= 2;
			}
		}
		else {
			playPreloadedSample("blip.ogg");
			tguiFocusPrevious();
		}
	}
	else if (ie.down == DOWN || scrollwheel_dir > 0) {
		use_input_event();
		if (scrollwheel_dir > 0) {
			scrollwheel_dir--;
			if ((top+(rows*2)) < MAX_INVENTORY) {
				top += 2;
			}
			playPreloadedSample("blip.ogg");
		}
		else if (selected < (MAX_INVENTORY-2)) {
			playPreloadedSample("blip.ogg");
			selected += 2;
			if (((selected - top) / 2) >= rows) {
				top += 2;
			}
		}
		else {
			playPreloadedSample("blip.ogg");
			tguiFocusNext();
		}
	}
	else if (ie.button3 == DOWN) {
		use_input_event();
		int index = inventory[selected].index;
		reset();
		if (index >= 0) {
			playPreloadedSample("select.ogg");
			show_item_info_on_flip = index;
		}
		else {
			playPreloadedSample("error.ogg");
		}
	}

	bool play_sound = true;

	if ((_id.button1 || clicked) && pressed < 0) {
		double start = al_get_time();
		while (_id.button1) {
			pump_events();
			_id = getInput()->getDescriptor();
			if (al_get_time()-start > 0.6) {
				if (inventory[selected].index >= 0) {
					playPreloadedSample("select.ogg");
					int index = inventory[selected].index;
					reset();
					show_item_info_on_flip = index;
				}
				else {
					reset();
					playPreloadedSample("error.ogg");
				}
				goto END;
			}
		}
		
		if (inventory[selected].index < 0) {
			playPreloadedSample("error.ogg");
			goto END;
		}

		playPreloadedSample("select.ogg");

		if (!clicked || (!canArrange && !isShop)) {
			pressed = selected;
		}

		if (!canArrange && (!clicked || !isShop)) {
			clicked = true;
			play_sound = false;
		}
		else {
			clicked = false;
		}
	}
	
	if ((_id.button1 || clicked) && pressed >= 0) {
		if (play_sound) {
			playPreloadedSample("select.ogg");
		}

		getInput()->waitForReleaseOr(4, 250);
	
		if (pressed == selected) {
			clicked = false;
			downCount = 0;
			pressed = -1;
			if (!use_dpad) {
				down = false;
				dragging = false;
				if (dragBmp)
					m_destroy_bitmap(dragBmp);
				dragBmp = NULL;
				first_finger_x = -1;
				first_finger_y = -1;
			}
			return TGUI_RETURN;
		}
		// Arrange
		else {
			if (!clicked || was_dragged) {
				was_dragged = false;
				if (clicked) {
					int tmp = pressed;
					pressed = selected;
					selected = tmp;
				}
				// Group
				if (inventory[selected].index == inventory[pressed].index) {
					int q = 99-inventory[selected].quantity;
					if (q > inventory[pressed].quantity) {
						q = inventory[pressed].quantity;
					}
					inventory[selected].quantity += q;
					inventory[pressed].quantity -= q;
					if (inventory[pressed].quantity <= 0) {
						inventory[pressed].index = -1;
						inventory[pressed].quantity = 0;
					}
				}
				// Swap
				else {
					Inventory tmp;
					tmp.index = inventory[pressed].index;
					tmp.quantity = inventory[pressed].quantity;
					inventory[pressed].index = inventory[selected].index;
					inventory[pressed].quantity = inventory[selected].quantity;
					inventory[selected].index = tmp.index;
					inventory[selected].quantity = tmp.quantity;
					//selected = pressed;
				}
				pressed = -1;
			}
			else if (clicked) {
				pressed = selected;
			}
			else {
				pressed = -1;
			}
			clicked = false;
		}
	}
	
END:;

	if (this == tguiActiveWidget && (ie.button2 == DOWN || (!dragging && iphone_shaken(0.1)))) {
		use_input_event();
		iphone_clear_shaken();
		playPreloadedSample("select.ogg");
		if (pressed < 0) {
			downCount = 0;
			selected = -1;
			return TGUI_RETURN;
		}
		else {
			downCount = 0;
			pressed = -1;
		}
	}

	return TGUI_CONTINUE;
}


int MItemSelector::getSelected(void)
{
	return selected;
}


void MItemSelector::setSelected(int s)
{
	selected = s;
}


void MItemSelector::setTop(int t)
{
	top = t;
}


bool MItemSelector::acceptsFocus(void)
{
	return true;
}


MItemSelector::MItemSelector(int y1, int y2, int top, int selected,
	bool canArrange)
{
	this->x = 0;
	this->y = y1;
	this->width = BW;
	this->height = y2-y1;
	this->hotkeys = 0;
	this->y2 = y2;
	this->top = top;
	this->selected = selected;
	this->canArrange = canArrange;
	rows = height / 15;
	pressed = -1;
	arrow = m_load_bitmap(getResource("media/arrow.png"));
	up_arrow = m_load_bitmap(getResource("media/up.png"));
	clicked = false;
	growing = 0;
	grow_count = 0;
	start_height = this->height;
	start_y = this->y;
	down = false;
	downX = -1;
	downY = -1;
	downCount = 0;
	dragging = false;
	scrollCount = 0;
	down2X = -1;
	down2Y = -1;
	scrolling = false;
	scroll_offs = 0;
	first_finger_x = -1;
	first_finger_y = -1;
	isShop = false;
	this->inventory = ::inventory;
	dragBmp = NULL;
	maybe_scrolling = false;
	drop_x = -1;
	drop_y = -1;
	was_dragged = false;

#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_ANDROID
	ALLEGRO_MOUSE_STATE state;
	al_get_mouse_state(&state);
	last_z = state.z;
#endif
	scrollwheel_dir = 0;
}



MItemSelector::~MItemSelector(void)
{
	m_destroy_bitmap(arrow);
	m_destroy_bitmap(up_arrow);
}


void MManSelector::mouseDown(int x, int y, int b)
{
	x += this->x;
	y += this->y;

	for (int i = 0; i < (int)mans.size(); i++) {
		int xx = mans[i].x * TILE_SIZE - area->getOriginX();
		int yy = (mans[i].y+1) * TILE_SIZE - area->getOriginY();
		for (int j = 0; j < 3; j++) {
			if (x > xx && y > (yy+j*TILE_SIZE) && x < (xx+TILE_SIZE) && y < (yy+(j+1)*TILE_SIZE)) {
				holdi = i;
				holdj = j;
				holdTime = 600;
				holdx = x;
				holdy = y;
				return;
			}
		}
	}
}


void MManSelector::mouseUp(int x, int y, int b)
{
	holdTime = 0;

	if (x >= 0 && y >= 0) {
		/* get go pos */
		int xx = mans[6].x * TILE_SIZE - area->getOriginX();
		int yy = (mans[6].y+1) * TILE_SIZE - area->getOriginY();
		if (x > xx && y > yy && x < (xx+TILE_SIZE) && y < (yy+TILE_SIZE)) {
			pos = 6;
			clicked = true;
			return;
		}
		for (int i = 0; i < (int)mans.size(); i++) {
			if (i == 6) continue;
			int xx = mans[i].x * TILE_SIZE - area->getOriginX();
			int yy = (mans[i].y+1) * TILE_SIZE - area->getOriginY();
			for (int j = 0; j < 3; j++) {
				if (x > xx && y > (yy+j*TILE_SIZE) && x < (xx+TILE_SIZE) && y < (yy+(j+1)*TILE_SIZE)) {
					if (!mans[i].dead && !mans[i].used) {
						lua_State *state = area->getLuaState();
						callLua(state, "move_player", "ii>", i, j);
						break;
					}
					else
						loadPlayDestroy("error.ogg");
				}
			}
		}
	}
}


void MManSelector::draw()
{
	MBITMAP *tmp = m_create_bitmap(16, 16);
	ALLEGRO_BITMAP *old_target = al_get_target_bitmap();
	m_set_target_bitmap(tmp);
	al_clear_to_color(al_map_rgba_f(0, 0, 0, 0));
	ALLEGRO_COLOR yellow = al_map_rgb_f(1, 1, 0);
	for (int i = 1; i < 15; i++) {
		m_draw_pixel(i, 1, yellow);
		m_draw_pixel(1, i, yellow);
		m_draw_pixel(i, 14, yellow);
		m_draw_pixel(14, i, yellow);
	}
	al_set_target_bitmap(old_target);

	al_hold_bitmap_drawing(true);

	for (int i = 0; i < (int)mans.size(); i++) {
		if (i == 6) continue;
		int xx = mans[i].x * TILE_SIZE - area->getOriginX();
		int yy = (mans[i].y+1) * TILE_SIZE - area->getOriginY();
		for (int k = 0; k < 3; k++) {
			m_draw_tinted_bitmap(
				tmp, al_map_rgba(alpha, alpha, alpha, alpha),
				xx, yy+k*TILE_SIZE, 0
			);
		}
	}

	al_hold_bitmap_drawing(false);

	m_destroy_bitmap(tmp);

	// Draw arrow
	int xx = mans[pos].x * TILE_SIZE - area->getOriginX() + TILE_SIZE/2 - m_get_bitmap_width(arrow)/2;
	int yy = mans[pos].y * TILE_SIZE - area->getOriginY();
	m_draw_bitmap(arrow, xx, yy, 0);
	
	for (unsigned int i = 0; i < mans.size(); i++) {
		if (mans[i].go) {
			go->draw(mans[i].x*TILE_SIZE-area->getOriginX(), mans[i].y*TILE_SIZE-area->getOriginY()+TILE_SIZE, 0);
			continue;
		}

		int index = i;
		if (index > 6) index--;

		if (mans[i].dead) {
			callLua(area->getLuaState(), "setManSubAnim", "is>", index, "unconscious");
		}
		else if (mans[i].used) {
			callLua(area->getLuaState(), "setManSubAnim", "is>", index, "hurt");
		}
		else {
			callLua(area->getLuaState(), "setManSubAnim", "is>", index, "stand_s");
		}
	}
}


int MManSelector::update(int millis)
{
	if (was_down && getInput()->getDescriptor().button1)
		return TGUI_CONTINUE;
	else
		was_down = false;

	go->update(millis);
	
	lua_State *state = area->getLuaState();

	alpha += alpha_inc * millis;
	if (alpha_inc < 0) {
		if (alpha < 0) {
			alpha = 0;
			alpha_inc = -alpha_inc;
		}
	}
	else {
		if (alpha > 255) {
			alpha = 255;
			alpha_inc = -alpha_inc;
		}
	}

	if (holdTime > 0) {
		holdTime -= millis;
		if (holdTime <= 0) {
			bool used = false;
#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_ANDROID
			ALLEGRO_MOUSE_STATE s;
			m_get_mouse_state(&s);
			if (abs(s.x-holdx) < 10 && abs(s.y-holdy) < 10 && holdi != 6)  {
				if (holdi > 6) holdi--;
				callLua(state, "do_info", "ii>", holdi, holdj);
				used = true;
				holdx = holdy = -1000;
			}
#endif
			if (!used && pos != 6) {
				int p = pos;
				if (p > 6) p--;
				callLua(state, "do_info", "ii>", p, -1);
				clicked = false;
				need_release = false;
			}
			holdTime = 0;
		}
	}


	INPUT_EVENT ie;
	ie = get_next_input_event();

	if (ie.left == DOWN) {
		use_input_event();
		int tmp = pos-1;
		while (tmp >= 0 && (mans[tmp].used || mans[tmp].dead)) {
			tmp--;
		}
		if (tmp >= 0 && !(mans[tmp].used || mans[tmp].dead)) {
			pos = tmp;
			playPreloadedSample("blip.ogg");
		}
		else
			playPreloadedSample("error.ogg");
						
	}
	else if (ie.right == DOWN) {
		use_input_event();
		int tmp = pos+1;
		while (tmp <= (int)(mans.size()-1) && (mans[tmp].used || mans[tmp].dead)) {
			tmp++;
		}
		if (tmp <= (int)(mans.size()-1) && !(mans[tmp].used || mans[tmp].dead)) {
			pos = tmp;
			playPreloadedSample("blip.ogg");
		}
		else
			playPreloadedSample("error.ogg");
	}
	if (ie.button3 == DOWN) {
		use_input_event();
		if (pos != 6) {
			int p = pos;
			if (p > 6) p--;
			callLua(state, "do_info", "ii>", p, -1);
		}
	}

	InputDescriptor id = getInput()->getDescriptor();

	if (!id.button1 && need_release) {
		holdTime = 0;
		clicked = true;
		need_release = false;
	}
	else if ((ie.button1 == DOWN || clicked) && !need_release) {
		use_input_event();
		if (!clicked && holdTime == 0) {
			need_release = true;
			holdTime = 600;
		}
		if (clicked) {
			clicked = false;
			if (mans[pos].go == true) {
				callLua(state, "go", ">i");
				int retval = lua_tonumber(state, -1);
				lua_pop(state, 1);
				if (retval == 0) {
					return TGUI_RETURN;
				}
				else
					return TGUI_CONTINUE;
			}
			else {
				callLua(state, "toggle", "i>", pos);
			}
		}
	}
	else
		return TGUI_CONTINUE;

	return TGUI_CONTINUE;
}


bool MManSelector::acceptsFocus(void)
{
	return true;
}


void MManSelector::mark(int index, bool used, bool dead)
{
	mans[index].used = used;
	mans[index].dead = dead;
}


MManSelector::MManSelector(std::vector<MMan> mans)
{
	this->x = 0;
	this->y = 0;
	this->width = BW;
	this->height = BH;
	this->hotkeys = 0;
	arrow = m_load_bitmap(getResource("media/down_arrow.png"));
	pos = 0;
	this->mans = mans;

	go = new AnimationSet(getResource("media/go.png"));

	alpha = 255;
	alpha_inc = -(255.0/1000.0);

	clicked = false;

	holdTime = 0;

	need_release = false;

	holdx = holdy = -1000;
}


MManSelector::~MManSelector(void)
{
	m_destroy_bitmap(arrow);
	delete go;
}

void MMultiChooser::mouseDownAbs(int x, int y, int b)
{
	down = true;

	for (int i = 0; i < (int)points.size(); i++) {
		int w = m_get_bitmap_width(arrow)/2;
		int dx = (x) - (points[i].x + ((points[i].west) ? w : -w));
		int dy = (y) - points[i].y;
		int d = sqrtf(dx*dx + dy*dy);
		if (d <= 5) {
			return;
		}
	}

	closest = 1;
}


void MMultiChooser::mouseUp(int xx, int yy, int b)
{
	if (!down) {
		return;
	}

	if (closest >= 0) {
		closest = -1;

		/* Missed, find closest */
		int i;

		int found = -1;
		int mindist = 9999;

		for (int i = 0; i < (int)points.size(); i++) {
			int w = m_get_bitmap_width(arrow)/2;
			int dx = (x+xx) - (points[i].x + ((points[i].west) ? w : -w));
			int dy = (y+yy) - points[i].y;
			int d = sqrtf(dx*dx + dy*dy);
			if (d < mindist) {
				found = i;
				mindist = d;
			}
		}

		i = found;

		playPreloadedSample("blip.ogg");
		if (can_multi) {
			if (current.size() > 0 && points[i].west != points[current[0]].west) {
				current.clear();
			}
			std::vector<int>::iterator it = std::find(current.begin(), current.end(), i);
			if (it != current.end()) {
				if (current.size() > 1) {
					it = current.erase(it);
				}
			}
			else {
				current.push_back(i);
			}
		}
		else {
			current.clear();
			current.push_back(i);
		}
		call_callback = true;
		return;
	}

	if (xx >= 0 && yy >= 0) {
		for (int i = 0; i < (int)points.size(); i++) {
			int w = m_get_bitmap_width(arrow)/2;
			int dx = (x+xx) - (points[i].x + ((points[i].west) ? w : -w));
			int dy = (y+yy) - points[i].y;
			int d = sqrtf(dx*dx + dy*dy);
			if (d <= 5) {
				playPreloadedSample("blip.ogg");
				if (can_multi) {
					if (current.size() > 0 && points[i].west != points[current[0]].west) {
						current.clear();
					}
					std::vector<int>::iterator it = std::find(current.begin(), current.end(), i);
					if (it != current.end()) {
						if (current.size() > 1) {
							it = current.erase(it);
						}
					}
					else {
						current.push_back(i);
					}
				}
				else {
					current.clear();
					current.push_back(i);
				}
				call_callback = true;
				break;
			}
		}
	}
}

void MMultiChooser::draw()
{
	static bool show = false;
	show = !show;

	for (unsigned int i = 0; i < points.size(); i++) {
		m_save_blender();
		int alpha = 100;
		std::vector<int>::iterator it = std::find(current.begin(), current.end(), i);
		if (it != current.end()) {
			alpha = 255;
		}
		m_set_blender(ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA,
			m_map_rgba(alpha, alpha, alpha, alpha));

		int flags;
		int draw_x, draw_y;
		MultiPoint *p = &points[i];

		if (p->west) {
			flags = 0;
			draw_x = (int)p->x;
		}
		else {
			int bmp_w = m_get_bitmap_width(arrow);
			flags = M_FLIP_HORIZONTAL;
			draw_x = (int)(p->x - bmp_w);
		}
		int bmp_h = m_get_bitmap_height(arrow);
		draw_y = (int)(p->y - (bmp_h/2));

		if (inset) {
			if (alpha == 255) {
				draw_x -= m_get_bitmap_width(arrow);
			}
			else {
				alpha = 0;
			}
		}
		if (current.size() > 1 && points[current[0]].west == p->west && alpha == 255) {
			if (show) {
				m_draw_bitmap(arrow, draw_x, draw_y, flags);
			}
		}
		else
		{
			m_draw_bitmap(arrow, draw_x, draw_y, flags);
		}

		m_restore_blender();
	}
}

int MMultiChooser::getTapped()
{
	if (call_callback)
		return current[0];
	else
		return -1;
}

void MMultiChooser::setTapped(bool t)
{
	call_callback = false;
}

int MMultiChooser::update(int millis)
{
	std::vector<int> possibilities;
	bool want_all = false;

	INPUT_EVENT ie;

	if (this != tguiActiveWidget)
		ie = EMPTY_INPUT_EVENT;
	else
		ie = get_next_input_event();

	if (ie.up == DOWN && current.size() <= 1) {
		use_input_event();
		playPreloadedSample("blip.ogg");
		// reverse order hack
		if (points.size() >= 1) {
			for (int i = points.size()-1; i >= 0; i--) {
				if (points[i].y < points[current[0]].y &&
					points[i].west == points[current[0]].west) {
					possibilities.push_back(i);
				}
			}
		}
	}
	else if (ie.down == DOWN && current.size() <= 1) {
		use_input_event();
		playPreloadedSample("blip.ogg");
		for (int i = 0; i < (int)points.size(); i++) {
			if (points[i].y > points[current[0]].y &&
				points[i].west == points[current[0]].west) {
				possibilities.push_back(i);
			}
		}
	}
	else if (ie.left == DOWN) {
		use_input_event();
		playPreloadedSample("blip.ogg");
		if (current.size() <= 1) {
			for (int i = 0; i < (int)points.size(); i++) {
				if (points[i].x < points[current[0]].x) {
					possibilities.push_back(i);
				}
			}
		}
		// unselect all
		else if (can_multi) {
			int min = BW;
			int index = 0;
			for (int i = 0; i < (int)points.size(); i++) {
				if (points[i].x < min) {
					min = (int)points[i].x;
					index = i;
				}
			}
			possibilities.push_back(index);
		}
		if (possibilities.size() <= 0 && can_multi) {
			for (int i = 0; i < (int)points.size(); i++) {
				if (points[i].west) {
					possibilities.push_back(i);
				}
			}
			want_all = true;
		}
	}
	else if (ie.right == DOWN) {
		use_input_event();
		playPreloadedSample("blip.ogg");
		if (current.size() <= 1) {
			for (int i = 0; i < (int)points.size(); i++) {
				if (points[i].x > points[current[0]].x) {
					possibilities.push_back(i);
				}
			}
		}
		// unselect all
		else if (can_multi) {
			int max = 0;
			int index = 0;
			for (int i = 0; i < (int)points.size(); i++) {
				if (points[i].x > max) {
					max = (int)points[i].x;
					index = i;
				}
			}
			possibilities.push_back(index);
		}
		if (possibilities.size() <= 0 && can_multi) {
			for (int i = 0; i < (int)points.size(); i++) {
				if (!points[i].west) {
					possibilities.push_back(i);
				}
			}
			want_all = true;
		}
	}
	else if (ie.button1 == DOWN) {
		use_input_event();
		call_callback = true;
		playPreloadedSample("select.ogg");
		return TGUI_RETURN;
	}
	else if (ie.button2 == DOWN || iphone_shaken(0.1)) {
		use_input_event();
		iphone_clear_shaken();
		playPreloadedSample("select.ogg");
		current.clear();
		return TGUI_RETURN;
	}

	if (possibilities.size() > 0) {
		int curr = current[0];
		current.clear();

		if (want_all) {
			for (int i = 0; i < (int)possibilities.size(); i++) {
				current.push_back(possibilities[i]);
			}
		}
		else {
			int closest = 0;
			int closest_dist = INT_MAX;
			for (int i = 0; i < (int)possibilities.size(); i++) {
				int dx = points[possibilities[i]].x - points[curr].x;
				int dy = points[possibilities[i]].y - points[curr].y;
				int dist = (int)fabs(sqrt((float)(dx*dx + dy*dy)));
				if (dist < closest_dist) {
					closest = possibilities[i];
					closest_dist = dist;
				}
			}
			current.push_back(closest);
		}

		possibilities.clear();
	}

	if (!use_dpad) {
		if (current.size() > 0) {
			IPHONE_LINE_DIR dir;
			IPHONE_LINE_DIR opposite;
			if (points[current[0]].west) {
				dir = IPHONE_LINE_DIR_WEST;
				opposite = IPHONE_LINE_DIR_EAST;
			}
			else {
				dir = IPHONE_LINE_DIR_EAST;
				opposite = IPHONE_LINE_DIR_WEST;
			}
			if (iphone_line(dir, 0.1)) {
				iphone_clear_line(dir);
				playPreloadedSample("select.ogg");
				return TGUI_RETURN;
			}
			else if (iphone_line(opposite, 0.1)) {
				iphone_clear_line(opposite);
				for (size_t i = 0; i < current.size(); i++) {
					current[i] = -current[i] - 1;
				}
				return TGUI_RETURN;
			}
			else if (this == tguiActiveWidget && iphone_shaken(0.1)) {
				iphone_clear_shaken();
				playPreloadedSample("blip.ogg");
				current.clear();
				return TGUI_RETURN;
 			}
		}
	}
	
	return TGUI_CONTINUE;
}


std::vector<int> &MMultiChooser::getSelected(void)
{
	return current;
}

void MMultiChooser::setSelected(std::vector<int> sel)
{
	current.clear();
	for (int i = 0; i < (int)sel.size(); i++) {
		current.push_back(sel[i]);
	}
}

bool MMultiChooser::acceptsFocus(void)
{
	return true;
}

void MMultiChooser::setInset(bool i) {
	inset = i;
}


MMultiChooser::MMultiChooser(std::vector<MultiPoint> points, bool can_multi)
{
   this->points = points;
	this->x = 0;
	this->y = 0;
	this->width = BW;
	this->height = BH;
	this->hotkeys = 0;
	arrow = m_load_bitmap(getResource("media/arrow.png"));
	current.push_back(0);
	this->can_multi = can_multi;
	call_callback = false;
	closest = -1;
	inset = false;
	down = false;
}


MMultiChooser::~MMultiChooser(void)
{
	m_destroy_bitmap(arrow);
	current.clear();
}

bool MPartySelector::didDragSomething()
{
	return draggedSomething;
}

void MPartySelector::setFocus(bool f)
{
	TGUIWidget::setFocus(f);
}

void MPartySelector::mouseDown(int xx, int yy, int b)
{
	downCount = 0;
	downX = x+xx;
	downY = y+yy;
	down = true;
	tguiRaiseWidget(this);
}

void MPartySelector::mouseUp(int x, int y, int b)
{
	if (growing) return;

	if (!down) return;

	down = false;

	if (x >= 0 && y >= 0) {
		if (x < 20)
			left_half_clicked = true;
		else if (x > (BW-20))
			right_half_clicked = true;
	}
	else if (dragging) {
		draggedSomething = true;
		done = true;
	}

	if (dragging) {
		m_destroy_bitmap(dragBmp);
		dragBmp = NULL;
		dragging = false;
	}
}

void MPartySelector::draw()
{
	mDrawFrame(3, y, BW-6, height);

	if (index >= 0 && index < MAX_PARTY && party[index]) {
		drawSimpleStatus(index, 5, y);
		drawSimpleEquipment(index, BW/2+10, y+2, 69); //height);
		CombatantInfo &info = party[index]->getInfo();
		char text[100];
		if (info.abilities.hp <= 0) {
			strcpy(text, _t("Unconscious"));
		}
		else {
			std::string cond = getConditionName(info.condition);
			sprintf(text, "%s", cond == "" ? _t("Normal") : _t(cond.c_str()));
		}
		mTextout(game_font, text, 5, y+40,
			grey, black,
			WGT_TEXT_DROP_SHADOW, false);
	}
	else {
		if (index >= MAX_PARTY) {
			mTextout(game_font, _t("Trash"), BW/2, y+height/2,
				grey, black,
				WGT_TEXT_DROP_SHADOW, true);
		}
		else {
			mTextout(game_font, _t("Empty"), BW/2, y+height/2,
				grey, black,
				WGT_TEXT_DROP_SHADOW, true);
		}
	}

#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_ANDROID
	if (true) {
#else
	if (!use_dpad || this == tguiActiveWidget) {
#endif
		if (((unsigned)tguiCurrentTimeMillis() % 300) < 150) {
			int w = m_get_bitmap_width(arrow);
			int h = m_get_bitmap_height(arrow);
			m_draw_bitmap(arrow, 0, y+height/2-h/2, 0);
			m_draw_bitmap(arrow, BW-w, y+height/2-h/2,
				M_FLIP_HORIZONTAL);
		}
	}

	if (dragging) {
		ALLEGRO_MOUSE_STATE state;
		m_get_mouse_state(&state);
		m_save_blender();
		m_set_blender(M_ONE, M_INVERSE_ALPHA, al_map_rgba(128, 128, 128, 128));
		int w = m_get_bitmap_width(dragBmp);
		int h = m_get_bitmap_height(dragBmp);
		m_draw_scaled_bitmap(dragBmp, 0, 0, w, h,
			state.x-w, state.y-h,
			w*2, h*2, 0, 255);
		m_restore_blender();
	}
}


void MPartySelector::next(void)
{
	do {
		index++;
		int max = show_trash ? MAX_PARTY : MAX_PARTY-1;
		if (index > max) {
			index = 0;
		}
		else if (index == MAX_PARTY)
			break;
		if (party[index]) break;
	} while (1);
}


int MPartySelector::update(int millis)
{
	if (growing) {
		grow_count += millis;
		if (grow_count >= 1000) {
			if (growing > 0)
				height = 72;
			else
				height = 52;
			growing = 0;
			grow_count = 0;
		}
		else {
			float extra = grow_count/1000.0;
			if (growing > 0)
				height = 52 + extra*20;
			else
				height = 72 - extra*20;
		}
	}

	if (dragging) {
	}
	else if (down && show_trash) {
		ALLEGRO_MOUSE_STATE mstate;
		m_get_mouse_state(&mstate);

		if ((abs(mstate.x-downX) >= 10 || abs(mstate.y-downY) >= 10) && index != MAX_PARTY) {
			int tmp = (downY - 3 - y) / ((69/*height-5*/)/5);
			if (tmp >= 0 && tmp < 5) {
				equipIndex = tmp;
				int *e;
				switch (equipIndex) {
					case 0:
						e = &party[index]->getInfo().equipment.lhand;
						break;
					case 1:
						e = &party[index]->getInfo().equipment.rhand;
						break;
					case 2:
						e = &party[index]->getInfo().equipment.harmor;
						break;
					case 3:
						e = &party[index]->getInfo().equipment.carmor;
						break;
					default:
						e = &party[index]->getInfo().equipment.farmor;
						break;
				}
				if (*e != -1) {
					dragging = true;
					char s[100];
					sprintf(s, "%s", getItemName(*e).c_str()
							  );
					int w = m_text_length(game_font, _t(s))+1;
					m_push_target_bitmap();
					dragBmp = m_create_alpha_bitmap(w, m_text_height(game_font)+4); // check
					m_set_target_bitmap(dragBmp);
					m_clear(m_map_rgba(0, 0, 0, 0));
					mTextout(game_font, _t(s), 0, 2,
						grey, black,
						WGT_TEXT_DROP_SHADOW, false);
					m_pop_target_bitmap();
				}
			}
		}
		else if (index != MAX_PARTY) {
			downCount += millis;
			if (downCount >= 600) {
				bool go = false;
				int chosen = -1;
				if (abs(mstate.x-downX) <= 10 && abs(mstate.y-downY) <= 10) {
					chosen = (downY - 3 - y) / ((height-5)/5);
					if (chosen >= 0 && chosen < 5)
						go = true;
					
				}
				if (party[index] && go) {
					int *toUnequip = 0;
					switch (chosen) {
						case 0:
							toUnequip = &party[index]->getInfo().equipment.lhand;
							break;
						case 1:
							toUnequip = &party[index]->getInfo().equipment.rhand;
							break;
						case 2:
							toUnequip = &party[index]->getInfo().equipment.harmor;
							break;
						case 3:
							toUnequip = &party[index]->getInfo().equipment.carmor;
							break;
						case 4:
							toUnequip = &party[index]->getInfo().equipment.farmor;
							break;
					}
					if (*toUnequip >= 0) {
						down = false;
						playPreloadedSample("select.ogg");
						show_item_info_on_flip = index;
					}
				}
				down = false;
				downX = -1;
				downY = -1;
				downCount = 0;
			}
		}
	}

	if (this == tguiActiveWidget) {
		INPUT_EVENT ie;
		if (this != tguiActiveWidget)
			ie = EMPTY_INPUT_EVENT;
		else
			ie = get_next_input_event();

		if (ie.left == DOWN || left_half_clicked) {
			use_input_event();
			left_half_clicked = false;
			playPreloadedSample("blip.ogg");
			do {
				index--;
				if (index < 0) {
					if (show_trash) {
						index = MAX_PARTY;
						break;
					}
					else {
						index = MAX_PARTY-1;
					}
				}
				if (party[index]) break;
			} while (1);
		}
		else if (ie.right == DOWN || right_half_clicked) {
			use_input_event();
			right_half_clicked = false;
			playPreloadedSample("blip.ogg");
			next();
		}
		else if (ie.up == DOWN) {
			use_input_event();
			playPreloadedSample("blip.ogg");
			tguiFocusPrevious();
		}
		else if (ie.down == DOWN) {
			use_input_event();
			playPreloadedSample("blip.ogg");
			tguiFocusNext();
		}
		else if (ie.button1 == DOWN) {
			use_input_event();
			playPreloadedSample("select.ogg");
			return TGUI_RETURN;
		}
		else if (ie.button2 == DOWN || iphone_shaken(0.1)) {
			use_input_event();
			iphone_clear_shaken();
			if (dragging) {
				dragging = false;
				m_destroy_bitmap(dragBmp);
				dragBmp = NULL;
				down = false;
			}
			else {
				playPreloadedSample("select.ogg");
				index = -1;
				return TGUI_RETURN;
			}
		}
	}

	if (done) {
		done = false;
		return TGUI_RETURN;
	}

	draggedSomething = false;

	return TGUI_CONTINUE;
}

bool MPartySelector::acceptsFocus(void)
{
	return true;
}



MPartySelector::MPartySelector(int y, int index, bool show_trash)
{
	this->x = 0;
	this->y = y;
	this->width = BW;
	this->height = 65;
	this->hotkeys = 0;
	this->index = index;
	this->show_trash = show_trash;
	arrow = m_load_bitmap(getResource("media/arrow.png"));
	debug_message("arrow = %p\n", arrow);
	left_half_clicked = false;
	right_half_clicked = false;
	growing = 0;
	grow_count = 0;
	dragging = false;
	equipIndex = 0;
	dragBmp = NULL;
	done = false;
	down = false;
	draggedSomething = false;

	if (index != MAX_PARTY && !party[index]) {
		next();
	}
}


MPartySelector::~MPartySelector(void)
{
	m_destroy_bitmap(arrow);
	if (dragBmp) {
		m_destroy_bitmap(dragBmp);
	}
}


void MStateSelector::draw()
{
	m_draw_rectangle(0, 0, BW, BH, blue, M_FILLED);

}


int MStateSelector::update(int millis)
{
	return TGUI_CONTINUE;
}


MStateSelector::MStateSelector(void)
{
	this->x = 0;
	this->y = 0;
	this->width = BW;
	this->height = BH;
	this->hotkeys = 0;
}


MStateSelector::~MStateSelector(void)
{
}




void MParty::draw()
{
	for (int i = 0; i < MAX_PARTY; i++) {
		if (party[i]) {
			int dx;
			if (party[i]->getFormation() == FORMATION_FRONT) {
				dx = 5;
			}
			else {
				dx = 15;
			}
			drawSimpleStatus(i, dx, 5+(i*37));
		}
	}
}


int MParty::update(int millis)
{
	return TGUI_CONTINUE;
}


MParty::MParty(void)
{
	this->x = 0;
	this->y = 0;
	this->width = BW/2;
	this->height = BH;
	this->hotkeys = 0;
}


MParty::~MParty(void)
{
}


void MIcon::pre_draw()
{
}

void MIcon::draw()
{
	m_draw_tinted_bitmap(bitmap, tint, x, y, 0);
	if (this == tguiActiveWidget && show_focus) {
		m_save_blender();
		al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ONE);
		double t = fmod(al_get_time(), 1.0);
		if (t >= 0.5) t = 0.5 - (t-0.5);
		float a = t / 0.5;
		al_draw_tinted_bitmap(bitmap->bitmap, al_map_rgba_f(a, a, a, a), x, y, 0);
		m_restore_blender();
	}
	if (down && show_name) {
		m_draw_rectangle(
			x+m_get_bitmap_width(bitmap)+10,
			y-25,
			x+112+10,
			y-10,
			black, M_FILLED);
		m_draw_rectangle(
			x+m_get_bitmap_width(bitmap)+10+0.5,
			y-25+0.5,
			x+112+10+0.5,
			y-10+0.5,
			white, 0);

		mTextout_simple(_t(name), x+m_get_bitmap_width(bitmap)+6+10, y-7-m_text_height(game_font)/2-9,
			white);
	}
}


int MIcon::update(int millis)
{
	if (should_return) {
		lastDown = al_get_time();
		should_return = false;
		return TGUI_RETURN;
	}
	if (mouse_is_down && lastDown+0.33 < al_get_time() && generate_repeat_presses) {
		playPreloadedSample("select.ogg");
		lastDown = al_get_time();
		return TGUI_RETURN;
	}

	if (this == tguiActiveWidget) {
		INPUT_EVENT ie = get_next_input_event();
		if (ie.button1 == DOWN) {
			use_input_event();
			playPreloadedSample("select.ogg");
			return TGUI_RETURN;
		}
		else if (ie.left == DOWN) {
			use_input_event();
			playPreloadedSample("blip.ogg");
			if (left_widget)
				tguiSetFocus(left_widget);
			else
				tguiFocusPrevious();
		}
		else if (ie.up == DOWN) {
			use_input_event();
			playPreloadedSample("blip.ogg");
			tguiFocusPrevious();
		}
		else if (ie.right == DOWN) {
			use_input_event();
			playPreloadedSample("blip.ogg");
			if (right_widget)
				tguiSetFocus(right_widget);
			else
				tguiFocusNext();

		}
		else if (ie.down == DOWN) {
			use_input_event();
			playPreloadedSample("blip.ogg");
			tguiFocusNext();
		}
	}

	return TGUI_CONTINUE;
}


MIcon::MIcon(int x, int y, std::string filename, MCOLOR tint, bool accFocus,
	const char *name, bool show_name, bool alpha_image, bool show_focus, bool return_on_mouse_down,
	bool generate_repeat_presses)
{
	this->x = x;
	this->y = y;

	this->return_on_mouse_down = return_on_mouse_down;

	if (alpha_image)
		bitmap = m_load_alpha_bitmap(filename.c_str());
	else
		bitmap = m_load_bitmap(filename.c_str());

	this->width = m_get_bitmap_width(bitmap);
	this->height = m_get_bitmap_height(bitmap);

	this->hotkeys = 0;

	this->tint = tint;

	this->accFocus = accFocus;

	this->should_return = false;

	this->down = false;

	this->name = name;
	this->show_name = show_name;
	
	this->show_focus = show_focus;
	
	this->generate_repeat_presses = generate_repeat_presses;

	mouse_is_down = false;

	left_widget = right_widget = NULL;
}


MIcon::~MIcon(void) {
	m_destroy_bitmap(bitmap);
}


void MSingleToggle::mouseDown(int mx, int my, int b)
{
	playPreloadedSample("blip.ogg");
	selected++;
	if (selected >= (int)options.size())
		selected = 0;
}


void MSingleToggle::draw(void)
{
	MCOLOR c;

	if (this == tguiActiveWidget) {
		c = m_map_rgb(255, 255, 0);
	}
	else {
		c = grey;
	}

	mTextout(game_font, _t(options[selected].c_str()), x, y,
		c, black,
		megashadow ? WGT_TEXT_SQUARE_BORDER : WGT_TEXT_DROP_SHADOW,
		false);
}


int MSingleToggle::update(int millis)
{
	width = m_text_length(game_font, _t(options[selected].c_str()));

	if (this != tguiActiveWidget) {
		return TGUI_CONTINUE;
	}

	INPUT_EVENT ie = get_next_input_event();

	if (ie.left == DOWN) {
		use_input_event();
		playPreloadedSample("blip.ogg");
		selected--;
		if (selected < 0) selected = options.size()-1;
	}
	else if (ie.right == DOWN) {
		use_input_event();
		playPreloadedSample("blip.ogg");
		selected++;
		if (selected == (int)options.size()) selected = 0;
	}
	else if (ie.up == DOWN) {
		use_input_event();
		playPreloadedSample("blip.ogg");
		tguiFocusPrevious();
	}
	else if (ie.down == DOWN) {
		use_input_event();
		playPreloadedSample("blip.ogg");
		tguiFocusNext();
	}

	return TGUI_CONTINUE;
}


bool MSingleToggle::acceptsFocus(void)
{
	return true;
}


int MSingleToggle::getSelected(void)
{
	return selected;
}

void MSingleToggle::setSelected(int sel)
{
	selected = sel;
}


MSingleToggle::MSingleToggle(int x, int y, std::vector<std::string> options, bool megashadow)
{
	this->x = x;
	this->y = y;
	int max = 0;
	for (int i = 0; i < (int)options.size(); i++) {
		int len = m_text_length(game_font, _t(options[i].c_str()));
		if (len > max)
			max = len;
	}
	this->width = max;
	this->height = m_text_height(game_font)+2;
	this->options = options;
	this->hotkeys = 0;
	selected = 0;
	this->megashadow = megashadow;
}
	
	
MSingleToggle::~MSingleToggle(void)
{
}


void MDoubleToggle::mouseUp(int mx, int my, int b)
{
	int w1 = m_text_length(game_font, _t(text1.c_str()));
	
	if (mx >= 0 && my >= 0) {
		if (mx < w1) {
			selected = 0;
		}
		else {
			selected = 1;
		}
	}
}


void MDoubleToggle::draw(void)
{
	MCOLOR color1;
	MCOLOR color2;

	int w1 = m_text_length(game_font, _t(text1.c_str()));
	int w2 = m_text_length(game_font, _t(text2.c_str()));
	int h = m_text_height(game_font);
	int x2 = x + w1 + 15;	

	if (selected == 0) {
		color1 = m_map_rgb(255,255,0);
		color2 = m_map_rgb(255,255,255);
		if (((unsigned)tguiCurrentTimeMillis() % 1000) < 500 && this == tguiActiveWidget)
			m_draw_rectangle(x, y, x+w1+10, y+h, m_map_rgb(50,50,50),M_FILLED);
	}
	else {
		color2 = m_map_rgb(255,255,0);
		color1 = m_map_rgb(255,255,255);
		if (((unsigned)tguiCurrentTimeMillis() % 1000) < 500 && this == tguiActiveWidget)
			m_draw_rectangle(x2, y, x2+w2+10, y+h, m_map_rgb(50,50,50),M_FILLED);
	}

	mTextout(game_font, _t(text1.c_str()), x+5, y,
		color1, black,
		WGT_TEXT_DROP_SHADOW, false);
	
	mTextout(game_font, _t(text2.c_str()), x2+5, y,
		color2, black,
		WGT_TEXT_DROP_SHADOW, false);
	
}


int MDoubleToggle::update(int millis)
{
	if (this != tguiActiveWidget) {
		return TGUI_CONTINUE;
	}

	INPUT_EVENT ie = get_next_input_event();

	if (ie.left == DOWN && selected) {
		use_input_event();
		playPreloadedSample("blip.ogg");
		selected = 0;
	}
	else if (ie.right == DOWN && !selected) {
		use_input_event();
		playPreloadedSample("blip.ogg");
		selected = 1;
	}
	else if (ie.up == DOWN) {
		use_input_event();
		playPreloadedSample("blip.ogg");
		tguiFocusPrevious();
	}
	else if (ie.down == DOWN) {
		use_input_event();
		playPreloadedSample("blip.ogg");
		tguiFocusNext();
	}

	return TGUI_CONTINUE;
}


bool MDoubleToggle::acceptsFocus(void)
{
	return true;
}


int MDoubleToggle::getSelected(void)
{
	return selected;
}

void MDoubleToggle::setSelected(int sel)
{
	selected = sel;
}


MDoubleToggle::MDoubleToggle(int x, int y, std::string text1,
		std::string text2)
{
	this->x = x;
	this->y = y;
	this->width = m_text_length(game_font, _t(text1.c_str())) + m_text_length(game_font, _t(text2.c_str())) + 15;
	this->height = m_text_height(game_font);
	this->text1 = text1;
	this->text2 = text2;
	this->hotkeys = 0;
	selected = 0;
}
	
	
MDoubleToggle::~MDoubleToggle(void)
{
}


void MShop::mouseUp(int mx, int my, int b)
{
	if (!(mx >= 0 && my >= 0))
		return;

	int yy = y;

	if (top != 0 && (mx+x) > x-3 && (mx+x) < x+m_get_bitmap_width(up)+3 && (my+y) > yy-3 && (my+y) < yy+m_get_bitmap_height(up)+3) {
		up_clicked = true;
		return;
	}
	
	yy += 10*5;
	
	if (top != nItems-6 && (mx+x) > x-3 && (mx+x) < x+m_get_bitmap_width(up)+3 && (my+y) > yy-3 && (my+y) < yy+m_get_bitmap_height(up)+3) {
		down_clicked = true;
		return;
	}
	
	yy = y;
	
	for (int i = 0; i < 6; i++) {
		int r = top + i;
		if (r >= nItems)
			break;
		if ((i == 0 && top != 0) || (i == 5 && r < nItems-1)) {
			yy += 10;
			continue;
		}
		int x1 = 160;
		int x2 = 200;
		if ((mx+x) < x1 && (my+y) > yy && (my+y) < yy+10) {
			row = r;
			col = 0;
			return;
		}
		if ((mx+x) > x1 && (mx+x) < x1+30 && (my+y) > yy && (my+y) < yy+10) {
			row = r;
			col = 0;
			clicked = true;
			return;
		}
		if ((mx+x) > x2 && (mx+x) < x2+50 && (my+y) > yy && (my+y) < yy+10) {
			row = r;
			col = 1;
			clicked = true;
			return;
		}
		yy += 10;
	}
}


void MShop::draw(void)
{
	int yy = y;

	for (int i = 0; i < 6; i++) {
		int r = top + i;
		if (r >= nItems)
			break;
		if (i == 0 && r != 0) {
#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_ANDROID
			if (row == r && this == tguiActiveWidget) {
				if ((unsigned)tguiCurrentTimeMillis() % 600 < 300) {
					m_draw_bitmap(up, x, yy, 0);
				}
			}
			else {
#endif
				m_draw_bitmap(up, x, yy, 0);
#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_ANDROID
			}
#endif
		}
		else if (i == 5 && r < (nItems-1)) {
#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_ANDROID
			if (row == r && this == tguiActiveWidget) {
				if ((unsigned)tguiCurrentTimeMillis() % 600 < 300) {
					m_draw_bitmap(up, x, yy, M_FLIP_VERTICAL);
				}
			}
			else {
#endif
				m_draw_bitmap(up, x, yy, M_FLIP_VERTICAL);
#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_ANDROID
			}
#endif
		}
		else {
			MCOLOR color;
			if (row == r && this == tguiActiveWidget) {
				color = m_map_rgb(255,255,0);
				int xx;
				if (col == 0) {
					xx = 160 - 12; 
				}
				else {
					xx = 200 - 12;
				}

				if ((((unsigned)tguiCurrentTimeMillis()) % 1000) < 800)
					m_draw_bitmap(cursor, xx, yy, 0);
			}
			else {
				color = m_map_rgb(220,220,220);
			}
			mTextout_simple((getItemIcon(things[r])+
				std::string(_t(items[things[r]].name.c_str()))).c_str(),
				10, yy, color);
			char c[32];
			sprintf(c, "%d", costs[r]);
			mTextout_simple(c, 100, yy, color);
			mTextout_simple(_t("Buy"), 160, yy, color);
			mTextout_simple(_t("Info"), 200, yy, color);
		}
		yy += 10;
	}

	bool can_use[MAX_PARTY] = { false, };

	ItemType type = items[things[row]].type;

	if (type == ITEM_TYPE_STATUS) {
		for (int i = 0; i < MAX_PARTY; i++) {
			if (!party[i])
				continue;
			can_use[i] = true;
		}
	}
	else if (type == ITEM_TYPE_WEAPON) {
		for (int i = 0; i < MAX_PARTY; i++) {
			if (!party[i])
				continue;
			CombatantInfo &info = party[i]->getInfo();
			if (info.characterClass & weapons[items[things[row]].id].classes) {
				can_use[i] = true;
			}
		}
	}
	else if (type == ITEM_TYPE_HEAD_ARMOR) {
		for (int i = 0; i < MAX_PARTY; i++) {
			if (!party[i])
				continue;
			CombatantInfo &info = party[i]->getInfo();
			if (info.characterClass & helmets[items[things[row]].id].classes) {
				can_use[i] = true;
			}
		}
	}
	else if (type == ITEM_TYPE_CHEST_ARMOR) {
		for (int i = 0; i < MAX_PARTY; i++) {
			if (!party[i])
				continue;
			CombatantInfo &info = party[i]->getInfo();
			if (info.characterClass & chestArmors[items[things[row]].id].classes) {
				can_use[i] = true;
			}
		}
	}
	else if (type == ITEM_TYPE_FEET_ARMOR) {
		for (int i = 0; i < MAX_PARTY; i++) {
			if (!party[i])
				continue;
			CombatantInfo &info = party[i]->getInfo();
			if (info.characterClass & feetArmors[items[things[row]].id].classes) {
				can_use[i] = true;
			}
		}
	}

	if (this == tguiActiveWidget && !(top != 0 && row == top) && !(row == top+5 && row != nItems-1)) {
		for (int i = 0; i < MAX_PARTY; i++) {
			if (can_use[i] && partyAnims[i]) {
				partyAnims[i]->draw(BW-10-16*MAX_PARTY+i*16, 130, 0);
			}
		}
	}
}

int MShop::update(int millis)
{
	if (this != tguiActiveWidget)
	{
		return TGUI_CONTINUE;
	}

	INPUT_EVENT ie = get_next_input_event();

	if (ie.up == DOWN && row == top) {
		use_input_event();
		playPreloadedSample("blip.ogg");
		tguiFocusPrevious();
	}
	else if (ie.up == DOWN) {
		use_input_event();
		playPreloadedSample("blip.ogg");
		row--;
	}
	else if (ie.down == DOWN && ((top+5 == row) || (row >= nItems-1))) {
		use_input_event();
		playPreloadedSample("blip.ogg");
		tguiFocusNext();
	}
	else if (ie.down == DOWN) {
		use_input_event();
		playPreloadedSample("blip.ogg");
		row++;
	}
	else if (ie.button1 == DOWN || clicked) {
		use_input_event();
		playPreloadedSample("select.ogg");
		if (row != 0 && row == top && ie.button1 == DOWN) {
			top--;
			row--;
		}
		else if (row != nItems-1 && row == top+5 && ie.button1 == DOWN) {
			top++;
			row++;
		}
		else if (ie.button1 == DOWN || clicked) {
			clicked = false;
			return TGUI_RETURN;
		}
	}
	else if (up_clicked && top != 0) {
		playPreloadedSample("select.ogg");
		up_clicked = false;
		row--;
		top--;
	}
	else if (down_clicked && top != nItems-6) {
		playPreloadedSample("select.ogg");
		down_clicked = false;
		row++;
		top++;
	}
	else if (ie.left == DOWN) {
		use_input_event();
		playPreloadedSample("blip.ogg");
		col = 0;
	}
	else if (ie.right == DOWN) {
		use_input_event();
		playPreloadedSample("blip.ogg");
		col = 1;
	}

	
	return TGUI_CONTINUE;
}

bool MShop::acceptsFocus(void)
{
	return true;
}

int MShop::getRow(void)
{
	return row;
}

void MShop::setRow(int row)
{
	this->row = row;
}

int MShop::getCol(void)
{
	return col;
}

void MShop::setCol(int col)
{
	this->col = col;
}

MShop::MShop(int x, int y, int nItems, int *things, int *costs)
{
	width = BW;
	height = 125;
	this->x = x;
	this->y = y;
	this->nItems = nItems;
	this->things = things;
	this->costs = costs;
	this->hotkeys = 0;
	row = 0;
	col = 0;
	top = 0;
	up = m_load_bitmap(getResource("media/up.png"));
	for (int i = 0; i < MAX_PARTY; i++) {
		if (party[i]) {
			partyAnims[i] = new AnimationSet(getResource("objects/%s.png", party[i]->getName().c_str()));
			partyAnims[i]->setSubAnimation("stand_s");
		}
		else {
			partyAnims[i] = NULL;
		}
	}
	clicked = false;
	up_clicked = false;
	down_clicked = false;
}


MShop::~MShop(void)
{
	m_destroy_bitmap(up);
	for (int i = 0; i < MAX_PARTY; i++) {
		if (partyAnims[i])
			delete partyAnims[i];
	}
}

void FakeWidget::draw(void) {
	if (this == tguiActiveWidget && draw_outline) {
		int n = (unsigned)tguiCurrentTimeMillis() % 1000;
		float p;
		if (n < 500) p = n / 500.0f;
		else p = (500-(n-500)) / 500.0f;
		int a = p*255;
		m_draw_rectangle(x+0.5f, y+0.5f, x+width+0.5f, y+height+0.5f,
			m_map_rgba(0xff*a/255, 0xd8*a/255, 0, a), 0);
	}
}

void FakeWidget::mouseDown(int x, int y, int b)
{
	holdStart = tguiCurrentTimeMillis();
}

void FakeWidget::mouseUp(int x, int y, int b)
{
	if (x >= 0 && y >= 0) {
		clicked = true;
	}
	holdStart = 0;
	buttonHoldStarted = false;
}

unsigned long FakeWidget::getHoldStart(void)
{
	if (b3_pressed) {
		b3_pressed = false;
		return tguiCurrentTimeMillis() - 1000UL;
	}
	else {
		return holdStart;
	}
}

void FakeWidget::reset(void) 
{
	mouseUp(-1, -1, 0);
}

bool FakeWidget::acceptsFocus() { return accFocus; }

int FakeWidget::update(int step)
{
	if (was_down && getInput()->getDescriptor().button1)
		return TGUI_CONTINUE;
	else
		was_down = false;

	if (tguiActiveWidget == this) {
		INPUT_EVENT ie = get_next_input_event();
		InputDescriptor id = getInput()->getDescriptor();

		if (ie.button3 == DOWN) {
			b3_pressed = true;
		}
		else if (buttonHoldStarted && !id.button1 && holdStart != 0) {
			use_input_event();
			if (holdStart+250 > tguiCurrentTimeMillis()) {
				clicked = true;
			}
			holdStart = 0;
		}
		else if (id.button1 && holdStart == 0) {
			use_input_event();
			holdStart = tguiCurrentTimeMillis();
			buttonHoldStarted = true;
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

void FakeWidget::setFocus(bool fcs)
{
	TGUIWidget::setFocus(fcs);

	was_down = getInput()->getDescriptor().button1;
}

FakeWidget::FakeWidget(int x, int y, int w, int h, bool accFocus, bool draw_outline) {
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
	buttonHoldStarted = false;
}

void MTab::draw(void)
{
	if (selected) {
		m_draw_bitmap(bmp, x, y, 0);
	}
	MCOLOR color = grey;
	if (this == tguiActiveWidget)
		color = m_map_rgb(255, 255, 0);
	mTextout_simple(_t(text.c_str()), x+width/2-m_text_length(game_font, _t(text.c_str()))/2, y+3, color);
}

void MTab::mouseUp(int x, int y, int b)
{
	if (x >= 0 && y >= 0) {
		clicked = true;
	}
}

bool MTab::acceptsFocus()
{
	return true;
}

int MTab::update(int step)
{
	if (this == tguiActiveWidget) {
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

void MTab::setSelected(bool s)
{
	selected = s;
}

MTab::MTab(std::string text, int x, int y) {
	this->hotkeys = 0;
	this->x = x;
	this->y = y;
	this->width = m_text_length(game_font, _t(text.c_str())) + 6;
	this->height = 15;
	this->text = text;
	clicked = false;
	int flags = al_get_new_bitmap_flags();
	al_set_new_bitmap_flags(flags & ~ALLEGRO_NO_PRESERVE_TEXTURE);
	bmp = m_create_bitmap(width, 14);
	al_set_new_bitmap_flags(flags);
	m_push_target_bitmap();
	m_set_target_bitmap(bmp);
	mDrawFrame(2, 2, width-4, 20);
	m_pop_target_bitmap();
	selected = false;
}

MTab::~MTab()
{
	m_destroy_bitmap(bmp);
}

#ifdef EDITOR

void WgtFrame::draw()
{
	MCOLOR color2;
	color2 = white;

	m_draw_rectangle(x, y, x+width, y+height, color, M_FILLED);
	m_draw_rectangle(x, y, x+width, y+height, color2, M_OUTLINED);

	if (moveable) {
		color2 = m_map_rgb(40, 80, 100);
		m_draw_rectangle(x, y, x+width, y+16, color2, M_FILLED);
		color2 = white;
		m_draw_rectangle(x, y, x+width, y+16, color2, M_OUTLINED);
	}

	if (closeable) {
		int xx = x+width-17;
		int xy = y;
		m_draw_line(xx+1, xy+1, xx+1+16, xy+1+16, black);
		m_draw_line(xx+1, xy+1+16, xx+1+16, xy+1, black);
		m_draw_line(xx, xy, xx+16, xy+16, white);
		m_draw_line(xx, xy+16, xx+16, xy, white);
	}
}


int WgtFrame::update(int millis)
{
	if (closed) {
		closed = false;
		return TGUI_RETURN;
	}

	if (!dragging)
		return TGUI_CONTINUE;

	int mx = tguiGetMouseX();
	int my = tguiGetMouseY();

	int dx = mx - lastMouseX;
	int dy = my - lastMouseY;


	if ((dx == 0) && (dy == 0))
		return TGUI_CONTINUE;

	lastMouseX = mx;
	lastMouseY = my;

	tguiTranslateWidget(this, dx, dy);

	return TGUI_CONTINUE;
}


void WgtFrame::mouseDown(int mx, int my, int mb)
{
	if (dragging)
		return;

	lastMouseX = tguiGetMouseX();
	lastMouseY = tguiGetMouseY();

	if (pointInBox(lastMouseX, lastMouseY, x+width-16, y+0, x+width, y+16)) {
		closePressed = true;
		return;
	}

	if (moveable)
		dragging = true;
	
	tguiRaiseWidget(this);
	
	tguiSetFocus(homeWidget);
}
	

void WgtFrame::mouseUp(int x, int y, int b)
{
	dragging = false;
	if (closePressed && pointInBox(x, y, width-16, 0, width, 16)) {
		closed = true;
		closePressed = false;
	}
}


WgtFrame::WgtFrame(int x, int y, int width, int height, bool moveable,
	bool closeable, MCOLOR color, TGUIWidget *homeWidget)
{
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
	this->hotkeys = 0;
	this->moveable = moveable;
	this->color = color;
	dragging = false;
	this->closeable = closeable;
	this->homeWidget = homeWidget;
	closed = false;
}


WgtFrame::~WgtFrame()
{
}


void WgtButton::draw()
{
	MCOLOR color;

	m_draw_rectangle(x, y, x+width, y+height, button_color, M_FILLED);

	if (focus)
		color = m_map_rgb(0, 200, 255);
	else
		color = black;

	m_draw_rectangle(x, y, x+width, y+height, color, 0);
	m_draw_rectangle(x+1, y+1, x+width-1, y+height-1, color, 0);

	color = white;

	mTextout(font, text.c_str(), x+width/2, y+height/2,
		text_color, color,
		WGT_TEXT_BORDER, true);
}


int WgtButton::update(int millis)
{
	if (clicked) {
		clicked = false;
		return TGUI_RETURN;
	}

	return TGUI_CONTINUE;
}


void WgtButton::mouseDown(int mx, int my, int mb)
{
	pressed = true;
}


void WgtButton::mouseUp(int x, int y, int b)
{
	pressed = false;
	if (x >= 0)
		clicked = true;
}


bool WgtButton::handleKey(int keycode, int unichar)
{
	switch (keycode) {
		case ALLEGRO_KEY_ENTER:
		case ALLEGRO_KEY_SPACE:
			clicked = true;
			return true;
		case ALLEGRO_KEY_LEFT:
		case ALLEGRO_KEY_UP:
			tguiFocusPrevious();
			return true;
		case ALLEGRO_KEY_RIGHT:
		case ALLEGRO_KEY_DOWN:
			tguiFocusNext();
			return true;
	}

	return false;
}


bool WgtButton::acceptsFocus()
{
	return true;
}


WgtButton::WgtButton(int x, int y, int width, int height,
			std::string text, MCOLOR text_color,
			MCOLOR button_color,
			MFONT *font) :
	text(text),
	font(font)
{
	this->x = x;
	this->y = y;
	if (width < 0)
		this->width = m_text_length(font, text.c_str()) + 10;
	else
		this->width = width;
	if (height < 0)
		this->height = m_text_height(font) + 10;
	else
		this->height = height;
	this->hotkeys = 0;
	this->text_color = text_color;
	this->button_color = button_color;
	pressed = false;
	clicked = false;
}


WgtButton::~WgtButton()
{
}


void WgtScrollbar::draw()
{
	MCOLOR color;
	color = black;

	m_draw_rectangle(x, y, x+width, y+height, trough_color, M_FILLED);

	if (vertical) {
		m_draw_rectangle(x, y+tabPos, x+width, y+tabPos+tab_size, tab_color, M_FILLED);
		m_draw_rectangle(x, y+tabPos, x+width, y+tabPos+tab_size, color, 0);
		int x1 = x+4;
		int y1 = y+tabPos+(tab_size/2);
		int x2 = x+width-7;
		int y2 = y1;
		m_draw_line(x1, y1, x2, y2, color);
		m_draw_line(x1, y1-5, x2, y2-5, color);
		m_draw_line(x1, y1+5, x2, y2+5, color);
	}
	else {
		m_draw_rectangle(x+tabPos, y, x+tabPos+tab_size, y+height, tab_color, M_FILLED);
		m_draw_rectangle(x+tabPos, y, x+tabPos+tab_size, y+height, color, 0);
		int x1 = x+tabPos+(tab_size/2);
		int y1 = y+4;
		int x2 = x1;
		int y2 = y+height-7;
		m_draw_line(x1, y1, x2, y2, color);
		m_draw_line(x1-5, y1, x2-5, y2, color);
		m_draw_line(x1+5, y1, x2+5, y2, color);
	}
}


int WgtScrollbar::update(int millis)
{
	if (!pressed)
		return TGUI_CONTINUE;

	int mx = tguiGetMouseX();
	int my = tguiGetMouseY();

	int length, d;

	if (vertical) {
		length = height;
		d = my - pressedPos;
	}
	else {
		length = width;
		d = mx - pressedPos;
	}

	if (d != 0) {
		if (d < 0) {
			if (tabPos > 0) {
				tabPos += d;
				if (tabPos < 0)
					tabPos = 0;
			}
		}
		else {
			int max = length - tab_size - 1;
			if (tabPos < max) {
				tabPos += d;
			if (tabPos > max)
					tabPos = max;
			}
		}
		pressedPos += d;
	}

	return TGUI_CONTINUE;
}


void WgtScrollbar::mouseDown(int mx, int my, int mb)
{
	pressed = true;

	if (vertical) {
		pressedPos = my + y;
	}
	else {
		pressedPos = mx + x;
	}
}


void WgtScrollbar::mouseUp(int x, int y, int b)
{
	pressed = false;
}


float WgtScrollbar::getValue(void)
{
	int length;

	if (vertical)
		length = height;
	else
		length = width;

	int max = length - tab_size - 1;

	return (end - start) * ((float)tabPos / (float)max) + start;
}


void WgtScrollbar::setValue(float value)
{
	tabPos = (int) value;
}


void WgtScrollbar::setParams(float start, float end, float extent)
{
	this->start = start;
	this->end = end;
	this->extent = extent;
	tab_size = calcTabSize();
}


WgtScrollbar::WgtScrollbar(bool vertical, int x, int y, int width, int height,
		float start, float end, float extent,
		MCOLOR tab_color, MCOLOR trough_color)
{
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
	this->start = start;
	this->end = end;
	this->extent = extent;
	this->tab_color = tab_color;
	this->trough_color = trough_color;
	this->hotkeys = 0;
	pressed = false;
	pressedPos = -1;
	this->vertical = vertical;
	this->tab_size = calcTabSize();
	this->tabPos = 0;
}


WgtScrollbar::~WgtScrollbar()
{
}


int WgtScrollbar::calcTabSize(void)
{
	int length;

	if (vertical)
		length = height;
	else
		length = width;

	float size = (extent / (end - start + extent)) * length;

	if (size > length) size = length;
	if (size < TAB_MIN_SIZE) size = TAB_MIN_SIZE;

	return (int)size;
}


void WgtMenuItem::draw()
{
	if (this == tguiActiveWidget) {
		m_draw_rectangle(x, y, x+width, y+height, highlightBgColor, M_FILLED);
		mTextout(font, text.c_str(), x+4, y,
			highlightTextColor, black,
			WGT_TEXT_NORMAL, false);
	}
	else {
		m_draw_rectangle(x, y, x+width, y+height, bgColor, M_FILLED);
		mTextout(font, text.c_str(), x+4, y,
			textColor, black,
			WGT_TEXT_NORMAL, false);
	}

}


int WgtMenuItem::update(int millis)
{
	if (clicked) {
		clicked = false;
		return TGUI_RETURN;
	}

	int mx = tguiGetMouseX();
	int my = tguiGetMouseY();

	if (lastMouseX == mx && lastMouseY == my)
		return TGUI_CONTINUE;

	if (mx >= x && mx < (x+width) && my >= y && my < (y+height)) {
		tguiSetFocus(this);
		lastMouseX = mx;
		lastMouseY = my;
	}

	return TGUI_CONTINUE;
}


void WgtMenuItem::mouseDown(int mx, int my, int mb)
{
	pressed = true;
}


void WgtMenuItem::mouseUp(int x, int y, int b)
{
	pressed = false;
	if (x >= 0)
		clicked = true;
}


bool WgtMenuItem::acceptsFocus()
{
	return true;
}


void WgtMenuItem::show(void)
{
	tguiAddWidget(this);
}


void WgtMenuItem::hide(void)
{
	tguiDeleteWidget(this);
	clicked = false;
}


bool WgtMenuItem::handleKey(int keycode, int unichar)
{
	switch (keycode) {
		case ALLEGRO_KEY_ENTER:
		case ALLEGRO_KEY_SPACE:
			clicked = true;
			return true;
		case ALLEGRO_KEY_LEFT:
		case ALLEGRO_KEY_UP:
			tguiFocusPrevious();
			return true;
		case ALLEGRO_KEY_RIGHT:
		case ALLEGRO_KEY_DOWN:
			tguiFocusNext();
			return true;
	}

	return false;
}


WgtMenuItem::WgtMenuItem(int x, int y, int width, int height,
		std::string text, MCOLOR textColor,
		MCOLOR bgColor,
		MCOLOR highlightTextColor,
		MCOLOR highlightBgColor,
		MFONT *font) :
	text(text),
	font(font)
{
	this->x = x;
	this->y = y;
	if (width < 0)
		this->width = m_text_length(font, text.c_str()) + 10;
	else
		this->width = width;
	if (height < 0)
		this->height = m_text_height(font) + 10;
	else
		this->height = height;
	this->hotkeys = 0;
	this->textColor = textColor;
	this->bgColor = bgColor;
	this->highlightTextColor = highlightTextColor;
	this->highlightBgColor = highlightBgColor;
	pressed = false;
	clicked = false;
	lastMouseX = -1;
	lastMouseY = -1;
}


WgtMenuItem::~WgtMenuItem()
{
}

	
std::string Menu::getName(void)
{
	return name;
}


bool Menu::addItem(WgtMenuItem *item)
{
	items.push_back(item);
	return true;
}


void Menu::setX(int x)
{
	for (unsigned int i = 0; i < items.size(); i++) {
		items[i]->setX(x);
	}
}


void Menu::hide(void)
{
	for (unsigned int i = 0; i < items.size(); i++) {
		WgtMenuItem *w = items[i];
		w->hide();
	}
}


void Menu::show(void)
{
	tguiSetParent(0);

	for (unsigned int i = 0; i < items.size(); i++) {
		WgtMenuItem *w = items[i];
		w->show();
	}

	tguiSetFocus(items[0]);
}


Menu::Menu(std::string name) :
	name(name)
{
}

Menu::~Menu(void)
{
	for (unsigned int i = 0; i < items.size(); i++) {
		WgtMenuItem *w = items[i];
		w->hide();
		delete w;
	}

	items.clear();
}


void WgtMenuBar::draw(void)
{
	m_draw_rectangle(x, y, x+width, y+height, bgColor, M_FILLED);

	unsigned int i, ty;

	ty = y + height/2 - m_text_height(font)/2;

	for (i = 0; i < menus.size(); i++) {
		std::string text = menus[i]->getName();
		mTextout(font, text.c_str(), textPos[i], ty,
			textColor, black, WGT_TEXT_NORMAL, false);
	}
}

int WgtMenuBar::update(int millis)
{
	return TGUI_CONTINUE;
}

void WgtMenuBar::mouseDown(int mx, int my, int mb)
{
	for (unsigned int i = 0; i < menus.size(); i++) {
		menus[i]->hide();
	}

	int absolute_mx = mx + x;

	for (unsigned int i = 0; i < menus.size(); i++) {
		if (absolute_mx < textPos[i] ||
				absolute_mx > (textPos[i]+textWidth[i]))
			continue;
		// found
		if (active) {
			menus[i]->hide();
			if ((int)i != activeIndex)
				menus[i]->show();
		}
		else {
			menus[i]->show();
		}
		active = !active;
		activeIndex = i;
		return;
	}
}


void WgtMenuBar::hideMenus(void)
{
	for (unsigned int i = 0; i < menus.size(); i++) {
		menus[i]->hide();
	}
	active = false;
}


bool WgtMenuBar::handleHotkey(int hotkey)
{
	int flags = tguiGetHotkeyFlags(hotkey);
	int keycode = tguiGetHotkeyKey(hotkey);

	if (keycode == ALLEGRO_KEY_F && (flags & TGUI_KEYFLAG_ALT)) {
		mouseDown(textPos[0]+2, 2, 1);
		return true;
	}
	if (keycode == ALLEGRO_KEY_E && (flags & TGUI_KEYFLAG_ALT)) {
		mouseDown(textPos[1]+2, 2, 1);
		return true;
	}
	if (keycode == ALLEGRO_KEY_T && (flags & TGUI_KEYFLAG_ALT)) {
		mouseDown(textPos[2]+2, 2, 1);
		return true;
	}
	if (keycode == ALLEGRO_KEY_L && (flags & TGUI_KEYFLAG_ALT)) {
		mouseDown(textPos[3]+2, 2, 1);
		return true;
	}
	if (keycode == ALLEGRO_KEY_I && (flags & TGUI_KEYFLAG_ALT)) {
		mouseDown(textPos[4]+2, 2, 1);
		return true;
	}
	
	return false;
}


WgtMenuBar::WgtMenuBar(int x, int y, int width, int height,
		std::vector<Menu *>& menus,
		MCOLOR textColor, MCOLOR bgColor,
		MFONT *font,
		std::vector<int> *hotkeys) :
	menus(menus),
	font(font)
{
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
	this->hotkeys = hotkeys;
	this->textColor = textColor;
	this->bgColor = bgColor;
	calcPositions();
	active = false;
}


WgtMenuBar::~WgtMenuBar()
{
	textPos.clear();
	textWidth.clear();

	for (unsigned int i = 0; i < menus.size(); i++) {
		delete menus[i];
	}

	menus.clear();
}


void WgtMenuBar::calcPositions(void)
{
	unsigned int i, tx;

	for (i = 0, tx = 5; i < menus.size(); i++) {
		std::string text = menus[i]->getName();
		textPos.push_back(tx);
		menus[i]->setX(tx);
		textWidth.push_back(m_text_length(font, text.c_str()));
		tx += 8 + m_text_length(font, text.c_str());
	}
}


void WgtRadioButton::draw()
{
	if (tguiActiveWidget == this) {
		MCOLOR yellow;
		yellow = m_map_rgb(0, 200, 255);
		m_draw_circle(x+height/2, y+height/2, height/2, yellow, M_FILLED);
	}
	else {
		m_draw_circle(x+height/2, y+height/2, height/2, textColor, M_FILLED);
	}
	m_draw_circle(x+height/2, y+height/2, height/2, black, M_OUTLINED);

	if (checked) {
		m_draw_circle(x+height/2, y+height/2, height/2-4, highlightColor, M_FILLED);
	}

	mTextout(font, text.c_str(), x + height + 5, y, textColor, black,
		WGT_TEXT_BORDER, false);
}


int WgtRadioButton::update(int millis)
{
	return TGUI_CONTINUE;
}


void WgtRadioButton::mouseDown(int mx, int my, int mb)
{
	setChecked(true);
}


bool WgtRadioButton::handleKey(int keycode, int unichar)
{
	switch (keycode) {
		case ALLEGRO_KEY_ENTER:
		case ALLEGRO_KEY_SPACE:
			setChecked(!checked);
			return true;
		case ALLEGRO_KEY_LEFT:
		case ALLEGRO_KEY_UP:
			tguiFocusPrevious();
			return true;
		case ALLEGRO_KEY_RIGHT:
		case ALLEGRO_KEY_DOWN:
			tguiFocusNext();
			return true;
	}

	return false;
}


bool WgtRadioButton::acceptsFocus()
{
	return true;
}


void WgtRadioButton::setChecked(bool c)
{
	if (c) {
		WgtRadioButton *r;
		r = prev;
		while (r) {
			r->setChecked(false);
			r = r->getPrev();
		}
		r = next;
		while (r) {
			r->setChecked(false);
			r = r->getNext();
		}
		checked = true;
	}
	else
		checked = false;
}


WgtRadioButton *WgtRadioButton::getNext(void)
{
	return next;
}


WgtRadioButton *WgtRadioButton::getPrev(void)
{
	return prev;
}


bool WgtRadioButton::isChecked(void)
{
	return checked;
}


void WgtRadioButton::setNext(WgtRadioButton *next)
{
	this->next = next;
}


WgtRadioButton::WgtRadioButton(int x, int y, std::string text,
			MCOLOR textColor,
			MCOLOR highlightColor,
			MFONT *font,
			WgtRadioButton *previous) :
	text(text),
	font(font)
{
	this->x = x;
	this->y = y;
	height = m_text_height(font);
	width = m_text_length(font, text.c_str()) + height + 5;
	this->hotkeys = 0;
	this->textColor = textColor;
	this->highlightColor = highlightColor;
	checked = false;
	if (previous)
		previous->setNext(this);
	prev = previous;
	next = NULL;
}


WgtRadioButton::~WgtRadioButton()
{
}


void WgtLabel::draw()
{
	mTextout(font, text.c_str(), x, y, textColor, shadowColor,
		WGT_TEXT_BORDER, false);
}


int WgtLabel::update(int millis)
{
	return TGUI_CONTINUE;
}


WgtLabel::WgtLabel(int x, int y, std::string text,
			MCOLOR textColor,
			MCOLOR shadowColor,
			MFONT *font) :
	text(text),
	font(font)
{
	this->x = x;
	this->y = y;
	width = m_text_length(font, text.c_str());
	height = m_text_height(font);
	this->hotkeys = 0;
	this->textColor = textColor;
	this->shadowColor = shadowColor;
}


WgtLabel::~WgtLabel()
{
}


void WgtTextField::draw()
{
	m_draw_rectangle(x, y, x+width, y+height, bgColor, M_FILLED);
	m_draw_rectangle(x, y, x+width, y+height, black, M_OUTLINED);

	mTextout(font, buffer, x+4, y+(height/2)-m_text_height(font)/2,
		textColor, black, WGT_TEXT_NORMAL, false);

	if (this == tguiActiveWidget && ((unsigned)tguiCurrentTimeMillis() % 1000) < 500) {
		MCOLOR green = m_map_rgb(0, 255, 0);
		int rx = x + 6 + m_text_length(font, buffer);
		int ry = y + (height/2) - m_text_height(font)/2;
		m_draw_rectangle(rx, ry, rx+4, ry+m_text_height(font),
			green, M_FILLED);
	}
}


int WgtTextField::update(int millis)
{
	return TGUI_CONTINUE;
}


bool WgtTextField::handleKey(int keycode, int unichar)
{
	int length = strlen(buffer);

	switch (keycode) {
		case ALLEGRO_KEY_BACKSPACE:
			if (length > 0) {
				buffer[length-1] = 0;
			}
			return true;
		case ALLEGRO_KEY_LEFT:
		case ALLEGRO_KEY_UP:
			tguiFocusPrevious();
			return true;
		case ALLEGRO_KEY_RIGHT:
		case ALLEGRO_KEY_DOWN:
			tguiFocusNext();
			return true;
	}

	if (length < maxChars && keyPassesFilter(unichar)) {
		buffer[length] = unichar;
		buffer[length+1] = 0;
	}

	return true;
}

bool WgtTextField::acceptsFocus()
{
	return true;
}


void WgtTextField::setText(std::string newText)
{
	strcpy(buffer, newText.c_str());
}


std::string WgtTextField::getText(void)
{
	return std::string(buffer);
}


WgtTextField::WgtTextField(int x, int y, int width, int height, int maxChars,
			std::string text,
			std::string filterChars,
			MCOLOR textColor,
			MCOLOR bgColor, MFONT *font) :
	text(text),
	filterChars(filterChars)
{
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
	this->hotkeys = 0;
	this->maxChars = maxChars;
	this->textColor = textColor;
	this->bgColor = bgColor;
	this->font = font;
	buffer = new char[maxChars+1];
	strcpy(buffer, text.c_str());
}


WgtTextField::~WgtTextField()
{
	delete[] buffer;
}


bool WgtTextField::keyPassesFilter(int key)
{
	const char *allowed = filterChars.c_str();

	for (int i = 0; allowed[i]; i++) {
		if (allowed[i] == key)
			return true;
	}

	return false;
}


void WgtAreaEditor::draw()
{
	// Using 2 as scale here...

	int mx = tguiGetMouseX();
	int my = tguiGetMouseY();

	if (!(mx >= x && my >= y && mx < (x+width) && my < (y+height)))
		return;

	int tx = (mx - x) / (TILE_SIZE * 2);
	int ty = (my - y) / (TILE_SIZE * 2);

	int x1 = x + (tx*TILE_SIZE*2);
	int y1 = y + (ty*TILE_SIZE*2);
	int x2 = x1 + TILE_SIZE*2;
	int y2 = y1 + TILE_SIZE*2;

	MCOLOR yellow = m_map_rgb(255, 255, 0);

	m_draw_rectangle(x1, y1, x2, y2, yellow, 0);
}


int WgtAreaEditor::update(int millis)
{
	int mx = tguiGetMouseX();
	int my = tguiGetMouseY();

	if (!brushStarted || !pointInBox(mx, my, x, y, x+width, y+height))
		return TGUI_CONTINUE;

	if (myBrush == BRUSH || myBrush == CLONE) {
		int i = checkTile();
		brush(i, mx, my);
	}
	else if (myBrush == SOLIDS) {
		int tx = mx / (TILE_SIZE*2) + (int)hbarMain->getValue();
		int ty = (my - 52) / (TILE_SIZE*2) + (int)vbarMain->getValue();

		if (tx >= 0 && ty >= 0 &&
				tx < (int)area->getWidth() && ty < (int)area->getHeight()) {
			if (tx == lastSolidX && ty == lastSolidY) {
				return TGUI_CONTINUE;
			}
			lastSolidX = tx;
			lastSolidY = ty;
			Tile *t = area->getTile(tx, ty);
			t->setSolid(!t->isSolid());
		}
	}
	else if (myBrush == CLEAR) {
		int tx = mx / (TILE_SIZE*2) + (int)hbarMain->getValue();
		int ty = (my - 52) / (TILE_SIZE*2) + (int)vbarMain->getValue();
		if (tx >= 0 && ty >= 0 && tx < (int)area->getWidth() &&
				ty < (int)area->getHeight()) {
			Tile *t = area->getTile(tx, ty);
			for (int i = 0; i < TILE_LAYERS; i++) {
				t->setAnimationNum(i, -1);
			}
		}
	}

	return TGUI_CONTINUE;
}


void WgtAreaEditor::mouseDown(int mx, int my, int mb)
{
	menuBar->hideMenus();
	tguiSetFocus(this);

	int abs_mx = mx + x;
	int abs_my = my + y;
	int tx = mx / (TILE_SIZE*2) + (int)hbarMain->getValue();
	int ty = my / (TILE_SIZE*2) + (int)vbarMain->getValue();

	if (tx >= (int)area->getWidth() || ty >= (int)area->getHeight())
		return;

	if (mb & 1) {
		if (myBrush == BRUSH || myBrush == CLONE) {
			int i = checkTile();
			pushUndoInfo();
			brush(i, abs_mx, abs_my);
			clearRedo();
			brushStarted = true;
		}
		else if (myBrush == FILL) {
			int i = checkTile();
			pushUndoInfo();
			Tile *t = area->getTile(tx, ty);
			int was = t->getAnimationNum(currentLayer);
			if (was != i)
				fill(was, i, tx, ty);
			clearRedo();
		}
		else if (myBrush == SOLIDS) {
			pushUndoInfo();
			if (tx >= 0 && ty >= 0 &&
					tx < (int)area->getWidth() && ty < (int)area->getHeight()) {
				lastSolidX = tx;
				lastSolidY = ty;
				Tile *t = area->getTile(tx, ty);
				t->setSolid(!t->isSolid());
			}
			clearRedo();
			brushStarted = true;
		}
		else if (myBrush == CLEAR) {
			pushUndoInfo();
			Tile *t = area->getTile(tx, ty);
			for (int i = 0; i < TILE_LAYERS; i++) {
				t->setAnimationNum(i, -1);
			}
			clearRedo();
			brushStarted = true;
		}
		else if (myBrush == MACRO) {
			pushUndoInfo();
			std::vector<MacroInfo *> *v = macros[currentMacro];
			for (unsigned int i = 0; i < v->size(); i++) {
				MacroInfo *m = (*v)[i];
				int xx = tx+m->x;
				int yy = ty+m->y;
				if (xx < 0 || yy < 0 || xx >= (int)area->getWidth() || yy >= (int)area->getHeight())
					continue;
				Tile *t = area->getTile(xx, yy);
				t->setAnimationNum(m->layer, m->tile);
			}
			//FIXME:
			clearRedo();
		}
	}
	else if (mb & 2) { // popup delete/add rows/columns
		if (!contextMenuOpen) {
			tguiSetParent(0);
			tguiAddWidget(contextFrame);
			tguiSetParent(contextFrame);
			tguiAddWidget(contextInsertRowBefore);
			tguiAddWidget(contextInsertRowAfter);
			tguiAddWidget(contextInsertColumnBefore);
			tguiAddWidget(contextInsertColumnAfter);
			tguiAddWidget(contextDeleteRow);
			tguiAddWidget(contextDeleteColumn);
			tguiAddWidget(contextInsert);
			tguiAddWidget(contextDelete);
			tguiSetParent(0);
			tguiSetFocus(contextInsertRowBefore);
			tguiTranslateWidget(contextFrame, -contextFrame->getX(),
				-contextFrame->getY());
			tguiTranslateWidget(contextFrame, abs_mx, abs_my);
			contextFrameX = abs_mx;
			contextFrameY = abs_my;
			contextMenuOpen = true;
		}
	}
}
	

void WgtAreaEditor::mouseUp(int x, int y, int b)
{
	brushStarted = false;
	clone_tile_x = -1;
	clone_tile_y = -1;
	clone_start_x = -1;
	clone_start_y = -1;
	lastSolidX = lastSolidY = -1;
}


WgtAreaEditor::WgtAreaEditor(int x, int y, int width, int height)
{
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
	this->hotkeys = 0;
	brushStarted = false;
	lastSolidX = lastSolidY = -1;
}


WgtAreaEditor::~WgtAreaEditor()
{
}


void WgtTileSelector::draw()
{
	// Draw the mini tilemap

	int sx, sy;
	int x1 = sx = (int)hbar->getValue();
	int y1 = sy = (int)vbar->getValue();
	int x2 = x1 + cols;
	int y2 = y1 + rows;

	for (; y1 < y2; y1++) {
		x1 = sx;
		for (; x1 < x2; x1++) {
			int i = y1 * (512 / TILE_SIZE) + x1;
			tile = area->tileAnimations[area->newmap[area->tileAnimationNums[i]]]->getCurrentFrame()->getImage()->getBitmap();
			int xx = ((x1 - sx) * (TILE_SIZE*scale)) + x;
			int yy = ((y1 - sy) * (TILE_SIZE*scale)) + y;
			m_draw_scaled_bitmap(tile, 0, 0, TILE_SIZE, TILE_SIZE,
				xx, yy, TILE_SIZE*scale, TILE_SIZE*scale, 0, 255);
		}
	}

	int mx = tguiGetMouseX();
	int my = tguiGetMouseY();

	if (!(mx >= x && my >= y && mx < (x+width) && my < (y+height)))
		return;

	int tx = (mx - x) / (TILE_SIZE * scale);
	int ty = (my - y) / (TILE_SIZE * scale);

	x1 = x + (tx*TILE_SIZE*scale);
	y1 = y + (ty*TILE_SIZE*scale);
	x2 = x1 + TILE_SIZE*scale;
	y2 = y1 + TILE_SIZE*scale;

	MCOLOR yellow = m_map_rgb(255, 255, 0);

	m_draw_rectangle(x1, y1, x2, y2, yellow, 0);
}


int WgtTileSelector::update(int millis)
{
	return TGUI_CONTINUE;
}


void WgtTileSelector::mouseUp(int mx, int my, int mb)
{
	if (mx < 0) return;

	menuBar->hideMenus();

	if (mb & 1) {
		int gx = (mx) / (TILE_SIZE*scale);
		int gy = (my) / (TILE_SIZE*scale);
		int bx = (int)hbar->getValue();
		int by = (int)vbar->getValue();
		currentTile = ((by+gy)*(512/TILE_SIZE)) + (bx+gx);
	}

}
	

WgtTileSelector::WgtTileSelector(int x, int y, int width, int height,
	int scale, WgtScrollbar *vbar, WgtScrollbar *hbar)
{
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
	this->hotkeys = 0;
	this->scale = scale;
	this->vbar = vbar;
	this->hbar = hbar;
	this->cols = width / (TILE_SIZE * scale);
	this->rows = height / (TILE_SIZE * scale);
}


WgtTileSelector::~WgtTileSelector()
{
}

void WgtCrappyWidget::draw()
{
	m_draw_rectangle(x, y, x+width, y+height, bgColor, M_FILLED);
	m_draw_rectangle(x, y, x+width, y+height, black, M_OUTLINED);

	mTextout(font, buffer, x+4, y+(height/2)-m_text_height(font)/2,
		textColor, black, WGT_TEXT_NORMAL, false);

	if (this == tguiActiveWidget && ((unsigned)tguiCurrentTimeMillis() % 1000) < 500) {
		MCOLOR green = m_map_rgb(0, 255, 0);
		int rx = x + 6 + m_text_length(font, buffer);
		int ry = y + (height/2) - m_text_height(font)/2;
		m_draw_rectangle(rx, ry, rx+4, ry+m_text_height(font),
			green, M_FILLED);
	}
}


int WgtCrappyWidget::update(int millis)
{
	if (ret) return TGUI_RETURN;
	return TGUI_CONTINUE;
}


bool WgtCrappyWidget::handleKey(int keycode, int unichar)
{
	int length = strlen(buffer);

	switch (keycode) {
		case ALLEGRO_KEY_BACKSPACE:
			if (length > 0) {
				buffer[length-1] = 0;
			}
			return true;
		case ALLEGRO_KEY_LEFT:
		case ALLEGRO_KEY_UP:
			tguiFocusPrevious();
			return true;
		case ALLEGRO_KEY_RIGHT:
		case ALLEGRO_KEY_DOWN:
			tguiFocusNext();
			return true;
		case ALLEGRO_KEY_ENTER:
			ret = true;
			return true;
	}

	if (length < maxChars && keyPassesFilter(unichar)) {
		buffer[length] = unichar;
		buffer[length+1] = 0;
	}

	return true;
}

bool WgtCrappyWidget::acceptsFocus()
{
	return true;
}


void WgtCrappyWidget::setText(std::string newText)
{
	strcpy(buffer, newText.c_str());
}


std::string WgtCrappyWidget::getText(void)
{
	return std::string(buffer);
}


WgtCrappyWidget::WgtCrappyWidget(int x, int y, int width, int height, int maxChars,
			std::string text,
			std::string filterChars,
			MCOLOR textColor,
			MCOLOR bgColor, MFONT *font) :
	text(text),
	filterChars(filterChars)
{
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
	this->hotkeys = 0;
	this->maxChars = maxChars;
	this->textColor = textColor;
	this->bgColor = bgColor;
	this->font = font;
	buffer = new char[maxChars+1];
	strcpy(buffer, text.c_str());
	ret = false;
}


WgtCrappyWidget::~WgtCrappyWidget()
{
	delete[] buffer;
}


bool WgtCrappyWidget::keyPassesFilter(int key)
{
	const char *allowed = filterChars.c_str();

	for (int i = 0; allowed[i]; i++) {
		if (allowed[i] == key)
			return true;
	}

	return false;
}


#endif // EDITOR

