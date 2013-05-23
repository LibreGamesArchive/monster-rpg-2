#ifndef COMBAT_STATS_HPP
#define COMBAT_STATS_HPP

enum CombatLocation {
	LOCATION_LEFT = 0,
	LOCATION_RIGHT
};


enum CombatLoyalty {
	LOYALTY_GOOD = 0,
	LOYALTY_EVIL
};


enum CombatFormation {
	FORMATION_FRONT = 0,
	FORMATION_BACK
};

enum CombatCondition {
	CONDITION_NORMAL = -1,
	CONDITION_POISONED = 0,
	CONDITION_PARALYZED,
	CONDITION_STONED,
	CONDITION_SLOW,
	CONDITION_QUICK,
	CONDITION_CHARMED,
	CONDITION_MUSHROOM,
	CONDITION_SHADOW,
	CONDITION_WEBBED,
	CONDITION_SWALLOWED
};


// order is important here
struct CombatantAbilities {
	int hp;
	int maxhp;
	int attack;
	int defense;
	int speed;
	int mp;
	int maxmp;
	int mdefense; // magic defense
	int luck;
};


// order is important here
struct CombatantEquipment {
	int lhand; // left hand weapon
	int rhand; // left hand weapon
	int harmor; // head armor
	int carmor; // chest armor
	int farmor; // feet armor
	int lquantity; // quantity in left hand
	int rquantity; // quantity in right hand
};


struct CombatantInfo {
	CombatantAbilities abilities;
	CombatantEquipment equipment;
	std::string spells[MAX_SPELLS];
	int experience;
	int characterClass;
	CombatCondition condition;

	// Nothing here is saved or kept after battle
	int paralyzeCount;
	int i;
	bool missed_extra; // for slow/quick
	int charmedCount;
};


#endif

