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

#define A(row,col)  a[(col<<2)+row]
#define B(row,col)  b[(col<<2)+row]
#define P(row,col)  product[(col<<2)+row]

static void mesa_mult(float *product, const float *a, const float *b)
{
	int i;
	for (i = 0; i < 4; i++) {
		const float ai0=A(i,0),  ai1=A(i,1),  ai2=A(i,2),  ai3=A(i,3);
		P(i,0) = ai0 * B(0,0) + ai1 * B(1,0) + ai2 * B(2,0) + ai3 * B(3,0);
		P(i,1) = ai0 * B(0,1) + ai1 * B(1,1) + ai2 * B(2,1) + ai3 * B(3,1);
		P(i,2) = ai0 * B(0,2) + ai1 * B(1,2) + ai2 * B(2,2) + ai3 * B(3,2);
		P(i,3) = ai0 * B(0,3) + ai1 * B(1,3) + ai2 * B(2,3) + ai3 * B(3,3);
	}
}

void mesa_frustum(float *transform,
                  float left, float right,
                  float bottom, float top,
                  float nearval, float farval)
{
	float x, y, a, b, c, d;
	float m[16];

	x = (2.0F*nearval) / (right-left);
	y = (2.0F*nearval) / (top-bottom);
	a = (right+left) / (right-left);
	b = (top+bottom) / (top-bottom);
	c = -(farval+nearval) / ( farval-nearval);
	d = -(2.0F*farval*nearval) / (farval-nearval);  /* error? */

#define M(row,col)  m[col*4+row]
	M(0,0) = x;
	M(0,1) = 0.0F;
	M(0,2) = a;
	M(0,3) = 0.0F;
	M(1,0) = 0.0F;
	M(1,1) = y;
	M(1,2) = b;
	M(1,3) = 0.0F;
	M(2,0) = 0.0F;
	M(2,1) = 0.0F;
	M(2,2) = c;
	M(2,3) = d;
	M(3,0) = 0.0F;
	M(3,1) = 0.0F;
	M(3,2) = -1.0F;
	M(3,3) = 0.0F;
#undef M

	mesa_mult(transform, transform, m);
}

