#include "monster2.hpp"

#include <allegro5/allegro_opengl.h>

#ifdef ALLEGRO_ANDROID
#include "java.h"
#endif

static std::vector<LoadedBitmap *> loaded_bitmaps;

ALLEGRO_BITMAP *my_al_create_bitmap(int w, int h)
{
	ALLEGRO_BITMAP *bmp = al_create_bitmap(w, h);
	return bmp;
}

static float get_trap_peak(int topw, int bottomw, int length)
{
	int w = (bottomw - topw) / 2.0f;
	float a = atan((float)length/w);
	return tan(a) * (topw/2.0f);
}

static void null_lb(LoadedBitmap *lb)
{
	lb->load_type = (LoadType)0;
	lb->load.filename = "";
	lb->load.redraw = 0;
	lb->load.data = 0;
	lb->destroy.func = 0;
	lb->recreate.func = 0;
	lb->recreate.data = 0;
	lb->recreate.w = 0;
	lb->recreate.h = 0;
	lb->bitmap = 0;
	lb->format = 0;
	lb->delayed = 0;
}

MCOLOR m_map_rgba(int r, int g, int b, int a)
{
	return al_map_rgba(r, g, b, a);
}


MCOLOR m_map_rgb(int r, int g, int b)
{
	return al_map_rgb(r, g, b);
}

MCOLOR m_map_rgb_f(float r, float g, float b)
{
	return al_map_rgb_f(r, g, b);
}

void m_unmap_rgba(MCOLOR c,
	unsigned char *r,
	unsigned char *g,
	unsigned char *b,
	unsigned char *a)
{
	al_unmap_rgba(c, r, g, b, a);
}

MBITMAP *m_create_alpha_bitmap(int w, int h, void (*create)(MBITMAP *bitmap, RecreateData *), RecreateData *data, void (*destroy)(MBITMAP *b), bool delayed) // check
{
	int f = al_get_new_bitmap_format();
	al_set_new_bitmap_format(ALPHA_FMT);
	ALLEGRO_BITMAP *bitmap;
	int flags = al_get_new_bitmap_flags();

	int new_flags = flags;

	if (config.getUseOnlyMemoryBitmaps()) {
		al_set_new_bitmap_flags(new_flags|ALLEGRO_MEMORY_BITMAP);
		bitmap = my_al_create_bitmap(w, h);
	}
	else {
		bitmap = my_al_create_bitmap(w, h);
	}

	al_set_new_bitmap_flags(flags);
	al_set_new_bitmap_format(f);

	MBITMAP *m = new_mbitmap(bitmap);

	if (create) {
		create(m, data);
	}

#if defined ALLEGRO_ANDROID || defined ALLEGRO_WINDOWS
	if (
#ifdef ALLEGRO_WINDOWS
			(al_get_display_flags(display) & ALLEGRO_DIRECT3D) &&
#endif
			(al_get_bitmap_flags(bitmap) & ALLEGRO_NO_PRESERVE_TEXTURE) &&
			!(al_get_bitmap_flags(bitmap) & ALLEGRO_MEMORY_BITMAP)
	) {
		LoadedBitmap *lb = new LoadedBitmap;
		null_lb(lb);
		lb->load_type = LOAD_CREATE;
		lb->flags = al_get_bitmap_flags(bitmap);
		lb->format = al_get_bitmap_format(bitmap);
		lb->destroy.func = destroy;
		lb->recreate.func = create;
		lb->recreate.data = data;
		lb->recreate.w = w;
		lb->recreate.h = h;
		lb->bitmap = m;
		lb->delayed = delayed;
		loaded_bitmaps.push_back(lb);
	}
	else {
#endif
	if (data) {
		delete data;
	}
#if defined ALLEGRO_ANDROID || defined ALLEGRO_WINDOWS
	}
#endif

	return m;
}

void m_draw_alpha_bitmap(MBITMAP *b, int x, int y)
{
	m_draw_bitmap(b, x, y, 0);
}

void m_draw_alpha_bitmap(MBITMAP *b, int x, int y, int flags)
{
	m_draw_bitmap(b, x, y, flags);
}

static bool text_draw_on = false;
static ALLEGRO_TRANSFORM old_view;

void start_text()
{
	if (!text_draw_on) {
		text_draw_on = true;
		al_copy_transform(&old_view, al_get_current_transform());
		ALLEGRO_TRANSFORM v;
		al_identity_transform(&v);
		if (al_get_target_bitmap() == screenshot->bitmap) {
			al_scale_transform(&v, 0.5f/textScaleX, 0.5f/textScaleY);
		}
		else {
			int dx, dy, dw, dh;
			get_screen_offset_size(&dx, &dy, &dw, &dh);
			al_scale_transform(&v, textScaleX, textScaleY);
			if (al_get_bitmap_width(al_get_target_bitmap()) == al_get_display_width(display) && al_get_bitmap_height(al_get_target_bitmap()) == al_get_display_height(display)) {
				al_translate_transform(&v, dx, dy);
			}
		}
		ttf_quick(false);
		al_use_transform(&v);
		ttf_quick(true);
	}
}

void end_text()
{
	if (text_draw_on) {
		ttf_quick(false);
		al_use_transform(&old_view);
		text_draw_on = false;
	}
}

bool drawing_text()
{
	return text_draw_on;
}

void m_textout_f(const MFONT *font, const char *text, float x, float y, MCOLOR color)
{
	bool end = !text_draw_on;

	if (font == game_font) {
		start_text();
		if (al_get_target_bitmap() == screenshot->bitmap) {
			x *= textScaleX;
			y *= textScaleY;
		}
		else {
			x *= (screenScaleX/textScaleX);
			y *= (screenScaleY/textScaleY);
		}
	}

	al_draw_text(font, color, x, y, 0, text);

	if (font == game_font && end) {
		end_text();
	}
}

void m_textout(const MFONT *font, const char *text, int x, int y, MCOLOR color)
{
	m_textout_f(font, text, x, y, color);
}

void m_textout_centre(const MFONT *font, const char *text, int x, int y, MCOLOR color)
{
	int length = m_text_length(font, text);
	m_textout(font, text, x-length/2, y, color);
}


int m_text_height(const MFONT *font)
{
	if (font != game_font) {
		return al_get_font_line_height(font);
	}
	return al_get_font_line_height(font) / (screenScaleX < screenScaleY ? (screenScaleX / textScaleX) : (screenScaleY / textScaleY));
}

void m_draw_rectangle(float x1, float y1, float x2, float y2, MCOLOR color,
	int flags)
{
	if (flags & M_FILLED) {
		al_draw_filled_rectangle(x1, y1, x2, y2, color);
	}
	else {
		al_draw_rectangle(x1, y1, x2, y2, color, 1);
	}
}


void m_draw_line(int x1, int y1, int x2, int y2, MCOLOR color)
{
	al_draw_line(x1+0.5, y1+0.5, x2+0.5, y2+0.5, color, 1);
}


