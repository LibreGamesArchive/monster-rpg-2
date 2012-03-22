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

MBITMAP *m_create_alpha_bitmap(int w, int h)
{
	int f = al_get_new_bitmap_format();
	al_set_new_bitmap_format(ALPHA_FMT);
	MBITMAP *b;
	int flags = al_get_new_bitmap_flags();
	
	if (config.getUseOnlyMemoryBitmaps()) {
		al_set_new_bitmap_flags(flags|ALLEGRO_MEMORY_BITMAP);
		b = al_create_bitmap(w, h);
	}
	else {
		//al_set_new_bitmap_flags(flags);
		b = al_create_bitmap(w, h);
	}
	
	al_set_new_bitmap_flags(flags);
	al_set_new_bitmap_format(f);

	return b;
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
	ALLEGRO_BITMAP *bitmap;
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
				al_destroy_bitmap(sb->bitmap);
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
	destroy_string_bitmaps((uint64_t)pow(2, 64));
}

ALLEGRO_BITMAP *get_string_bitmap(const MFONT *font, std::string s, ALLEGRO_COLOR c)
{
	ALLEGRO_BITMAP *ret = NULL;
	
	std::vector<StringBitmap *> &v = string_bitmaps[s];
	for (unsigned int i = 0; i < v.size(); i++) {
		StringBitmap *sb = v[i];
		if (!memcmp(&sb->color, &c, sizeof(ALLEGRO_COLOR))) {
			ret = sb->bitmap;
			break;
		}
	}
	
	if (!ret) {
		ALLEGRO_BITMAP *bmp;
		bmp = al_create_bitmap(m_text_length(font, s.c_str()), m_text_height(font)+2);
		ALLEGRO_STATE state;
		al_store_state(&state, ALLEGRO_STATE_TARGET_BITMAP);
		al_set_target_bitmap(bmp);
		al_clear_to_color(al_map_rgba(0, 0, 0, 0));
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
//	ALLEGRO_BITMAP *bmp = get_string_bitmap(font, text, color);
//	al_draw_bitmap(bmp, x, y-2, 0);
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
#ifndef WIZ
		al_draw_filled_rectangle(x1, y1, x2, y2, color);
		return;
#else
		ALLEGRO_BITMAP *bmp = m_get_target_bitmap();

		//if (bmp == al_get_backbuffer()) {// || (color.a < 1 &&
				//bmp->format 
				//	!= ALLEGRO_PIXEL_FORMAT_RGB_565)) {
			//al_draw_filled_rectangle(x1, y1, x2, y2, color);
			//return;
		//}

		/* This assumes 16 bit */

		int ix1, ix2, iy1, iy2;
		if (x1 < x2) {
			ix1 = x1;
			ix2 = x2;
		}
		else {
			ix1 = x2;
			ix2 = x1;
		}
		if (y1 < y2) {
			iy1 = y1;
			iy2 = y2;
		}
		else {
			iy1 = y2;
			iy2 = y1;
		}

		if (ix1 < 0) ix1 = 0;
		if (iy1 < 0) iy1 = 0;
		if (ix2 >= BW) ix2 = BW-1;
		if (iy2 >= BH) iy2 = BH-1;

		if (color.a < 0.98) {
			uint16_t pixel;
			ALLEGRO_COLOR dst, result;
			for (int y = iy1; y <= iy2; y++) {
				uint16_t *data =
					(uint16_t *)(bmp->memory+y*bmp->pitch+ix1*2);
				for (int x = ix1; x <= ix2; x++) {
					pixel = *data;
					dst.r = ((pixel & 0xF800) >> 11) / (float)0x1F;
					dst.g = ((pixel & 0x07E0) >> 5) / (float)0x3F;
					dst.b = ((pixel & 0x001F)) / (float)0x1F;
					dst.a = 1;
					_al_blend_inline(&color, 
						&dst, &result);
					pixel = ((int)(result.r*31) << 11) |
						((int)(result.g*63) << 5) |
						(int)(result.b*31);
					*data = pixel;
					data++;
				}
			}
		}
		else {
			al_put_pixel(ix1, iy1, color);
			uint16_t *data = (uint16_t *)(bmp->memory+iy1*bmp->pitch+ix1*2);
			int pixel = *(uint16_t *)(data);

			for (int y = iy1; y <= iy2; y++) {
				data = (uint16_t *)(bmp->memory+y*bmp->pitch+ix1*2);
				for (int x = ix1; x <= ix2; x++) {
					*data = pixel;
					data++;
				}
			}
		}
#endif
	}
	else
		al_draw_rectangle(x1, y1, x2, y2, color, 1);
}


void m_draw_line(int x1, int y1, int x2, int y2, MCOLOR color)
{
	al_draw_line(x1+0.5, y1+0.5, x2+0.5, y2+0.5, color, 1);
}


