#include "monster2.hpp"

#ifdef ALLEGRO_ANDROID
#include "java.h"
#endif

MBITMAP *battle_buf = NULL;

bool transitioning;

bool global_draw_controls = true;
static TemporaryTextWidget omnipotentTexts[MAX_PARTY];
static double current_time = -1;

void stopAllOmni()
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
	if (use_dpad && dpad_buttons && draw_controls && global_draw_controls) {
		if ((dpad_type == DPAD_TOTAL_2 || dpad_type == DPAD_HYBRID_2)) {
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
		else {
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
	if (transitioning || preparingForScreenGrab) {
		return;
	}

	long now = al_get_time();
	bool draw_red = ((now - last_shake_check) < 0.5) && !on_title_screen;

	if (!gamepadConnected() && draw_red && global_draw_red && !path_head) {
		m_draw_triangle(0, 0, 16, 0, 0, 16, al_map_rgba_f(tint.r*tint.a, 0, 0, tint.a));
	}

#ifndef NO_GAMECENTER
#if defined ALLEGRO_IPHONE || defined ALLEGRO_MACOSX
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
			quick_draw(
				achievement_bmp->bitmap,
				al_map_rgba_f(alpha*tint.r, alpha*tint.g, alpha*tint.b, alpha*tint.a),
				m_get_bitmap_width(achievement_bmp)/2, m_get_bitmap_height(achievement_bmp)/2,
				BW/2, BH/2, scale, scale, 0, 0
			);
		}
	}
#endif
#endif

	draw_the_controls(draw_controls, tint);
}

void drawBufferToScreen(bool draw_controls)
{
	int dx, dy, dw, dh;
	get_screen_offset_size(&dx, &dy, &dw, &dh);

	set_target_backbuffer();

	if (!transitioning) {
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
		if (!gamepadConnected()) {
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
					
					mTextout(medium_font, _t(text), (len+4)/2, (h+4)/2,
						white, black,
						WGT_TEXT_NORMAL, true);

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
	}

	drawOverlay((gamepadConnected() ? false : draw_controls), al_map_rgba_f(0.5f, 0.5f, 0.5f, 0.5f));

	if (fps_on && !preparingForScreenGrab) {
		char buf[10];
		sprintf(buf, "%d", fps);
		mTextout_simple(buf, 1, 1, al_map_rgb_f(1, 1, 0));
	}

#ifdef ALLEGRO_RASPBERRYPI
	if (!preparingForScreenGrab && !is_cursor_hidden() && !hide_mouse) {
		ALLEGRO_BITMAP *target = al_get_target_bitmap();
		if (target == al_get_backbuffer(display)) {
			int cx, cy, cw, ch;
			al_get_clipping_rectangle(&cx, &cy, &cw, &ch);
			al_set_clipping_rectangle(
				0, 0,
				al_get_display_width(display),
				al_get_display_height(display)
			);
			ALLEGRO_MOUSE_STATE state;
			al_get_mouse_state(&state);
			m_draw_bitmap_identity_view(custom_cursor_bmp, state.x, state.y, 0);
			al_set_clipping_rectangle(cx, cy, cw, ch);
		}
	}
#endif
}

void draw_shadow(MBITMAP *bmp, int x, int y, bool hflip)
{
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
	
	float hx = cx + (cos(ha)*(r-4));
	float hy = cy + (sin(ha)*(r-4));

	al_draw_line(cx, cy, mx, my, black, 1);
	al_draw_line(cx, cy, hx, hy, black, 1);
}


void m_put_alpha_pixel(MBITMAP *bmp, int x, int y, MCOLOR c)
{
	MCOLOR bg = m_get_pixel(bmp, x, y);
	
	ALLEGRO_BITMAP *old_target = al_get_target_bitmap();
	al_set_target_bitmap(bmp->bitmap);
	
	al_put_pixel(x, y, m_map_rgb_f(
		MIN(1.0f, c.r*c.a+bg.r*(1-c.a)),
		MIN(1.0f, c.g*c.a+bg.g*(1-c.a)),
		MIN(1.0f, c.b*c.a+bg.b*(1-c.a))
	));
	
	al_set_target_bitmap(old_target);
}

/* BITMAP IS IGNORED FIXME */
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

	delete[] verts;
}

