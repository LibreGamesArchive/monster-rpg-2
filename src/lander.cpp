#include "monster2.hpp"

#include <allegro5/internal/aintern_pixels.h>

MSAMPLE boost = 0;
float boost_volume = 0.0f;

#define MAX_PARTICLES 200

class MLanderButton : public TGUIWidget {
public:
	static const int RADIUS = 20;
	static const int X = BW-5-RADIUS*2;
	static const int Y = BH-5-RADIUS*2;
	void draw(void) {
		if (getPressed()) {
			m_draw_bitmap(buttondown, x, y, 0);
		}
		else {
			m_draw_bitmap(buttonup, x, y, 0);
		}
	}
	int update(int millis) {
		monitor->update();
		return TGUI_CONTINUE;
	}
	bool acceptsFocus(void) { return true; }
	bool on(int xx, int yy) {
		float dx = xx - (x+RADIUS);
		float dy = yy - (Y+RADIUS);
		float dist = sqrtf(dx*dx + dy*dy);
		if (dist <= RADIUS)
			return true;
		return false;
	}

	bool getPressed(void) {
		for (size_t i = 0; i < monitor->touches.size(); i++) {
			if (on(monitor->touches[i].x, monitor->touches[i].y)) {
				return true;
			}
		}
		return false;
	}

	MLanderButton(bool left) {
		this->hotkeys = 0;
		this->x = X;
		this->y = Y;
		this->width = RADIUS*2;
		this->height = RADIUS*2;
		this->left = left;
		buttonup = m_load_bitmap(getResource("media/shooter/buttonup.png"));
		buttondown = m_load_bitmap(getResource("media/shooter/buttondown.png"));
		if (left) {
			x = 5;
		}
		monitor = new MouseMonitor();
	}
	virtual ~MLanderButton(void) {
		m_destroy_bitmap(buttonup);
		m_destroy_bitmap(buttondown);
		delete monitor;
	}
protected:
	bool left;
	MBITMAP *buttonup;
	MBITMAP *buttondown;
	MouseMonitor *monitor;
};


// Returns a random number between lo and hi
static float randf(float lo, float hi)
{
   float range = hi - lo;
   int n = rand() % 10000;
   float f = range * n / 10000.0f;
   return lo + f;
}

bool do_lander(void)
{
	// stop set_sets (astar with mouse)
	getInput()->set(false, false, false, false, false, false, false);

	dpad_off();

#define ADD_PARTICLE(dir) { \
	Particle p; \
	float c = lander_angle+M_PI+(M_PI/6*dir); \
	float s = lander_angle+M_PI+(M_PI/6*dir); \
	p.x = lander_x + cos(c) * 9; \
	p.y = lander_y + sin(s) * 9; \
	p.dx = cos(-c) * 0.01f + randf(-0.002f, 0.002f); \
	p.dy = sin(-s) * 0.01f + randf(-0.002f, 0.002f); \
	p.ground = BH; \
	p.color = m_map_rgb(255, 0, 0); \
	particles.push_back(p); \
	}

top:
	hide_mouse_cursor();

	playMusic("lander.ogg");

	boost = streamSample("boost.ogg", 0.0f);
	float lastVolume = 0;

	MBITMAP *lander_bmp = m_load_alpha_bitmap(getResource("media/lander.png"));
	MBITMAP *land_bmp = m_load_alpha_bitmap(getResource("media/landing_area.png"));
	MBITMAP *sky_bmp = m_load_bitmap(getResource("media/landing_area_sky.png"));

	MBITMAP *land_mem = m_load_alpha_bitmap(getResource("media/landing_area.png"), true);
	MBITMAP *lander_mem = m_load_alpha_bitmap(getResource("media/lander.png"), true);

	ALLEGRO_STATE state;
	al_store_state(&state, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS | ALLEGRO_STATE_TARGET_BITMAP);
	al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
	MBITMAP *lander_tmp = m_create_bitmap(30, 30); // check
	m_set_target_bitmap(lander_tmp);
	m_clear(al_map_rgba(0, 0, 0, 0));
	m_draw_bitmap(lander_mem, 3, 3, 0);
	al_restore_state(&state);

	AnimationSet *explosion = new_AnimationSet(getResource("media/explosion.png"));

#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
	MLanderButton *left_button = NULL;
	MLanderButton *right_button = NULL;
	if (!use_dpad) {
		left_button = new MLanderButton(true);
		right_button = new MLanderButton(false);
		tguiSetParent(0);
		tguiAddWidget(left_button);
		tguiAddWidget(right_button);
	}
