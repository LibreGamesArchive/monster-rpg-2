// Portions (c) Mesa Library

#define NO_SIN
#include "monster2.hpp"

#if defined ALLEGRO_IPHONE || defined ALLEGRO_MACOSX
#include "joypad.hpp"
#endif

// stuff allegro doesn't have yet
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
#define glFrustum glFrustumf
#define glOrtho glOrthof
#endif

#include <cfloat>

bool in_archery = false;

void disable_zbuffer(void)
{
	al_set_render_state(ALLEGRO_DEPTH_TEST, 0);
	al_set_render_state(ALLEGRO_WRITE_MASK, ALLEGRO_MASK_RGBA);
}

void enable_zbuffer(bool less_equal)
{
	al_set_render_state(ALLEGRO_DEPTH_TEST, 1);
	al_set_render_state(ALLEGRO_WRITE_MASK,
		ALLEGRO_MASK_RGBA | ALLEGRO_MASK_DEPTH);
	if (less_equal) {
		al_set_render_state(ALLEGRO_DEPTH_FUNCTION,
			ALLEGRO_RENDER_LESS_EQUAL);
	}
	else {
		al_set_render_state(ALLEGRO_DEPTH_FUNCTION,
			ALLEGRO_RENDER_LESS);
	}
}

static void clear_zbuffer(void)
{
	al_clear_depth_buffer(1.0f);
}

struct VERT {
	float x, y, z;
};
struct TEXCOORD {
	float u, v;
};
struct FACE {
	int v[4];
	int t[4];
	int n;
};

static MODEL *load_model2(const char *filename, MBITMAP *tex)
{
	int sz;
	unsigned char *bytes = slurp_file(filename, &sz);
	if (!bytes)
		return NULL;

	ALLEGRO_FILE *file = al_open_memfile(bytes, sz, "rb");
	if (!file)
		return NULL;
	
	int true_w, true_h;
#ifdef A5_D3D
	al_get_d3d_texture_size(tex->bitmap, &true_w, &true_h);
#else
	al_get_opengl_texture_size(tex->bitmap, &true_w, &true_h);
#endif

	VERT v;
	TEXCOORD t;
	FACE f;

	std::vector<VERT> verts;
	std::vector<TEXCOORD> texcoords;
	std::vector<FACE> faces;
	std::vector<int> splits;

	char line[1000];

	int ntris = 0; // num triangles

	while ((al_fgets(file, line, 1000))) {
		if (line[0] == '-') {
			splits.push_back(ntris * 3);
		}
		else if (line[0] == 'v' && line[1] == ' ') {
			sscanf(line, "v %g %g %g", &v.x, &v.y, &v.z);
			verts.push_back(v);
		}
		else if (line[0] == 'v') {
			sscanf(line, "vt %g %g", &t.u, &t.v);
			t.u *= true_w;
			t.v *= true_h;;
			t.v = true_h - t.v;
			texcoords.push_back(t);
		}
		else if (line[0] == 'f') {
			f.n = sscanf(line, "f %d/%d %d/%d %d/%d %d/%d",
				&f.v[0], &f.t[0],
				&f.v[1], &f.t[1],
				&f.v[2], &f.t[2],
				&f.v[3], &f.t[3]
			) / 2;
			for (int i = 0; i < 4; i++) {
				f.v[i]--;
				f.t[i]--;
			}
			faces.push_back(f);
			if (f.n == 4) {
				ntris += 2;
			}
			else {
				ntris++;
			}
		}
	}

	splits.push_back(ntris*3);

	MODEL *m = new MODEL;

	for (size_t i = 0; i < splits.size(); i++) {
		int nv;
		if (i == 0) {
			nv = splits[i];
		}
		else {
			nv = splits[i] - splits[i-1];
		}
		m->num_verts.push_back(nv);
		m->verts.push_back(new ALLEGRO_VERTEX[nv]);
	}

	int cv = 0; // current vertex
	int cp = 0; // current part (0->splits.size()-1)

	for (size_t i = 0; i < faces.size(); i++) {
		FACE &face = faces[i];
		ALLEGRO_VERTEX *vert;
		if (face.n == 4) {
			int order[] = { 0, 1, 2, 0, 2, 3 };
			//int order[] = { 2, 1, 0, 3, 2, 0 };
			for (int j = 0; j < 6; j++) {
				vert = &m->verts[cp][cv];
				cv++;
				int o = order[j];
				vert->x = verts[face.v[o]].x;
				vert->y = verts[face.v[o]].y;
				vert->z = verts[face.v[o]].z;
				vert->u = texcoords[face.t[o]].u;
				vert->v = texcoords[face.t[o]].v;
				vert->color = al_map_rgb_f(1, 1, 1);
			}
		}
		else {
			for (int j = 0; j < 3; j++) {
				vert = &m->verts[cp][cv];
				cv++;
				vert->x = verts[face.v[j]].x;
				vert->y = verts[face.v[j]].y;
				vert->z = verts[face.v[j]].z;
				vert->u = texcoords[face.t[j]].u;
				vert->v = texcoords[face.t[j]].v;
				vert->color = al_map_rgb_f(1, 1, 1);
			}
		}
		if (cv == splits[cp]) {
			cp++;
			cv = 0;
		}
	}

	al_fclose(file);

	delete[] bytes;

	return m;
}