void m_draw_scaled_bitmap(MBITMAP *bmp, float sx, float sy, float sw, float sh,
	float dx, float dy, float dw, float dh, int flags)
{
	m_draw_tinted_scaled_bitmap(bmp, white, sx, sy, sw, sh, dx, dy, dw, dh, flags);
}

void m_draw_scaled_bitmap(MBITMAP *bmp, float sx, float sy, float sw, float sh,
	float dx, float dy, float dw, float dh, int flags, float alpha)
{
	float alpha_f = alpha/255.0;
	quick_draw(bmp->bitmap, al_map_rgba_f(alpha_f, alpha_f, alpha_f, alpha_f), sx, sy, sw, sh, dx, dy, dw, dh, flags);
}


int m_get_bitmap_width(MBITMAP *bmp)
{
	return al_get_bitmap_width(bmp->bitmap);
}


int m_get_bitmap_height(MBITMAP *bmp)
{
	return al_get_bitmap_height(bmp->bitmap);
}

MBITMAP *new_mbitmap(ALLEGRO_BITMAP *bitmap)
{
	MBITMAP *m = new MBITMAP;
	m->bitmap = bitmap;
	return m;
}

MBITMAP *m_load_bitmap(const char *name, bool force_memory, bool ok_to_fail)
{
	ALLEGRO_BITMAP *bitmap = 0;
	int flags = al_get_new_bitmap_flags();

	if (force_memory || config.getUseOnlyMemoryBitmaps()) {
		al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
	}
	else {
		al_set_new_bitmap_flags((flags & ~ALLEGRO_MEMORY_BITMAP));
	}

	bitmap = my_load_bitmap(name, ok_to_fail);

	al_set_new_bitmap_flags(flags);

	if (!bitmap) {
		return NULL;
	}

	MBITMAP *m = new_mbitmap(bitmap);

#if defined ALLEGRO_ANDROID || defined ALLEGRO_WINDOWS
	if (
#ifdef ALLEGRO_WINDOWS
			(al_get_display_flags(display) & ALLEGRO_DIRECT3D) &&
#endif
			(al_get_bitmap_flags(bitmap) & ALLEGRO_NO_PRESERVE_TEXTURE) &&
			!(al_get_bitmap_flags(bitmap) & ALLEGRO_MEMORY_BITMAP)
	) {
		LoadedBitmap *lb = new LoadedBitmap;
		null_lb(lb);
		lb->load_type = LOAD_LOAD;
		lb->flags = al_get_bitmap_flags(bitmap);
		lb->format = al_get_bitmap_format(bitmap);
		lb->load.filename = name;
		lb->load.redraw = NULL;
		lb->load.data = NULL;
		lb->bitmap = m;
		loaded_bitmaps.push_back(lb);
	}
#endif

	return m;
}


MBITMAP *m_load_bitmap_redraw(const char *name, void (*redraw)(MBITMAP *bmp, RecreateData *data), RecreateData *data, bool delayed)
{
	ALLEGRO_BITMAP *bitmap = 0;

	bitmap = my_load_bitmap(name);

	if (!bitmap) {
		debug_message("Error loading bitmap %s\n", name);
		return NULL;
	}

	MBITMAP *m = new_mbitmap(bitmap);

	if (redraw) {
		redraw(m, data);
	}

#if defined ALLEGRO_ANDROID || defined ALLEGRO_WINDOWS
	if (
#ifdef ALLEGRO_WINDOWS
			(al_get_display_flags(display) & ALLEGRO_DIRECT3D) &&
#endif
			(al_get_bitmap_flags(bitmap) & ALLEGRO_NO_PRESERVE_TEXTURE) &&
			!(al_get_bitmap_flags(bitmap) & ALLEGRO_MEMORY_BITMAP)
	) {
		LoadedBitmap *lb = new LoadedBitmap;
		null_lb(lb);
		lb->load_type = LOAD_LOAD;
		lb->flags = al_get_bitmap_flags(bitmap);
		lb->format = al_get_bitmap_format(bitmap);
		lb->load.filename = name;
		lb->load.redraw = redraw;
		lb->load.data = data;
		lb->bitmap = m;
		lb->delayed = delayed;
		loaded_bitmaps.push_back(lb);
	}
#endif

	return m;
}


MBITMAP *m_load_alpha_bitmap(const char *name, bool force_memory)
{
	ALLEGRO_BITMAP *bitmap = 0;

	int old = al_get_new_bitmap_format();
	al_set_new_bitmap_format(ALPHA_FMT);

	if (!force_memory)
		bitmap = my_load_bitmap(name);

	if (!bitmap) {
		ALLEGRO_STATE s;
		al_store_state(&s, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS);
		al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
		bitmap = my_load_bitmap(name);
		al_restore_state(&s);
	}

	al_set_new_bitmap_format(old);

	if (!bitmap) {
		return NULL;
	}

	MBITMAP *m = new_mbitmap(bitmap);

#if defined ALLEGRO_ANDROID || defined ALLEGRO_WINDOWS
	if (
#ifdef ALLEGRO_WINDOWS
			(al_get_display_flags(display) & ALLEGRO_DIRECT3D) &&
#endif
			(al_get_bitmap_flags(bitmap) & ALLEGRO_NO_PRESERVE_TEXTURE) &&
			!(al_get_bitmap_flags(bitmap) & ALLEGRO_MEMORY_BITMAP)
	) {
		LoadedBitmap *lb = new LoadedBitmap;
		null_lb(lb);
		lb->load_type = LOAD_LOAD;
		lb->flags = al_get_bitmap_flags(bitmap);
		lb->format = al_get_bitmap_format(bitmap);
		lb->load.filename = name;
		lb->load.redraw = NULL;
		lb->load.data = NULL;
		lb->bitmap = m;
		loaded_bitmaps.push_back(lb);
	}
#endif

	return m;
}


MFONT *m_load_font(const char *name)
{
	ALLEGRO_FONT *f;

	int flags = al_get_new_bitmap_flags();
	al_set_new_bitmap_flags(flags & ~ALLEGRO_NO_PRESERVE_TEXTURE);

	f = al_load_font(name, 0, 0);
	if (f) {
		al_set_new_bitmap_flags(flags);
		return f;
	}

	debug_message("couldn't load font: using mem bitmaps");

	al_set_new_bitmap_flags(flags|ALLEGRO_MEMORY_BITMAP);
	f = al_load_font(name, 0, 0);
	al_set_new_bitmap_flags(flags);
	return f;
}


void my_clear_bitmap(MBITMAP *b)
{
	ALLEGRO_BITMAP *target = al_get_target_bitmap();
	m_set_target_bitmap(b);
	m_clear(al_map_rgba(0, 0, 0, 0));
	al_set_target_bitmap(target);
}


