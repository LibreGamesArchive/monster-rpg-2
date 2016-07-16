#include "monster2.hpp"
#include "CombatEntity.hpp"

const float Ice1Effect::VELOCITY = 0.5f;
const float WaveEffect::SPEED = 0.16f;
const float WhirlpoolEffect::SPEED = 0.007f;
const float SlimeEffect::SPEED = 0.032f;
const float Bolt3Effect::MAX_DIST = 36;
const float Bolt3Effect::MAX_AMPLITUDE = 10;
const int Bolt3Effect::LIFETIME = 3500;
const int Bolt3Effect::PTS_PER_ARC = 3;
const int Bolt3Effect::MAX_RAD = 30;
const float AttackSwoosh::SPEED = 0.5f;
const float Fire2Effect::NUM_PIXELS = 0.5f;
const float Fire2Effect::RISE_SPEED = 0.05f;

static MBITMAP **bolt2_bmps;
static int bolt2_bmp_ref_count = 0;
static int num_bolt2_bmps;

static void get_bolt2_bitmaps(int n, MBITMAP **bmps)
{
	num_bolt2_bmps = n;
	if (bolt2_bmp_ref_count == 0) {
		bolt2_bmps = new MBITMAP *[n];
		for (int i = 0; i < num_bolt2_bmps; i++) {
			char name[100];
			sprintf(name, "Bolt2_%d.png", i);
			bolt2_bmps[i] = m_load_alpha_bitmap(getResource("combat_media/%s", name));
		}
	}
	for (int i = 0; i < num_bolt2_bmps; i++) {
		bmps[i] = bolt2_bmps[i];
	}
	bolt2_bmp_ref_count++;
}

static void release_bolt2_bitmaps(void)
{
	bolt2_bmp_ref_count--;
	if (bolt2_bmp_ref_count == 0) {
		for (int i = 0; i < num_bolt2_bmps; i++) {
			m_destroy_bitmap(bolt2_bmps[i]);
		}
		delete[] bolt2_bmps;
	}
}

float CombatEntity::getX(void)
{
	return x;
}

float CombatEntity::getY(void)
{
	return y;
}

void CombatEntity::setX(float n)
{
	x = n;
}

void CombatEntity::setY(float n)
{
	y = n;
}

int CombatEntity::getId(void)
{
	return _id;
}

float CombatEntity::getOx(void)
{
	return ox;
}

float CombatEntity::getOy(void)
{
	return oy;
}

float CombatEntity::getAngle(void)
{
	return angle;
}

void CombatEntity::setId(int i)
{
	_id = i;
}

void CombatEntity::setOx(float o)
{
	ox = o;
}

void CombatEntity::setOy(float o)
{
	oy = o;
}

void CombatEntity::setAngle(float a)
{
	angle = a;
}


bool CombatEntity::isDead(void)
{
	if (type == COMBATENTITY_TYPE_PLAYER || type
			== COMBATENTITY_TYPE_ENEMY) {
		Combatant *c = (Combatant *)this;
		if (c->getInfo().abilities.hp <= 0) {
			return true;
		}
	}
	return dead;
}


CombatEntity::CombatEntity() :
	dead(false),
	_id(-1),
	ox(0.0f),
	oy(0.0f),
	angle(0.0f)
{
	x = y = 0.0f;
}


CombatEntity::~CombatEntity()
{
}


ExplodeEffect::ExplodeEffect(Combatant *target)
{
	type = COMBATENTITY_TYPE_FRILL;

	count = 0;
	x = target->getX();
	y = target->getY()+1;
	w = target->getAnimationSet()->getWidth();
	h = target->getAnimationSet()->getHeight();
	numExplosionCircles = (w*h)/64;
	explosionCircles = new ExplosionCircle[numExplosionCircles];
	for (int i = 0; i < numExplosionCircles; i++) {
		explosionCircles[i].x = x + ((rand() % w) -  (w/2));
		explosionCircles[i].y = y - rand() % h;
		int n = rand() % 4;
		switch (n) {
			case 0:
				explosionCircles[i].color = m_map_rgb(230, 230, 230);
				break;
			case 1:
				explosionCircles[i].color = m_map_rgb(250, 40, 0);
				break;
			case 2:
				explosionCircles[i].color = m_map_rgb(200, 250, 0);
				break;
			case 3:
				explosionCircles[i].color = m_map_rgb(30, 30, 30);
				break;
		}
		explosionCircles[i].radius = 5 + rand() % 10;
		explosionCircles[i].count = 0;
		explosionCircles[i].lifetime = 250 + rand() % 400;
	}
}

// return true when done
bool TemporaryText::act(int step, Battle *b)
{
	return false;
}

void TemporaryText::draw(void)
{
	ALLEGRO_COLOR darker;
	darker.r = color.r * 0.2f;
	darker.g = color.g * 0.2f;
	darker.b = color.b * 0.2f;
	darker.a = color.a;
	mTextout(game_font, _t(text), cx, (int)(cy + yoffs),
		color, darker,
		WGT_TEXT_8WAY_SHADOW, true);
}

bool TemporaryText::update(int step)
{
	yoffs -= 0.01f * step;
	if (yoffs < -10)
		return true;
	return false;
}

TemporaryText::TemporaryText(int cx, int cy, const std::string text,
	MCOLOR color)
{
	x = 0;
	y = INT_MAX;
	this->cx = cx;
	this->cy = cy;
	this->color = color;
	yoffs = 0;
	type = COMBATENTITY_TYPE_FRILL;
	dead = false;
	strcpy(this->text, text.c_str());
}

TemporaryText::~TemporaryText()
{
}


bool AttackSwoosh::act(int step, Battle *b)
{
	return false;
}


void AttackSwoosh::draw(void)
{
	int flags;
	
	if (dir == DIRECTION_WEST) {
		flags = 0;
	}
	else {
		flags = M_FLIP_HORIZONTAL;
	}

	for (int i = 0; i < NUM; i++) {
		m_draw_bitmap(bitmaps[i], (int)currX[i]-w/2, dy-h/2, flags);
	}
}


bool AttackSwoosh::update(int step)
{
	for (int i = 0; i < NUM; i++) {
		currX[i] += speed * step;
		if (dir == DIRECTION_WEST) {
			if (currX[i] < dx) {
				currX[i] = dx;
				if (i == (NUM-1)) {
					if (attacked->getType() == COMBATENTITY_TYPE_PLAYER || !attacked->isDefending()) {
						if (!(attacked->getAnimationSet()->getSubName() == "Dragon_transform"))
							attacked->getAnimationSet()->setSubAnimation("stand");
					}
					if (attacker->getType() == COMBATENTITY_TYPE_ENEMY)
						attacker->getAnimationSet()->setSubAnimation("stand");
					return true;
				}
			}
		}
		else {
			if (currX[i] > dx) {
				currX[i] = dx;
				if (i == (NUM-1)) {
					if (attacked->getType() == COMBATENTITY_TYPE_PLAYER || !attacked->isDefending())
						attacked->getAnimationSet()->setSubAnimation("stand");
					if (attacker->getType() == COMBATENTITY_TYPE_ENEMY)
						attacker->getAnimationSet()->setSubAnimation("stand");
					return true;
				}
			}
		}
	}

	return false;
}


AttackSwoosh::AttackSwoosh(int dx, int dy, int dir, Combatant *attacked, Combatant *attacker)
{
	type = COMBATENTITY_TYPE_FRILL;

	x = attacked->getX();
	y = attacked->getY()+1;
	this->dx = dx;
	this->dy = dy;
	this->dir = dir;
	this->attacked = attacked;
	this->attacker = attacker;

	attacked->getAnimationSet()->setSubAnimation("hit");

	for (int i = 0; i < NUM; i++) {
		char name[30];
		sprintf(name, "swoosh%d.png", i+1);
		bitmaps[i] = m_load_bitmap(getResource("combat_media/%s", name));

		if (dir == DIRECTION_WEST) {
			if (i == 0) {
				currX[0] = dx + 50;
			}
			else {
				currX[i] = currX[i-1] + (i*10);
			}
		}
		else {
			if (i == 0) {
				currX[0] = dx - 50;
			}
			else {
				currX[i] = currX[i-1] - (i*10);
			}
		}
	}

	if (dir == DIRECTION_WEST) {
		speed = -SPEED;
	}
	else {
		speed = SPEED;
	}

	w = m_get_bitmap_width(bitmaps[0]);
	h = m_get_bitmap_height(bitmaps[0]);
	
	type = COMBATENTITY_TYPE_FRILL;
	dead = false;
}


AttackSwoosh::~AttackSwoosh()
{
	for (int i = 0; i < NUM; i++) {
		m_destroy_bitmap(bitmaps[i]);
	}
}

int Bolt2Effect::getLifetime(void)
{
	return LIFETIME;
}

void Bolt2Effect::draw(void)
{
	int bmpIndex;

	if (target->getLocation() == LOCATION_LEFT) {
		bmpIndex = (NUM_BITMAPS-1) - (count / (LIFETIME/(NUM_BITMAPS+1)));
		if (bmpIndex < 0)
			bmpIndex = 0;
	}
	else {
		bmpIndex = count / (LIFETIME/(NUM_BITMAPS+1));
		if (bmpIndex >= NUM_BITMAPS)
			bmpIndex = NUM_BITMAPS-1;
	}

	int dx = (int)(target->getX() - (m_get_bitmap_width(bitmaps[0])/2));
	int dy = (int)(target->getY() - m_get_bitmap_height(bitmaps[0]));

	m_draw_bitmap(bitmaps[bmpIndex], dx, dy, 0);
}


bool Bolt2Effect::update(int step)
{
	count += step;

	if (count >= LIFETIME) {
		return true;
	}

	return false;
}


Bolt2Effect::Bolt2Effect(Combatant *target) :
	count(0)
{
	type = COMBATENTITY_TYPE_FRILL;
	
	x = target->getX();
	y = target->getY()+1;

	this->target = target;

	get_bolt2_bitmaps(NUM_BITMAPS, bitmaps);
}


Bolt2Effect::~Bolt2Effect(void)
{
	release_bolt2_bitmaps();
}

int LightningEffect::getLifetime(void)
{
	return LIFETIME;
}

void LightningEffect::draw(void)
{
	int bmpIndex;
	if (count < (LIFETIME/3)) {
		bmpIndex = (float)count/(LIFETIME/3) * NUM_FAST;
	}
	else {
		bmpIndex = NUM_FAST + (float)(count-(LIFETIME/3))/(LIFETIME/2) * (NUM_BITMAPS-NUM_FAST);
	}
	if (bmpIndex >= NUM_BITMAPS)
		bmpIndex = NUM_BITMAPS-1;
	int dx = (int)(target->getX() - (m_get_bitmap_width(bitmaps[0])/2));
	int dy = (int)(target->getY() - m_get_bitmap_height(bitmaps[0]));

	m_draw_bitmap(bitmaps[bmpIndex], dx, dy, 0);
}


bool LightningEffect::update(int step)
{
	count += step;

	if (count >= LIFETIME) {
		return true;
	}

	return false;
}


LightningEffect::LightningEffect(Combatant *target) :
	count(0)
{
	type = COMBATENTITY_TYPE_FRILL;
	
	x = target->getX();
	y = target->getY()+1;

	this->target = target;

	for (int i = 0; i < NUM_BITMAPS; i++) {
		char name[100];
		sprintf(name, "bolt1_%d.png", i);
		bitmaps[i] = m_load_alpha_bitmap(getResource("combat_media/%s", name));
	}

	debug_message("after loading lightning bitmaps");
}


LightningEffect::~LightningEffect(void)
{
	for (int i = 0; i < NUM_BITMAPS; i++) {
		m_destroy_bitmap(bitmaps[i]);
	}
}


bool updateParticle(Particle &part, int step)
{
	part.x += part.dx * step;
	part.y += (part.dy + GRAVITY) * step;
	if (part.y > part.ground) {
		// dead
		return true;
	}
	return false;
}


// FIXME: bah, angle names for this ice effect are all wrong

int Ice1Effect::getLifetime(void)
{
	return LIFETIME;
}

void Ice1Effect::draw(void)
{
	if (target->getLocation() == LOCATION_LEFT) {
		for (int i = 0; i < (int)icicles.size(); i++) {
			SmallIcicle &icicle = icicles[i];
			int dx = (int)icicle.x;
			int dy = (int)(icicle.y - m_get_bitmap_height(bitmap));
			m_draw_bitmap(bitmap, dx, dy, 0);
		}
	}
	else {
		for (int i = 0; i < (int)icicles.size(); i++) {
			SmallIcicle &icicle = icicles[i];
			int dx = (int)(icicle.x - m_get_bitmap_width(bitmap));
			int dy = (int)(icicle.y - m_get_bitmap_height(bitmap));
			m_draw_bitmap(bitmap, dx, dy, M_FLIP_HORIZONTAL);
		}
	}

#if defined ALLEGRO_ANDROID || defined ALLEGRO_RASPBERRYPI
	if (particles.size() > 0) {
		ALLEGRO_VERTEX *verts = new ALLEGRO_VERTEX[particles.size()];
		for (int i = 0; i < (int)particles.size(); i++) {
			Particle &part = particles[i];
			verts[i].x = part.x;
			verts[i].y = part.y;
			verts[i].z = 0;
			verts[i].color = part.color;
		}
		m_draw_prim(verts, 0, NULL, 0, particles.size(), ALLEGRO_PRIM_POINT_LIST);
		delete[] verts;
	}
#else
	for (int i = 0; i < (int)particles.size(); i++) {
		Particle &part = particles[i];
		m_draw_pixel((int)part.x, (int)part.y, part.color);
	}
#endif
}


bool Ice1Effect::update(int step)
{
	count += step;

	if (count >= LIFETIME) {
		return true;
	}

	std::vector<SmallIcicle>::iterator it;
	int numToAdd = 0;

	for (it = icicles.begin(); it != icicles.end();) {
		SmallIcicle &icicle = *it;
		icicle.x += dx * step;
		icicle.y += dy * step;
		if (icicle.y >= y) {
			it = icicles.erase(it);
			if (particles.size() > 256) {
				particles.erase(particles.begin(), particles.begin()+10);
			}
			for (int i = 0; i < 10; i++) {
				Particle part;
				part.x = icicle.x;
				part.y = icicle.y;
				part.dx = (float)((rand() % 1000) - 500) / 10000.0f;
				part.dy = ((float)(rand() % 1000) / 1000.0f) * -0.05f;
				part.ground = (int)y;
				int r = 0;
				int g = 200 + rand() % 55;
				int b = 200 + rand() % 55;
				int a = 255;
				part.color = m_map_rgba(r, g, b, a);
				particles.push_back(part);
			}
			numToAdd++;
			continue;
		}
		it++;
	}

	for (int i = 0; i < numToAdd; i++) {
		addIcicle();
	}

	std::vector<Particle>::iterator it2;

	for (it2 = particles.begin(); it2 != particles.end();) {
		Particle &part = *it2;
		bool dead = updateParticle(part, step);
		if (dead) {
			it2 = particles.erase(it2);
		}
		else {
			it2++;
		}
	}

	return false;
}


void Ice1Effect::addIcicle(void)
{
	if (target->getLocation() == LOCATION_LEFT) {
		float cos60 = cos((M_PI*2)/12*10);
		float sin60 = sin((M_PI*2)/12*10);
		float ix, iy;
		float h = hyp + rand() % (int)hyp;
		ix = cos60 * h;
		iy = sin60 * h;
		ix += ((rand() % 30) - 15);
		SmallIcicle icicle;
		icicle.x = ix + x + 30;
		icicle.y = iy;
		icicles.push_back(icicle);
	}
	else if (target->getLocation() == LOCATION_RIGHT) {
		float cos120 = cos((M_PI*2)/12*8);
		float sin120 = sin((M_PI*2)/12*8);
		float ix, iy;
		float h = hyp + rand() % (int)hyp;
		ix = cos120 * h;
		iy = sin120 * h;
		ix += ((rand() % 30) - 15);
		SmallIcicle icicle;
		icicle.x = ix + x - 30;
		icicle.y = iy;
		icicles.push_back(icicle);
	}
}