static MODEL *load_model(const char *filename, bool is_volcano = false, int tex_size = 0)
{
	MODEL *m = new MODEL;

	union {
		float fl;
		uint32_t in;
	};

	int sz;
	unsigned char *bytes = slurp_file(filename, &sz);
	if (!bytes)
		return NULL;

	ALLEGRO_FILE *f = al_open_memfile(bytes, sz, "rb");
	if (!f)
		return NULL;


	int vcount = 0;

	/* count everything */
	while (1) {
		int nv = al_fgetc(f);
		if (nv == EOF)
			break;
		for (int n = 0 ; n < nv; n++) {
			al_fgetc(f); // skip
			al_fgetc(f); // skip
			al_fgetc(f); // skip
			al_fread32le(f); // skip
			al_fread32le(f); // skip
			al_fread32le(f); // skip
		}
		if (nv == 3)
			vcount += 3;
		else
			vcount += 6;
	}

	m->num_verts.push_back(vcount);
	m->verts.push_back(new ALLEGRO_VERTEX[m->num_verts[0]]);

	al_fseek(f, 0, ALLEGRO_SEEK_SET);

	bool load_txt = false;

	if (false) {
		load_txt = true;
	}
	
	if (load_txt) {
		char real_filename[1000];
		strcpy(real_filename, filename);
		strcpy(real_filename+strlen(real_filename)-3, "txt");
		
		int i = 0;

		while (1) {
			int rgb[4][3];
			double xyz[4][3];

			char line[2000];
			char *silence_warning = al_fgets(f, line, 2000);
			(void)silence_warning;

			int n = sscanf(line,
				"%d %d %d %lf %lf %lf "
				"%d %d %d %lf %lf %lf "
				"%d %d %d %lf %lf %lf "
				"%d %d %d %lf %lf %lf ",
				&rgb[0][0], &rgb[0][1], &rgb[0][2],
				&xyz[0][0], &xyz[0][1], &xyz[0][2],
				&rgb[1][0], &rgb[1][1], &rgb[1][2],
				&xyz[1][0], &xyz[1][1], &xyz[1][2],
				&rgb[2][0], &rgb[2][1], &rgb[2][2],
				&xyz[2][0], &xyz[2][1], &xyz[2][2],
				&rgb[3][0], &rgb[3][1], &rgb[3][2],
				&xyz[3][0], &xyz[3][1], &xyz[3][2]
			);
			
			for (int j = 0; j < 3; j++) {
				m->verts[0][i].x = (float)xyz[j][0];
				m->verts[0][i].y = (float)xyz[j][1];
				m->verts[0][i].z = (float)xyz[j][2];
				m->verts[0][i].color = al_map_rgb(
					rgb[j][0],
					rgb[j][1],
					rgb[j][2]
				    );
				i++;
			}
			if (n > (6*3)) {
				m->verts[0][i] = m->verts[0][i-3];
				m->verts[0][i+1] = m->verts[0][i-1];
				m->verts[0][i+2].x = (float)xyz[3][0];
				m->verts[0][i+2].y = (float)xyz[3][1];
				m->verts[0][i+2].z = (float)xyz[3][2];
				m->verts[0][i+2].color = al_map_rgb(
					  rgb[3][0],
					  rgb[3][1],
					  rgb[3][2]
				      );
				i += 3;
			}
			if (i == vcount)
				break;
		}
	}
	else {
		int i = 0;

		while (1) {
			int r, g, b;
			float x, y, z;
			std::vector<float> vv;
			std::vector<int> cv;
			int nv = al_fgetc(f);
			if (nv == EOF)
				break;
			for (int n = 0; n < nv; n++) {
				r = al_fgetc(f);
				g = al_fgetc(f);
				b = al_fgetc(f);
				in = (int)al_fread32le(f);
				x = fl;
				in = (int)al_fread32le(f);
				y = fl;
				in = (int)al_fread32le(f);
				z = fl;
				vv.push_back(x);
				vv.push_back(y);
				vv.push_back(z);
				cv.push_back(r);
				cv.push_back(g);
				cv.push_back(b);
			}
			for (int j = 0; j < 3; j++) {
				m->verts[0][i].x = vv[j*3+0];
				m->verts[0][i].y = vv[j*3+1];
				m->verts[0][i].z = vv[j*3+2];
				m->verts[0][i].color = al_map_rgb(
							cv[j*3+0],
							cv[j*3+1],
							cv[j*3+2]
						    );
				i++;
			}
			if (vv.size() > 9) {
				m->verts[0][i] = m->verts[0][i-3];
				m->verts[0][i+1] = m->verts[0][i-1];
				m->verts[0][i+2].x = vv[3*3+0];
				m->verts[0][i+2].y = vv[3*3+1];
				m->verts[0][i+2].z = vv[3*3+2];
				m->verts[0][i+2].color = al_map_rgb(
							  cv[3*3+0],
							  cv[3*3+1],
							  cv[3*3+2]
						      );
				i += 3;
			}
			vv.clear();
			cv.clear();
		}

		if (is_volcano) {
			for (int i = 0; i < vcount; i += 6) {
				m->verts[0][i+0].z *= 0.5f;
				m->verts[0][i+1].z *= 0.5f;
				m->verts[0][i+2].z *= 0.5f;
				m->verts[0][i+3].z *= 0.5f;
				m->verts[0][i+4].z *= 0.5f;
				m->verts[0][i+5].z *= 0.5f;

				float inc = tex_size / 50;
				float xx = ((i/6) % 50) * inc;
				float yy = ((i/6) / 50) * inc;

				m->verts[0][i+0].u = xx;
				m->verts[0][i+0].v = yy;
				m->verts[0][i+1].u = xx+inc;
				m->verts[0][i+1].v = yy;
				m->verts[0][i+2].u = xx+inc;
				m->verts[0][i+2].v = yy+inc;
				m->verts[0][i+3].u = xx+inc;
				m->verts[0][i+3].v = yy+inc;
				m->verts[0][i+4].u = xx;
				m->verts[0][i+4].v = yy+inc;
				m->verts[0][i+5].u = xx;
				m->verts[0][i+5].v = yy;
			}
		}
	}

	al_fclose(f);

	delete[] bytes;

	return m;
}


