#ifndef COMBATANT_HPP
#define COMBATANT_HPP



class Combatant : public CombatEntity {
public:
	CombatantInfo &getInfo(void) { return info; }
	int getDirection(void);
	AnimationSet *getAnimationSet(void);
	AnimationSet *getWhiteAnimationSet(void);
	CombatLocation getLocation(void);
	CombatLoyalty getLoyalty(void);
	std::string getName(void);
	std::string getPrintableName(void);
	bool isDefending(void);
	void setDefending(bool def);

	virtual void attacked(void) { };

	void setDirection(Direction dir);
	void setInfo(CombatantInfo &inf);
	void setLocation(CombatLocation l);
	void setLoyalty(CombatLoyalty l);

	Combatant(std::string name, bool alpha);
	Combatant(std::string name);
	virtual ~Combatant();

protected:
	void construct(std::string name, bool alpha);

	std::string name;
	Direction direction;
	AnimationSet *animSet;
	AnimationSet *whiteAnimSet;
	CombatantInfo info;
	CombatLocation location;
	CombatLoyalty loyalty;
	std::string printableName;
	bool defending;
};


#endif