Ice1Effect::Ice1Effect(Combatant *target) :
	count(0)
{
	type = COMBATENTITY_TYPE_FRILL;
	
	x = target->getX();
	y = target->getY()+1;

	this->target = target;

	bitmap = m_load_bitmap(getResource("combat_media/small_icicle.png"));

	float yl = y;
	float tan60 = tan((M_PI*2)/12*10);
	float xl = yl / tan60;
	hyp = sqrt(yl*yl + xl*xl);

	for (int i = 0; i < NUM_ICICLES; i++) {
		addIcicle();
	}

	if (target->getLocation() == LOCATION_LEFT) {
		dx = cos((M_PI*2)/12*4);
		dy = sin((M_PI*2)/12*4);
	}
	else {
		dx = cos((M_PI*2)/12*2);
		dy = sin((M_PI*2)/12*2);
	}

	dx *= VELOCITY;
	dy *= VELOCITY;
}


Ice1Effect::~Ice1Effect(void)
{
	m_destroy_bitmap(bitmap);
}


int Ice2Effect::getLifetime(void)
{
	return LIFETIME;
}

void Ice2Effect::draw(void)
{
	if (count < 1000) {
		float w = m_get_bitmap_width(icecube) * scale;
		float h = m_get_bitmap_height(icecube) * scale;
		float yy = 0;
		int sh = m_get_bitmap_height(icecube)-yy;
		if (sh <= 0) sh = 1;
		int dh = h - (yy*scale);
		if (dh <= 0) dh = 1;
		m_draw_scaled_bitmap(icecube, 0, yy, m_get_bitmap_width(icecube), sh,
			x-w/2, (y-h)+(yy*scale), w, dh, 0, 225);
		for (int i = 0; i < (int)snowflakes.size(); i++) {
			m_draw_rotated_bitmap(snowflake, 
				m_get_bitmap_width(snowflake)/2,
				m_get_bitmap_height(snowflake)/2,
				snowflakes[i].x, y - snowflakes[i].y, snowflakes[i].angle, 0);
		}
	}
	else {
		if (!shattered) {
			shattered = true;
		}

		for (int i = 0; i < numshards; i++) {
			float a = shards[i].color.a = 1 - ((float)(count - 1000) / (getLifetime() - 1000));
			ALLEGRO_COLOR color = al_map_rgba_f(
				shards[i].color.r*a,
				shards[i].color.g*a,
				shards[i].color.b*a,
				a
			);
			float x1, y1, x2, y2, x3, y3;
			x1 = x + shards[i].x + cos(shards[i].angles[0]) * shards[i].lengths[0];
			y1 = y - shards[i].y + sin(shards[i].angles[0]) * shards[i].lengths[0];
			x2 = x + shards[i].x + cos(shards[i].angles[1]) * shards[i].lengths[1];
			y2 = y - shards[i].y + sin(shards[i].angles[1]) * shards[i].lengths[1];
			x3 = x + shards[i].x + cos(shards[i].angles[2]) * shards[i].lengths[2];
			y3 = y - shards[i].y + sin(shards[i].angles[2]) * shards[i].lengths[2];
			verts[i*3].x = x1;
			verts[i*3].y = y1;
			verts[i*3].z = 0;
			verts[i*3].color = color;
			verts[i*3+1].x = x2;
			verts[i*3+1].y = y2;
			verts[i*3+1].z = 0;
			verts[i*3+1].color = color;
			verts[i*3+2].x = x3;
			verts[i*3+2].y = y3;
			verts[i*3+2].z = 0;
			verts[i*3+2].color = color;
		}
		
		m_draw_prim(verts, 0, 0, 0, numshards*3, ALLEGRO_PRIM_TRIANGLE_LIST);
	}
}


bool Ice2Effect::update(int step)
{
	count += step;

	if (count >= LIFETIME) {
		return true;
	}

	for (int i = 0; i < (int)snowflakes.size(); i++) {
		snowflakes[i].angle += 0.001f * step;
		snowflakes[i].y = (m_get_bitmap_height(icecube) * scale) * ((float)count / 1000.0f);
	}

	if (!shattered) return false;

	for (int i = 0; i < numshards; i++) {
		shards[i].x += shards[i].dx * step;
		shards[i].y += shards[i].dy * step;
	}

	return false;
}


Ice2Effect::Ice2Effect(Combatant *target) :
	count(0)
{
	type = COMBATENTITY_TYPE_FRILL;
	
	x = target->getX();
	y = target->getY()+1;

	this->target = target;

	icecube = m_load_bitmap(getResource("combat_media/Ice2.png"));
	snowflake = m_load_bitmap(getResource("combat_media/snowflake.png"));

	ysize = 0.0f;
	
	int bmpsize = m_get_bitmap_width(icecube);
	int enemysize = MAX(target->getAnimationSet()->getWidth(), target->getAnimationSet()->getHeight());
	scale = ((float)enemysize / bmpsize) * 1.20f;

	int numflakes = (int)(scale * 40);

	for (int i = 0; i < numflakes; i++) {
		Ice2Flake f;
		f.x = x + ((((float)(rand() % RAND_MAX)/RAND_MAX) * enemysize) - enemysize/2);
		f.y = 0.0f;
		f.angle = ((float)(rand() % RAND_MAX) / RAND_MAX) * M_PI*2;
		snowflakes.push_back(f);
	}

	numshards = (10 * scale) * (10 * scale);
	shards = new Ice2Shard[numshards];
	verts = new ALLEGRO_VERTEX[numshards*3];

	for (int i = 0; i < numshards; i++) {
		Ice2Shard s;
		s.x = ((((float)(rand() % RAND_MAX)/RAND_MAX) * enemysize) - enemysize/2);
		s.y = ((((float)(rand() % RAND_MAX)/RAND_MAX) * enemysize));
		for (int j = 0; j < 3; j++) {
			s.angles[j] = ((float)(rand() % RAND_MAX) / RAND_MAX) * M_PI*2;
			s.lengths[j] = 5 + rand() % 5;
		}
		s.color = m_map_rgb(
			0,
			150 + rand() % 155,
			255
		);
		float angle = ((float)(rand() % RAND_MAX) / RAND_MAX) * M_PI*2;
		float speed = ICE2SHARD_MIN_SPEED + ((float)(rand() % RAND_MAX) / RAND_MAX) * ICE2SHARD_MIN_SPEED;
		s.dx = cos(angle) * speed;
		s.dy = sin(angle) * speed;
		shards[i] = s;
	}

	shattered = false;
}


Ice2Effect::~Ice2Effect(void)
{
	m_destroy_bitmap(icecube);
	m_destroy_bitmap(snowflake);

	delete[] shards;
	snowflakes.clear();
	delete[] verts;
}


int Fire1Effect::getLifetime(void)
{
	return animSet->getCurrentAnimation()->getLength();
}

void Fire1Effect::draw(void)
{
	int dx = (int)(x - animSet->getWidth()/2);
	int dy = (int)(y - animSet->getHeight());
	animSet->draw(dx, dy, 0);
}


bool Fire1Effect::update(int step)
{
	count += step;

	if (count >= getLifetime()) {
		return true;
	}

	animSet->update(step);

	return false;
}


Fire1Effect::Fire1Effect(Combatant *target) :
	count(0)
{
	type = COMBATENTITY_TYPE_FRILL;
	
	x = target->getX();
	y = target->getY()+1;

	this->target = target;

	animSet = new_AnimationSet(getResource("combat_media/fire1.png"));
}


Fire1Effect::~Fire1Effect(void)
{
	delete animSet;
}


int Darkness1Effect::getLifetime(void)
{
	return 3500;
}

void Darkness1Effect::draw(void)
{
	for (int i = 0; i < NUM_BLOBS; i++) {
		float dx = blobs[i].x;
		float dy = blobs[i].y;
		int half_w = m_get_bitmap_width(bitmaps[0])/2;
		int half_h = m_get_bitmap_height(bitmaps[0])/2;

		m_draw_scaled_rotated_bitmap(bitmaps[blobs[i].bmpIndex],
			half_w, half_h, dx, dy,
			blobs[i].scale, blobs[i].scale,
			-blobs[i].angle, 0);
	}
}


bool Darkness1Effect::update(int step)
{
	count += step;

	if (count >= getLifetime()) {
		return true;
	}

	for (int i = 0; i < NUM_BLOBS; i++) {
		blobs[i].x += blobs[i].dx * step;
		blobs[i].y += blobs[i].dy * step;
		blobs[i].angle += 0.001f * step;
		blobs[i].scale -= blobs[i].ds * step;
		if (blobs[i].scale <= 0.01f) {
			createBlob(blobs, i);
		}
	}

	return false;
}

void Darkness1Effect::createBlob(Darkness1Blob *blobs, int i)
{
	if (target->getLocation() == LOCATION_RIGHT) {
		blobs[i].x = BW-(rand()%50);
		blobs[i].dx = -((rand()%5000/5000.0f)*0.05f);
	}
	else {
		blobs[i].x = rand() % 50;
		blobs[i].dx = (rand()%5000/5000.0f)*0.05f;
	}
	blobs[i].y = 32+rand()%80;
	blobs[i].dy = 0;
	blobs[i].bmpIndex = rand() % 3;
	blobs[i].angle = (rand()%5000/5000.0f)*(M_PI*2);
	blobs[i].scale = 1.0f;
	blobs[i].ds = 0.0005f + 0.0005f * (rand()%5000/5000.0f);
}


Darkness1Effect::Darkness1Effect(Combatant *target) :
	count(0)
{
	type = COMBATENTITY_TYPE_FRILL;
	
	x = 0;
	y = INT_MAX;

	this->target = target;

	for (int i = 0; i < 3; i++) {
		char name[100];
		sprintf(name, "darkness%d.png", i+1);
		bitmaps[i] = m_load_alpha_bitmap(getResource("combat_media/%s", name));
	}

	for (int i = 0; i < NUM_BLOBS; i++) {
		createBlob(blobs, i);
	}
}


Darkness1Effect::~Darkness1Effect(void)
{
	for (int i = 0; i < 3; i++) {
		m_destroy_bitmap(bitmaps[i]);
	}

	loadPlayDestroy("Darkness1.ogg");
}





int WeepEffect::getLifetime(void)
{
	return 2000;
}

void WeepEffect::draw(void)
{
	for (int i = 0; i < NUM_DROPS; i++) {
		MCOLOR color = m_map_rgba(
			drops[i].color.r*drops[i].alpha*255,
			drops[i].color.g*drops[i].alpha*255,
			drops[i].color.b*drops[i].alpha*255,
			drops[i].alpha*255);
		if (drops[i].length <= 0) {
			continue;
		}
		m_draw_line(drops[i].x, 0, drops[i].x, drops[i].length,
			color);
	}
}


bool WeepEffect::update(int step)
{
	count += step;

	if (count >= getLifetime()) {
		return true;
	}

	for (int i = 0; i < NUM_DROPS; i++) {
		if (drops[i].length >= y) {
			drops[i].alpha -= 0.003f * step;
			if (drops[i].alpha < 0) {
				// new drop
				drops[i].x = x + ((rand() % 30) - 15);
				drops[i].length = -(rand() % (int)y);
				drops[i].alpha = 1.0f;
				int r = rand() % 100;
				drops[i].color = m_map_rgb(0, 155+r, 155+r);
			}
		}
		else {
			drops[i].length += 0.3f * step;
			if (drops[i].length > y) {
				drops[i].length = y;
			}
			drops[i].alpha = 1.0f-((drops[i].length/y)*0.5f);
		}
	}

	return false;
}


WeepEffect::WeepEffect(Combatant *target) :
	count(0)
{
	type = COMBATENTITY_TYPE_FRILL;
	
	x = target->getX();
	y = target->getY()+1;

	this->target = target;

	for (int i = 0; i < NUM_DROPS; i++) {
		drops[i].x = x + ((rand() % 30) - 15);
		drops[i].length = -(rand() % (int)y);
		drops[i].alpha = 1.0f;
		int r = rand()%100;
		drops[i].color = m_map_rgb(0, 155+r, 155+r);
	}

	loadPlayDestroy("Weep.ogg");
}


WeepEffect::~WeepEffect(void)
{
}



int WaveEffect::getLifetime(void)
{
	return 2000;
}

void WaveEffect::draw(void)
{
	int drawx;
	int flags;

	if (target->getLocation() == LOCATION_RIGHT) {
		drawx = wx;
		flags = M_FLIP_HORIZONTAL;
	}
	else {
		drawx = wx - m_get_bitmap_width(bitmap);
		flags = 0;
	}

	m_draw_bitmap(bitmap, drawx, wy-m_get_bitmap_height(bitmap), flags);
}


bool WaveEffect::update(int step)
{
	count += step;

	if (count >= getLifetime()) {
		return true;
	}

	wx += step * dx;


	return false;
}


WaveEffect::WaveEffect(Combatant *target) :
	count(0)
{
	type = COMBATENTITY_TYPE_FRILL;
	
	x = 0;
	y = INT_MAX;

	this->target = target;
	
	if (target->getLocation() == LOCATION_RIGHT) {
		wx = BW/3;
		wy = target->getY()+5;
		dx = SPEED;
	}
	else {
		wx = BW/3*2;
		wy = target->getY()+5;
		dx = -SPEED;
	}

	bitmap = m_load_bitmap(getResource("combat_media/Wave.png"));
}


WaveEffect::~WaveEffect(void)
{
	m_destroy_bitmap(bitmap);
}



int WhirlpoolEffect::getLifetime(void)
{
	return 2000;
}

void WhirlpoolEffect::draw(void)
{
	ALLEGRO_TRANSFORM view_backup, t;
	al_copy_transform(&view_backup, al_get_current_transform());

	al_identity_transform(&t);
	al_scale_transform_3d(&t, 1, 0.5f, 1);
	al_use_transform(&t);

	int dx, dy, dw, dh;
	get_screen_offset_size(&dx, &dy, &dw, &dh);

	int spx = target->getX();
	int spy = target->getY();

	int xx = dx + (spx * screenScaleX);
	int yy = (dy + (spy * screenScaleY) - (8*screenScaleY)) * 2; // 8 == depth of water

	quick_draw(
		spiral->bitmap,
		w/2, h/2,
		xx, yy,
		screenScaleX/2, screenScaleY/2,
		angle, 0
	);

	al_use_transform(&view_backup);
}



bool WhirlpoolEffect::update(int step)
{
	count += step;

	if (count >= getLifetime()) {
		return true;
	}

	angle += step * SPEED;

	return false;
}


WhirlpoolEffect::WhirlpoolEffect(Combatant *target) :
	count(0)
{
	type = COMBATENTITY_TYPE_FRILL;
	
	x = 0;
	y = 0;

	angle = 0.0;

	this->target = target;

	spiral = m_load_alpha_bitmap(getResource("combat_media/Whirlpool.png"));
	w = m_get_bitmap_width(spiral);
	h = m_get_bitmap_height(spiral);
}


WhirlpoolEffect::~WhirlpoolEffect(void)
{
	m_destroy_bitmap(spiral);
}


