#ifndef COMBAT_SPELLS_HPP
#define COMBAT_SPELLS_HPP

#include "Combatant.hpp"


enum SpellAlignment {
	SPELL_WHITE = 0,
	SPELL_BLACK
};


struct BoltPoint {
	int x, y;
	int r, max;
	std::vector<BoltPoint> branch;
};


class GenericCureEffect;
class GenericHealEffect;
class GenericHolyWaterEffect;


class Spell {
public:
	virtual bool update(int step) = 0;
	virtual void apply(void) = 0;
	virtual void init(Combatant *caster, Combatant **targets, int numTargets);
	virtual ~Spell(void);

protected:
	Combatant *caster;
	Combatant **targets;
	int numTargets;
};


class LightningSpell : public Spell {
public:
	bool update(int step);
	void init(Combatant *caster, Combatant **targets, int numTargets);
	void apply(void);

	LightningSpell(void);
	virtual ~LightningSpell(void);

protected:
	int count;
	int lifetime;
};


class Bolt2Spell : public Spell {
public:
	bool update(int step);
	void init(Combatant *caster, Combatant **targets, int numTargets);
	void apply(void);

	Bolt2Spell(void);
	virtual	~Bolt2Spell(void);

protected:
	int count;
	int lifetime;
};


class Ice1Spell : public Spell {
public:
	bool update(int step);
	void init(Combatant *caster, Combatant **targets, int numTargets);
	void apply(void);

	Ice1Spell(void);
	virtual ~Ice1Spell(void);

protected:
	int count;
	int lifetime;
};


class Ice2Spell : public Spell {
public:
	bool update(int step);
	void init(Combatant *caster, Combatant **targets, int numTargets);
	void apply(void);

	Ice2Spell(void);
	virtual ~Ice2Spell(void);

protected:
	int count;
	int lifetime;
};


class Fire1Spell : public Spell {
public:
	bool update(int step);
	void init(Combatant *caster, Combatant **targets, int numTargets);
	void apply(void);

	Fire1Spell(void);
	virtual ~Fire1Spell(void);

protected:
	int count;
	int lifetime;
};



class Darkness1Spell : public Spell {
public:
	bool update(int step);
	void init(Combatant *caster, Combatant **targets, int numTargets);
	void apply(void);

	Darkness1Spell(void);
	virtual ~Darkness1Spell(void);

protected:
	int count;
	int lifetime;
};


class WeepSpell : public Spell {
public:
	bool update(int step);
	void init(Combatant *caster, Combatant **targets, int numTargets);
	void apply(void);

	WeepSpell(void);
	virtual ~WeepSpell(void);

protected:
	int count;
	int lifetime;
};



class WaveSpell : public Spell {
public:
	bool update(int step);
	void init(Combatant *caster, Combatant **targets, int numTargets);
	void apply(void);

	WaveSpell(void);
	virtual ~WaveSpell(void);

protected:
	int count;
	int lifetime;
};




class WhirlpoolSpell : public Spell {
public:
	bool update(int step);
	void init(Combatant *caster, Combatant **targets, int numTargets);
	void apply(void);

	WhirlpoolSpell(void);
	virtual ~WhirlpoolSpell(void);

protected:
	int count;
	int lifetime;
};

class CureSpell : public Spell {
public:
	bool update(int step);
	void init(Combatant *caster, Combatant **targets, int numTargets);
	void apply(void);

	CureSpell(int amount, std::string name);
	virtual ~CureSpell(void);

protected:
	int count;
	int lifetime;
	std::string name;
	int amount;
	GenericCureEffect *g;
};


class HealSpell : public Spell {
public:
	bool update(int step);
	void init(Combatant *caster, Combatant **targets, int numTargets);
	void apply(void);

	HealSpell(void);
	virtual ~HealSpell(void);

protected:
	int count;
	int lifetime;
	GenericHealEffect *g;
};

class Fire2Spell : public Spell {
public:
	bool update(int step);
	void init(Combatant *caster, Combatant **targets, int numTargets);
	void apply(void);

	Fire2Spell(void);
	virtual ~Fire2Spell(void);

protected:
	int count;
	int lifetime;
};


class StoneSpell : public Spell {
public:
	bool update(int step);
	void init(Combatant *caster, Combatant **targets, int numTargets);
	void apply(void);

	StoneSpell(void);
	virtual ~StoneSpell(void);

protected:
	int count;
	int lifetime;
};


class SludgeSpell : public Spell {
public:
	bool update(int step);
	void init(Combatant *caster, Combatant **targets, int numTargets);
	void apply(void);

	SludgeSpell(void);
	virtual ~SludgeSpell(void);

protected:
	int count;
	int lifetime;
};


class RendSpell : public Spell {
public:
	bool update(int step);
	void init(Combatant *caster, Combatant **targets, int numTargets);
	void apply(void);

	RendSpell(int damage);
	virtual ~RendSpell(void);

protected:
	int count;
	int lifetime;
	int damage;
};


