#ifndef ANIMATION_HPP
#define ANIMATION_HPP


enum LoopMode {
	LOOP_NORMAL = 0,
	LOOP_PINGPONG
};


class Animation {
public:
	void displayConvert(void) {
		for (int i = 0; i < (int)frames.size(); i++) {
			frames[i]->displayConvert();
		}
	}

	/* Tags */
	void setTags(std::vector<std::string> &tags) {
		this->tags = tags;
	}

	std::vector<std::string> getTags(void)
	{
		return tags;
	}

	bool hasTag(std::string tagName) {
		for (int i = 0; i < (int)tags.size(); i++) {
			if (tags[i] == tagName)
				return true;
		}

		return false;
	}

	/* Returns false on fail.
	 * frame is not copied so should not be destroyed.
	 */
	bool addFrame(Frame *frame);
	int getLength(void);

	/* -1 for current
	 */
	Frame *getFrame(int num)
	{
		return frames[num];
	}

	Frame *getCurrentFrame(void)
	{
		return frames[currentFrame];
	}

	bool isFinished(void) {
		if ((!looping && ((loopMode != LOOP_PINGPONG && currentFrame == nFrames-1 &&
		count >= frames[currentFrame]->getDelay()) ||
		(loopMode == LOOP_PINGPONG && increment == -1 &&
			currentFrame == 0 && count >= frames[0]->getDelay()))) ||
		(nFrames == 1 && count >= 100))
			return true;
		return false;
	}


	unsigned int getCurrentFrameNum(void)
	{
		return currentFrame;
	}


	unsigned int getNumFrames(void)
	{
		return nFrames;
	}


	std::string getName(void)
	{
		return name;
	}


	/* Returns how many frames passed
	 * Can go backwards.
	 * Step is in milliseconds.
	 */
	int update(int step);

	void setLooping(bool l);
	void setLoopMode(LoopMode m);

	void reset(void);
	void setFrame(int frame);
	void draw(int x, int y, int flags = 0);
	void drawRotated(int x, int y, float angle, int flags = 0);
	
	int getAlpha(void);

	Animation *clone(int type, MBITMAP *clone_from, MBITMAP *clone_to, int x, int y);

	Animation(void);
	Animation(const char *name, int alpha);
	/* Frames are destroyed
	 */
	~Animation(void);

protected:
	void wrap(void);

	std::vector<Frame *> frames;
	int nFrames;
	int currentFrame;
	int count;
	std::string name;
	int alpha; // only respected when drawn with draw(...)
	bool looping;
	LoopMode loopMode;
	int increment;
	std::vector<std::string> tags;
};

#endif