void SlimeEffect::draw(void)
{
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID || defined ALLEGRO_RASPBERRYPI
	m_draw_prim(verts, 0, blob, 0, NUM_BLOBS*6, ALLEGRO_PRIM_TRIANGLE_LIST);
#else
	for (int i = 0; i < NUM_BLOBS; i++) {
		m_draw_circle(
			blobs[i].x,
			blobs[i].y,
			4,
			blobs[i].color,
			M_FILLED);
	}
#endif
}

int SlimeEffect::getLifetime(void)
{
	return 1000;
}


bool SlimeEffect::update(int step)
{
	count += step;

	if (count >= getLifetime())
		return true;

	for (int i = 0; i < NUM_BLOBS; i++) {
		#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID || defined ALLEGRO_RASPBERRYPI
		int j = i*6;
		int end = j + 6;
		for (; j < end; j++) {
			verts[j].x += blobs[i].dx * step;
			verts[j].y += blobs[i].dy * step;
		}
		#else
		blobs[i].x += blobs[i].dx * step;
		blobs[i].y += blobs[i].dy * step;
		#endif
	}

	return false;
}


SlimeEffect::SlimeEffect(Combatant *target) :
	count(0)
{
	this->target = target;

	type = COMBATENTITY_TYPE_FRILL;

	int bx = target->getX();
	int by = target->getY() - target->getAnimationSet()->getHeight()/2;

	x = target->getX();
	y = target->getY()+1;

	for (int i = 0; i < NUM_BLOBS; i++) {
		float a = M_PI*2 * ((float)(rand() % 5000) / 5000);
		blobs[i].x = bx;
		blobs[i].y = by;
		blobs[i].dx = cos(a) * (SPEED + (((float)(rand()%5000) / 5000) * SPEED));
		blobs[i].dy = sin(a) * (SPEED + (((float)(rand()%5000) / 5000) * SPEED));
		int r = 200 + rand() % 55;
		blobs[i].color = m_map_rgb(r, 0, 0);
	}

	#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID || defined ALLEGRO_RASPBERRYPI
	blob = m_create_bitmap(10, 10); // check
	ALLEGRO_BITMAP *t = al_get_target_bitmap();
	m_set_target_bitmap(blob);
	m_clear(m_map_rgba(0, 0, 0, 0));
	al_draw_filled_circle(5, 5, 4, white);
	al_set_target_bitmap(t);

	for (int i = 0; i < NUM_BLOBS; i++) {
		ALLEGRO_COLOR pc = blobs[i].color;
		int j = i*6;
		verts[j].u = 0;
		verts[j].v = 0;
		verts[j].x = blobs[i].x-5;
		verts[j].y = blobs[i].y-5;
		verts[j].z = 0;
		verts[j].color = pc;
		j++;
		verts[j].u = 10;
		verts[j].v = 10;
		verts[j].x = blobs[i].x+5;
		verts[j].y = blobs[i].y+5;
		verts[j].z = 0;
		verts[j].color = pc;
		j++;
		verts[j].u = 0;
		verts[j].v = 10;
		verts[j].x = blobs[i].x-5;
		verts[j].y = blobs[i].y+5;
		verts[j].z = 0;
		verts[j].color = pc;
		j++;
		verts[j].u = 0;
		verts[j].v = 0;
		verts[j].x = blobs[i].x-5;
		verts[j].y = blobs[i].y-5;
		verts[j].z = 0;
		verts[j].color = pc;
		j++;
		verts[j].u = 10;
		verts[j].v = 0;
		verts[j].x = blobs[i].x+5;
		verts[j].y = blobs[i].y-5;
		verts[j].z = 0;
		verts[j].color = pc;
		j++;
		verts[j].u = 10;
		verts[j].v = 10;
		verts[j].x = blobs[i].x+5;
		verts[j].y = blobs[i].y+5;
		verts[j].z = 0;
		verts[j].color = pc;
	}
	#endif
}

SlimeEffect::~SlimeEffect()
{
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID || defined ALLEGRO_RASPBERRYPI
	m_destroy_bitmap(blob);
#endif
}


int Fire2Effect::getLifetime(void)
{
	return 2500;
}

bool Fire2Effect::update(int step)
{
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID || defined ALLEGRO_RASPBERRYPI
	animSet->update(step);
#else
	for (int i = 0; i < 4; i++) {
		animSet->setSubAnimation(i);
		animSet->update(step);
	}
#endif

	count += step;
	if (count >= getLifetime()) {
		return true;
	}

	for (int i = 0; i < numPixels; i++) {
		pixels[i].y += pixels[i].dy * step;
		if (pixels[i].y > target->getAnimationSet()->getHeight()*2) {
			pixels[i].y = 0;
		}
		pixels[i].x = cos((pixels[i].y/y)*3.0f*M_PI*2.0f) * maxDepth;
		pixels[i].z = sin((pixels[i].y/y)*3.0f*M_PI*2.0f) * maxDepth;
	}

	return false;
}


void Fire2Effect::draw(void)
{
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID || defined ALLEGRO_RASPBERRYPI
	int n = 0;
	const int w = animSet->getWidth();
	const int h = animSet->getHeight();
#endif
	
	// draw pixels behind player
	for (int i = 0; i < numPixels; i++) {
		Fire2Pixel *p = &pixels[i];
		if (p->z < 0) {
			continue;
		}
#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_ANDROID
		animSet->setSubAnimation(p->animIndex);
#endif
		int yy;
		int th = target->getAnimationSet()->getHeight();
		if (p->y > th) {
			yy = y-((th*2)-p->y)+p->offset;
		}
		else {
			yy = y-p->y+p->offset;
		}
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID || defined ALLEGRO_RASPBERRYPI
		const int xx = x+p->x;
		verts[n*6].x = xx;
		verts[n*6].y = yy;
		verts[n*6].z = 0;
		verts[n*6+1].x = xx+w;
		verts[n*6+1].y = yy+h;
		verts[n*6+1].z = 0;
		verts[n*6+2].x = xx;
		verts[n*6+2].y = yy+h;
		verts[n*6+2].z = 0;
		verts[n*6+3].x = xx;
		verts[n*6+3].y = yy;
		verts[n*6+3].z = 0;
		verts[n*6+4].x = xx+w;
		verts[n*6+4].y = yy;
		verts[n*6+4].z = 0;
		verts[n*6+5].x = xx+w;
		verts[n*6+5].y = yy+h;
		verts[n*6+5].z = 0;
		n++;
#else
		animSet->draw(x+p->x, yy, 0);
#endif
	}

#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID || defined ALLEGRO_RASPBERRYPI
	m_draw_prim(verts, 0, animSet->getCurrentAnimation()->getCurrentFrame()->getImage()->getBitmap(), 0, n*6, ALLEGRO_PRIM_TRIANGLE_LIST);
	n = 0;
#endif
	
	target->draw();

	// draw pixels in front of player
	for (int i = 0; i < numPixels; i++) {
		Fire2Pixel *p = &pixels[i];
		if (p->z > 0) {
			continue;
		}
#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_ANDROID
		animSet->setSubAnimation(p->animIndex);
#endif
		int yy;
		int th = target->getAnimationSet()->getHeight();
		if (p->y > th) {
			yy = y-((th*2)-p->y)+p->offset;
		}
		else {
			yy = y-p->y+p->offset;
		}
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID || defined ALLEGRO_RASPBERRYPI
		const int xx = x+p->x;
		verts[n*6].x = xx;
		verts[n*6].y = yy;
		verts[n*6].z = 0;
		verts[n*6+1].x = xx+w;
		verts[n*6+1].y = yy+h;
		verts[n*6+1].z = 0;
		verts[n*6+2].x = xx;
		verts[n*6+2].y = yy+h;
		verts[n*6+2].z = 0;
		verts[n*6+3].x = xx;
		verts[n*6+3].y = yy;
		verts[n*6+3].z = 0;
		verts[n*6+4].x = xx+w;
		verts[n*6+4].y = yy;
		verts[n*6+4].z = 0;
		verts[n*6+5].x = xx+w;
		verts[n*6+5].y = yy+h;
		verts[n*6+5].z = 0;
		n++;
#else
		animSet->draw(x+p->x, yy, 0);
#endif
	}
	
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID || defined ALLEGRO_RASPBERRYPI
	m_draw_prim(verts, 0, animSet->getCurrentAnimation()->getCurrentFrame()->getImage()->getBitmap(), 0, n*6, ALLEGRO_PRIM_TRIANGLE_LIST);
#endif
	
}


Fire2Effect::Fire2Effect(Combatant *target)
{
	this->target = target;
	x = target->getX();
	y = target->getY()+1;

	count = 0;

	type = COMBATENTITY_TYPE_FRILL;
		
	int anim_w = target->getAnimationSet()->getWidth();
	int anim_h = target->getAnimationSet()->getHeight();

	numPixels = (int)(NUM_PIXELS * sqrt(anim_w*anim_w + anim_h*anim_h));

	pixels = new Fire2Pixel[numPixels];

	maxDepth = target->getAnimationSet()->getWidth()/2 + 5;

	for (int i = 0; i < numPixels; i++) {
		pixels[i].y = (float)(rand() % RAND_MAX) / RAND_MAX * target->getAnimationSet()->getHeight();
		pixels[i].x = cos((pixels[i].y/y)*3.0f*M_PI*2.0f) * maxDepth;
		pixels[i].z = sin((pixels[i].y/y)*3.0f*M_PI*2.0f) * maxDepth;
		pixels[i].offset = rand() % 5 - 2;
		pixels[i].animIndex = rand() % 4;
		pixels[i].dy = RISE_SPEED;
	}

	animSet = new_AnimationSet(getResource("combat_media/Fire2.png"));
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID || defined ALLEGRO_RASPBERRYPI
	verts = new ALLEGRO_VERTEX[numPixels*6];
	
	const int w = animSet->getWidth();
	const int h = animSet->getHeight();
												 
	 for (int i = 0; i < numPixels; i++) {
		 verts[i*6].u = 0;
		 verts[i*6].v = 0;
		 verts[i*6+1].u = w;
		 verts[i*6+1].v = h;
		 verts[i*6+2].u = 0;
		 verts[i*6+2].v = h;
		 verts[i*6+3].u = 0;
		 verts[i*6+3].v = 0;
		 verts[i*6+4].u = w;
		 verts[i*6+4].v = 0;
		 verts[i*6+5].u = w;
		 verts[i*6+5].v = h;
		 verts[i*6].color = m_map_rgb(255, 255, 255);
		 verts[i*6+1].color = m_map_rgb(255, 255, 255);
		 verts[i*6+2].color = m_map_rgb(255, 255, 255);
		 verts[i*6+3].color = m_map_rgb(255, 255, 255);
		 verts[i*6+4].color = m_map_rgb(255, 255, 255);
		 verts[i*6+5].color = m_map_rgb(255, 255, 255);
	 }
#endif
												 
}


Fire2Effect::~Fire2Effect(void)
{
	delete[] pixels;
	delete animSet;
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID || defined ALLEGRO_RASPBERRYPI
	delete[] verts;
#endif
}



int SludgeFlyEffect::getLifetime(void)
{
	return LIFETIME;
}

bool SludgeFlyEffect::update(int step)
{
	count += step;
	if (count >= getLifetime()) {
		loadPlayDestroy("slime.ogg");
		return true;
	}

	return false;
}


void SludgeFlyEffect::draw(void)
{
	int xx = ((float)count/LIFETIME) * (dest_x - start_x) + start_x;
	int yy = ((float)count/LIFETIME) * (dest_y - start_y) + start_y;

	m_draw_bitmap(bitmap, xx, yy, flags);
}

SludgeFlyEffect::SludgeFlyEffect(Combatant *target, Combatant *caster, std::string bmp_name)
{
	this->target = target;
	this->caster = caster;

	count = 0;

	type = COMBATENTITY_TYPE_FRILL;

	bitmap = m_load_bitmap(getResource("combat_media/%s.png", bmp_name.c_str()));

	start_y = caster->getY()-caster->getAnimationSet()->getHeight()/2-m_get_bitmap_height(bitmap)/2;
	dest_y = target->getY()-target->getAnimationSet()->getHeight()/2-m_get_bitmap_height(bitmap)/2;

	if (caster->getLocation() == LOCATION_LEFT) {
		flags = M_FLIP_HORIZONTAL;
		start_x = caster->getX()+caster->getAnimationSet()->getWidth()/2;
		dest_x = target->getX()-m_get_bitmap_width(bitmap);
	}
	else {
		flags = 0;
		start_x = caster->getX()-caster->getAnimationSet()->getWidth()/2-m_get_bitmap_width(bitmap);
		dest_x = target->getX();
	}
	
	x = target->getX();
	y = target->getY();
}


SludgeFlyEffect::~SludgeFlyEffect(void)
{
	m_destroy_bitmap(bitmap);
}

int SludgeEffect::getLifetime(void)
{
	return LIFETIME;
}

void SludgeEffect::draw()
{
	if (target->getAnimationSet()->getCurrentAnimation()->getName() != anim_name) {
		destroy_info();
		get_info();
	}

	int frame = target->getAnimationSet()->getCurrentAnimation()->getCurrentFrameNum();

	MBITMAP *bmp = target->getAnimationSet()->getCurrentAnimation()->getCurrentFrame()->getImage()->getBitmap();

	int top = m_get_bitmap_height(bmp) * ((float)count / LIFETIME);

	int yy = target->getY()-target->getAnimationSet()->getHeight();

	ALLEGRO_VERTEX *verts = new ALLEGRO_VERTEX[m_get_bitmap_width(bmp)*m_get_bitmap_height(bmp)];
	int vcount = 0;

	for (int i = top; i < m_get_bitmap_height(bmp); i++) {
		int n = i - top;
		int size;
		if (n < FALLOFF_SIZE)
			size = depths[i] * ((float)n / FALLOFF_SIZE);
		else
			size = depths[i];

		int startx, endx, xinc;

		int xx;

		if (left_pixels[frame][i] < 0)
			continue;

		if (target->getLocation() == LOCATION_RIGHT) {
			xx = target->getX() - target->getAnimationSet()->getWidth()/2 + left_pixels[frame][i];
			xinc = 1;
		}
		else {
			xx = target->getX() - target->getAnimationSet()->getWidth()/2 + target->getAnimationSet()->getWidth() - left_pixels[frame][i];
			xinc = -1;
		}

		startx = xx;
		endx = xx + size*xinc;

		verts[vcount].x = startx;
		verts[vcount].y = yy+i+0.5f;
		verts[vcount].z = 0;
		verts[vcount].color = color;
		vcount++;
		verts[vcount].x = endx;
		verts[vcount].y = yy+i+0.5f;
		verts[vcount].z = 0;
		verts[vcount].color = color;
		vcount++;
	}

	m_draw_prim(verts, 0, 0, 0, vcount, ALLEGRO_PRIM_LINE_LIST);

	delete[] verts;

	int puddle_max = m_get_bitmap_width(bmp) * 1.5f;

	int puddle = puddle_max * ((float)count / LIFETIME);

	int x = target->getX();
	int y = target->getY();

	m_draw_line(x-puddle/2, y, x+puddle/2, y, color);
}

bool SludgeEffect::update(int step)
{
	count += step;

	if (count >= LIFETIME)
		return true;

	return false;
}

SludgeEffect::SludgeEffect(Combatant *target, MCOLOR color)
{
	type = COMBATENTITY_TYPE_FRILL;
	this->target = target;
	this->x = target->getX();
	this->y = target->getY()+1;

	this->color = color;

	count = 0;

	int max = target->getAnimationSet()->getWidth()/4;
	int d = (rand() % (max/2)) + max/2;
	
	for (int i = 0; i < target->getAnimationSet()->getHeight(); i++) {
		int r = (rand() % 3 - 1);
		if (d+r >= max/2) d += r;
		depths.push_back(d);
	}

	get_info();
}