static void draw_model_tex(MODEL *m, MBITMAP *texture)
{
	for (size_t i = 0; i < m->verts.size(); i++) {
		m_draw_prim(
			m->verts[i],
			0,
			texture,
			0,
			m->num_verts[i],
			ALLEGRO_PRIM_TRIANGLE_LIST
		);
	}
}

void set_projection(float neer, float farr, bool reverse_y, bool rotate)
{
	ALLEGRO_DISPLAY *display = al_get_current_display();
	float aspect = (float)BW/BH;
	float ymax = 0.01f*(float)tan((R2D(M_PI/4))*M_PI/360.0f);
	float ymin = -ymax;
	if (reverse_y) {
		float tmp = ymax;
		ymax = ymin;
		ymin = tmp;
	}
	float xmin = ymin * aspect;
	float xmax = ymax * aspect;

	ALLEGRO_TRANSFORM t;
	al_identity_transform(&t);
	/* Not really sure why ymin/ymax are swapped here */
	al_perspective_transform(&t, xmin, ymax, neer, xmax, ymin, farr);
	al_set_projection_transform(display, &t);
}

float goblin_speed;

static AnimationSet *goblin_animSet;
static AnimationSet *goblin_dead;
static AnimationSet *goblin_swipe;

struct Goblin {
	float x, y;
	int w, h;
	float dx, dy;
	int climbCount;
	int restCount;
	int swipeCount;
	int deadCount;
};

static void setGoblinDest(Goblin &g)
{
	g.dx = g.x + (rand() % 100 - 50);
	g.dy = g.y + rand() % 20 + 10;
}

static void newGoblin(Goblin &g)
{
	g.x = rand() % (BW-100) + 50;
	g.y = -rand()%20;
	g.w = TILE_SIZE;
	g.h = TILE_SIZE;
	setGoblinDest(g);
	g.climbCount = 0;
	g.restCount = 0;
	g.swipeCount = 0;
	g.deadCount = 0;
}

// returns player_dead == true
static bool moveGoblin(Goblin &g, int step)
{
	if (g.deadCount > 0) {
		g.deadCount -= step;
		if (g.deadCount <= 0)
			g.deadCount = -1;
		return false;
	}
	if (g.swipeCount) {
		g.swipeCount -= step;
		if (g.swipeCount <= 0)
			return true;
	}

	if (g.restCount) {
		g.restCount -= step;
		if (g.restCount <= 0) {
			g.restCount = 0;
			if (use_dpad)
				g.swipeCount = 1000;
			else
				g.swipeCount = 500;
			loadPlayDestroy("swipe.ogg");
		}
		return false;
	}

	if (g.climbCount) {
		g.climbCount -= step;
		if (g.climbCount <= 0) {
			g.climbCount = 0;
			g.restCount = 1500;
		}
		return false;
	}

	float angle;
	angle = (float)atan2(g.dy-g.y, g.dx-g.x);

	g.x += goblin_speed * step * (float)cos(angle);
	g.y += goblin_speed * step * (float)sin(angle);

	if (fabs(g.x-g.dx) < 1.5 || fabs(g.y-g.dy) < 1.5)
		setGoblinDest(g);

	if (g.y >= BW-TILE_SIZE) {
		g.climbCount = 4000;
		g.x = rand() % (BW-100) + 50;
		g.y = BH-60;
		g.w = 50;
		g.h = 50;
	}

	return false;
}

static int NUM_GOBLINS;

