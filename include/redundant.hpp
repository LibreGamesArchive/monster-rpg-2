#ifndef MREDUNDANT_HPP
#define MREDUNDANT_HPP

#include <stack>

void m_get_mouse_state(ALLEGRO_MOUSE_STATE *s);
float my_get_opengl_version(void);

class RecreateData
{
public:
	virtual ~RecreateData() {}
};

const int M_FLIP_HORIZONTAL = 1;
const int M_FLIP_VERTICAL = 2;

void my_clear_bitmap(MBITMAP *b);
#define m_draw_tinted_bitmap(bmp, tint, x, y, flags) \
	al_draw_tinted_bitmap(bmp->bitmap, tint, (int)x, (int)y, flags)
#define m_draw_bitmap(bmp, x, y, flags) \
	m_draw_tinted_bitmap(bmp, white, (int)x, (int)y, flags)
#define m_draw_tinted_bitmap_region(bmp, tint, sx, sy, sw, sh, dx, dy, flags) \
	al_draw_tinted_bitmap_region(bmp->bitmap, tint, (int)sx, (int)sy, sw, sh, (int)dx, (int)dy, flags)
#define m_draw_bitmap_region(bmp, sx, sy, sw, sh, dx, dy, flags) \
	m_draw_tinted_bitmap_region(bmp, white, (int)sx, (int)sy, sw, sh, (int)dx, (int)dy, flags)
#define m_draw_tinted_bitmap(bmp, tint, x, y, flags) al_draw_tinted_bitmap(bmp->bitmap, tint, (int)x, (int)y, flags)
#define m_draw_scaled_rotated_bitmap(bmp, cx, cy, dx, dy, xscale, yscale, angle, flags) \
	al_draw_scaled_rotated_bitmap(bmp->bitmap, cx, cy, (int)dx, (int)dy, xscale, yscale, angle, flags)
#define m_draw_tinted_scaled_rotated_bitmap(bmp, tint, cx, cy, dx, dy, xscale, yscale, angle, flags) \
	al_draw_tinted_scaled_rotated_bitmap(bmp->bitmap, tint, cx, cy, (int)dx, (int)dy, xscale, yscale, angle, flags)
#define m_draw_tinted_scaled_bitmap(bmp, tint, sx, sy, sw, sh, dx, dy, dw, dh, flags) \
	al_draw_tinted_scaled_bitmap(bmp->bitmap, tint, (int)sx, (int)sy, sw, sh, (int)dx, (int)dy, dw, dh, flags)

void my_do_line(int x1, int y1, int x2, int y2, void *data, void (*proc)(int, int, void *));
void my_do_circle(int x, int y, int radius, MCOLOR d,
	void (*proc)(int, int, MCOLOR));

int m_makecol(MCOLOR c);
int m_makecol32(MCOLOR c);
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
MBITMAP *m_load_bitmap(const char *name, bool force_memory = false, bool ok_to_fail = false);
MBITMAP *m_load_bitmap_redraw(const char *name, void (*redraw)(MBITMAP *bmp, RecreateData *data), RecreateData *data, bool delayed = false);
MFONT *m_load_font(const char *name);
MBITMAP *m_create_bitmap(int w, int h, void (*create)(MBITMAP *bitmap, RecreateData *data) = NULL, RecreateData *data = NULL, void (*destroy)(MBITMAP *b) = NULL, bool delayed = false); // check
MBITMAP *m_create_sub_bitmap(MBITMAP *parent, int x, int y, int w, int h); // check
void m_destroy_bitmap(MBITMAP *bmp, bool internals_only = false);
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
void m_draw_tinted_rotated_bitmap(MBITMAP *bitmap, MCOLOR tint, int cx, int cy, int dx, int dy,
	float angle, int flags);
void m_draw_rotated_bitmap(MBITMAP *bitmap, int cx, int cy, int dx, int dy,
	float angle, int flags);
void m_push_target_bitmap(void);
void m_pop_target_bitmap(void);
void m_fill_ellipse(int x, int y, int rx, int ry, MCOLOR color);
void m_draw_triangle(int x1, int y1,
	int x2, int y2,
	int x3, int y3,
	MCOLOR c);