class StompSpell : public Spell {
public:
	bool update(int step);
	void init(Combatant *caster, Combatant **targets, int numTargets);
	void apply(void);

	StompSpell(void);
	virtual ~StompSpell(void);

protected:
	int count;
	int lifetime;
};


class SpraySpell : public Spell {
public:
	bool update(int step);
	void init(Combatant *caster, Combatant **targets, int numTargets);
	void apply(void);

	SpraySpell(void);
	virtual ~SpraySpell(void);

protected:
	int count;
	int lifetime;
};


class ReviveSpell : public Spell {
public:
	bool update(int step);
	void init(Combatant *caster, Combatant **targets, int numTargets);
	void apply(void);

	ReviveSpell(void);
	virtual ~ReviveSpell(void);

protected:
	int count;
	int lifetime;
	std::string name;
	GenericHolyWaterEffect *g;
};

class StunSpell : public Spell {
public:
	bool update(int step);
	void init(Combatant *caster, Combatant **targets, int numTargets);
	void apply(void);

	StunSpell();
	virtual ~StunSpell(void);
private:
	int count;
};


class SlowSpell : public Spell {
public:
	bool update(int step);
	void init(Combatant *caster, Combatant **targets, int numTargets);
	void apply(void);

	SlowSpell();
	virtual ~SlowSpell(void);
private:
	int count;
};


class QuickSpell : public Spell {
public:
	bool update(int step);
	void init(Combatant *caster, Combatant **targets, int numTargets);
	void apply(void);

	QuickSpell();
	virtual ~QuickSpell(void);
private:
	int count;
};


class AcidSpell : public Spell {
public:
	bool update(int step);
	void init(Combatant *caster, Combatant **targets, int numTargets);
	void apply(void);

	AcidSpell(int damage);
	virtual ~AcidSpell(void);

protected:
	int damage;
	int count;
	int lifetime;
};


class PunchSpell : public Spell {
public:
	bool update(int step);
	void init(Combatant *caster, Combatant **targets, int numTargets);
	void apply(void);

	PunchSpell(int damage);
	virtual ~PunchSpell(void);

protected:
	int count;
	int lifetime;
	int damage;
};


class TorrentSpell : public Spell {
public:
	bool update(int step);
	void init(Combatant *caster, Combatant **targets, int numTargets);
	void apply(void);

	TorrentSpell(void);
	virtual ~TorrentSpell(void);

protected:
	int count;
	int lifetime;
};


class BeamSpell : public Spell {
public:
	bool update(int step);
	void init(Combatant *caster, Combatant **targets, int numTargets);
	void apply(void);

	BeamSpell(void);
	virtual ~BeamSpell(void);

protected:
	int count;
	int lifetime;
};


class Fire3Spell : public Spell {
public:
	bool update(int step);
	void init(Combatant *caster, Combatant **targets, int numTargets);
	void apply(void);

	Fire3Spell(void);
	virtual ~Fire3Spell(void);

protected:
	int count;
	int lifetime;
};


class Ice3Spell : public Spell {
public:
	bool update(int step);
	void init(Combatant *caster, Combatant **targets, int numTargets);
	void apply(void);

	Ice3Spell(void);
	virtual ~Ice3Spell(void);

protected:
	int count;
	int lifetime;
};


class Bolt3Spell : public Spell {
public:
	bool update(int step);
	void init(Combatant *caster, Combatant **targets, int numTargets);
	void apply(void);

	Bolt3Spell(void);
	virtual ~Bolt3Spell(void);

protected:
	int count;
	int lifetime;
};


class CharmSpell : public Spell {
public:
	bool update(int step);
	void init(Combatant *caster, Combatant **targets, int numTargets);
	void apply(void);

	CharmSpell();
	virtual ~CharmSpell(void);
private:
	int count;
};


class Darkness2Spell : public Spell {
public:
	bool update(int step);
	void init(Combatant *caster, Combatant **targets, int numTargets);
	void apply(void);

	Darkness2Spell(void);
	virtual ~Darkness2Spell(void);

protected:
	int count;
	int lifetime;
};


class TwisterSpell : public Spell {
public:
	bool update(int step);
	void init(Combatant *caster, Combatant **targets, int numTargets);
	void apply(void);

	TwisterSpell(void);
	virtual ~TwisterSpell(void);

protected:
	int count;
	int lifetime;
};


class WhipSpell : public Spell {
public:
	bool update(int step);
	void init(Combatant *caster, Combatant **targets, int numTargets);
	void apply(void);

	WhipSpell(void);
	virtual ~WhipSpell(void);

protected:
	int count;
	int lifetime;
};


class AcornsSpell : public Spell {
public:
	bool update(int step);
	void init(Combatant *caster, Combatant **targets, int numTargets);
	void apply(void);

	AcornsSpell(void);
	virtual ~AcornsSpell(void);

protected:
	int count;
	int lifetime;
};


class DaisySpell : public Spell {
public:
	bool update(int step);
	void init(Combatant *caster, Combatant **targets, int numTargets);
	void apply(void);