SludgeEffect::~SludgeEffect()
{
	depths.clear();
	destroy_info();
}

void SludgeEffect::get_info(void)
{
	Animation *a = target->getAnimationSet()->getCurrentAnimation();
	anim_name = a->getName();
	numFrames = a->getNumFrames();
	left_pixels = new int*[numFrames];
	right_pixels = new int*[numFrames];

	for (int i = 0; i < numFrames; i++) {
		MBITMAP *b = a->getFrame(i)->getImage()->getBitmap();
		m_lock_bitmap(b, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READONLY);
		left_pixels[i] = new int[m_get_bitmap_height(b)];
		right_pixels[i] = new int[m_get_bitmap_height(b)];
		for (int y = 0; y < m_get_bitmap_height(b); y++) {
			int first = INT_MAX;
			int last = 0;
			for (int x = 0; x < m_get_bitmap_width(b); x++) {
				ALLEGRO_COLOR pixel = al_get_pixel(b->bitmap, x, y);
				if (pixel.a > 0.5) {
					if (first == INT_MAX) {
						first = x;
					}
					if (x > last) {
						last = x;
					}
				}
			}
			if (first == INT_MAX) {
				left_pixels[i][y] = -1;
			}
			else {
				left_pixels[i][y] = first;
			}
			if (last == 0) {
				right_pixels[i][y] = -1;
			}
			else {
				right_pixels[i][y] = last;
			}
		}
		m_unlock_bitmap(b);
	}
}

void SludgeEffect::destroy_info(void)
{
	for (int i = 0; i < numFrames; i++) {
		delete[] left_pixels[i];
		delete[] right_pixels[i];
	}
	delete[] left_pixels;
	delete[] right_pixels;
}

int RendEffect::getLifetime(void)
{
	return LIFETIME;
}

void RendEffect::draw(void)
{
	int dx = (int)(target->getX() - (animSet->getWidth()/2));
	int dy = (int)(target->getY() - target->getAnimationSet()->getHeight()/2 - animSet->getHeight()/2);

	animSet->draw(dx, dy, 0);
}


bool RendEffect::update(int step)
{
	count += step;

	if (count >= LIFETIME) {
		return true;
	}

	animSet->update(step);

	return false;
}


RendEffect::RendEffect(Combatant *target) :
	count(0)
{
	type = COMBATENTITY_TYPE_FRILL;
	
	x = target->getX();
	y = target->getY()+1;

	this->target = target;

	animSet = new_AnimationSet(getResource("combat_media/Rend.png"));
	LIFETIME = animSet->getCurrentAnimation()->getLength();
}


RendEffect::~RendEffect(void)
{
	delete animSet;
}


int StompEffect::getLifetime(void)
{
	return LIFETIME;
}

void StompEffect::draw(void)
{
	float a;
	if (count < LIFETIME/2) {
		a = (float)count/(LIFETIME/2) * M_PI/4;
	}
	else {
		a = (float)(count-LIFETIME/2)/(LIFETIME/2) * M_PI/4;
	}

	float x, y;

	if (location == LOCATION_LEFT) {
		if (count < LIFETIME/2) {
			x = BW - (sin(a) * (BW/2-40) + BW/2);
			y = sin(a) * 105 - 105;
		}
		else {
			x = BW - (sin(a) * (BW/2-40) + (BW-40));
			y = -(sin(a) * 105);
		}
		m_draw_bitmap(foot, x-m_get_bitmap_width(foot)/2,  y, M_FLIP_HORIZONTAL);
	}
	else {
		if (count < LIFETIME/2) {
			x = sin(a) * (BW/2-40) + BW/2;
			y = sin(a) * 105 - 105;
		}
		else {
			x = sin(a) * (BW/2-40) + (BW-40);
			y = -(sin(a) * 105);
		}
		m_draw_bitmap(foot, x-m_get_bitmap_width(foot)/2, y, 0);
	}

	if (count >= LIFETIME/2) {
		if (!puffed) {
			puffed = true;
		}
		int index = (float)(count-LIFETIME/2)/(LIFETIME/2) * puffs.size();
		if (index >= (int)puffs.size())
			index = (int)puffs.size()-1;
		for (int i = 0; i < (int)puffData.size(); i++) {
			m_draw_rotated_bitmap(puffs[index], 8, 8, puffData[i].x, puffData[i].y, 0, 0);
		}
	}
}


bool StompEffect::update(int step)
{
	count += step;

	if (count >= LIFETIME) {
		return true;
	}

	if (count >= LIFETIME/2) {
		for (unsigned int i = 0; i < puffData.size(); i++) {
			puffData[i].x += puffData[i].dx * step;
			puffData[i].y += puffData[i].dy * step;
		}
	}

	return false;
}


StompEffect::StompEffect(CombatLocation l) :
	count(0),
	puffed(false)
{
	type = COMBATENTITY_TYPE_FRILL;
	
	x = 0;
	y = 9999;

	location = l;

	foot = m_load_bitmap(getResource("combat_media/Stomp.png"));

	MBITMAP *tmp = m_load_alpha_bitmap(getResource("combat_media/puff.png"));

	m_push_target_bitmap();
	for (int i = 0; i < m_get_bitmap_width(tmp); i += 16) {
		MBITMAP *b = m_create_alpha_bitmap(16, 16);
		m_set_target_bitmap(b);
		m_clear(m_map_rgba(0, 0, 0, 0));
		m_draw_bitmap_region(tmp, i, 0, i+16, 16, 0, 0, 0);
		puffs.push_back(b);
	}
	m_pop_target_bitmap();

	m_destroy_bitmap(tmp);

	for (int i = 0; i < 40; i++) {
		StompPuff p;
		int r = (rand() % 80);
		p.x = (location == LOCATION_LEFT ? 40 : BW-40) + (r-40);
		p.y = 105;
		float a = M_PI - ((r/80.0f) * M_PI);
		float speed = ((float)(rand() % RAND_MAX) / RAND_MAX) * 0.15f;
		p.dx = cos(a) * speed;
		p.dy = sin(-a) * speed;
		puffData.push_back(p);
	}
}


StompEffect::~StompEffect(void)
{
	m_destroy_bitmap(foot);
	for (int i = 0; i < (int)puffs.size(); i++) {
		m_destroy_bitmap(puffs[i]);
	}
	puffs.clear();
	puffData.clear();
}


int SprayEffect::getLifetime(void)
{
	return LIFETIME;
}

void SprayEffect::draw(void)
{
	al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_ALPHA);
	
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID || defined ALLEGRO_RASPBERRYPI
	int count = 0;
	for (int yy = 0; yy < HEIGHT; yy++) {
		int dx = lx+ox;
		for (int l = 0; l < (int)points[yy].size(); l++) {
			SprayPoint &p = points[yy][l];
			if (dx+p.length >= rx) {
				int l = rx - dx;
				dx = lx;
				l = p.length-l;
				dx += l;
				count += 2;
			}
			else {
				dx += p.length;
				count++;
			}
		}
	}
	ALLEGRO_VERTEX verts[count*2];

	for (int i = 0; i < count*2; i++)
		verts[i].z = 0;

	count = 0;
#endif

	for (int y = 0, yy = cy-HEIGHT/2; y < HEIGHT; y++, yy++) {
		int dx = lx+ox;
		for (int l = 0; l < (int)points[y].size(); l++) {
			SprayPoint &p = points[y][l];
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID || defined ALLEGRO_RASPBERRYPI
			if (dx+p.length >= rx) {
				int l = rx - dx;
				verts[count].x = dx;
				verts[count].y = yy;
				verts[count].color = p.color;
				count++;
				verts[count].x = dx+l;
				verts[count].y = yy;
				verts[count].color = p.color;
				count++;
				dx = lx;
				l = p.length-l;
				verts[count].x = dx;
				verts[count].y = yy;
				verts[count].color = p.color;
				count++;
				verts[count].x = dx+l;
				verts[count].y = yy;
				verts[count].color = p.color;
				count++;
				dx += l;
			}
			else {
				verts[count].x = dx;
				verts[count].y = yy;
				verts[count].color = p.color;
				count++;
				verts[count].x = dx+p.length;
				verts[count].y = yy;
				verts[count].color = p.color;
				count++;
				dx += p.length;
			}
#else
			if (dx+p.length >= rx) {
				int l = rx - dx;
				m_draw_line(dx, yy, dx+l, yy, p.color);
				dx = lx;
				l = p.length-l;
				m_draw_line(dx, yy, dx+l, yy, p.color);
				dx += l;
			}
			else {
				m_draw_line(dx, yy, dx+p.length, yy, p.color);
				dx += p.length;
			}
#endif
		}
	}

#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID || defined ALLEGRO_RASPBERRYPI
	m_draw_prim(verts, 0, 0, 0, count, ALLEGRO_PRIM_LINE_LIST);
#endif
	
	al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA);
}


bool SprayEffect::update(int step)
{
	count += step;

	if (count >= LIFETIME) {
		return true;
	}

	if (target->getLocation() == LOCATION_RIGHT) {
		ox += 0.5f * step;

		if (ox >= (rx-lx)) {
			ox -= (rx-lx);
		}
	}
	else {
		ox -= 0.5f * step;

		if (ox < 0) {
			ox += (rx-lx);
		}
	}

	return false;
}


SprayEffect::SprayEffect(Combatant *c) :
	count(0)
{
	type = COMBATENTITY_TYPE_FRILL;
	
	x = c->getX();
	y = c->getY()+1;

	ox = 0;

	target = c;

	cy = target->getY()-target->getAnimationSet()->getHeight()/2;

	if (target->getLocation() == LOCATION_LEFT) {
		lx = target->getX();
		rx = BW/2;
	}
	else {
		lx = BW/2;
		rx = target->getX();
	}

	int total_len = rx - lx;

	for (int i = 0; i < HEIGHT; i++) {
		std::vector<SprayPoint> v;
		int x = 0;
		int total = 0;
		while (1) {
			int length;
			if (total + MAXLEN >= total_len)
				length = total_len - total;
			else
				length = (rand() % ((MINLEN-1) + (MAXLEN-MINLEN))) + 1;
			SprayPoint p;
			//p.x = x;
			p.length = length;
			int c;
			if (i < HEIGHT/2)
				c = ((float)i/(HEIGHT/2))*(rand() % (TOP-MIDDLE) + MIDDLE) + rand() % 10;
			else
				c = ((float)((HEIGHT/2)-(i-(HEIGHT/2)))/(HEIGHT/2))*(rand() % (TOP-MIDDLE) + MIDDLE) + rand() % 10;
			p.color = m_map_rgb(c, c, c);
			v.push_back(p);
			x += length;
			total += length;
			if (total >= total_len)
				break;
		}
		points.push_back(v);
	}

	loadPlayDestroy("Spray.ogg");
}


SprayEffect::~SprayEffect(void)
{
	for (unsigned int i = 0; i < points.size(); i++) {
		points[i].clear();
	}
	points.clear();
}


int PunchEffect::getLifetime(void)
{
	return LIFETIME;
}

void PunchEffect::draw(void)
{
	animSet->draw(x-animSet->getWidth()/2, y-animSet->getHeight()/2, flags);
}


bool PunchEffect::update(int step)
{
	count += step;

	if (count >= LIFETIME) {
		return true;
	}

	float r = (float)count / LIFETIME;

	if (r > 0.8) {
		animSet->setSubAnimation("closed");
	}

	x = ((dx - sx) * r) + sx;
	y = ((dy - sy) * r) + sy;

	return false;
}


PunchEffect::PunchEffect(Combatant *target, Combatant *caster) :
	count(0)
{
	type = COMBATENTITY_TYPE_FRILL;

	sx = caster->getX();
	sy = caster->getY()-caster->getAnimationSet()->getHeight()/2;
	dx = target->getX();
	dy = target->getY()-target->getAnimationSet()->getHeight()/2;
	if (target->getLocation() == LOCATION_LEFT)
		flags = M_FLIP_HORIZONTAL;
	else
		flags = 0;
	
	x = sx;
	y = sy;

	animSet = new_AnimationSet(getResource("combat_media/Punch.png"));
}


PunchEffect::~PunchEffect(void)
{
	delete animSet;
}


int TorrentEffect::getLifetime(void)
{
	return LIFETIME;
}


void TorrentEffect::draw(void)
{
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID || defined ALLEGRO_RASPBERRYPI
	ALLEGRO_VERTEX verts[NUM_DROPS*2];
	
	for (int i = 0; i < NUM_DROPS*2; i++)
		verts[i].z = 0;
#endif

	for (int i = 0; i < NUM_DROPS; i++) {
		float tx, ty;
		float angle;
		if (loc == LOCATION_LEFT)
			angle = D2R(300);
		else
			angle = D2R(240);
		const int len = 20;
		tx = drops[i].x + (cos(angle) * len);
		ty = drops[i].y + (sin(angle) * len);
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID || defined ALLEGRO_RASPBERRYPI
		verts[i*2].x = drops[i].x;
		verts[i*2].y = drops[i].y;
		float a = drops[i].color.a;
		verts[i*2].color = al_map_rgba_f(
			drops[i].color.r*a,
			drops[i].color.g*a,
			drops[i].color.b*a,
			a
		);
		verts[i*2+1].x = tx;
		verts[i*2+1].y = ty;
		verts[i*2+1].color = al_map_rgba_f(
			drops[i].color.r*a,
			drops[i].color.g*a,
			drops[i].color.b*a,
			a
		);
#else
		ALLEGRO_COLOR c = drops[i].color;
		c.r *= c.a;
		c.g *= c.a;
		c.b *= c.a;
		m_draw_line(drops[i].x, drops[i].y, tx, ty, c);
#endif
	}

#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID || defined ALLEGRO_RASPBERRYPI
	m_draw_prim(verts, 0, 0, 0, NUM_DROPS*2, ALLEGRO_PRIM_LINE_LIST);
#endif
}


bool TorrentEffect::update(int step)
{
	count += step;

	if (count >= LIFETIME) {
		return true;
	}

	for (int i = 0; i < NUM_DROPS; i++) {
		float angle;
		if (loc == LOCATION_LEFT)
			angle = D2R(120);
		else
			angle = D2R(60);
		drops[i].x += cos(angle) * (drops[i].speed * step);
		drops[i].y += sin(angle) * (drops[i].speed * step);
		if (drops[i].y > BH) {
			drops[i].x = drops[i].start_x;
			drops[i].y = drops[i].start_y;
		}
	}

	return false;
}


TorrentEffect::TorrentEffect(Combatant *caster) :
	count(0)
{
	type = COMBATENTITY_TYPE_FRILL;

	x = 0;
	y = 9999;

	loc = caster->getLocation() == LOCATION_LEFT ?
		LOCATION_RIGHT : LOCATION_LEFT;

	for (int i = 0; i < NUM_DROPS; i++) {
		int x = rand() % (BW*2/3);
		if (loc == LOCATION_RIGHT)
			x += BW/3;
		int y = -(rand() % 100);
		drops[i].start_x = x;
		drops[i].start_y = y;
		drops[i].x = x;
		drops[i].y = y;
		float r;
		r = (float)(rand() % RAND_MAX) / RAND_MAX;
		drops[i].speed = 0.2*r + 0.2;
		r = (float)(rand() % RAND_MAX) / RAND_MAX;
		float r2 = (float)(rand() % RAND_MAX) / RAND_MAX;
		drops[i].color = m_map_rgba(220+r*35, 220+r*35, 255, 50+r2*150);
	}
}


TorrentEffect::~TorrentEffect(void)
{
}


