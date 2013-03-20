#include "monster2.hpp"

const float Fish::SPEED = 0.02f;


static unsigned int last_id = 0;

static int xoffsets[] = {
	-1, 0, 1,
	-1, 0, 1,
	-1, 0, 1
};
static int yoffsets[] = {
	-1, -1, -1,
	0, 0, 0,
	1, 1, 1
};

static float poison_x;


static unsigned int findUniqueId()
{
	// Ok, this is easy enough...
	return last_id++;
}

void resetIds()
{
	last_id = 0;
}

void Object::faceInputsLikeSprite()
{
	if (animationSet) {
		std::string subName = animationSet->getSubName();
		Input *i = getInput();
		if (subName == "stand_s" || subName == "walk_s") {
			if (i) i->setDirection(DIRECTION_SOUTH);
			if (input) input->setDirection(DIRECTION_SOUTH);
		}
		else if (subName == "stand_n" || subName == "walk_n") {
			if (i) i->setDirection(DIRECTION_NORTH);
			if (input) input->setDirection(DIRECTION_NORTH);
		}
		else if (subName == "stand_e" || subName == "walk_e") {
			if (i) i->setDirection(DIRECTION_EAST);
			if (input) input->setDirection(DIRECTION_EAST);
		}
		else if (subName == "stand_w" || subName == "walk_w") {
			if (i) i->setDirection(DIRECTION_WEST);
			if (input) input->setDirection(DIRECTION_WEST);
		}
	}
}

void Object::setDimensions(int w, int h)
{
	this->w = w;
	this->h = h;
	clearOccupied();
	addOccupied(x, y);
}


void Object::setFloater(bool floater)
{
	this->floater = floater;
}


void Object::setDest(int dx, int dy)
{
	this->dx = dx;
	this->dy = dy;
}

void Object::setPosition(int x, int y)
{
	this->x = x;
	this->y = y;
	clearOccupied();
	addOccupied(x, y);
}


void Object::setOffset(int ox, int oy)
{
	this->ox = ox;
	this->oy = oy;
}

void Object::setAnimationSet(std::string name)
{
	if (animationSet) {
		delete animationSet;
		animationSet = NULL;
	}

	animationSet = new_AnimationSet(name.c_str());

	animationSet->setSubAnimation("stand_s");
	Animation *a = animationSet->getCurrentAnimation();
	w = a->getCurrentFrame()->getImage()->getWidth();
	h = a->getCurrentFrame()->getImage()->getHeight();
	clearOccupied();
	addOccupied(x, y);
}


void Object::setSolid(bool solid)
{
	this->solid = solid;
}


void Object::setPerson(bool person)
{
	this->person = person;
}


void Object::getPosition(int *x, int *y)
{
	*x = this->x;
	*y = this->y;
}


void Object::getOffset(int *ox, int *oy)
{
	*ox = this->ox;
	*oy = this->oy;
}


uint Object::getId( void )
{
    return this->_id;
}

bool Object::isHigh()
{
	return high;
}

bool Object::isLow()
{
	return low;
}

void Object::setHigh(bool high)
{
	this->high = high;
}

void Object::setLow(bool low)
{
	this->low = low;
}

bool Object::isHidden()
{
	return hidden;
}

void Object::setHidden(bool hidden)
{
	this->hidden = hidden;
}

int Object::getX()
{
	return x;
}

int Object::getY()
{
	return y;
}

Input *Object::getInput()
{
	return input;
}

std::vector<int *> &Object::getOccupied()
{
	return occupied;
}


void Object::setInput(Input *i)
{
	input = i;
}


AnimationSet *Object::getAnimationSet()
{
	return animationSet;
}


bool Object::isSolid()
{
	return solid;
}


bool Object::isMoving()
{
	return moving;
}


int Object::getMoveDirection()
{
	return moveDirection;
}


void Object::getDimensions(int *w, int *h)
{
	*w = this->w;
	*h = this->h;
}


void Object::draw(float x, float y)
{
	int o = 0;

	if (moving && floater) {
		o = ((unsigned)tguiCurrentTimeMillis() % 600) < 300 ? 1 : 0;
	}

	animationSet->draw(x, y-o);
}

void Object::draw()
{
	if (!animationSet)
		return;

	int dx, dy;

	dx = ((x * TILE_SIZE) - area->getOriginX() + ox);
	dy = ((y * TILE_SIZE) - area->getOriginY()
		- (h - TILE_SIZE) + oy);

	draw(dx, dy);
}


