#ifndef COMBAT_ENTITY
#define COMBAT_ENTITY

class Combatant;

enum CombatEntityType {
	COMBATENTITY_TYPE_PLAYER = 0,
	COMBATENTITY_TYPE_ENEMY,
	COMBATENTITY_TYPE_FRILL // graphical frills etc
};


class CombatEntity {
public:
	float getX(void);
	float getY(void);
	void setX(float new_x);
	void setY(float new_y);
	int getId(void);
	void setId(int id);
	float getOx(void);
	float getOy(void);
	void setOx(float ox);
	void setOy(float oy);
	float getAngle(void);
	void setAngle(float a);

	CombatEntityType getType(void) { return type; }
	bool isDead(void);

	// return true when done
	virtual bool act(int step, Battle *b) = 0;
	virtual void draw(void) = 0;
	virtual bool update(int step) { return false; };
	virtual int getLifetime(void) { return -1; }


	CombatEntity();
	virtual ~CombatEntity();

protected:
	CombatEntityType type;
	float x, y;
	bool dead;
	int _id;
	float ox, oy; // offsets can be set by script to move
	float angle; // angle can be set by script to rotate
	int draw_flags;
};


class TemporaryText : public CombatEntity {
public:
	// return true when done
	bool act(int step, Battle *b);
	void draw(void);
	bool update(int step);


	// c"enter"x, c"enter"y
	TemporaryText(int cx, int cy, const std::string text, MCOLOR color);
	virtual ~TemporaryText();

protected:
	char text[100];
	int cx, cy;
	MCOLOR color;
	float yoffs;
};


class AttackSwoosh : public CombatEntity {
public:
	static const float SPEED;
	static const int NUM = 3;
	// return true when done
	bool act(int step, Battle *b);
	void draw(void);
	bool update(int step);


	// c"enter"x, c"enter"y
	AttackSwoosh(int dx, int dy, int dir, Combatant *attacked, Combatant *attacker);
	virtual ~AttackSwoosh();

protected:
	int dx, dy;
	int dir;
	MBITMAP *bitmaps[NUM];
	float currX[NUM];
	float speed;
	int w, h;
	Combatant *attacked;
	Combatant *attacker;
};


class LightningEffect : public CombatEntity {
public:
	static const int LIFETIME = 1100;
	static const int NUM_BITMAPS = 9;
	static const int NUM_FAST = 5; // first x frames play faster
	bool act(int step, Battle *battle) { return false; }
	bool update(int step);
	void draw(void);
	int getLifetime(void);

	LightningEffect(Combatant *target);
	virtual ~LightningEffect(void);

protected:
	int count;
	Combatant *target;
	MBITMAP *bitmaps[NUM_BITMAPS];
	//MSAMPLE sample;
};


class Bolt2Effect : public CombatEntity {
public:
	static const int LIFETIME = 1000;
	static const int NUM_BITMAPS = 8;
	bool act(int step, Battle *battle) { return false; }
	bool update(int step);
	void draw(void);
	int getLifetime(void);

	Bolt2Effect(Combatant *target);
	virtual ~Bolt2Effect(void);

protected:
	int count;
	Combatant *target;
	MBITMAP *bitmaps[NUM_BITMAPS];
};


struct SmallIcicle {
	float x, y;
};


const float GRAVITY = 0.001f;

struct Particle {
	float x, y;
	float dx, dy;
	int ground;
	MCOLOR color;
};


class Ice1Effect : public CombatEntity {
public:
	static const int LIFETIME = 3000;
	static const float VELOCITY;
	static const int NUM_ICICLES = 12;
	bool act(int step, Battle *battle) { return false; }
	bool update(int step);
	void draw(void);
	int getLifetime(void);

	Ice1Effect(Combatant *target);
	virtual ~Ice1Effect(void);

protected:
	void addIcicle(void);