static float interpolate(float f1, float f2, float p)
{
	return (f1 * (1 - p)) + (f2 * p);
}


int BeamP1Effect::getLifetime(void)
{
	return LIFETIME;
}

void BeamP1Effect::draw(void)
{
	float distx = dx - sx;
	float disty = dy - sy;
	float angle = atan2(disty, distx);
	float xinc = cos(angle+M_PI/2);
	float yinc = sin(angle+M_PI/2);

	MCOLOR color = m_map_rgb(255, 0, 0);

	color.a = 0.2f;
	
#define tmp NULL

	m_draw_precise_line(tmp, sx, sy, dx, dy, color);
	
	for (float i = 0.333f; i < 5.0f/2; i += 0.333f) {
		color.a = interpolate(0.2f, 0.0f, (i/(5.0f/2)));
		float xx1 = sx + xinc * i;
		float yy1 = sy + yinc * i;
		float xx2 = dx + xinc * i;
		float yy2 = dy + yinc * i;
		m_draw_precise_line(tmp, xx1, yy1, xx2, yy2, color);
		xx1 = sx - xinc * i;
		yy1 = sy - yinc * i;
		xx2 = dx - xinc * i;
		yy2 = dy - yinc * i;
		m_draw_precise_line(tmp, xx1, yy1, xx2, yy2, color);
	}

#undef tmp
}


bool BeamP1Effect::update(int step)
{
	count += step;

	if (count >= LIFETIME) {
		return true;
	}

	return false;
}


BeamP1Effect::BeamP1Effect(Combatant *target, Combatant *caster) :
	count(0)
{
	type = COMBATENTITY_TYPE_FRILL;

	CombatLocation loc = caster->getLocation();

	sx = caster->getX() + ((loc == LOCATION_LEFT) ? 10 : -10);
	sy = caster->getY()-40;
	dx = target->getX();
	dy = target->getY()-target->getAnimationSet()->getHeight()/2;

	float distx = dx - sx;
	float disty = dy - sy;
	float dist = sqrt(distx*distx+disty*disty);
	float angle = atan2(disty, distx);

	dx = sx + cos(angle) * (dist/2+0.5f);
	dy = sy + sin(angle) * (dist/2+0.5f);

	x = 0;
	y = caster->getY()+1;
}


BeamP1Effect::~BeamP1Effect(void)
{
}



int BeamP2Effect::getLifetime(void)
{
	return LIFETIME;
}

void BeamP2Effect::draw(void)
{
	float distx = dx - sx;
	float disty = dy - sy;
	float angle = atan2(disty, distx);
	float xinc = cos(angle+M_PI/2);
	float yinc = sin(angle+M_PI/2);

	MCOLOR color = m_map_rgb(255, 0, 0);

	color.a = 0.2f;

#define tmp NULL

	m_draw_precise_line(tmp, sx, sy, dx, dy, color);
	
	for (float i = 0.333f; i < 5.0f/2; i += 0.333f) {
		color.a = interpolate(0.2f, 0.0f, (i/(5.0f/2)));
		float xx1 = sx + xinc * i;
		float yy1 = sy + yinc * i;
		float xx2 = dx + xinc * i;
		float yy2 = dy + yinc * i;
		m_draw_precise_line(tmp, xx1, yy1, xx2, yy2, color);
		xx1 = sx - xinc * i;
		yy1 = sy - yinc * i;
		xx2 = dx - xinc * i;
		yy2 = dy - yinc * i;
		m_draw_precise_line(tmp, xx1, yy1, xx2, yy2, color);
	}

#undef tmp
	
	for (int i = 0; i < (int)puffs.size(); i++) {
		m_draw_alpha_bitmap(smoke,
			puffs[i].x-m_get_bitmap_width(smoke)/2,
			puffs[i].y-m_get_bitmap_height(smoke)/2);
	}

}


bool BeamP2Effect::update(int step)
{
	count += step;

	if (count >= LIFETIME) {
		return true;
	}

	if ((count < LIFETIME/2) && (int)puffs.size() < (count/100)) {
		LaserSmoke p;
		p.x = dx + ((rand() % 15) - 7);
		p.y = dy + ((rand() % 15) - 7);
		p.life = 400;
		puffs.push_back(p);
	}

	std::vector<LaserSmoke>::iterator it;

	for (it = puffs.begin(); it != puffs.end();) {
		LaserSmoke &puff = *it;
		puff.y -= 0.05 * step;
		puff.x += 0.05 * step * (rand() % 3 - 1);
		puff.life -= step;
		if (puff.life <= 0) {
			it = puffs.erase(it);
		}
		else {
			it++;
		}
	}

	return false;
}


BeamP2Effect::BeamP2Effect(Combatant *target, Combatant *caster) :
	count(0)
{
	type = COMBATENTITY_TYPE_FRILL;

	CombatLocation loc = caster->getLocation();

	sx = caster->getX() + ((loc == LOCATION_LEFT) ? 10 : -10);
	sy = caster->getY()-40;
	dx = target->getX();
	dy = target->getY()-target->getAnimationSet()->getHeight()/2;

	float distx = dx - sx;
	float disty = dy - sy;
	float dist = sqrt(distx*distx+disty*disty);
	float angle = atan2(disty, distx) + M_PI;
 
	sx = dx + cos(angle) * (dist/2);
	sy = dy + sin(angle) * (dist/2);

	x = 0;
	y = target->getY()+1;

	smoke = m_load_alpha_bitmap(getResource("media/smoke.png"));

	puffCount = 0;

}


BeamP2Effect::~BeamP2Effect(void)
{
	m_destroy_bitmap(smoke);
	puffs.clear();
}


int Fire3Effect::getLifetime(void)
{
	return LIFETIME;
}


void Fire3Effect::draw(void)
{
	int sy;
	int dest;

	if (offs < H) {
		sy = 0;
		dest = offs;
	}
	else {
		sy = offs - H;
		dest = H;
	}

	int min = MIN(H, offs);
	ALLEGRO_VERTEX *verts = new ALLEGRO_VERTEX[min * 2];

	for (int y = 0; y < min; y++) {
		float alpha = (float)y/min;
		MCOLOR c = m_map_rgba(alpha*255, alpha*255, alpha*255, alpha*255);
		verts[y*2+0].x = dx+0.5;
		verts[y*2+0].y = dy-dest+y+0.5;
		verts[y*2+0].z = 0;
		verts[y*2+0].color = c;
		verts[y*2+0].u = 0;
		verts[y*2+0].v = y+sy;
		verts[y*2+1].x = dx+W+0.5;
		verts[y*2+1].y = dy-dest+y+0.5;
		verts[y*2+1].z = 0;
		verts[y*2+1].color = c;
		verts[y*2+1].u = W;
		verts[y*2+1].v = y+sy;
	}

	m_draw_prim(verts, NULL, fire_bmp, 0, min*2, ALLEGRO_PRIM_LINE_LIST);

	delete[] verts;
}


bool Fire3Effect::update(int step)
{
	count += step;

	if (count >= LIFETIME) {
		return true;
	}

	offs = (float)count/LIFETIME * m_get_bitmap_height(fire_bmp);

	return false;
}


Fire3Effect::Fire3Effect(Combatant *target) :
	count(0)
{
	type = COMBATENTITY_TYPE_FRILL;
	
	fire_bmp = m_load_alpha_bitmap(getResource("combat_media/Fire3.png"));

	dx = target->getX()-m_get_bitmap_width(fire_bmp)/2;
	dy = target->getY();
	
	x = dx;
	y = target->getY()+1;

	offs = 0;
}


Fire3Effect::~Fire3Effect(void)
{
	m_destroy_bitmap(fire_bmp);
}




int Ice3Effect::getLifetime(void)
{
	return LIFETIME;
}


void Ice3Effect::draw(void)
{
	if (count < 1000) {
		m_draw_bitmap(icicle, x-m_get_bitmap_width(icicle)/2, yy-m_get_bitmap_height(icicle), 0);
	}
	else {
		int cx, cy, cw, ch;
		al_get_clipping_rectangle(&cx, &cy, &cw, &ch);
		m_set_clip(
			x-m_get_bitmap_width(icicle)/2, y-m_get_bitmap_height(icicle),
			x-m_get_bitmap_width(icicle)/2+m_get_bitmap_width(icicle), y
		);
		m_draw_trans_bitmap(icicle, x-m_get_bitmap_width(icicle)/2, y-m_get_bitmap_height(icicle)+BELOW_GROUND-(dy-yy), alpha*255);
		al_set_clipping_rectangle(cx, cy, cw, ch);

		ALLEGRO_VERTEX verts[NUM_PARTICLES];
		for (int i = 0; i < NUM_PARTICLES; i++) {
			particles[i].color.a = alpha;
			ALLEGRO_COLOR c = particles[i].color;
			verts[i].x = particles[i].x;
			verts[i].y = particles[i].y;
			verts[i].z = 0;
			verts[i].color = al_map_rgba_f(c.r*c.a, c.g*c.a, c.b*c.a, c.a);
		}
		m_draw_prim(verts, NULL, NULL, 0, NUM_PARTICLES, ALLEGRO_PRIM_POINT_LIST);
	}
}


bool Ice3Effect::update(int step)
{
	count += step;

	if (count >= LIFETIME) {
		return true;
	}

	if (count < 1000) {
		yy = sy + ((float)count/1000.0f * (dy - sy - BELOW_GROUND));
	}
	else {
		yy = y + (float)(count-1000)/500.0f * (BELOW_GROUND);
		alpha = 1.0f - ((float)(count-1000)/500.0f);
		for (int i = 0; i < NUM_PARTICLES; i++) {
			if (updateParticle(particles[i], step))
				genparticle(i);
		}
	}

	return false;
}

void Ice3Effect::genparticle(int i)
{
	Particle &p = particles[i];
	p.x = x + (rand() % 40 - 20);
	p.y = y-1-rand()%20;
	p.dx = (rand() % 3 - 1) * 0.3f;
	p.dy = -((float)(rand()%RAND_MAX)/RAND_MAX * 0.3f);
	p.ground = y;
	p.color = white;
}


Ice3Effect::Ice3Effect(Combatant *target) :
	count(0)
{
	type = COMBATENTITY_TYPE_FRILL;

	icicle = m_load_bitmap(getResource("combat_media/Ice3.png"));
	
	yy = sy = target->getY() - 200;
	dy = target->getY() + BELOW_GROUND;
	
	x = target->getX();
	y = target->getY()+1;

	alpha = 1.0f;

	// generate particles
	for (int i = 0; i < NUM_PARTICLES; i++) {
		genparticle(i);
	}
}


Ice3Effect::~Ice3Effect(void)
{
	m_destroy_bitmap(icicle);
}

static void draw_bolt_arc_worker1(MPoint &p1, MPoint &p2, MBITMAP *bmp)
{
	float distx = p2.x - p1.x;
	float disty = p2.y - p1.y;
	float dist = sqrt(distx*distx + disty*disty);
	float amplitude = dist / 16.0f;

	ALLEGRO_VERTEX verts[11*6];
	int vcount = 0;

	for (float p = 0.0f; p < 1.0f; p += 0.1f) {
		float angle = M_PI*2*p*2;
		float x = interpolate(p1.x, p2.x, p);
		float y = interpolate(p1.y, p2.y, p) - sin(angle)*amplitude;
		int w = m_get_bitmap_width(bmp);
		int h = m_get_bitmap_height(bmp);
		float xx = x-w/2.0;
		float yy = y-h/2.0;
		verts[vcount].x = xx;
		verts[vcount].y = yy;
		verts[vcount].z = 0;
		verts[vcount].color = white;
		verts[vcount].u = 0;
		verts[vcount].v = 0;
		vcount++;
		verts[vcount].x = xx+w;
		verts[vcount].y = yy;
		verts[vcount].z = 0;
		verts[vcount].color = white;
		verts[vcount].u = w;
		verts[vcount].v = 0;
		vcount++;
		verts[vcount].x = xx;
		verts[vcount].y = yy+h;
		verts[vcount].z = 0;
		verts[vcount].color = white;
		verts[vcount].u = 0;
		verts[vcount].v = h;
		vcount++;
		verts[vcount].x = xx;
		verts[vcount].y = yy+h;
		verts[vcount].z = 0;
		verts[vcount].color = white;
		verts[vcount].u = 0;
		verts[vcount].v = h;
		vcount++;
		verts[vcount].x = xx+w;
		verts[vcount].y = yy;
		verts[vcount].z = 0;
		verts[vcount].color = white;
		verts[vcount].u = w;
		verts[vcount].v = 0;
		vcount++;
		verts[vcount].x = xx+w;
		verts[vcount].y = yy+h;
		verts[vcount].z = 0;
		verts[vcount].color = white;
		verts[vcount].u = w;
		verts[vcount].v = h;
		vcount++;
	}

	m_draw_prim(verts, 0, bmp, 0, vcount, ALLEGRO_PRIM_TRIANGLE_LIST);
}

static void draw_bolt_arc_worker2(MPoint &p1, MPoint &p2)
{
	float distx = p2.x - p1.x;
	float disty = p2.y - p1.y;
	float dist = sqrt(distx*distx + disty*disty);
	float amplitude = dist / 16.0f;

	for (float p = 0.0f; p < 1.0f; p += 0.03f) {
		float angle = M_PI*2*p*2;
		float x = interpolate(p1.x, p2.x, p);
		float y = interpolate(p1.y, p2.y, p) - sin(angle)*amplitude;
#ifdef ALLEGRO_WINDOWS
		if (al_get_display_flags(display) & ALLEGRO_DIRECT3D) {
			m_draw_pixel(x+0.5, y+0.5, white);
		}
		else
#endif
		{
			m_draw_pixel(x, y, white);
		}
	}
}

static void draw_bolt_arc(std::vector<MPoint> &pts, int stage, MBITMAP *bmp)
{
	if (stage == 0) {
		for (int i = 0; i < (int)pts.size()-1; i++) {
			draw_bolt_arc_worker1(pts[i], pts[i+1], bmp);
		}
	}
	else {
		for (int i = 0; i < (int)pts.size()-1; i++) {
			draw_bolt_arc_worker2(pts[i], pts[i+1]);
		}
	}
}



int Bolt3Effect::getLifetime(void)
{
	return LIFETIME;
}


void Bolt3Effect::draw(void)
{
	int w = m_get_bitmap_width(ring);
	m_draw_scaled_bitmap(ring, 0, 0,
		w, w,
		cx-circ_rad, cy-circ_rad, circ_rad*2, circ_rad*2, 0, 255);

	const int hw = m_get_bitmap_width(predrawn)/2;
	const int hh = m_get_bitmap_height(predrawn)/2;
	m_draw_rotated_bitmap(predrawn,
		hw, hh,
		cx,
		cy,
		angle,
		0);
}


bool Bolt3Effect::update(int step)
{
	count += step;

	if (count >= LIFETIME) {
		return true;
	}

	circ_rad += 0.1f * step;
	if (circ_rad > MAX_RAD)
		circ_rad -= MAX_RAD;
	
	angle += 0.003f * step;

	// move around in circle
	for (unsigned int i = 0; i < arcs.size(); i++) {
		std::vector<MPoint> &pts = arcs[i];
		for (unsigned int j = 0; j < pts.size(); j++) {
			MPoint &p = pts[j];
			float dx = p.x - cx;
			float dy = p.y - cy;
			float dist = sqrt(dx*dx + dy*dy);
			float angle = atan2(dy, dx);
			angle += 0.003f * step;
			p.x = cx + cos(angle)*dist;
			p.y = cy + sin(angle)*dist;
		}
	}

	// move points back and forth
	for (unsigned int i = 0; i < arcs.size(); i++) {
		std::vector<MPoint> &pts = arcs[i];
		for (unsigned int j = 1; j < pts.size(); j++) {
			MPoint &p = pts[j];
			float dx = p.x - cx;
			float dy = p.y - cy;
			float dist = sqrt(dx*dx + dy*dy);
			float angle = atan2(dy, dx);
			angle += ((rand()%3-1) * M_PI/2) * cos(amp) * 0.001f * step;
			p.x = cx + cos(angle)*dist;
			p.y = cy + sin(angle)*dist + (rand()%3-1)*0.01f*step;
		}
	}

	amp += 0.001f * step;

	return false;
}