void Object::drawUpper()
{
	if (poisoned && poisonBlocks[0].color.a > 0.0f) {
		int dx = poisonBlocks[0].x;
		int dy = poisonBlocks[0].y;
		dx *= TILE_SIZE;
		dy *= TILE_SIZE;
		dx -= area->getOriginX();
		dy -= area->getOriginY();
		dx += (int)ox;
		dy += (int)oy;
		m_push_target_bitmap();
		m_set_target_bitmap(poison_bmp_tmp);
		m_clear(m_map_rgba(0, 0, 0, 0));
		int w = m_get_bitmap_width(poison_bmp);
		int h = m_get_bitmap_height(poison_bmp);
		int w2 = m_get_bitmap_width(poison_bmp_tmp);
		int h2 = m_get_bitmap_height(poison_bmp_tmp);
		ALLEGRO_VERTEX *verts = new ALLEGRO_VERTEX[MAX(w, MAX(h, MAX(w2, h2))) * 2];
		for (int i = 0; i < w; i++) {
			verts[i*2+0].x = i;
			verts[i*2+0].y = 5+5*sin((float)i/w*M_PI*4+poison_x);
			verts[i*2+0].z = 0;
			verts[i*2+0].color = white;
			verts[i*2+0].u = i;
			verts[i*2+0].v = 0;
			verts[i*2+1].x = i;
			verts[i*2+1].y = verts[i*2+0].y + h;
			verts[i*2+1].z = 0;
			verts[i*2+1].color = white;
			verts[i*2+1].u = i;
			verts[i*2+1].v = h;

		}
		m_draw_prim(verts, NULL, poison_bmp, 0, w*2, ALLEGRO_PRIM_LINE_LIST);
		m_set_target_bitmap(poison_bmp_tmp2);
		m_clear(m_map_rgba(0, 0, 0, 0));
		for (int i = 0; i < h2; i++) {
			verts[i*2+0].x = 5+5*cos((float)i/h2*M_PI*4+poison_x);
			verts[i*2+0].y = i;
			verts[i*2+0].z = 0;
			verts[i*2+0].color = white;
			verts[i*2+0].u = 0;
			verts[i*2+0].v = i;
			verts[i*2+1].x = verts[i*2+0].x + w2;
			verts[i*2+1].y = i; 
			verts[i*2+1].z = 0;
			verts[i*2+1].color = white;
			verts[i*2+1].u = w2;
			verts[i*2+1].v = i;
		}
		m_draw_prim(verts, NULL, poison_bmp_tmp, 0, h2*2, ALLEGRO_PRIM_LINE_LIST);
		delete[] verts;
		m_pop_target_bitmap();
		ALLEGRO_COLOR col, tmp = poisonBlocks[0].color;
		col.r = tmp.r * tmp.a;
		col.g = tmp.g * tmp.a;
		col.b = tmp.b * tmp.a;
		col.a = tmp.a;
		m_draw_bitmap(poison_bmp_tmp2, dx-5, dy-5, 0);
	}
}

void Object::stop()
{
	moving = false;
	clearOccupied();
	addOccupied(x, y);
}