MBITMAP *create_trapezoid(int dir, int topw, int bottomw, int length, MCOLOR c);
void m_draw_trans_bitmap(MBITMAP *b, int x, int y, int alpha);
void m_destroy_font(MFONT *f);
void m_draw_alpha_bitmap(MBITMAP *b, int x, int y);
void m_draw_alpha_bitmap(MBITMAP *b, int x, int y, int flags);
MBITMAP *m_create_alpha_bitmap(int w, int h, void (*create)(MBITMAP *bitmap, RecreateData *data) = NULL, RecreateData *data = NULL, void (*destroy)(MBITMAP *b) = NULL, bool delayed = false); // check
MBITMAP *m_load_alpha_bitmap(const char *name, bool force_memory = false);

MBITMAP *m_make_display_bitmap(MBITMAP *b);
MBITMAP *m_make_alpha_display_bitmap(MBITMAP *b);

struct SAVED_BLENDER {
	int oldColorOp, oldAlphaOp;
	int oldSrcColorFactor, oldDestColorFactor;
	int oldSrcAlphaFactor, oldDestAlphaFactor;
	ALLEGRO_COLOR oldBlendColor;
};

#define m_get_pixel(b, x, y) al_get_pixel(b->bitmap, x, y)
#define m_draw_pixel m_draw_trans_pixel

MBITMAP *new_mbitmap(ALLEGRO_BITMAP *bitmap);

enum LoadType {
	LOAD_LOAD,
	LOAD_CREATE
};

// use al_get_bitmap_(flags|format) to reload
struct Load {
	std::string filename;
	void (*redraw)(MBITMAP *b, RecreateData *data);
	RecreateData *data;
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
	bool delayed;
};

void _destroy_loaded_bitmaps(void);
extern "C" {
void _reload_loaded_bitmaps(void);
}
void _reload_loaded_bitmaps_delayed(void);

void m_draw_prim (const void* vtxs, const ALLEGRO_VERTEX_DECL* decl, MBITMAP* texture, int start, int end, int type);

ALLEGRO_LOCKED_REGION *m_lock_bitmap(MBITMAP *b, int format, int flags);
ALLEGRO_LOCKED_REGION *m_lock_bitmap_region(MBITMAP *b, int x, int y, int w, int h, int format, int flags);
void m_unlock_bitmap(MBITMAP *b);
MBITMAP *m_clone_bitmap(MBITMAP *b);

void m_draw_bitmap_to_self(MBITMAP *b, int x, int y, int flags);
void m_draw_bitmap_region_to_self(MBITMAP *b, int sx, int sy, int sw, int sh, int dx, int dy, int flags);

void m_draw_scaled_backbuffer(int sx, int sy, int sw, int sh, int dx, int dy, int dw, int dh, MBITMAP *dest);
void m_draw_tinted_bitmap_identity_view(MBITMAP *bmp, MCOLOR tint, int x, int y, int flags);
void m_draw_bitmap_identity_view(MBITMAP *bmp, int x, int y, int flags);
void m_draw_tinted_scaled_bitmap_identity_view(
	MBITMAP *bmp, MCOLOR tint, int sx, int sy, int sw, int sh, int dx, int dy, int dw, int dh, int flags
);
void m_draw_scaled_bitmap_identity_view(
	MBITMAP *bmp, int sx, int sy, int sw, int sh, int dx, int dy, int dw, int dh, int flags
);

ALLEGRO_BITMAP *my_al_create_bitmap(int w, int h);

void m_draw_scaled_target(MBITMAP *src, int sx, int sy, int sw, int sh,
	int dx, int dy, int dw, int dh, MBITMAP *dst);

void prepareForScreenGrab1();
void prepareForScreenGrab2();
void draw_tmpbuffer(int dx, int dy, int dw, int dh, ALLEGRO_BITMAP *bmp = NULL);

void set_target_backbuffer();

void m_set_mouse_xy(ALLEGRO_DISPLAY *display, int x, int y);

extern bool preparingForScreenGrab;

void use_shader(ALLEGRO_SHADER *shader);

int cursor_offset(bool centered = false);

void start_text();
void end_text();
bool drawing_text();

#endif