void Bolt3Effect::generate(int n)
{
	float angle = (float)(rand()%RAND_MAX)/RAND_MAX * M_PI * 2;
	for (int i = 0; i < n; i++) {
		std::vector<MPoint> pts;
		float offset = 0;
		int ppa = PTS_PER_ARC + rand() % 3;
		int dir = rand() % 2;
		if (dir == 0) dir = -1;
		for (int i = 0; i < ppa; i++) {
			float aoffs = (float)(rand()%RAND_MAX)/RAND_MAX * 0.12f + 0.12f;
			aoffs *= dir;
			dir = -dir;
			float x = cx + cos(angle+aoffs)*offset;
			float y = cy + sin(angle+aoffs)*offset;
			MPoint p;
			p.x = x;
			p.y = y;
			pts.push_back(p);
			offset += MAX_DIST/ppa * ((float)(rand()%RAND_MAX)/RAND_MAX*0.5+0.75);
		}
		arcs.push_back(pts);
		angle += (M_PI*2)/n;
	}
}

void Bolt3Effect::draw_arcs(std::vector< std::vector<MPoint> > &arcs)
{
	for (unsigned int i = 0; i < arcs.size(); i++) {
		draw_bolt_arc(arcs[i], 0, alpha_sprite);
	}
	for (unsigned int i = 0; i < arcs.size(); i++) {
		draw_bolt_arc(arcs[i], 1, NULL);
	}
}

Bolt3Effect::Bolt3Effect(Combatant *target) :
	count(0)
{
	type = COMBATENTITY_TYPE_FRILL;

	alpha_sprite = m_load_alpha_bitmap(getResource("combat_media/Bolt3_alpha.png"));

	ring = m_load_bitmap(getResource("media/ring.png"));

	x = target->getX();
	y = target->getY()+1;

	amp_dir = 1;
	amp = 0.0f;
	circ_rad = 0.0f;

	cx = MAX_DIST;
	cy = MAX_DIST;
	predrawn = m_create_alpha_bitmap(MAX_DIST*2, MAX_DIST*2); // check

	generate(rand()%2+4);
	
	cx = x;
	cy = y - target->getAnimationSet()->getHeight()/2;
	ALLEGRO_BITMAP *old_target = al_get_target_bitmap();
	m_set_target_bitmap(predrawn);
	m_clear(m_map_rgba(0, 0, 0, 0));
	draw_arcs(arcs);
	al_set_target_bitmap(old_target);
	angle = 0;
}


Bolt3Effect::~Bolt3Effect(void)
{
	m_destroy_bitmap(alpha_sprite);
	m_destroy_bitmap(ring);
	for (unsigned int i = 0; i < arcs.size(); i++) {
		arcs[i].clear();
	}
	arcs.clear();
	m_destroy_bitmap(predrawn);
}


int Darkness2Effect::getLifetime(void)
{
	return LIFETIME;
}

void Darkness2Effect::draw(void)
{
	float amplitude;

	amplitude = sin((float)count/LIFETIME*M_PI*6) * BW / 4;;

	for (int y = 0; y < 110; y++) {
		float r = (float)y/110 * M_PI * 2;
		float len = sin(r+f) * amplitude;
		len += cos(r+f*2) * amplitude;
		if (loc == LOCATION_LEFT) {
			m_draw_line(0, y, len, y, black);
		}
		else {
			m_draw_line(BW, y, BW-len, y, black);
		}
	}
}


bool Darkness2Effect::update(int step)
{
	count += step;

	if (count >= getLifetime()) {
		return true;
	}

	f += 0.005f * step;

	return false;
}

Darkness2Effect::Darkness2Effect(CombatLocation loc) :
	count(0),
	loc(loc)
{
	type = COMBATENTITY_TYPE_FRILL;
	
	x = 0;
	y = INT_MAX;

	loadPlayDestroy("Darkness2.ogg");

	f = 0.0f;
}


Darkness2Effect::~Darkness2Effect(void)
{
}



int TwisterEffect::getLifetime(void)
{
	return LIFETIME;
}

void TwisterEffect::draw(void)
{
	int w = m_get_bitmap_width(bmp);
	int h = m_get_bitmap_height(bmp);
	float fl = ((float)count / getLifetime()) * (M_PI*6);
	int dx = x - w/2;
	int dy = y - h;
	quick(true);
	for (int i = 0; i < h; i++, dy++) {
		float f = fl + (((float)i/h) * (M_PI*2));
		int new_dx = dx + (sin(f)*6);
		m_draw_bitmap_region(bmp,
			0, i, w, 1, new_dx, dy, 0);
	}
	quick(false);
}


bool TwisterEffect::update(int step)
{
	count += step;

	if (count >= getLifetime()) {
		return true;
	}

	rotate_count += step;

	f += 0.008f * step;

	return false;
}

TwisterEffect::TwisterEffect(Combatant *target) :
	count(0),
	target(target)
{
	type = COMBATENTITY_TYPE_FRILL;
	
	x = target->getX();
	y = target->getY()+1;

#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID || defined ALLEGRO_RASPBERRYPI
	bmp = m_load_alpha_bitmap(getResource("combat_media/Twister.png"));
#else
	bmp = m_load_bitmap(getResource("combat_media/Twister.png"));
#endif

	rotate_count = 0;
	f = 0;
}


TwisterEffect::~TwisterEffect(void)
{
	m_destroy_bitmap(bmp);
}





int WhipEffect::getLifetime(void)
{
	return LIFETIME;
}

void WhipEffect::draw(void)
{
	ALLEGRO_VERTEX *verts = new ALLEGRO_VERTEX[POINTS];
	for (int i = 0; i < POINTS; i++) {
		verts[i].x = curr[i].x;
		verts[i].y = curr[i].y;
		verts[i].z = 0;
		verts[i].color = curr[i].color;
	}
	m_draw_prim(verts, 0, 0, 0, POINTS, ALLEGRO_PRIM_POINT_LIST);
	delete[] verts;
}


bool WhipEffect::update(int step)
{
	count += step;

	if (count >= getLifetime()) {
		return true;
	}

	for (int i = 0; i < POINTS; i++) {
		float p = (float)count/LIFETIME;
		p += 1-(float)i/POINTS;
		if (p > 1) p = 1;
		curr[i].x = (pts2[i].x-pts1[i].x)*p+pts1[i].x;
		curr[i].y = (pts2[i].y-pts1[i].y)*p+pts1[i].y;
	}

	return false;
}

WhipEffect::WhipEffect(Combatant *caster, Combatant *target) :
	count(0)
{
	type = COMBATENTITY_TYPE_FRILL;

	x = 0;
	y = target->getY()+1;

	int cy = caster->getY()-caster->getAnimationSet()->getHeight()/2;
	int cx = caster->getX();
	int ty = target->getY()-target->getAnimationSet()->getHeight()/2;
	int tx = target->getX();

	for (int i = 0; i < POINTS; i++) {
		float p = (float)i/POINTS;
		float a = p*M_PI*2;
		pts1[i].x = cx+sin(a)*25;
		pts1[i].y = cy-75*p;
		curr[i].x = pts1[i].x;
		curr[i].y = pts1[i].y;
		pts2[i].x = (tx-cx)*p+cx;
		pts2[i].y = (ty-cy)*p+cy;
		int c = rand() % 5;
		if (c == 0)
			curr[i].color = m_map_rgb(150, 80, 10);
		else if (c == 1)
			curr[i].color = m_map_rgb(140, 200, 30);
		else
			curr[i].color = m_map_rgb(200, 120, 20);
	}
}


WhipEffect::~WhipEffect(void)
{
}


int AcornsEffect::getLifetime(void)
{
	return LIFETIME;
}

void AcornsEffect::draw(void)
{
	for (int i = 0; i < ACORNS; i++) {
		if (acorns[i].y >= target->getY()) {
			for (int j = 0; j < CIRCLES; j++) {
				int r = explosions[i][j].radius * ((float)(explosions[i][j].count%500)/500);
				m_draw_circle(explosions[i][j].x, explosions[i][j].y,
					r, explosions[i][j].color, M_FILLED);
			}
		}
		else {
			m_draw_rotated_bitmap(bitmap,
				m_get_bitmap_width(bitmap)/2,
				m_get_bitmap_height(bitmap)/2,
				acorns[i].x, acorns[i].y,
				acorns[i].angle, 0);
		}
	}
}


bool AcornsEffect::update(int step)
{
	count += step;

	if (count >= getLifetime()) {
		return true;
	}

	for (int i = 0; i < ACORNS; i++) {
		if (acorns[i].y >= target->getY()) {
			for (int j = 0; j < CIRCLES; j++) {
				explosions[i][j].count += step;
				if (explosions[i][j].count > 500)
					explosions[i][j].count = 500;
			}
		}
		else {
			acorns[i].x += acorns[i].dx*step;
			acorns[i].y += acorns[i].dy*step;
			acorns[i].angle += 0.05f*step;
		}
	}

	return false;
}

AcornsEffect::AcornsEffect(Combatant *caster, Combatant *target) :
	count(0),
	target(target)
{
	type = COMBATENTITY_TYPE_FRILL;

	x = 0;
	y = target->getY()+1;

	bitmap = m_load_bitmap(getResource("combat_media/acorn.png"));

	CombatLocation loc = target->getLocation();
	const float fallAngle = loc == LOCATION_RIGHT ? D2R(-120) : D2R(-60);
	const float yy = target->getY();
	const float xx = yy / tan(fallAngle);
	const float minDist = sqrt(xx*xx + yy*yy);

	for (int i = 0; i < ACORNS; i++) {
		int destX = target->getX() + ((rand() % 40) - 20);
		float dist = minDist + rand() % 100;
		acorns[i].x = destX+cos(fallAngle)*dist;
		acorns[i].y = target->getY()+sin(fallAngle)*dist;
		acorns[i].dx = cos(fallAngle+M_PI)*0.25f;
		acorns[i].dy = sin(fallAngle+M_PI)*0.25f;
		acorns[i].angle = 0.0f;
		for (int j = 0; j < CIRCLES; j++) {
			explosions[i][j].x = target->getX() + ((rand() % 30) - 15);
			explosions[i][j].y = target->getY() - (rand() % 20);
			explosions[i][j].radius = 5 + rand() % 10;
			int n = rand() % 4;
			switch (n) {
				case 0:
					explosions[i][j].color = m_map_rgb(230, 230, 230);
					break;
				case 1:
					explosions[i][j].color = m_map_rgb(250, 40, 0);
					break;
				case 2:
					explosions[i][j].color = m_map_rgb(200, 250, 0);
					break;
				case 3:
					explosions[i][j].color = m_map_rgb(30, 30, 30);
					break;
			}
			explosions[i][j].count = 0;
		}
	}
}


AcornsEffect::~AcornsEffect(void)
{
	m_destroy_bitmap(bitmap);
}


int DaisyEffect::getLifetime(void)
{
	return LIFETIME;
}

void DaisyEffect::draw(void)
{
	animSet->draw(dx, dy, flags);
}


bool DaisyEffect::update(int step)
{
	count += step;

	if (count >= getLifetime()) {
		return true;
	}

	animSet->update(step);

	return false;
}

DaisyEffect::DaisyEffect(Combatant *target) :
	count(0)
{
	type = COMBATENTITY_TYPE_FRILL;

	x = 0;
	y = target->getY()+1;

	animSet = new_AnimationSet(getResource("combat_media/Daisy.png"));

	if (target->getLocation() == LOCATION_RIGHT) {
		dx = target->getX()-target->getAnimationSet()->getWidth()/2-animSet->getWidth();
		flags = 0;
	}
	else {
		dx = target->getX()+target->getAnimationSet()->getWidth()/2;
		flags = M_FLIP_HORIZONTAL;
	}

	dy = target->getY()-animSet->getHeight();
}


DaisyEffect::~DaisyEffect(void)
{
	delete animSet;
}


int VampireEffect::getLifetime(void)
{
	return LIFETIME;
}

void VampireEffect::draw(void)
{
	MBITMAP *bmp = 
		target->getAnimationSet()->getCurrentAnimation()->getCurrentFrame()->getImage()->getBitmap();
	m_draw_tinted_bitmap(bmp, m_map_rgba(alpha, alpha, alpha, alpha), cx, cy, target->getLocation() == LOCATION_LEFT ?
		M_FLIP_HORIZONTAL : 0);
}


bool VampireEffect::update(int step)
{
	count += step;

	if (count >= getLifetime()) {
		return true;
	}

	float p = (float)count / getLifetime();
	cx = p * (dx-sx) + sx;
	cy = p * (dy-sy) + sy;
	alpha = 200 - (p * 200);
	

	return false;
}

VampireEffect::VampireEffect(Combatant *target) :
	count(0)
{
	type = COMBATENTITY_TYPE_FRILL;

	x = 0;
	y = target->getY()+1;

	this->target = target;

	if (target->getLocation() == LOCATION_RIGHT) {
		sx = target->getX()-target->getAnimationSet()->getWidth()/2;
		sy = target->getY()-target->getAnimationSet()->getHeight();
		dx = sx-50;
		dy = sy;
	}
	else {
		sx = target->getX()-target->getAnimationSet()->getWidth()/2;
		sy = target->getY()-target->getAnimationSet()->getHeight();
		dx = sx+50;
		dy = sy;
	}

	cx = sx;
	cy = sy;
	alpha = 128;
}


VampireEffect::~VampireEffect(void)
{
}


int ArcEffect::getLifetime(void)
{
	return LIFETIME;
}

void ArcEffect::draw(void)
{
	draw_bolt_arc(arc, 0, alphaAnim->getCurrentAnimation()->getCurrentFrame()->getImage()->getBitmap());
	draw_bolt_arc(arc, 1, NULL);
}


bool ArcEffect::update(int step)
{
	count += step;

	if (count >= getLifetime()) {
		return true;
	}

	alphaAnim->update(step);

	return false;
}

ArcEffect::ArcEffect(Combatant *caster, Combatant *target) :
	count(0)
{
	type = COMBATENTITY_TYPE_FRILL;

	x = 0;
	y = target->getY()+1;

	alphaAnim = new_AnimationSet(getResource("combat_media/Arc.png"), true);

	int MINDIST = 15;
	int MAXDIST = 30;

	int sx = caster->getX();
	int sy = caster->getY()-caster->getAnimationSet()->getHeight()/2;
	int dx = target->getX();
	int dy = target->getY()-target->getAnimationSet()->getHeight()/2;

	int cx = sx;
	int cy = sy;
	int total = 0;
	float angle = atan2((float)dy-sy, (float)dx-sx);
	
	for (;;) {
		total += (rand() % (MAXDIST-MINDIST)) + MINDIST;
		cx = sx + cos(angle) * total;
		cy = sy + sin(angle) * total;
		MPoint p;
		p.x = cx + rand() % 10;
		p.y = cy + rand() % 10;
		arc.push_back(p);
		float distx = dx - cx;
		float disty = dy - cy;
		float dist = sqrt(distx*distx  + disty*disty);
		if (dist <= MAXDIST) {
			MPoint p;
			p.x = dx;
			p.y = dy;
			arc.push_back(p);
			break;
		}
	}
}