MBITMAP *m_create_bitmap(int w, int h, void (*create)(MBITMAP *bitmap, RecreateData *), RecreateData *data, void (*destroy)(MBITMAP *b), bool delayed) // check
{
	ALLEGRO_BITMAP *bitmap = NULL;
	int flags = al_get_new_bitmap_flags();
	
	int new_flags = flags;

	if (!config.getUseOnlyMemoryBitmaps()) {
		al_set_new_bitmap_flags(new_flags);
		bitmap = my_al_create_bitmap(w, h);
	}
	if (!bitmap) {
		al_set_new_bitmap_flags(new_flags|ALLEGRO_MEMORY_BITMAP);
		bitmap = my_al_create_bitmap(w, h);
	}

	al_set_new_bitmap_flags(flags);
		
	if (!bitmap) {
		printf("error creating bitmap\n");
		return NULL;
	}

	MBITMAP *m = new_mbitmap(bitmap);

	my_clear_bitmap(m);

	if (create) {
		create(m, data);
	}

#if defined ALLEGRO_ANDROID || defined ALLEGRO_WINDOWS
	if (
#ifdef ALLEGRO_WINDOWS
			(al_get_display_flags(display) & ALLEGRO_DIRECT3D) &&
#endif
			(al_get_bitmap_flags(bitmap) & ALLEGRO_NO_PRESERVE_TEXTURE) &&
			!(al_get_bitmap_flags(bitmap) & ALLEGRO_MEMORY_BITMAP)
	) {
		LoadedBitmap *lb = new LoadedBitmap;
		null_lb(lb);
		lb->load_type = LOAD_CREATE;
		lb->flags = al_get_bitmap_flags(bitmap);
		lb->format = al_get_bitmap_format(bitmap);
		lb->destroy.func = destroy;
		lb->recreate.func = create;
		lb->recreate.data = data;
		lb->recreate.w = w;
		lb->recreate.h = h;
		lb->bitmap = m;
		lb->delayed = delayed;
		loaded_bitmaps.push_back(lb);
	}
	else {
#endif
	if (data) {
		delete data;
	}
#if defined ALLEGRO_ANDROID || defined ALLEGRO_WINDOWS
	}
#endif

	return m;
}


void m_destroy_bitmap(MBITMAP *bmp, bool internals_only)
{
	for (size_t i = 0; i < loaded_bitmaps.size(); i++) {
		if (loaded_bitmaps[i]->bitmap == bmp) {
			if (loaded_bitmaps[i]->load_type == LOAD_CREATE && loaded_bitmaps[i]->recreate.data) {
				delete loaded_bitmaps[i]->recreate.data;
			}
			delete loaded_bitmaps[i];
			loaded_bitmaps.erase(loaded_bitmaps.begin()+i);
			break;
		}
	}

	if (bmp->bitmap) {
		al_destroy_bitmap(bmp->bitmap);
		bmp->bitmap = NULL;
	}
	
	if (!internals_only) {
		delete bmp;
	}
}


void m_flip_display(void)
{
	bool skip_flip = false;

	if (prompt_for_close_on_next_flip) {
		prompt_for_close_on_next_flip = false;
		prepareForScreenGrab2();
		bool hidden = is_cursor_hidden();
		show_mouse_cursor();
		int r = triple_prompt("", "Really quit game or return to menu?", "", "Menu", "Quit", "Cancel", 2, true);
		if (hidden) {
			hide_mouse_cursor();
		}
		if (r == 0) {
			break_main_loop = true;
		}
		else if (r == 1) {
			do_close_exit_game();
		}
		skip_flip = true;
	}
	else if (show_item_info_on_flip >= 0) {
		int tmp = show_item_info_on_flip;
		show_item_info_on_flip = -1;
		prepareForScreenGrab2();
		showItemInfo(tmp, true);
		skip_flip = true;
	}
	else if (show_player_info_on_flip) {
		show_player_info_on_flip = false;
		prepareForScreenGrab2();
		if (player_to_show_on_flip) {
			showPlayerInfo_ptr(player_to_show_on_flip);
			player_to_show_on_flip = NULL;
		}
		else {
			showPlayerInfo_number(player_number_to_show_on_flip);
		}
		skip_flip = true;
	}
	else if (close_pressed_for_configure) {
		close_pressed_for_configure = false;
		int _dx, _dy, _dw, _dh;
		get_screen_offset_size(&_dx, &_dy, &_dw, &_dh);
		ALLEGRO_STATE state;
		al_store_state(&state, ALLEGRO_STATE_TARGET_BITMAP | ALLEGRO_STATE_NEW_BITMAP_PARAMETERS);
		al_set_new_bitmap_format(al_get_bitmap_format(tmpbuffer->bitmap));
		al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
		ALLEGRO_BITMAP *tmp = al_create_bitmap(
			al_get_bitmap_width(tmpbuffer->bitmap),
			al_get_bitmap_height(tmpbuffer->bitmap)
		);
		al_set_target_bitmap(tmp);
		al_clear_to_color(black);
		quick_draw(tmpbuffer->bitmap, 0, 0, 0);
		al_restore_state(&state);
		config_menu();
		int __dx, __dy, __dw, __dh;
		get_screen_offset_size(&__dx, &__dy, &__dw, &__dh);
		al_store_state(&state, ALLEGRO_STATE_TARGET_BITMAP);
		al_set_target_bitmap(tmpbuffer->bitmap);
		al_clear_to_color(black);
		quick_draw(
			tmp,
			_dx, _dy, _dw, _dh,
			__dx, __dy, __dw, __dh,
			0
		);
		al_restore_state(&state);
		al_destroy_bitmap(tmp);
	}

	if (!skip_flip) {
		al_flip_display();
	}

	int xxx, yyy, www, hhh;
	al_get_clipping_rectangle(&xxx, &yyy, &www, &hhh);
	al_set_clipping_rectangle(
		0, 0,
		al_get_display_width(display),
		al_get_display_height(display)
	);
	m_clear(black);
	al_set_clipping_rectangle(xxx, yyy, www, hhh);

	fps_frames++;
	double elapsed = al_get_time() - fps_counter;
	if (fps_on && elapsed > 2) {
		fps = (int)(round((float)fps_frames/elapsed));
		fps_counter = al_get_time();
		fps_frames = 0;
	}
}

void m_draw_circle(int x, int y, int radius, MCOLOR color,
	int flags)
{
	if (flags & M_FILLED) {
		al_draw_filled_circle(x, y, radius, color);
	}
	else {
		al_draw_circle(x, y, radius, color, 1);
	}
}


void m_rest(double seconds)
{
	int i = (int)seconds;
	double remainder = seconds-i;

	for (int j = 0; j < i; j++) {
		al_rest(1);
		if (is_close_pressed()) {
			do_close();
			close_pressed = false;
		}
		if (break_main_loop) {
			break;
		}
	}

	al_rest(remainder);
}


int m_get_display_width(void)
{
	return al_get_display_width(display);
}


int m_get_display_height(void)
{
	return al_get_display_height(display);
}

void m_clear(MCOLOR color)
{
	al_clear_to_color(color);
}


static int m_text_length_real(const MFONT *font, const char *text)
{
	if (font != game_font) {
		return al_get_text_width(font, text);
	}
	return al_get_text_width(font, text) / (screenScaleX < screenScaleY ? (screenScaleX / textScaleX) : (screenScaleY / textScaleY));
}

void m_set_target_bitmap(MBITMAP *bmp)
{
	al_set_target_bitmap(bmp->bitmap);
}