	std::vector<SmallIcicle> icicles;
	std::vector<Particle> particles;
	int count;
	Combatant *target;
	MBITMAP *bitmap;
	float hyp;
	float dx, dy;
};



struct Ice2Flake {
	float x, y;
	float angle;
};

const float ICE2SHARD_MIN_SPEED = 0.02f;

struct Ice2Shard {
	float x, y;
	float angles[3];
	float lengths[3];
	MCOLOR color;
	float dx, dy;
};


class Ice2Effect : public CombatEntity {
public:
	static const int LIFETIME = 2500;
	bool act(int step, Battle *battle) { return false; }
	bool update(int step);
	void draw(void);
	int getLifetime(void);

	Ice2Effect(Combatant *target);
	virtual ~Ice2Effect(void);

protected:
	std::vector<Ice2Flake> snowflakes;
	Ice2Shard *shards;
	Combatant *target;
	MBITMAP *icecube;
	MBITMAP *snowflake;
	float scale;
	float ysize;
	int count;
	bool shattered;
	int numshards;
	ALLEGRO_VERTEX *verts;
};


class Fire1Effect : public CombatEntity {
public:
	bool act(int step, Battle *battle) { return false; }
	bool update(int step);
	void draw(void);
	int getLifetime(void);

	Fire1Effect(Combatant *target);
	virtual ~Fire1Effect(void);

protected:
	Combatant *target;
	AnimationSet *animSet;
	int count;
	//MSAMPLE sample;
};


struct Darkness1Blob {
	float x, y;
	float dx, dy;
	int bmpIndex;
	float angle;
	float scale;
	float ds;
};


class Darkness1Effect : public CombatEntity {
public:
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID || defined ALLEGRO_RASPBERRYPI
	static const int NUM_BLOBS = 10;
#else
	static const int NUM_BLOBS = 15;
#endif
	bool act(int step, Battle *battle) { return false; }
	bool update(int step);
	void draw(void);
	int getLifetime(void);

	Darkness1Effect(Combatant *target);
	virtual ~Darkness1Effect(void);

protected:
	void createBlob(Darkness1Blob *blobs, int i);

	Combatant *target;
	int count;
	MBITMAP *bitmaps[3];
	Darkness1Blob blobs[NUM_BLOBS];
	MBITMAP *buffer;
};


struct WeepDrop {
	float length;
	float x;
	float alpha;
	MCOLOR color;
};

class WeepEffect : public CombatEntity {
public:
	static const int NUM_DROPS = 28;
	bool act(int step, Battle *battle) { return false; }
	bool update(int step);
	void draw(void);
	int getLifetime(void);

	WeepEffect(Combatant *target);
	virtual ~WeepEffect(void);

protected:
	Combatant *target;
	int count;
	WeepDrop drops[NUM_DROPS];
};


class WaveEffect : public CombatEntity {
public:
	static const float SPEED;
	bool act(int step, Battle *battle) { return false; }
	bool update(int step);
	void draw(void);
	int getLifetime(void);

	WaveEffect(Combatant *target);
	virtual ~WaveEffect(void);

protected:
	Combatant *target;
	int count;
	float wx, wy, dx;
	MBITMAP *bitmap;
};

class WhirlpoolEffect : public CombatEntity {
public:
	static const float SPEED;
	bool act(int step, Battle *battle) { return false; }
	bool update(int step);
	void draw(void);
	int getLifetime(void);

	WhirlpoolEffect(Combatant *target);
	virtual ~WhirlpoolEffect(void);

protected:
	Combatant *target;
	int count;
	float angle;
	int w, h;
	MBITMAP *spiral;
};


struct SlimeBlob {
	float x, y;
	float dx, dy;
	MCOLOR color;
};


class SlimeEffect : public CombatEntity {
public:
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID || defined ALLEGRO_RASPBERRYPI
	static const int NUM_BLOBS = 5;
#else
	static const int NUM_BLOBS = 20;
#endif
	static const float SPEED;
	bool act(int step, Battle *battle) { return false; }
	bool update(int step);
	void draw(void);
	int getLifetime(void);

