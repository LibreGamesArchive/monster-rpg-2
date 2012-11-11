#include "monster2.hpp"


class GenericEffect : public CombatEntity {
public:
	virtual bool act(int step, Battle *b) { return false; }
	virtual bool update(int step) = 0;
	virtual void draw(void) = 0;
	virtual int getLifetime(void) = 0;
	virtual void finalize(Combatant *target) = 0;

	GenericEffect(Combatant *user, Combatant *target) {
		this->user = user;
		this->target = target;
		x = target->getX();
		y = target->getY()+1;
	}
	virtual ~GenericEffect() {}
protected:
	Combatant *user;
	Combatant *target;
};



class GenericCureEffect : public GenericEffect {
public:
	bool update(int step);
	void draw(void);
	int getLifetime(void);
	void finalize(Combatant *target);

	GenericCureEffect(Combatant *user, Combatant *target, int amount, std::string name, bool finish = false);
	virtual ~GenericCureEffect();
protected:
	AnimationSet *animation;
	int count;
	int amount;
	bool finish;
};




struct GenericHolyWaterPixel {
	float x, y, z;
	int offset;
};




class GenericHolyWaterEffect : public GenericEffect {
public:
	static const int NUM_PIXELS = 7;
	static const float RISE_SPEED;

	bool update(int step);
	void draw(void);
	int getLifetime(void);
	void finalize(Combatant *target);

	GenericHolyWaterEffect(Combatant *user, Combatant *target, bool finish = false);
	virtual ~GenericHolyWaterEffect();
protected:
	int count;
	GenericHolyWaterPixel *pixels;
	int numPixels;
	int maxDepth;
	bool finish;
};



struct GenericHealPixel {
	float a, radius;
	MCOLOR color;
};


class GenericHealEffect : public GenericEffect {
public:
	static const int NUM_PIXELS = 300;
	static const int MAX_OFS = 5;



	bool update(int step);
	void draw(void);
	int getLifetime(void);
	void finalize(Combatant *target);

	GenericHealEffect(Combatant *user, Combatant *target, bool finish = false);
	virtual ~GenericHealEffect();
protected:
	int count;
	GenericHealPixel pixels[NUM_PIXELS];
	float cx, cy;
	bool finish;
	float r;
};




class GenericElixirEffect : public GenericEffect {
public:
	bool update(int step);
	void draw(void);
	int getLifetime(void);
	void finalize(Combatant *target);

	GenericElixirEffect(Combatant *user, Combatant *target);
	virtual ~GenericElixirEffect();
protected:
	int count;
	MBITMAP *bitmap;
	int cx, cy;
	float angle, angle2;
	float xs[4];
	float ys[4];
};




