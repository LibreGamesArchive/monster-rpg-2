#include "monster2.hpp"

//void draw_points_locked(ALLEGRO_VERTEX *verts, int n);

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

#ifdef ALLEGRO_IPHONE
	playMusic("lander.flac");
	playAmbience("boost.flac");
#else
	playMusic("lander.ogg");
	playAmbience("boost.ogg");
#endif

	setAmbienceVolume(0);

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
	m_draw_bitmap(land_bmp, 3, 3, 0);
	al_restore_state(&state);
	
	AnimationSet *explosion = new AnimationSet(getResource("media/explosion.png"));

	MLanderButton *left_button = NULL;
	MLanderButton *right_button = NULL;

	if (!use_dpad) {
		left_button = new MLanderButton(true);
		right_button = new MLanderButton(false);
		tguiSetParent(0);
		tguiAddWidget(left_button);
		tguiAddWidget(right_button);
	}

	std::list<Particle> particles;

	float lander_angle = M_PI*3/2;
	//float lander_w = m_get_bitmap_width(lander_bmp);
	//float lander_h = m_get_bitmap_height(lander_bmp);
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

	long last_particle_1 = tguiCurrentTimeMillis();
	long last_particle_2 = tguiCurrentTimeMillis();

	while  (1) {
		int green = 0;
		
		al_wait_cond(wait_cond, wait_mutex);
		
		// Logic
		int tmp_counter = logic_counter;
		logic_counter = 0;
		if (tmp_counter > 10)
			tmp_counter = 1;
			
		while (tmp_counter > 0) {
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
				if (explode_count > 3000) {
					goto done;
				}
			}

			/* apply gravity */
			lander_x += cos(M_PI/2) * gravity * LOGIC_MILLIS;
			lander_y += sin(M_PI/2) * gravity * LOGIC_MILLIS;

			/* apply jets */
			bool left = false, right = false;

			if (!use_dpad)
				tguiUpdate();
		
			if (!dead) {
				if (use_dpad) {
					InputDescriptor ie = getInput()->getDescriptor();
					left = ie.left | ie.button1;
					right = ie.right | ie.button2;
				}
				else {
					if (left_button->getPressed())
						left = true;
					if (right_button->getPressed())
						right = true;
				}
			}
				
			if (left) {
				setAmbienceVolume(1);
				lander_vel += lander_vel_delta * LOGIC_MILLIS;
				if (lander_vel > lander_vel_max)
					lander_vel = lander_vel_max;
				lander_angle += lander_angle_delta * LOGIC_MILLIS;
				lander_x += cos(lander_angle) * lander_vel * LOGIC_MILLIS;
				lander_y += sin(lander_angle) * lander_vel * LOGIC_MILLIS;
				if (particles.size() < 200) {
					int t = tguiCurrentTimeMillis();
					//int n = t-last_particle_1;
					last_particle_1 = t;
					for (int i = 0; i < 1; i++) {
						ADD_PARTICLE(1)
					}
				}
			}
			if (right) {
				setAmbienceVolume(1);
				lander_vel += lander_vel_delta * LOGIC_MILLIS;
				if (lander_vel > lander_vel_max)
					lander_vel = lander_vel_max;
				lander_angle -= lander_angle_delta * LOGIC_MILLIS;
				lander_x += cos(lander_angle) * lander_vel * LOGIC_MILLIS;
				lander_y += sin(lander_angle) * lander_vel * LOGIC_MILLIS;
				if (particles.size() < 200) {
					int t = tguiCurrentTimeMillis();
					//int n = t-last_particle_2;
					last_particle_2 = t;
					for (int i = 0; i < 1; i++) {
						ADD_PARTICLE(-1)
					}
				}
			}

			if (!left && !right)
				setAmbienceVolume(0);

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
			m_set_target_bitmap(lander_tmp);
			m_clear(al_map_rgba(0, 0, 0, 0));
			m_draw_rotated_bitmap(lander_mem, 15, 15, 15, 15, -(lander_angle-M_PI*3/2), 0);
			
			green = 0;
			
			int x1, x2, y1, y2;
			
			for (y1 = 0, y2 = lander_y - 15; y1 < 30; y1++, y2++) {
				for (x1 = 0, x2 = lander_x - 15; x1 < 30; x1++, x2++) {
					MCOLOR c1 = m_get_pixel(lander_tmp, x1, y1);
					MCOLOR c2 = m_get_pixel(land_mem, x2, y2);
					unsigned char r1, g1, b1, a1;
					unsigned char r2, g2, b2, a2;
					m_unmap_rgba(c1, &r1, &g1, &b1, &a1);
					m_unmap_rgba(c2, &r2, &g2, &b2, &a2);
					
					//printf("1: %d %d %d %d\n", r1, g1, b1, a1);
					//printf("2: %d %d %d %d\n", r2, g2, b2, a2);
					
					bool done = false;
					
					if (x2 < -3 || x2 >= BW+3 || y2 < -3 || y2 >= BH+3) {
						done = true;
					}
					else if (a1 < 255 || a2 < 255) {
						continue;
					}
					
					if (!exploding) {
					// 5 degrees ->
						if ((done || r2 != 0 || g2 != 255 || b2 != 0 || fabs(lander_angle-(M_PI*3/2)) > (M_PI/36))) {
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
			
			if (!dead && green > 3) {
				draw_counter++;
				break;
			}			
		}

		if (draw_counter > 0) {
			draw_counter = 0;

			m_set_blender(M_ONE, M_INVERSE_ALPHA, white);

			m_set_target_bitmap(buffer);
			
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

			ALLEGRO_VERTEX verts[particles.size()];
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

#ifdef __linux__XXX
			draw_points_locked(verts, vcount);
#else
			m_draw_prim(verts, 0, 0, 0, vcount, ALLEGRO_PRIM_POINT_LIST);
#endif
		
			if (!use_dpad)
				tguiDraw();

			drawBufferToScreen();
			m_flip_display();
		}

		if (!dead && green > 3) {
			goto done;
		}			
		
		if (is_close_pressed()) {
			do_close();
			close_pressed = false;
		}
	}

done:

	clear_input_events();

	if (!use_dpad) {
		tguiDeleteWidget(left_button);
		tguiDeleteWidget(right_button);
		delete left_button;
		delete right_button;
	}

	m_destroy_bitmap(lander_bmp);
	m_destroy_bitmap(land_bmp);
	m_destroy_bitmap(land_mem);
	m_destroy_bitmap(sky_bmp);
	m_destroy_bitmap(lander_tmp);
	m_destroy_bitmap(lander_mem);
	delete explosion;

	//m_set_target_bitmap(buffer);
	fadeOut(black);
	m_set_target_bitmap(buffer);
	m_clear(black);
	m_rest(5);

	playAmbience("");
	playMusic("");

	playMusic("underwater_final.ogg");

	if (dead) {
		if (prompt("G A M E O V E R", "Try Again?", 1, 1))
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
			if (party[i])
			 while (levelUp(party[i], 50))
			 	;
		}
		dpad_on();
		return true;
	}

#undef ADD_PARTICLE
}