	SlimeEffect(Combatant *target);
	virtual ~SlimeEffect(void);

protected:
	Combatant *target;
	int count;
	SlimeBlob blobs[NUM_BLOBS];
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID || defined ALLEGRO_RASPBERRYPI
	ALLEGRO_VERTEX verts[NUM_BLOBS*6];
	MBITMAP *blob;
#endif
};


struct Fire2Pixel {
	float x, y, z;
	int offset;
	int animIndex;
	float dy;
};


class Fire2Effect : public CombatEntity {
public:
	static const float NUM_PIXELS;
	static const float RISE_SPEED;

	bool update(int step);
	void draw(void);
	int getLifetime(void);
	bool act(int step, Battle *battle) { return false; }

	Fire2Effect(Combatant *target);
	virtual ~Fire2Effect();
protected:
	int count;
	Fire2Pixel *pixels;
	int numPixels;
	int maxDepth;
	AnimationSet *animSet;
	Combatant *target;
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID || defined ALLEGRO_RASPBERRYPI
	ALLEGRO_VERTEX *verts;
#endif
};


class SludgeFlyEffect : public CombatEntity {
public:
	static const int LIFETIME = 300;

	bool update(int step);
	void draw(void);
	int getLifetime(void);
	bool act(int step, Battle *battle) { return false; }

	SludgeFlyEffect(Combatant *target, Combatant *caster, std::string bmp_name);
	virtual ~SludgeFlyEffect();
protected:
	int count;
	MBITMAP *bitmap;
	int flags;
	int start_x, start_y;
	int dest_x, dest_y;
	Combatant *target;
	Combatant *caster;
};


class SludgeEffect : public CombatEntity {
public:
	static const int FALLOFF_SIZE = 6;
	static const int LIFETIME = 1500;

	void draw(void);
	bool update(int step);
	int getLifetime(void);
	bool act(int step, Battle *battle) { return false; }

	SludgeEffect(Combatant *target, MCOLOR color);
	virtual ~SludgeEffect();
private:
	void get_info(void);
	void destroy_info(void);

	MCOLOR color;
	int count;
	std::vector<int> depths;
	Combatant *target;
	int **left_pixels;
	int **right_pixels;
	std::string anim_name;
	int numFrames;
};

class RendEffect : public CombatEntity {
public:
	bool act(int step, Battle *battle) { return false; }
	bool update(int step);
	void draw(void);
	int getLifetime(void);

	RendEffect(Combatant *target);
	virtual ~RendEffect(void);

protected:
	int count;
	Combatant *target;
	AnimationSet *animSet;
	int LIFETIME; //lazy
};


struct StompPuff
{
	float x, y;
	float dx, dy;
};

class StompEffect : public CombatEntity {
public:
	static const int LIFETIME = 1200;
	bool act(int step, Battle *battle) { return false; }
	bool update(int step);
	void draw(void);
	int getLifetime(void);

	StompEffect(CombatLocation l);
	virtual ~StompEffect(void);

protected:
	int count;
	CombatLocation location;
	MBITMAP *foot;
	std::vector<MBITMAP *> puffs;
	std::vector<StompPuff> puffData;
	bool puffed;
};


struct SprayPoint {
	int x;
	int length;
	MCOLOR color;
};


class SprayEffect : public CombatEntity {
public:
	static const int MAXLEN = 24;
	static const int MINLEN = 8;
	static const int TOP = 200;
	static const int MIDDLE = 30;
	static const int LIFETIME = 2000;
	static const int HEIGHT = 16;
	bool act(int step, Battle *battle) { return false; }
	bool update(int step);
	void draw(void);
	int getLifetime(void);

