#include "monster2.hpp"

#define ASSERT ALLEGRO_ASSERT
// ugh FIXME
#include <allegro5/internal/aintern.h>
#include <allegro5/internal/aintern_bitmap.h>
#include <allegro5/internal/aintern_opengl.h>

#ifdef ALLEGRO_IPHONE
extern "C" {
#include <allegro5/internal/aintern_iphone.h>
}
#endif

#if defined ALLEGRO_IPHONE || defined ALLEGRO_MACOSX
#include "joypad.hpp"
#endif

#ifdef ALLEGRO4 // need a lookup table for tinting
#define NUM_TINTER_SHADES 8
uint16_t *tinter_table[NUM_TINTER_SHADES];
uint16_t variable_table[NUM_TINTER_SHADES*65536];
uint16_t purple_table[65536];
#endif

bool global_draw_controls = true;
static TemporaryTextWidget omnipotentTexts[MAX_PARTY];
static double current_time = -1;

MBITMAP *blueblocks[8];
MBITMAP *airplay_dpad;
MBITMAP *white_button;
MBITMAP *black_button;
MBITMAP *airplay_logo;
double blueblock_times[7] = { -1.0, };

void stopAllOmni(void)
{
	for (int i = 0; i < MAX_PARTY; i++) {
		omnipotentTexts[i].stop();
	}
}

void addOmnipotentText(std::string text, int cx, int cy, MCOLOR color)
{
	current_time = al_current_time();

	for (int i = 0; i < MAX_PARTY; i++) {
		if (omnipotentTexts[i].isDisplayed())
			continue;
		omnipotentTexts[i].start(text, cx, cy, color);
		break;
	}
}

int curr_orientation = 0;
int next_orientation = 0;
double orientation_change_start;
float orientation_angle;

