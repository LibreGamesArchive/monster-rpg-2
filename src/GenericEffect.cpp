#include "monster2.hpp"

#include <allegro5/internal/aintern_bitmap.h>

const float GenericHolyWaterEffect::RISE_SPEED = 0.1f;

void GenericCureEffect::finalize(Combatant *target)
{
	if (!battle)
		return;

	int tx, ty;
	
	if (target->getLocation() == LOCATION_RIGHT) {
		tx = (int)(target->getX() - target->getAnimationSet()->getWidth()/2);
	}
	else {
		tx = (int)(target->getX() + target->getAnimationSet()->getWidth()/2);
	}

	ty = (int)(target->getY() - (target->getAnimationSet()->getHeight()/3*2));

	int maxhp = target->getInfo().abilities.maxhp;
	int hp = target->getInfo().abilities.hp;

	if (target->getName() == "Zombie" || target->getName() == "Mummy") {
		char buf[100];
		sprintf(buf, "%d", MIN(hp, amount));
		std::string amountS = std::string(buf);

		TemporaryText *tt = new TemporaryText(tx, ty, amountS, white);
		battle->addEntity(tt);
	}
	else if (hp > 0) {
		char buf[100];
		sprintf(buf, "%d", MIN(maxhp-hp, amount));
		std::string amountS = std::string(buf);

		TemporaryText *tt = new TemporaryText(tx, ty, amountS, m_map_rgb(0, 255, 0));
		battle->addEntity(tt);
	}
}


int GenericCureEffect::getLifetime(void)
{
	return animation->getCurrentAnimation()->getLength();
}

bool GenericCureEffect::update(int step)
{
	count += step;
	if (count >= getLifetime()) {
		if (finish)
			finalize(target);
		return true;
	}

	animation->update(step);

	return false;
}


void GenericCureEffect::draw(void)
{
	int x = (int)(target->getX() - animation->getWidth()/2);
	int y = (int)((target->getY() - target->getAnimationSet()->getHeight()/2) -
		(animation->getHeight()/2));
	animation->draw(x, y, 0);
}


GenericCureEffect::GenericCureEffect(Combatant *user, Combatant *target, int amount, std::string name, bool finish) :
	GenericEffect(user, target)
{
	this->user = user;
	this->target = target;
	this->amount = amount;
	this->finish = finish;

	animation = new_AnimationSet(getResource("combat_media/%s.png", name.c_str()));
	count = 0;

	type = COMBATENTITY_TYPE_FRILL;
}


GenericCureEffect::~GenericCureEffect(void)
{
	delete animation;
}



void GenericHolyWaterEffect::finalize(Combatant *target)
{
	if (!battle)
		return;

	int tx, ty;
	
	if (target->getLocation() == LOCATION_RIGHT) {
		tx = (int)(target->getX() - target->getAnimationSet()->getWidth()/2);
	}
	else {
		tx = (int)(target->getX() + target->getAnimationSet()->getWidth()/2);
	}

	ty = (int)(target->getY() - (target->getAnimationSet()->getHeight()/3*2));

	TemporaryText *tt;

	int maxhp = target->getInfo().abilities.maxhp;
	int hp = target->getInfo().abilities.hp;
	
	if (target->getName() == "Zombie" || target->getName() == "Mummy") {
		char buf[100];
		if (hp >= 1000)
			strcpy(buf, "1000");
		else
			sprintf(buf, "%d", hp);
		tt = new TemporaryText(tx, ty, std::string(buf), white);
	}
	else {
		if (hp > 0) {
			char buf[100];
			sprintf(buf, "%d", maxhp-hp);
			tt = new TemporaryText(tx, ty, std::string(buf), m_map_rgb(0, 255, 0));
		}
		else {
			tt = new TemporaryText(tx, ty, "LIFE!", m_map_rgb(0, 255, 0));
		}
	}
	battle->addEntity(tt);

	if (target != user) {
		target->getAnimationSet()->setSubAnimation("stand");
	}
}


int GenericHolyWaterEffect::getLifetime(void)
{
	return 2500;
}

