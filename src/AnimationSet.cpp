#include "monster2.hpp"

static void draw_bitmap_with_borders(MBITMAP *bmp, int x, int y)
{
	#define BMP_W m_get_bitmap_width(bmp)
	#define BMP_H m_get_bitmap_height(bmp)

	int sides[4][6] = {
		{ /* src */ 0, 0, BMP_W, 1, /* dest */ 0, -1 }, // top
		{ /* src */ 0, BMP_H-1, BMP_W, 1, /* dest */ 0, BMP_H }, // bottom
		{ /* src */ 0, 0, 1, BMP_H, /* dest */ -1, 0 }, // left
		{ /* src */ BMP_W-1, 0, 1, BMP_H, /* dest */ BMP_W, 0 } // right
	};

	int corners[4][4] = {
		{ /* src */ 0, 0, /* dest */ -1, -1 }, // top left
		{ /* src */ BMP_W-1, 0, /* dest */ BMP_W, -1 }, // top right
		{ /* src */ 0, BMP_H-1, /* dest */ -1, BMP_H }, // bottom left
		{ /* src */ BMP_W-1, BMP_H-1, /* dest */ BMP_W, BMP_H } // bottom right
	};

	#undef BMP_W
	#undef BMP_H

	// do sides
	for (int i = 0; i < 4; i++) {
		quick_draw(
			bmp->bitmap,
			sides[i][0],
			sides[i][1],
			sides[i][2],
			sides[i][3],
			x+sides[i][4],
			y+sides[i][5],
			0
		);
	}

	// do corners
	for (int i = 0; i < 4; i++) {
		quick_draw(
			bmp->bitmap,
			corners[i][0],
			corners[i][1],
			1,
			1,
			x+corners[i][2],
			y+corners[i][3],
			0
		);
	}

	quick_draw(bmp->bitmap, x, y, 0);
}

MBITMAP *AnimationSet::getBitmap()
{
	return bitmap;
}

void AnimationSet::setSubAnimation(int anim)
{
	currAnim = anim;
}

int AnimationSet::getNumAnims()
{
	return (int)anims.size();
}

/*
 * Returns true if an animation by this name exists
 */
bool AnimationSet::setSubAnimation(std::string subName)
{
	/* MORPG2: this small hack makes Eny hold her hands up in the first battle */
	if (battle && battle->getName() == "first_battle" && subName == "stand" && !checkSubAnimationExists("cast"))
		subName = "stand2";

	std::string sub = prefix + subName;

	for (uint i = 0; i < anims.size(); i++) {
		if (anims[i]->getName() == sub) {
			currAnim = i;
			return true;
		}
	}

	// If not found with prefix, looks for it without
	sub = subName;

	for (uint i = 0; i < anims.size(); i++) {
		if (anims[i]->getName() == sub) {
			currAnim = i;
			return true;
		}
	}

	debug_message("Sub anim %s not found\n", sub.c_str());

	return false;
}


/*
 * Returns true if an animation by this name exists
 */
bool AnimationSet::checkSubAnimationExists(std::string subName)
{
	std::string sub = prefix + subName;

	for (uint i = 0; i < anims.size(); i++) {
		if (anims[i]->getName() == sub) {
			return true;
		}
	}

	return false;
}


std::string AnimationSet::getSubName(void)
{
	return anims[currAnim]->getName();
}



void AnimationSet::setFrame(int frame)
{
	anims[currAnim]->setFrame(frame);
}



int AnimationSet::getFrame()
{
	return anims[currAnim]->getCurrentFrameNum();
}



void AnimationSet::draw(int x, int y, int flags)
{
	anims[currAnim]->draw(x, y, flags);
}


void AnimationSet::drawRotated(MCOLOR tint, int x, int y, float angle, int flags)
{
	anims[currAnim]->drawRotated(tint, x, y, angle, flags);
}