void m_set_clip(int x1, int y1, int x2, int y2)
{
	int dx, dy, dw, dh;
	get_screen_offset_size(&dx, &dy, &dw, &dh);
	ALLEGRO_BITMAP *target = al_get_target_bitmap();
	if (al_get_bitmap_width(target) == al_get_display_width(display) && al_get_bitmap_height(target) == al_get_display_height(display)) {
		al_set_clipping_rectangle(dx+x1*screenScaleX, dy+y1*screenScaleY, (x2-x1)*screenScaleX, (y2-y1)*screenScaleY);
	}
	else {
		al_set_clipping_rectangle(
			x1, y1, x2-x1, y2-y1
		);
	}
}



void m_put_pixel(int x, int y, MCOLOR color)
{
	al_put_pixel(x, y, color);
}

void m_draw_trans_pixel(int x, int y, MCOLOR color)
{
	m_draw_rectangle(x, y, x+1, y+1, color, M_FILLED);
}

// -angle?!?! really?!
void m_draw_tinted_rotated_bitmap(MBITMAP *bitmap, MCOLOR tint, int cx, int cy,
	int dx, int dy, float angle, int flags)
{
	quick_draw(bitmap->bitmap, tint, cx, cy, dx, dy, -angle, flags);
}

void m_draw_rotated_bitmap(MBITMAP *bitmap, int cx, int cy,
	int dx, int dy, float angle, int flags)
{
	m_draw_tinted_rotated_bitmap(bitmap, white, cx, cy, dx, dy, angle, flags);
}

void m_fill_ellipse(int x, int y, int rx, int ry, MCOLOR color)
{
	if (rx <= 0 || ry <= 0) return;
	al_draw_filled_ellipse(x, y, rx, ry, color);
}

void m_draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3, MCOLOR c)
{
	al_draw_filled_triangle(x1, y1, x2, y2, x3, y3, c);
}

MBITMAP *create_trapezoid(int dir, int topw, int bottomw, int length, MCOLOR color)
{
	MBITMAP *b;

	color.a = 1.0f;
	
	m_push_target_bitmap();

	int flgs = al_get_new_bitmap_flags();
	al_set_new_bitmap_flags(flgs & ~ALLEGRO_NO_PRESERVE_TEXTURE);
	
	if (dir == DIRECTION_NORTH) {
		b = m_create_bitmap(bottomw, length); // check
	}
	else if (dir == DIRECTION_EAST) {
		b = m_create_bitmap(length, bottomw); // check
	}
	else if (dir == DIRECTION_SOUTH) {
		b = m_create_bitmap(bottomw, length); // check
	}
	else {
		b = m_create_bitmap(length, bottomw); // check
	}

	al_set_new_bitmap_flags(flgs);

	m_set_target_bitmap(b);
	m_clear(m_map_rgba(0, 0, 0, 0));

	float y = get_trap_peak(topw, bottomw, length);

	if (dir == DIRECTION_NORTH) {
		m_draw_triangle(bottomw/2, length+y-1, 0, 0, bottomw-1, 0, color);
	}
	else if (dir == DIRECTION_EAST) {
		m_draw_triangle(-y, bottomw/2, length-1, 0, length-1, bottomw-1, color);
	}
	else if (dir == DIRECTION_SOUTH) {
		m_draw_triangle(bottomw/2, -y, bottomw-1, length-1, 0, length-1, color);
	}
	else if (dir == DIRECTION_WEST) {
		m_draw_triangle(length+y-1, bottomw/2, 0, 0, 0, bottomw-1, color);
	}

	m_pop_target_bitmap();

	return b;
}



void m_draw_trans_bitmap(MBITMAP *b, int x, int y, int alpha)
{
	m_draw_tinted_bitmap(b, m_map_rgba(alpha, alpha, alpha, alpha), x, y, 0);
}


void m_destroy_font(MFONT *f)
{
	al_destroy_font(f);
}

MBITMAP *m_clone_bitmap(MBITMAP *b)
{
	ALLEGRO_BITMAP *bmp = al_clone_bitmap(b->bitmap);
	MBITMAP *m = new_mbitmap(bmp);
	return m;
}

MBITMAP *m_make_display_bitmap(MBITMAP *b)
{
	if (!b) return NULL;
	MBITMAP *bmp = m_clone_bitmap(b);
	m_destroy_bitmap(b);
	return bmp;
}

MBITMAP *m_make_alpha_display_bitmap(MBITMAP *in)
{
	int old = al_get_new_bitmap_format();
	al_set_new_bitmap_format(ALPHA_FMT);

	MBITMAP *bmp = m_clone_bitmap(in);

	al_set_new_bitmap_format(old);

	m_destroy_bitmap(in);

	return bmp;
}



static int find_char(const char *text)
{
	int i = 0;

	while (text[i]) {
		if (text[i] == '{') {
			return i;
		}
		i++;
	}

	return -1;
}

// find length taking into account {xxx} special icon codes
int m_text_length(const MFONT *font, const char *text)
{
	char part[100];
	int n;
	const char *p = text;
	int length = 0;

	while ((n = find_char(p)) >= 0) {
		int i;
		for (i = 0; i < n; i++) {
			part[i] = p[i];
		}
		part[i] = 0;
		length += m_text_length_real(font, part) + m_text_height(font) + 2;
		p += n+5;
	}

	length += m_text_length_real(font, p);

	return length;
}

static ALLEGRO_BITMAP *pushed_target;

void m_push_target_bitmap(void)
{
	pushed_target = al_get_target_bitmap();
}

void m_pop_target_bitmap(void)
{
	al_set_target_bitmap(pushed_target);
}

void m_get_mouse_state(ALLEGRO_MOUSE_STATE *s)
{
	al_get_mouse_state(s);
	if (!config.getMaintainAspectRatio())
		tguiConvertMousePosition(&s->x, &s->y, 0, 0, screen_ratio_x, screen_ratio_y);
	else
		tguiConvertMousePosition(&s->x, &s->y, screen_offset_x, screen_offset_y, 1, 1);
}

float my_get_opengl_version(void)
{
   const char *s = (char *)glGetString(GL_VERSION);
   if (strstr(s, "2.0")) {
      return 2.0;
   }
   else if (strstr(s, "1.1"))
      return 1.5;
   else
      return 1.3;
}