void mesa_rotate(float *transform,
                 float angle, float x, float y, float z )
{
	float xx, yy, zz, xy, yz, zx, xs, ys, zs, one_c, s, c;
	float m[16];
	bool optimized;

	s = (float) sin( angle * (M_PI/180.0) );
	c = (float) cos( angle * (M_PI/180.0) );

	memset(m, 0, sizeof(float)*16);
	m[0] = m[5] = m[10] = m[15] = 1.0f;

	optimized = false;

#define M(row,col)  m[col*4+row]

	if (x == 0.0F) {
		if (y == 0.0F) {
			if (z != 0.0F) {
				optimized = true;
				/* rotate only around z-axis */
				M(0,0) = c;
				M(1,1) = c;
				if (z < 0.0F) {
					M(0,1) = s;
					M(1,0) = -s;
				}
				else {
					M(0,1) = -s;
					M(1,0) = s;
				}
			}
		}
		else if (z == 0.0F) {
			optimized = true;
			/* rotate only around y-axis */
			M(0,0) = c;
			M(2,2) = c;
			if (y < 0.0F) {
				M(0,2) = -s;
				M(2,0) = s;
			}
			else {
				M(0,2) = s;
				M(2,0) = -s;
			}
		}
	}
	else if (y == 0.0F) {
		if (z == 0.0F) {
			optimized = true;
			/* rotate only around x-axis */
			M(1,1) = c;
			M(2,2) = c;
			if (x < 0.0F) {
				M(1,2) = s;
				M(2,1) = -s;
			}
			else {
				M(1,2) = -s;
				M(2,1) = s;
			}
		}
	}

	if (!optimized) {
		const float mag = sqrtf(x * x + y * y + z * z);

		if (mag <= 1.0e-4) {
			/* no rotation, leave mat as-is */
			return;
		}

		x /= mag;
		y /= mag;
		z /= mag;


		/*
		 *     Arbitrary axis rotation matrix.
		 *
		 *  This is composed of 5 matrices, Rz, Ry, T, Ry', Rz', multiplied
		 *  like so:  Rz * Ry * T * Ry' * Rz'.  T is the final rotation
		 *  (which is about the X-axis), and the two composite transforms
		 *  Ry' * Rz' and Rz * Ry are (respectively) the rotations necessary
		 *  from the arbitrary axis to the X-axis then back.  They are
		 *  all elementary rotations.
		 *
		 *  Rz' is a rotation about the Z-axis, to bring the axis vector
		 *  into the x-z plane.  Then Ry' is applied, rotating about the
		 *  Y-axis to bring the axis vector parallel with the X-axis.  The
		 *  rotation about the X-axis is then performed.  Ry and Rz are
		 *  simply the respective inverse transforms to bring the arbitrary
		 *  axis back to it's original orientation.  The first transforms
		 *  Rz' and Ry' are considered inverses, since the data from the
		 *  arbitrary axis gives you info on how to get to it, not how
		 *  to get away from it, and an inverse must be applied.
		 *
		 *  The basic calculation used is to recognize that the arbitrary
		 *  axis vector (x, y, z), since it is of unit length, actually
		 *  represents the sines and cosines of the angles to rotate the
		 *  X-axis to the same orientation, with theta being the angle about
		 *  Z and phi the angle about Y (in the order described above)
		 *  as follows:
		 *
		 *  cos ( theta ) = x / sqrt ( 1 - z^2 )
		 *  sin ( theta ) = y / sqrt ( 1 - z^2 )
		 *
		 *  cos ( phi ) = sqrt ( 1 - z^2 )
		 *  sin ( phi ) = z
		 *
		 *  Note that cos ( phi ) can further be inserted to the above
		 *  formulas:
		 *
		 *  cos ( theta ) = x / cos ( phi )
		 *  sin ( theta ) = y / sin ( phi )
		 *
		 *  ...etc.  Because of those relations and the standard trigonometric
		 *  relations, it is pssible to reduce the transforms down to what
		 *  is used below.  It may be that any primary axis chosen will give the
		 *  same results (modulo a sign convention) using thie method.
		 *
		 *  Particularly nice is to notice that all divisions that might
		 *  have caused trouble when parallel to certain planes or
		 *  axis go away with care paid to reducing the expressions.
		 *  After checking, it does perform correctly under all cases, since
		 *  in all the cases of division where the denominator would have
		 *  been zero, the numerator would have been zero as well, giving
		 *  the expected result.
		 */

		xx = x * x;
		yy = y * y;
		zz = z * z;
		xy = x * y;
		yz = y * z;
		zx = z * x;
		xs = x * s;
		ys = y * s;
		zs = z * s;
		one_c = 1.0F - c;

		/* We already hold the identity-matrix so we can skip some statements */
		M(0,0) = (one_c * xx) + c;
		M(0,1) = (one_c * xy) - zs;
		M(0,2) = (one_c * zx) + ys;
		/*    M(0,3) = 0.0F; */

		M(1,0) = (one_c * xy) + zs;
		M(1,1) = (one_c * yy) + c;
		M(1,2) = (one_c * yz) - xs;
		/*    M(1,3) = 0.0F; */

		M(2,0) = (one_c * zx) - ys;
		M(2,1) = (one_c * yz) + xs;
		M(2,2) = (one_c * zz) + c;
		/*    M(2,3) = 0.0F; */

		/*
		      M(3,0) = 0.0F;
		      M(3,1) = 0.0F;
		      M(3,2) = 0.0F;
		      M(3,3) = 1.0F;
		*/
	}
#undef M

	mesa_mult(transform, transform, m);
}

void mesa_scale(float *transform, float x, float y, float z)
{
	float *m = transform;
	m[0] *= x;
	m[4] *= y;
	m[8]  *= z;
	m[1] *= x;
	m[5] *= y;
	m[9]  *= z;
	m[2] *= x;
	m[6] *= y;
	m[10] *= z;
	m[3] *= x;
	m[7] *= y;
	m[11] *= z;
}