bool Object::update(Area *area, int step)
{
	if (animationSet)
		animationSet->update(step);

	if (!input) {
		return true;
	}
	
	InputDescriptor ie = input->getDescriptor();

	if (speechDialog || dpad_panning) {
		ie.left = ie.right = ie.up = ie.down = false;
		if (dpad_panning)
			ie.button1 = ie.button2 = false;
	}

	if (person) {
		if (moving) {
			moveCount += step;
			while (moveCount >= (int)moveDelay) {
				moveCount -= moveDelay;
				pixelsMoved++;
				if (pixelsMoved >= TILE_SIZE) {
					pixelsMoved = 0;
					ox = 0;
					oy = 0;
					int nowOccupiedx = 0;
					int nowOccupiedy = 0;
					switch (moveDirection) {
						case DIRECTION_NORTH:
							if (!ie.up || y-1 == dy) {
								moving = false;
							}
							y--;
							if (moving) {
								nowOccupiedx = x;
								nowOccupiedy = y-1;
							}
							break;
						case DIRECTION_EAST:
							if (!ie.right || x+1 == dx) {
								moving = false;
							}
							x++;
							if (moving) {
								nowOccupiedx = x+1;
								nowOccupiedy = y;
							}
							break;
						case DIRECTION_SOUTH:
							if (!ie.down || y+1 == dy) {
								moving = false;
							}

							y++;
							if (moving) {
								nowOccupiedx = x;
								nowOccupiedy = y+1;
							}
							break;
						case DIRECTION_WEST:
							if (!ie.left || x-1 == dx) {
								moving = false;
							}

							x--;
							if (moving) {
								nowOccupiedx = x-1;
								nowOccupiedy = y;
							}
							break;
						case DIRECTION_NONE:
							break;
					}
					if (moving) {
						if (area->isOccupied(_id, nowOccupiedx, nowOccupiedy)) {
							moving = false;
						}
						else {
							clearOccupied();
							addOccupied(nowOccupiedx, nowOccupiedy);
						}
					}
					break;
				}
				else {
					switch (moveDirection) {
						case DIRECTION_WEST:
							ox--;
							break;
						case DIRECTION_EAST:
							ox++;
							break;
						case DIRECTION_NORTH:
							oy--;
							break;
						case DIRECTION_SOUTH:
							oy++;
							break;
						case DIRECTION_NONE:
							break;
					}
				}
			}
		}
		else {
			if (ie.left || ie.right || ie.up || ie.down) {
				moving = false; // not really needed
				int nowOccupiedx = 0;
				int nowOccupiedy = 0;
				stood = false;
				switch (ie.direction) {
					case DIRECTION_WEST:
						nowOccupiedx = x - 1;
						nowOccupiedy = y;
						if (!area->isOccupied(_id, nowOccupiedx, nowOccupiedy)) {
							if (!animationSet->setSubAnimation("walk_w")) {
								animationSet->setSubAnimation("stand_w");
							}
							moving = true;
						}
						else {
							animationSet->setSubAnimation("stand_w");
						}
						break;
					case DIRECTION_EAST:
						nowOccupiedx = x + 1;
						nowOccupiedy = y;
						if (!area->isOccupied(_id, nowOccupiedx, nowOccupiedy)) {
							if (!animationSet->setSubAnimation("walk_e")) {
								animationSet->setSubAnimation("stand_e");
							}
							moving = true;
						}
						else {
							animationSet->setSubAnimation("stand_e");
						}
						break;
					case DIRECTION_NORTH:
						nowOccupiedx = x;
						nowOccupiedy = y-1;
						if (!area->isOccupied(_id, nowOccupiedx, nowOccupiedy)) {
							if (!animationSet->setSubAnimation("walk_n")) {
								animationSet->setSubAnimation("stand_n");
							}
							moving = true;
						}
						else {
							animationSet->setSubAnimation("stand_n");
						}
						break;
					case DIRECTION_SOUTH:
						nowOccupiedx = x;
						nowOccupiedy = y + 1;
						if (!area->isOccupied(_id, nowOccupiedx, nowOccupiedy)) {
							if (!animationSet->setSubAnimation("walk_s")) {
								animationSet->setSubAnimation("stand_s");
							}
							moving = true;
						}
						else {
							animationSet->setSubAnimation("stand_s");
						}
						break;
					case DIRECTION_NONE:
						break;
				}
				if (moving) {
					pixelsMoved = 0;
					moveCount = 0;
					moveDirection = ie.direction;
					clearOccupied();
					addOccupied(nowOccupiedx, nowOccupiedy);
				}
			}
			else if (!moving && !stood) {
				stood = true;
				switch (ie.direction) {
					case DIRECTION_NORTH:
						animationSet->setSubAnimation("stand_n");
						break;
					case DIRECTION_EAST:
						animationSet->setSubAnimation("stand_e");
						break;
					case DIRECTION_SOUTH:
						animationSet->setSubAnimation("stand_s");
						break;
					case DIRECTION_WEST:
						animationSet->setSubAnimation("stand_w");
						break;
					case DIRECTION_NONE:
						break;
				}
			}
		}
	}

	if (poisoned) {
		poison_x += 0.01 * step;
		int px = x;
		int py = y;
		if (poisonBlocks[4].x != px || poisonBlocks[4].y != py) {
			// do damage
			for (int i = 0; i < MAX_PARTY; i++) {
				Player *p = party[i];
				if (!p) {
					continue;
				}
				CombatantInfo &info = p->getInfo();
				if (info.condition != CONDITION_POISONED) {
					continue;
				}
				int damage = info.abilities.maxhp * 0.01f;
				if (damage < 1) {
					damage = 1;
				}
				info.abilities.hp -= damage;
				if (info.abilities.hp < 1) {
					info.abilities.hp = 1;
				}
			}
			for (int i = 0; i < 9; i++) {
				poisonBlocks[i].x = px + xoffsets[i];
				poisonBlocks[i].y = py + yoffsets[i];
				int n = 128 + rand() % 128;
				poisonBlocks[i].color = m_map_rgba(n, 0, n, 255);
			}
		}
		else {
			for (int i = 0; i < 9; i++) {
				if (poisonBlocks[i].color.a > 0.0f) {
					poisonBlocks[i].color.a -= 0.001f*step;
					if (poisonBlocks[i].color.a < 0.0f) {
						poisonBlocks[i].color.a = 0.0f;
					}
				}
			}
		}
	}

	return true;
}