static void fade(int startAlpha, int endAlpha, int length, MCOLOR color)
{
	transitioning = true;

	dpad_off();
	global_draw_red = false;
	global_draw_controls = false;

	unsigned char r, g, b;
	long total = 0;

	int dx, dy, dw, dh;
	get_screen_offset_size(&dx, &dy, &dw, &dh);

	ALLEGRO_TRANSFORM backup, t;
	al_copy_transform(&backup, al_get_current_transform());
	al_identity_transform(&t);
	al_use_transform(&t);

	long start = tguiCurrentTimeMillis();

	while (total < length) {
		long now = tguiCurrentTimeMillis();
		int elapsed = (int)(now - start);
		total += elapsed;
		start = now;
		float p = (float)total / length;
		if (p > 1) p = 1;
		int a = (int)(startAlpha - (p * (startAlpha - endAlpha)));
		r = (int)(color.r*a);
		g = (int)(color.g*a);
		b = (int)(color.b*a);

		set_target_backbuffer();
	
		m_draw_bitmap(tmpbuffer, 0, 0, 0);

		al_draw_filled_rectangle(dx, dy, dx+dw, dy+dh, al_map_rgba(r, g, b, a));
		
		drawBufferToScreen(false);
		
		m_flip_display();
	}

	if (startAlpha < endAlpha) {
		m_clear(color);
	}
	else {
		m_draw_bitmap(tmpbuffer, 0, 0, 0);
	}
	drawBufferToScreen(false);
	m_flip_display();

	al_use_transform(&backup);

	dpad_on();
	global_draw_red = true;
	global_draw_controls = true;

	transitioning = false;
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
static bool transition(bool focusing, double length, bool can_cancel = false, bool toggle_dpad = true)
{
	transitioning = true;
	
	int dx, dy, dw, dh;
	get_screen_offset_size(&dx, &dy, &dw, &dh);
	int disp_w = al_get_display_width(display);
	int disp_h = al_get_display_height(display);

	dpad_off();
	global_draw_red = false;
	global_draw_controls = false;

	int flags = al_get_new_bitmap_flags();
	al_set_new_bitmap_flags(flags | NO_PRESERVE_TEXTURE);
	MBITMAP *tmp = m_create_bitmap(disp_w, disp_h);
	al_set_new_bitmap_flags(flags);

	ALLEGRO_TRANSFORM backup, t;
	al_copy_transform(&backup, al_get_current_transform());
	al_identity_transform(&t);
	al_use_transform(&t);
		
	double start = al_get_time();
	bool ret = false;

	while (true) {
		double now = al_get_time();
		double elapsed = (now - start);
		float p = (float)(elapsed / length);
		if (p > 1) p = 1;
		if (p < 0) p = 0;
		if (focusing)
			p = 1.0 - p;
		int size = p*32*screenScaleX + screenScaleX;
		if (size < 1) size = 1;
		p = 1.0 - p;
		int rectw = p * dw;
		int recth = p * dh;
		if (rectw < 1) rectw = 1;
		if (recth < 1) recth = 1;

		m_set_target_bitmap(tmp);
		quick_draw(tmpbuffer->bitmap, dx, dy, dw, dh, 0, 0, dw/size, dh/size, 0);

		set_target_backbuffer();
		m_clear(black);

		quick_draw(tmp->bitmap, 0, 0, dw/size, dh/size, dx, dy, dw, dh, 0);

		al_draw_filled_rectangle(0, 0, dx+(dw-rectw)/2, disp_h, black);
		al_draw_filled_rectangle(0, 0, disp_w, dy+(dh-recth)/2, black);
		al_draw_filled_rectangle(dx+(dw+rectw)/2, 0, disp_w, disp_h, black);
		al_draw_filled_rectangle(0, dy+(dh+recth)/2, disp_w, disp_h, black);

		drawBufferToScreen(false);

		m_flip_display();
		
		if ((now - start) >= length) {
			break;
		}
	}

	if (!focusing) {
		set_target_backbuffer();
		m_clear(black);
		drawBufferToScreen(false);
		m_flip_display();
	}

	m_destroy_bitmap(tmp);

	set_target_backbuffer();
	al_use_transform(&backup);

	dpad_on();
	global_draw_red = true;
	global_draw_controls = true;

	transitioning = false;

	return ret;
}

/* Transition draws a increasing/decreasing rectangular view of
 * the area with the inside focussing in/out.
 */
bool transitionIn(bool can_cancel, bool toggle_dpad)
{
	bool ret = transition(true, 0.6, can_cancel, toggle_dpad);
	return ret;
}


void transitionOut(bool toggle_dpad)
{
	transition(false, 0.6, false, toggle_dpad);
}


void battleTransition()
{
	debug_message("battleTransition 1\n");

	transitioning = true;
	
	int dx, dy, dw, dh;
	get_screen_offset_size(&dx, &dy, &dw, &dh);

	if (!use_programmable_pipeline) {
		int flags = al_get_new_bitmap_flags();
		al_set_new_bitmap_flags(flags | NO_PRESERVE_TEXTURE);
		MBITMAP *tmp = m_create_bitmap(dw, dh);
		al_set_new_bitmap_flags(flags);
		m_draw_scaled_target(tmpbuffer, dx, dy, dw, dh, 0, 0, dw, dh, tmp);
		debug_message("battleTransition 2\n");

		set_target_backbuffer();
		m_draw_bitmap_identity_view(tmpbuffer, 0, 0, 0);
		m_flip_display();

		dpad_off();
		global_draw_red = false;
		global_draw_controls = false;

		int format = al_get_new_bitmap_format();
		al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_RGB_565);
		flags = al_get_new_bitmap_flags();
		al_set_new_bitmap_flags(flags | NO_PRESERVE_TEXTURE);
		MBITMAP *bufcopy2 = m_create_bitmap(dw, dh);
		al_set_new_bitmap_flags(flags);
		al_set_new_bitmap_format(format);

		prepareForScreenGrab1();
		battle->draw();
		prepareForScreenGrab2();

		m_draw_scaled_target(tmpbuffer, dx, dy, dw, dh, 0, 0, dw, dh, bufcopy2);

		float *heights = new float[dw];
		int phases = rand() % 3 + 1;
		int *phase_lengths = new int[phases];
		int *phase_peaks = new int[phases];
		int max = INT_MIN;
		for (int i = 0; i < phases; i++) {
			phase_lengths[i] = dw/phases;
		}
		for (int i = 0; i < phases-1; i++) {
			int d = (rand()%60-30);
			phase_lengths[i] += d;
			phase_lengths[i+1] -= d;
		}
		for (int i = 0; i < phases; i++) {
			phase_peaks[i] = (rand() % (int)(30*screenScaleY)) + (25*screenScaleY);
			if (phase_peaks[i] > max) {
				max = phase_peaks[i];
			}
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

		ALLEGRO_TRANSFORM backup, t;
		set_target_backbuffer();
		al_copy_transform(&backup, al_get_current_transform());
		al_identity_transform(&t);
		al_use_transform(&t);

		while (elapsed < length) {
			if (step) {
				set_target_backbuffer();

				m_draw_bitmap(tmp, dx, dy, 0);

				quick(true);
				for (int i = 0; i < dw; i++) {
					float p = (float)elapsed/length;
					int h = (heights[i] + p*(dh+max));
					if (h > 0)
						m_draw_bitmap_region(bufcopy2,
							i, 0, 1, h, dx+i, dy, 0);
				}
				quick(false);

				drawBufferToScreen(false);

				m_flip_display();
			}

			long end = tguiCurrentTimeMillis();
			step = (int)(end - start);
			elapsed += step;
			start = end;
		}

		al_use_transform(&backup);
		
		dpad_on();
		global_draw_red = true;
		global_draw_controls = true;

		m_destroy_bitmap(bufcopy2);
		m_destroy_bitmap(tmp);

		delete[] phase_lengths;
		delete[] phase_peaks;
		delete[] heights;
	
		transitioning = false;
		return;
	}
	debug_message("battleTransition 3\n");

	dpad_off();
		
	int flags = al_get_new_bitmap_flags();
	al_set_new_bitmap_flags(flags | NO_PRESERVE_TEXTURE);
	MBITMAP *tmp = m_create_bitmap(
		al_get_bitmap_width(tmpbuffer->bitmap),
		al_get_bitmap_height(tmpbuffer->bitmap)
	);
	al_set_new_bitmap_flags(flags);
	ALLEGRO_STATE s;
	al_store_state(&s, ALLEGRO_STATE_TARGET_BITMAP);
	m_set_target_bitmap(tmp);
	ALLEGRO_TRANSFORM t;
	al_identity_transform(&t);
	al_scale_transform(&t, screenScaleX, screenScaleY);
	al_translate_transform(&t, dx, dy);
	al_use_transform(&t);
	battle->draw();

	long now = tguiCurrentTimeMillis();
	long start = now;
	int length = 700;
	float angle = 0;

	while ((now - start) < (length*2)) {
		int elapsed = (int)(now - start);
		angle = ((double)elapsed / (length*2.0)) * (M_PI*2);

		set_target_backbuffer();
		ALLEGRO_TRANSFORM backup, t;
		al_copy_transform(&backup, al_get_current_transform());
		al_identity_transform(&t);
		al_use_transform(&t);
		use_shader(warp);
		al_set_shader_float("alpha", 1);
		al_set_shader_float("angle", angle);
		al_set_shader_float("tex_bot", 1);
		m_draw_bitmap(tmpbuffer, 0, 0, 0);
		use_shader(NULL);
		use_shader(warp);
		float alpha = angle / (M_PI*2);
		al_set_shader_float("alpha", alpha);
		al_set_shader_float("angle", angle);
		al_set_shader_float("tex_bot", 1);
		m_draw_bitmap(tmp, 0, 0, 0);
		use_shader(NULL);
		al_use_transform(&backup);

		drawBufferToScreen(false);
		m_flip_display();
		now = tguiCurrentTimeMillis();
	}
	debug_message("battleTransition 15\n");

	debug_message("battleTransition 16\n");

	dpad_on();

	m_destroy_bitmap(tmp);

	al_restore_state(&s);

	transitioning = false;
	debug_message("battleTransition 17\n");
}

void add_blit(MBITMAP *src, int dx, int dy, MCOLOR color, float amount, int flags)
{
	int src_w = m_get_bitmap_width(src);
	int src_h = m_get_bitmap_height(src);
	ALLEGRO_BITMAP *target = al_get_target_bitmap();
	int tw = al_get_bitmap_width(target);
	int th = al_get_bitmap_height(target);
	int sy;

	if (dx < 0) {
		src_w += dx;
		dx = 0;
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

