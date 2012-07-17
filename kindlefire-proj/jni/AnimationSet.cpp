#include "monster2.hpp"

#define ASSERT ALLEGRO_ASSERT
#include <allegro5/internal/aintern_opengl.h>

#include "quantize.hpp"

static std::vector<AnimationSet *> all_animsets;

void animset_post_reset(void)
{
/*
	for (size_t i = 0; i < all_animsets.size(); i++) {
		all_animsets[i]->post_reset();
	}
*/
}

/*
ALLEGRO_BITMAP *make_paletted(ALLEGRO_BITMAP *bitmap)
{
#ifdef ALLEGRO_ANDROID
	// Convert to an 8 bit paletted texture
	int w = al_get_bitmap_width(bitmap);
	int h = al_get_bitmap_height(bitmap);
	int true_w, true_h;
	al_get_opengl_texture_size(bitmap, &true_w, &true_h);
	int sz = (256*4) + (true_w * true_h);
	unsigned char *imgdata = new unsigned char[sz];
	ALLEGRO_DEBUG("Trying palette stuff 2");
	ALLEGRO_DEBUG("Trying palette stuff 3");
	gen_palette(bitmap, imgdata);
	ALLEGRO_DEBUG("Trying palette stuff 4");
	gen_paletted_image(bitmap, imgdata, imgdata+(256*4));
	ALLEGRO_DEBUG("Trying palette stuff 5");
	ALLEGRO_DEBUG("Trying palette stuff 6");
	Paletted_Image_Data p;
	p.data = imgdata;
	p.size = sz;
	ALLEGRO_DEBUG("Trying palette stuff 7");
	ALLEGRO_BITMAP *pal = al_create_custom_bitmap(w, h, upload_paletted_image, &p);
	ALLEGRO_DEBUG("Trying palette stuff 8");
	ALLEGRO_DEBUG("pal=%p", pal);
	delete[] imgdata;

	al_destroy_bitmap(bitmap);
	return pal;
#else
	return bitmap;
#endif
}
*/

void AnimationSet::displayConvert(void)
{
	for (int i = 0; i < (int)anims.size(); i++) {
		anims[i]->displayConvert();
	}
}

void AnimationSet::setSubAnimation(int anim)
{
	currAnim = anim;
}



/*
 * Returns true if an animation by this name exists
 */
bool AnimationSet::setSubAnimation(std::string subName)
{
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
	/*
	if (anims[currAnim]->getName() == "Dragon_transform") {
		if ((int)anims[currAnim]->getCurrentFrameNum() == (int)anims[currAnim]->getLength()-1) {
			setSubAnimation("stand");
		}
	}
	*/
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
	//currAnim = 0;
	anims[currAnim]->reset();
}