static int real_archery(int *accuracy_pts)
{
	in_archery = true;

	hide_mouse_cursor();

	dpad_off();

	int scr_w = al_get_display_width(display);
	int scr_h = al_get_display_height(display);
	m_set_mouse_xy(display, scr_w/2, scr_h/2);
	int last_mouse_x = scr_w/2;
	int last_mouse_y = scr_h/2;

	// stop set_sets (astar with mouse)
	getInput()->set(false, false, false, false, false, false, false);

	if (config.getDifficulty() == CFG_DIFFICULTY_EASY) {
		NUM_GOBLINS = 25;
	}
	else {
		NUM_GOBLINS = 50;
	}

#ifdef ALLEGRO_IPHONE
#if defined WITH_60BEAT
	if (use_dpad || joypad_connected() || airplay_connected || is_sb_connected())
#else
	if (use_dpad || joypad_connected() || airplay_connected)
#endif
#else
#if defined ALLEGRO_MACOSX
	bool jp_conn = joypad_connected();
#else
	bool jp_conn = false;
#endif
	if (use_dpad || jp_conn)
#endif
		goblin_speed = 0.015f;
	else
		goblin_speed = 0.03f;

	int num_shots = 0;

	bool clicked = false;
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
	current_mouse_x = BW/2;
	current_mouse_y = BH/2;
#endif
	
	set_target_backbuffer();
	m_clear(m_map_rgb(0, 0, 0));

	playMusic("shmup2.ogg");

	std::vector<Goblin> goblins;
	int total_goblins = 0;
	int dead_goblins = 0;
	int new_goblin_count = 2000;

	float target_x = BW/2;
	float target_y = BH/2;

	int flags = al_get_new_bitmap_flags();

	int mipmap = 0;
	int linear = 0;
#if !defined OPENGLES && !defined A5_D3D
	mipmap = ALLEGRO_MIPMAP;
	linear = ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR;
#endif

	al_set_new_bitmap_flags(flags | linear | mipmap);
	MBITMAP *bow_tex = m_load_bitmap(getResource("models/bow.png"));
	MBITMAP *arrow_tex =
		m_load_bitmap(getResource("models/arrow.png"));
	al_set_new_bitmap_flags(flags);

	MODEL *bow_model = load_model2(getResource("models/bow.vtx"), bow_tex);
	if (!bow_model) {
		native_error("Couldn't load models/bow.vtx.");
	}
	MODEL *arrow_model = load_model2(getResource("models/arrow.vtx"), arrow_tex);
	if (!arrow_model) {
		native_error("Couldn't load models/arrow.vtx.");
	}

	MBITMAP *grass, *tower;

	grass =	m_load_bitmap(getResource("media/towergrass.png"));
	tower = m_load_bitmap(getResource("media/towertop.png"));

	goblin_animSet = new_AnimationSet(getResource("objects/Goblin.png"));
	goblin_animSet->setSubAnimation("walk_s");
	goblin_dead = new_AnimationSet(getResource("objects/Goblin.png"));
	goblin_dead->setSubAnimation("dead_arrow");
	goblin_swipe = new_AnimationSet(getResource("media/goblin_hand_swipe.png"));

	MBITMAP *progress = m_load_bitmap(getResource("media/Goblin_meter.png"));


	const float max_yrot = M_PI/2;
	const float max_xrot = M_PI/2;
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
	const float aim_speed = 0.16f;
#else
	const float aim_speed = 0.22f;
#endif
	const float arrow_start_z = -125;
	const float bow_z = -125;

	float arrow_z = arrow_start_z;
	bool drawing = true;
	bool drawn = false;
	int hiddenCount = 0;
	bool dead = false;

	bool really_done = false;

	playPreloadedSample("bow_draw.ogg");

	bool break_for_fade_after_draw = false;

	clear_input_events();

	while  (1) {
		al_wait_cond(wait_cond, wait_mutex);
		int tmp_counter = logic_counter;
		logic_counter = 0;
		if (tmp_counter > 10)
			tmp_counter = 1;
		while  (!break_for_fade_after_draw && tmp_counter > 0) {
			next_input_event_ready = true;

			tmp_counter--;
			if (is_close_pressed()) {
				do_close();
				close_pressed = false;
			}
			if (break_main_loop) {
				goto done;
			}

			runtime_ms += LOGIC_MILLIS;
			while (runtime_ms >= 1000) {
				runtime++;
				runtime_ms -= 1000;
			}

			new_goblin_count -= LOGIC_MILLIS;
			if (total_goblins < NUM_GOBLINS && new_goblin_count <= 0) {
				new_goblin_count = 2000;
				Goblin g;
				newGoblin(g);
				goblins.push_back(g);
				total_goblins++;
			}
			else if (total_goblins >= NUM_GOBLINS && goblins.size() == 0) {
				really_done = true;
				break_for_fade_after_draw = true;
				prepareForScreenGrab1();
				continue;
			}

			goblin_animSet->update(LOGIC_MILLIS);

			for (unsigned int j = 0; j < goblins.size(); j++) {
				if (moveGoblin(goblins[j], LOGIC_MILLIS)) {
					dead = true;
					break;
				}
			}

			if (dead) {
				break_for_fade_after_draw = true;
				prepareForScreenGrab1();
				continue;
			}

			std::vector<Goblin>::iterator it;

			for (it = goblins.begin(); it != goblins.end();) {
				Goblin &g = *it;
				if (g.deadCount < 0) {
					it = goblins.erase(it);
					dead_goblins++;
				}
				else
					it++;
			}

			const float draw_speed = -0.001f*0.5;
			const float max_draw = -0.5f*0.5;

			if (drawing) {
				arrow_z -= draw_speed * LOGIC_MILLIS;
				if (fabs(arrow_z-arrow_start_z) >= fabs(max_draw)) {
					arrow_z = arrow_start_z-max_draw;
					drawing = false;
					drawn = true;
				}
			}

			if (hiddenCount > 0) {
				hiddenCount -= LOGIC_MILLIS;
				if (hiddenCount <= 0) {
					hiddenCount = 0;
					arrow_z = arrow_start_z;
					drawing = true;
				}
			}

			InputDescriptor id = { 0, };
			id = getInput()->getDescriptor();

			bool mouse_button_1_pressed = false;

#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_ANDROID
			ALLEGRO_MOUSE_STATE state;
			al_get_mouse_state(&state);
			mouse_button_1_pressed = state.buttons & 1;
#else
			if (!use_dpad) {
				if (!clicked && !released) {
					clicked = true;
				}
			}
#endif

			if (((id.button1 || mouse_button_1_pressed) && drawn) || (!use_dpad && clicked && released && drawn)) {
				num_shots++;
				clicked = false;
				drawn = false;
				hiddenCount = 500;
				playPreloadedSample("bow_release_and_draw.ogg");
				// check for hit on goblin, set dead count, set sub anim
				std::vector<Goblin>::iterator it;
				for (it = goblins.begin(); it != goblins.end();) {
					Goblin &g = *it;
					if (g.deadCount == 0) {
						if (g.restCount > 0 || g.swipeCount > 0) {
							if (fabs(g.x-target_x) < 50 && fabs(g.y-target_y) < 50) {
								loadPlayDestroy("cartoon_fall.ogg");
								it = goblins.erase(it);
								dead_goblins++;
								break;
							}
						}
						else if (fabs(g.x-target_x) < TILE_SIZE/2 && g.y > target_y &&
						         g.y-TILE_SIZE < target_y) {
							g.deadCount = 2000;
							break;
						}
					}
					it++;
				}
			}

#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
			if (!use_dpad) {
				al_lock_mutex(click_mutex);
				target_x = current_mouse_x;
				target_y = current_mouse_y;
				al_unlock_mutex(click_mutex);
			}
#endif

			if (id.left) {
				target_x -= aim_speed * LOGIC_MILLIS;
				if (target_x < 0) target_x = 0;
			}
			else if (id.right) {
				target_x += aim_speed * LOGIC_MILLIS;
				if (target_x >= BW)
					target_x = BW-1;
			}

			if (id.up) {
				target_y -= aim_speed * LOGIC_MILLIS;
				if (target_y < 0) target_y = 0;
			}
			else if (id.down) {
				target_y += aim_speed * LOGIC_MILLIS;
				if (target_y >= BH)
					target_y = BH-1;
			}

#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_ANDROID
			al_get_mouse_state(&state);
			int mx = state.x;
			int my = state.y;
			if (mx != last_mouse_x || my != last_mouse_y) {
				last_mouse_x = mx;
				last_mouse_y = my;
				scr_w = al_get_display_width(display);
				scr_h = al_get_display_height(display);
				target_x = (float)mx / scr_w * BW;
				target_y = (float)my / scr_h * BH;
			}
#endif
		}

		if (break_for_fade_after_draw || draw_counter > 0) {
			draw_counter = 0;

			disable_zbuffer();
		
			if (!break_for_fade_after_draw) {
				set_target_backbuffer();
			}

			float yrot = (target_x / BW - 0.5f) * max_xrot;
			float xrot = (target_y / BH - 0.5f) * max_yrot;

			float arrow_dist = 0.1;

			m_set_blender(M_ONE, M_ZERO, white);

			m_draw_bitmap(grass, 0, 0, 0);

			m_set_blender(M_ONE, M_INVERSE_ALPHA, white);

			// Need 3 passes to draw goblins
			for (unsigned int i = 0; i < goblins.size(); i++) {
				if (!goblins[i].climbCount && !goblins[i].restCount && !goblins[i].swipeCount) {
					if (goblins[i].deadCount > 0)
						goblin_dead->draw(goblins[i].x-TILE_SIZE/2, goblins[i].y-TILE_SIZE, 0);
					else
						goblin_animSet->draw(goblins[i].x-TILE_SIZE/2, goblins[i].y-TILE_SIZE, 0);
				}
			}
			for (unsigned int i = 0; i < goblins.size(); i++) {
				if (goblins[i].swipeCount || goblins[i].restCount) {
					goblin_animSet->drawScaled(goblins[i].x-25, goblins[i].y-25, 100, 100);
				}
			}
			for (unsigned int i = 0; i < goblins.size(); i++) {
				if (goblins[i].swipeCount) {
					float x = (1-(float)goblins[i].swipeCount / 500) * BW;
					if (x < BW/2) {
						goblin_swipe->setSubAnimation("left");
					}
					else
						goblin_swipe->setSubAnimation("right");
					goblin_swipe->drawScaled(x-50, BH/2-50, 100, 100);
				}
			}

			m_set_blender(M_ONE, M_INVERSE_ALPHA, white);
			m_draw_bitmap(tower, 0, 0, 0);

			m_draw_line(target_x-4, target_y, target_x+4, target_y, m_map_rgb(255, 0, 0));
			m_draw_line(target_x, target_y-4, target_x, target_y+4, m_map_rgb(255, 0, 0));

			/* Draw progress meter */
			m_draw_rectangle(3, 3, 9, 51, white, M_FILLED);
			int meter_height;
			if (really_done) meter_height = 48;
			else meter_height = 47 * (float)dead_goblins/NUM_GOBLINS;
			m_draw_rectangle(3, 51, 9, 51-meter_height,
					 m_map_rgb(255, 0, 0), M_FILLED);
			m_draw_bitmap(progress, 2, 2, 0);

			ALLEGRO_TRANSFORM proj_push;
			ALLEGRO_TRANSFORM view_push;
			ALLEGRO_TRANSFORM view_transform;
			al_copy_transform(&view_push, al_get_current_transform());
			al_copy_transform(&proj_push, al_get_projection_transform(display));

			set_projection(1, 1000);

#ifdef A5_D3D
			D3DVIEWPORT9 backup_vp;
			al_get_d3d_device(display)->GetViewport(&backup_vp);
			D3DVIEWPORT9 vp;
			vp.X = 0;
			vp.Y = 0;
			vp.Width = al_get_display_width(display);
			vp.Height = al_get_display_height(display);
			vp.MinZ = 0;
			vp.MaxZ = 1;
			al_get_d3d_device(display)->SetViewport(&vp);
#endif

			// without this the feathers sometimes go invisible
			float old_xrot = xrot;
			float old_yrot = yrot;
			if (fabs(xrot) < 0.001) xrot = 0.001;
			if (fabs(yrot) < 0.001) yrot = 0.001;
			
			enable_zbuffer(false);
			clear_zbuffer();
			enable_cull_face(false);

			al_identity_transform(&view_transform);
			al_rotate_transform_3d(&view_transform, 1, 0, 0, -xrot);
			al_rotate_transform_3d(&view_transform, 0, 1, 0, -yrot);
			al_translate_transform_3d(&view_transform, 0, 0, bow_z);
#ifdef A5_D3D
			al_translate_transform_3d(&view_transform, 0.5f, 0.5f, 0.0f);
#endif
			al_use_transform(&view_transform);
			draw_model_tex(bow_model, bow_tex);

			if (!hiddenCount) {
				al_identity_transform(&view_transform);
				al_translate_transform_3d(&view_transform, 0, 0, -(arrow_start_z-arrow_z));
				al_rotate_transform_3d(&view_transform, 1, 0, 0, -xrot);
				al_rotate_transform_3d(&view_transform, 0, 1, 0, -yrot);
				al_translate_transform_3d(&view_transform, arrow_dist, 0, arrow_start_z);
#ifdef A5_D3D
				al_translate_transform_3d(&view_transform, 0.5f, 0.5f, 0.0f);
#endif
				al_use_transform(&view_transform);
				draw_model_tex(arrow_model, arrow_tex);
			}

			disable_cull_face();
			disable_zbuffer();

			xrot = old_xrot;
			yrot = old_yrot;

#ifdef A5_D3D
			al_get_d3d_device(display)->SetViewport(&backup_vp);
#endif

			al_use_transform(&view_push);
			al_set_projection_transform(display, &proj_push);

			drawBufferToScreen();

			if (break_for_fade_after_draw) {
				break;
			}

#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_ANDROID
			bool reset_mouse = false;
			int reset_x, reset_y;
			if (prompt_for_close_on_next_flip) {
				reset_mouse = true;
				ALLEGRO_MOUSE_STATE state;
				al_get_mouse_state(&state);
				reset_x = state.x;
				reset_y = state.y;
			}
#endif
			
			m_flip_display();

#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_ANDROID
			if (reset_mouse && !break_main_loop) {
				m_set_mouse_xy(display, reset_x, reset_y);
			}
#endif
		}
	}

done:

	if (break_for_fade_after_draw) {
		break_for_fade_after_draw = false;
		prepareForScreenGrab2();
		fadeOut(black);
	}

	disable_zbuffer();
	
	set_target_backbuffer();

	goblins.clear();

	delete bow_model;
	delete arrow_model;

	m_destroy_bitmap(grass);
	m_destroy_bitmap(tower);

	delete goblin_animSet;
	delete goblin_dead;
	delete goblin_swipe;

	m_destroy_bitmap(progress);

	playMusic("underwater_final.ogg");

	*accuracy_pts = ((float)NUM_GOBLINS/num_shots) * 30 + 2;

	dpad_on();

	show_mouse_cursor();
	int dx, dy, dw, dh;
	get_screen_offset_size(&dx, &dy, &dw, &dh);
	int mousex = al_get_display_width(display)-custom_cursor_w-20-dx;
	int mousey = al_get_display_height(display)-custom_cursor_h-20-dy;
	m_set_mouse_xy(display, mousex, mousey);

	in_archery = false;

	if (break_main_loop) {
		return 2;
	}

	return dead;
}