void AnimationSet::drawScaled(int x, int y, int w, int h, int flags)
{
	MBITMAP *bmp = anims[currAnim]->getCurrentFrame()->getImage()->getBitmap();
	m_draw_scaled_bitmap(bmp, 0, 0, m_get_bitmap_width(bmp), m_get_bitmap_height(bmp),
		x, y, w, h, flags, 255);
}


// Returns how many frames have passed
int AnimationSet::update(int step)
{
	return anims[currAnim]->update(step);
}



Animation* AnimationSet::getCurrentAnimation(void)
{
	return anims[currAnim];
}



// Gets the width of the current frame
int AnimationSet::getWidth(void)
{
	return anims[currAnim]->getCurrentFrame()->getImage()->getWidth();
}



// Gets the height of the current frame
int AnimationSet::getHeight(void)
{
	return anims[currAnim]->getCurrentFrame()->getImage()->getHeight();
}


void AnimationSet::setPrefix(std::string prefix)
{
	this->prefix = prefix;
}


void AnimationSet::reset(void)
{
	anims[currAnim]->reset();
}

AnimationSet *new_AnimationSet(const char *filename, bool alpha, CloneType type)
{
	if (object_atlas_map.find(std::string(filename)) != object_atlas_map.end()) {
		type = CLONE_OBJECT;
	}
	AnimationSet *tmp = new AnimationSet(filename, alpha);
	AnimationSet *a = tmp->clone(type); // creates 1px border around every frame
	delete tmp;
	return a;
}

AnimationSet::AnimationSet(const char *filename, bool alpha) :
	currAnim(0),
	prefix("")
{
	clone_type = CLONE_NOT_A_CLONE;
	this->filename = filename;

	int flags = al_get_new_bitmap_flags();
	al_set_new_bitmap_flags(flags & ~ALLEGRO_NO_PRESERVE_TEXTURE);
	bitmap = m_load_bitmap(filename);
	al_set_new_bitmap_flags(flags);
	
	std::vector<int> delays;
	XMLData *xml;
	std::string filenameS = std::string(filename);
	std::string xml_filename = filenameS + ".xml";

	xml = new XMLData(xml_filename);
	if (!xml) {
		debug_message("Error loading xml %s.\n", xml_filename.c_str());
		return;
	}

	debug_message("Loaded AnimationSet descriptor %s.\n", xml_filename.c_str());

	std::list<XMLData *> &nodes = xml->getNodes();

	std::list<XMLData *>::iterator it;

	for (it = nodes.begin(); it != nodes.end(); it++) {
		XMLData *sub = *it;
		XMLData *tmp;
		std::string name = sub->getName();
		tmp = sub->find("x");
		std::string xS = tmp->getValue();
		int x = atoi(xS.c_str());
		tmp = sub->find("y");
		std::string yS = tmp->getValue();
		int y = atoi(yS.c_str());
		tmp = sub->find("width");
		std::string wS = tmp->getValue();
		int width = atoi(wS.c_str());
		tmp = sub->find("height");
		std::string hS = tmp->getValue();
		int height = atoi(hS.c_str());
		tmp = sub->find("frames");
		std::string framesS = tmp->getValue();
		int frames = atoi(framesS.c_str());
		bool looping = true;
		LoopMode loopMode = LOOP_NORMAL;
		tmp = sub->find("looping");
		if (tmp) {
			if (tmp->getValue() == "false") {
				looping = false;
			}
		}
		tmp = sub->find("loop_mode");
		if (tmp) {
			if (tmp->getValue() == "pingpong") {
				loopMode = LOOP_PINGPONG;
			}
		}
		if (frames > 1) {
			XMLData *delayNode = sub->find("delays");
			XMLData *tmp2;
			for (int i = 0; i < frames; i++) {
				char n[100];
				sprintf(n, "%d", i);
				std::string numS = std::string(n);
				tmp2 = delayNode->find(numS);
				std::string dS = tmp2->getValue();
				int d = atoi(dS.c_str());
				delays.push_back(d);
			}
		}
			else
			delays.push_back(0);
		Animation *anim = new Animation(name.c_str(), 255);

		for (int i = 0; i < frames; i++) {
			Image *image;
			if (alpha)
				image = new Image(true);
			else
				image = new Image();
			image->setTransparent(true);
			int x1 = x+(i*width);
			int y1 = y;
			int x2 = x1+width;
			int y2 = y + height;
			image->load(bitmap, x1, y1, x2, y2);
			Frame *frame = new Frame(image, delays[i]);
			anim->addFrame(frame);
		}

		anim->setLooping(looping);
		anim->setLoopMode(loopMode);
		/* Add 'user defined' tags to animation */
		std::list<XMLData *> nodes = sub->getNodes();
		std::list<XMLData *>::iterator it3 = nodes.begin();
		std::vector<std::string> tags;
		for (; it3 != nodes.end(); it3++) {
			XMLData *node = *it3;
			if (node->getName() == "tag") {
				tags.push_back(node->getValue());
			}
		}
		anim->setTags(tags);
		delays.clear();
		anims.push_back(anim);
	}

	delete xml;

	debug_message("Animation set %s loaded\n", filenameS.c_str());
}

