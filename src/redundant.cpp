#include "monster2.hpp"

// FIXME: oh boy
#define ASSERT ALLEGRO_ASSERT
#include <allegro5/internal/aintern_bitmap.h>
#include <allegro5/internal/aintern.h>
#include <allegro5/internal/aintern_convert.h>
#include <allegro5/internal/aintern_display.h>

#ifdef A5_OGL
#include "allegro5/internal/aintern_opengl.h"
#endif

static std::vector<LoadedBitmap> loaded_bitmaps;

static float get_trap_peak(int topw, int bottomw, int length)
{
	int w = (bottomw - topw) / 2.0f;
	float a = atan((float)length/w);
	return tan(a) * (topw/2.0f);
}


/* my_do_line: copied from Allegro
 */
void my_do_line(int x1, int y1, int x2, int y2, void *data, void (*proc)(int, int, void *))
{
   int dx = x2-x1;
   int dy = y2-y1;
   int i1, i2;
   int x, y;
   int dd;

   /* worker macro */
   #define DO_LINE(pri_sign, pri_c, pri_cond, sec_sign, sec_c, sec_cond)     \
   {                                                                         \
      if (d##pri_c == 0) {                                                   \
	 proc(x1, y1, data);                                                 \
	 return;                                                             \
      }                                                                      \
									     \
      i1 = 2 * d##sec_c;                                                     \
      dd = i1 - (sec_sign (pri_sign d##pri_c));                              \
      i2 = dd - (sec_sign (pri_sign d##pri_c));                              \
									     \
      x = x1;                                                                \
      y = y1;                                                                \
									     \
      while (pri_c pri_cond pri_c##2) {                                      \
	 proc(x, y, data);                                                   \
									     \
	 if (dd sec_cond 0) {                                                \
	    sec_c = sec_c sec_sign 1;                                        \
	    dd += i2;                                                        \
	 }                                                                   \
	 else                                                                \
	    dd += i1;                                                        \
									     \
	 pri_c = pri_c pri_sign 1;                                           \
      }                                                                      \
   }

   if (dx >= 0) {
      if (dy >= 0) {
	 if (dx >= dy) {
	    /* (x1 <= x2) && (y1 <= y2) && (dx >= dy) */
	    DO_LINE(+, x, <=, +, y, >=);
	 }
	 else {
	    /* (x1 <= x2) && (y1 <= y2) && (dx < dy) */
	    DO_LINE(+, y, <=, +, x, >=);
	 }
      }
      else {
	 if (dx >= -dy) {
	    /* (x1 <= x2) && (y1 > y2) && (dx >= dy) */
	    DO_LINE(+, x, <=, -, y, <=);
	 }
	 else {
	    /* (x1 <= x2) && (y1 > y2) && (dx < dy) */
	    DO_LINE(-, y, >=, +, x, >=);
	 }
      }
   }
   else {
      if (dy >= 0) {
	 if (-dx >= dy) {
	    /* (x1 > x2) && (y1 <= y2) && (dx >= dy) */
	    DO_LINE(-, x, >=, +, y, >=);
	 }
	 else {
	    /* (x1 > x2) && (y1 <= y2) && (dx < dy) */
	    DO_LINE(+, y, <=, -, x, <=);
	 }
      }
      else {
	 if (-dx >= -dy) {
	    /* (x1 > x2) && (y1 > y2) && (dx >= dy) */
	    DO_LINE(-, x, >=, -, y, <=);
	 }
	 else {
	    /* (x1 > x2) && (y1 > y2) && (dx < dy) */
	    DO_LINE(-, y, >=, -, x, <=);
	 }
      }
   }

   #undef DO_LINE
}


/* Copied from do_circle
 *  Helper function for the circle drawing routines. Calculates the points
 *  in a circle of radius r around point x, y, and calls the specified 
 *  routine for each one. The output proc will be passed first a copy of
 *  the bmp parameter, then the x, y point, then a copy of the d parameter
 *  (so putpixel() can be used as the callback).
 */
void my_do_circle(int x, int y, int radius, MCOLOR d,
	void (*proc)(int, int, MCOLOR))
{
   int cx = 0;
   int cy = radius;
   int df = 1 - radius; 
   int d_e = 3;
   int d_se = -2 * radius + 5;

   do {
      proc(x+cx, y+cy, d); 

      if (cx) 
	 proc(x-cx, y+cy, d); 

      if (cy) 
	 proc(x+cx, y-cy, d);

      if ((cx) && (cy)) 
	 proc(x-cx, y-cy, d); 

      if (cx != cy) {
	 proc(x+cy, y+cx, d); 

	 if (cx) 
	    proc(x+cy, y-cx, d);

	 if (cy) 
	    proc(x-cy, y+cx, d); 

	 if (cx && cy) 
	    proc(x-cy, y-cx, d); 
      }

      if (df < 0)  {
	 df += d_e;
	 d_e += 2;
	 d_se += 2;
      }
      else { 
	 df += d_se;
	 d_e += 2;
	 d_se += 4;
	 cy--;
      } 

      cx++; 

   } while (cx <= cy);
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

MBITMAP *m_create_alpha_bitmap(int w, int h, void (*func)(MBITMAP *bitmap, RecreateData *), RecreateData *data) // check
{
	int f = al_get_new_bitmap_format();
	al_set_new_bitmap_format(ALPHA_FMT);
	ALLEGRO_BITMAP *b;
	int flags = al_get_new_bitmap_flags();

	ALLEGRO_DEBUG("in m_create_alpha_bitmap flags=%d", flags);
	
	if (config.getUseOnlyMemoryBitmaps()) {
		ALLEGRO_DEBUG("USE ONLY MEM BITMAPS");
		al_set_new_bitmap_flags(flags|ALLEGRO_MEMORY_BITMAP);
		b = al_create_bitmap(w, h);
	}
	else {
		ALLEGRO_DEBUG("NO don't use only mem bitmaps");
		b = al_create_bitmap(w, h);
	}
	
	al_set_new_bitmap_flags(flags);
	al_set_new_bitmap_format(f);

	MBITMAP *m = new_mbitmap(b);

	if (func) {
		func(m, data);
	}

	LoadedBitmap lb;
	lb.load_type = LOAD_CREATE;
	lb.flags = al_get_bitmap_flags(b);
	lb.format = al_get_bitmap_format(b);
	lb.recreate.func = func;
	lb.recreate.data = data;
	lb.recreate.w = w;
	lb.recreate.h = h;
	lb.bitmap = m;
	loaded_bitmaps.push_back(lb);

	return m;
}

std::stack<SAVED_BLENDER> blender_stack;
ALLEGRO_COLOR _blend_color;

void m_draw_alpha_bitmap(MBITMAP *b, int x, int y)
{
	m_draw_bitmap(b, x, y, 0);
}

void m_draw_alpha_bitmap(MBITMAP *b, int x, int y, int flags)
{
	m_draw_bitmap(b, x, y, flags);
}

// cache string bitmaps for 1 second
struct StringBitmap {
	uint64_t _id;
	ALLEGRO_COLOR color;
	MBITMAP *bitmap;
};
static std::map<std::string, std::vector<StringBitmap *> > string_bitmaps;
static uint64_t string_bitmap_id = 0;

static void destroy_string_bitmaps(uint64_t keep)
{
	std::map<std::string, std::vector<StringBitmap *> >::iterator it;
	for (it = string_bitmaps.begin(); it != string_bitmaps.end();) {
		bool erased = false;
		std::pair<std::string const, std::vector<StringBitmap *> > &p = *it;
		std::vector<StringBitmap *> &v = p.second;
		for (unsigned int i = 0; i < v.size(); i++) {
			StringBitmap *sb = v[i];
			if (string_bitmap_id > keep && sb->_id < string_bitmap_id-keep) {
				m_destroy_bitmap(sb->bitmap);
				delete sb;
				v.erase(v.begin() + i);
				i--;
				if (v.size() == 0) {
					std::map<std::string, std::vector<StringBitmap *> >::iterator tmp;
					tmp = it;
					it++;
					string_bitmaps.erase(tmp);
					erased = true;
					break;
				}
			}
		}
		if (!erased)
			it++;
	}
}

void destroy_string_bitmaps(void)
{
	destroy_string_bitmaps((uint64_t)powf(2, 64));
}

MBITMAP *get_string_bitmap(const MFONT *font, std::string s, ALLEGRO_COLOR c)
{
	MBITMAP *ret = NULL;
	
	std::vector<StringBitmap *> &v = string_bitmaps[s];
	for (unsigned int i = 0; i < v.size(); i++) {
		StringBitmap *sb = v[i];
		if (!memcmp(&sb->color, &c, sizeof(ALLEGRO_COLOR))) {
			ret = sb->bitmap;
			break;
		}
	}
	
	if (!ret) {
		MBITMAP *bmp;
		int flgs = al_get_new_bitmap_flags();
		al_set_new_bitmap_flags(flgs & ~ALLEGRO_NO_PRESERVE_TEXTURE);
		bmp = m_create_bitmap(m_text_length(font, s.c_str()), m_text_height(font)+2); // check
		al_set_new_bitmap_flags(flgs);
		ALLEGRO_STATE state;
		al_store_state(&state, ALLEGRO_STATE_TARGET_BITMAP);
		m_set_target_bitmap(bmp);
		m_clear_to_color(al_map_rgba(0, 0, 0, 0));
		al_draw_text(font, c, 0, 0, 0, s.c_str());
		al_restore_state(&state);
		StringBitmap *sb = new StringBitmap;
		sb->color = c;
		sb->bitmap = bmp;
		sb->_id = string_bitmap_id++;
		v.push_back(sb);
		ret = bmp;
	}
	
	destroy_string_bitmaps(500);
	
	return ret;
}

void m_textout(const MFONT *font, const char *text, int x, int y, MCOLOR color)
{
	al_draw_text(font, color, x, y-2, 0, text);
}

void m_textout_f(const MFONT *font, const char *text, float x, float y, MCOLOR color)
{
	al_draw_text(font, color, x, y-2, 0, text);
}

void m_textout_centre(const MFONT *font, const char *text, int x, int y, MCOLOR color)
{
	int length = al_get_text_width(font, text);
	m_textout(font, text, x-length/2, y, color);
}


int m_text_height(const MFONT *font)
{
	return al_get_font_line_height(font);
}

static INLINE float get_factor(int operation, float alpha)
{
   switch (operation) {
       case ALLEGRO_ZERO: return 0;
       case ALLEGRO_ONE: return 1;
       case ALLEGRO_ALPHA: return alpha;
       case ALLEGRO_INVERSE_ALPHA: return 1 - alpha;
   }
   return 0; /* silence warning in release build */
}

static INLINE void _al_blend_inline(
   const ALLEGRO_COLOR *scol, const ALLEGRO_COLOR *dcol,
   ALLEGRO_COLOR *result)
{
	float a = scol->a;
	float inv_a = (1 - scol->a);
	result->r = scol->r*a + dcol->r*inv_a;
	result->g = scol->g*a + dcol->g*inv_a;
	result->b = scol->b*a + dcol->b*inv_a;
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


void m_draw_scaled_bitmap(MBITMAP *bmp, int sx, int sy, int sw, int sh,
	int dx, int dy, int dw, int dh, int flags)
{
	al_draw_tinted_scaled_bitmap(bmp->bitmap, _blend_color, sx, sy, sw, sh, dx, dy, dw, dh, flags);
}

void m_draw_scaled_bitmap(MBITMAP *bmp, int sx, int sy, int sw, int sh,
	int dx, int dy, int dw, int dh, int flags, int alpha)
{
	m_save_blender();
	float alpha_f = alpha/255.0;
	m_set_blender(M_ONE, M_INVERSE_ALPHA, al_map_rgba_f(alpha_f, alpha_f, alpha_f, alpha_f));
	al_draw_tinted_scaled_bitmap(bmp->bitmap, _blend_color, sx, sy, sw, sh, dx, dy, dw, dh, flags);
	m_restore_blender();
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

// HERE FIXME
MBITMAP *m_load_bitmap(const char *name, bool force_memory)
{
	ALLEGRO_BITMAP *bitmap = 0;
	int flags = al_get_new_bitmap_flags();

	if (!force_memory && !config.getUseOnlyMemoryBitmaps()) {
		al_set_new_bitmap_flags((flags & ~ALLEGRO_MEMORY_BITMAP));
		bitmap = my_load_bitmap(name);
	}

	if (!bitmap) {
		ALLEGRO_STATE s;
		al_store_state(&s, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS);
		al_set_new_bitmap_flags(flags|ALLEGRO_MEMORY_BITMAP);
		bitmap = my_load_bitmap(name);
		al_restore_state(&s);
	}

	al_set_new_bitmap_flags(flags);

	if (!bitmap) {
		debug_message("Error loading bitmap %s\n", name);
		return NULL;
	}

	MBITMAP *m = new_mbitmap(bitmap);

	LoadedBitmap lb;
	lb.load_type = LOAD_LOAD;
	lb.flags = al_get_bitmap_flags(bitmap);
	lb.format = al_get_bitmap_format(bitmap);
	lb.load.filename = name;
	lb.bitmap = m;
	loaded_bitmaps.push_back(lb);

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

	LoadedBitmap lb;
	lb.load_type = LOAD_LOAD;
	lb.flags = al_get_bitmap_flags(bitmap);
	lb.format = al_get_bitmap_format(bitmap);
	lb.load.filename = name;
	lb.bitmap = m;
	loaded_bitmaps.push_back(lb);

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

	ALLEGRO_DEBUG("couldn't load font: using mem bitmaps");

	al_set_new_bitmap_flags(flags|ALLEGRO_MEMORY_BITMAP);
	f = al_load_font(name, 0, 0);
	al_set_new_bitmap_flags(flags);
	return f;
}


void my_clear_bitmap(MBITMAP *b)
{
	ALLEGRO_BITMAP *target = al_get_target_bitmap();
	m_set_target_bitmap(b);
	al_clear_to_color(al_map_rgba(0, 0, 0, 0));
	al_set_target_bitmap(target);
}


MBITMAP *m_create_bitmap(int w, int h, void (*func)(MBITMAP *bitmap, RecreateData *), RecreateData *data) // check
{
	ALLEGRO_BITMAP *bitmap = NULL;

	if (!config.getUseOnlyMemoryBitmaps()) {
		bitmap = al_create_bitmap(w, h);
	}

	if (!bitmap) {
		int flags = al_get_new_bitmap_flags();
		al_set_new_bitmap_flags(flags|ALLEGRO_MEMORY_BITMAP);
		bitmap = al_create_bitmap(w, h);
		al_set_new_bitmap_flags(flags);
	}
		
	if (!bitmap) {
		printf("error creating bitmap\n");
		return NULL;
	}

	MBITMAP *m = new_mbitmap(bitmap);
	my_clear_bitmap(m);
	
	if (func) {
		func(m, data);
	}

	LoadedBitmap lb;
	lb.load_type = LOAD_CREATE;
	lb.flags = al_get_bitmap_flags(bitmap);
	lb.format = al_get_bitmap_format(bitmap);
	lb.recreate.func = func;
	lb.recreate.data = data;
	lb.recreate.w = w;
	lb.recreate.h = h;
	lb.bitmap = m;
	loaded_bitmaps.push_back(lb);

	return m;
}


void m_destroy_bitmap(MBITMAP *bmp)
{
	for (size_t i = 0; i < loaded_bitmaps.size(); i++) {
		if (loaded_bitmaps[i].bitmap == bmp) {
			if (loaded_bitmaps[i].load_type == LOAD_CREATE && loaded_bitmaps[i].recreate.data) {
				delete loaded_bitmaps[i].recreate.data;
			}
			loaded_bitmaps.erase(loaded_bitmaps.begin()+i);
			break;
		}
	}

	al_destroy_bitmap(bmp->bitmap);
	delete bmp;
}


void m_flip_display(void)
{
	al_flip_display();

	if (do_toggle_fullscreen) {
		do_toggle_fullscreen = false;
		toggle_fullscreen();
	}
	
	if (controller_display)
	{
		ALLEGRO_BITMAP *target = al_get_target_bitmap();
		al_set_target_backbuffer(controller_display);
		al_flip_display();
		al_set_target_bitmap(target);
	}
}

void m_draw_circle(int x, int y, int radius, MCOLOR color,
	int flags)
{
	if (flags & M_FILLED) {
	//	draw_circle_filled( (int)x, (int)y, (int)radius, color);
		al_draw_filled_circle(x, y, radius, color);
	}
	else {
		//draw_circle_outline( (int)x, (int)y, (int)radius, color);
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
	}

	al_rest(remainder);
}


int m_get_display_width(void)
{
#ifdef WIZ
	return 320;
#else
	return al_get_display_width(display);
#endif
}


int m_get_display_height(void)
{
#ifdef WIZ
	return 240;
#else
	return al_get_display_height(display);
#endif
}


void m_clear(MCOLOR color)
{
	al_clear_to_color(color);
#if defined A5_OGL && defined ALLEGRO_WINDOWS
	//glClear(GL_DEPTH_BUFFER_BIT);
#endif
}


static int m_text_length_real(const MFONT *font, const char *text)
{
	return al_get_text_width(font, text);
}

void m_set_target_bitmap(MBITMAP *bmp)
{

#ifdef A5_D3D
	LPDIRECT3DDEVICE9 dev = al_get_d3d_device(display);
	LPDIRECT3DSURFACE9 curr_surf;
	al_lock_mutex(d3d_resource_mutex);
	if (!d3d_device_lost) {
		dev->GetDepthStencilSurface(&curr_surf);
		if (curr_surf != big_depth_surface)
			dev->SetDepthStencilSurface(big_depth_surface);
		if (curr_surf)
			curr_surf->Release();
	}
	al_unlock_mutex(d3d_resource_mutex);
#endif
	al_set_target_bitmap(bmp->bitmap);
}


void m_set_clip(int x1, int y1, int x2, int y2)
{
	al_set_clipping_rectangle(x1, y1, x2-x1, y2-y1);
}



void m_put_pixel(int x, int y, MCOLOR color)
{
	al_put_pixel(x, y, color);
}

void m_draw_trans_pixel(int x, int y, MCOLOR color)
{
#if defined A5_D3D || defined ALLEGRO_IPHONE
	al_draw_pixel(x, y, color);
#else
	// Workaround for some OpenGL drivers
	m_draw_rectangle(x, y, x+1, y+1, color, M_FILLED);
#endif
}

// -angle?!?! really?!
void m_draw_rotated_bitmap(MBITMAP *bitmap, int cx, int cy,
	int dx, int dy, float angle, int flags)
{
	al_draw_tinted_rotated_bitmap(bitmap->bitmap, _blend_color, cx, cy, dx, dy, -angle, flags);
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

MBITMAP *create_trapezoid(Direction dir, int topw, int bottomw, int length, MCOLOR color)
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
		//int y = ((float)topw/bottomw*length);
		m_draw_triangle(bottomw/2, length+y-1, 0, 0, bottomw-1, 0, color);
	}
	else if (dir == DIRECTION_EAST) {
		//int x = ((float)topw/bottomw*length);
		m_draw_triangle(-y, bottomw/2, length-1, 0, length-1, bottomw-1, color);
	}
	else if (dir == DIRECTION_SOUTH) {
		//int y = ((float)topw/bottomw*length);
		m_draw_triangle(bottomw/2, -y, bottomw-1, length-1, 0, length-1, color);
	}
	else if (dir == DIRECTION_WEST) {
		//int x = ((float)topw/bottomw*length);
		m_draw_triangle(length+y-1, bottomw/2, 0, 0, 0, bottomw-1, color);
	}

	m_pop_target_bitmap();

	return b;
}



void m_draw_trans_bitmap(MBITMAP *b, int x, int y, int alpha)
{
	m_save_blender();

	m_set_blender(M_ONE, M_INVERSE_ALPHA, m_map_rgba(alpha, alpha, alpha, alpha));

	m_draw_bitmap(b, x, y, 0);

	m_restore_blender();
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
	ALLEGRO_BITMAP *tmp = al_clone_bitmap(b->bitmap);
	al_destroy_bitmap(b->bitmap);
	b->bitmap = tmp;
	return b;
}

MBITMAP *m_make_alpha_display_bitmap(MBITMAP *in)
{
	MBITMAP *bitmap = 0;

	int old = al_get_new_bitmap_format();
	al_set_new_bitmap_format(ALPHA_FMT);

	bitmap = m_clone_bitmap(in);

	al_set_new_bitmap_format(old);

	m_destroy_bitmap(in);

	return bitmap;
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
	if ((have_mouse || is_ipad()) && !config.getMaintainAspectRatio())
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

void set_linear_mag_filter(MBITMAP *bitmap, bool onoff)
{
#ifdef A5_OGL
	GLuint tex = al_get_opengl_texture(bitmap->bitmap);
	glBindTexture(GL_TEXTURE_2D, tex);
	if (onoff) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
#endif
}

void m_set_blender(int s, int d, MCOLOR c)
{
	_blend_color = c;
	al_set_blender(ALLEGRO_ADD, s, d);
}

void m_save_blender(void)
{
	SAVED_BLENDER sb;

	al_get_separate_blender(
		&sb.oldColorOp,
		&sb.oldSrcColorFactor,
		&sb.oldDestColorFactor,
		&sb.oldAlphaOp,
		&sb.oldSrcAlphaFactor,
		&sb.oldDestAlphaFactor
	);
	sb.oldBlendColor = _blend_color;

	blender_stack.push(sb);
}


void m_restore_blender(void)
{
	SAVED_BLENDER sb = blender_stack.top();
	blender_stack.pop();

	al_set_separate_blender(
		sb.oldColorOp,
		sb.oldSrcColorFactor,
		sb.oldDestColorFactor,
		sb.oldAlphaOp,
		sb.oldSrcAlphaFactor,
		sb.oldDestAlphaFactor
	);
	_blend_color = sb.oldBlendColor;
}

void m_draw_prim (const void* vtxs, const ALLEGRO_VERTEX_DECL* decl, MBITMAP* texture, int start, int end, int type)
{
#ifdef __linux__
	// work around for nvidia+gallium
	if (type == ALLEGRO_PRIM_POINT_LIST) {
		ALLEGRO_VERTEX *verts = (ALLEGRO_VERTEX *)vtxs;
		for (int i = start; i < end; i++) {
			m_draw_trans_pixel(verts[i].x, verts[i].y, verts[i].color);
		}
		return;
	}
#endif
	al_draw_prim(vtxs, decl, texture->bitmap, start, end, type);
}

MBITMAP *m_create_sub_bitmap(MBITMAP *parent, int x, int y, int w, int h) // check
{
	ALLEGRO_BITMAP *sub = al_create_sub_bitmap(parent->bitmap, x, y, w, h);
	return new_mbitmap(sub);
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
		if (!(loaded_bitmaps[i].flags & ALLEGRO_NO_PRESERVE_TEXTURE)) {
			continue;
		}
		MBITMAP *m = loaded_bitmaps[i].bitmap;
		al_destroy_bitmap(m->bitmap);
	}
}

void _reload_loaded_bitmaps(void)
{
	int flags = al_get_new_bitmap_flags();
	int format = al_get_new_bitmap_format();

	for (size_t i = 0; i < loaded_bitmaps.size(); i++) {
		if (!(loaded_bitmaps[i].flags & ALLEGRO_NO_PRESERVE_TEXTURE)) {
			continue;
		}
		MBITMAP *m = loaded_bitmaps[i].bitmap;
		al_set_new_bitmap_flags(loaded_bitmaps[i].flags);
		al_set_new_bitmap_format(loaded_bitmaps[i].format);
		if (loaded_bitmaps[i].load_type == LOAD_LOAD) {
			m->bitmap = my_load_bitmap(loaded_bitmaps[i].load.filename.c_str());
		}
		else { // create
			Recreate *d = &loaded_bitmaps[i].recreate;
			m->bitmap = al_create_bitmap(d->w, d->h);
			if (d->func) { // recreate with func
				d->func(m, d->data);
			}
		}
	}

	al_set_new_bitmap_flags(flags);
	al_set_new_bitmap_format(format);
}