bool archery(bool for_points)
{
	if (for_points) {
		anotherDoDialogue("Faelon: Here comes the horde! Let's have some fun!...\n", true, false, false);
	}

	prepareForScreenGrab1();
	m_clear(m_map_rgb(0, 0, 0));
	prepareForScreenGrab2();

	if (dpad_type != DPAD_TOTAL_1 && dpad_type != DPAD_TOTAL_2)
		notify("Drag your bow into", "position and release", "to fire...");

	while (1) {
		int accuracy_pts;
		int dead = real_archery(&accuracy_pts);
		if (dead == 2) {
			return 2;
		}

		prepareForScreenGrab1();
		m_clear(black);
		m_rest(0.5);

		if (dead) {
			bool ret = prompt("G A M E O V E R", "Try Again?", 1, 1);
			prepareForScreenGrab2();
			if (ret)
				continue;
		}
		else {
			char buf[100];

			if (for_points) {
				sprintf(buf, _t("%d level points (%d accuracy bonus)"), accuracy_pts+30, accuracy_pts);
			}
			else {
				buf[0] = 0;
			}

			bool again = prompt("V I C T O R Y !", "Try Again?", 0, 0, std::string(buf));
			prepareForScreenGrab2();

			if (again)
				continue;
		}

		if (dead) {
			if (saveFilename) saveTime(saveFilename);
			return false;
		}
		else {
			if (for_points) {
				for (int i = 0; i < MAX_PARTY; i++) {
					if (break_main_loop) {
						break;
					}
					if (party[i]) {
						while (levelUp(party[i],  30+accuracy_pts)) {
							if (break_main_loop) {
								break;
							}
						}
					}
				}
			}
			return true;
		}
	}
}