	SprayEffect(Combatant *c);
	virtual ~SprayEffect(void);

protected:
	Combatant *target;
	std::vector< std::vector<SprayPoint> > points;
	int lx, rx, cy;
	float ox;
	int count;
};


class PunchEffect : public CombatEntity {
public:
	static const int LIFETIME = 300;
	bool act(int step, Battle *battle) { return false; }
	bool update(int step);
	void draw(void);
	int getLifetime(void);

	PunchEffect(Combatant *target, Combatant *caster);
	virtual ~PunchEffect(void);

protected:
	float sx, sy, dx, dy;
	int flags;
	int count;
	AnimationSet *animSet;
};


struct TorrentDrop {
	float start_x;
	float start_y;
	float x;
	float y;
	float speed;
	MCOLOR color;
};


class TorrentEffect : public CombatEntity {
public:
	static const int LIFETIME = 3000;
	static const int NUM_DROPS = 300;
	bool act(int step, Battle *battle) { return false; }
	bool update(int step);
	void draw(void);
	int getLifetime(void);

	TorrentEffect(Combatant *caster);
	virtual ~TorrentEffect(void);

protected:
	int count;
	TorrentDrop drops[NUM_DROPS];
	CombatLocation loc;
};

struct LaserSmoke {
	float x, y;
	int life;
};

class BeamP1Effect : public CombatEntity {
public:
	static const int LIFETIME = 2200;
	bool act(int step, Battle *battle) { return false; }
	bool update(int step);
	void draw(void);
	int getLifetime(void);

	BeamP1Effect(Combatant *target, Combatant *caster);
	virtual ~BeamP1Effect(void);

protected:
	float sx, sy, dx, dy;
	int count;
};


class BeamP2Effect : public CombatEntity {
public:
	static const int LIFETIME = 2200;
	bool act(int step, Battle *battle) { return false; }
	bool update(int step);
	void draw(void);
	int getLifetime(void);

	BeamP2Effect(Combatant *target, Combatant *caster);
	virtual ~BeamP2Effect(void);

protected:
	float sx, sy, dx, dy;
	int count;
	int puffCount;
	MBITMAP *smoke;
	std::vector<LaserSmoke> puffs;
};


class Fire3Effect : public CombatEntity {
public:
	static const int LIFETIME = 3000;
	static const int W = 70;
	static const int H = 70;
	bool act(int step, Battle *battle) { return false; }
	bool update(int step);
	void draw(void);
	int getLifetime(void);

	Fire3Effect(Combatant *target);
	virtual ~Fire3Effect(void);

protected:
	int count;
	MBITMAP *fire_bmp;
	int offs;
	int dx, dy;
};

struct ExplosionCircle {
	float x, y;
	MCOLOR color;
	float radius;
	int count;
	int lifetime;
};



class ExplodeEffect : public CombatEntity {
public:
	static const int LIFETIME = 2000;
	bool act(int step, Battle *battle) { return false; }
	bool update(int step) {
		count += step;

		if (count >= LIFETIME) {
			return true;
		}

		for (int i = 0; i < numExplosionCircles; i++) {
			explosionCircles[i].count += step;
			explosionCircles[i].color.a = 255 * (1 - (float)explosionCircles[i].count / explosionCircles[i].lifetime);
			if (explosionCircles[i].count > explosionCircles[i].lifetime) {
				explosionCircles[i].count = 0;
				explosionCircles[i].x = x + ((rand() % w) - (w/2));
				explosionCircles[i].y = y - rand() % h;
			}
		}

		return false;
	}
	void draw(void) {
		for (int i = 0; i < numExplosionCircles; i++) {
			int r = explosionCircles[i].radius * (float)explosionCircles[i].count / explosionCircles[i].lifetime;
			m_draw_circle(explosionCircles[i].x, explosionCircles[i].y,
				r, explosionCircles[i].color, M_FILLED);
		}
	}

	int getLifetime(void)
	{
		return LIFETIME;
	}

