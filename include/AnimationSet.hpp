#ifndef ANIMSET_H
#define ANIMSET_H

#include "monster2.hpp"

class AnimationSet {
public:
	void setSubAnimation(int index);
	bool setSubAnimation(std::string subName);
	void setFrame(int frame);
	void setPrefix(std::string prefix);
	bool checkSubAnimationExists(std::string subName);

	std::string getSubName(void);
	int getFrame();
	Animation* getCurrentAnimation();
	int getWidth(); // gets width of current frame
	int getHeight(); // gets height of current frame

	void reset(void);
	void draw(int x, int y, int flags = 0);
	void drawRotated(int x, int y, float angle, int flags = 0);
	void drawScaled(int x, int y, int w, int h, int flags = 0);
	int update(int step);

	AnimationSet *clone(int type);
	void post_reset(void);

	AnimationSet(void);
	AnimationSet(const char *filename, bool alpha = false);
	~AnimationSet();
	
private:

	std::string name;
	std::vector<Animation *> anims;
	int currAnim;
	std::string prefix;
	bool destroy;
	MBITMAP *bitmap;
	std::string filename;
};

#endif