void m_draw_scaled_bitmap(MBITMAP *bmp, int sx, int sy, int sw, int sh,
	int dx, int dy, int dw, int dh, int flags)
{
	al_draw_tinted_scaled_bitmap(bmp, _blend_color, sx, sy, sw, sh, dx, dy, dw, dh, flags);
}

void m_draw_scaled_bitmap(MBITMAP *bmp, int sx, int sy, int sw, int sh,
	int dx, int dy, int dw, int dh, int flags, int alpha)
{
	m_save_blender();
	float alpha_f = alpha/255.0;
	m_set_blender(M_ONE, M_INVERSE_ALPHA, al_map_rgba_f(alpha_f, alpha_f, alpha_f, alpha_f));
	al_draw_tinted_scaled_bitmap(bmp, _blend_color, sx, sy, sw, sh, dx, dy, dw, dh, flags);
	m_restore_blender();
}


int m_get_bitmap_width(MBITMAP *bmp)
{
	return al_get_bitmap_width(bmp);
}


int m_get_bitmap_height(MBITMAP *bmp)
{
	return al_get_bitmap_height(bmp);
}


MBITMAP *m_load_bitmap(const char *name, bool force_memory)
{
	ALLEGRO_BITMAP *bitmap = 0;
	int flags = al_get_new_bitmap_flags();

	if (!force_memory && !config.getUseOnlyMemoryBitmaps()) {
		al_set_new_bitmap_flags((flags & ~ALLEGRO_MEMORY_BITMAP) | ALLEGRO_CONVERT_BITMAP | PRESERVE_TEXTURE);
		bitmap = al_load_bitmap(name);
	}

	if (!bitmap) {
		ALLEGRO_STATE s;
		al_store_state(&s, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS);
		al_set_new_bitmap_flags(flags|ALLEGRO_MEMORY_BITMAP);
		bitmap = al_load_bitmap(name);
		al_restore_state(&s);
	}

	if (bitmap) {
		/*
		MCOLOR pink;
		pink = m_map_rgb(255, 0, 255);
		al_convert_mask_to_alpha(bitmap, pink);
		*/
	}
	else {
		debug_message("Error loading bitmap %s\n", name);
	}

	return bitmap;
}


MBITMAP *m_load_alpha_bitmap(const char *name, bool force_memory)
{
	ALLEGRO_BITMAP *bitmap = 0;

	int old = al_get_new_bitmap_format();
	al_set_new_bitmap_format(ALPHA_FMT);

	if (!force_memory)
		bitmap = al_load_bitmap(name);

	if (!bitmap) {
		ALLEGRO_STATE s;
		al_store_state(&s, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS);
		al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
		bitmap = al_load_bitmap(name);
		al_restore_state(&s);
	}

	al_set_new_bitmap_format(old);

	return bitmap;
}


MFONT *m_load_font(const char *name)
{
	ALLEGRO_FONT *f;

#ifndef WIZ
	f = al_load_font(name, 0, 0);
	if (f) return f;
#endif

	int flags = al_get_new_bitmap_flags();
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
	m_set_target_bitmap(target);
}


MBITMAP *m_create_bitmap(int w, int h, int extra_flags)
{
	MBITMAP *bitmap;
	int flags = al_get_new_bitmap_flags();

	if (!config.getUseOnlyMemoryBitmaps() && !(flags & ALLEGRO_MEMORY_BITMAP)) {
		al_set_new_bitmap_flags(((flags | extra_flags) & ~ALLEGRO_MEMORY_BITMAP) | ALLEGRO_CONVERT_BITMAP | PRESERVE_TEXTURE);
		bitmap = al_create_bitmap(w, h);
		al_set_new_bitmap_flags(flags);
		if (bitmap) {
			my_clear_bitmap(bitmap);
			return bitmap;
		}
	}

	ALLEGRO_STATE s;
	al_store_state(&s, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS);
	al_set_new_bitmap_flags(flags|ALLEGRO_MEMORY_BITMAP|extra_flags);
	bitmap = al_create_bitmap(w, h);
	al_restore_state(&s);

	if (!bitmap) {
		printf("error creating bitmap\n");
		return NULL;
	}

	my_clear_bitmap(bitmap);

	return bitmap;
}


void m_destroy_bitmap(MBITMAP *bmp)
{
	al_destroy_bitmap(bmp);
}


