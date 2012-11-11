#include "monster2.hpp"

#define ASSERT ALLEGRO_ASSERT
#include <allegro5/internal/aintern_opengl.h>

#include "quantize.hpp"

void AnimationSet::setSubAnimation(int anim)
{
	currAnim = anim;
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


void AnimationSet::drawRotated(int x, int y, float angle, int flags)
{
	anims[currAnim]->drawRotated(x, y, angle, flags);
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

AnimationSet::AnimationSet(const char *filename, bool alpha) :
	currAnim(0),
	prefix("")
{
	this->filename = filename;

	all_animsets.push_back(this);

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
	all_animsets.push_back(this);
}

AnimationSet::~AnimationSet()
{
	for (size_t i = 0; i < all_animsets.size(); i++) {
		if (all_animsets[i] == this) {
			all_animsets.erase(all_animsets.begin() + i);
			break;
		}
	}

	for (uint i = 0; i < anims.size(); i++) {
		delete anims[i];
	}
	anims.clear();

	m_destroy_bitmap(bitmap);
}

AnimationSet *AnimationSet::clone(int type)
{
	AnimationSet *a = new AnimationSet();

	a->filename = filename;
	
	int flags = al_get_new_bitmap_flags();
	al_set_new_bitmap_flags(flags & ~ALLEGRO_NO_PRESERVE_TEXTURE);
	a->bitmap = m_load_bitmap(filename.c_str());
	al_set_new_bitmap_flags(flags);
	
	if (type != CLONE_FULL) {
		MBITMAP *tmp = m_clone_bitmap(a->bitmap);
		ALLEGRO_BITMAP *target = al_get_target_bitmap();
		m_set_target_bitmap(a->bitmap);
		add_blit(tmp, 0, 0, white, 0.7, 0);
		m_destroy_bitmap(tmp);
		al_set_target_bitmap(target);
	}
	
	a->name = name;

	for (size_t i = 0; i < anims.size(); i++) {
		a->anims.push_back(anims[i]->clone(type, a->bitmap));
	}

	a->currAnim = currAnim;
	a->prefix = prefix;
	a->destroy = destroy;

	return a;
}

void AnimationSet::post_reset(void)
{
	for (size_t i = 0; i < anims.size(); i++) {
		int n = anims[i]->getNumFrames();
		for (int j = 0; j < n; j++) {
			MBITMAP *bmp = anims[i]->getFrame(j)->getImage()->getBitmap();
			ALLEGRO_BITMAP_EXTRA_OPENGL *extra = (ALLEGRO_BITMAP_EXTRA_OPENGL *)bmp->bitmap->extra;
			extra->texture = al_get_opengl_texture(bitmap->bitmap);
		}
	}
}