void Object::setPoisoned(bool p)
{
	poisoned = p;
}

bool Object::getPoisoned()
{
	return poisoned;
}


bool Object::isPoisoned()
{
	return poisoned;
}


bool Object::isSpecialWalkable()
{
	return specialWalkable;
}

void Object::setSpecialWalkable(bool s)
{
	specialWalkable = s;
}


Object::Object()
{
	x = y = 0;
	ox = oy = 0;
	_id = findUniqueId();
	high = false;
	low = false;
	hidden = false;
	input = NULL;
	animationSet = NULL;
	w = TILE_SIZE;
	h = TILE_SIZE;
	moving = false;
	moveDelay = 12;
	addOccupied(x, y);
	solid = true;
	person = false;
	stood = false;
	dx = dy = -1;
	floater = false;
	poisoned = false;
	specialWalkable = false;
	pixelsMoved = 0;
}


Object::~Object()
{
	if (input)
		delete input;
	if (animationSet) {
		delete animationSet;
	}
	clearOccupied();
}

void Object::addOccupied(int occx, int occy)
{
	for (int x = 0; x < (w/TILE_SIZE); x++) {
		int *v = new int[2];
		v[0] = occx + x;
		v[1] = occy;
		occupied.push_back(v);
	}
}

void Object::clearOccupied()
{
	for (uint i = 0; i < occupied.size(); i++)
		delete[] occupied[i];
	occupied.clear();
}



// for fountains



bool SparklySpiral::update(Area *area, int step)
{
	std::vector<Sparkle>::iterator it;

	for (it = sparkles.begin(); it != sparkles.end();) {
		Sparkle &s = *it;
		if (s.stillCount > 0) {
			s.stillCount -= step;
		}
		else {
			s.radius += 0.07f * step;
			s.angle += 0.01f * step;
		}
		if (s.radius > 100) {
			it = sparkles.erase(it);
		}
		else {
			it++;
		}
	}

	if (sparkles.size() <= 0) {
		return false;
	}
	
	return true;
}


void SparklySpiral::draw()
{
	int cx = m_get_bitmap_width(bitmap)/2;
	int cy = m_get_bitmap_height(bitmap)/2;

	al_hold_bitmap_drawing(true);
	for (int i = 0; i < (int)sparkles.size(); i++) {
		int dx = x + cos(sparkles[i].angle) * sparkles[i].radius;
		int dy = y + sin(sparkles[i].angle) * sparkles[i].radius;
		dx -= area->getOriginX();
		dy -= area->getOriginY();
		m_draw_rotated_bitmap(bitmap, cx, cy,
			dx, dy, sparkles[i].angle, 0);
	}
	al_hold_bitmap_drawing(false);
}



SparklySpiral::SparklySpiral(float x, float y)
{
	this->x = x;
	this->y = y;

	high = true;

	bitmap = m_load_bitmap(getResource("media/sparkle.png"));

	int stillCount = 0;

	for (int i = 0; i < NUM_SPARKLES; i++) {
		Sparkle s;
		s.stillCount = stillCount;
		stillCount += 30;
		s.radius = 0.0f;
		s.angle = 0.0f;
		sparkles.push_back(s);
	}

	specialWalkable = false;
}


SparklySpiral::~SparklySpiral()
{
	m_destroy_bitmap(bitmap);
	sparkles.clear();
}


// smoke

bool Smoke::update(Area *area, int step)
{
	for (int i = 0; i < NUM_PUFFS; i++) {
		puffs[i].height += 0.02f * step;
		if (puffs[i].height > MAX_HEIGHT) {
			puffs[i].height -= MAX_HEIGHT;
			puffs[i].x = (rand() % 10) - 5;
		}
	}

	return true;
}