void m_draw_prim (const void* vtxs, const ALLEGRO_VERTEX_DECL* decl, MBITMAP* texture, int start, int end, int type)
{
	if (type == ALLEGRO_PRIM_POINT_LIST) {
		int n = end-start;
		ALLEGRO_VERTEX *v = new ALLEGRO_VERTEX[n*6];
		ALLEGRO_VERTEX *verts = (ALLEGRO_VERTEX *)vtxs;
		for (int i = 0; i < n; i++) {
			v[i*6+0].x = verts[i+start].x;
			v[i*6+0].y = verts[i+start].y;
			v[i*6+0].z = 0;
			v[i*6+0].color = verts[i+start].color;
			v[i*6+1].x = verts[i+start].x+1;
			v[i*6+1].y = verts[i+start].y;
			v[i*6+1].z = 0;
			v[i*6+1].color = verts[i+start].color;
			v[i*6+2].x = verts[i+start].x+1;
			v[i*6+2].y = verts[i+start].y+1;
			v[i*6+2].z = 0;
			v[i*6+2].color = verts[i+start].color;
			v[i*6+3].x = verts[i+start].x;
			v[i*6+3].y = verts[i+start].y+1;
			v[i*6+3].z = 0;
			v[i*6+3].color = verts[i+start].color;
			v[i*6+4].x = verts[i+start].x;
			v[i*6+4].y = verts[i+start].y;
			v[i*6+4].z = 0;
			v[i*6+4].color = verts[i+start].color;
			v[i*6+5].x = verts[i+start].x+1;
			v[i*6+5].y = verts[i+start].y+1;
			v[i*6+5].z = 0;
			v[i*6+5].color = verts[i+start].color;
		}
		al_draw_prim(v, decl, (texture ? texture->bitmap : NULL), start, n*6, ALLEGRO_PRIM_TRIANGLE_LIST);
		delete[] v;
	}
	else if (type == ALLEGRO_PRIM_LINE_LIST) {
		int n = (end-start)/2;
		ALLEGRO_VERTEX *v = new ALLEGRO_VERTEX[n*6];
		ALLEGRO_VERTEX *verts = (ALLEGRO_VERTEX *)vtxs;
		for (int i = 0; i < n; i++) {
			float x1 = verts[(i*2+0)+start].x;
			float y1 = verts[(i*2+0)+start].y;
			float x2 = verts[(i*2+1)+start].x;
			float y2 = verts[(i*2+1)+start].y;
			float dx = x2 - x1;
			float dy = y2 - y1;
			float a = atan2(dy, dx);
			float a1 = a + M_PI/2;
			float a2 = a - M_PI/2;
			float newx1 = x1 + cos(a1)/2;
			float newy1 = y1 + sin(a1)/2;
			float newx2 = x1 + cos(a2)/2;
			float newy2 = y1 + sin(a2)/2;
			float newx3 = x2 + cos(a1)/2;
			float newy3 = y2 + sin(a1)/2;
			float newx4 = x2 + cos(a2)/2;
			float newy4 = y2 + sin(a2)/2;
			float u1 = verts[(i*2+0)+start].u;
			float v1 = verts[(i*2+0)+start].v;
			float u2 = verts[(i*2+1)+start].u;
			float v2 = verts[(i*2+1)+start].v;
			v[i*6+0].x = newx1;
			v[i*6+0].y = newy1;
			v[i*6+0].z = 0;
			v[i*6+0].u = u1;
			v[i*6+0].v = v1;
			v[i*6+0].color = verts[(i*2+0)+start].color;
			v[i*6+1].x = newx3;
			v[i*6+1].y = newy3;
			v[i*6+1].z = 0;
			v[i*6+1].color = verts[(i*2+0)+start].color;
			v[i*6+1].u = u2;
			v[i*6+1].v = v2;
			v[i*6+2].x = newx4;
			v[i*6+2].y = newy4;
			v[i*6+2].z = 0;
			v[i*6+2].color = verts[(i*2+0)+start].color;
			v[i*6+2].u = u2;
			v[i*6+2].v = v2;
			v[i*6+3].x = newx1;
			v[i*6+3].y = newy1;
			v[i*6+3].z = 0;
			v[i*6+3].color = verts[(i*2+0)+start].color;
			v[i*6+3].u = u1;
			v[i*6+3].v = v1;
			v[i*6+4].x = newx2;
			v[i*6+4].y = newy2;
			v[i*6+4].z = 0;
			v[i*6+4].color = verts[(i*2+0)+start].color;
			v[i*6+4].u = u1;
			v[i*6+4].v = v1;
			v[i*6+5].x = newx4;
			v[i*6+5].y = newy4;
			v[i*6+5].z = 0;
			v[i*6+5].color = verts[(i*2+0)+start].color;
			v[i*6+5].u = u2;
			v[i*6+5].v = v2;
		}
		al_draw_prim(v, decl, (texture ? texture->bitmap : NULL), start, n*6, ALLEGRO_PRIM_TRIANGLE_LIST);
		delete[] v;
	}
	else
		al_draw_prim(vtxs, decl, (texture ? texture->bitmap : NULL), start, end, type);
}

MBITMAP *m_create_sub_bitmap(MBITMAP *parent, int x, int y, int w, int h) // check
{
	ALLEGRO_BITMAP *sub = al_create_sub_bitmap(parent->bitmap, x, y, w, h);
	MBITMAP *b = new_mbitmap(sub);
	return b;
}

ALLEGRO_LOCKED_REGION *m_lock_bitmap(MBITMAP *b, int format, int flags)
{
	return al_lock_bitmap(b->bitmap, format, flags);
}

ALLEGRO_LOCKED_REGION *m_lock_bitmap_region(MBITMAP *b, int x, int y, int w, int h, int format, int flags)
{
	return al_lock_bitmap_region(b->bitmap, x, y, w, h, format, flags);
}

void m_unlock_bitmap(MBITMAP *b)
{
	al_unlock_bitmap(b->bitmap);
}

void _destroy_loaded_bitmaps(void)
{
	if (cached_bitmap) {
		al_destroy_bitmap(cached_bitmap);
		cached_bitmap = NULL;
		cached_bitmap_filename = "";
	}

	for (size_t i = 0; i < loaded_bitmaps.size(); i++) {
		if (!(loaded_bitmaps[i]->flags & ALLEGRO_NO_PRESERVE_TEXTURE)) {
			continue;
		}
		if (loaded_bitmaps[i]->load_type == LOAD_CREATE && loaded_bitmaps[i]->destroy.func) {
			(*loaded_bitmaps[i]->destroy.func)(loaded_bitmaps[i]->bitmap);
		}
		else {
			MBITMAP *m = loaded_bitmaps[i]->bitmap;
			al_destroy_bitmap(m->bitmap);
			m->bitmap = NULL;
		}
	}
}

void _reload_loaded_bitmaps(void)
{
	int flags = al_get_new_bitmap_flags();
	int format = al_get_new_bitmap_format();

	for (size_t i = 0; i < loaded_bitmaps.size(); i++) {
		MBITMAP *m = loaded_bitmaps[i]->bitmap;
		if ((loaded_bitmaps[i]->flags & ALLEGRO_NO_PRESERVE_TEXTURE) && !loaded_bitmaps[i]->delayed) {
			al_set_new_bitmap_flags(loaded_bitmaps[i]->flags);
			al_set_new_bitmap_format(loaded_bitmaps[i]->format);
			if (loaded_bitmaps[i]->load_type == LOAD_LOAD) {
				m->bitmap = my_load_bitmap(loaded_bitmaps[i]->load.filename.c_str());
				if (loaded_bitmaps[i]->load.redraw) {
					loaded_bitmaps[i]->load.redraw(m, loaded_bitmaps[i]->load.data);
				}
			}
			else { // create
				Recreate *d = &loaded_bitmaps[i]->recreate;
				m->bitmap = my_al_create_bitmap(d->w, d->h);
				if (d->func) { // recreate with func
					d->func(m, d->data);
				}
			}
			al_use_shader(NULL);
		}
	}

	al_set_new_bitmap_flags(flags);
	al_set_new_bitmap_format(format);
}

