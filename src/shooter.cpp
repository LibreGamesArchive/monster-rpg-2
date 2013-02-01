#include "monster2.hpp"
#define ASSERT ALLEGRO_ASSERT
#ifdef ALLEGRO_IPHONE
#include <allegro5/allegro_iphone.h>
#endif
#include <allegro5/internal/aintern_opengl.h>

#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID || defined ALLEGRO_RASPBERRYPI
#define glFrustum glFrustumf
#endif

bool in_shooter = false;
bool break_shooter_pause = false;
bool shooter_paused = false;

static void vecXmat(double x, double y, double z, ALLEGRO_TRANSFORM *mat, double *ox, double *oy, double *oz, double *ow)
{
	*ox = x*mat->m[0][0] + y*mat->m[1][0] + z*mat->m[2][0] + mat->m[3][0];
	*oy = x*mat->m[0][1] + y*mat->m[1][1] + z*mat->m[2][1] + mat->m[3][1];
	*oz = x*mat->m[0][2] + y*mat->m[1][2] + z*mat->m[2][2] + mat->m[3][2];
	*ow = x*mat->m[0][3] + y*mat->m[1][3] + z*mat->m[2][3] + mat->m[3][3];
}

static void draw_billboard_bitmap(ALLEGRO_TRANSFORM *proj, MBITMAP *b, int x, int y)
{
	double ox, oy, oz, ow;

	vecXmat(x, y, 0, proj, &ox, &oy, &oz, &ow);

	ox /= ow;
	oy /= ow;
	oz /= ow;

	ox += 1;
	ox *= BW/2;
	oy = -oy;
	oy += 1;
	oy *= BH/2;

	int dw = m_get_bitmap_width(b)*1.5*(1.0-oz);
	int dh = m_get_bitmap_height(b)*1.5*(1.0-oz);
	m_draw_scaled_bitmap(
		b, 0, 0, m_get_bitmap_width(b), m_get_bitmap_height(b),
		ox-dw/2, oy-dh,
		dw, dh,
		0);
}

class MShooterSlider : public TGUIWidget {
public:
	static const int RADIUS = 8;
	static const int WIDTH = BW/3;
	static const int X = BW/4;
	static const int Y = BH-5-RADIUS*2;
	void draw(void) {
		int xx = X + (WIDTH/2) * value;
		m_draw_bitmap(slider, X-WIDTH/2, Y, 0);
		m_draw_bitmap(tab, xx-RADIUS, Y, 0);
	}
	bool acceptsFocus(void) { return true; }
	bool on(int x, int y) {
		if (x >= X-WIDTH/2 && y >= Y && x < X+WIDTH/2 &&
				Y < Y+RADIUS*2) {
			return true;
		}
		return false;
	}

	void moveSlider(int x, int y)
	{
		x -= (X-WIDTH/2);
		value = (x-WIDTH/2)/(double)(WIDTH/2);
	}
	
	int update(int millis) {
		monitor->update();

		size_t i;

		for (i = 0; i < monitor->touches.size(); i++) {
			if (on(monitor->touches[i].x, monitor->touches[i].y)) {
				moveSlider(monitor->touches[i].x, monitor->touches[i].y);
				break;
			}
		}

		if (i >= monitor->touches.size()) {
			value = 0;
		}

		return TGUI_CONTINUE;
	}

	double getValue(void) { return value; }

	MShooterSlider(void) {
		this->hotkeys = 0;
		this->x = X-WIDTH/2;
		this->y = Y;
		this->width = WIDTH;
		this->height = RADIUS*2;
		value = 0;
		slider = m_load_bitmap(getResource("media/shooter/slider.png"));
		tab = m_load_bitmap(getResource("media/shooter/tab.png"));
		
		monitor = new MouseMonitor();
	}
	virtual ~MShooterSlider(void) {
		m_destroy_bitmap(slider);
		m_destroy_bitmap(tab);
		delete monitor;
	}
protected:
	double value;
	TGUIWidget *otherWidget;
	MBITMAP *slider;
	MBITMAP *tab;
	MouseMonitor *monitor;
};