AnimationSet::AnimationSet()
{
	clone_type = CLONE_NOT_A_CLONE;
}

AnimationSet::~AnimationSet()
{
	for (uint i = 0; i < anims.size(); i++) {
		delete anims[i];
	}

	if (clone_type != CLONE_OBJECT) {
		m_destroy_bitmap(bitmap);
	}
}
	
struct Order {
	int y;
	int anim_num;
};

static int sort(const void *a, const void *b)
{
	Order *o1 = (Order *)a;
	Order *o2 = (Order *)b;
	return o1->y - o2->y;
}

AnimationSet *AnimationSet::clone(int type)
{
	AnimationSet *a = new AnimationSet();

	a->filename = filename;
	a->name = name;
	a->currAnim = currAnim;
	a->prefix = prefix;
	a->destroy = destroy;
	a->clone_type = (CloneType)type;
	
	MBITMAP *clone_from, *clone_to;
	MBITMAP *tmp = NULL;

	if (type == CLONE_FULL) {
		tmp = m_load_bitmap(filename.c_str());

		int extra_y = (int)anims.size()*2; // max it can be
		int minw = INT_MAX;

		for (size_t anim = 0; anim < anims.size(); anim++) {
			int numFrames = anims[anim]->getNumFrames();
			for (int frame = 0; frame < numFrames; frame++) {
				Image *img = anims[anim]->getFrame(frame)->getImage();
				int w = img->getWidth();
				if (w < minw) minw = w;
			}
		}

		int extra_x = (m_get_bitmap_width(tmp)/minw+1)*2;

		int w = m_get_bitmap_width(tmp) + extra_x;
		int h = m_get_bitmap_height(tmp) + extra_y;

		int flags = al_get_new_bitmap_flags();
		al_set_new_bitmap_flags(flags & ~ALLEGRO_NO_PRESERVE_TEXTURE);
		a->bitmap = m_create_bitmap(w, h);
		al_set_new_bitmap_flags(flags);
		ALLEGRO_BITMAP *old_target = al_get_target_bitmap();
		al_set_target_bitmap(a->bitmap->bitmap);
		m_clear(m_map_rgba(0, 0, 0, 0));
		al_set_target_bitmap(old_target);
		
		clone_from = tmp;
		clone_to = a->bitmap;
	}
	else if (type == CLONE_OBJECT) {
		int id = object_atlas_map[std::string(filename)];
		ATLAS_ITEM *item = atlas_get_item_by_id(object_atlas, id);
		a->bitmap = atlas_get_item_sub_bitmap(item);
		clone_to = a->bitmap;
		clone_from = a->bitmap;
	}
	else if (type == CLONE_COPY_BORDERS) {
		tmp = m_clone_bitmap(bitmap);
		int flags = al_get_new_bitmap_flags();
		al_set_new_bitmap_flags(flags & ~ALLEGRO_NO_PRESERVE_TEXTURE);
		a->bitmap = m_create_bitmap(
			m_get_bitmap_width(tmp),
			m_get_bitmap_height(tmp)
		);
		al_set_new_bitmap_flags(flags);
		ALLEGRO_BITMAP *target = al_get_target_bitmap();
		m_set_target_bitmap(a->bitmap);
		m_clear(m_map_rgba(0, 0, 0, 0));
		for (size_t i = 0; i < anims.size(); i++) {
			Animation *a = anims[i];
			for (size_t j = 0; j < a->getNumFrames(); j++) {
				Frame *f = a->getFrame((int)j);
				Image *img = f->getImage();
				draw_bitmap_with_borders(
					img->getBitmap(),
					img->getX(),
					img->getY()
				);
			}
		}
		// for each frame redraw
		m_destroy_bitmap(tmp);
		al_set_target_bitmap(target);
		clone_from = a->bitmap;
		clone_to = a->bitmap;
	}
	else { // CLONE_PLAYER or CLONE_ENEMY
		tmp = m_clone_bitmap(bitmap);
		int flags = al_get_new_bitmap_flags();
		al_set_new_bitmap_flags(flags & ~ALLEGRO_NO_PRESERVE_TEXTURE);
		a->bitmap = m_create_bitmap(
			m_get_bitmap_width(tmp),
			m_get_bitmap_height(tmp)
		);
		al_set_new_bitmap_flags(flags);
		ALLEGRO_BITMAP *target = al_get_target_bitmap();
		m_set_target_bitmap(a->bitmap);
		m_clear(m_map_rgba(0, 0, 0, 0));
		add_blit(tmp, 0, 0, white, 0.7, 0);
		m_destroy_bitmap(tmp);
		al_set_target_bitmap(target);
		clone_from = a->bitmap;
		clone_to = a->bitmap;
	}

	Order *o;
	
	o = new Order[anims.size()];

	for (size_t i = 0; i < anims.size(); i++) {
		Order ord;
		ord.y = anims[i]->getFrame(0)->getImage()->getY();
		ord.anim_num = (int)i;
		o[i] = ord;
	}

	qsort(o, anims.size(), sizeof(Order), sort);

	for (size_t i = 0; i < anims.size(); i++) {
		int x, y;
		Image *img = anims[i]->getFrame(0)->getImage();
		bool found_dup = false;

		int oidx = 0;
		for (size_t j = 0; j < anims.size(); j++) {
			if (o[j].anim_num == (int)i) {
				oidx = (int)j;
				break;
			}
		}
		y = img->getY() + oidx*2 + 1;
		x = img->getX();
		for (size_t j = 0; j < i; j++) {
			Animation *anim = a->anims[j];
			int nf = anim->getNumFrames();
			for (int f = 0; f < nf; f++) {
				Image *img2 = anim->getFrame(f)->getImage();
				if (img->getX() == img2->getOrigX() && img->getY() == img2->getOrigY()) {
					x = img2->getX();
					y = img2->getY();
					found_dup = true;
					break;
				}
				if (y >= img2->getY()+img2->getHeight() || y+img->getHeight() <= img2->getY()) {
					continue;
				}
				if (img2->getX()+img2->getWidth() > x) {
					x = img2->getX()+img2->getWidth();
				}
			}
			if (found_dup) {
				break;
			}
		}
		if (!found_dup) {
			x++;
		}

		a->anims.push_back(anims[i]->clone(type, clone_from, clone_to, x, y, found_dup || (type == CLONE_OBJECT)));
	}

	delete[] o;

	if (type == CLONE_FULL) {
		m_destroy_bitmap(tmp);
	}

	return a;
}