void _reload_loaded_bitmaps_delayed(void)
{
	int flags = al_get_new_bitmap_flags();
	int format = al_get_new_bitmap_format();

	for (size_t i = 0; i < loaded_bitmaps.size(); i++) {
		MBITMAP *m = loaded_bitmaps[i]->bitmap;
		if ((loaded_bitmaps[i]->flags & ALLEGRO_NO_PRESERVE_TEXTURE) && loaded_bitmaps[i]->delayed) {
			al_set_new_bitmap_flags(loaded_bitmaps[i]->flags);
			al_set_new_bitmap_format(loaded_bitmaps[i]->format);
			if (loaded_bitmaps[i]->load_type == LOAD_LOAD) {
				m->bitmap = my_load_bitmap(loaded_bitmaps[i]->load.filename.c_str());
				if (loaded_bitmaps[i]->load.redraw) {
					loaded_bitmaps[i]->load.redraw(m, loaded_bitmaps[i]->load.data);
				}
			}
			else { // create
				Recreate *d = &loaded_bitmaps[i]->recreate;
				m->bitmap = my_al_create_bitmap(d->w, d->h);
				if (d->func) { // recreate with func
					d->func(m, d->data);
				}
			}
			al_use_shader(NULL);
		}
	}

	al_set_new_bitmap_flags(flags);
	al_set_new_bitmap_format(format);
}

static MBITMAP *clone_sub_bitmap(MBITMAP *b)
{
	ALLEGRO_STATE st;
	al_store_state(&st, ALLEGRO_STATE_TARGET_BITMAP | ALLEGRO_STATE_BLENDER | ALLEGRO_STATE_NEW_BITMAP_PARAMETERS);

	MBITMAP *clone = m_create_alpha_bitmap(
		m_get_bitmap_width(b),
		m_get_bitmap_height(b)
	);

	m_set_target_bitmap(clone);
	m_clear(al_map_rgba_f(0, 0, 0, 0));

	m_draw_bitmap(b, 0, 0, 0);

	al_restore_state(&st);

	return clone;
}

void m_draw_bitmap_to_self(MBITMAP *b, int x, int y, int flags)
{
	MBITMAP *tmp = clone_sub_bitmap(b);
	m_draw_bitmap(tmp, x, y, flags);
	m_destroy_bitmap(tmp);
}

void m_draw_bitmap_region_to_self(MBITMAP *b, int sx, int sy, int sw, int sh, int dx, int dy, int flags)
{
	MBITMAP *tmp = clone_sub_bitmap(b);
	m_draw_bitmap_region(tmp, sx, sy, sw, sh, dx, dy, flags);
	m_destroy_bitmap(tmp);
}

void m_draw_scaled_backbuffer(int sx, int sy, int sw, int sh, int dx, int dy, int dw, int dh, MBITMAP *dest)
{
	ALLEGRO_BITMAP *old_target = al_get_target_bitmap();
	int old_format = al_get_new_bitmap_format();
	al_set_new_bitmap_format(al_get_bitmap_format(al_get_backbuffer(display)));
	MBITMAP *tmp = m_create_bitmap(sw, sh);
	int scr_w = al_get_display_width(display);
	int scr_h = al_get_display_height(display);
	if (sx+sw > scr_w) {
		sw = scr_w-sx;
	}
	else if (sx < 0) {
		sw -= sx;
		sx = 0;
	}
	if (sy+sh > scr_h) {
		sh = scr_h-sy;
	}
	else if (sy < 0) {
		sh -= sy;
		sy = 0;
	}

#if defined ALLEGRO_RASPBERRYPI || defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
	ALLEGRO_LOCKED_REGION *lr1 = al_lock_bitmap(tmp->bitmap, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_WRITEONLY);
	ALLEGRO_LOCKED_REGION *lr2 = al_lock_bitmap_region(
		al_get_backbuffer(display),
		sx, sy, sw, sh,
		ALLEGRO_PIXEL_FORMAT_ANY,
		ALLEGRO_LOCK_READONLY
	);
	int pixel_size = al_get_pixel_size(al_get_bitmap_format(al_get_backbuffer(display)));
	for (int y = 0; y < sh; y++) {
		uint8_t *d1 = (uint8_t *)lr1->data + lr1->pitch * y;
		uint8_t *d2 = (uint8_t *)lr2->data + lr2->pitch * y;
		memcpy(d1, d2, pixel_size*sw);
	}
	al_unlock_bitmap(tmp->bitmap);
	al_unlock_bitmap(al_get_backbuffer(display));
#else
	m_set_target_bitmap(tmp);
	quick_draw(al_get_backbuffer(display), sx, sy, sw, sh, 0, 0, 0);
#endif
	m_set_target_bitmap(dest);
	quick_draw(
		tmp->bitmap,
		0, 0, sw, sh,
		dx, dy, dw, dh,
		0
	);
	m_destroy_bitmap(tmp);
	al_set_target_bitmap(old_target);
	al_set_new_bitmap_format(old_format);
}

void m_draw_tinted_bitmap_identity_view(MBITMAP *bmp, MCOLOR tint, int x, int y, int flags)
{
	ALLEGRO_TRANSFORM backup, t;
	al_copy_transform(&backup, al_get_current_transform());
	al_identity_transform(&t);
	al_use_transform(&t);
	m_draw_tinted_bitmap(bmp, tint, x, y, flags);
	al_use_transform(&backup);
}

void m_draw_bitmap_identity_view(MBITMAP *bmp, int x, int y, int flags)
{
	m_draw_tinted_bitmap_identity_view(bmp, white, x, y, flags);
}

void m_draw_tinted_scaled_bitmap_identity_view(
	MBITMAP *bmp, MCOLOR tint, int sx, int sy, int sw, int sh, int dx, int dy, int dw, int dh, int flags
)
{
	ALLEGRO_TRANSFORM backup, t;
	al_copy_transform(&backup, al_get_current_transform());
	al_identity_transform(&t);
	al_use_transform(&t);
	quick_draw(
		bmp->bitmap,
		tint,
		sx, sy, sw, sh,
		dx, dy, dw, dh,
		0
	);
	al_use_transform(&backup);
}

void m_draw_scaled_bitmap_identity_view(
	MBITMAP *bmp, int sx, int sy, int sw, int sh, int dx, int dy, int dw, int dh, int flags
)
{
	m_draw_tinted_scaled_bitmap_identity_view(bmp, white, sx, sy, sw, sh, dx, dy, dw, dh, flags);
}

void m_draw_scaled_target(MBITMAP *src, int sx, int sy, int sw, int sh,
	int dx, int dy, int dw, int dh, MBITMAP *dst)
{
	ALLEGRO_BITMAP *old_target = al_get_target_bitmap();
	m_set_target_bitmap(dst);
	quick_draw(src->bitmap, sx, sy, sw, sh, dx, dy, dw, dh, 0);
	al_set_target_bitmap(old_target);
}