void enable_cull_face(bool ccw)
{
#ifdef A5_OGL
	glCullFace(ccw ? GL_FRONT : GL_BACK);
	glEnable(GL_CULL_FACE);
#else
	LPDIRECT3DDEVICE9 device = al_get_d3d_device(display);
	device->SetRenderState(D3DRS_CULLMODE, ccw ? D3DCULL_CCW : D3DCULL_CW);
#endif
}

void disable_cull_face(void)
{
#ifdef A5_OGL
	glDisable(GL_CULL_FACE);
#else
	LPDIRECT3DDEVICE9 device = al_get_d3d_device(display);
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
#endif
}


static MODEL *create_ring(int sd /* subdivisions */, MBITMAP *texture)
{
	MODEL *m = new MODEL;
	m->num_verts.push_back(6*sd);
	m->verts.push_back(new ALLEGRO_VERTEX[m->num_verts[0]]);

	ALLEGRO_COLOR trans_white = al_map_rgba_f(0.75f, 0.75f, 0.75f, 0.75f);

	float angle_inc = (M_PI*2) / sd;
	float outer_dist = 1.0;
	float inner_dist = 0.75;
	float scale = 4.0;

	for (int i = 0; i < sd; i++) {
		float angle = i * angle_inc;
		float angle2 = angle + angle_inc;
		float inner_angle = ((angle2 - angle) / 2) + angle;
		m->verts[0][i*6+0].x = sin(angle) * outer_dist * scale;
		m->verts[0][i*6+0].y = cos(angle) * outer_dist * scale;
		m->verts[0][i*6+0].z = 1;
		m->verts[0][i*6+0].u = 0;
		m->verts[0][i*6+0].v = m_get_bitmap_height(texture)-1;
		m->verts[0][i*6+0].color = trans_white;
		m->verts[0][i*6+1].x = sin(angle2) * outer_dist * scale;
		m->verts[0][i*6+1].y = cos(angle2) * outer_dist * scale;
		m->verts[0][i*6+1].z = 1;
		m->verts[0][i*6+1].u = m_get_bitmap_width(texture)-1;
		m->verts[0][i*6+1].v = m_get_bitmap_height(texture)-1;
		m->verts[0][i*6+1].color = trans_white;
		m->verts[0][i*6+2].x = sin(inner_angle) * inner_dist * scale;
		m->verts[0][i*6+2].y = cos(inner_angle) * inner_dist * scale;
		m->verts[0][i*6+2].z = 1;
		m->verts[0][i*6+2].u = m_get_bitmap_width(texture)/2;
		m->verts[0][i*6+2].v = 0;
		m->verts[0][i*6+2].color = trans_white;
		float tmp = angle2;
		angle = inner_angle;
		angle2 = inner_angle + angle_inc;
		inner_angle = tmp;
		m->verts[0][i*6+3].x = sin(angle) * inner_dist * scale;
		m->verts[0][i*6+3].y = cos(angle) * inner_dist * scale;
		m->verts[0][i*6+3].z = 1;
		m->verts[0][i*6+3].u = 0;
		m->verts[0][i*6+3].v = 0;
		m->verts[0][i*6+3].color = trans_white;
		m->verts[0][i*6+4].x = sin(angle2) * inner_dist * scale;
		m->verts[0][i*6+4].y = cos(angle2) * inner_dist * scale;
		m->verts[0][i*6+4].z = 1;
		m->verts[0][i*6+4].u = m_get_bitmap_width(texture)-1;
		m->verts[0][i*6+4].v = 0;
		m->verts[0][i*6+4].color = trans_white;
		m->verts[0][i*6+5].x = sin(inner_angle) * outer_dist * scale;
		m->verts[0][i*6+5].y = cos(inner_angle) * outer_dist * scale;
		m->verts[0][i*6+5].z = 1;
		m->verts[0][i*6+5].u = m_get_bitmap_width(texture)/2;
		m->verts[0][i*6+5].v = m_get_bitmap_height(texture)-1;
		m->verts[0][i*6+5].color = trans_white;
	}

	return m;
}