ArcEffect::~ArcEffect(void)
{
	delete alphaAnim;
	arc.clear();
}


int BananaEffect::getLifetime(void)
{
	return LIFETIME;
}

void BananaEffect::draw(void)
{
	if (count < getLifetime()/2) {
		m_draw_rotated_bitmap(bitmap,
			m_get_bitmap_width(bitmap)/2,
			m_get_bitmap_height(bitmap)/2,
			cx, cy, bmpAngle, 0);
	}
	else {
		ALLEGRO_VERTEX *verts = new ALLEGRO_VERTEX[pixels.size()];
		for (unsigned int i = 0; i < pixels.size(); i++) {
			verts[i].x = pixels[i].cp.x;
			verts[i].y = pixels[i].cp.y;
			verts[i].z = 0;
			verts[i].color = pixels[i].cp.color;
		}
		m_draw_prim(verts, 0, 0, 0, (int)pixels.size(), ALLEGRO_PRIM_POINT_LIST);
		delete[] verts;
	}
}


bool BananaEffect::update(int step)
{
	count += step;

	if (count >= getLifetime()) {
		return true;
	}

	if (count > getLifetime()/2) {
		for (unsigned int i = 0; i < pixels.size(); i++) {
			pixels[i].cp.x += pixels[i].dx * step;
			pixels[i].cp.y += pixels[i].dy * step;
		}
	}
	else {
		cx = sx + ((float)count/(getLifetime()/2))*(dx - sx);
		cy = sy + ((float)count/(getLifetime()/2))*(dy - sy);
		cy -= sin((float)count/getLifetime()*M_PI*2)*30;
		bmpAngle += 0.001f * step;

		if (count > getLifetime()/4) {
			if (!switchedY) {
				y = newY;
				battle->resortEntity(this);
			}
		}
	}

	return false;
}


static void getExplodingPixels(MBITMAP *bmp, float dx, float dy, std::vector<ExplodingBitmapPoint> &v)
{
	float w = m_get_bitmap_width(bmp);
	float h = m_get_bitmap_height(bmp);

	m_lock_bitmap(bmp, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READONLY);

	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			MCOLOR color = m_get_pixel(bmp, x, y);
			unsigned char r, g, b, a;
			m_unmap_rgba(color, &r, &g, &b, &a);
			if (a == 0)
				continue;
			float distx = x - (float)w/2;
			float disty = y - (float)h/2;
			float angle = atan2(disty, distx);
			ExplodingBitmapPoint p;
			p.cp.x = dx - distx;
			p.cp.y = dy - disty;
			p.cp.color = color;
			p.dx = cos(angle)*(0.05f+((rand()%RAND_MAX)/(float)RAND_MAX)*0.05f);
			p.dy = sin(angle)*(0.05f+((rand()%RAND_MAX)/(float)RAND_MAX)*0.05f);
			v.push_back(p);
		}
	}

	m_unlock_bitmap(bmp);
}


BananaEffect::BananaEffect(Combatant *caster, Combatant *target, std::string imgName) :
	count(0)
{
	type = COMBATENTITY_TYPE_FRILL;

	x = 0;
	y = caster->getY()+1;

	bitmap = m_load_bitmap(getResource("combat_media/%s.png", imgName.c_str()));
	bmpAngle = 0;

	switchedY = false;
	newY = target->getY()+1;

	if (caster->getLocation() == LOCATION_LEFT) {
		sx = caster->getX()+10;
	}
	else {
		sx = caster->getX()-10;
	}
	if (imgName == "boulder") {
		sy = caster->getY()-caster->getAnimationSet()->getHeight()+10;
	}
	else {
		sy = caster->getY()-10;
	}
	dx = target->getX();
	dy = target->getY()-target->getAnimationSet()->getHeight()/2;

	cx = sx;
	cy = sy;

	getExplodingPixels(bitmap, dx, dy, pixels);
}


BananaEffect::~BananaEffect(void)
{
	m_destroy_bitmap(bitmap);
}


int FireballEffect::getLifetime(void)
{
	return LIFETIME;
}

void FireballEffect::draw(void)
{
	MBITMAP *b = animSet->getCurrentAnimation()->getCurrentFrame()->getImage()->getBitmap();
	
	m_draw_bitmap(b, cx-w/2, cy-h/2, flags);
}


bool FireballEffect::update(int step)
{
	count += step;

	if (count >= getLifetime()) {
		return true;
	}

	animSet->update(step);

	cx = ((float)count/getLifetime()) * (dx - sx) + sx;
	cy = ((float)count/getLifetime()) * (dy - sy) + sy;

	return false;
}


FireballEffect::FireballEffect(Combatant *caster, Combatant *target) :
	count(0)
{
	type = COMBATENTITY_TYPE_FRILL;

	x = 0;
	y = target->getY()+1;

	animSet = new_AnimationSet(getResource("combat_media/Fireball.png"));

	if (caster->getLocation() == LOCATION_LEFT) {
		flags = M_FLIP_HORIZONTAL;
		sx = caster->getX()+10;
		dx = target->getX()-10;
	}
	else {
		flags = 0;
		sx = caster->getX()-10;
		dx = target->getX()+10;
	}

	sy = caster->getY()-16;
	dy = target->getY()-target->getAnimationSet()->getHeight()/2;

	cx = sx;
	cy = sy;

	w = animSet->getWidth();
	h = animSet->getHeight();
}


FireballEffect::~FireballEffect(void)
{
	delete animSet;
}


int KissOfDeathEffect::getLifetime(void)
{
	return LIFETIME;
}

void KissOfDeathEffect::draw(void)
{
	m_draw_trans_bitmap(bitmap, dx, dy, alpha);
}


bool KissOfDeathEffect::update(int step)
{
	count += step;

	if (count >= getLifetime()) {
		return true;
	}

	alpha = 255 * ((float)count/getLifetime());

	return false;
}


KissOfDeathEffect::KissOfDeathEffect(Combatant *caster, Combatant *target) :
	count(0)
{
	type = COMBATENTITY_TYPE_FRILL;

	x = 0;
	y = 0;

	bitmap = m_load_bitmap(getResource("combat_media/red_pentagram.png"));

	dx = target->getX() - m_get_bitmap_width(bitmap)/2;
	dy = target->getY() - m_get_bitmap_height(bitmap)/2;
	alpha = 0;
}


KissOfDeathEffect::~KissOfDeathEffect(void)
{
	m_destroy_bitmap(bitmap);
}


int BoFEffect::getLifetime(void)
{
	return LIFETIME;
}

void BoFEffect::draw(void)
{
	int line = (int)offs % w;
	line = w - line;
	int lt = LIFETIME/3;

	if (count < LIFETIME/3) {
		int w1 = w - line;
		int w2 = w - w1;
		float height = (h/3) * ((float)count/lt);
		if (w1)
			m_draw_scaled_bitmap(bitmap, line, 0, w1, h, BW/2, 110/2-height/2,
				w1, height, 0, 255);
		if (w2)
			m_draw_scaled_bitmap(bitmap, 0, 0, w2, h, BW/2+(w-line), 110/2-height/2,
				w2, height, 0, 255);
	}
	else if (count < LIFETIME/3*2) {
		int w1 = w - line;
		int w2 = w - w1;
		if (w1)
			m_draw_scaled_bitmap(bitmap, line, 0, w1, h, BW/2, 110/2-(h/3)/2,
				w1, h/3, 0, 255);
		if (w2)
			m_draw_scaled_bitmap(bitmap, 0, 0, w2, h, BW/2+(w-line), 110/2-(h/3)/2,
				w2, h/3, 0, 255);
	}
	else {
		int c = count - (count/3*2);
		float extra = ((110-h) * ((float)c/lt));
		ALLEGRO_VERTEX *verts = new ALLEGRO_VERTEX[w*2];
		for (int i = 0; i < w; i++) {
			float height = (h/3) + (1-cos(((float)i/w)*M_PI/2)) * extra;
			int dy = 110/2 - height/2;
			verts[i*2+0].x = BW/2+i+0.5;
			verts[i*2+0].y = dy+0.5;
			verts[i*2+0].z = 0;
			verts[i*2+0].color = white;
			verts[i*2+0].u = line;
			verts[i*2+0].v = 0;
			verts[i*2+1].x = BW/2+i+0.5;
			verts[i*2+1].y = dy+height+0.5;
			verts[i*2+1].z = 0;
			verts[i*2+1].color = white;
			verts[i*2+1].u = line;
			verts[i*2+1].v = h;
			line++;
			line %= w;
		}
		m_draw_prim(verts, NULL, bitmap, 0, w*2, ALLEGRO_PRIM_LINE_LIST);
		delete[] verts;
	}
}


bool BoFEffect::update(int step)
{
	count += step;

	if (count >= getLifetime()) {
		return true;
	}

	offs += 0.2f * step;

	return false;
}


BoFEffect::BoFEffect(Combatant *caster, Combatant *target) :
	count(0)
{
	type = COMBATENTITY_TYPE_FRILL;

	x = 0;
	y = 9999;

	bitmap = m_load_bitmap(getResource("combat_media/BoF.png"));

	offs = 0;

	w = m_get_bitmap_width(bitmap);
	h = m_get_bitmap_height(bitmap);
}


BoFEffect::~BoFEffect(void)
{
	m_destroy_bitmap(bitmap);
}


int DropEffect::getLifetime(void)
{
	return LIFETIME;
}

void DropEffect::draw(void)
{
	if (!exploded) {
		m_draw_rotated_bitmap(bitmap,
			m_get_bitmap_width(bitmap)/2,
			m_get_bitmap_height(bitmap)/2,
			x, rock_y+target->getY()-target->getAnimationSet()->getHeight()/2,
			bmpAngle, 0);
	}
	else {
		ALLEGRO_VERTEX *verts = new ALLEGRO_VERTEX[pixels.size()];
		for (unsigned int i = 0; i < pixels.size(); i++) {
			verts[i].x = pixels[i].cp.x;
			verts[i].y = pixels[i].cp.y;
			verts[i].z = 0;
			verts[i].color = pixels[i].cp.color;
		}
		m_draw_prim(verts, 0, 0, 0, (int)pixels.size(), ALLEGRO_PRIM_POINT_LIST);
		delete[] verts;
	}
}


bool DropEffect::update(int step)
{
	count += step;

	if (count >= getLifetime()) {
		return true;
	}

	if (count < LIFETIME/2) {
		oy = ((float)count/(LIFETIME/2))*dy;
		caster->setOy(oy);
	}
	else {
		if (!dropped) {
			dropped = true;
			loadPlayDestroy("cartoon_fall.ogg");
		}
		oy = dy - (((float)(count-(LIFETIME/2))/(LIFETIME/2))*dy);
		caster->setOy(oy);
		if (rock_y < 0) {
			rock_y += 0.3f * step;
		}
		else {
			if (!exploded)
				loadPlayDestroy("explosion.ogg");
			exploded = true;
			for (unsigned int i = 0; i < pixels.size(); i++) {
				pixels[i].cp.x += pixels[i].dx * step;
				pixels[i].cp.y += pixels[i].dy * step;
			}
		}
	}

	bmpAngle += 0.002f * step;

	return false;
}


DropEffect::DropEffect(Combatant *caster, Combatant *target) :
	count(0)
{
	type = COMBATENTITY_TYPE_FRILL;

	x = target->getX();
	y = target->getY()+1;

	bitmap = m_load_bitmap(getResource("combat_media/Rock.png"));
	bmpAngle = 0;

	oy = 0;
	dy = -200;
	rock_y = -120;
	exploded = false;
	dropped = false;

	getExplodingPixels(bitmap, x, target->getY()-target->getAnimationSet()->getHeight()/2, pixels);

	this->caster = caster;
	this->target = target;

	caster->getAnimationSet()->setSubAnimation("fly");
}


DropEffect::~DropEffect(void)
{
	caster->setOy(0);
	caster->getAnimationSet()->setSubAnimation("stand");
	m_destroy_bitmap(bitmap);
}


int BlazeEffect::getLifetime(void)
{
	return LIFETIME;
}

void BlazeEffect::draw(void)
{
	float distx = dx - sx;
	float disty = dy - sy;
	float angle = atan2(disty, distx);
	float xinc = cos(angle+M_PI/2);
	float yinc = sin(angle+M_PI/2);


	MBITMAP *target;
	target = NULL;

	MCOLOR color = m_map_rgb(255, 0, 0);

	color.a = 0.2f;
	m_draw_precise_line(target, sx, sy, dx, dy, color);
	
	for (float i = 0.333f; i < 5.0f/2; i += 0.333f) {
		color.a = interpolate(0.2f, 0.0f, (i/(5.0f/2)));
		float xx1 = sx + xinc * i;
		float yy1 = sy + yinc * i;
		float xx2 = dx + xinc * i;
		float yy2 = dy + yinc * i;
		m_draw_precise_line(target, xx1, yy1, xx2, yy2, color);
		xx1 = sx - xinc * i;
		yy1 = sy - yinc * i;
		xx2 = dx - xinc * i;
		yy2 = dy - yinc * i;
		m_draw_precise_line(target, xx1, yy1, xx2, yy2, color);
	}
}


bool BlazeEffect::update(int step)
{
	count += step;

	if (count >= LIFETIME) {
		return true;
	}

	dy = (float)count/LIFETIME * 110;

	return false;
}


BlazeEffect::BlazeEffect(Combatant *caster) :
	count(0)
{
	type = COMBATENTITY_TYPE_FRILL;

	x = 0;
	y = 9999;

	CombatLocation loc = caster->getLocation();

	sx = caster->getX() + ((loc == LOCATION_LEFT) ? 4 : -4);
	sy = caster->getY()-86;
	if (loc == LOCATION_LEFT) {
		dx = BW-32;
	}
	else {
		dx = 32;
	}
	dy = 0;
}


BlazeEffect::~BlazeEffect(void)
{
}


int MachineGunEffect::getLifetime(void)
{
	return LIFETIME;
}

void MachineGunEffect::draw(void)
{
	MCOLOR color = m_map_rgb(255, 255, 0);

	ALLEGRO_VERTEX *verts = new ALLEGRO_VERTEX[25];

	for (int i = 0; i < 25; i++) {
		int rx = rand() % 20 - 10;
		int ry = rand() % 20 - 10;
		verts[i].x = cx+rx;
		verts[i].y = cy+ry;
		verts[i].z = 0;
		verts[i].color = color;
	}

	m_draw_prim(verts, 0, 0, 0, 25, ALLEGRO_PRIM_POINT_LIST);

	delete[] verts;
}


bool MachineGunEffect::update(int step)
{
	count += step;

	if (count >= LIFETIME) {
		return true;
	}

	return false;
}


MachineGunEffect::MachineGunEffect(Combatant *caster, Combatant *target) :
	count(0)
{
	type = COMBATENTITY_TYPE_FRILL;

	x = 0;
	y = target->getY()+1;

	cx = target->getX();
	cy = target->getY()-target->getAnimationSet()->getHeight()/2;
}


MachineGunEffect::~MachineGunEffect(void)
{
}


int LaserEffect::getLifetime(void)
{
	return LIFETIME;
}