static ALLEGRO_TRANSFORM prepareForScreenGrabBackup;
static ALLEGRO_BITMAP *prepareForScreenGrabBackupBitmap;
bool preparingForScreenGrab = false;

void prepareForScreenGrab1()
{
	prepareForScreenGrabBackupBitmap = al_get_target_bitmap();
	int dx, dy, dw, dh;
	get_screen_offset_size(&dx, &dy, &dw, &dh);
	m_set_target_bitmap(tmpbuffer);
	ALLEGRO_TRANSFORM t;
	al_copy_transform(&prepareForScreenGrabBackup, al_get_current_transform());
	al_identity_transform(&t);
	al_scale_transform(&t, screenScaleX, screenScaleY);
	al_translate_transform(&t, dx, dy);
	al_use_transform(&t);
	preparingForScreenGrab = true;
}

void prepareForScreenGrab2()
{
	m_set_target_bitmap(tmpbuffer);
	al_use_transform(&prepareForScreenGrabBackup);
	al_set_target_bitmap(prepareForScreenGrabBackupBitmap);
	preparingForScreenGrab = false;
}

void set_target_backbuffer()
{
	if (!prompt_for_close_on_next_flip && show_item_info_on_flip < 0 && !show_player_info_on_flip && !close_pressed_for_configure) {
		al_set_target_backbuffer(display);
	}
}

void m_set_mouse_xy(ALLEGRO_DISPLAY *display, int x, int y)
{
	al_set_mouse_xy(display, x, y);
}

void use_shader(ALLEGRO_SHADER *shader)
{
	al_use_shader(shader);
}

int cursor_offset(bool centered)
{
	if (centered) {
		return -m_get_bitmap_height(cursor)/2;
	}
	return m_text_height(game_font)/2-m_get_bitmap_height(cursor)/2;
}

void my_get_keyboard_state(ALLEGRO_KEYBOARD_STATE *state)
{
	al_get_keyboard_state(state);
}

static ALLEGRO_VERTEX *vertex_cache;
static int cache_size = 0;
static ALLEGRO_BITMAP *cache_bitmap;
static bool caching;
static int vcount = 0;

static ALLEGRO_BITMAP *real_bitmap(ALLEGRO_BITMAP *bitmap)
{
	if (bitmap == 0) {
		return 0;
	}
	ALLEGRO_BITMAP *parent = bitmap;
	while (al_is_sub_bitmap(parent)) {
		parent = al_get_parent_bitmap(parent);
	}
	return parent;
}

static void quick_resize_cache(int num_verts)
{
	if (vcount + num_verts <= cache_size) {
		return;
	}

	int resize_amount = ((num_verts / 256) + 1) * 256;

	cache_size += resize_amount;

	vertex_cache = (ALLEGRO_VERTEX *)realloc(vertex_cache, cache_size * sizeof(ALLEGRO_VERTEX));
}

static void quick_flush()
{
	if (vcount > 0) {
		al_draw_prim(vertex_cache, 0, cache_bitmap, 0, vcount, ALLEGRO_PRIM_TRIANGLE_LIST);
		vcount = 0;
	}
	cache_bitmap = 0;
}

void quick(bool onoff)
{
	if (caching == onoff) {
		return;
	}

	caching = onoff;

	if (onoff == false) {
		quick_flush();
	}
}

bool is_quick_on()
{
	return caching;
}

void quick_draw(ALLEGRO_BITMAP *bitmap, float sx, float sy, float sw, float sh, ALLEGRO_COLOR tint, float cx, float cy, float dx, float dy, float xscale, float yscale, float angle, int flags)
{
	ALLEGRO_BITMAP *real = real_bitmap(bitmap);
	if (cache_bitmap != real) {
		quick_flush();
		cache_bitmap = real;
	}

	quick_resize_cache(6);

	ALLEGRO_VERTEX *v = &vertex_cache[vcount];

	sx += al_get_bitmap_x(bitmap);
	sy += al_get_bitmap_y(bitmap);
	int u1 = sx;
	int u2 = sx + sw;
	int v1 = sy;
	int v2 = sy + sh;

	if (flags & ALLEGRO_FLIP_HORIZONTAL) {
		int tmp = u1;
		u1 = u2;
		u2 = tmp;
	}
	if (flags & ALLEGRO_FLIP_VERTICAL) {
		int tmp = v1;
		v1 = v2;
		v2 = tmp;
	}

	ALLEGRO_TRANSFORM t;
	al_identity_transform(&t);
	al_translate_transform(&t, -cx, -cy);
	al_scale_transform(&t, xscale, yscale);
	al_rotate_transform(&t, angle);
	al_translate_transform(&t, dx, dy);

	float x1 = 0;
	float y1 = 0;
	float x2 = sw;
	float y2 = 0;
	float x3 = sw;
	float y3 = sh;
	float x4 = 0;
	float y4 = sh;

	al_transform_coordinates(&t, &x1, &y1);
	al_transform_coordinates(&t, &x2, &y2);
	al_transform_coordinates(&t, &x3, &y3);
	al_transform_coordinates(&t, &x4, &y4);

	v[0].x = x1;
	v[0].y = y1;
	v[0].z = 0;
	v[0].u = u1;
	v[0].v = v1;
	v[0].color = tint;

	v[1].x = x2;
	v[1].y = y2;
	v[1].z = 0;
	v[1].u = u2;
	v[1].v = v1;
	v[1].color = tint;
	
	v[2].x = x3;
	v[2].y = y3;
	v[2].z = 0;
	v[2].u = u2;
	v[2].v = v2;
	v[2].color = tint;

	v[3].x = x1;
	v[3].y = y1;
	v[3].z = 0;
	v[3].u = u1;
	v[3].v = v1;
	v[3].color = tint;

	v[4].x = x3;
	v[4].y = y3;
	v[4].z = 0;
	v[4].u = u2;
	v[4].v = v2;
	v[4].color = tint;

	v[5].x = x4;
	v[5].y = y4;
	v[5].z = 0;
	v[5].u = u1;
	v[5].v = v2;
	v[5].color = tint;

	vcount += 6;

	if (caching == false) {
		quick_flush();
	}
}

void quick_draw(ALLEGRO_BITMAP *bitmap, float sx, float sy, float sw, float sh, float cx, float cy, float dx, float dy, float xscale, float yscale, float angle, int flags)
{
	quick_draw(bitmap, sx, sy, sw, sh, al_map_rgb(255, 255, 255), cx, cy, dx, dy, xscale, yscale, angle, flags);
}

void quick_draw(ALLEGRO_BITMAP *bitmap, ALLEGRO_COLOR tint, float cx, float cy, float dx, float dy, float xscale, float yscale, float angle, int flags)
{
	quick_draw(bitmap, 0, 0, al_get_bitmap_width(bitmap), al_get_bitmap_height(bitmap), tint, cx, cy, dx, dy, xscale, yscale, angle, flags);
}