void mesa_translate(float *mat, float x, float y, float z)
{
	float *m = mat;
	m[12] = m[0] * x + m[4] * y + m[8]  * z + m[12];
	m[13] = m[1] * x + m[5] * y + m[9]  * z + m[13];
	m[14] = m[2] * x + m[6] * y + m[10] * z + m[14];
	m[15] = m[3] * x + m[7] * y + m[11] * z + m[15];
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

static MODEL *load_model2(const char *filename)
{
	int sz;
	unsigned char *bytes = slurp_file(filename, &sz);

	ALLEGRO_FILE *file = al_open_memfile(bytes, sz, "rb");
	if (!file)
		return NULL;

	VERT v;
	TEXCOORD t;
	FACE f;

	std::vector<VERT> verts;
	std::vector<TEXCOORD> texcoords;
	std::vector<FACE> faces;

	char line[1000];

	int ntris = 0; // num triangles

	while ((al_fgets(file, line, 1000))) {
		if (line[0] == 'v' && line[1] == ' ') {
			sscanf(line, "v %f %f %f", &v.x, &v.y, &v.z);
			verts.push_back(v);
		}
		else if (line[0] == 'v') {
			sscanf(line, "vt %f %f", &t.u, &t.v);
			t.u *= 512; // FIXME: textures are NOW 512 maybe not later
			t.v *= 512;
#ifdef A5_OGL // FIXME: Is this right for D3D?
			t.v = 512 - t.v;
#endif
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

	MODEL *m = new MODEL;
	m->num_verts = ntris * 3;
	m->verts = new ALLEGRO_VERTEX[m->num_verts];

	int cv = 0; // current vertex

	for (size_t i = 0; i < faces.size(); i++) {
		FACE &face = faces[i];
		ALLEGRO_VERTEX *vert;
		if (face.n == 4) {
			int order[] = { 0, 1, 2, 0, 2, 3 };
			//int order[] = { 2, 1, 0, 3, 2, 0 };
			for (int j = 0; j < 6; j++) {
				vert = &m->verts[cv];
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
				vert = &m->verts[cv];
				cv++;
				vert->x = verts[face.v[j]].x;
				vert->y = verts[face.v[j]].y;
				vert->z = verts[face.v[j]].z;
				vert->u = texcoords[face.t[j]].u;
				vert->v = texcoords[face.t[j]].v;
				vert->color = al_map_rgb_f(1, 1, 1);
			}
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

	m->num_verts = vcount;
	m->verts = new ALLEGRO_VERTEX[vcount];

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
				m->verts[i].x = (float)xyz[j][0];
				m->verts[i].y = (float)xyz[j][1];
				m->verts[i].z = (float)xyz[j][2];
				m->verts[i].color = al_map_rgb(
					rgb[j][0],
					rgb[j][1],
					rgb[j][2]
				    );
				i++;
			}
			if (n > (6*3)) {
				m->verts[i] = m->verts[i-3];
				m->verts[i+1] = m->verts[i-1];
				m->verts[i+2].x = (float)xyz[3][0];
				m->verts[i+2].y = (float)xyz[3][1];
				m->verts[i+2].z = (float)xyz[3][2];
				m->verts[i+2].color = al_map_rgb(
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
				m->verts[i].x = vv[j*3+0];
				m->verts[i].y = vv[j*3+1];
				m->verts[i].z = vv[j*3+2];
				m->verts[i].color = al_map_rgb(
							cv[j*3+0],
							cv[j*3+1],
							cv[j*3+2]
						    );
				i++;
			}
			if (vv.size() > 9) {
				m->verts[i] = m->verts[i-3];
				m->verts[i+1] = m->verts[i-1];
				m->verts[i+2].x = vv[3*3+0];
				m->verts[i+2].y = vv[3*3+1];
				m->verts[i+2].z = vv[3*3+2];
				m->verts[i+2].color = al_map_rgb(
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
				float inc = tex_size / 50;
				float xx = ((i/6) % 50) * inc;
				float yy = (49 - ((i/6) / 50)) * inc;

				m->verts[i+0].u = xx;
				m->verts[i+0].v = yy;
				m->verts[i+1].u = xx+inc;
				m->verts[i+1].v = yy;
				m->verts[i+2].u = xx;
				m->verts[i+2].v = yy+inc;

				m->verts[i+3].u = xx+inc;
				m->verts[i+3].v = yy+inc;
				m->verts[i+4].u = xx+inc;
				m->verts[i+4].v = yy;
				m->verts[i+5].u = xx;
				m->verts[i+5].v = yy+inc;
			}
		}
	}

	al_fclose(f);

	delete[] bytes;

	return m;
}


static void draw_model(MODEL *m)
{
	al_draw_prim(m->verts, 0, 0, 0, m->num_verts, ALLEGRO_PRIM_TRIANGLE_LIST);
}


static void draw_model_tex(MODEL *m, MBITMAP *texture)
{
	m_draw_prim(m->verts, 0, texture, 0, m->num_verts, ALLEGRO_PRIM_TRIANGLE_LIST);
}

static void draw_model(MODEL *m, MBITMAP *texture)
{
	m_set_blender(M_ONE, M_INVERSE_ALPHA, white);
	m_draw_prim(m->verts, 0, texture, 0, m->num_verts, ALLEGRO_PRIM_TRIANGLE_LIST);
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
	mesa_frustum((float *)t.m, xmin, xmax, ymin, ymax, neer, farr);
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

static bool real_archery(int *accuracy_pts)
{
	dpad_off();

	// stop set_sets (astar with mouse)
	getInput()->set(false, false, false, false, false, false, false);

	if (config.getDifficulty() == CFG_DIFFICULTY_EASY) {
		NUM_GOBLINS = 25;
	}
	else {
		NUM_GOBLINS = 50;
	}

#ifdef ALLEGRO_IPHONE
	if (use_dpad || joypad_connected() || airplay_connected || is_sb_connected())
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

	bool really_done = false;
	bool clicked = false;
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
	current_mouse_x = BW/2;
	current_mouse_y = BH/2;
	al_set_mouse_xy(display, current_mouse_x, current_mouse_y);
#endif

	m_set_target_bitmap(buffer);
	m_clear(m_map_rgb(0, 0, 0));

	playMusic("shmup2.ogg");

	std::vector<Goblin> goblins;
	int total_goblins = 0;
	int dead_goblins = 0;
	int new_goblin_count = 2000;

	float target_x = BW/2;
	float target_y = BH/2;

	MODEL *bow_model = load_model2(getResource("models/bow.vtx"));
	MODEL *arrow_model = load_model2(getResource("models/arrow.vtx"));

	int flags = al_get_new_bitmap_flags();
	al_set_new_bitmap_flags(flags | ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);
	MBITMAP *bow_tex = m_load_bitmap(getResource("models/bow.png"));
	MBITMAP *arrow_tex =
		m_load_bitmap(getResource("models/arrow.png"));
	al_set_new_bitmap_flags(flags);

	MBITMAP *grass, *tower;

	grass =	m_load_bitmap(getResource("media/towergrass.png"));
	tower = m_load_bitmap(getResource("media/towertop.png"));

	goblin_animSet = new AnimationSet(getResource("objects/Goblin.png"));
	goblin_animSet->setSubAnimation("walk_s");
	goblin_dead = new AnimationSet(getResource("objects/Goblin.png"));
	goblin_dead->setSubAnimation("dead_arrow");
	goblin_swipe = new AnimationSet(getResource("media/goblin_hand_swipe.png"));

	MBITMAP *progress = m_load_bitmap(getResource("media/Goblin_meter.png"));


	const float max_yrot = M_PI/2;
	const float max_xrot = M_PI/2;
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
	const float aim_speed = 0.16f;
#else
	const float aim_speed = 0.22f;
#endif
	const float arrow_start_z = -250.2;
	const float bow_z = -250;
	float bow_scale = 1;
	float arrow_scale = 1;

	float arrow_z = arrow_start_z;
	bool drawing = true;
	bool drawn = false;
	int hiddenCount = 0;
	bool dead = false;

	playPreloadedSample("bow_draw.ogg");

	while  (1) {
		al_wait_cond(wait_cond, wait_mutex);
		int tmp_counter = logic_counter;
		logic_counter = 0;
		if (tmp_counter > 10)
			tmp_counter = 1;
		while  (tmp_counter > 0) {
			next_input_event_ready = true;

			tmp_counter--;
			if (is_close_pressed()) {
				do_close();
				close_pressed = false;
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
			}

			goblin_animSet->update(LOGIC_MILLIS);

			for (unsigned int j = 0; j < goblins.size(); j++) {
				if (moveGoblin(goblins[j], LOGIC_MILLIS)) {
					dead = true;
					break;
				}
			}

			if (dead) {
				fadeOut(black);
				goto done;
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

			if (!use_dpad) {
				if (!clicked && !released) {
					clicked = true;
				}
			}
			InputDescriptor ie = { 0, };
			if (use_dpad) {
				ie = getInput()->getDescriptor();
			}

			if ((use_dpad && ie.button1 && drawn) || (!use_dpad && clicked && released && drawn)) {
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

			if (!use_dpad) {
				al_lock_mutex(click_mutex);
				target_x = current_mouse_x;
				target_y = current_mouse_y;
				al_unlock_mutex(click_mutex);
			}
			if (use_dpad) {
				if (ie.left) {
					target_x -= aim_speed * LOGIC_MILLIS;
					if (target_x < 0) target_x = 0;
				}
				else if (ie.right) {
					target_x += aim_speed * LOGIC_MILLIS;
					if (target_x >= BW)
						target_x = BW-1;
				}

				if (ie.up) {
					target_y -= aim_speed * LOGIC_MILLIS;
					if (target_y < 0) target_y = 0;
				}
				else if (ie.down) {
					target_y += aim_speed * LOGIC_MILLIS;
					if (target_y >= BH)
						target_y = BH-1;
				}
			}
		}

		if (draw_counter > 0)
			draw_counter = 0;

			disable_zbuffer();
			
			m_set_target_bitmap(buffer);

			float yrot = (target_x / BW - 0.5f) * max_xrot;
			float xrot = (target_y / BH - 0.5f) * max_yrot;

#ifdef A5_OGL
			float arrow_dist = 0.175;
#else
			float arrow_dist = -0.1;
			xrot = -xrot;
			yrot = -yrot;
#endif

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

			drawBufferToScreen();
			al_set_target_backbuffer(display);

			ALLEGRO_TRANSFORM proj_push;
			ALLEGRO_TRANSFORM view_push;
			ALLEGRO_TRANSFORM view_transform;
			al_copy_transform(&view_push, al_get_current_transform());
			al_copy_transform(&proj_push, al_get_projection_transform(display));

			set_projection(1, 1000);

#ifdef A5_D3D
#define OTHER1 mesa_translate((float *)view_transform.m, 0, 0.5, -0.4);
#define OTHER2 mesa_translate((float *)view_transform.m, -0.6, 0.45, 0);
#else
#define OTHER1
#define OTHER2
#endif
#define SETUP_BOW_ES2 \
    al_identity_transform(&view_transform); \
    mesa_scale((float *)view_transform.m, bow_scale, bow_scale, bow_scale); \
    mesa_translate((float *)view_transform.m, 0, 0, bow_z); \
    mesa_rotate((float *)view_transform.m, -R2D(xrot), 1, 0, 0); \
    mesa_rotate((float *)view_transform.m, -R2D(yrot), 0, 1, 0); \
    mesa_rotate((float *)view_transform.m, R2D(M_PI/2), 0, 0, 1); \
    mesa_rotate((float *)view_transform.m, -R2D(M_PI/2), 0, 1, 0); \
    OTHER1 \
    al_use_transform(&view_transform);

#define SETUP_ARROW_ES2 \
    al_identity_transform(&view_transform); \
    mesa_scale((float *)view_transform.m, arrow_scale, arrow_scale, arrow_scale); \
    mesa_translate((float *)view_transform.m, 0, 0, arrow_start_z); \
    mesa_rotate((float *)view_transform.m, -R2D(xrot), 1, 0, 0); \
    mesa_rotate((float *)view_transform.m, -R2D(yrot), 0, 1, 0); \
    mesa_rotate((float *)view_transform.m, -R2D(M_PI/2), 1, 0, 0); \
    mesa_translate((float *)view_transform.m, 0, arrow_start_z-arrow_z, 0); \
    mesa_translate((float *)view_transform.m, arrow_dist, 0, 0); \
    OTHER2 \
    al_use_transform(&view_transform);

	#ifdef A5_D3D
	D3DVIEWPORT9 backup_vp;
	al_get_d3d_device(display)->GetViewport(&backup_vp);
	D3DVIEWPORT9 vp;
	vp.X = 0;
	vp.Y = 0;
	vp.Width = BW;
	vp.Height = BH;
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
	SETUP_BOW_ES2;
	draw_model_tex(bow_model, bow_tex);
	if (!hiddenCount) {
		SETUP_ARROW_ES2;
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

		m_flip_display();

		if (really_done) {
			fadeOut(black);
			break;
		}
	}

done:

	disable_zbuffer();
	
	clear_input_events();

	m_set_target_bitmap(buffer);

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

	return dead;
}


bool archery(bool for_points)
{
	if (for_points) {
		anotherDoDialogue("Faelon: Here comes the horde! Let's have some fun!...\n", true);
	}

	m_set_target_bitmap(buffer);
	m_clear(m_map_rgb(0, 0, 0));

	if (dpad_type != DPAD_TOTAL_1 && dpad_type != DPAD_TOTAL_2)
		notify("Drag your bow into", "position and release", "to fire...");

	while (1) {
		int accuracy_pts;
		bool dead = real_archery(&accuracy_pts);

		m_rest(0.5);

		if (dead) {
			if (prompt("G A M E O V E R", "Try Again?", 1, 1))
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
					if (party[i])
						while(levelUp(party[i], 30+accuracy_pts))
							;
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
	m->num_verts = 6*sd;
	m->verts = new ALLEGRO_VERTEX[m->num_verts];

	float angle_inc = (M_PI*2) / sd;
	float outer_dist = 1.0;
	float inner_dist = 0.75;
	float scale = 4.0;

	for (int i = 0; i < sd; i++) {
		float angle = i * angle_inc;
		float angle2 = angle + angle_inc;
		float inner_angle = ((angle2 - angle) / 2) + angle;
		m->verts[i*6+0].x = sin(angle) * outer_dist * scale;
		m->verts[i*6+0].y = cos(angle) * outer_dist * scale;
		m->verts[i*6+0].z = 1;
		m->verts[i*6+0].u = 0;
		m->verts[i*6+0].v = m_get_bitmap_height(texture)-1;
		m->verts[i*6+0].color = white;
		m->verts[i*6+1].x = sin(angle2) * outer_dist * scale;
		m->verts[i*6+1].y = cos(angle2) * outer_dist * scale;
		m->verts[i*6+1].z = 1;
		m->verts[i*6+1].u = m_get_bitmap_width(texture)-1;
		m->verts[i*6+1].v = m_get_bitmap_height(texture)-1;
		m->verts[i*6+1].color = white;
		m->verts[i*6+2].x = sin(inner_angle) * inner_dist * scale;
		m->verts[i*6+2].y = cos(inner_angle) * inner_dist * scale;
		m->verts[i*6+2].z = 1;
		m->verts[i*6+2].u = m_get_bitmap_width(texture)/2;
		m->verts[i*6+2].v = 0;
		m->verts[i*6+2].color = white;
		float tmp = angle2;
		angle = inner_angle;
		angle2 = inner_angle + angle_inc;
		inner_angle = tmp;
		m->verts[i*6+3].x = sin(angle) * inner_dist * scale;
		m->verts[i*6+3].y = cos(angle) * inner_dist * scale;
		m->verts[i*6+3].z = 1;
		m->verts[i*6+3].u = 0;
		m->verts[i*6+3].v = 0;
		m->verts[i*6+3].color = white;
		m->verts[i*6+4].x = sin(angle2) * inner_dist * scale;
		m->verts[i*6+4].y = cos(angle2) * inner_dist * scale;
		m->verts[i*6+4].z = 1;
		m->verts[i*6+4].u = m_get_bitmap_width(texture)-1;
		m->verts[i*6+4].v = 0;
		m->verts[i*6+4].color = white;
		m->verts[i*6+5].x = sin(inner_angle) * outer_dist * scale;
		m->verts[i*6+5].y = cos(inner_angle) * outer_dist * scale;
		m->verts[i*6+5].z = 1;
		m->verts[i*6+5].u = m_get_bitmap_width(texture)/2;
		m->verts[i*6+5].v = m_get_bitmap_height(texture)-1;
		m->verts[i*6+5].color = white;
	}

	return m;
}

void volcano_scene(void)
{
	dpad_off();

	m_set_target_bitmap(buffer);
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
	float dland_angle = 0.0001f;
	float land_angle = 0.0f;
	const float MAX_LAND_ANGLE = M_PI/16;

	float staff_oy = 0.0f;
	float staff_dy1 = 0.0002f;
	float staff_oz = 0.0f;
	float staff_dz = 0.001f;
	float staff_a = 0;

	MBITMAP *stars, *moon, *ring_texture;

	stars = m_load_bitmap(getResource("media/stars.png"));
	moon = m_load_bitmap(getResource("media/moon.png"));
	ring_texture = m_load_alpha_bitmap(getResource("media/ring_texture.png"));

	MBITMAP *land_texture = m_load_bitmap(getResource("media/volcano_texture.png"));
	MODEL *land_model = load_model(getResource("models/volcano_new.raw"), true, m_get_bitmap_width(land_texture));


	MODEL *staff_model = load_model(getResource("models/staff.raw"));
	MODEL *ring_model = create_ring(32, ring_texture);

	float star_top_y = 0-(m_get_bitmap_height(stars)-BH);
	float star_top_dy1 = 0.02f;
	float star_top_dy2 = 0.1f;

	float ring_z = -100;
	float ring_z_delta = 0.1;


	ALLEGRO_TRANSFORM orig_proj;
	al_copy_transform(&orig_proj, al_get_projection_transform(display));
	ALLEGRO_TRANSFORM proj_transform, view_transform;

	while  (1) {
		al_wait_cond(wait_cond, wait_mutex);
		int tmp_counter = logic_counter;
		logic_counter = 0;
		if (tmp_counter > 10)
			tmp_counter = 1;
		while  (tmp_counter > 0) {
			next_input_event_ready = true;

			tmp_counter--;
			if (is_close_pressed()) {
				do_close();
				close_pressed = false;
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
				if (count > 5000 && fabs(land_angle) < 0.01f) {
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
			else if (stage == STAGE_POOFING) {
				ring_z += ring_z_delta * LOGIC_MILLIS;
				if (count > 5000) {
					goto done;
				}
			}
		}

		if (draw_counter > 0) {
			draw_counter = 0;

			disable_zbuffer();
			m_set_target_bitmap(buffer);

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

			al_set_target_backbuffer(display);
			
			clear_zbuffer();

			ALLEGRO_TRANSFORM proj_push, view_push;
			if (true /*use_programmable_pipeline*/) {
				al_copy_transform(&proj_push, al_get_projection_transform(display));
				al_copy_transform(&view_push, al_get_current_transform());
			}
			else {
				glMatrixMode(GL_MODELVIEW);
				glPushMatrix();
				glMatrixMode(GL_PROJECTION);
				glPushMatrix();

			}

			al_identity_transform(&proj_transform);
			mesa_frustum((float *)proj_transform.m, -1, 1, (float)BH/BW, -(float)BH/BW, 1, 1000);
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


			if (true) {
				al_identity_transform(&view_transform);
				mesa_scale((float *)view_transform.m, 50, 50, 50);

				mesa_translate((float *)view_transform.m, 0.01, 0.1+staff_oy, -0.3);
				mesa_rotate((float *)view_transform.m, R2D(land_angle), 0, 1, 0);
				mesa_rotate((float *)view_transform.m, 90, 1, 0, 0);
				al_use_transform(&view_transform);
			}

			draw_model(land_model, land_texture);
			clear_zbuffer();

			if (true) {

				mesa_translate((float *)view_transform.m, 0, 0.12-staff_oz, 0.05+staff_oy);
				mesa_rotate((float *)view_transform.m, R2D(staff_a), 1, 0, 0);
				mesa_scale((float *)view_transform.m, 1.0/256, 1.0/256, 1.0/256);
				al_use_transform(&view_transform);
			}
			if (stage != STAGE_POOFING)
				draw_model(staff_model);
			else {
				if (true) {
					al_identity_transform(&view_transform);
					mesa_translate((float *)view_transform.m, 0.5, 0.5, ring_z);
					al_use_transform(&view_transform);
					disable_cull_face();
					draw_model(ring_model, ring_texture);
					enable_cull_face(true);
				}
			}

			disable_cull_face();
			disable_zbuffer();

			if (true) {
				al_set_projection_transform(display, &proj_push);
				al_use_transform(&view_push);
			}

#ifdef A5_D3D
			device->SetViewport(&old);
#endif
			
			m_flip_display();
		}
	}
done:

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

	if (true /*use_programmable_pipeline*/)
	{
		al_set_projection_transform(display, &orig_proj);
	}

	dpad_on();

	clear_input_events();
}