MBITMAP *m_get_backbuffer(void)
{
	return al_get_backbuffer(display);
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


/* 
 * Draw a circle outline
 */
 /*
static void draw_circle_outline(int x, int y, int radius, ALLEGRO_COLOR color)
{
   int sx, sy, dx, dy;
   ALLEGRO_BITMAP *bmp = al_get_target_bitmap();
   

   sx = x-radius-1;
   sy = y-radius-1;
   dx = x+radius+1;
   dy = y+radius+1;

   if (!al_lock_bitmap(bmp, ALLEGRO_PIXEL_FORMAT_ANY, 0))
	   return;

   my_do_circle(x, y, radius, color, al_put_pixel);

   al_unlock_bitmap(bmp);
}
*/

/*
 * Draw a filled circle
 */
 /*
static void draw_circle_filled(int x, int y, int radius, ALLEGRO_COLOR color)
{
   int cx = 0;
   int cy = radius;
   int df = 1 - radius; 
   int d_e = 3;
   int d_se = -2 * radius + 5;
   int sx, sy, dx, dy;

   sx = x-radius-1;
   sy = y-radius-1;
   dx = x+radius+1;
   dy = y+radius+1;

   do {
      m_draw_line(x-cy, y-cx, x+cy, y-cx, color, 1);

      if (cx)
	 m_draw_line(x-cy, y+cx, x+cy, y+cx, color, 1);

      if (df < 0)  {
	 df += d_e;
	 d_e += 2;
	 d_se += 2;
      }
      else { 
	 if (cx != cy) {
	    m_draw_line(x-cx, y-cy, x+cx, y-cy, color, 1);

	    if (cy)
	       m_draw_line(x-cx, y+cy, x+cx, y+cy, color, 1);
	 }

	 df += d_se;
	 d_e += 2;
	 d_se += 4;
	 cy--;
      } 

      cx++; 

   } while (cx <= cy);
}
*/

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


MBITMAP *m_get_target_bitmap(void)
{
	return al_get_target_bitmap();
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
	al_set_target_bitmap(bmp);
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
#if defined A5_D3D || defined IPHONE
	al_draw_pixel(x, y, color);
#else
	// Workaround for some OpenGL drivers
	m_draw_rectangle(x, y, x+1, y+1, color, M_FILLED);
#endif
}


#ifdef WIZ
/*
* Get scanline starts, ends and deltas, and clipping coordinates.
*/
#define top_bmp_y    corner_bmp_y[0]
#define right_bmp_y  corner_bmp_y[1]
#define bottom_bmp_y corner_bmp_y[2]
#define left_bmp_y   corner_bmp_y[3]
#define top_bmp_x    corner_bmp_x[0]
#define right_bmp_x  corner_bmp_x[1]
#define bottom_bmp_x corner_bmp_x[2]
#define left_bmp_x   corner_bmp_x[3]
#define top_spr_y    corner_spr_y[0]
#define right_spr_y  corner_spr_y[1]
#define bottom_spr_y corner_spr_y[2]
#define left_spr_y   corner_spr_y[3]
#define top_spr_x    corner_spr_x[0]
#define right_spr_x  corner_spr_x[1]
#define bottom_spr_x corner_spr_x[2]
#define left_spr_x   corner_spr_x[3]
static void draw_rotated_scaled_bitmap_memory_fast(ALLEGRO_BITMAP *src,
   int cx, int cy, int dx, int dy, float xscale, float yscale,
   float angle, int flags)
{
   /* Index in xs[] and ys[] to topmost point. */
   int top_index;
   /* Rightmost point has index (top_index+right_index) int xs[] and ys[]. */
   int right_index;
   /* Loop variables. */
   int index, i;
   /* Coordinates in bmp ordered as top-right-bottom-left. */
   al_fixed corner_bmp_x[4], corner_bmp_y[4];
   /* Coordinates in spr ordered as top-right-bottom-left. */
   al_fixed corner_spr_x[4], corner_spr_y[4];
   /* y coordinate of bottom point, left point and right point. */
   int clip_bottom_i, l_bmp_y_bottom_i, r_bmp_y_bottom_i;
   /* Left and right clipping. */
   al_fixed clip_left, clip_right;
   /* Temporary variable. */
   al_fixed extra_scanline_fraction;
   /* Top scanline of destination */
   int clip_top_i;

   ALLEGRO_LOCKED_REGION *src_region;
   ALLEGRO_LOCKED_REGION *dst_region;
   int ssize;
   int dsize;

   ALLEGRO_BITMAP *dst = al_get_target_bitmap();

   bool sub_pixel_accuracy = false;

   al_fixed xs[4], ys[4];
   al_fixed fix_dx;
   al_fixed fix_dy;
   al_fixed fix_cx;
   al_fixed fix_cy;
   al_fixed fix_angle;
   al_fixed fix_xscale;
   al_fixed fix_yscale;

   /*
    * Variables used in the loop
    */
   /* Coordinates of sprite and bmp points in beginning of scanline. */
   al_fixed l_spr_x, l_spr_y, l_bmp_x, l_bmp_dx;
   /* Increment of left sprite point as we move a scanline down. */
   al_fixed l_spr_dx, l_spr_dy;
   /* Coordinates of sprite and bmp points in end of scanline. */
   al_fixed r_bmp_x, r_bmp_dx;
   /*#ifdef KEEP_TRACK_OF_RIGHT_SPRITE_SCANLINE*/
   al_fixed r_spr_x, r_spr_y;
   /* Increment of right sprite point as we move a scanline down. */
   al_fixed r_spr_dx, r_spr_dy;
   /*#endif*/
   /* Increment of sprite point as we move right inside a scanline. */
   al_fixed spr_dx, spr_dy;
   /* Positions of beginning of scanline after rounding to integer coordinate
      in bmp. */
   al_fixed l_spr_x_rounded, l_spr_y_rounded, l_bmp_x_rounded;
   al_fixed r_bmp_x_rounded;
   /* Current scanline. */
   int bmp_y_i;
   /* Right edge of scanline. */
   int right_edge_test;

   angle = -angle;
   
   fix_dx = ftofix(dx);
   fix_dy = ftofix(dy);
   fix_cx = ftofix(cx);
   fix_cy = ftofix(cy);
   fix_angle = ftofix(-angle*256/(ALLEGRO_PI*2));
   fix_xscale = ftofix(xscale);
   fix_yscale = ftofix(yscale);

   _al_rotate_scale_flip_coordinates(src->w << 16, src->h << 16,
      fix_dx, fix_dy, fix_cx, fix_cy, fix_angle, fix_xscale, fix_yscale,
      flags & ALLEGRO_FLIP_HORIZONTAL, flags & ALLEGRO_FLIP_VERTICAL, xs, ys);

   /* Get index of topmost point. */
   top_index = 0;
   if (ys[1] < ys[0])
      top_index = 1;
   if (ys[2] < ys[top_index])
      top_index = 2;
   if (ys[3] < ys[top_index])
      top_index = 3;

   /* Get direction of points: clockwise or anti-clockwise. */
   right_index = (double)(xs[(top_index+1) & 3] - xs[top_index]) *
      (double)(ys[(top_index-1) & 3] - ys[top_index]) >
      (double)(xs[(top_index-1) & 3] - xs[top_index]) *
      (double)(ys[(top_index+1) & 3] - ys[top_index]) ? 1 : -1;
   /*FIXME: why does fixmul overflow below?*/
   /*if (fixmul(xs[(top_index+1) & 3] - xs[top_index],
      ys[(top_index-1) & 3] - ys[top_index]) >
         fixmul(xs[(top_index-1) & 3] - xs[top_index],
            ys[(top_index+1) & 3] - ys[top_index]))
      right_index = 1;
   else
      right_index = -1;*/

   /*
    * Get coordinates of the corners.
    */

   /* corner_*[0] is top, [1] is right, [2] is bottom, [3] is left. */
   index = top_index;
   for (i = 0; i < 4; i++) {
      corner_bmp_x[i] = xs[index];
      corner_bmp_y[i] = ys[index];
      if (index < 2)
         corner_spr_y[i] = 0;
      else
         /* Need `- 1' since otherwise it would be outside sprite. */
         corner_spr_y[i] = (src->h << 16) - 1;
      if ((index == 0) || (index == 3))
         corner_spr_x[i] = 0;
      else
         corner_spr_x[i] = (src->w << 16) - 1;
      index = (index + right_index) & 3;
   }

   /* Calculate left and right clipping. */
   clip_left = dst->cl << 16;
   clip_right = dst->cr_excl << 16;

   /* Quit if we're totally outside. */
   if ((left_bmp_x > clip_right) &&
       (top_bmp_x > clip_right) &&
       (bottom_bmp_x > clip_right))
      return;
   if ((right_bmp_x < clip_left) &&
       (top_bmp_x < clip_left) &&
       (bottom_bmp_x < clip_left))
      return;

   /* Bottom clipping. */
   if (sub_pixel_accuracy)
      clip_bottom_i = (bottom_bmp_y + 0xffff) >> 16;
   else
      clip_bottom_i = (bottom_bmp_y + 0x8000) >> 16;

   /* Bottom clipping */
   if (clip_bottom_i > dst->cb_excl)
      clip_bottom_i = dst->cb_excl;

   /* Calculate y coordinate of first scanline. */
   if (sub_pixel_accuracy)
      bmp_y_i = top_bmp_y >> 16;
   else
      bmp_y_i = (top_bmp_y + 0x8000) >> 16;

   /* Top clipping */
   if (bmp_y_i < dst->ct)
      bmp_y_i = dst->ct;

   if (bmp_y_i < 0)
      bmp_y_i = 0;

   /* Sprite is above or below bottom clipping area. */
   if (bmp_y_i >= clip_bottom_i)
      return;

   /* Vertical gap between top corner and centre of topmost scanline. */
   extra_scanline_fraction = (bmp_y_i << 16) + 0x8000 - top_bmp_y;
   /* Calculate x coordinate of beginning of scanline in bmp. */
   l_bmp_dx = fixdiv(left_bmp_x - top_bmp_x,
                   left_bmp_y - top_bmp_y);
   l_bmp_x = top_bmp_x + fixmul(extra_scanline_fraction, l_bmp_dx);
   /* Calculate x coordinate of beginning of scanline in spr. */
   /* note: all these are rounded down which is probably a Good Thing (tm) */
   l_spr_dx = fixdiv(left_spr_x - top_spr_x,
                   left_bmp_y - top_bmp_y);
   l_spr_x = top_spr_x + fixmul(extra_scanline_fraction, l_spr_dx);
   /* Calculate y coordinate of beginning of scanline in spr. */
   l_spr_dy = fixdiv(left_spr_y - top_spr_y,
                   left_bmp_y - top_bmp_y);
   l_spr_y = top_spr_y + fixmul(extra_scanline_fraction, l_spr_dy);

   /* Calculate left loop bound. */
   l_bmp_y_bottom_i = (left_bmp_y + 0x8000) >> 16;
   if (l_bmp_y_bottom_i > clip_bottom_i)
      l_bmp_y_bottom_i = clip_bottom_i;

   /* Calculate x coordinate of end of scanline in bmp. */
   r_bmp_dx = fixdiv(right_bmp_x - top_bmp_x,
                   right_bmp_y - top_bmp_y);
   r_bmp_x = top_bmp_x + fixmul(extra_scanline_fraction, r_bmp_dx);
   /*#ifdef KEEP_TRACK_OF_RIGHT_SPRITE_SCANLINE*/
   /* Calculate x coordinate of end of scanline in spr. */
   r_spr_dx = fixdiv(right_spr_x - top_spr_x,
                   right_bmp_y - top_bmp_y);
   r_spr_x = top_spr_x + fixmul(extra_scanline_fraction, r_spr_dx);
   /* Calculate y coordinate of end of scanline in spr. */
   r_spr_dy = fixdiv(right_spr_y - top_spr_y,
                   right_bmp_y - top_bmp_y);
   r_spr_y = top_spr_y + fixmul(extra_scanline_fraction, r_spr_dy);
   /*#endif*/

   /* Calculate right loop bound. */
   r_bmp_y_bottom_i = (right_bmp_y + 0x8000) >> 16;

   /* Get dx and dy, the offsets to add to the source coordinates as we move
      one pixel rightwards along a scanline. This formula can be derived by
      considering the 2x2 matrix that transforms the sprite to the
      parallelogram.
      We'd better use double to get this as exact as possible, since any
      errors will be accumulated along the scanline.
   */
   spr_dx = (al_fixed)((ys[3] - ys[0]) * 65536.0 * (65536.0 * src->w) /
                    ((xs[1] - xs[0]) * (double)(ys[3] - ys[0]) -
                     (xs[3] - xs[0]) * (double)(ys[1] - ys[0])));
   spr_dy = (al_fixed)((ys[1] - ys[0]) * 65536.0 * (65536.0 * src->h) /
                    ((xs[3] - xs[0]) * (double)(ys[1] - ys[0]) -
                     (xs[1] - xs[0]) * (double)(ys[3] - ys[0])));

   /* Lock the bitmaps */
   src_region = al_lock_bitmap(src, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READONLY);

   clip_top_i = bmp_y_i;

   if (!(dst_region = al_lock_bitmap_region(dst,
         clip_left>>16,
         clip_top_i,
         (clip_right>>16)-(clip_left>>16),
         clip_bottom_i-clip_top_i,
         ALLEGRO_PIXEL_FORMAT_ANY, 0)))
      return;

   ssize = al_get_pixel_size(src->format);
   dsize = al_get_pixel_size(dst->format);

   /*
    * Loop through scanlines.
    */

   while (1) {
      /* Has beginning of scanline passed a corner? */
      if (bmp_y_i >= l_bmp_y_bottom_i) {
         /* Are we done? */
         if (bmp_y_i >= clip_bottom_i)
            break;

         /* Vertical gap between left corner and centre of scanline. */
         extra_scanline_fraction = (bmp_y_i << 16) + 0x8000 - left_bmp_y;
         /* Update x coordinate of beginning of scanline in bmp. */
         l_bmp_dx = fixdiv(bottom_bmp_x - left_bmp_x,
                         bottom_bmp_y - left_bmp_y);
         l_bmp_x = left_bmp_x + fixmul(extra_scanline_fraction, l_bmp_dx);
         /* Update x coordinate of beginning of scanline in spr. */
         l_spr_dx = fixdiv(bottom_spr_x - left_spr_x,
                         bottom_bmp_y - left_bmp_y);
         l_spr_x = left_spr_x + fixmul(extra_scanline_fraction, l_spr_dx);
         /* Update y coordinate of beginning of scanline in spr. */
         l_spr_dy = fixdiv(bottom_spr_y - left_spr_y,
                         bottom_bmp_y - left_bmp_y);
         l_spr_y = left_spr_y + fixmul(extra_scanline_fraction, l_spr_dy);

         /* Update loop bound. */
         if (sub_pixel_accuracy)
            l_bmp_y_bottom_i = (bottom_bmp_y + 0xffff) >> 16;
         else
            l_bmp_y_bottom_i = (bottom_bmp_y + 0x8000) >> 16;
         if (l_bmp_y_bottom_i > clip_bottom_i)
            l_bmp_y_bottom_i = clip_bottom_i;
      }

      /* Has end of scanline passed a corner? */
      if (bmp_y_i >= r_bmp_y_bottom_i) {
         /* Vertical gap between right corner and centre of scanline. */
         extra_scanline_fraction = (bmp_y_i << 16) + 0x8000 - right_bmp_y;
         /* Update x coordinate of end of scanline in bmp. */
         r_bmp_dx = fixdiv(bottom_bmp_x - right_bmp_x,
                         bottom_bmp_y - right_bmp_y);
         r_bmp_x = right_bmp_x + fixmul(extra_scanline_fraction, r_bmp_dx);
         /*#ifdef KEEP_TRACK_OF_RIGHT_SPRITE_SCANLINE*/
         /* Update x coordinate of beginning of scanline in spr. */
         r_spr_dx = fixdiv(bottom_spr_x - right_spr_x,
                         bottom_bmp_y - right_bmp_y);
         r_spr_x = right_spr_x + fixmul(extra_scanline_fraction, r_spr_dx);
         /* Update y coordinate of beginning of scanline in spr. */
         r_spr_dy = fixdiv(bottom_spr_y - right_spr_y,
                         bottom_bmp_y - right_bmp_y);
         r_spr_y = right_spr_y + fixmul(extra_scanline_fraction, r_spr_dy);
         /*#endif*/

         /* Update loop bound: We aren't supposed to use this any more, so
            just set it to some big enough value. */
         r_bmp_y_bottom_i = clip_bottom_i;
      }

      /* Make left bmp coordinate be an integer and clip it. */
      if (sub_pixel_accuracy)
         l_bmp_x_rounded = l_bmp_x;
      else
         l_bmp_x_rounded = (l_bmp_x + 0x8000) & ~0xffff;
      if (l_bmp_x_rounded < clip_left)
         l_bmp_x_rounded = clip_left;

      /* ... and move starting point in sprite accordingly. */
      if (sub_pixel_accuracy) {
         l_spr_x_rounded = l_spr_x +
                           fixmul((l_bmp_x_rounded - l_bmp_x), spr_dx);
         l_spr_y_rounded = l_spr_y +
                           fixmul((l_bmp_x_rounded - l_bmp_x), spr_dy);
      }
      else {
         l_spr_x_rounded = l_spr_x +
            fixmul(l_bmp_x_rounded + 0x7fff - l_bmp_x, spr_dx);
         l_spr_y_rounded = l_spr_y +
            fixmul(l_bmp_x_rounded + 0x7fff - l_bmp_x, spr_dy);
      }

      /* Make right bmp coordinate be an integer and clip it. */
      if (sub_pixel_accuracy)
         r_bmp_x_rounded = r_bmp_x;
      else
         r_bmp_x_rounded = (r_bmp_x - 0x8000) & ~0xffff;
      if (r_bmp_x_rounded > clip_right)
         r_bmp_x_rounded = clip_right;

      /* Draw! */
      if (l_bmp_x_rounded <= r_bmp_x_rounded) {
         if (!sub_pixel_accuracy) {
            /* The bodies of these ifs are only reached extremely seldom,
               it's an ugly hack to avoid reading outside the sprite when
               the rounding errors are accumulated the wrong way. It would
               be nicer if we could ensure that this never happens by making
               all multiplications and divisions be rounded up or down at
               the correct places.
               I did try another approach: recalculate the edges of the
               scanline from scratch each scanline rather than incrementally.
               Drawing a sprite with that routine took about 25% longer time
               though.
            */
            if ((unsigned)(l_spr_x_rounded >> 16) >= (unsigned)src->w) {
               if (((l_spr_x_rounded < 0) && (spr_dx <= 0)) ||
                   ((l_spr_x_rounded > 0) && (spr_dx >= 0))) {
                  /* This can happen. */
                  goto skip_draw;
               }
               else {
                  /* I don't think this can happen, but I can't prove it. */
                  do {
                     l_spr_x_rounded += spr_dx;
                     l_bmp_x_rounded += 65536;
                     if (l_bmp_x_rounded > r_bmp_x_rounded)
                        goto skip_draw;
                  } while ((unsigned)(l_spr_x_rounded >> 16) >=
                           (unsigned)src->w);

               }
            }
            right_edge_test = l_spr_x_rounded +
                              ((r_bmp_x_rounded - l_bmp_x_rounded) >> 16) *
                              spr_dx;
            if ((unsigned)(right_edge_test >> 16) >= (unsigned)src->w) {
               if (((right_edge_test < 0) && (spr_dx <= 0)) ||
                   ((right_edge_test > 0) && (spr_dx >= 0))) {
                  /* This can happen. */
                  do {
                     r_bmp_x_rounded -= 65536;
                     right_edge_test -= spr_dx;
                     if (l_bmp_x_rounded > r_bmp_x_rounded)
                        goto skip_draw;
                  } while ((unsigned)(right_edge_test >> 16) >=
                           (unsigned)src->w);
               }
               else {
                  /* I don't think this can happen, but I can't prove it. */
                  goto skip_draw;
               }
            }
            if ((unsigned)(l_spr_y_rounded >> 16) >= (unsigned)src->h) {
               if (((l_spr_y_rounded < 0) && (spr_dy <= 0)) ||
                   ((l_spr_y_rounded > 0) && (spr_dy >= 0))) {
                  /* This can happen. */
                  goto skip_draw;
               }
               else {
                  /* I don't think this can happen, but I can't prove it. */
                  do {
                     l_spr_y_rounded += spr_dy;
                     l_bmp_x_rounded += 65536;
                     if (l_bmp_x_rounded > r_bmp_x_rounded)
                        goto skip_draw;
                  } while (((unsigned)l_spr_y_rounded >> 16) >=
                           (unsigned)src->h);
               }
            }
            right_edge_test = l_spr_y_rounded +
                              ((r_bmp_x_rounded - l_bmp_x_rounded) >> 16) *
                              spr_dy;
            if ((unsigned)(right_edge_test >> 16) >= (unsigned)src->h) {
               if (((right_edge_test < 0) && (spr_dy <= 0)) ||
                   ((right_edge_test > 0) && (spr_dy >= 0))) {
                  /* This can happen. */
                  do {
                     r_bmp_x_rounded -= 65536;
                     right_edge_test -= spr_dy;
                     if (l_bmp_x_rounded > r_bmp_x_rounded)
                        goto skip_draw;
                  } while ((unsigned)(right_edge_test >> 16) >=
                           (unsigned)src->h);
               }
               else {
                  /* I don't think this can happen, but I can't prove it. */
                  goto skip_draw;
               }
            }
         }

	ALLEGRO_COLOR bc;
	al_get_blender(NULL, NULL, NULL, &bc);

   if (al_get_bitmap_flags(src) & ALLEGRO_ALPHA_TEST) {
      if (bc.r == 1 && bc.g == 1 && bc.b == 1 && bc.a == 1) {
         int c;
         unsigned char *addr;
         unsigned char *end_addr;
         int my_r_bmp_x_i = r_bmp_x_rounded >> 16;
         int my_l_bmp_x_i = l_bmp_x_rounded >> 16;
         al_fixed my_l_spr_x = l_spr_x_rounded;
         al_fixed my_l_spr_y = l_spr_y_rounded;
         addr = (unsigned char *)(((char *)dst_region->data) +
            (bmp_y_i - clip_top_i) * dst_region->pitch);
         /* adjust for locking offset */
         addr -= (clip_left >> 16) * dsize;
         end_addr = addr + my_r_bmp_x_i * dsize;
         addr += my_l_bmp_x_i * dsize;
         for (; addr < end_addr; addr += dsize) {
          c = bmp_read16((void *)(((char *)src_region->data) +
             (my_l_spr_y>>16) * src_region->pitch + ssize * (my_l_spr_x>>16)));
          if (c & 0xF) {
             c = ALLEGRO_CONVERT_RGBA_4444_TO_RGB_565(c);
             bmp_write16(addr, c);
          }
          my_l_spr_x += spr_dx;
          my_l_spr_y += spr_dy;
         }
      }
      else {
         int c;
         unsigned char *addr;
         unsigned char *end_addr;
         int my_r_bmp_x_i = r_bmp_x_rounded >> 16;
         int my_l_bmp_x_i = l_bmp_x_rounded >> 16;
         al_fixed my_l_spr_x = l_spr_x_rounded;
         al_fixed my_l_spr_y = l_spr_y_rounded;
         addr = (unsigned char *)(((char *)dst_region->data) +
            (bmp_y_i - clip_top_i) * dst_region->pitch);
         /* adjust for locking offset */
         addr -= (clip_left >> 16) * dsize;
         end_addr = addr + my_r_bmp_x_i * dsize;
         addr += my_l_bmp_x_i * dsize;
         for (; addr < end_addr; addr += dsize) {
            c = bmp_read16((void *)(((char *)src_region->data) +
            (my_l_spr_y>>16) * src_region->pitch + ssize * (my_l_spr_x>>16)));
            if (c & 0xF) {
               int r = c >> 12;
               int g = (c >> 8) & 0xF;
               int b = (c >> 4) & 0xF;
               r *= bc.r;
               g *= bc.g;
               b *= bc.b;
               c = (r << 11) | (g << 5) | b;
               bmp_write16(addr, c);
            }
            my_l_spr_x += spr_dx;
            my_l_spr_y += spr_dy;
         }
      }
   }
   else {
         int c;
         unsigned char *addr;
         unsigned char *end_addr;
         int my_r_bmp_x_i = r_bmp_x_rounded >> 16;
         int my_l_bmp_x_i = l_bmp_x_rounded >> 16;
         al_fixed my_l_spr_x = l_spr_x_rounded;
         al_fixed my_l_spr_y = l_spr_y_rounded;
         addr = (unsigned char *)(((char *)dst_region->data) +
            (bmp_y_i - clip_top_i) * dst_region->pitch);
         /* adjust for locking offset */
         addr -= (clip_left >> 16) * dsize;
         end_addr = addr + my_r_bmp_x_i * dsize;
         addr += my_l_bmp_x_i * dsize;
         for (; addr < end_addr; addr += dsize) {
          c = bmp_read16((void *)(((char *)src_region->data) +
             (my_l_spr_y>>16) * src_region->pitch + ssize * (my_l_spr_x>>16)));
          if (c & 0xF) {
               uint16_t d = bmp_read16(addr);
               al_fixed a = fixdiv(itofix(c&0xF), itofix(0xF));
               al_fixed inv_a = fixsub(itofix(1), a);
               al_fixed r1 = fixmul(itofix((c & 0xF000) >> 11), a);
               al_fixed r2 = fixmul(itofix((d & 0xF000) >> 11), inv_a);
               int r = fixtoi(fixadd(r1, r2)) * bc.r;
               al_fixed g1 = fixmul(itofix((c & 0x0F00) >> 6), a);
               al_fixed g2 = fixmul(itofix((d & 0x0F00) >> 6), inv_a);
               int g = fixtoi(fixadd(g1, g2)) * bc.g;
               al_fixed b1 = fixmul(itofix((c & 0x00F0) >> 3), a);
               al_fixed b2 = fixmul(itofix((d & 0x00F0) >> 3), inv_a);
               int b = fixtoi(fixadd(b1, b2)) * bc.b;
               c = (r << 11) | (g << 5) | b;
               bmp_write16(addr, c);
          }
          my_l_spr_x += spr_dx;
          my_l_spr_y += spr_dy;
         }
      }
   }
      /* I'm not going to apoligize for this label and its gotos: to get
         rid of it would just make the code look worse. */
      skip_draw:

      /* Jump to next scanline. */
      bmp_y_i++;
      /* Update beginning of scanline. */
      l_bmp_x += l_bmp_dx;
      l_spr_x += l_spr_dx;
      l_spr_y += l_spr_dy;
      /* Update end of scanline. */
      r_bmp_x += r_bmp_dx;
      /*#ifdef KEEP_TRACK_OF_RIGHT_SPRITE_SCANLINE*/
      r_spr_x += r_spr_dx;
      r_spr_y += r_spr_dy;
      /*#endif*/
   }

   al_unlock_bitmap(src);
   al_unlock_bitmap(dst);
}
#endif

// -angle?!?! really?!
void m_draw_rotated_bitmap(MBITMAP *bitmap, int cx, int cy,
	int dx, int dy, float angle, int flags)
{
	#ifdef WIZ
	if (al_get_bitmap_format(bitmap) == ALLEGRO_PIXEL_FORMAT_RGBA_4444 &&
			al_get_bitmap_format(al_get_target_bitmap()) ==
			ALLEGRO_PIXEL_FORMAT_RGB_565) {
		draw_rotated_scaled_bitmap_memory_fast(bitmap, cx, cy, dx, dy,1, 1, -angle, flags);
	}
	else {
		al_draw_rotated_bitmap(bitmap, cx, cy, dx, dy, -angle, flags);
	}
	#else
	al_draw_tinted_rotated_bitmap(bitmap, _blend_color, cx, cy, dx, dy, -angle, flags);
	#endif
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
	
	if (dir == DIRECTION_NORTH) {
		b = m_create_bitmap(bottomw, length);
	}
	else if (dir == DIRECTION_EAST) {
		b = m_create_bitmap(length, bottomw);
	}
	else if (dir == DIRECTION_SOUTH) {
		b = m_create_bitmap(bottomw, length);
	}
	else {
		b = m_create_bitmap(length, bottomw);
	}

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

MBITMAP *m_make_display_bitmap(MBITMAP *b)
{
	if (!b) return NULL;
	MBITMAP *tmp = al_clone_bitmap(b);
	m_destroy_bitmap(b);
	return tmp;
}

MBITMAP *m_make_alpha_display_bitmap(MBITMAP *in)
{
	ALLEGRO_BITMAP *bitmap = 0;

	int old = al_get_new_bitmap_format();
	al_set_new_bitmap_format(ALPHA_FMT);

	bitmap = al_clone_bitmap(in);

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


static MBITMAP *pushed_target;

void m_push_target_bitmap(void)
{
	pushed_target = m_get_target_bitmap();
}

void m_pop_target_bitmap(void)
{
	m_set_target_bitmap(pushed_target);
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

void set_linear_mag_filter(ALLEGRO_BITMAP *bitmap, bool onoff)
{
#ifdef A5_OGL
	GLuint tex = al_get_opengl_texture(bitmap);
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

void m_draw_prim (const void* vtxs, const ALLEGRO_VERTEX_DECL* decl, ALLEGRO_BITMAP* texture, int start, int end, int type)
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
	al_draw_prim(vtxs, decl, texture, start, end, type);
}