void Smoke::draw()
{
	int dx, dy;
	int half_w = m_get_bitmap_width(bitmap)/2;
	int half_h = m_get_bitmap_height(bitmap)/2;

	al_hold_bitmap_drawing(true);
	for (int i = 0; i < NUM_PUFFS; i++) {
		dx = x + cos((puffs[i].height / MAX_HEIGHT) * (M_PI*2) + puffs[i].add) * 5;
		dy = y - puffs[i].height;
		dx -= area->getOriginX();
		dy -= area->getOriginY();
		m_draw_bitmap(bitmap, dx-half_w, dy-half_h, 0);
	}
	al_hold_bitmap_drawing(false);
}



Smoke::Smoke(float x, float y)
{
	this->x = x;
	this->y = y;

	high = true;

	bitmap = m_load_alpha_bitmap(getResource("media/smoke.png"));

	for (int i = 0; i < NUM_PUFFS; i++) {
		puffs[i].height = rand() % MAX_HEIGHT;
		puffs[i].x = ((rand() % 10) - 5);
		puffs[i].add = ((rand() % 1000) / 1000.0f) * M_PI*2;
	}

	specialWalkable = false;
}


Smoke::~Smoke()
{
	m_destroy_bitmap(bitmap);
}

// Light 

bool Light::update(Area *area, int step)
{
	return true;
}


void Light::draw()
{
	m_push_blender();

	m_set_blender(M_ALPHA, M_ONE, m_map_rgba(255, 255, 255, alpha));

	m_draw_bitmap(bmp, x-area->getOriginX(), y-area->getOriginY(), 0);

	m_pop_blender();
}

Light::Light(float x, float y, int dir, int topw, int bottomw, int length, MCOLOR color) :
	Object(),
	x(x),
	y(y)
{
	high = true;

	bmp = create_trapezoid(dir, topw, bottomw, length, color);

	alpha = (int)(color.a * 255);

	specialWalkable = false;
}


Light::~Light()
{
	m_destroy_bitmap(bmp);
}


void Fish::draw()
{
	m_push_target_bitmap();

	m_set_target_bitmap(tmpbmp);

	m_clear(m_map_rgba(0, 0, 0, 0));

	al_hold_bitmap_drawing(true);
	for (int i = 0; i < w; i++) {
		int a = wiggle;
		a += (float)i/w * (M_PI*2);
		int o = sin((float)a) * MAX_WIGGLE;
		m_draw_bitmap_region(bmp, i, 0, 1, h, i, MAX_WIGGLE+o, 0);
	}
	al_hold_bitmap_drawing(false);

	m_pop_target_bitmap();

	ALLEGRO_COLOR tint = al_map_rgba(alpha, alpha, alpha, alpha);
	al_draw_tinted_rotated_bitmap(tmpbmp->bitmap, tint, w/2, (h+MAX_WIGGLE*2)/2,
		x-area->getOriginX(), y-area->getOriginY(), angle, 0);
}


bool Fish::update(Area *area, int step)
{
	if (waiting > 0) {
		waiting -= step;
		if (waiting <= 0) {
			remain = rand() % 40;
			angle = (float)rand() / RAND_MAX * (M_PI*2);
		}
	}
	else {
		if ((alpha_up && alpha < alpha_target) || (!alpha_up && alpha > alpha_target)) {
			if (alpha_up) {
				alpha += step * 0.128f;
			}
			else {
				alpha -= step * 0.128f;
			}
		}
		else {
			alpha_up = !alpha_up; 
			if (alpha_up) {
				alpha_target = 255;
			}
			else {
				alpha_target = 255 - (rand() % 128);
			}
		}


		float dx = cos(angle) * SPEED * step;
		float dy = sin(-angle) * SPEED * step;
		float dt = sqrt(dx*dx + dy*dy);

		remain -= dt;
		x += dx;
		y += dy;

		int aw = area->getWidth()*TILE_SIZE;
		int ah = area->getHeight()*TILE_SIZE;
		// check bounds
		if (x < 0 || y < 0 || x >= aw || y >= ah) {
			x -= dx;
			y -= dy;
		}
		else {
			// Check area for solids
			Tile *tile = area->getTile(x/TILE_SIZE, y/TILE_SIZE);
			if (tile->isSolid()) {
				x -= dx;
				y -= dy;
			}
		}

		if (remain <= 0) {
			waiting = 500 + rand() % 1000;
		}
	}

	wiggle += 0.01f * step;

	return true;
}


