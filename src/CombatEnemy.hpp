#ifndef COMBAT_ENEMY_HPP
#define COMBAT_ENEMY_HPP


class CombatEnemy : public Combatant {
public:
	static const int DIE_TIME = 600;
	
	virtual bool act(int step, Battle *b);
	virtual void draw(void);
	virtual void draw_shadow(void);
	bool update(int step);

	Element getStrength(void);
	Element getWeakness(void);
	void die(int attackerId);
	void initId(int id);
	bool spellHasNoEffect(std::string name);
	lua_State *getLuaState(void);
	void added(void);
	void start(void);

	void attacked(void);
	void mkdeath(void);

	CombatEnemy(std::string name, int x, int y, bool alpha);
	CombatEnemy(std::string name, int x, int y);
	virtual ~CombatEnemy();

protected:
	void construct(std::string name, int x, int y, bool alpha);

	void initLua(void);

	lua_State *luaState;
	Element strength;
	Element weakness;
	Spell *spell;
	int thinkCount;
	int deadCount;
	bool sample_played;
	CombatStatusInfo status;
	AnimationSet *charmAnim;
	ExplosionCircle *explosionCircles;
	int numExplosionCircles;
	int swingTime;
	bool spellInited;
	int numTargets;
	Combatant **targets;
	AnimationSet *oldAnim;
	int float_height;
	MBITMAP *work;
	bool death_blitted;
};


class CombatEnemyTode : public CombatEnemy
{
public:
	virtual void draw_shadow(void);
	void puke(void);
	void swallow(MBITMAP *bmp, float angle);
	void puke_next(void);
	void draw(void);
	bool act(int step, Battle *b);

	CombatEnemyTode(std::string name, int x, int y);
	virtual ~CombatEnemyTode();
private:
	MBITMAP *swallowed[4];
	float angles[4];
	bool pukenext;
};


#endif