class MShooterButton : public TGUIWidget {
public:
	static const int RADIUS = 20;
	static const int X = BW-5-RADIUS*2;
	static const int Y = BH-5-RADIUS*2;
	void draw(void) {
		if (getPressed()) {
			m_draw_bitmap(buttondown, X, Y, 0);
		}
		else {
			m_draw_bitmap(buttonup, X, Y, 0);
		}
	}
	int update(int millis) {
		monitor->update();
		return TGUI_CONTINUE;
	}
	bool acceptsFocus(void) { return true; }
	bool on(int x, int y) {
		double dx = x - (X+RADIUS);
		double dy = y - (Y+RADIUS);
		double dist = sqrtf(dx*dx + dy*dy);
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

	MShooterButton(void) {
		this->hotkeys = 0;
		this->x = X;
		this->y = Y;
		this->width = RADIUS*2;
		this->height = RADIUS*2;
		buttonup = m_load_bitmap(getResource("media/shooter/buttonup.png"));
		buttondown = m_load_bitmap(getResource("media/shooter/buttondown.png"));
		monitor = new MouseMonitor();
	}
	virtual ~MShooterButton(void) {
		m_destroy_bitmap(buttonup);
		m_destroy_bitmap(buttondown);
		delete monitor;
	}
protected:
	MBITMAP *buttonup;
	MBITMAP *buttondown;
	MouseMonitor *monitor;
};

static const int EXP_POOL_SIZE = 10;
static AnimationSet *explosion_pool[EXP_POOL_SIZE];
static int exp_pool_index =  0;


class Explosion {
public:
	Explosion(int ox, int oy) {
		this->ox = ox;
		this->oy = oy;
		animSet = explosion_pool[exp_pool_index++];
		exp_pool_index %= EXP_POOL_SIZE;
		animSet->reset();
	}

	void draw(ALLEGRO_TRANSFORM *proj, int cx, int cy) {
		MBITMAP *b = animSet->getCurrentAnimation()->getCurrentFrame()->getImage()->getBitmap();
		draw_billboard_bitmap(
			proj,
			b,
			ox-cx,
			oy-cy+m_get_bitmap_height(b)
		);
	}