static void draw_the_controls(bool draw_controls, ALLEGRO_COLOR tint)
{
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
#if defined ALLEGRO_IPHONE
	if ((airplay_connected || (config.getDpadType() == DPAD_TOTAL_1 || config.getDpadType() == DPAD_TOTAL_2)) || (use_dpad && dpad_buttons && draw_controls && global_draw_controls)) {
		if (controller_display) {
			if (joypad_connected()) {
				mTextout(game_font_second_display, "Joypad connected...", 2, 2, white, black, WGT_TEXT_NORMAL, false);
			}
			else {	
#else
	if (((config.getDpadType() == DPAD_TOTAL_1 || config.getDpadType() == DPAD_TOTAL_2)) || (use_dpad && dpad_buttons && draw_controls && global_draw_controls)) {
		if (false) {
			if (false) {
#endif
				ALLEGRO_STATE state;
				al_store_state(&state, ALLEGRO_STATE_BLENDER);
				al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ONE);
				
				// 4 because the screen is scaled to have 960x640 "pixels" when created (960/240 = 4 etc)
				float scalex = 4;
				float scaley = 4;
				
				const int BUFW = BW*scalex;
				const int BUFH = BH*scaley;
				const int BSZX = BUTTON_SIZE*scalex;
				const int BSZY = BUTTON_SIZE*scaley;

				int padx = 10*scalex, pady;
				int b1x = BUFW-BSZX*2-25*scalex, b2y;
				int b2x = BUFW-BSZX-20*scalex, b1y;

				if (dpad_at_top) {
					pady = 5*scaley;
					b1y = 5*scaley;
					b2y = 5*scaley;
				}
				else {
					pady = BUFH-BSZY*3-5*scaley;
					b1y = BUFH-BSZY-5*scaley;
					b2y = BUFH-BSZY-5*scaley;
				}

				if (config.getSwapButtons()) {
					int tx = b1x;
					int ty = b1y;
					b1x = b2x;
					b1y = b2y;
					b2x = tx;
					b2y = ty;
				}

				int centers[6][2] = {
					{ padx+BSZX/2, pady+BSZY*3/2 },
					{ padx+BSZX*2.5, pady+BSZY*3/2 },
					{ padx+BSZX*3/2, pady+BSZY/2 },
					{ padx+BSZX*3/2, pady+BSZY*2.5 },
					{ b1x+BSZX/2, b1y+BSZY/2 },
					{ b2x+BSZX/2, b2y+BSZY/2 }
				};

				double now = al_get_time();
				
				for (int i = 0; i < 6; i++) {
					double t = now - blueblock_times[i];
					if (t >= 1.0) {
						blueblock_times[i] = -1;
						continue;
					}
					if (t < 0)
						continue;
					int frame = (t / 1.0) * 8;
					float alpha = 1.0;
					int sizex = m_get_bitmap_width(blueblocks[frame]);
					int sizey = m_get_bitmap_height(blueblocks[frame]);
					al_draw_tinted_bitmap(blueblocks[frame]->bitmap,
						al_map_rgba_f(alpha, alpha, alpha, alpha),
						centers[i][0]-sizex/2, centers[i][1]-sizey/2,
						0
					);
				}
				
				al_restore_state(&state);

				al_draw_bitmap(airplay_dpad->bitmap, padx, pady, 0);
				al_draw_bitmap(white_button->bitmap, b1x, b1y, 0);
				al_draw_bitmap(black_button->bitmap, b2x, b2y, 0);
				
				al_draw_bitmap(airplay_logo->bitmap,
					960/2-m_get_bitmap_width(airplay_logo)/2,
					640/2-m_get_bitmap_height(airplay_logo)/2-100,
					0
				);
			}
		}
		else
#ifdef ALLEGRO_IPHONE
		if (!joypad_connected() && !is_sb_connected() && (dpad_type == DPAD_TOTAL_2 || dpad_type == DPAD_HYBRID_2)) {
#else
		if ((dpad_type == DPAD_TOTAL_2 || dpad_type == DPAD_HYBRID_2)) {
#endif
			ALLEGRO_COLOR light = al_map_rgba(100*tint.r, 200*tint.g, 100*tint.b, 255*tint.a);
			ALLEGRO_COLOR dark = al_map_rgba(150*tint.r, 250*tint.g, 150*tint.b, 255*tint.a);
			int x = BUTTON_SIZE+10;
			int y = dpad_at_top ? 5 : BH-5-BUTTON_SIZE;

			if (!config.getSwapButtons()) {
				al_draw_circle(BW-20-BUTTON_SIZE/2, y+BUTTON_SIZE/2, 
					BUTTON_SIZE/2, light, 2);
				al_draw_circle(BW-20-BUTTON_SIZE/2, y+BUTTON_SIZE/2, 
					BUTTON_SIZE/2+1, dark, 1);

				al_draw_filled_circle(BW-25-BUTTON_SIZE*1.5, y+BUTTON_SIZE/2, BUTTON_SIZE/2, light);
				al_draw_circle(BW-25-BUTTON_SIZE*1.5, y+BUTTON_SIZE/2, BUTTON_SIZE/2, dark, 1);
			}
			else {
				al_draw_circle(BW-25-BUTTON_SIZE*1.5, y+BUTTON_SIZE/2, 
					BUTTON_SIZE/2, light, 2);
				al_draw_circle(BW-25-BUTTON_SIZE*1.5, y+BUTTON_SIZE/2, 
					BUTTON_SIZE/2+1, dark, 1);

				al_draw_filled_circle(BW-20-BUTTON_SIZE/2, y+BUTTON_SIZE/2, BUTTON_SIZE/2, light);
				al_draw_circle(BW-20-BUTTON_SIZE/2, y+BUTTON_SIZE/2, BUTTON_SIZE/2, dark, 1);
			}

			y = dpad_at_top ? 5 : BH-BUTTON_SIZE*3-5;

			int l = BUTTON_SIZE*1.5-10;

			al_draw_rounded_rectangle(10, y, 10+BUTTON_SIZE*3, y+BUTTON_SIZE*3, 5, 5, light, 2);
			al_draw_rounded_rectangle(9, y-1, 10+BUTTON_SIZE*3+1, y+BUTTON_SIZE*3+1, 5, 5, dark, 1);
			m_draw_line(17, y+7, 17+l, y+7+l, dark);
			m_draw_line(10+BUTTON_SIZE*3-8, y+7, 10+BUTTON_SIZE*3-8-l, y+7+l, dark);
			m_draw_line(17, y+BUTTON_SIZE*3-7, 17+l, y+BUTTON_SIZE*3-7-l, dark);
			m_draw_line(10+BUTTON_SIZE*3-7, y+BUTTON_SIZE*3-7, 10+BUTTON_SIZE*3-7-l, y+BUTTON_SIZE*3-7-l, dark);

			al_draw_filled_triangle(x+4, y+BUTTON_SIZE-4, x+BUTTON_SIZE/2, y+4, x+BUTTON_SIZE-4, y+BUTTON_SIZE-4, light/*, 1*/);
			al_draw_triangle(x+4, y+BUTTON_SIZE-4, x+BUTTON_SIZE/2, y+4, x+BUTTON_SIZE-4, y+BUTTON_SIZE-4, dark, 1);
			
			x = 10;
			y += BUTTON_SIZE;
		
			al_draw_filled_triangle(x+4, y+BUTTON_SIZE/2, x+BUTTON_SIZE-4, y+4, x+BUTTON_SIZE-4, y+BUTTON_SIZE-4, light/*, 1*/);
			al_draw_triangle(x+4, y+BUTTON_SIZE/2, x+BUTTON_SIZE-4, y+4, x+BUTTON_SIZE-4, y+BUTTON_SIZE-4, dark, 1);

			x = 10+BUTTON_SIZE*2;
			
			al_draw_filled_triangle(x+BUTTON_SIZE-4, y+BUTTON_SIZE/2, x+4, y+BUTTON_SIZE-4, x+4, y+4, light/*, 1*/);
			al_draw_triangle(x+BUTTON_SIZE-4, y+BUTTON_SIZE/2, x+4, y+BUTTON_SIZE-4, x+4, y+4, dark, 1);

			x = 10+BUTTON_SIZE;
			y += BUTTON_SIZE;
		
			al_draw_filled_triangle(x+4, y+4,
				x+BUTTON_SIZE/2, y+BUTTON_SIZE-4,
				x+BUTTON_SIZE-4, y+4, light/*, 1*/);
			al_draw_triangle(x+4, y+4,
				x+BUTTON_SIZE/2, y+BUTTON_SIZE-4,
				x+BUTTON_SIZE-4, y+4, dark, 1);
		}
#ifdef ALLEGRO_IPHONE
		else if (!joypad_connected() && !is_sb_connected()) {
#else
		else if (true) {
#endif
			ALLEGRO_VERTEX verts[36];

			InputDescriptor ie = getInput()->getDescriptor();

			int x = BUTTON_SIZE+10;
			int y = dpad_at_top ? 5 : BH-5-BUTTON_SIZE;
			const int s = 20; // bitmap size
			const int o = (BUTTON_SIZE-s)/2; // offset to draw

			int nv = 0;
			int vx, vy;
			int vu, vv;

			#define SETXY \
				verts[nv].x = vx; \
				verts[nv].y = vy; \
				verts[nv].z = 0; \
				verts[nv+1].x = vx+20; \
				verts[nv+1].y = vy; \
				verts[nv+1].z = 0; \
				verts[nv+2].x = vx; \
				verts[nv+2].y = vy+20; \
				verts[nv+2].z = 0; \
				verts[nv+3].x = vx+20; \
				verts[nv+3].y = vy; \
				verts[nv+3].z = 0; \
				verts[nv+4].x = vx+20; \
				verts[nv+4].y = vy+20; \
				verts[nv+4].z = 0; \
				verts[nv+5].x = vx; \
				verts[nv+5].y = vy+20; \
				verts[nv+5].z = 0;

			#define SETUV \
				verts[nv].u = vu; \
				verts[nv].v = vv; \
				verts[nv+1].u = vu+20; \
				verts[nv+1].v = vv; \
				verts[nv+2].u = vu; \
				verts[nv+2].v = vv+20; \
				verts[nv+3].u = vu+20; \
				verts[nv+3].v = vv; \
				verts[nv+4].u = vu+20; \
				verts[nv+4].v = vv+20; \
				verts[nv+5].u = vu; \
				verts[nv+5].v = vv+20; \
				nv += 6;

			if (config.getSwapButtons()) {
				vx = BW-20-BUTTON_SIZE+o;
				vy = y+o;
			}
			else {
				vx = BW-25-BUTTON_SIZE*2;
				vy = y+o;
			}

			SETXY

			if (ie.button1) {
				vu = 20;
				vv = 40;

				SETUV
			}
			else {
				vu = 0;
				vv = 40;

				SETUV
			}

			if (config.getSwapButtons()) {
				vx = BW-25-BUTTON_SIZE*2;
				vy = y+o;
			}
			else {
				vx = BW-20-BUTTON_SIZE+o;
				vy = y+o;
			}

			SETXY

			if (ie.button2) {
				vu = 60;
				vv = 40;

				SETUV
			}
			else {
				vu = 40;
				vv = 40;

				SETUV
			}

			y = dpad_at_top ? 5: BH-BUTTON_SIZE*3-5;

			vx = x+o;
			vy = y+o;

			SETXY

			if (ie.up) {
				vu = 20;
				vv = 20;

				SETUV
			}
			else {
				vu = 0;
				vv = 20;

				SETUV
			}

			x = 10;
			y += BUTTON_SIZE;

			vx = x+o;
			vy = y+o;

			SETXY
			
			if (ie.left) {
				vu = 60;
				vv = 0;

				SETUV
			}
			else {
				vu = 40;
				vv = 0;

				SETUV
			}

			x = 10+BUTTON_SIZE*2;

			vx = x+o;
			vy = y+o;

			SETXY
			
			if (ie.right) {
				vu = 20;
				vv = 0;

				SETUV
			}
			else {
				vu = 0;
				vv = 0;

				SETUV
			}

			x = 10+BUTTON_SIZE;
			y += BUTTON_SIZE;

			vx = x+o;
			vy = y+o;

			SETXY
			
			if (ie.down) {
				vu = 60;
				vv = 20;

				SETUV
			}
			else {
				vu = 40;
				vv = 20;
				
				SETUV
			}

			for (int i = 0; i < nv; i++) {
				verts[i].color=al_map_rgba(255*tint.r, 255*tint.g, 255*tint.b, 255*tint.a);
			}

			m_draw_prim(verts, 0, dpad_buttons, 0, nv,
				ALLEGRO_PRIM_TRIANGLE_LIST);
		}
	}
#endif
}

static void drawOverlay(bool draw_controls, ALLEGRO_COLOR tint)
{
	long now = tguiCurrentTimeMillis();
	bool draw_red = ((now - last_shake_check) < 500);

#if defined ALLEGRO_IPHONE
	if (draw_red && global_draw_red && !path_head && !joypad_connected() && !is_sb_connected() && !airplay_connected) {
#else
#ifdef ALLEGRO_MACOSX
	bool jp_conn = joypad_connected();
#else
	bool jp_conn = false;
#endif
	if (draw_red && global_draw_red && !path_head && !jp_conn) {
#endif
		m_draw_triangle(0, 0, 16, 0, 0, 16, al_map_rgba_f(tint.r*tint.a, 0, 0, tint.a));
	}
	
#if defined(ALLEGRO_IPHONE) && !defined(LITE)
	if (isGameCenterAPIAvailable())
	{
		if (achievement_show) {
			double diff = al_get_time() - achievement_time;
			if (diff >= 3) {
				achievement_show = false;
				diff = 3;
			}
			
			float alpha = 1.0 - (diff / 3.0);
			float scale = diff;
			al_draw_tinted_scaled_rotated_bitmap(
				achievement_bmp->bitmap,
				al_map_rgba_f(alpha*tint.r, alpha*tint.g, alpha*tint.b, alpha*tint.a),
				m_get_bitmap_width(achievement_bmp)/2, m_get_bitmap_height(achievement_bmp)/2,
				BW/2, BH/2, scale, scale, 0, 0
			);
		}
	}
#endif

#ifdef ALLEGRO_IPHONE
	if (controller_display) {
		al_set_target_backbuffer(controller_display);
		m_clear(al_map_rgb(0, 0, 0));
	}
#endif

	draw_the_controls(draw_controls, tint);
}

static void drawBufferToScreen(MBITMAP *buf, bool draw_controls)
{
//#if !defined ALLEGRO_IPHONE || !defined ALLEGRO_ANDROID
#if 1
	m_set_target_bitmap(overlay);
	m_clear(m_map_rgba(0, 0, 0, 0));
	drawOverlay(draw_controls, white);
#endif

	m_set_target_bitmap(buffer);

#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
	bool on = ((unsigned)tguiCurrentTimeMillis() % 1000) < 500;
	const char *text = NULL;
	if (onscreen_swipe_to_attack) {
		text = "SWIPE TO ATTACK!";
	}
	else if (onscreen_drag_to_use) {
		text = "Drag to use!";
	}
	if (text && area && area->getName() != "tutorial") {
		int len = m_text_length(medium_font, _t(text));
		int h = m_text_height(medium_font);		
		float scale = 32.0/(h+4);
		if ((len+4)*scale > BW) scale = (float)BW/(len+4);
		al_draw_filled_rectangle(0, (BH/2)-(h/2*scale+10), BW, (BH/2)+(h/2*scale+10), m_map_rgba(45, 45, 45, 160));
		al_draw_line(0, (BH/2)-(h/2*scale+10), BW, (BH/2)-(h/2*scale+10), black, 1);
		al_draw_line(0, (BH/2)+(h/2*scale+10), BW, (BH/2)+(h/2*scale+10), black, 1);
	}
	if (on && !use_dpad && area && area->getName() != "tutorial") {
		if (text) {
			int len = m_text_length(medium_font, _t(text));
			int h = m_text_height(medium_font);

			ALLEGRO_STATE state;
			al_store_state(&state, ALLEGRO_STATE_TARGET_BITMAP | ALLEGRO_STATE_NEW_BITMAP_PARAMETERS);

			MBITMAP *tmp = m_create_alpha_bitmap(len+4, h+4); // check
			m_set_target_bitmap(tmp);
			m_clear(m_map_rgba(0, 0, 0, 0));
			
			mTextout(medium_font, _t(text), 1, 1,
				white, black,
				WGT_TEXT_NORMAL, false);

			float scale = 32.0/(h+4);
			if ((len+4)*scale > BW) scale = (float)BW/(len+4);
			
			al_restore_state(&state);

			m_draw_scaled_bitmap(tmp, 0, 0, len+4, h+4,
				(BW/2)-(((len+4)*scale)/2),
				(BH/2)-(((h+4)*scale)/2),
				(len+2)*scale, (h+2)*scale,
				0
			);

			m_destroy_bitmap(tmp);
		}
	}
#endif

	int step;
	if (current_time == -1) {
		current_time = al_current_time();
		step = 0;
	}
	else {
		double now = al_current_time();
		double diff = now - current_time;
		current_time = now;
		step = diff * 1000;
	}
	
	for (int i = 0; i < MAX_PARTY; i++) {
		omnipotentTexts[i].update(step);
		omnipotentTexts[i].draw();
	}
	
#if defined ALLEGRO_IPHONE
	if (batteryIcon) {
		float batteryLevel = getBatteryLevel();
		if (batteryLevel >= 0 && batteryLevel <= 0.16) {
			long t = tguiCurrentTimeMillis();
			if ((unsigned)t % 2000 < 1000) {
				m_draw_bitmap(batteryIcon,
					BW-5-m_get_bitmap_width(batteryIcon),
					5, 0);
			}
		}
	}
#endif

	al_set_target_backbuffer(display);

	m_clear(black);

	ALLEGRO_SHADER *scale2x_shader = NULL;

#if defined ALLEGRO_IPHONE
	if (airplay_connected) {
		//config.setFilterType(FILTER_NONE);
	}
#endif
	
	if (use_programmable_pipeline) {
		if (config.getFilterType() == FILTER_SCALE2X) {
			scale2x_shader = scale2x; // HERE
			al_set_shader(display, scale2x_shader);
			al_use_shader(scale2x_shader, true);
		}
		else {
			al_set_shader(display, cheap_shader);
			al_set_shader_sampler(cheap_shader, "tex", buf->bitmap, 0);
			al_use_shader(cheap_shader, true);
		}
	}

	MBITMAP *buf_save = buf;

	if (use_programmable_pipeline && config.getFilterType() == FILTER_SCALE2X) {
		ALLEGRO_STATE s;
		al_store_state(&s, ALLEGRO_STATE_TARGET_BITMAP);
		m_set_target_bitmap(scaleXX_buffer);
		m_draw_scaled_bitmap(
			buf,
			0, 0,
			BW, BH,
			0, 0,
			m_get_bitmap_width(scaleXX_buffer), m_get_bitmap_height(scaleXX_buffer),
			0
		);
		al_restore_state(&s);
		buf = scaleXX_buffer;
		al_use_shader(scale2x_shader, false);
		al_set_shader(display, cheap_shader);
		al_set_shader_sampler(cheap_shader, "tex", buf->bitmap, 0);
		al_use_shader(cheap_shader, true);
	}
	
	int dw, dh;
	int dx, dy;
	
	ScreenDescriptor *sd = config.getWantedGraphicsMode();
	
	if (config.getMaintainAspectRatio() == ASPECT_FILL_SCREEN) {
		dx = dy = 0;
		dw = sd->width;
		dh = sd->height;
	}
	else {
		dx = screen_offset_x;
		dy = screen_offset_y;
		dw = screenScaleX*BW;
		dh = screenScaleY*BH;
	}

	m_draw_scaled_bitmap(buf, 0, 0, m_get_bitmap_width(buf), m_get_bitmap_height(buf), dx, dy, dw, dh, 0);

	buf = buf_save;

	if (use_programmable_pipeline) {
		if (config.getFilterType() == FILTER_SCALE2X) {
			al_use_shader(scale2x_shader, false);
			al_set_shader(display, default_shader);
		}
	}

#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
	m_draw_tinted_scaled_bitmap(overlay, al_map_rgba_f(0.4, 0.4, 0.4, 0.4), 0, 0, m_get_bitmap_width(overlay), m_get_bitmap_height(overlay), dx, dy, dw, dh, 0);
#else
	if (config.getMaintainAspectRatio()) {
		m_draw_tinted_scaled_bitmap(overlay, al_map_rgba_f(0.4, 0.4, 0.4, 0.4), 0, 0, BW, BH, screen_offset_x, screen_offset_y,
			screenScaleX*BW, screenScaleY*BH,
			0
		);
	}
	else {
		m_draw_tinted_scaled_bitmap(overlay, al_map_rgba_f(0.4, 0.4, 0.4, 0.4), 0, 0, BW, BH, 0, 0,
			sd->width, sd->height,
			0
		);
	}
#endif

	if (use_programmable_pipeline) {
		if (!(config.getFilterType() == FILTER_SCALE2X)) {
			al_use_shader(cheap_shader, false);
			al_set_shader(display, default_shader);
		}
	}
}

void drawBufferToScreen(void)
{
	drawBufferToScreen(buffer, true);
}

void drawBufferToScreen(bool draw_controls)
{
	drawBufferToScreen(buffer, draw_controls);
}

void draw_shadow(MBITMAP *bmp, int x, int y, bool hflip)
{
	m_set_blender(M_ONE, M_INVERSE_ALPHA, white);

	ALLEGRO_VERTEX verts[6];
	int w = m_get_bitmap_width(bmp);
	int h = m_get_bitmap_height(bmp);
	ALLEGRO_COLOR trans_black = al_map_rgba(0, 0, 0, 50);

	int xplus = w/2;

	float uleft, uright;

	if (hflip) {
		uleft = w-1;
		uright = 0;
	}
	else {
		uleft = 0;
		uright = w-1;
	}

	verts[0].x = x+xplus;
	verts[0].y = y-h/2;
	verts[0].z = 0;
 	verts[0].u = uleft;
	verts[0].v = 0;
 	verts[0].color = trans_black;
	verts[1].x = x-xplus;
	verts[1].y = y;
	verts[1].z = 0;
 	verts[1].u = uleft;
	verts[1].v = h-1;
 	verts[1].color = trans_black;
	verts[2].x = x+xplus*3;
	verts[2].y = y-h/2;
	verts[2].z = 0;
 	verts[2].u = uright;
	verts[2].v = 0;
 	verts[2].color = trans_black;
	verts[3].x = x+xplus*3;
	verts[3].y = y-h/2;
	verts[3].z = 0;
	verts[3].u = uright;
	verts[3].v = 0;
 	verts[3].color = trans_black;
	verts[4].x = x-xplus;
	verts[4].y = y;
	verts[4].z = 0;
	verts[4].u = uleft;
	verts[4].v = h-1;
 	verts[4].color = trans_black;
	verts[5].x = x+xplus;
	verts[5].y = y;
	verts[5].z = 0;
 	verts[5].u = uright;
	verts[5].v = h-1;
	verts[5].color = trans_black;

	m_draw_prim(verts, 0, bmp, 0, 6, ALLEGRO_PRIM_TRIANGLE_LIST);
}


void draw_clock(int cx, int cy, int r, bool reverse)
{
	m_draw_circle(cx, cy, r, black, M_FILLED);
	m_draw_circle(cx, cy, r-2, white, M_FILLED);
	int t = (unsigned)tguiCurrentTimeMillis() % (500*12);
	float m = (t % 500) / 500.0f;
	float h = t / (float)(500*12);
	if (reverse) {
		m = 1 - m;
		h = 1 - h;
	}

	float ma = m * M_PI*2;
	float ha = h * M_PI*2;

	float mx = cx + (cos(ma)*(r-3));
	float my = cy + (sin(ma)*(r-3));
	
	float hx = cx + (cos(ha)*(r-3));
	float hy = cy + (sin(ha)*(r-3));

	m_draw_line(cx, cy, mx, my, black);
	m_draw_line(cx, cy, hx, hy, black);
}


void m_put_alpha_pixel(MBITMAP *bmp, int x, int y, MCOLOR c)
{
#ifdef ALLEGRO4
	MCOLOR bg = m_get_pixel(bmp, x, y);

	putpixel(bmp, x, y, m_makecol(m_map_rgb_f(
		MIN(1.0f, c.r*c.a+bg.r*(1-c.a)),
		MIN(1.0f, c.g*c.a+bg.g*(1-c.a)),
		MIN(1.0f, c.b*c.a+bg.b*(1-c.a))
	)));
#else
	MCOLOR bg = m_get_pixel(bmp, x, y);
	
	_al_put_pixel(bmp->bitmap, x, y, m_map_rgb_f(
		MIN(1.0f, c.r*c.a+bg.r*(1-c.a)),
		MIN(1.0f, c.g*c.a+bg.g*(1-c.a)),
		MIN(1.0f, c.b*c.a+bg.b*(1-c.a))
	));
#endif
}

void m_draw_precise_line(MBITMAP *bitmap, float x1, float y1, float x2, float y2, MCOLOR color)
{
	float dx = x2 - x1;
	float dy = y2 - y1;
	float dist = sqrt(dx*dx + dy*dy);
	float angle = atan2(dy, dx);
	float xinc = cos(angle);
	float yinc = sin(angle);

	const float step = 0.333f;
	
	int nverts = dist / step;
	ALLEGRO_VERTEX *verts = new ALLEGRO_VERTEX[nverts];
	int vcount = 0;

	color.r *= color.a;
	color.g *= color.a;
	color.b *= color.a;

	m_save_blender();
	m_set_blender(ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA, white);

	for (float i = 0; i < dist && vcount < nverts; i += step) {
		float x = x1 + xinc * i;
		float y = y1 + yinc * i;
		verts[vcount].x = x;
		verts[vcount].y = y;
		verts[vcount].z = 0;
		verts[vcount].color = color;
		vcount++;
	}

	if (vcount > 0) {
		m_draw_prim(verts, 0, 0, 0, vcount, ALLEGRO_PRIM_POINT_LIST);
	}

	m_restore_blender();

	delete[] verts;
}

static void fade(int startAlpha, int endAlpha, int length, MCOLOR color)
{
	dpad_off();
	global_draw_red = false;
	global_draw_controls = false;

	unsigned char r, g, b;
	long total = 0;

	int flags = al_get_new_bitmap_flags();
	if (config.getFilterType() == FILTER_LINEAR) {
		al_set_new_bitmap_flags(flags | ALLEGRO_NO_PRESERVE_TEXTURE | ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);
	}
	MBITMAP *tmpbuf = m_create_bitmap(BW, BH); // check
	al_set_new_bitmap_flags(flags);

	long start = tguiCurrentTimeMillis();

	while (total < length) {
		long now = tguiCurrentTimeMillis();
		int elapsed = now - start;
		total += elapsed;
		start = now;
		float p = (float)total / length;
		if (p > 1) p = 1;
		int a = (int)(startAlpha - (p * (startAlpha - endAlpha)));
		r = (int)(color.r*a);
		g = (int)(color.g*a);
		b = (int)(color.b*a);

		m_set_target_bitmap(tmpbuf);
		m_draw_bitmap(buffer, 0, 0, 0);
		al_draw_filled_rectangle(0, 0, BW, BH, al_map_rgba(r, g, b, a));
		
		drawBufferToScreen(tmpbuf, true);
		
		m_flip_display();
	}

	m_set_target_bitmap(tmpbuf);
	if (startAlpha < endAlpha) {
		al_draw_filled_rectangle(0, 0, BW, BH, color);
	}
	else {
		m_draw_bitmap(buffer, 0, 0, 0);
	}
	drawBufferToScreen(tmpbuf, true);
	m_flip_display();

	m_destroy_bitmap(tmpbuf);

	dpad_on();
	global_draw_red = true;
	global_draw_controls = true;

	clear_input_events();
}



void fadeIn(MCOLOR color)
{
	fade(255, 0, 600, color);
}



void fadeOut(MCOLOR color)
{
	fade(0, 255, 600, color);
}


/* initialRectSize/endRectSize are 0-60 (for percent)
 * initialFocus/destFocus is the amount the buffer is focussed
 * 	in/out (e.g., focus of 2 would draw a two pixel square
 *	of the same color for every other pixel).
 */
static bool transition(bool focusing, int length, bool can_cancel = false, bool toggle_dpad = true)
{
	dpad_off();
	global_draw_red = false;
	global_draw_controls = false;

	MBITMAP *tmp = m_create_bitmap(BW, BH);
	MBITMAP *bufdup = m_clone_bitmap(buffer);

	unsigned long start = (unsigned long)(al_get_time()*1000);
	unsigned long now = start;
	
	while ((now - start) < (unsigned long)length) {
		INPUT_EVENT ie = get_next_input_event();
		if (ie.button1 == DOWN || ie.button2 == DOWN || !released) {
			use_input_event();
			if (can_cancel) {
				dpad_on();
				m_destroy_bitmap(tmp);
				m_destroy_bitmap(bufdup);
				m_set_target_bitmap(buffer);
				global_draw_red = true;
				global_draw_controls = true;
				return true;
			}
		}
		now = (unsigned long)(al_get_time()*1000);
		int elapsed = now - start;
		float p = (float)elapsed / length;
		if (p > 1) p = 1;
		if (focusing)
			p = 1.0 - p;
		int size = p * 32;
		if (size < 1) size = 1;
		p = 1.0 - p;
		int rectw = p * BW;
		int recth = p * BH;
		if (rectw < 1) rectw = 1;
		if (recth < 1) recth = 1;

		m_set_target_bitmap(tmp);
		al_draw_scaled_bitmap(bufdup->bitmap, 0, 0, BW, BH, 0, 0, BW/size, BH/size, 0);

		m_set_target_bitmap(buffer);
		m_clear(black);

		int cx, cy, cw, ch;
		al_get_clipping_rectangle(&cx, &cy, &cw, &ch);
		al_set_clipping_rectangle(
			(BW-rectw)/2,
			(BH-recth)/2,
			rectw, recth
		);

		al_draw_scaled_bitmap(tmp->bitmap, 0, 0, BW/size, BH/size, 0, 0, BW, BH, 0);

		al_set_clipping_rectangle(cx, cy, cw, ch);

		drawBufferToScreen(buffer, true);
		m_flip_display();
	}

	if (!focusing) {
		al_set_target_backbuffer(display);
		m_clear(black);
		m_flip_display();
	}

	m_destroy_bitmap(tmp);
	m_destroy_bitmap(bufdup);

	m_set_target_bitmap(buffer); // be safe

	dpad_on();
	global_draw_red = true;
	global_draw_controls = true;

	clear_input_events();

	return false;
}

/* Transition draws a increasing/decreasing rectangular view of
 * the area with the inside focussing in/out.
 */
bool transitionIn(bool can_cancel, bool toggle_dpad)
{
	real_auto_save_screenshot();
	save_memory(true);
	bool ret = transition(true, 600, can_cancel, toggle_dpad);
	return ret;
}


void transitionOut(bool toggle_dpad)
{
	transition(false, 600, false, toggle_dpad);
}


void battleTransition(void)
{
	if (!use_programmable_pipeline) {
		dpad_off();
		global_draw_red = false;
		global_draw_controls = false;

		int format = al_get_new_bitmap_format();
		al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_RGB_565);
		int flags = al_get_new_bitmap_flags();
		al_set_new_bitmap_flags(flags | ALLEGRO_NO_PRESERVE_TEXTURE);
		MBITMAP *bufcopy1 = m_create_bitmap(BW, BH); // check
		MBITMAP *bufcopy2 = m_create_bitmap(BW, BH); // check
		al_set_new_bitmap_flags(flags);
		al_set_new_bitmap_format(format);

		m_set_target_bitmap(bufcopy1);
		m_draw_bitmap(buffer, 0, 0, 0);
		m_set_target_bitmap(buffer);
		battle->draw();
		m_set_target_bitmap(bufcopy2);
		battle->draw();

		float heights[BW];
		int phases = rand() % 3 + 1;
		int *phase_lengths = new int[phases];
		int *phase_peaks = new int[phases];
		int max = 0;
		for (int i = 0; i < phases; i++) {
			phase_lengths[i] = BW/phases;
		}
		for (int i = 0; i < phases-1; i++) {
			int d = (rand()%60-30);
			phase_lengths[i] += d;
			phase_lengths[i+1] -= d;
		}
		for (int i = 0; i < phases; i++) {
			phase_peaks[i] = rand() % 30 + 25;
			if (phase_peaks[i] > max)
				max = phase_peaks[i];
		}
		int n = 0;
		for (int i = 0; i < phases; i++) {
			for (int j = 0; j < phase_lengths[i]; j++) {
				int peak = phase_peaks[i];
				int h = sin((float)j/phase_lengths[i]*M_PI*2)*peak;
				heights[n++] = h;
			}
		}

		int length = 1100;
		long elapsed = 0;
		long start = tguiCurrentTimeMillis();
		int step = 0;

		while (elapsed < length) {
			if (step) {
				m_set_target_bitmap(buffer);

				m_draw_bitmap(bufcopy1, 0, 0, 0);

				for (int i = 0; i < BW; i++) {
					float p = (float)elapsed/length;
					int h = (heights[i] + p*(BH+max)) - max;
					if (h > 0)
						m_draw_bitmap_region(bufcopy2,
							i, 0, 1, h, i, 0, 0);
				}

				drawBufferToScreen();

				m_flip_display();
			}

			long end = tguiCurrentTimeMillis();
			step = end - start;
			elapsed += step;
			start = end;
		}
		
		dpad_on();
		global_draw_red = true;
		global_draw_controls = true;

		m_destroy_bitmap(bufcopy1);
		m_destroy_bitmap(bufcopy2);

		clear_input_events();

		delete[] phase_lengths;
		delete[] phase_peaks;

		return;
	}

	MBITMAP *xfade_buf = m_create_bitmap(BW, BH); // check
	MBITMAP *battle_buf = m_create_bitmap(BW, BH); // check

	//set_linear_mag_filter(xfade_buf, config.getFilterType() == FILTER_LINEAR);

	m_set_target_bitmap(battle_buf);
	battle->draw();

	dpad_off();

	long now = tguiCurrentTimeMillis();
	long start = now;
	int length = 700;
	float angle = 0;

	while ((now - start) < (length*2)) {
		int elapsed = now - start;
		angle = ((double)elapsed / (length*2.0)) * (M_PI*2);

			ALLEGRO_STATE state;
			al_store_state(&state, ALLEGRO_STATE_TARGET_BITMAP);
			m_save_blender();
			m_set_target_bitmap(xfade_buf);
			m_draw_bitmap(buffer, 0, 0, 0);
			float alpha = angle / (M_PI*2);
			m_set_blender(M_ONE, M_INVERSE_ALPHA, al_map_rgba_f(alpha, alpha, alpha, alpha));
			m_draw_bitmap(battle_buf, 0, 0, 0);
			al_restore_state(&state);
			m_restore_blender();


		ALLEGRO_STATE s;
		al_store_state(&s, ALLEGRO_STATE_TARGET_BITMAP);
		m_set_target_bitmap(buffer);
		al_set_shader(display, warp);
		al_set_shader_float(warp, "angle", angle);
		int buffer_true_w, buffer_true_h;
		get_buffer_true_size(&buffer_true_w, &buffer_true_h);
		float tex_bot = (float)BH / buffer_true_h;
		al_set_shader_float(warp, "tex_bot", tex_bot);
		al_set_shader_sampler(warp, "tex", xfade_buf->bitmap, 0);
		al_use_shader(warp, true);
		m_draw_bitmap(xfade_buf, 0, 0, 0);
		al_use_shader(warp, false);
		al_set_shader(display, default_shader);
		al_restore_state(&s);
		drawBufferToScreen(buffer, true);
		m_flip_display();
		now = tguiCurrentTimeMillis();
	}

	m_destroy_bitmap(xfade_buf);
	m_destroy_bitmap(battle_buf);

	dpad_on();
	clear_input_events();
}

#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
void add_blit(MBITMAP *src, int dx, int dy, MCOLOR color, float amount, int flags)
{
	int src_w = m_get_bitmap_width(src);
	int src_h = m_get_bitmap_height(src);
	ALLEGRO_BITMAP *target = al_get_target_bitmap();
	int tw = al_get_bitmap_width(target);
	int th = al_get_bitmap_height(target);
	int sx, sy;
        int cx, cy, cw, ch;
       
        al_get_clipping_rectangle(&cx, &cy, &cw, &ch);

	// clip to screen
	if (dx < 0) {
		sx = -dx;
		src_w += dx;
		dx = 0;
	}
	else {
		sx = 0;
	}
	if (dx + src_w > tw) {
		src_w -= (dx + src_w) - tw;
	}
	// clip to clipping rectangle
	if (dx < cx) {
		sx += (cx-dx);
		src_w -= (cx-dx);
		dx = cx;
	}
	else if (dx+src_w > cx+cw) {
		src_w -= (dx+src_w) - (cx+cw);
	}
	
	// clip to screen
	if (dy < 0) {
		sy = -dy;
		src_h += dy;
		dy = 0;
	}
	else {
		sy = 0;
	}
	if (dy + src_h > th) {
		src_h -= (dy + src_h) - th;
	}
	// clip to clipping rectangle
	if (dy < cy) {
		sy += (cy-dy);
		src_h -= (cy-dy);
		dy = cy;
	}
	else if (dy+src_h > cy+ch) {
		src_h -= (dy+src_h) - (cy+ch);
	}
	
#ifdef ALLEGRO_ANDROID
	ALLEGRO_BITMAP *tolock;
	int ofsx = 0, ofsy = 0;
	if (al_get_parent_bitmap(src->bitmap)) {
		tolock = al_get_parent_bitmap(src->bitmap);
		ofsx += src->bitmap->xofs;
		ofsx += src->bitmap->yofs;
	}
	else {
		tolock = src->bitmap;
	}
	ALLEGRO_LOCKED_REGION *sreg = al_lock_bitmap(tolock, ALLEGRO_PIXEL_FORMAT_RGBA_4444, ALLEGRO_LOCK_READONLY);
	ALLEGRO_LOCKED_REGION *dreg = al_lock_bitmap(target, ALLEGRO_PIXEL_FORMAT_RGBA_4444, ALLEGRO_LOCK_READWRITE);
#else
	ALLEGRO_LOCKED_REGION *sreg = m_lock_bitmap_region(src, 0, sy, src_w, src_h, ALLEGRO_PIXEL_FORMAT_RGBA_4444, ALLEGRO_LOCK_READONLY);
	if (!sreg) { ALLEGRO_DEBUG("!sreg returning"); return; }
	ALLEGRO_LOCKED_REGION *dreg = al_lock_bitmap_region(target, dx, dy, src_w, src_h, ALLEGRO_PIXEL_FORMAT_RGBA_4444, ALLEGRO_LOCK_READWRITE);
	if (!dreg) { m_unlock_bitmap(src); ALLEGRO_DEBUG("!dreg returning"); return; }
	int ofsx = 0, ofsy = 0;
#endif

	float src_factor = 1 - amount;
	
	float add_r = (color.r * amount) * 0xf;
	float add_g = (color.g * amount) * 0xf;
	float add_b = (color.b * amount) * 0xf;

	for (int yy = 0; yy < src_h; yy++) {
#ifdef ALLEGRO_ANDROID
		unsigned char *sptr = ((unsigned char *)sreg->data + (yy+ofsy) * sreg->pitch + (ofsx * 2));
		unsigned char *dptr = ((unsigned char *)dreg->data + (yy+dy) * dreg->pitch + (dx * 2));
#else
		unsigned char *sptr = ((unsigned char *)sreg->data + yy * sreg->pitch);
		unsigned char *dptr = ((unsigned char *)dreg->data + yy * dreg->pitch);
#endif
		if (flags & M_FLIP_HORIZONTAL) {
			dptr += src_w*2 - 1;
		}
		for (int xx = 0; xx < src_w; xx++) {
			unsigned char b1 = *sptr++;
			unsigned char b2 = *sptr++;
			if ((b1 & 0xf) < 0xf) {
				if (flags & M_FLIP_HORIZONTAL) {
					dptr -= 2;
				}
				else {
					dptr += 2;
				}
				continue;
			}

			unsigned char r = (b2 >> 4) & 0xf;
			unsigned char g = b2 & 0xf;
			unsigned char b = (b1 >> 4) & 0xf;

			r *= src_factor;
			r += add_r;
			g *= src_factor;
			g += add_g;
			b *= src_factor;
			b += add_b;

			b1 = (b1 & 0xf) | (b << 4);
			b2 = (r << 4) | g;

			if (flags & M_FLIP_HORIZONTAL) {
				*dptr-- = b2;
				*dptr-- = b1;
			}
			else {
				*dptr++ = b1;
				*dptr++ = b2;
			}
		}
	}

#ifdef ALLEGRO_ANDROID
	al_unlock_bitmap(tolock);
#else
	m_unlock_bitmap(src);
#endif
	al_unlock_bitmap(target);
}
#else
void add_blit(MBITMAP *src, int dx, int dy, MCOLOR color, float amount, int flags)
{
	int src_w = m_get_bitmap_width(src);
	int src_h = m_get_bitmap_height(src);
	ALLEGRO_BITMAP *target = al_get_target_bitmap();
	int tw = al_get_bitmap_width(target);
	int th = al_get_bitmap_height(target);
	int sx, sy;

	if (dx < 0) {
		sx = -dx;
		src_w += dx;
		dx = 0;
	}
	else {
		sx = 0;
	}
	if (dx + src_w > tw) {
		src_w -= (dx + src_w) - tw;
	}

	if (dy < 0) {
		sy = -dy;
		src_h += dy;
		dy = 0;
	}
	else {
		sy = 0;
	}
	if (dy + src_h > th) {
		src_h -= (dy + src_h) - th;
	}
	
	m_lock_bitmap_region(src, 0, sy, src_w, src_h, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READONLY);
	al_lock_bitmap_region(target, dx, dy, src_w, src_h, ALLEGRO_PIXEL_FORMAT_ANY, 0);

	float src_factor = 1 - amount;
	
	float add_r = color.r * amount;
	float add_g = color.g * amount;
	float add_b = color.b * amount;

	
	for (int yy = 0; yy < src_h; yy++) {
		for (int xx = 0; xx < src_w; xx++) {
			MCOLOR pixel;
			pixel = m_get_pixel(src, xx, yy+sy);
			if (pixel.a != 1.0f) continue;
			pixel.r *= src_factor;
			pixel.r += add_r;
			pixel.g *= src_factor;
			pixel.g += add_g;
			pixel.b *= src_factor;
			pixel.b += add_b;
			if (flags & M_FLIP_HORIZONTAL) {
				al_put_pixel(dx+(src_w-xx-1), yy+dy, pixel);
			}
			else {
				al_put_pixel(xx+dx, yy+dy, pixel);
			}
		}
	}

	
	m_unlock_bitmap(src);
	al_unlock_bitmap(target);
}
#endif

// Turns sprite to grayscale then tints
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
void death_blit_region(MBITMAP *src, int x, int y, int w, int h, int dx, int dy, MCOLOR color, int flags)
{
	ALLEGRO_BITMAP *target = al_get_target_bitmap();
	int tw = al_get_bitmap_width(target);
	int th = al_get_bitmap_height(target);

	if (dx < 0) {
		x = -dx;
		dx = 0;
		w -= x;
	}
	if (dx+w > tw) {
		w -= (dx+w)-tw;
	}
	if (dy < 0) {
		y= -dy;
		dy = 0;
		h -= y;
	}
	if (dy+h > th) {
		h -= (dy+h)-th;
	}

#ifdef ALLEGRO_ANDROID
	ALLEGRO_BITMAP *tolock;
	if (al_get_parent_bitmap(src->bitmap)) {
		tolock = al_get_parent_bitmap(src->bitmap);
		x += src->bitmap->xofs;
		y += src->bitmap->yofs;
	}
	else {
		tolock = src->bitmap;
	}
	ALLEGRO_LOCKED_REGION *sreg = al_lock_bitmap(tolock, ALLEGRO_PIXEL_FORMAT_RGBA_4444, ALLEGRO_LOCK_READONLY);
	ALLEGRO_LOCKED_REGION *dreg = al_lock_bitmap(target, ALLEGRO_PIXEL_FORMAT_RGBA_4444, ALLEGRO_LOCK_READWRITE);
#else
	ALLEGRO_LOCKED_REGION *sreg = m_lock_bitmap_region(src, x, y, w, h, ALLEGRO_PIXEL_FORMAT_RGBA_4444, ALLEGRO_LOCK_READONLY);
	ALLEGRO_LOCKED_REGION *dreg = al_lock_bitmap_region(target, dx, dy, w, h, ALLEGRO_PIXEL_FORMAT_RGBA_4444, ALLEGRO_LOCK_READWRITE);
#endif

	for (int yy = 0; yy < h; yy++) {
#ifdef ALLEGRO_ANDROID
		unsigned char *sptr = ((unsigned char *)sreg->data + (yy+y) * sreg->pitch + (x * 2));
		unsigned char *dptr = ((unsigned char *)dreg->data + (yy+dy) * dreg->pitch + (dx * 2));
#else
		unsigned char *sptr = ((unsigned char *)sreg->data + yy * sreg->pitch);
		unsigned char *dptr = ((unsigned char *)dreg->data + yy * dreg->pitch);
#endif
		if (flags & M_FLIP_HORIZONTAL) {
			dptr += w*2 - 1;
		}
		for (int xx = 0; xx < w; xx++) {
			unsigned char b1 = *sptr++;
			unsigned char b2 = *sptr++;
			if ((b1 & 0xf) < 0xf) {
				if (flags & M_FLIP_HORIZONTAL) {
					dptr -= 2;
				}
				else {
					dptr += 2;
				}
				continue;
			}

			unsigned char r = (b2 >> 4) & 0xf;
			unsigned char g = b2 & 0xf;
			unsigned char b = (b1 >> 4) & 0xf;
			unsigned char avg = (r + g + b) / 3;

			b1 = (b1 & 0xf) | (((int)(avg * color.b) << 4));
			b2 = ((int)(avg * color.g)) | (((int)(avg * color.r) << 4));

			if (flags & M_FLIP_HORIZONTAL) {
				*dptr-- = b2;
				*dptr-- = b1;
			}
			else {
				*dptr++ = b1;
				*dptr++ = b2;
			}
		}
	}

#ifdef ALLEGRO_ANDROID
	al_unlock_bitmap(tolock);
#else
	m_unlock_bitmap(src);
#endif
	al_unlock_bitmap(target);
}
#else
void death_blit_region(MBITMAP *src, int x, int y, int w, int h, int dx, int dy, MCOLOR color, int flags)
{
	ALLEGRO_BITMAP *target = al_get_target_bitmap();
	int tw = al_get_bitmap_width(target);
	int th = al_get_bitmap_height(target);

	if (dx < 0) {
		x = -dx;
		dx = 0;
		w -= x;
	}
	if (dx+w > tw) {
		w -= (dx+w)-tw;
	}
	if (dy < 0) {
		y= -dy;
		dy = 0;
		h -= y;
	}
	if (dy+h > th) {
		h -= (dy+h)-th;
	}

	m_lock_bitmap_region(src, x, y, w, h, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READONLY);
	al_lock_bitmap_region(target, dx, dy, w, h, ALLEGRO_PIXEL_FORMAT_ANY, 0);

	for (int yy = 0; yy < h; yy++) {
		for (int xx = 0; xx < w; xx++) {
			MCOLOR pixel = m_get_pixel(src, xx+x, yy+y);
			if (pixel.a < 1.0f) continue;
			float avg = (pixel.r + pixel.g + pixel.b) / 3.0f;
			pixel.r = avg * color.r;
			pixel.g = avg * color.g;
			pixel.b = avg * color.b;
			if (flags & M_FLIP_HORIZONTAL) {
				al_put_pixel((w-xx-1)+dx, yy+dy, pixel);
			}
			else {
				al_put_pixel(xx+dx, yy+dy, pixel);
			}
		}
	}
	
	m_unlock_bitmap(src);
	al_unlock_bitmap(target);
}
#endif