#endif

	std::list<Particle> particles;

	float lander_angle = M_PI*3/2;
	float lander_x = 15; // center
	float lander_y = 15; // center
	float lander_vel = 0;
	const float lander_vel_delta = 0.001f;
	const float lander_vel_max = 0.04f;
	const float lander_angle_delta = 0.001f;
	const float gravity = 0.02f;
	bool dead = false;
	bool exploding = false;
	int explode_count = 0;
	int expl_x = 0, expl_y = 0;

	bool break_for_fade_after_draw = false;

	clear_input_events();

	while  (1) {
		int green = 0;

		al_wait_cond(wait_cond, wait_mutex);

		// Logic
		int tmp_counter = logic_counter;
		logic_counter = 0;
		if (tmp_counter > 10)
			tmp_counter = 1;

		while  (!break_for_fade_after_draw && tmp_counter > 0) {
			next_input_event_ready = true;

			tmp_counter--;

			runtime_ms += LOGIC_MILLIS;

			while (runtime_ms >= 1000) {
				runtime++;
				runtime_ms -= 1000;
			}

			/* explode */
			if (exploding) {
				explosion->update(LOGIC_MILLIS);
				explode_count += LOGIC_MILLIS;
			}

			/* apply gravity */
			lander_x += cos(M_PI/2) * gravity * LOGIC_MILLIS;
			lander_y += sin(M_PI/2) * gravity * LOGIC_MILLIS;

			/* apply jets */
			bool left = false, right = false;

#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
			if (!use_dpad)
				tguiUpdate();
#endif

			if (!dead) {
				InputDescriptor id = getInput()->getDescriptor();
				left = id.left || id.button1;
				right = id.right || id.button2;
#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_ANDROID
				ALLEGRO_MOUSE_STATE state;
				al_get_mouse_state(&state);
				if (state.buttons & 1) {
					left = true;
				}
				if (state.buttons & 2) {
					right = true;
				}
#else
				if (!use_dpad) {
					if (left_button->getPressed())
						left = true;
					if (right_button->getPressed())
						right = true;
				}
#endif
			}

			if (left) {
				if (lastVolume != 1) {
					setStreamVolume(boost, 1);
					boost_volume = 1;
					lastVolume = 1;
				}
				lander_vel += lander_vel_delta * LOGIC_MILLIS;
				if (lander_vel > lander_vel_max)
					lander_vel = lander_vel_max;
				lander_angle += lander_angle_delta * LOGIC_MILLIS;
				lander_x += cos(lander_angle) * lander_vel * LOGIC_MILLIS;
				lander_y += sin(lander_angle) * lander_vel * LOGIC_MILLIS;
				if (particles.size() < MAX_PARTICLES) {
					for (int i = 0; i < 1; i++) {
						ADD_PARTICLE(1)
					}
				}
			}
			if (right) {
				if (lastVolume != 1) {
					setStreamVolume(boost, 1);
					boost_volume = 1;
					lastVolume = 1;
				}
				lander_vel += lander_vel_delta * LOGIC_MILLIS;
				if (lander_vel > lander_vel_max)
					lander_vel = lander_vel_max;
				lander_angle -= lander_angle_delta * LOGIC_MILLIS;
				lander_x += cos(lander_angle) * lander_vel * LOGIC_MILLIS;
				lander_y += sin(lander_angle) * lander_vel * LOGIC_MILLIS;
				if (particles.size() < MAX_PARTICLES) {
					for (int i = 0; i < 1; i++) {
						ADD_PARTICLE(-1)
					}
				}
			}

			if (!left && !right && lastVolume != 0) {
				setStreamVolume(boost, 0);
				boost_volume = 0;
				lastVolume = 0;
			}

			/* Update particles */

			std::list<Particle>::iterator it;
			for (it = particles.begin(); it != particles.end();) {
				Particle &p = *it;
				p.x += p.dx * LOGIC_MILLIS;
				p.y += p.dy * LOGIC_MILLIS;
				p.color.a -= 0.001f * LOGIC_MILLIS;
				p.color.r = p.color.a;
				if (p.color.a < 0) {
					it = particles.erase(it);
				}
				else
					it++;
			}

			// end game logic check
			// this drawing really has to be done here unfortunately

			ALLEGRO_LOCKED_REGION *lr1 = al_lock_bitmap(lander_tmp->bitmap, ALLEGRO_PIXEL_FORMAT_ANY, 0);
			ALLEGRO_LOCKED_REGION *lr2 = al_lock_bitmap(land_mem->bitmap, ALLEGRO_PIXEL_FORMAT_ANY, 0);
			al_lock_bitmap(lander_mem->bitmap, ALLEGRO_PIXEL_FORMAT_ANY, 0);

			m_set_target_bitmap(lander_tmp);
			m_clear(al_map_rgba(0, 0, 0, 0));
			m_draw_rotated_bitmap(lander_mem, 15, 15, 15, 15, -(lander_angle-M_PI*3/2), 0);

			green = 0;

			int x1, x2, y1, y2;

			for (y1 = 0, y2 = lander_y - 15; y1 < 30; y1++, y2++) {
				x2 = lander_x - 15;

				char *data1 = (char *)lr1->data + y1 * lr1->pitch;
				char *data2 = (char *)lr2->data + y2 * lr2->pitch + x2 * al_get_pixel_size(lr2->format);

				for (x1 = 0; x1 < 30; x1++, x2++) {
					bool done = false;

					unsigned char r1, g1, b1, a1;
					unsigned char r2, g2, b2, a2;

					if (x2 < 0 || y2 < 0 || x2 >= al_get_bitmap_width(land_mem->bitmap) || y2 >= al_get_bitmap_height(land_mem->bitmap)) {
						done = true;
					}

					if (!done) {
						MCOLOR c1;
						MCOLOR c2;

						_AL_INLINE_GET_PIXEL(lr1->format, data1, c1, true);
						_AL_INLINE_GET_PIXEL(lr2->format, data2, c2, true);

						m_unmap_rgba(c1, &r1, &g1, &b1, &a1);
						m_unmap_rgba(c2, &r2, &g2, &b2, &a2);

						if (a1 < 255 || a2 < 255) {
							continue;
						}
					}

					if (!exploding) {
						if ((done || r2 != 0 || g2 != 255 || b2 != 0 || fabs(lander_angle-(M_PI*3/2)) > (M_PI/12))) {
							exploding = true;
							expl_x = lander_x-explosion->getWidth()/2,
							expl_y = lander_y-explosion->getHeight()/2,
							explode_count = 0;
							dead = true;
							loadPlayDestroy("explosion.ogg");
							break;
						}
						else {
							green++;
						}
					}
				}
			}

			al_unlock_bitmap(lander_tmp->bitmap);
			al_unlock_bitmap(land_mem->bitmap);
			al_unlock_bitmap(lander_mem->bitmap);


			if (!dead && green > 3) {
				draw_counter++;
				break;
			}
		}

		if (break_for_fade_after_draw || draw_counter > 0) {
			draw_counter = 0;

			if (!break_for_fade_after_draw) {
				set_target_backbuffer();
			}

			m_draw_bitmap(sky_bmp, 0, 0, 0);
			m_draw_bitmap(land_bmp, 0, 0, 0);

			if (exploding) {
				explosion->draw(expl_x, expl_y, 0);
			}
			else {
				m_draw_rotated_bitmap(lander_bmp, 15, 15,
						      lander_x, lander_y, -(lander_angle-M_PI*3/2), 0);
			}

			/* draw particles */

			ALLEGRO_VERTEX *verts = new ALLEGRO_VERTEX[particles.size()];
			int vcount = 0;

			std::list<Particle>::iterator it;
			for (it = particles.begin(); it != particles.end(); it++) {
				Particle& p = *it;
				verts[vcount].x = p.x;
				verts[vcount].y = p.y;
				verts[vcount].z = 0;
				verts[vcount].color = al_map_rgba_f(
					p.color.r*p.color.a,
					p.color.g*p.color.a,
					p.color.b*p.color.a,
					p.color.a
				);
				vcount++;
			}

			m_draw_prim(verts, 0, 0, 0, vcount, ALLEGRO_PRIM_POINT_LIST);

			delete[] verts;

#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
			if (!use_dpad)
				tguiDraw();
#endif

			drawBufferToScreen();

			if (break_for_fade_after_draw) {
				break;
			}

			if ((!dead && green > 3) ||  (explode_count > 3000)) {
				break_for_fade_after_draw = true;
				prepareForScreenGrab1();
				continue;
			}

			m_flip_display();
		}

		if (is_close_pressed()) {
			do_close();
			close_pressed = false;
		}
		if (break_main_loop) {
			break;
		}
	}

	if (break_for_fade_after_draw) {
		break_for_fade_after_draw = false;
		prepareForScreenGrab2();
		fadeOut(black);
	}

	show_mouse_cursor();