void quick_draw(ALLEGRO_BITMAP *bitmap, float cx, float cy, float dx, float dy, float xscale, float yscale, float angle, int flags)
{
	quick_draw(bitmap, 0, 0, al_get_bitmap_width(bitmap), al_get_bitmap_height(bitmap), al_map_rgb(255, 255, 255), cx, cy, dx, dy, xscale, yscale, angle, flags);
}

void quick_draw(ALLEGRO_BITMAP *bitmap, ALLEGRO_COLOR tint, float cx, float cy, float dx, float dy, float angle, int flags)
{
	quick_draw(bitmap, 0, 0, al_get_bitmap_width(bitmap), al_get_bitmap_height(bitmap), tint, cx, cy, dx, dy, 1.0f, 1.0f, angle, flags);
}

void quick_draw(ALLEGRO_BITMAP *bitmap, float cx, float cy, float dx, float dy, float angle, int flags)
{
	quick_draw(bitmap, 0, 0, al_get_bitmap_width(bitmap), al_get_bitmap_height(bitmap), al_map_rgb(255, 255, 255), cx, cy, dx, dy, 1.0f, 1.0f, angle, flags);
}

void quick_draw(ALLEGRO_BITMAP *bitmap, ALLEGRO_COLOR tint, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh, int flags)
{
	ALLEGRO_BITMAP *real = real_bitmap(bitmap);
	if (cache_bitmap != real) {
		quick_flush();
		cache_bitmap = real;
	}

	quick_resize_cache(6);

	ALLEGRO_VERTEX *v = &vertex_cache[vcount];
	
	sx += al_get_bitmap_x(bitmap);
	sy += al_get_bitmap_y(bitmap);
	int u1 = sx;
	int u2 = sx + sw;
	int v1 = sy;
	int v2 = sy + sh;

	if (flags & ALLEGRO_FLIP_HORIZONTAL) {
		int tmp = u1;
		u1 = u2;
		u2 = tmp;
	}
	if (flags & ALLEGRO_FLIP_VERTICAL) {
		int tmp = v1;
		v1 = v2;
		v2 = tmp;
	}

	v[0].x = dx;
	v[0].y = dy;
	v[0].z = 0;
	v[0].u = u1;
	v[0].v = v1;
	v[0].color = tint;

	v[1].x = dx+dw;
	v[1].y = dy;
	v[1].z = 0;
	v[1].u = u2;
	v[1].v = v1;
	v[1].color = tint;
	
	v[2].x = dx+dw;
	v[2].y = dy+dh;
	v[2].z = 0;
	v[2].u = u2;
	v[2].v = v2;
	v[2].color = tint;

	v[3].x = dx;
	v[3].y = dy;
	v[3].z = 0;
	v[3].u = u1;
	v[3].v = v1;
	v[3].color = tint;

	v[4].x = dx+dw;
	v[4].y = dy+dh;
	v[4].z = 0;
	v[4].u = u2;
	v[4].v = v2;
	v[4].color = tint;

	v[5].x = dx;
	v[5].y = dy+dh;
	v[5].z = 0;
	v[5].u = u1;
	v[5].v = v2;
	v[5].color = tint;

	vcount += 6;

	if (caching == false) {
		quick_flush();
	}
}

void quick_draw(ALLEGRO_BITMAP *bitmap, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh, int flags)
{
	quick_draw(bitmap, al_map_rgb(255, 255, 255), sx, sy, sw, sh, dx, dy, dw, dh, flags);
}

void quick_draw(ALLEGRO_BITMAP *bitmap, ALLEGRO_COLOR tint, float sx, float sy, float sw, float sh, float dx, float dy, int flags)
{
	quick_draw(bitmap, tint, sx, sy, sw, sh, dx, dy, sw, sh, flags);
}

void quick_draw(ALLEGRO_BITMAP *bitmap, float sx, float sy, float sw, float sh, float dx, float dy, int flags)
{
	quick_draw(bitmap, al_map_rgb(255, 255, 255), sx, sy, sw, sh, dx, dy, sw, sh, flags);
}

void quick_draw(ALLEGRO_BITMAP *bitmap, ALLEGRO_COLOR tint, float dx, float dy, int flags)
{
	quick_draw(bitmap, tint, 0, 0, al_get_bitmap_width(bitmap), al_get_bitmap_height(bitmap), dx, dy, al_get_bitmap_width(bitmap), al_get_bitmap_height(bitmap), flags);
}

void quick_draw(ALLEGRO_BITMAP *bitmap, float dx, float dy, int flags)
{
	quick_draw(bitmap, al_map_rgb(255, 255, 255), 0, 0, al_get_bitmap_width(bitmap), al_get_bitmap_height(bitmap), dx, dy, al_get_bitmap_width(bitmap), al_get_bitmap_height(bitmap), flags);
}

void quick_cache(ALLEGRO_BITMAP *bitmap, ALLEGRO_COLOR tint, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, int flags)
{
	ALLEGRO_BITMAP *real = real_bitmap(bitmap);
	if (cache_bitmap != real) {
		quick_flush();
		cache_bitmap = real;
	}

	quick_resize_cache(6);

	ALLEGRO_VERTEX *v = &vertex_cache[vcount];

	int u1 = al_get_bitmap_x(bitmap);
	int u2 = u1 + al_get_bitmap_width(bitmap);
	int v1 = al_get_bitmap_y(bitmap);
	int v2 = v1 + al_get_bitmap_height(bitmap);

	if (flags & ALLEGRO_FLIP_HORIZONTAL) {
		int tmp = u1;
		u1 = u2;
		u2 = tmp;
	}
	if (flags & ALLEGRO_FLIP_VERTICAL) {
		int tmp = v1;
		v1 = v2;
		v2 = tmp;
	}

	v[0].x = x1;
	v[0].y = y1;
	v[0].z = 0;
	v[0].u = u1;
	v[0].v = v1;
	v[0].color = tint;

	v[1].x = x2;
	v[1].y = y2;
	v[1].z = 0;
	v[1].u = u2;
	v[1].v = v1;
	v[1].color = tint;
	
	v[2].x = x3;
	v[2].y = y3;
	v[2].z = 0;
	v[2].u = u2;
	v[2].v = v2;
	v[2].color = tint;

	v[3].x = x1;
	v[3].y = y1;
	v[3].z = 0;
	v[3].u = u1;
	v[3].v = v1;
	v[3].color = tint;

	v[4].x = x3;
	v[4].y = y3;
	v[4].z = 0;
	v[4].u = u2;
	v[4].v = v2;
	v[4].color = tint;

	v[5].x = x4;
	v[5].y = y4;
	v[5].z = 0;
	v[5].u = u1;
	v[5].v = v2;
	v[5].color = tint;

	vcount += 6;

	if (caching == false) {
		quick_flush();
	}
}

void quick_shutdown()
{
	free(vertex_cache);
}

#ifdef STOCK_ALLEGRO
bool ttf_is_quick()
{
	return al_is_bitmap_drawing_held();
}

void ttf_quick(bool onoff)
{
	al_hold_bitmap_drawing(onoff);
}
#endif