void LaserEffect::draw(void)
{
	float xinc = cos(angle+M_PI/2);
	float yinc = sin(angle+M_PI/2);

	float sx, sy, dx, dy;

	if (caster->getLocation() == LOCATION_LEFT) {
		sx = cx - 8 * cos(angle);
		dx = cx + 8 * cos(angle);
		sy = cy - 8 * sin(angle);
		dy = cy + 8 * sin(angle);
	}
	else {
		sx = cx + 8 * cos(angle);
		dx = cx - 8 * cos(angle);
		sy = cy + 8 * sin(angle);
		dy = cy - 8 * sin(angle);
	}


	MBITMAP *target;
	target = NULL;

	MCOLOR color = m_map_rgb(0, 200, 255);

	for (float i = 0.333f; i < 5.0f/2; i += 0.333f) {
		color.a = interpolate(0.2f, 0.0f, (i/(5.0f/2)));
		float xx1 = sx + xinc * i;
		float yy1 = sy + yinc * i;
		float xx2 = dx + xinc * i;
		float yy2 = dy + yinc * i;
		m_draw_precise_line(target, xx1, yy1, xx2, yy2, color);
		xx1 = sx - xinc * i;
		yy1 = sy - yinc * i;
		xx2 = dx - xinc * i;
		yy2 = dy - yinc * i;
		m_draw_precise_line(target, xx1, yy1, xx2, yy2, color);
	}
}


bool LaserEffect::update(int step)
{
	count += step;

	if (count >= LIFETIME) {
		return true;
	}

	cx = ((float)count/LIFETIME)*(end_x-start_x)+start_x;
	cy = ((float)count/LIFETIME)*(end_y-start_y)+start_y;

	return false;
}


LaserEffect::LaserEffect(Combatant *caster, Combatant *target) :
	count(0)
{
	type = COMBATENTITY_TYPE_FRILL;

	x = 0;
	y = target->getY()+1;

	this->caster = caster;

	if (caster->getLocation() == LOCATION_LEFT) {
		start_x = caster->getX() + 15;
		end_x = target->getX()-target->getAnimationSet()->getWidth()/2;
	}
	else {
		start_x = caster->getX()-15;
		end_x = target->getX()+target->getAnimationSet()->getWidth()/2;
	}
	
	start_y = caster->getY()-30;
	end_y = target->getY()-target->getAnimationSet()->getHeight()/2;

	cx = start_x;
	cy = start_y;

	angle = atan2(end_y-start_y, end_x-start_x);
}


LaserEffect::~LaserEffect(void)
{
}


int UFOEffect::getLifetime(void)
{
	return 3000;
}

void UFOEffect::draw(void)
{
	if (count < 200 || count > (getLifetime()-200)) {
		return;
	}

	float depth = 0;

	ALLEGRO_VERTEX *verts = new ALLEGRO_VERTEX[(int)(MAXLEN/0.05)*2];
	int vcount = 0;

	for (float f = 0; f < MAXLEN; f += 0.05f)
	{
		float dist_from_start = sqrt((float)(cx-start_x)*(cx-start_x) +
			(cy-start_y)*(cy-start_y));
		if (dist_from_start < MAXLEN)
			continue;
		float dist_from_end = sqrt((float)(end_x-cx)*(end_x-cx) +
			(end_y-cy)*(end_y-cy));
		if (dist_from_end < MAXLEN)
			continue;
		float px = cx + cos(angle)*f;
		float py = cy + sin(angle)*f;
		py += sin(angle+M_PI/2+offs+(f/(MAXLEN/10.0f)*M_PI*2))*8;
		float xinc = cos(angle-M_PI/2+offs+(f/(MAXLEN/10.0f)*M_PI*2));
		depth = 127 + (128 - (64*(xinc+1)));
		px += xinc*8;
		verts[vcount].x = px;
		verts[vcount].y = py;
		verts[vcount].z = 0;
		verts[vcount].color = m_map_rgb(0, depth, 0);
		vcount++;
	}

	m_draw_prim(verts, 0, 0, 0, vcount, ALLEGRO_PRIM_POINT_LIST);

	delete[] verts;
}


bool UFOEffect::update(int step)
{
	count += step;

	if (count >= getLifetime()) {
		return true;
	}

	if (caster->getLocation() == LOCATION_LEFT) {
		offs += step * 0.01f;
	}
	else {
		offs -= step * 0.01f;
	}

	if (count < 200) {
		caster->setY(ufo_start_y-50.0f*(count/200.0f));
	}
	else if (count >= (getLifetime()-200)) {
		caster->setAngle(0);
		int c = count - (getLifetime()-200);
		caster->setY(ufo_start_y-50+(c/200.0f*50));
	}
	else {
		caster->setAngle(caster->getLocation() == LOCATION_LEFT ?
			M_PI/4 : -M_PI/4);
		cx = ((float)count/getLifetime())*(end_x-start_x)+start_x;
		cy = ((float)count/getLifetime())*(end_y-start_y)+start_y;
	}

	return false;
}


UFOEffect::UFOEffect(Combatant *caster, Combatant *target) :
	count(0)
{
	type = COMBATENTITY_TYPE_FRILL;

	x = 0;
	y = target->getY()+1;

	this->caster = caster;

	if (caster->getLocation() == LOCATION_LEFT) {
		start_x = caster->getX() + 20;
		end_x = target->getX()-target->getAnimationSet()->getWidth()/2;
	}
	else {
		start_x = caster->getX()-20;
		end_x = target->getX()+target->getAnimationSet()->getWidth()/2;
	}
	
	start_y = caster->getY()-50;
	end_y = target->getY()-target->getAnimationSet()->getHeight()/2;

	cx = start_x;
	cy = start_y;

	angle = atan2((float)end_y-start_y, end_x-start_x);

	start_x += cos(angle+M_PI) * MAXLEN;
	start_y += sin(angle+M_PI) * MAXLEN;
	end_x += cos(angle) * MAXLEN;
	end_y += sin(angle) * MAXLEN;

	offs = 0;
	length = 0;

	ufo_start_y = caster->getY();

	caster->getAnimationSet()->setSubAnimation("raise");
}


UFOEffect::~UFOEffect(void)
{
	caster->setY(ufo_start_y);
	caster->getAnimationSet()->setSubAnimation("stand");
}


int OrbitEffect::getLifetime(void)
{
	return LIFETIME;
}

void OrbitEffect::draw(void)
{
}


bool OrbitEffect::update(int step)
{
	count += step;

	if (count >= LIFETIME) {
		return true;
	}

	float r; // radius
	float a; // angle
	float cx; // center x
	if (caster->getLocation() == LOCATION_LEFT) {
		r = (BW-20-caster->getX())/2;
		a = M_PI + ((float)count/LIFETIME)*M_PI*2;
		cx = caster->getX()+r;
	}
	else {
		r = (caster->getX()-20)/2;
		a = 0 + ((float)count/LIFETIME)*M_PI*2;
		cx = caster->getX()-r;
	}

	float cy = caster->getY();
	float xx = cx + cos(a)*r;
	float yy = cy + sin(a)*r;
	float ox = xx-caster->getX();
	float oy = yy-caster->getY();
	caster->setOx(ox);
	caster->setOy(oy);

	return false;
}


OrbitEffect::OrbitEffect(Combatant *caster, Combatant *target) :
	count(0)
{
	type = COMBATENTITY_TYPE_FRILL;

	x = 0;
	y = 9999;

	this->caster = caster;
}


OrbitEffect::~OrbitEffect(void)
{
	caster->setOx(0);
	caster->setOy(0);
}


int WebEffect::getLifetime(void)
{
	return LIFETIME;
}

void WebEffect::draw(void)
{
	m_draw_rotated_bitmap(bitmap, m_get_bitmap_width(bitmap)/2, m_get_bitmap_height(bitmap)/2, cx, cy, 0, draw_flags);
}


bool WebEffect::update(int step)
{
	count += step;

	if (count >= LIFETIME) {
		return true;
	}

	cx = sx + (((float)count/LIFETIME)*(dx-sx));
	cy = sy + (((float)count/LIFETIME)*(dy-sy));

	angle += step * 0.01f;

	return false;
}


WebEffect::WebEffect(Combatant *caster, Combatant *target) :
	count(0)
{
	type = COMBATENTITY_TYPE_FRILL;

	x = 0;
	y = target->getY()+1;

	if (caster->getLocation() == LOCATION_LEFT) {
		sx = caster->getX()+caster->getAnimationSet()->getWidth()/2;
		draw_flags = 0;
	}
	else {
		sx = caster->getX()-caster->getAnimationSet()->getWidth()/2;
		draw_flags = ALLEGRO_FLIP_HORIZONTAL;
	}
	sy = caster->getY()-caster->getAnimationSet()->getHeight()/2;

	dx = target->getX();
	dy = target->getY()-target->getAnimationSet()->getHeight()/2;

	cx = sx;
	cy = sy;
	
	angle = 0;

	bitmap = m_load_bitmap(getResource("combat_media/web.png"));
}


WebEffect::~WebEffect(void)
{
	m_destroy_bitmap(bitmap);
}


int Darkness3Effect::getLifetime(void)
{
	return LIFETIME;
}

void Darkness3Effect::draw(void)
{
	if (dark)
		m_draw_rectangle(0, 0, BW, BH, black, M_FILLED);
}


bool Darkness3Effect::update(int step)
{
	count += step;

	if (count >= LIFETIME) {
		return true;
	}

	if ((unsigned)tguiCurrentTimeMillis() % 100 < 50)
		dark = true;
	else
		dark = false;

	return false;
}


Darkness3Effect::Darkness3Effect(void) :
	count(0)
{
	type = COMBATENTITY_TYPE_FRILL;

	x = 0;
	y = 9999;

	dark = true;
}


Darkness3Effect::~Darkness3Effect(void)
{
}

int SwallowEffect::getLifetime(void)
{
	return LIFETIME;
}

void SwallowEffect::draw(void)
{
	if (count > SWALLOW_TIME)
		return;

	int w = m_get_bitmap_width(bitmap);
	int h = m_get_bitmap_height(bitmap);

	m_draw_rotated_bitmap(bitmap, w/2, h/2, cx, cy, angle, 0);


	float a = atan2((float)dy-sy, dx-sx);
	float xinc = cos(a+M_PI/2);
	float yinc = sin(a+M_PI/2);

	MCOLOR color = m_map_rgb(200, 100, 100);

	color.a = 0.2f;

#define tmp NULL

	m_draw_precise_line(tmp, dx, dy, cx, cy, color);
	
	for (float i = 0.333f; i < 5.0f/2; i += 0.333f) {
		color.a = interpolate(0.2f, 0.0f, (i/(5.0f/2)));
		float xx1 = dx + xinc * i;
		float yy1 = dy + yinc * i;
		float xx2 = cx + xinc * i;
		float yy2 = cy + yinc * i;
		m_draw_precise_line(tmp, xx1, yy1, xx2, yy2, color);
		xx1 = dx - xinc * i;
		yy1 = dy - yinc * i;
		xx2 = cx - xinc * i;
		yy2 = cy - yinc * i;
		m_draw_precise_line(tmp, xx1, yy1, xx2, yy2, color);
	}

	m_draw_circle(cx, cy, 3, m_map_rgb(200, 100, 100), M_FILLED);
}


bool SwallowEffect::update(int step)
{
	count += step;

	if (count >= LIFETIME) {
		return true;
	}

	if (count > SWALLOW_TIME)
		return false;

	cx = sx + ((float)count/SWALLOW_TIME)*(dx-sx);
	cy = sy + ((float)count/SWALLOW_TIME)*(dy-sy);

	return false;
}


SwallowEffect::SwallowEffect(Combatant *caster, Combatant *target) :
	count(0)
{
	type = COMBATENTITY_TYPE_FRILL;

	this->caster = (CombatEnemyTode *)caster;
	this->target = target;

	x = 0;
	y = target->getY()+1;
	
	if (caster->getLocation() == LOCATION_LEFT) {
		dx = caster->getX()+caster->getAnimationSet()->getWidth()/2-20;
	}
	else {
		dx = caster->getX()-caster->getAnimationSet()->getWidth()/2+20;
	}
	dy = caster->getY()-caster->getAnimationSet()->getHeight()+30;

	sx = target->getX();
	sy = target->getY()-target->getAnimationSet()->getHeight()/2;

	cx = sx;
	cy = sy;

	AnimationSet *a = target->getAnimationSet();
	a->setSubAnimation("hit");
	int w = a->getWidth();
	int h = a->getHeight();
	int flags = al_get_new_bitmap_flags();
	al_set_new_bitmap_flags(flags & ~ALLEGRO_NO_PRESERVE_TEXTURE);
	bitmap = m_create_bitmap(w, h); // check
	al_set_new_bitmap_flags(flags);

	ALLEGRO_BITMAP *oldTarget = al_get_target_bitmap();
	m_set_target_bitmap(bitmap);
	my_clear_bitmap(bitmap);
	a->draw(0, 0, 0);
	al_set_target_bitmap(oldTarget);

	angle = (float)(rand() % RAND_MAX)/RAND_MAX * M_PI * 2;
}


SwallowEffect::~SwallowEffect(void)
{
	m_destroy_bitmap(bitmap);
}


int PukeEffect::getLifetime(void)
{
	return LIFETIME;
}

static bool puke_sound;

void PukeEffect::draw(void)
{
	if (count < START_TIME || count > START_TIME+PUKE_TIME) return;

	if (!puke_sound) {
		loadPlayDestroy("Puke.ogg");
		puke_sound = true;
	}

	ALLEGRO_VERTEX *verts = new ALLEGRO_VERTEX[lines.size()*2];
	for (int i = 0; i < (int)lines.size(); i++) {
		int x1, y1, x2, y2;
		x1 = -cos(lines[i].angle) * 10 + lines[i].cx;
		y1 = -sin(lines[i].angle) * 10 + lines[i].cy;
		x2 = cos(lines[i].angle) * 10 + lines[i].cx;
		y2 = sin(lines[i].angle) * 10 + lines[i].cy;
		verts[i*2+0].x = x1;
		verts[i*2+0].y = y1;
		verts[i*2+0].z = 0;
		verts[i*2+0].color = lines[i].color;
		verts[i*2+1].x = x2;
		verts[i*2+1].y = y2;
		verts[i*2+1].z = 0;
		verts[i*2+1].color = lines[i].color;
	}
	m_draw_prim(verts, NULL, NULL, 0, (int)lines.size()*2, ALLEGRO_PRIM_LINE_LIST);
	delete[] verts;
}


bool PukeEffect::update(int step)
{
	count += step;

	if (count >= LIFETIME) {
		return true;
	}

	if (count < START_TIME || count > START_TIME+PUKE_TIME)
		return false;

	int desired_lines = ((float)count/LIFETIME)*500+50;

	for (int i = 0; i < (int)(desired_lines-lines.size()); i++) {
		genline();
	}

	std::vector<PukeLine>::iterator it;
	for (it = lines.begin(); it != lines.end();) {
		PukeLine &l = *it;
		l.cx += cos(l.angle) * step * 0.2f;
		l.cy += sin(l.angle) * step * 0.2f;
		if (l.cx > BW) {
			it = lines.erase(it);
		}
		else
			it++;
	}

	return false;
}

void PukeEffect::genline(void)
{
	PukeLine l;
	l.angle = ((float)(rand() % RAND_MAX)/RAND_MAX)*(M_PI*.36)-0.35;
	l.cx = caster->getX()+caster->getAnimationSet()->getWidth()/2-11+(cos(l.angle)*10);
	l.cy = caster->getY()-caster->getAnimationSet()->getHeight()+30+(sin(l.angle)*10);
	l.color = m_map_rgb(
		25+rand()%25,
		205+rand()%50,
		0
	);
	lines.push_back(l);
}


PukeEffect::PukeEffect(Combatant *caster) :
	count(0)
{
	puke_sound = false;

	type = COMBATENTITY_TYPE_FRILL;

	this->caster = caster;

	x = 0;
	y = 9999;

	for (int i = 0; i < 50; i++) {
		genline();
	}
}


PukeEffect::~PukeEffect(void)
{
}