	bool update(int step) {
		animSet->update(step);
		if ((int)animSet->getFrame() == (int)animSet->getCurrentAnimation()->getNumFrames()-1)
			return false;
		return true;
	}

private:
	int ox, oy;
	AnimationSet *animSet;
};






static const int NUM_TILE_TYPES = 12;

static const int w = (BW*8)/TILE_SIZE;
static int h;
// these two must be divisible by 3!!!!!!!!!!!!!!!!!
static const int easy_h = (BH/TILE_SIZE)*210;
static const int default_h = (BH/TILE_SIZE)*420;
static const int valleyw = (BW/TILE_SIZE);
static int numsharks;
static const int easy_num_sharks = 10;
static const int default_num_sharks = 20;

static double x;
static double o;
static double py;
static bool dead;
bool shooter_restoring = false;
static AnimationSet *crab;
static AnimationSet *shark_anim;
static MBITMAP *sub_bmp;
static MBITMAP *bullet;
static MBITMAP *crab_bmp;
static MBITMAP *shark_bmp;

static char *scene;
static char *solid;
static int *starts;

static MBITMAP *underwater;

static double tu[NUM_TILE_TYPES];
static double tv[NUM_TILE_TYPES];

struct Bullet : public MPoint {
	double fireTime;
};

struct Shark : public MPoint {
	int distFromSide;
};


static std::vector<MPoint> crabs;
static std::vector<Shark> sharks;
static std::vector<MPoint> crabs_start;
static std::vector<Shark> sharks_start;
static std::vector<Explosion *> explosions;
static std::vector<Bullet> bullets;

inline void set_row(int y, int startx, int dir)
{
	if (y >= h) return;
	
	starts[y] = startx;
	
	memset(solid+y*w, 1, startx);
	memset(solid+y*w+startx, 0, valleyw);
	memset(solid+y*w+startx+valleyw, 1, w-(startx+valleyw));
	
	memset(scene+y*w, 10, startx);
	memset(scene+y*w+startx, 9, valleyw);
	memset(scene+y*w+startx+valleyw, 10, w-(startx+valleyw));
	
	scene[y*w+startx+valleyw] = 11;

	switch (dir) {
		case -1:
			scene[y*w+startx-2] = 4;
			scene[y*w+startx-1] = 5;
			break;
		case 0:
			scene[y*w+startx-1] = 8;
			break;
		case 1:
			scene[y*w+startx-2] = 0;
			scene[y*w+startx-1] = 1;
			break;
	}

	// add crabs
	if (y < h-50) {
		int prob = (double)y/h * 15 + 15;
		if (rand() % prob == 0) {
			int x = startx + rand() % valleyw;
			MPoint p;
			p.x = x*TILE_SIZE;
			p.y = y*TILE_SIZE;
			crabs_start.push_back(p);	
		}
	}
}


static void generate(void)
{
	int startx = w/2-(BW/TILE_SIZE/2);
	const int minx = startx-(BW/TILE_SIZE);
	const int maxx = startx+(BW/TILE_SIZE);
	
	scene = new char[w*h];
	solid = new char[w*h];
	starts = new int[h];
	
	int y = 0;
	
#define NEXT() ((rand() % 2) ? -1 : 1)
	
	while (y < h) {
		int next;
		if (startx <= minx) next = 1;
		else if (startx >= maxx) next = -1;
		else next = NEXT();
		if (next == -1) {
			set_row(y++, startx, 0);
			set_row(y++, startx, -1);
			startx += next;
			set_row(y++, startx, 0);
		}
		else {
			set_row(y++, startx, 0);
			startx += next;
			set_row(y++, startx, next);
			set_row(y++, startx, 0);
		}
	}
	
#undef NEXT
	
	// Fill in right side
	for (y = 1; y < h-1; y++) {
		int a = starts[y-1];
		int b = starts[y];
		int c = starts[y+1];
		
		if (b > a) {
			// right a out
			int yy = y-1;
			scene[yy*w+a+valleyw] = 6;
			scene[yy*w+a+valleyw+1] = 7;
		}
		else if (b > c) {
			// right c in
			int yy = y+1;
			scene[yy*w+c+valleyw] = 2;
			scene[yy*w+c+valleyw+1] = 3;
		}
	}

	// Add sharks
	for (int i = 0; i < numsharks; i++) {
		Shark s;
		
		int ty = (int)(((rand()%RAND_MAX)/(double)RAND_MAX)*h);
		if (ty <= 0)
			ty = 1;
		else if (ty >= h-1)
			ty = h-2;
		s.distFromSide = rand() % valleyw;
		s.y = ty*TILE_SIZE;
		int startx = starts[ty];
		s.x = (startx+s.distFromSide)*TILE_SIZE;
		sharks_start.push_back(s);
	}
}

static MBITMAP *bbot;
static MBITMAP *btop;
static int rendered;
static int start_render;
static int end_render;
static MBITMAP *curr_bmp;
static double start_cy;
bool do_swap;
int old_yoffset;

static double get_xofs(double cx)
{
	double abs_center = (w*TILE_SIZE)/2;
	double player_ofs = cx - abs_center;
	return -512-player_ofs;
}

static void render(int start, int end, MBITMAP *bmp)
{
	if (start >= end)
		return;

	ALLEGRO_VERTEX *verts = new ALLEGRO_VERTEX[64*(end-start)*6];
	int vcount = 0;

	const int xofs = (w/2)-32;

	ALLEGRO_BITMAP *old_target = al_get_target_bitmap();
	m_set_target_bitmap(bmp);

	for (int y = end; y > start; y--) {
		int tiley = 63 + (y-end-rendered);
		int pixy = tiley * 16;
		for (int x = 0; x < 64; x++) {
			int tilex = xofs + x;
			if (tilex < 0 || tilex >= w)
				continue;
			int tile = scene[y*w+tilex];
			int xx = x * 16;
			verts[vcount].x = xx;
			verts[vcount].y = pixy;
			verts[vcount].z = 0;
			verts[vcount].u = tu[tile];
			verts[vcount].v = tv[tile];
			verts[vcount].color = white;
			vcount++;
			verts[vcount].x = xx + 16;
			verts[vcount].y = pixy;
			verts[vcount].z = 0;
			verts[vcount].u = tu[tile] + 16;
			verts[vcount].v = tv[tile];
			verts[vcount].color = white;
			vcount++;
			verts[vcount].x = xx;
			verts[vcount].y = pixy + 16;
			verts[vcount].z = 0;
			verts[vcount].u = tu[tile];
			verts[vcount].v = tv[tile] + 16;
			verts[vcount].color = white;
			vcount++;
			verts[vcount].x = xx + 16;
			verts[vcount].y = pixy;
			verts[vcount].z = 0;
			verts[vcount].u = tu[tile] + 16;
			verts[vcount].v = tv[tile];
			verts[vcount].color = white;
			vcount++;
			verts[vcount].x = xx;
			verts[vcount].y = pixy + 16;
			verts[vcount].z = 0;
			verts[vcount].u = tu[tile];
			verts[vcount].v = tv[tile] + 16;
			verts[vcount].color = white;
			vcount++;
			verts[vcount].x = xx + 16;
			verts[vcount].y = pixy + 16;
			verts[vcount].z = 0;
			verts[vcount].u = tu[tile] + 16;
			verts[vcount].v = tv[tile] + 16;
			verts[vcount].color = white;
			vcount++;
		}
	}

	m_draw_prim(verts, 0, underwater, 0, vcount, ALLEGRO_PRIM_TRIANGLE_LIST);
	al_set_target_bitmap(old_target);
	
	end_render = start;

	delete[] verts;
}

static void render(void)
{
	int start = start_render;
	int end = end_render;
	if (start < end) {
		int n = end - start;
		int xtra = (rendered+n) - 64;
		if (xtra > 0) {
			n -= xtra;
		}
		render(end-n, end, curr_bmp);
		rendered += n;
		if (rendered == 64) {
			rendered = 0;
			if (curr_bmp == bbot)	
				curr_bmp = btop;
			else
				curr_bmp = bbot;
			do_swap = true;
		}
	}
}

static void draw(double cx, double cy, bool draw_objects = true)
{
	render();

	int yoffset = (int)(start_cy - cy) % 1024;

	if ((old_yoffset != -1) && (yoffset < old_yoffset) && do_swap) {
		MBITMAP *tmp = bbot;
		bbot = btop;
		btop = tmp;
		do_swap = false;
	}

	old_yoffset = yoffset;
	
	double x = get_xofs(cx);
	double y = -2048+yoffset;
	
	ALLEGRO_DISPLAY *display = al_get_current_display();
	ALLEGRO_TRANSFORM proj_push;
	
	ALLEGRO_TRANSFORM proj;
	if (true /*use_programmable_pipeline*/) {
		al_copy_transform(&proj_push, al_get_projection_transform(display));

		al_identity_transform(&proj);

		mesa_frustum((float *)proj.m, -BW/2, BW/2, BH/2, -BH/2, 1, 1000);
		mesa_scale((float *)proj.m, 4, 2, 1);
		mesa_translate((float *)proj.m, 0, 137, -1);
		mesa_rotate((float *)proj.m, 1, 1, 0, 0);

		al_set_projection_transform(display, &proj);
	}
	else {
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glFrustum(-BW/2, BW/2, BH/2, -BH/2, 1, 1000);
		glScalef(4, 2, 1);
		glTranslatef(0, 137, -1);
		glRotatef(1, 1, 0, 0);
	}

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

	disable_cull_face();
	disable_zbuffer();

	m_draw_bitmap(btop, x, y, 0);
	m_draw_bitmap(bbot, x, y+1024, 0);

	if (true /*use_programmable_pipeline*/) {
		al_set_projection_transform(display, &proj_push);
	}
	else {
		glPopMatrix();
	}
	
	#ifdef A5_D3D
	al_get_d3d_device(display)->SetViewport(&backup_vp);
	#endif

	if (!draw_objects) return;

	// draw player
	if (!dead) {
		draw_billboard_bitmap(
			&proj,
			sub_bmp,
			0,
			py-cy
		);
	}

	// draw explosions
	for (int i = 0; i < (int)explosions.size(); i++) {
		explosions[i]->draw(&proj, cx, cy);
	}
	// draw crabs
	crab_bmp = crab->getCurrentAnimation()->getCurrentFrame()->getImage()->getBitmap();
	for (int i = 0; i < (int)crabs.size(); i++) {
		bool held = al_is_bitmap_drawing_held();
		al_hold_bitmap_drawing(true);
		if (crabs[i].y > o-600 && crabs[i].y < o+64) {
			draw_billboard_bitmap(
				&proj,
				crab_bmp,
				crabs[i].x-cx+m_get_bitmap_width(crab_bmp)/2,
				crabs[i].y-cy+m_get_bitmap_height(crab_bmp)
			);
		}
		al_hold_bitmap_drawing(held);
	}
	// draw bullets
	for (int i = 0; i < (int)bullets.size(); i++) {
		bool held = al_is_bitmap_drawing_held();
		al_hold_bitmap_drawing(true);
		draw_billboard_bitmap(
			&proj,
			bullet,
			bullets[i].x-cx,
			bullets[i].y-cy+m_get_bitmap_height(bullet)
		);
		al_hold_bitmap_drawing(held);
	}
	// draw sharks
	shark_bmp = shark_anim->getCurrentAnimation()->getCurrentFrame()->getImage()->getBitmap();
	for (int i = 0; i < (int)sharks.size(); i++) {
		bool held = al_is_bitmap_drawing_held();
		al_hold_bitmap_drawing(true);
		if (sharks[i].y > o-600 && sharks[i].y < o+64) {
			draw_billboard_bitmap(
				&proj,
				shark_bmp,
				sharks[i].x-cx+m_get_bitmap_width(shark_bmp)/2,
				sharks[i].y-cy+m_get_bitmap_height(shark_bmp)
			);
		}
		al_hold_bitmap_drawing(held);
	}
}

static void draw_all(void)
{
	rendered = 0;
	int endy = o / TILE_SIZE;
	int starty2 = endy - 64;
	int starty1 = starty2 - 64;
	render(starty2, endy, bbot);
	render(starty1, starty2, btop);
	start_render = starty1-1;
	end_render = starty1-1;
	start_cy = o;
	curr_bmp = bbot;
	do_swap = false;
	old_yoffset = -1;
}

bool shooter(bool for_points)
{
	in_shooter = true;

	// stop set_sets (astar with mouse)
	getInput()->set(false, false, false, false, false, false, false);

	int shark_value, crab_value;

	int flags = al_get_new_bitmap_flags();
	al_set_new_bitmap_flags(flags | ALLEGRO_NO_PRESERVE_TEXTURE);
	bbot = m_create_bitmap(1024, 1024); // check
	btop = m_create_bitmap(1024, 1024); // check
	al_set_new_bitmap_flags(flags);

	if (config.getDifficulty() == CFG_DIFFICULTY_EASY) {
		h = easy_h;
		numsharks = easy_num_sharks;
		shark_value = 20;
		crab_value = 200;
	}
	else {
		h = default_h;
		numsharks = default_num_sharks;
		shark_value = 10;
		crab_value = 100;
	}

	bool can_pause = true;

	dpad_off();

	int crabs_destroyed;
	int sharks_destroyed;

	underwater = m_load_bitmap(getResource("media/underwater.png"));

	sub_bmp = m_load_bitmap(getResource("media/sub.png"));
	bullet = m_load_bitmap(getResource("media/bullet.png"));
	for (int i = 0; i < EXP_POOL_SIZE; i++) {
		explosion_pool[i] = new AnimationSet(getResource("media/explosion.png"));
	}

	crab = new AnimationSet(getResource("media/crab.png"));
	shark_anim = new AnimationSet(getResource("media/shark.png"));

	crab_bmp = crab->getCurrentAnimation()->getCurrentFrame()->getImage()->getBitmap();
	shark_bmp = shark_anim->getCurrentAnimation()->getCurrentFrame()->getImage()->getBitmap();

	MBITMAP *shark_icon = m_load_bitmap(getResource("media/shark_icon.png"));
	MBITMAP *crab_icon = m_load_bitmap(getResource("media/crab_icon.png"));
	
	MBITMAP *pause_icon = m_load_alpha_bitmap(getResource("media/sub_pause.png"));
	MBITMAP *sub_icon = m_load_bitmap(getResource("media/shooter/sub_small.png"));

	const int pause_icon_w = m_get_bitmap_width(pause_icon);
	const int pause_icon_h = m_get_bitmap_height(pause_icon);

	const int pause_pos_x = BW-pause_icon_w/2-4;
	const int pause_pos_y = 3+pause_icon_h/2;

#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
	std::list<ZONE>::iterator pause_zone = define_zone(
		pause_pos_x-pause_icon_w/2,
		pause_pos_y-pause_icon_h/2,
		pause_pos_x+pause_icon_w/2,
		pause_pos_y+pause_icon_h/2);
#endif

#define COPY(i, x, y) \
	tu[i] = x*TILE_SIZE; \
	tv[i] = y*TILE_SIZE;

	COPY(0, 0, 0)
	COPY(1, 2, 0)
	COPY(2, 3, 0)
	COPY(3, 5, 0)
	COPY(4, 0, 1)
	COPY(5, 2, 1)
	COPY(6, 3, 1)
	COPY(7, 5, 1)
	COPY(8, 0, 2)
	COPY(9, 2, 2)
	COPY(10, 3, 2)
	COPY(11, 5, 2)

#undef COPY

	int seed = rand();

	int deadCount;
	double lastFire;

	playMusic("underwater.ogg");
	srand(7);
	generate();
	
	anotherDoDialogue("Gunnar: Oh, no, the throttle jammed! Eny, you steer.\nMel, Rider, blast anything in our path! I'll go fix the engine!\n", true);

start:
	bool replay = false;

	playMusic("underwater.ogg");

	MShooterButton *button = NULL;
	MShooterSlider *slider = NULL;
	if (!use_dpad) {
		button = new MShooterButton();
		slider = new MShooterSlider();
		tguiSetParent(0);
		tguiAddWidget(button);
		tguiAddWidget(slider);
	}

	crabs = crabs_start;
	sharks = sharks_start;

	int starty = 176;
	int startx = 0;

	o = (h*TILE_SIZE)-starty;
	py = o - 64;

	for (int i = 0; i < w; i++) {
		if (!solid[i+(int(py/TILE_SIZE))*w]) {
			startx = i;
			break;
		}
	}
	
	int counter = 0;
	int frames = 0;
	bool fps_on = false;
	int fps;
	
	
	crabs_destroyed = 0;
	sharks_destroyed = 0;

	x = (startx+valleyw/2.0)*TILE_SIZE;

	dead = false;
	deadCount = 0;
	lastFire = al_get_time();
	
	draw_all();

	for (; o >= TILE_SIZE*140;) {
		if (is_close_pressed()) {
			do_close();
			close_pressed = false;
		}
		// WARNING
		if (break_main_loop) {
			goto done;
		}

		al_wait_cond(wait_cond, wait_mutex);

		// Logic
		int tmp_counter = logic_counter;
		logic_counter = 0;
		while  (tmp_counter > 0) {
			next_input_event_ready = true;

			counter += LOGIC_MILLIS;
			if (fps_on && counter > 2000) {
				fps = (int)((double)frames/((double)counter/1000.0));
				counter = frames = 0;
			}
			
			tmp_counter--;

			runtime_ms += LOGIC_MILLIS;
			while (runtime_ms >= 1000) {
				runtime++;
				runtime_ms -= 1000;
			}

			if (!dead) {
				o -= LOGIC_MILLIS * 0.3;
			}

			py = o - 64;
	
			start_render = (o - 2048) / TILE_SIZE;

			crab->update(LOGIC_MILLIS);
			shark_anim->update(LOGIC_MILLIS);

			Input *input = getInput();
			InputDescriptor ie = input->getDescriptor();

			int tx = (int)(x/TILE_SIZE);
			int ty = (int)(py/TILE_SIZE);

			if (!dead && solid[tx+ty*w]) {
				dead = true;
				Explosion *e = new Explosion(x, py-m_get_bitmap_height(sub_bmp));
				explosions.push_back(e);
				playPreloadedSample("explosion.ogg");
			}
			else if (!dead) {
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
				if (use_dpad) {
					if (ie.left) {
						x -= LOGIC_MILLIS * 0.2;
					}
					if (ie.right) {
						x += LOGIC_MILLIS * 0.2;
					}
				}
				else {
					x += LOGIC_MILLIS * slider->getValue()/3;
				}
#else
				if (ie.left) {
					x -= LOGIC_MILLIS * 0.2;
				}
				if (ie.right) {
					x += LOGIC_MILLIS * 0.2;
				}
#endif
			}

			// Check collisions with crabs
			std::vector<MPoint>::iterator cit;
			for (cit = crabs.begin(); cit != crabs.end();) {
				MPoint crab = *cit;
				bool collision = false;
				double dx = x - (crab.x+m_get_bitmap_width(crab_bmp)/2);
				double dy = py - crab.y;
				double dist = sqrt(dx*dx+dy*dy);
				if (dist < 12) {
					dead = true;
					playPreloadedSample("explosion.ogg");
					Explosion *e = new Explosion(x, py-m_get_bitmap_height(sub_bmp));
					explosions.push_back(e);
					e = new Explosion(crab.x+m_get_bitmap_width(crab_bmp)/2, crab.y);
					explosions.push_back(e);
					cit = crabs.erase(cit);
					collision = true;
				}
				// Check collisions with bullets
				std::vector<Bullet>::iterator bit;
				for (bit = bullets.begin(); bit != bullets.end();) {
					Bullet bull = *bit;
					double dx = (crab.x+m_get_bitmap_width(crab_bmp)/2) - (bull.x+m_get_bitmap_width(bullet)/2);
					double dy = crab.y - bull.y;
					double dist = sqrt(dx*dx+dy*dy);
					if (dist < 10) {
						playPreloadedSample("explosion.ogg");
						Explosion *e = new Explosion(crab.x+m_get_bitmap_width(crab_bmp)/2, crab.y);
						explosions.push_back(e);
						bit = bullets.erase(bit);
						cit = crabs.erase(cit);
						collision = true;
						crabs_destroyed++;
						break;
					}
					else {
						bit++;
					}
				}

				if (!collision) {
					cit++;
				}
			}

			// Check collisions with sharks
			std::vector<Shark>::iterator sit;
			for (sit = sharks.begin(); sit != sharks.end();) {
				Shark shark = *sit;
				bool collision = false;
				double dx = x - (shark.x+m_get_bitmap_width(shark_bmp)/2);
				double dy = py - shark.y;
				double dist = sqrt(dx*dx+dy*dy);
				if (dist < 20) {
					dead = true;
					playPreloadedSample("explosion.ogg");
					Explosion *e = new Explosion(x, py-m_get_bitmap_height(sub_bmp));
					explosions.push_back(e);
					e = new Explosion(shark.x+m_get_bitmap_width(shark_bmp)/2, shark.y);
					explosions.push_back(e);
					sit = sharks.erase(sit);
					collision = true;
				}
				// Check collisions with bullets
				std::vector<Bullet>::iterator bit;
				for (bit = bullets.begin(); bit != bullets.end();) {
					Bullet bull = *bit;
					double dx = (shark.x+m_get_bitmap_width(shark_bmp)/2) - (bull.x+m_get_bitmap_width(bullet)/2);
					double dy = shark.y - bull.y;
					double dist = sqrt(dx*dx+dy*dy);
					if (dist < 18) {
						playPreloadedSample("explosion.ogg");
						Explosion *e = new Explosion(shark.x+m_get_bitmap_width(shark_bmp)/2, shark.y);
						explosions.push_back(e);
						bit = bullets.erase(bit);
						sit = sharks.erase(sit);
						collision = true;
						sharks_destroyed++;
						break;
					}
					else {
						bit++;
					}
				}

				if (!collision) {
					sit++;
				}
			}


			if (dead) {
				deadCount += LOGIC_MILLIS;
				if (deadCount > 2000)
					goto done;
			}

			bool pressed = false;
			if (!use_dpad) {
				tguiUpdate();
				pressed = button->getPressed();
			}

			ALLEGRO_MOUSE_STATE state;
			if (al_is_mouse_installed()) {
				al_get_mouse_state(&state);
			}
			else {
				state.buttons = 0;
			}
			InputDescriptor in = getInput()->getDescriptor();
			if ((state.buttons && can_pause) || in.button2) {
				int press_x = state.x;
				int press_y = state.y;
				if (config.getMaintainAspectRatio() == ASPECT_FILL_SCREEN)
					tguiConvertMousePosition(&press_x, &press_y, 0, 0, screen_ratio_x, screen_ratio_y);
				else
					tguiConvertMousePosition(&press_x, &press_y, screen_offset_x, screen_offset_y, 1, 1);
				int dx = pause_pos_x - press_x;
				int dy = pause_pos_y - press_y;
				double dist = sqrt((float)dx*dx + dy*dy);
				if (dist < pause_icon_w/2 || in.button2) {
					while (in.button2) {
						in = getInput()->getDescriptor();
					}
					// pause
					can_pause = false;
					const char *pause_text = "Paused";
					int tw = m_text_length(game_font, _t(pause_text));
					int th = m_text_height(game_font);
					m_set_target_bitmap(buffer);
					m_draw_rectangle(BW/2-tw/2-5, BH/2-th/2-5, BW/2+tw/2+5, BH/2+th/2+5, black, M_FILLED);
					m_draw_rectangle(BW/2-tw/2-5+0.5, BH/2-th/2-5+0.5, BW/2+tw/2+5, BH/2+th/2+5, white, M_OUTLINED);
					mTextout_simple(_t(pause_text), BW/2-tw/2, BH/2-th/2, white);
					drawBufferToScreen();
					m_flip_display();

					al_stop_timer(logic_timer);

					shooter_paused = true;
					
					while (true) {
						in = getInput()->getDescriptor();
						next_input_event_ready = true;

						if (is_close_pressed()) {
							do_close();
							close_pressed = false;
						}
						// WARNING
						if (break_main_loop) {
							goto done;
						}
						if (break_shooter_pause) {
							can_pause = true;
							break;
						}
						if (al_is_mouse_installed()) {
							al_get_mouse_state(&state);
						}
						else {
							state.buttons = 0;
						}
						if ((state.buttons && can_pause) || in.button2) {
							press_x = state.x;
							press_y = state.y;
							if (is_ipad() && !config.getMaintainAspectRatio())
								tguiConvertMousePosition(&press_x, &press_y, 0, 0, screen_ratio_x, screen_ratio_y);
							else
								tguiConvertMousePosition(&press_x, &press_y, screen_offset_x, screen_offset_y, 1, 1);
							int dx = pause_pos_x - press_x;
							int dy = pause_pos_y - press_y;
							double dist = sqrt((float)dx*dx + dy*dy);
							if (dist < w/2 || in.button2) {
								while (in.button2) {
									in = getInput()->getDescriptor();
								}
								can_pause = false;
								break;
							}
						}
						else if (!state.buttons) {
							can_pause = true;
						}
						drawBufferToScreen();
						m_flip_display();
						m_rest(0.005);
					}

					al_start_timer(logic_timer);

					shooter_paused = false;
				}
			}
			else if (!state.buttons) {
				can_pause = true;
			}

			if (!dead && (ie.button1 || pressed) && (al_get_time()-lastFire > 0.2)) {
				playPreloadedSample("torpedo.ogg");
				lastFire = al_get_time();
				Bullet b;
				b.fireTime = lastFire;
				b.x = x;
				b.y = py-m_get_bitmap_height(sub_bmp);
				bullets.push_back(b);
			}
			

			std::vector<Explosion *>::iterator it;
			for (it = explosions.begin(); it != explosions.end();) {
				Explosion *e = *it;
				if (e->update(LOGIC_MILLIS) == false) {
					delete e;
					it = explosions.erase(it);
				}
				else {
					it++;
				}
			}

			// Move bullets
			std::vector<Bullet>::iterator bit;
			for (bit = bullets.begin(); bit != bullets.end();) {
				Bullet &b = *bit;
				double now = al_get_time();
				if ((now - b.fireTime) > 0.75) {
					bit = bullets.erase(bit);
				}
				else {
					b.y -= 0.6 * LOGIC_MILLIS;
					int tx = b.x / TILE_SIZE;
					int ty = b.y / TILE_SIZE;
					if (solid[tx+ty*w]) {
						bit = bullets.erase(bit);
					}
					else {
						bit++;
					}
				}
			}

			// Move sharks
			for (sit = sharks.begin(); sit != sharks.end();) {
				Shark &s = *sit;
				int dy = o - s.y;
				if (dy < BH*5) {
					// Shark is moving
					s.y += 0.2 * LOGIC_MILLIS;
					int ty = s.y / TILE_SIZE;
					int startx;
					startx = starts[ty];
					double target_x = (startx + s.distFromSide) * TILE_SIZE;
					if (abs(target_x-s.x) > 1) {
						if (s.x < target_x) {
							s.x += 0.2 * LOGIC_MILLIS;
						}
						else if (s.x > target_x) {
							s.x -= 0.2 * LOGIC_MILLIS;
						}
					}
				}
				if (dy < 0) {
					sit = sharks.erase(sit);
				}
				else {
					sit++;
				}
			}
		}

		if (draw_counter > 0) {
			draw_counter = 0;

			m_set_target_bitmap(buffer);
			m_clear(m_map_rgb(105, 115, 145));
	
			if (shooter_restoring) {
				shooter_restoring = false;
				replay = true;
				goto done;
			}
			else {
				draw(x, o);
			}

			if (!use_dpad)
				tguiDraw();
		
			m_save_blender();
			m_set_blender(M_ONE, M_INVERSE_ALPHA, white);
			m_draw_bitmap(crab_icon, 0, 0, 0);
			m_draw_bitmap(shark_icon, 0, 16, 0);
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
			m_draw_alpha_bitmap(pause_icon, BW-m_get_bitmap_width(pause_icon)-4, 3);
#endif
			char buf[100];
			sprintf(buf, "%d", crabs_destroyed);
			mTextout_simple(buf, 18, 2, white);
			sprintf(buf, "%d", sharks_destroyed);
			mTextout_simple(buf, 18, 16+2, white);

			double total_length = ((h*TILE_SIZE)-starty)-(TILE_SIZE*140);
			double progress = -(o - total_length);
			progress /= total_length;
			m_draw_line(BW/2-50, 8, BW/2+50, 8, black);
			int progx = (progress*100-50)+BW/2;
			m_draw_bitmap(
				sub_icon,
				progx-m_get_bitmap_width(sub_icon)/2,
				0,
				M_FLIP_HORIZONTAL
			);
	
			m_restore_blender();
			
			if (fps_on) {
				char msg[100];
				sprintf(msg, "%d", fps);
				mTextout(game_font, msg, 0, 0,
							white, black,
							WGT_TEXT_BORDER, false);
			}
			frames++;
			
			drawBufferToScreen();
			m_flip_display();
		}
	}
done:
	
	clear_input_events();

	if (!use_dpad) {
		tguiDeleteWidget(button);
		tguiDeleteWidget(slider);
		delete button;
		delete slider;
	}

	crabs.clear();
	bullets.clear();
	sharks.clear();
	
	for (int i = 0; i < (int)explosions.size(); i++) {
		delete explosions[i];
	}
	explosions.clear();

	if (replay) {
		goto start;
	}

	fadeOut(black);
	m_set_target_bitmap(buffer);
	m_clear(black);
	m_rest(5);

	playMusic("underwater_final.ogg");

	if (dead) {
		if (prompt("G A M E O V E R", "Try Again?", 1, 1))
			goto start;
	}
	else {
		char buf[100];
		if (for_points) {
			sprintf(buf, _t("%d gold (crabs), %d level pts. (sharks)"),
				crabs_destroyed * crab_value, sharks_destroyed * shark_value);
		}
		else {
			sprintf(buf, _t("%d crabs, %d sharks"),
				crabs_destroyed, sharks_destroyed);
		}

		bool again = prompt("V I C T O R Y !", "Try Again?", 0, 0, std::string(buf));

		if (again)
			goto start;
	}

	crabs_start.clear();
	sharks_start.clear();

	delete[] scene;
	delete[] solid;
	delete[] starts;

	m_destroy_bitmap(sub_bmp);
	m_destroy_bitmap(bullet);
	m_destroy_bitmap(shark_icon);
	m_destroy_bitmap(crab_icon);
	m_destroy_bitmap(pause_icon);
	m_destroy_bitmap(sub_icon);

	for (int i = 0; i < EXP_POOL_SIZE; i++) {
		delete explosion_pool[i];
	}

	delete crab;
	delete shark_anim;

#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
	delete_zone(pause_zone);
#endif

	m_destroy_bitmap(bbot);
	m_destroy_bitmap(btop);

	srand(seed);

	if (dead) {
		if (saveFilename) saveTime(saveFilename);
		dpad_on();
		in_shooter = false;
		return false;
	}
	else {
		if (for_points) {
			increaseGold(crabs_destroyed * crab_value);
			if (sharks_destroyed > 0) {
				for (int i = 0; i < MAX_PARTY; i++) {
					if (party[i])
					 while (levelUp(party[i], sharks_destroyed*shark_value))
						;
				}
			}
		}
		dpad_on();
		in_shooter = false;
		return true;
	}
}

