#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

extern int curr_orientation;
extern int next_orientation;
extern double orientation_change_start;
extern float orientation_angle;

void drawBufferToScreen(void);
void drawBufferToScreen(bool draw_controls);
void draw_clock(int cx, int cy, int r, bool reverse);
void draw_shadow(MBITMAP *bmp, int x, int y, bool hflip = false);
void m_put_alpha_pixel(MBITMAP *bmp, int x, int y, MCOLOR c);
void m_draw_precise_line(MBITMAP *bmp, float x1, float y1, float x2, float y2, MCOLOR color);

void fadeIn(MCOLOR color);
void fadeOut(MCOLOR color);
bool transitionIn(bool can_cancel = false, bool toggle_dpad = true);
void transitionOut(bool toggle_dpad = true);
void battleTransition(void);

#ifdef ALLEGRO4
void generateTinterTable(int dest_r, int dest_g, int dest_b);
void tinted_blit(MBITMAP *bmp, int x, int y, float ratio);
void tinted_blit_region(MBITMAP *src, int sx, int sy, int sw, int sh, 
	int dx, int dy, int flags, float ratio);
void brightened_blit(MBITMAP *src, int dx, int dy, int brightness, int flags);
void tint2_blit(MBITMAP *src, int dx, int dy, MCOLOR color, int flags);
#endif
#endif // non editor stuff

void addOmnipotentText(std::string text, int cx, int cy, MCOLOR color);
void stopAllOmni(void);
void add_blit(MBITMAP *src, int x, int y, MCOLOR color, float amount, int flags);
void death_blit_region(MBITMAP *src, int x, int y, int w, int h, int dx, int dy, MCOLOR color, int flags);
extern bool global_draw_controls;

extern MBITMAP *blueblocks[8];
extern MBITMAP *airplay_dpad;
extern MBITMAP *white_button;
extern MBITMAP *black_button;
extern MBITMAP *airplay_logo;
extern double blueblock_times[7];

