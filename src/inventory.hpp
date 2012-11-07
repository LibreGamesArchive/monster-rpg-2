#ifndef INVENTORY_HPP
#define INVENTORY_HPP

// order is important here
enum ItemType {
	ITEM_TYPE_WEAPON = 0,
	ITEM_TYPE_HEAD_ARMOR,
	ITEM_TYPE_CHEST_ARMOR,
	ITEM_TYPE_FEET_ARMOR,
	ITEM_TYPE_SPECIAL,
	ITEM_TYPE_STATUS,
	ITEM_TYPE_END
};

struct Item {
	std::string name;
	ItemType type;
	int id;
	int icon;
};

struct WeaponItem {
	int classes;
	int attack;
	int needs; // what it needs (ie arrows, an arbitrary number)
	int satisfies; // what it satisfies, also arbitrary
	bool ammo; // is ammo for other equipped weapon?
	Element element;
};

struct ArmorItem {
	int classes;
	int defense;
	int magicDefense;
};

struct SpecialItem {
};

struct StatusItem {
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

struct Inventory {
	int index;
	int quantity;
};


const int MAX_INVENTORY = 32;


extern Item items[];
extern WeaponItem weapons[];
extern ArmorItem helmets[];
extern Inventory inventory[MAX_INVENTORY];
extern ArmorItem chestArmors[];
extern ArmorItem feetArmors[];


int findEmptyInventorySlot(void);
int findUnfullInventorySlot(int index);
int findUsedInventorySlot(int index);
void setInventory(int slot, int index, int quantity);
std::string getItemName(int index);
int armorDefense(ItemType type, int num);
int armorMagicDefense(ItemType type, int num);
int weaponDamage(int num);
std::string getItemIcon(int index);
void use(Combatant *c, int index, bool can_equip, int numTargets = 1);
std::string getWeaponSound(CombatantInfo &info);
int getResaleValue(int itemIndex);



#endif