bool GenericHolyWaterEffect::update(int step)
{
	count += step;
	if (count >= getLifetime()) {
		if (finish)
			finalize(target);
		return true;
	}

	for (int i = 0; i < numPixels; i++) {
		pixels[i].y += RISE_SPEED * step;
		if (pixels[i].y > y) {
			pixels[i].y -= y;
		}
		pixels[i].x = cos((pixels[i].y/y)*3.0f*M_PI*2.0f) * maxDepth;
		pixels[i].z = sin((pixels[i].y/y)*3.0f*M_PI*2.0f) * maxDepth;
	}

	return false;
}


void GenericHolyWaterEffect::draw(void)
{
	int i, count;
	ALLEGRO_VERTEX *verts = new ALLEGRO_VERTEX[numPixels];

	// draw pixels behind target
	for (i = 0, count = 0; i < numPixels; i++) {
		GenericHolyWaterPixel *p = &pixels[i];
		if (p->z >= 0) {
			continue;
		}
		MCOLOR color;
		int c = (int)(p->z / maxDepth * 64.0f);
		color = m_map_rgb(0, 192+c, 192+c);
		verts[count].x = x+p->x;
		verts[count].y = y-p->y+p->offset;
		verts[count].z = 0;
		verts[count].color = color;
		count++;
	}

	m_draw_prim(verts, 0, 0, 0, count, ALLEGRO_PRIM_POINT_LIST);

	target->draw();

	// draw pixels in front of target
	for (i = 0, count = 0; i < numPixels; i++) {
		GenericHolyWaterPixel *p = &pixels[i];
		if (p->z < 0) {
			continue;
		}
		MCOLOR color;
		int c = (int)(p->z / maxDepth * 64.0f);
		color = m_map_rgb(0, 192+c, 192+c);
		verts[count].x = x+p->x;
		verts[count].y = y-p->y+p->offset;
		verts[count].z = 0;
		verts[count].color = color;
		count++;
	}

	m_draw_prim(verts, 0, 0, 0, count, ALLEGRO_PRIM_POINT_LIST);

	delete[] verts;
}


GenericHolyWaterEffect::GenericHolyWaterEffect(Combatant *user, Combatant *target, bool finish, bool sound) :
	GenericEffect(user, target)
{
	this->finish = finish;

	count = 0;

	type = COMBATENTITY_TYPE_FRILL;

	numPixels = (int)(NUM_PIXELS * y);

	pixels = new GenericHolyWaterPixel[numPixels];

	maxDepth = target->getAnimationSet()->getWidth()/2 + 5;

	for (int i = 0; i < numPixels; i++) {
		pixels[i].y = (float)(rand() % RAND_MAX) / RAND_MAX * y;
		pixels[i].x = cos((pixels[i].y/y)*3.0f*M_PI*2.0f) * maxDepth;
		pixels[i].z = sin((pixels[i].y/y)*3.0f*M_PI*2.0f) * maxDepth;
		pixels[i].offset = rand() % 5 - 2;
	}
	
	if (sound) {
		loadPlayDestroy("HolyWater.ogg");
	}
}


GenericHolyWaterEffect::~GenericHolyWaterEffect(void)
{
	delete[] pixels;
}


void GenericHealEffect::finalize(Combatant *target)
{
	if (!battle)
		return;

	int tx, ty;
	
	if (target->getLocation() == LOCATION_RIGHT) {
		tx = (int)(target->getX() - target->getAnimationSet()->getWidth()/2);
	}
	else {
		tx = (int)(target->getX() + target->getAnimationSet()->getWidth()/2);
	}

	ty = (int)(target->getY() - (target->getAnimationSet()->getHeight()/3*2));

	if (target->getInfo().abilities.hp > 0) {
		TemporaryText *tt = new TemporaryText(tx, ty, "HEAL!", m_map_rgb(0, 255, 0));
		battle->addEntity(tt);
	}
}


int GenericHealEffect::getLifetime(void)
{
	return 2000;
}

bool GenericHealEffect::update(int step)
{
	count += step;
	if (count >= getLifetime()) {
		if (finish)
			finalize(target);
		return true;
	}

	for (int i = 0; i < NUM_PIXELS; i++) {
		pixels[i].a += 0.01f * step;
	}

	return false;
}