/*
void redrawAnimSet(MBITMAP *bitmap, RecreateData *d)
{
	ALLEGRO_DEBUG("redrawAnimSet bitmap=%p d=%p", bitmap, d);

	RedrawData *data = (RedrawData *)d;

	ALLEGRO_DEBUG("data=%p", data);
	
	bitmap->bitmap = make_paletted(bitmap->bitmap);

	data->call_count++;
	ALLEGRO_DEBUG("After inc, call count = %d", data->call_count);
	if (data->call_count == 1) {
		ALLEGRO_DEBUG("Call count = 1, returning");
		return;
	}

	AnimationSet *animSet = data->animSet;

	ALLEGRO_DEBUG("animSet=%p size=%d", animSet, animSet->anims.size());

	for (size_t i = 0; i < animSet->anims.size(); i++) {
		ALLEGRO_DEBUG("i=%d", i);
		Animation *a = animSet->anims[i];
		ALLEGRO_DEBUG("Animation = %p", a);
		int n = a->getNumFrames();
		ALLEGRO_DEBUG("n = %d", n);
		for (int j = 0; j < n; j++) {
			ALLEGRO_DEBUG("j=%d", j);
			Frame *f = a->getFrame(j);
			ALLEGRO_DEBUG("f=%p", f);
			Image *img = f->getImage();
			ALLEGRO_DEBUG("refreshing frame %d of animation %d", j, i);
			img->refresh();
			ALLEGRO_DEBUG("done refreshing (%d %d)", j, i);
		}
	}

	ALLEGRO_DEBUG("Done redrawAnimSet");
}

void redrawAnimSetWhite(MBITMAP *bitmap, RecreateData *d)
{
	ALLEGRO_DEBUG("redrawAnimSet bitmap=%p d=%p", bitmap, d);

	RedrawData *data = (RedrawData *)d;

	ALLEGRO_DEBUG("data=%p", data);
	
	ALLEGRO_BITMAP *target = al_get_target_bitmap();
	ALLEGRO_DEBUG("-1");
	m_set_target_bitmap(bitmap);
	ALLEGRO_DEBUG("-2");
	int flags = al_get_new_bitmap_flags();
	ALLEGRO_DEBUG("-3");
	//al_set_new_bitmap_flags(flags | ALLEGRO_MEMORY_BITMAP);
	ALLEGRO_DEBUG("-4");
	MBITMAP *tmpbmp = m_clone_bitmap(bitmap);
	ALLEGRO_DEBUG("-5 tmpbmp=%p", tmpbmp);
	add_blit(tmpbmp, 0, 0, white, 0.7, 0);
	ALLEGRO_DEBUG("-6");
	al_set_target_bitmap(target);
	ALLEGRO_DEBUG("-7");
	al_set_new_bitmap_flags(flags);
	ALLEGRO_DEBUG("-8");
	m_destroy_bitmap(tmpbmp);
	ALLEGRO_DEBUG("-9");
	bitmap->bitmap = make_paletted(bitmap->bitmap);
	ALLEGRO_DEBUG("-10");

	data->call_count++;
	ALLEGRO_DEBUG("After inc, call count = %d", data->call_count);
	if (data->call_count == 1) {
		ALLEGRO_DEBUG("Call count = 1, returning");
		return;
	}

	AnimationSet *animSet = data->animSet;

	ALLEGRO_DEBUG("animSet=%p size=%d", animSet, animSet->anims.size());

	for (size_t i = 0; i < animSet->anims.size(); i++) {
		ALLEGRO_DEBUG("i=%d", i);
		Animation *a = animSet->anims[i];
		ALLEGRO_DEBUG("Animation = %p", a);
		int n = a->getNumFrames();
		ALLEGRO_DEBUG("n = %d", n);
		for (int j = 0; j < n; j++) {
			ALLEGRO_DEBUG("j=%d", j);
			Frame *f = a->getFrame(j);
			ALLEGRO_DEBUG("f=%p", f);
			Image *img = f->getImage();
			ALLEGRO_DEBUG("refreshing frame %d of animation %d", j, i);
			img->refresh();
			ALLEGRO_DEBUG("done refreshing (%d %d)", j, i);
		}
	}

	ALLEGRO_DEBUG("Done redrawAnimSet");
}
*/

AnimationSet::AnimationSet(const char *filename, bool alpha) :
	currAnim(0),
	prefix("")
{
	this->filename = filename;

	all_animsets.push_back(this);

	//RedrawData *data = new RedrawData();
	//data->animSet = this;
	//data->call_count = 0;
	//ALLEGRO_DEBUG("filename='%s'", filename);
	//bitmap = m_load_bitmap_redraw(filename, redrawAnimSet, data);

	int flags = al_get_new_bitmap_flags();
	al_set_new_bitmap_flags(flags & ~ALLEGRO_NO_PRESERVE_TEXTURE);
	bitmap = m_load_bitmap(filename);
	al_set_new_bitmap_flags(flags);
	
	//bitmap->bitmap = make_paletted(bitmap->bitmap);

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

	//RedrawData *data = new RedrawData();
	//data->animSet = a;
	//data->call_count = 0;
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
	
	//a->bitmap->bitmap = make_paletted(a->bitmap->bitmap);

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
	//bitmap->bitmap = make_paletted(bitmap->bitmap);

	for (size_t i = 0; i < anims.size(); i++) {
		int n = anims[i]->getNumFrames();
		for (int j = 0; j < n; j++) {
			MBITMAP *bmp = anims[i]->getFrame(j)->getImage()->getBitmap();
			ALLEGRO_BITMAP_EXTRA_OPENGL *extra = (ALLEGRO_BITMAP_EXTRA_OPENGL *)bmp->bitmap->extra;
			extra->texture = al_get_opengl_texture(bitmap->bitmap);
		}
	}
}