	ExplodeEffect(Combatant *target);
	virtual ~ExplodeEffect(void) {
		delete[] explosionCircles;
	}

protected:
	int numExplosionCircles;
	ExplosionCircle *explosionCircles;
	int count;
	int w, h;
};





class Ice3Effect : public CombatEntity {
public:
	static const int LIFETIME = 1500;
	static const int BELOW_GROUND = 32;
	static const int NUM_PARTICLES = 100;
	bool act(int step, Battle *battle) { return false; }
	bool update(int step);
	void draw(void);
	int getLifetime(void);

	void genparticle(int i);

	Ice3Effect(Combatant *target);
	virtual ~Ice3Effect(void);

protected:
	float sy, dy, yy;
	int count;
	MBITMAP *icicle;
	Particle particles[NUM_PARTICLES];
	float alpha;
};



class Bolt3Effect : public CombatEntity {
public:
	static const int LIFETIME = 3500;
	static const int PTS_PER_ARC = 7;
	static const float MAX_DIST;
	static const float MAX_AMPLITUDE;
	static const int MAX_RAD = 30;
	bool act(int step, Battle *battle) { return false; }
	bool update(int step);
	void draw(void);
	int getLifetime(void);

	void generate(int n);
	void draw_arcs(std::vector< std::vector<MPoint> > &arcs);

	Bolt3Effect(Combatant *target);
	virtual ~Bolt3Effect(void);

protected:
	float cx, cy;
	std::vector< std::vector<MPoint> > arcs;
	MBITMAP *alpha_sprite;
	MBITMAP *ring;
	int amp_dir;
	float amp;
	float circ_rad;
	int count;
	MBITMAP *predrawn;
	float angle;
};


class Darkness2Effect : public CombatEntity {
public:
	static const int LIFETIME = 3000;
	bool act(int step, Battle *battle) { return false; }
	bool update(int step);
	void draw(void);
	int getLifetime(void);

	Darkness2Effect(CombatLocation loc);
	virtual ~Darkness2Effect(void);

protected:
	float f;
	int count;
	CombatLocation loc;
};


class TwisterEffect : public CombatEntity {
public:
	static const int LIFETIME = 2000;
	bool act(int step, Battle *battle) { return false; }
	bool update(int step);
	void draw(void);
	int getLifetime(void);

	TwisterEffect(Combatant *target);
	virtual ~TwisterEffect(void);

protected:
	int count;
	Combatant *target;
	MBITMAP *bmp;
	float f;
	int rotate_count;
};


struct ColoredPoint {
	float x, y;
	MCOLOR color;
};


class WhipEffect : public CombatEntity {
public:
	static const int LIFETIME = 250;
	static const int POINTS = 300;
	bool act(int step, Battle *battle) { return false; }
	bool update(int step);
	void draw(void);
	int getLifetime(void);

	WhipEffect(Combatant *caster, Combatant *target);
	virtual ~WhipEffect(void);

protected:
	int count;
	MPoint pts1[POINTS];
	MPoint pts2[POINTS];
	ColoredPoint curr[POINTS];
};


struct Acorn {
	float x, y;
	float dx, dy;
	float angle;
};

class AcornsEffect : public CombatEntity {
public:
	static const int LIFETIME = 1500;
	static const int ACORNS = 6;
	static const int CIRCLES = 3;
	bool act(int step, Battle *battle) { return false; }
	bool update(int step);
	void draw(void);
	int getLifetime(void);

	AcornsEffect(Combatant *caster, Combatant *target);
	virtual ~AcornsEffect(void);

protected:
	int count;
	Combatant *target;

	Acorn acorns[ACORNS];
	ExplosionCircle explosions[ACORNS][CIRCLES];

	MBITMAP *bitmap;
};


class DaisyEffect : public CombatEntity {
public:
	static const int LIFETIME = 1500;
	bool act(int step, Battle *battle) { return false; }
	bool update(int step);
	void draw(void);
	int getLifetime(void);

