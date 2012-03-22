#ifndef EQUIPMENT_HPP
#define EQUIPMENT_HPP


enum ArmorType {
	ARMOR_NONE = 0,
	ARMOR_HEAD,
	ARMOR_CHEST,
	ARMOR_FEET,
};

enum WeaponType {
	WEAPON_NONE = 0,
	WEAPON_TWO_HANDED,
	WEAPON_ONE_HANDED
};


// for spells only
enum Element {
	ELEMENT_ALL = -2,
	ELEMENT_NONE = -1,
	ELEMENT_FIRE = 0,
	ELEMENT_ICE,
	ELEMENT_ELECTRICITY
};


struct Armor {
	const char *name;
	ArmorType type;
	int defense;
	int magicDefense;
};


struct Weapon {
	const char *name;
	WeaponType type;
	int damage;
	Element element;
};


class CombatEnemy;


int applyStrengthsAndWeaknesses(CombatEnemy *enemy, int damage, Element element);

#endif

