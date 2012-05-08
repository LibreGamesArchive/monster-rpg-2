#ifndef MREDUNDANT_HPP
#define MREDUNDANT_HPP

#include <stack>

void destroy_string_bitmaps(void);

void m_get_mouse_state(ALLEGRO_MOUSE_STATE *s);
float my_get_opengl_version(void);

class RecreateData
{
public:
	virtual ~RecreateData() {}
};


#ifdef ALLEGRO4
const int M_ZERO = 0;
const int M_ONE = 1;
const int M_ALPHA = 2;
const int M_INVERSE_ALPHA = 3;
#else
const int M_ZERO = ALLEGRO_ZERO;
const int M_ONE = ALLEGRO_ONE;
const int M_ALPHA = ALLEGRO_ALPHA;
const int M_INVERSE_ALPHA = ALLEGRO_INVERSE_ALPHA;
#endif

const int M_FLIP_HORIZONTAL = 1;
const int M_FLIP_VERTICAL = 2;

void my_clear_bitmap(MBITMAP *b);
#define m_draw_bitmap(bmp, x, y, flags) \
	al_draw_tinted_bitmap(bmp->bitmap, _blend_color, (int)x, (int)y, flags)
#define m_draw_bitmap_region(bmp, sx, sy, sw, sh, dx, dy, flags) \
	al_draw_tinted_bitmap_region(bmp->bitmap, _blend_color, (int)sx, (int)sy, sw, sh, (int)dx, (int)dy, flags)
#define m_draw_tinted_bitmap(bmp, tint, x, y, flags) al_draw_tinted_bitmap(bmp->bitmap, tint, (int)x, (int)y, flags)
#define m_draw_scaled_rotated_bitmap(bmp, cx, cy, dx, dy, xscale, yscale, angle, flags) \
	al_draw_scaled_rotated_bitmap(bmp->bitmap, cx, cy, (int)dx, (int)dy, xscale, yscale, angle, flags)
#define m_draw_tinted_scaled_rotated_bitmap(bmp, tint, cx, cy, dx, dy, xscale, yscale, angle, flags) \
	al_draw_tinted_scaled_rotated_bitmap(bmp->bitmap, tint, cx, cy, (int)dx, (int)dy, xscale, yscale, angle, flags)
#define m_draw_tinted_scaled_bitmap(bmp, tint, sx, sy, sw, sh, dx, dy, dw, dh, flags) \
	al_draw_tinted_scaled_bitmap(bmp->bitmap, tint, (int)sx, (int)sx, sw, sh, (int)dx, (int)dy, dw, dh, flags)

void my_do_line(int x1, int y1, int x2, int y2, void *data, void (*proc)(int, int, void *));
void my_do_circle(int x, int y, int radius, MCOLOR d,
	void (*proc)(int, int, MCOLOR));

int m_makecol(MCOLOR c);
int m_makecol32(MCOLOR c);
void m_textout_f(const MFONT *font, const char *text, float x, float y, MCOLOR color);
void m_textout(const MFONT *font, const char *text, int x, int y, MCOLOR color);
void m_textout_centre(const MFONT *font, const char *text, int x, int y, MCOLOR color);
int m_text_height(const MFONT *font);
int m_text_length(const MFONT *font, const char *text);

void m_set_target_bitmap(MBITMAP *b);
MCOLOR m_map_rgb(int r, int g, int b);
MCOLOR m_map_rgba(int r, int g, int b, int a);
MCOLOR m_map_rgb_f(float r, float g, float b);
void m_unmap_rgba(MCOLOR c,
	unsigned char *r,
	unsigned char *g,
	unsigned char *b,
	unsigned char *a);
void m_draw_rectangle(float x1, float y1, float x2, float y2, MCOLOR color,
	int flags);
void m_draw_line(int x1, int y1, int x2, int y2, MCOLOR color);
void m_draw_scaled_bitmap(MBITMAP *bmp, float sx, float sy, float sw, float sh,
	float dx, float dy, float dw, float dh, int flags, float alpha);