	DaisyEffect(Combatant *target);
	virtual ~DaisyEffect(void);

protected:
	int count;
	int dx, dy;
	int flags;
	AnimationSet *animSet;
};


class VampireEffect : public CombatEntity {
public:
	static const int LIFETIME = 1200;
	bool act(int step, Battle *battle) { return false; }
	bool update(int step);
	void draw(void);
	int getLifetime(void);

	VampireEffect(Combatant *target);
	virtual ~VampireEffect(void);

protected:
	int count;
	Combatant *target;
	int dx, dy;
	int sx, sy;
	int cx, cy, alpha;
};


class ArcEffect : public CombatEntity {
public:
	static const int LIFETIME = 1500;
	bool act(int step, Battle *battle) { return false; }
	bool update(int step);
	void draw(void);
	int getLifetime(void);

	ArcEffect(Combatant *caster, Combatant *target);
	virtual ~ArcEffect(void);

protected:
	int count;
	std::vector<MPoint> arc;
	AnimationSet *alphaAnim;
};


struct ExplodingBitmapPoint {
	ColoredPoint cp;
	float dx, dy;
};


class BananaEffect : public CombatEntity {
public:
	static const int LIFETIME = 2000;
	bool act(int step, Battle *battle) { return false; }
	bool update(int step);
	void draw(void);
	int getLifetime(void);

	BananaEffect(Combatant *caster, Combatant *target, std::string imgName);
	virtual ~BananaEffect(void);

protected:
	int count;
	int sx, sy, dx, dy, cx, cy;
	MBITMAP *bitmap;
	float bmpAngle;
	std::vector<ExplodingBitmapPoint> pixels;
	bool switchedY;
	float newY;
};


class FireballEffect : public CombatEntity {
public:
	static const int LIFETIME = 600;
	bool act(int step, Battle *battle) { return false; }
	bool update(int step);
	void draw(void);
	int getLifetime(void);

	FireballEffect(Combatant *caster, Combatant *target);
	virtual ~FireballEffect(void);

protected:
	int count;
	float sx, sy, dx, dy, cx, cy;
	int w, h;
	int flags;
	AnimationSet *animSet;
};


class KissOfDeathEffect : public CombatEntity {
public:
	static const int LIFETIME = 2000;
	bool act(int step, Battle *battle) { return false; }
	bool update(int step);
	void draw(void);
	int getLifetime(void);

	KissOfDeathEffect(Combatant *caster, Combatant *target);
	virtual ~KissOfDeathEffect(void);

protected:
	int count;
	int alpha;
	int dx, dy;
	MBITMAP *bitmap;
};


class BoFEffect : public CombatEntity {
public:
	static const int LIFETIME = 3000;
	bool act(int step, Battle *battle) { return false; }
	bool update(int step);
	void draw(void);
	int getLifetime(void);

	BoFEffect(Combatant *caster, Combatant *target);
	virtual ~BoFEffect(void);

protected:
	int count;
	MBITMAP *bitmap;
	float offs;
	int w, h;
};


class DropEffect : public CombatEntity {
public:
	static const int LIFETIME = 1800;
	bool act(int step, Battle *battle) { return false; }
	bool update(int step);
	void draw(void);
	int getLifetime(void);

	DropEffect(Combatant *caster, Combatant *target);
	virtual ~DropEffect(void);

protected:
	int count;
	float oy, dy, rock_y;
	MBITMAP *bitmap;
	std::vector<ExplodingBitmapPoint> pixels;
	Combatant *caster;
	Combatant *target;
	bool exploded;
	float bmpAngle;
	bool dropped;
};

class BlazeEffect : public CombatEntity {
public:
	static const int LIFETIME = 2200;
	bool act(int step, Battle *battle) { return false; }
	bool update(int step);
	void draw(void);
	int getLifetime(void);

