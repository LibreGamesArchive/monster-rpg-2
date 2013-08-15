#ifndef COMBAT_ITEMS_HPP
#define COMBAT_ITEMS_HPP

#include "Combatant.hpp"


class ItemEffect : public CombatEntity {
public:
	virtual bool act(int step, Battle *battle) { return false; }
	virtual bool update(int step) = 0;
	
	virtual void init(Combatant *user, Combatant **targets, int numTargets) {
		this->user = user;
		this->targets = targets;
		this->numTargets = numTargets;
	};

	int getLifetime(void) { return lifetime; }
	void draw(void) {}

	ItemEffect(int itemIndex) {
		this->itemIndex = itemIndex;
	}

	virtual ~ItemEffect(void) {
		delete[] targets;
	};

protected:
	void apply(void);

	int itemIndex;
	Combatant *user;
	Combatant **targets;
	int numTargets;
	int lifetime;
	GenericEffect *g;
};


class CureEffect : public ItemEffect {
public:
	bool update(int step);
	void finalize(CombatPlayer *player);
	void init(Combatant *user, Combatant **targets, int numTargets);
	
	CureEffect(int itemIndex, int amount, std::string name);
	virtual ~CureEffect(void);

protected:
	int count;
	int amount;
	std::string name;
};


class HolyWaterEffect : public ItemEffect {
public:
	bool update(int step);
	void finalize(CombatPlayer *player);
	void init(Combatant *user, Combatant **targets, int numTargets);
	
	HolyWaterEffect(int itemIndex);
	virtual ~HolyWaterEffect(void);

protected:
	int count;
};



class HealEffect : public ItemEffect {
public:
	bool update(int step);
	void finalize(CombatPlayer *player);
	void init(Combatant *user, Combatant **targets, int numTargets);
	
	HealEffect(int itemIndex);
	virtual ~HealEffect(void);

protected:
	int count;
};


class ElixirEffect : public ItemEffect {
public:
	bool update(int step);
	void finalize(CombatPlayer *player);
	void init(Combatant *user, Combatant **targets, int numTargets);
	
	ElixirEffect(int itemIndex);
	virtual ~ElixirEffect(void);

protected:
	int count;
};


class JuiceEffect : public ItemEffect {
public:
	bool update(int step);
	void finalize(CombatPlayer *player);
	void init(Combatant *user, Combatant **targets, int numTargets);
	
	JuiceEffect(int itemIndex);
	virtual ~JuiceEffect(void);

protected:
	int count;
};



class MeatballsEffect : public ItemEffect {
public:
	bool update(int step);
	void finalize(CombatPlayer *player);
	void init(Combatant *user, Combatant **targets, int numTargets);
	
	MeatballsEffect(int itemIndex);
	virtual ~MeatballsEffect(void);

protected:
	int count;
};



ItemEffect *createItemEffect(int index);

#endif

