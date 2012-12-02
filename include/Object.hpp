#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "monster2.hpp"

class Area;


struct PoisonBlock {
	int x, y;
	MCOLOR color;
};



class Object
{

public:
	bool isSpecialWalkable(void);
	void setSpecialWalkable(bool s);
	void setDest(int dx, int dy);
        void setPosition(int x, int y);
	void setOffset(int x, int y);
	void setAnimationSet(std::string name);
	void setInput(Input *i);
	void setHigh(bool high);
	void setLow(bool low);
	void setHidden(bool hidden);
	void setSolid(bool solid);
	void setPerson(bool person);
	void stop(void);
	void setFloater(bool floater);
	void setPoisoned(bool poisoned);
	bool getPoisoned(void);
	void setDimensions(int x, int y);
        
        void getPosition(int *x, int *y);
	void getOffset(int *x, int *y);
	int getX(void);
	int getY(void);
        uint getId( void );
	bool isHigh(void);
	bool isLow(void);
	bool isHidden(void);
	Input *getInput(void);
	AnimationSet *getAnimationSet(void);
	std::vector<int *> &getOccupied(void);
	bool isSolid(void);
	bool isMoving(void);
	int getMoveDirection(void);
	void getDimensions(int *x, int *y);
	bool isPoisoned(void);

	void draw(float x, float y);
	virtual void draw(void);
	virtual void drawUpper(void);
	// return false to destroy
	virtual bool update(Area *area, int step);

	Object(void);
	virtual ~Object(void);


protected:
        
	void clearOccupied(void);
	void addOccupied(int x, int y);

	int x, y;
	int ox, oy;
	int w, h;
        uint _id;
	bool high;
	bool low;
	bool hidden;
	Input *input;
	AnimationSet *animationSet;
	bool moving;
	int moveCount;
	int pixelsMoved;
	uint moveDelay;
	int moveDirection;
	std::vector<int *> occupied;
	bool solid;
	bool person;
	bool stood;
	int dx, dy;
	bool floater;
	bool poisoned;

	PoisonBlock poisonBlocks[9];
	bool specialWalkable;
};


struct Sparkle
{
	int stillCount;
	float radius, angle;
};


class SparklySpiral : public Object
{
public:
	static const int NUM_SPARKLES = 35;
	bool update(Area *area, int step);
	void draw(void);

	SparklySpiral(float x, float y);
	virtual ~SparklySpiral(void);

protected:
	float x, y;
	MBITMAP *bitmap;
	std::vector<Sparkle> sparkles;

};


struct Puff {
	float height;
	float x;
	float add;
};


class Smoke : public Object
{
public:
	static const int NUM_PUFFS = 6;
	static const int MAX_HEIGHT = 25;
	bool update(Area *area, int step);
	void draw(void);

	Smoke(float x, float y);
	virtual ~Smoke(void);

protected:
	float x, y;
	MBITMAP *bitmap;
	Puff puffs[NUM_PUFFS];
};


class Light : public Object
{
public:
	bool update(Area *area, int step);
	void draw(void);

	Light(float x, float y, int dir, int topw, int bottomw, int length, MCOLOR color);
	virtual ~Light(void);

protected:
	float x, y;
	MBITMAP *bmp;
	int alpha;
};


class Fish : public Object
{
public:
	static const int MAX_WIGGLE = 3;
	static const float SPEED;
	bool update(Area *area, int step);
	void draw(void);

	Fish(float x, float y);
	virtual ~Fish(void);

protected:
	float x, y;
	MBITMAP *bmp;
	MBITMAP *tmpbmp;
	float alpha;
	float alpha_target;
	bool alpha_up;
	float angle;
	float remain;
	float wiggle;
	int w, h;
	float waiting;
};


struct RocketExhaustPuff
{
	float bmp_angle;
	float x, y;
	float cx, cy;
	float angle;
	float da; // angle delta
};

class Rocket : public Object
{
public:
	static const int MAX_PUFFS = 20;
	static const int PUFF_RADIUS = TILE_SIZE*2;

	void start(void);
	
	bool update(Area *area, int step);
	void draw(void);

	Rocket(float x, float y, std::string spriteName);
	virtual ~Rocket(void);

protected:
	int area_ox, area_oy;
	float start_y;
	float x, y;
	int ox;
	MBITMAP *puff;
	MBITMAP *flames;
	bool started;
	int count;
	std::vector< RocketExhaustPuff > puffs;
	float ofs; // flame offset
	float acc;
	float vel;
};

void resetIds(void);

#endif