void m_draw_scaled_bitmap(MBITMAP *bmp, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh, int flags);
int m_get_bitmap_width(MBITMAP *bmp);
int m_get_bitmap_height(MBITMAP *bmp);
MBITMAP *m_load_bitmap(const char *name, bool force_memory = false);
MBITMAP *m_load_bitmap_redraw(const char *name, void (*redraw)(MBITMAP *bmp));
MFONT *m_load_font(const char *name);
MBITMAP *m_create_bitmap(int w, int h, void (*create)(MBITMAP *bitmap, RecreateData *data) = NULL, RecreateData *data = NULL, void (*destroy)(MBITMAP *b) = NULL); // check
MBITMAP *m_create_sub_bitmap(MBITMAP *parent, int x, int y, int w, int h); // check
void m_destroy_bitmap(MBITMAP *bmp);
void m_flip_display(void);
void m_draw_circle(int x, int y, int radius, MCOLOR color, int flags);
void m_rest(double seconds);
void m_flip_display(void);
int m_get_display_width(void);
int m_get_display_height(void);
extern "C" {
void m_clear(MCOLOR color);
}
void m_set_clip(int x1, int y1, int x2, int  y2);
void m_draw_tinted(MBITMAP *bitmap, int x, int  y,
	int r, int g, int b, int a,
	int flags);
void m_put_pixel(int x, int y, MCOLOR color);
void m_draw_trans_pixel(int x, int y, MCOLOR color);
void m_draw_rotated_bitmap(MBITMAP *bitmap, int cx, int cy, int dx, int dy,
	float angle, int flags);
void m_push_target_bitmap(void);
void m_pop_target_bitmap(void);
void m_fill_ellipse(int x, int y, int rx, int ry, MCOLOR color);
void m_draw_triangle(int x1, int y1,
	int x2, int y2,
	int x3, int y3,
	MCOLOR c);
MBITMAP *create_trapezoid(Direction dir, int topw, int bottomw, int length, MCOLOR c);
void m_draw_trans_bitmap(MBITMAP *b, int x, int y, int alpha);
void m_destroy_font(MFONT *f);
void m_draw_alpha_bitmap(MBITMAP *b, int x, int y);
void m_draw_alpha_bitmap(MBITMAP *b, int x, int y, int flags);
MBITMAP *m_create_alpha_bitmap(int w, int h, void (*create)(MBITMAP *bitmap, RecreateData *data) = NULL, RecreateData *data = NULL, void (*destroy)(MBITMAP *b) = NULL); // check
MBITMAP *m_load_alpha_bitmap(const char *name, bool force_memory = false);

#ifndef ALLEGRO4
MBITMAP *m_make_display_bitmap(MBITMAP *b);
MBITMAP *m_make_alpha_display_bitmap(MBITMAP *b);

struct SAVED_BLENDER {
	int oldColorOp, oldAlphaOp;
	int oldSrcColorFactor, oldDestColorFactor;
	int oldSrcAlphaFactor, oldDestAlphaFactor;
	ALLEGRO_COLOR oldBlendColor;
};

extern std::stack<SAVED_BLENDER> blender_stack;
extern ALLEGRO_COLOR _blend_color;

void m_set_blender(int s, int d, MCOLOR c);
void m_save_blender(void);
void m_restore_blender(void);

void set_linear_mag_filter(MBITMAP *bitmap, bool on);

#define m_get_pixel(b, x, y) al_get_pixel(b->bitmap, x, y)


#define m_draw_pixel m_draw_trans_pixel

#else
#define m_save_blender()
#define m_restore_blender()
#define m_draw_pixel m_put_pixel
#endif


#endif

MBITMAP *new_mbitmap(ALLEGRO_BITMAP *bitmap);

enum LoadType {
	LOAD_LOAD,
	LOAD_CREATE
};

// use al_get_bitmap_(flags|format) to reload
struct Load {
	std::string filename;
	void (*redraw)(MBITMAP *b);
};

struct Destroy {
	void (*func)(MBITMAP *bitmap);
};

struct Recreate {
	void (*func)(MBITMAP *bitmap, RecreateData *data);
	RecreateData *data;
	int w, h;
};

struct LoadedBitmap {
	LoadType load_type;
	Load load;
	Destroy destroy;
	Recreate recreate;
	MBITMAP *bitmap;
	int flags, format;
};

void _destroy_loaded_bitmaps(void);
void _reload_loaded_bitmaps(void);

void m_draw_prim (const void* vtxs, const ALLEGRO_VERTEX_DECL* decl, MBITMAP* texture, int start, int end, int type);

ALLEGRO_LOCKED_REGION *m_lock_bitmap(MBITMAP *b, int format, int flags);
ALLEGRO_LOCKED_REGION *m_lock_bitmap_region(MBITMAP *b, int x, int y, int w, int h, int format, int flags);
void m_unlock_bitmap(MBITMAP *b);
MBITMAP *m_clone_bitmap(MBITMAP *b);

void m_draw_bitmap_to_self(MBITMAP *b, int x, int y, int flags);
void m_draw_bitmap_region_to_self(MBITMAP *b, int sx, int sy, int sw, int sh, int dx, int dy, int flags);