void volcano_scene(void)
{
	hide_mouse_cursor();

	dpad_off();

	set_target_backbuffer();
	m_clear(m_map_rgb(0, 0, 0));

	enum {
		STAGE_GROUND,
		STAGE_RISING,
		STAGE_SHOOTING,
		STAGE_FLASHING,
		STAGE_POOFING
	};

	int stage = STAGE_GROUND;
	int count = 0;

	/* Ground stage variables */
	float dland_angle = 0.00001f;
	float land_angle = 0.0f;
	const float MAX_LAND_ANGLE = M_PI/128;

	float staff_oy = 0.0f;
	float staff_dy1 = 0.0002f;
	float staff_oz = 0.0f;
	float staff_dz = 0.015f;
	float staff_a = 0;

#ifdef ALLEGRO_RASPBERRYPI
	float staff_z_translate = 0.4f;
	float land_z_translate = 0.05f;
#else
	float staff_z_translate = 0.0f;
	float land_z_translate = 0.0f;
#endif

	MBITMAP *stars, *moon, *ring_texture;

	stars = m_load_bitmap(getResource("media/stars.png"));
	moon = m_load_bitmap(getResource("media/moon.png"));
	ring_texture = m_load_alpha_bitmap(getResource("media/ring_texture.png"));

	int flags = al_get_new_bitmap_flags();

	int mipmap = 0;
	int linear = 0;
#if !defined OPENGLES && !defined A5_D3D
	mipmap = ALLEGRO_MIPMAP;
	linear = ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR;
#endif

	al_set_new_bitmap_flags(flags | linear | mipmap);
	MBITMAP *land_texture = m_load_bitmap(getResource("media/volcano_texture.png"));
	MBITMAP *staff_tex = m_load_bitmap(getResource("models/staff.png"));
	al_set_new_bitmap_flags(flags);

	MODEL *land_model = load_model(getResource("models/volcano_new.raw"), true, m_get_bitmap_width(land_texture));
	if (!land_model) {
		native_error("Couldn't load models/volcano_new.raw.");
	}

	MODEL *staff_model = load_model2(getResource("models/staff.vtx"), staff_tex);
	if (!staff_model) {
		native_error("Couldn't load models/staff.vtx.");
	}
	MODEL *ring_model = create_ring(32, ring_texture);

	float star_top_y = 0-(m_get_bitmap_height(stars)-BH);
	float star_top_dy1 = 0.02f;
	float star_top_dy2 = 0.1f;

	float ring_z = -100;
	float ring_z_delta = 0.1;


	ALLEGRO_TRANSFORM orig_proj;
	al_copy_transform(&orig_proj, al_get_projection_transform(display));
	ALLEGRO_TRANSFORM proj_transform, view_transform;

	clear_input_events();

	bool break_for_fade_after_draw = false;

	while  (1) {
		al_wait_cond(wait_cond, wait_mutex);
		int tmp_counter = logic_counter;
		logic_counter = 0;
		if (tmp_counter > 10)
			tmp_counter = 1;
		while  (!break_for_fade_after_draw && tmp_counter > 0) {
			next_input_event_ready = true;

			tmp_counter--;
			if (is_close_pressed()) {
				do_close();
				close_pressed = false;
			}
			if (break_main_loop) {
				goto done;
			}
			
			count += LOGIC_MILLIS;

			if (stage == STAGE_GROUND) {
				land_angle += dland_angle * LOGIC_MILLIS;
				if (dland_angle > 0) {
					if (land_angle >= MAX_LAND_ANGLE) {
						dland_angle = -dland_angle;
						land_angle = MAX_LAND_ANGLE;
					}
				}
				else {
					if (land_angle <= -MAX_LAND_ANGLE) {
						dland_angle = -dland_angle;
						land_angle = -MAX_LAND_ANGLE;
					}
				}
				if (count > 5000 && fabs(land_angle) < 0.0001f) {
					count = 0;
					stage = STAGE_RISING;
					land_angle = 0;
				}
			}
			else if (stage == STAGE_RISING) {
				staff_oy += staff_dy1 * LOGIC_MILLIS;
				star_top_y += star_top_dy1 * LOGIC_MILLIS;
				if (star_top_y > -BH) {
					star_top_y = -BH;
					count = 0;
					stage = STAGE_SHOOTING;
					staff_a = M_PI/4;
					loadPlayDestroy("staff_fly.ogg");
				}
			}
			else if (stage == STAGE_SHOOTING) {
				star_top_y += star_top_dy2 * LOGIC_MILLIS;
				staff_oz += staff_dz * LOGIC_MILLIS;
				if (star_top_y >= 0) {
					star_top_y = 0;
					count = 0;
					stage = STAGE_FLASHING;
				}
			}
			else if (stage == STAGE_FLASHING) {
				if (count > 100) {
					count = 0;
					stage = STAGE_POOFING;
					loadPlayDestroy("staff_poof.ogg");
				}
			}
		}
			
		if (stage == STAGE_POOFING) {
			ring_z += ring_z_delta * LOGIC_MILLIS;
			if (!break_for_fade_after_draw && count > 5000) {
				break_for_fade_after_draw = true;
				prepareForScreenGrab1();
				continue;
			}
		}

		if (break_for_fade_after_draw || draw_counter > 0) {
			draw_counter = 0;

			disable_zbuffer();

			if (!break_for_fade_after_draw) {
				set_target_backbuffer();
			}

			m_set_blender(M_ONE, M_INVERSE_ALPHA, white);

			if (stage == STAGE_SHOOTING && count > 0) {
				// Can't do this without a single retained buffer to draw on
				m_clear(black);
			}
			else {
				if (stage == STAGE_FLASHING)
					m_clear(m_map_rgb(100, 100, 100));
				else
					m_clear(black);
			}
			m_draw_bitmap(stars, 0, star_top_y, 0);
			m_draw_bitmap(moon, (BW-m_get_bitmap_width(moon))/2, star_top_y+30, 0);

			drawBufferToScreen(false);

			clear_zbuffer();

			ALLEGRO_TRANSFORM proj_push, view_push;
			al_copy_transform(&proj_push, al_get_projection_transform(display));
			al_copy_transform(&view_push, al_get_current_transform());

			al_identity_transform(&proj_transform);
			al_perspective_transform(&proj_transform, -1, -(float)BH/BW, 1, 1, (float)BH/BW, 1000);
			al_set_projection_transform(display, &proj_transform);

#ifdef A5_D3D
			LPDIRECT3DDEVICE9 device = al_get_d3d_device(display);
			D3DVIEWPORT9 vp, old;
			device->GetViewport(&old);
			vp.X = 0;
			vp.Y = 0;
			vp.Width = al_get_display_width(display);
			vp.Height = al_get_display_height(display);
			vp.MinZ = 0;
			vp.MaxZ = 1;
			device->SetViewport(&vp);
#endif

			enable_zbuffer();
			enable_cull_face(true);

			al_identity_transform(&view_transform);
			al_rotate_transform_3d(&view_transform, 1, 0, 0, M_PI/2);
			al_rotate_transform_3d(&view_transform, 0, 1, 0, land_angle);
			al_translate_transform_3d(&view_transform, 0, 0.1f+staff_oy, -(0.33f+land_z_translate));
			al_scale_transform_3d(&view_transform, 50, 50, 50);
			al_use_transform(&view_transform);
			draw_model_tex(land_model, land_texture);

			clear_zbuffer();
			enable_cull_face(false);

			al_identity_transform(&view_transform);
			al_rotate_transform_3d(&view_transform, 0, 1, 0, -land_angle);
			al_rotate_transform_3d(&view_transform, 1, 0, 0, M_PI);
			al_rotate_transform_3d(&view_transform, 1, 0, 0, staff_a);
			al_translate_transform_3d(&view_transform, 0, 0, -1.25f);
			al_translate_transform_3d(&view_transform, 0, 0.25f, -(staff_oz+staff_z_translate));
			al_rotate_transform_3d(&view_transform, 0, 1, 0, -land_angle);
#ifdef A5_D3D
			al_translate_transform_3d(&view_transform, 0.5f, 0.5f, 0.0f);
#endif
			al_use_transform(&view_transform);

			if (stage != STAGE_POOFING) {
				draw_model_tex(staff_model, staff_tex);
			}
			else {
				al_identity_transform(&view_transform);
				al_translate_transform_3d(&view_transform, 0.25, 0.25, ring_z);
				al_use_transform(&view_transform);
				disable_cull_face();
				draw_model_tex(ring_model, ring_texture);
			}

			disable_cull_face();
			disable_zbuffer();

			al_set_projection_transform(display, &proj_push);
			al_use_transform(&view_push);

#ifdef A5_D3D
			device->SetViewport(&old);
#endif
			if (break_for_fade_after_draw) {
				break;
			}
			
			m_flip_display();
		}
	}
done:

	if (break_for_fade_after_draw) {
		break_for_fade_after_draw = false;
		prepareForScreenGrab2();
		fadeOut(black);
	}

#if defined A5_OGL
	disable_cull_face();
	disable_zbuffer();
#endif

	delete land_model;
	delete staff_model;
	delete ring_model;

	m_destroy_bitmap(stars);
	m_destroy_bitmap(moon);
	m_destroy_bitmap(ring_texture);

	m_destroy_bitmap(land_texture);

	m_destroy_bitmap(staff_tex);

	if (true /*use_programmable_pipeline*/)
	{
		al_set_projection_transform(display, &orig_proj);
	}

	dpad_on();

	show_mouse_cursor();
}
