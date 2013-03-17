#ifndef COMBATANT_HPP
#define COMBATANT_HPP

#include "CombatEntity.hpp"

class Combatant : public CombatEntity {
public:
	AnimationSet *animSet;

	CombatantInfo &getInfo() { return info; }
	int getDirection();
	AnimationSet *getAnimationSet();
	AnimationSet *getWhiteAnimationSet();
	CombatLocation getLocation();
	CombatLoyalty getLoyalty();
	std::string getName();
	std::string getPrintableName();
	bool isDefending();
	void setDefending(bool def);

	virtual void attacked() {}

	virtual void createStoneBmp() {}

	void setDirection(int dir);
	void setInfo(CombatantInfo &inf);
	void setLocation(CombatLocation l);
	void setLoyalty(CombatLoyalty l);

	Combatant(std::string name, bool alpha);
	Combatant(std::string name);
	virtual ~Combatant();

protected:
	void construct(std::string name, bool alpha);

	std::string name;
	int direction;
	AnimationSet *whiteAnimSet;
	CombatantInfo info;
	CombatLocation location;
	CombatLoyalty loyalty;
	std::string printableName;
	bool defending;
};


#endif

