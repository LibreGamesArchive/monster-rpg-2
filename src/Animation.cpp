#include "monster2.hpp"

void Animation::setLooping(bool l)
{
	looping = l;
}


void Animation::setLoopMode(LoopMode m)
{
	loopMode = m;
}


int Animation::getLength(void)
{
	int l = 0;

	for (int i = 0; i < nFrames; i++) {
		Frame *f = frames[i];
		l += f->getDelay();
	}
	
	if (loopMode == LOOP_PINGPONG) {
		l = l + (l - frames[nFrames-1]->getDelay());
	}

	return l;
}


/* Returns false on fail.
 * frame is not copied so should not be destroyed.
 */
bool Animation::addFrame(Frame *frame)
{
	frames.push_back(frame);
	nFrames++;
	return true;
}


/* Returns how many frames passed
 * Can go backwards.
 * Step is in milliseconds.
 */
int Animation::update(int step)
{
	int passed = 0;

	if (step < 0) {
		count += step;
		while (count < 0) {
			passed++;
			currentFrame-=increment;
			wrap();
			int thisDelay = frames[currentFrame]->getDelay();
			count += thisDelay;
			if (thisDelay <= 0)
				break;
		}
	}
	else {
		count += step;
		int thisDelay = frames[currentFrame]->getDelay();
		while (count >= thisDelay) {
			if ((!looping && ((loopMode == LOOP_NORMAL && currentFrame == nFrames-1) || (loopMode == LOOP_PINGPONG && increment == -1 && currentFrame == 0))) ||
					(nFrames == 1))
				break;
			count -= thisDelay;
			if (thisDelay <= 0)
				break;
			passed++;
			if (!(looping == false && loopMode == LOOP_PINGPONG && increment == -1 && currentFrame <= 0))
				currentFrame+=increment;
			wrap();
			thisDelay = frames[currentFrame]->getDelay();
		}
	}

	return passed;
}


void Animation::setFrame(int frame)
{
	debug_message("frame set to %d\n", frame);
	currentFrame = frame;
}


void Animation::draw(int x, int y, int flags)
{
	if (alpha != 255)
		frames[currentFrame]->getImage()->draw_trans(x, y, alpha);
	else
		frames[currentFrame]->getImage()->draw(x, y, flags);
}


void Animation::drawRotated(int x, int y, float angle, int flags)
{
	MBITMAP *b = frames[currentFrame]->getImage()->getBitmap();
	int w = m_get_bitmap_width(b);
	int h = m_get_bitmap_height(b);

	m_draw_rotated_bitmap(b, w/2, h/2, x, y, angle, flags);
}


void Animation::reset(void)
{
	currentFrame = 0;
	count = 0;
	increment = 1;
}


int Animation::getAlpha(void)
{
	return alpha;
}


Animation::Animation(void)
{
}

Animation::Animation(const char *name, int alpha) :
	nFrames(0)
{
	this->name = std::string(name);
	currentFrame = 0;
	count = 0;
	this->alpha = alpha;
	looping = true;
	loopMode = LOOP_NORMAL;
	increment = 1;
}


/* Frames are destroyed
 */
Animation::~Animation(void)
{
	for (int i = 0; i < nFrames; i++) {
		delete frames[i];
	}

	frames.clear();
}


void Animation::wrap(void)
{
	if (loopMode == LOOP_NORMAL) {
		if (looping) {
			if (currentFrame < 0) {
				currentFrame = nFrames-1;
			}
			else if (currentFrame >= nFrames) {
				currentFrame = 0;
			}
		}
		else {
			if (currentFrame >= nFrames) {
				currentFrame = nFrames-1;
			}
		}
	}
	else if (loopMode == LOOP_PINGPONG) {
		if (looping) {
			if (currentFrame < 0) {
				currentFrame = nFrames == 1 ? 0 : 1;
				increment = 1;
			}
			else if (currentFrame >= nFrames) {
				currentFrame = nFrames == 1 ? nFrames-1 : nFrames-2;
				increment = -1;
			}
		}
		else {
			if (currentFrame >= nFrames) {
				currentFrame = nFrames-1;
				increment = -1;
			}
			else if (currentFrame < 0 && increment < 0) {
				currentFrame = 0;
			}
		}
		
	}
}

Animation *Animation::clone(int type, MBITMAP *clone_from, MBITMAP *clone_to, int x, int y, bool skip_draw)
{
	Animation *a = new Animation();
		
	Image *img = frames[0]->getImage();

	for (size_t i = 0; i < frames.size(); i++) {
		a->frames.push_back(frames[i]->clone(type, clone_from, clone_to, x, y, skip_draw));
		x += frames[i]->getImage()->getWidth() + 2;
	}

	a->nFrames = nFrames;
	a->currentFrame = currentFrame;
	a->count = count;
	a->name = name;
	a->alpha = alpha;
	a->looping = looping;
	a->loopMode = loopMode;
	a->increment = increment;
	a->tags = tags;

	a->reset();

	return a;
}