#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
	if (!use_dpad) {
		tguiDeleteWidget(left_button);
		tguiDeleteWidget(right_button);
		delete left_button;
		delete right_button;
	}
#endif

	m_destroy_bitmap(lander_bmp);
	m_destroy_bitmap(land_bmp);
	m_destroy_bitmap(land_mem);
	m_destroy_bitmap(sky_bmp);
	m_destroy_bitmap(lander_tmp);
	m_destroy_bitmap(lander_mem);
	delete explosion;

	prepareForScreenGrab1();
	m_clear(black);
	prepareForScreenGrab2();
	m_rest(5);

	playMusic("");

	MSAMPLE tmpsamp = boost;
	boost = 0;
	destroyStream(tmpsamp);
	boost_volume = 0;

	playMusic("underwater_final.ogg");

	if (dead) {
		bool ret = prompt("G A M E O V E R", "Try Again?", 1, 1);
		if (ret)
			goto top;
	}
	else {
		char buf[100];
		sprintf(buf, "50 level points");

		bool again = prompt("V I C T O R Y !", "Try Again?", 0, 0, std::string(buf));

		if (again)
			goto top;
	}

	if (dead) {
		if (saveFilename) saveTime(saveFilename);
		dpad_on();
		return false;
	}
	else {
		for (int i = 0; i < MAX_PARTY; i++) {
			if (break_main_loop) {
				break;
			}
			if (party[i]) {
				while (levelUp(party[i], 50)) {
					if (break_main_loop) {
						break;
					}
				}
			}
		}
		dpad_on();
		return true;
	}

#undef ADD_PARTICLE
}