void GenericHealEffect::draw(void)
{
	float dx, dy;

	ALLEGRO_VERTEX verts[NUM_PIXELS];

	int i;

	for (i = 0; i < NUM_PIXELS; i++) {
		dx = cx + (pixels[i].radius * cos(pixels[i].a));
		dy = cy + (pixels[i].radius * sin(pixels[i].a));
		verts[i].x = dx;
		verts[i].y = dy;
		verts[i].z = 0;
		verts[i].color = pixels[i].color;
	}

	m_draw_prim(verts, 0, 0, 0, i, ALLEGRO_PRIM_POINT_LIST);
}


GenericHealEffect::GenericHealEffect(Combatant *user, Combatant *target, bool finish) :
	GenericEffect(user, target)
{
	this->finish = finish;

	count = 0;

	type = COMBATENTITY_TYPE_FRILL;
	
	int w = target->getAnimationSet()->getWidth();
	int h = target->getAnimationSet()->getHeight();

	if (w > h) {
		r = w/2.0f + 2;
	}
	else {
		r = h/2.0f + 2;
	}

	cx = target->getX();
	cy = target->getY() - (h/2);

	for (int i = 0; i < NUM_PIXELS; i++) {
		float a = ((float)(rand() % RAND_MAX) / (float)RAND_MAX) * (M_PI*2);
		pixels[i].a = a;
		float add = (((float)(rand() % RAND_MAX) / (float)RAND_MAX) * MAX_OFS);
		pixels[i].radius = r + add;
		pixels[i].color = m_map_rgb(0,
			(add/MAX_OFS)*128+127, 0);
	}
}


GenericHealEffect::~GenericHealEffect(void)
{
}



void GenericElixirEffect::finalize(Combatant *target)
{
	if (!battle)
		return;

	int tx, ty;
	
	if (target->getLocation() == LOCATION_RIGHT) {
		tx = (int)(target->getX() - target->getAnimationSet()->getWidth()/2);
	}
	else {
		tx = (int)(target->getX() + target->getAnimationSet()->getWidth()/2);
	}

	ty = (int)(target->getY() - (target->getAnimationSet()->getHeight()/3*2));

	int maxmp = target->getInfo().abilities.maxmp;
	int mp = target->getInfo().abilities.mp;

	if (target->getInfo().abilities.hp > 0) {
		char text[100];
		sprintf(text, "%d", MIN(maxmp-mp, 100));

		TemporaryText *tt = new TemporaryText(tx, ty, std::string(text), m_map_rgb(0xff, 0x7f, 0));
		battle->addEntity(tt);
	}
}


void GenericElixirEffect::draw(void)
{
	int half_w = m_get_bitmap_width(bitmap)/2;
	int half_h = m_get_bitmap_height(bitmap)/2;
 
	for (int i = 0; i < 4; i++) {
		m_draw_rotated_bitmap(bitmap, half_w, half_h,
			xs[i], ys[i], angle2, 0);
	}
}


bool GenericElixirEffect::update(int step)
{
	count += step;
	if (count >= getLifetime()) {
		finalize((Combatant *)target);
		return true;
	}

	angle += 0.005f * step;
	angle2 += 0.005f * step;

	int radius = m_get_bitmap_width(bitmap)/2+2;
	float worka = angle;

	for (int i = 0; i < 4; i++) {
		xs[i] = cx + (cos(worka) * radius);
		ys[i] = cy + (sin(worka) * radius);
		worka += (M_PI/2.0f);
	}

	return false;
}

int GenericElixirEffect::getLifetime(void)
{
	return 2000;
}


GenericElixirEffect::GenericElixirEffect(Combatant *user, Combatant *target) :
	GenericEffect(user, target)
{
	count = 0;

	type = COMBATENTITY_TYPE_FRILL;

	bitmap = m_load_bitmap(getResource("combat_media/magic_symbol.png"));

	cx = target->getX() + (target->getLocation() == LOCATION_RIGHT ? -20 : 20);
	cy = target->getY()-target->getAnimationSet()->getHeight()/2;
	angle = 0.0f;
	angle2 = 0.0f;

	loadPlayDestroy("Elixir.ogg");
}


GenericElixirEffect::~GenericElixirEffect(void)
{
	m_destroy_bitmap(bitmap);
}