Fish::Fish(float x, float y)
{
	this->x = x;
	this->y = y;

	bmp = m_load_bitmap(getResource("objects/fish.png"));
	
	w = m_get_bitmap_width(bmp);
	h = m_get_bitmap_height(bmp);

	tmpbmp = m_create_bitmap(w, h+MAX_WIGGLE*2); // check

	alpha = 255;
	alpha_target = 255 - rand()%128;
	alpha_up = false;

	angle = (float)rand() / RAND_MAX * (M_PI*2);

	remain = rand() % 48;
	
	wiggle = (float)rand() / RAND_MAX * (M_PI*2);

	waiting = 0;

	specialWalkable = false;
}


Fish::~Fish()
{
	m_destroy_bitmap(bmp);
	m_destroy_bitmap(tmpbmp);
}


void Rocket::draw()
{
	if (!started) {
		animationSet->draw(x+ox-area->getOriginX(), y-animationSet->getHeight()-area->getOriginY(), 0);
		return;
	}

	y -= 20;

	// Draw flames
	int w = m_get_bitmap_width(flames);
	int h = m_get_bitmap_height(flames);
	int nrows = MIN(h, (start_y+PUFF_RADIUS)-y);
	int start = h - ((int)ofs % h);

	for (int i = 0; i < nrows; i++) {
		int width = (float)i/nrows * 40 + 40;
		int row = (start + i) % h;
		m_draw_scaled_bitmap(flames, 0, row, w, 1,
			x+animationSet->getWidth()/2-width/2-area->getOriginX(),
			y+i-area->getOriginY(),
			width, 1, 0, 255);
	}

	// Draw smoke puffs
	std::vector<RocketExhaustPuff>::iterator it;
	for (it = puffs.begin(); it != puffs.end(); it++) {
		RocketExhaustPuff &p = *it;
		m_draw_rotated_bitmap(puff,
			m_get_bitmap_width(puff)/2,
			m_get_bitmap_height(puff)/2,
			p.x-area->getOriginX(), p.y-area->getOriginY(), p.bmp_angle, 0);
	}

	y += 20;
		
	animationSet->draw(x+ox-area->getOriginX(), y-animationSet->getHeight()-area->getOriginY(), 0);
}


bool Rocket::update(Area *area, int step)
{
	if (!started)
		return true;
	
	count += step;

	if ((int)puffs.size() < MAX_PUFFS-1 && (int)puffs.size() < count/50) {
		RocketExhaustPuff p;
		int w = animationSet->getWidth();
		int x = this->x + w/2;
		p.cy = this->y;
		p.bmp_angle = (float)(rand() % RAND_MAX)/RAND_MAX * M_PI*2;
		if (rand() % 2) {
			p.angle = 0;
			p.da = 0.01f;
			p.cx = x - (rand() % w/2) - PUFF_RADIUS;
		}
		else {
			p.angle = M_PI;
			p.da = -0.01f;
			p.cx = x + (rand() % w/2) + PUFF_RADIUS;
		}
		puffs.push_back(p);
	}

	// update exhaust puffs

	std::vector<RocketExhaustPuff>::iterator it;

	for (it = puffs.begin(); it != puffs.end();) {
		RocketExhaustPuff &p = *it;
		p.angle += p.da * step;
		if (p.da < 0) {
			if (p.angle < 0) {
				it = puffs.erase(it);
				continue;
			}
		}
		else {
			if (p.angle > M_PI) {
				it = puffs.erase(it);
				continue;
			}
		}
		p.bmp_angle += p.da * step;
		p.x = p.cx + cos(p.angle) * PUFF_RADIUS;
		p.y = p.cy + sin(p.angle) * PUFF_RADIUS;
		it++;
	}

	ox = rand() % 3 - 1;

	// update position of ship
	vel += acc * step;
	y -= vel;

	// update flames

	ofs += 0.2f * step;

	if (count > 10000)
		return false;

	return true;
}


void Rocket::start()
{
	started = true;
	area_ox = area_oy = 0;
	count = 0;
	ofs = 0;
	acc = 0.0002f;
	vel = 0.0001f;
}


Rocket::Rocket(float x, float y, std::string spriteName)
{
	this->x = x;
	this->y = y;
	this->start_y = y;
	ox = 0;
	high = true;

	animationSet = new_AnimationSet(getResource("objects/%s.png", spriteName.c_str()));
	puff = m_load_bitmap(getResource("media/exhaust_puff.png"));
	flames = m_load_bitmap(getResource("media/exhaust_flames.png"));

	started = false;

	specialWalkable = false;
}


Rocket::~Rocket()
{
	m_destroy_bitmap(puff);
	m_destroy_bitmap(flames);
}