	DaisySpell(void);
	virtual ~DaisySpell(void);

protected:
	int count;
	int lifetime;
};


class VampireSpell : public Spell {
public:
	bool update(int step);
	void init(Combatant *caster, Combatant **targets, int numTargets);
	void apply(void);

	VampireSpell(void);
	virtual ~VampireSpell(void);

protected:
	int count;
	int lifetime;
};


class ArcSpell : public Spell {
public:
	bool update(int step);
	void init(Combatant *caster, Combatant **targets, int numTargets);
	void apply(void);

	ArcSpell(void);
	virtual ~ArcSpell(void);

protected:
	int count;
	int lifetime;
};


class BananaSpell : public Spell {
public:
	bool update(int step);
	void init(Combatant *caster, Combatant **targets, int numTargets);
	void apply(void);

	BananaSpell(std::string imgName, int dmg);
	virtual ~BananaSpell(void);

protected:
	int count;
	int lifetime;
	std::string imageName;
	int damage;
};


class FireballSpell : public Spell {
public:
	bool update(int step);
	void init(Combatant *caster, Combatant **targets, int numTargets);
	void apply(void);

	FireballSpell(void);
	virtual ~FireballSpell(void);

protected:
	int count;
	int lifetime;
};


class KissOfDeathSpell : public Spell {
public:
	bool update(int step);
	void init(Combatant *caster, Combatant **targets, int numTargets);
	void apply(void);

	KissOfDeathSpell(void);
	virtual ~KissOfDeathSpell(void);

protected:
	int count;
	int lifetime;
	std::string imageName;
	int damage;
};


class BoFSpell : public Spell {
public:
	bool update(int step);
	void init(Combatant *caster, Combatant **targets, int numTargets);
	void apply(void);

	BoFSpell(void);
	virtual ~BoFSpell(void);

protected:
	int count;
	int lifetime;
};


class DropSpell : public Spell {
public:
	bool update(int step);
	void init(Combatant *caster, Combatant **targets, int numTargets);
	void apply(void);

	DropSpell(void);
	virtual ~DropSpell(void);

protected:
	int count;
	int lifetime;
};


class BlazeSpell : public Spell {
public:
	bool update(int step);
	void init(Combatant *caster, Combatant **targets, int numTargets);
	void apply(void);

	BlazeSpell(void);
	virtual ~BlazeSpell(void);

protected:
	int count;
	int lifetime;
};


class MachineGunSpell : public Spell {
public:
	bool update(int step);
	void init(Combatant *caster, Combatant **targets, int numTargets);
	void apply(void);

	MachineGunSpell(void);
	virtual ~MachineGunSpell(void);

protected:
	int count;
	int lifetime;
};


class LaserSpell : public Spell {
public:
	bool update(int step);
	void init(Combatant *caster, Combatant **targets, int numTargets);
	void apply(void);

	LaserSpell(void);
	virtual ~LaserSpell(void);

protected:
	int count;
	int lifetime;
};


class UFOSpell : public Spell {
public:
	bool update(int step);
	void init(Combatant *caster, Combatant **targets, int numTargets);
	void apply(void);

	UFOSpell(void);
	virtual ~UFOSpell(void);

protected:
	int count;
	int lifetime;
};


class OrbitSpell : public Spell {
public:
	bool update(int step);
	void init(Combatant *caster, Combatant **targets, int numTargets);
	void apply(void);

	OrbitSpell(void);
	virtual ~OrbitSpell(void);

protected:
	int count;
	int lifetime;
};


class WebSpell : public Spell {
public:
	bool update(int step);
	void init(Combatant *caster, Combatant **targets, int numTargets);
	void apply(void);

	WebSpell(void);
	virtual ~WebSpell(void);

protected:
	int count;
	int lifetime;
};


class Darkness3Spell : public Spell {
public:
	bool update(int step);
	void init(Combatant *caster, Combatant **targets, int numTargets);
	void apply(void);

	Darkness3Spell(void);
	virtual ~Darkness3Spell(void);

protected:
	int count;
	int lifetime;
};


class SwallowSpell : public Spell {
public:
	bool update(int step);
	void init(Combatant *caster, Combatant **targets, int numTargets);
	void apply(void);

	SwallowSpell(void);
	virtual ~SwallowSpell(void);

protected:
	int count;
	int lifetime;
};


class PukeSpell : public Spell {
public:
	bool update(int step);
	void init(Combatant *caster, Combatant **targets, int numTargets);
	void apply(void);

	PukeSpell(void);
	virtual ~PukeSpell(void);

protected:
	int count;
	int lifetime;
};


Spell *createSpell(std::string name);
SpellAlignment getSpellAlignment(std::string name);
int getSpellCost(std::string name);
void applyWhiteMagicSpell(std::string name, CombatantInfo &info, int numTargets, bool sound = false, Combatant *target = 0, int text_x = -1, int text_y = -1);

#endif