	BlazeEffect(Combatant *caster);
	virtual ~BlazeEffect(void);

protected:
	float sx, sy, dx, dy;
	int count;
};


class MachineGunEffect : public CombatEntity {
public:
	static const int LIFETIME = 1500;
	bool act(int step, Battle *battle) { return false; }
	bool update(int step);
	void draw(void);
	int getLifetime(void);

	MachineGunEffect(Combatant *caster, Combatant *target);
	virtual ~MachineGunEffect(void);

protected:
	int count;
	int cx, cy;
};


class LaserEffect : public CombatEntity {
public:
	static const int LIFETIME = 600;
	bool act(int step, Battle *battle) { return false; }
	bool update(int step);
	void draw(void);
	int getLifetime(void);

	LaserEffect(Combatant *caster, Combatant *target);
	virtual ~LaserEffect(void);

protected:
	int count;
	float cx, cy;
	float angle;
	float start_x, end_x;
	float start_y, end_y;
	Combatant *caster;
};


class UFOEffect : public CombatEntity {
public:
	static const int MAXLEN = 50;
	bool act(int step, Battle *battle) { return false; }
	bool update(int step);
	void draw(void);
	int getLifetime(void);

	UFOEffect(Combatant *caster, Combatant *target);
	virtual ~UFOEffect(void);

protected:
	int count;
	int start_x, start_y, end_x, end_y;
	int cx, cy;
	float angle;
	float offs;
	float length;
	Combatant *caster;
	float ufo_start_y;
};


class OrbitEffect : public CombatEntity {
public:
	static const int LIFETIME = 1500;
	static const int MAXLEN = 50;
	bool act(int step, Battle *battle) { return false; }
	bool update(int step);
	void draw(void);
	int getLifetime(void);

	OrbitEffect(Combatant *caster, Combatant *target);
	virtual ~OrbitEffect(void);

protected:
	int count;
	Combatant *caster;
};


class WebEffect : public CombatEntity {
public:
	static const int LIFETIME = 600;
	bool act(int step, Battle *battle) { return false; }
	bool update(int step);
	void draw(void);
	int getLifetime(void);

	WebEffect(Combatant *caster, Combatant *target);
	virtual ~WebEffect(void);

protected:
	int count;
	int cx, cy;
	int sx, sy, dx, dy;
	MBITMAP *bitmap;
	float angle;
};


class Darkness3Effect : public CombatEntity {
public:
	static const int LIFETIME = 2500;
	bool act(int step, Battle *battle) { return false; }
	bool update(int step);
	void draw(void);
	int getLifetime(void);

	Darkness3Effect(void);
	virtual ~Darkness3Effect(void);

protected:
	int count;
	bool dark;
};


class CombatEnemyTode;

class SwallowEffect : public CombatEntity {
public:
	static const int LIFETIME = 950;
	static const int SWALLOW_TIME = 800;
	bool act(int step, Battle *battle) { return false; }
	bool update(int step);
	void draw(void);
	int getLifetime(void);

	SwallowEffect(Combatant *caster, Combatant *target);
	virtual ~SwallowEffect(void);

protected:
	int count;
	Combatant *target;
	CombatEnemyTode *caster;
	int cx, cy, sx, sy, dx, dy;
	MBITMAP *bitmap;
	float angle;
};


struct PukeLine
{
	float cx, cy;
	float angle;
	MCOLOR color;
};

class PukeEffect : public CombatEntity {
public:
	static const int START_TIME = 1350;
	static const int PUKE_TIME = 2000;
	static const int LIFETIME = START_TIME+PUKE_TIME+150;
	bool act(int step, Battle *battle) { return false; }
	bool update(int step);
	void draw(void);
	int getLifetime(void);

	void genline(void);

	PukeEffect(Combatant *caster);
	virtual ~PukeEffect(void);

protected:
	int count;
	std::vector<PukeLine> lines;
	Combatant *caster;
};

bool updateParticle(Particle &part, int step);

#endif

