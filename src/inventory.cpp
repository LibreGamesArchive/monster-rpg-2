#include "monster2.hpp"

Item items[] = {
	{ "Bone", ITEM_TYPE_WEAPON, 0,  0 },		// 0
	{ "Bone", ITEM_TYPE_HEAD_ARMOR, 0, 1 },		// 1
	{ "Cure", ITEM_TYPE_STATUS, 0, 2 },		// 2
	{ "Letter", ITEM_TYPE_SPECIAL, 0, 3 },		// 3
	{ "Bone", ITEM_TYPE_CHEST_ARMOR, 0, 6 },	// 4
	{ "Wood", ITEM_TYPE_WEAPON, 1, 7 },		// 5
	{ "Holy Water", ITEM_TYPE_STATUS, 1, 2 },	// 6
	{ "Heal", ITEM_TYPE_STATUS, 2, 2 },		// 7
	{ "Iron", ITEM_TYPE_WEAPON, 2, 0 },		// 8
	{ "Elixir", ITEM_TYPE_STATUS, 3, 2 },		// 9
	{ "Badge", ITEM_TYPE_SPECIAL, 1, 9 },		// 10
	{ "Iron", ITEM_TYPE_WEAPON, 3, 7 },		// 11
	{ "Cloth", ITEM_TYPE_CHEST_ARMOR, 1, 10 },	// 12
	{ "Sandals", ITEM_TYPE_FEET_ARMOR, 0, 11 },	// 13
	{ "Fruit", ITEM_TYPE_SPECIAL, 2, 12 },		// 14
	{ "Juice", ITEM_TYPE_STATUS, 4, 13 },		// 15
	{ "Pistol", ITEM_TYPE_WEAPON, 4, 14 },		// 16
	{ "Round", ITEM_TYPE_WEAPON, 5, 15 },		// 17
	{ "Leather", ITEM_TYPE_CHEST_ARMOR, 2, 16 },	// 18
	{ "Leather", ITEM_TYPE_HEAD_ARMOR, 1, 17 },	// 19
	{ "Silver", ITEM_TYPE_SPECIAL, 3, 18 },		// 20
	{ "Cure2", ITEM_TYPE_STATUS, 5, 2 },		// 21
	{ "Key", ITEM_TYPE_SPECIAL, 4, 19 },		// 22
	{ "Guard", ITEM_TYPE_WEAPON, 6, 0 },		// 23
	{ "Bamboo", ITEM_TYPE_WEAPON, 7, 7 },		// 24
	{ "Mystic", ITEM_TYPE_CHEST_ARMOR, 3, 10 },	// 25
	{ "Mail", ITEM_TYPE_CHEST_ARMOR, 4, 6 },	// 26
	{ "Medallion", ITEM_TYPE_SPECIAL, 5, 20 },	// 27
	{ "Steel", ITEM_TYPE_HEAD_ARMOR, 2, 1 },	// 28
	{ "Soldier", ITEM_TYPE_WEAPON, 8, 0 },		// 29
	{ "Soldier", ITEM_TYPE_HEAD_ARMOR, 3, 1 },	// 30
	{ "Soldier", ITEM_TYPE_FEET_ARMOR, 1, 11 },	// 31
	{ "Mage", ITEM_TYPE_WEAPON, 9, 7 },		// 32
	{ "Mage", ITEM_TYPE_CHEST_ARMOR, 5, 10 },	// 33
	{ "Top Hat", ITEM_TYPE_HEAD_ARMOR, 4, 21 },	// 34
	{ "Flint", ITEM_TYPE_SPECIAL, 6, 33 },		// 35
	{ "Key", ITEM_TYPE_SPECIAL, 7, 19 },		// 36
	{ "Scope", ITEM_TYPE_SPECIAL, 8, 23 },		// 37
	{ "Mystic", ITEM_TYPE_WEAPON, 10, 0 },		// 38
	{ "Mystic", ITEM_TYPE_HEAD_ARMOR, 5, 1 },	// 39
	{ "Mystic", ITEM_TYPE_FEET_ARMOR, 2, 11 },	// 40
	{ "Key 1", ITEM_TYPE_SPECIAL, 9, 19 },		// 41
	{ "Key 2", ITEM_TYPE_SPECIAL, 10, 19 },		// 42
	{ "Cure3", ITEM_TYPE_STATUS, 6, 2 },		// 43
	{ "Emerald", ITEM_TYPE_WEAPON, 11, 7 },		// 44
	{ "Emerald", ITEM_TYPE_CHEST_ARMOR, 6, 10 },	// 45
	{ "Mystic", ITEM_TYPE_CHEST_ARMOR, 7, 6 },	// 46
	{ "Staff", ITEM_TYPE_SPECIAL, 11, 7 },          // 47
	{ "Jade", ITEM_TYPE_WEAPON, 12, 0 },		// 48
	{ "Jade", ITEM_TYPE_WEAPON, 13, 7 },		// 49
	{ "Jade", ITEM_TYPE_CHEST_ARMOR, 8, 6 },	// 50
	{ "Jade", ITEM_TYPE_HEAD_ARMOR, 6, 1 },		// 51
	{ "Crystal", ITEM_TYPE_SPECIAL, 12, 24 },       // 52
	{ "Onyx", ITEM_TYPE_WEAPON, 14, 0 },		// 53
	{ "Onyx", ITEM_TYPE_WEAPON, 15, 7 },		// 54
	{ "Onyx", ITEM_TYPE_HEAD_ARMOR, 7, 1 },		// 55
	{ "Onyx", ITEM_TYPE_CHEST_ARMOR, 9, 6 },	// 56
	{ "Onyx", ITEM_TYPE_CHEST_ARMOR, 10, 10 },	// 57
	{ "Onyx", ITEM_TYPE_FEET_ARMOR, 3, 11 },	// 58
	{ "Meatballs", ITEM_TYPE_STATUS, 7, 25 },	// 59
	{ "Alien", ITEM_TYPE_WEAPON, 16, 7 },		// 60
	{ "Alien", ITEM_TYPE_CHEST_ARMOR, 11, 10 },	// 61
	{ "Alien", ITEM_TYPE_HEAD_ARMOR, 8, 1 },	// 62
	{ "Alien", ITEM_TYPE_WEAPON, 17, 0 },		// 63
	{ "Light", ITEM_TYPE_WEAPON, 18, 7 },		// 64
	{ "Light", ITEM_TYPE_CHEST_ARMOR, 12, 6 },	// 65
	{ "Light", ITEM_TYPE_HEAD_ARMOR, 9, 1 },	// 66
	{ "Light", ITEM_TYPE_WEAPON, 19, 0 },		// 67
	{ "Light", ITEM_TYPE_FEET_ARMOR, 4, 11 },	// 68
	{ "Orb", ITEM_TYPE_SPECIAL, 13, 26 },           // 69
	{ "END", ITEM_TYPE_END, 0, }
};

void sortInventory()
{
	Inventory tmp[MAX_INVENTORY];

	for (int i = 0; i < MAX_INVENTORY; i++) {
		tmp[i].index = -1;
		tmp[i].quantity = 0;
	}

	int nitems;

	for (nitems = 0; items[nitems].type != ITEM_TYPE_END; nitems++);

	const int ntypes = 6;

	int types[ntypes] = {
		ITEM_TYPE_STATUS,
		ITEM_TYPE_SPECIAL,
		ITEM_TYPE_WEAPON,
		ITEM_TYPE_HEAD_ARMOR,
		ITEM_TYPE_CHEST_ARMOR,
		ITEM_TYPE_FEET_ARMOR
	};

	int curr_slot = 0;

	for (int type = 0; type < ntypes; type++) {
		for (int item = nitems-1; item >= 0; item--) {
			if (items[item].type != types[type]) {
				continue;
			}
			int quantity = 0;
			for (int slot = 0; slot < MAX_INVENTORY; slot++) {
				if (inventory[slot].index == item) {
					quantity += inventory[slot].quantity;
				}
			}
			if (quantity > 0) {
				do {
					int n = quantity > 99 ? 99 : quantity;
					tmp[curr_slot].index = item;
					tmp[curr_slot].quantity = n;
					curr_slot++;
					quantity -= n;
				} while (quantity > 0);
			}
		}
	}

	memcpy(inventory, &tmp, sizeof(tmp));
}

const int ITEM_MEATBALLS = 59;

WeaponItem weapons[] = {
	{ CLASS_WARRIOR, 10, -1, -1, false, ELEMENT_NONE },
	{ CLASS_MAGE|CLASS_CLERIC, 5, -1, -1, false, ELEMENT_NONE },
	{ CLASS_WARRIOR, 50, -1, -1, false, ELEMENT_NONE },
	{ CLASS_MAGE|CLASS_CLERIC, 30, -1, -1, false, ELEMENT_NONE },
	{ CLASS_STEAMPUNK, 25, 0, 1, false, ELEMENT_NONE },
	{ CLASS_STEAMPUNK, 25, 1, 0, true, ELEMENT_NONE },
	{ CLASS_WARRIOR, 100, -1, -1, false, ELEMENT_NONE },
	{ CLASS_MAGE|CLASS_CLERIC, 55, -1, -1, false, ELEMENT_NONE },
	{ CLASS_WARRIOR, 175, -1, -1, false, ELEMENT_NONE },
	{ CLASS_MAGE|CLASS_CLERIC, 120, -1, -1, false, ELEMENT_NONE },
	{ CLASS_ENY, 250, -1, -1, false, ELEMENT_NONE },
	{ CLASS_MAGE|CLASS_CLERIC, 200, -1, -1, false, ELEMENT_NONE },
	{ CLASS_WARRIOR, 450, -1, -1, false, ELEMENT_NONE },
	{ CLASS_MAGE|CLASS_CLERIC, 350, -1, -1, false, ELEMENT_NONE },
	{ CLASS_WARRIOR, 400, -1, -1, false, ELEMENT_NONE },
	{ CLASS_MAGE|CLASS_CLERIC, 300, -1, -1, false, ELEMENT_NONE },
	{ CLASS_MAGE|CLASS_CLERIC, 700, -1, -1, false, ELEMENT_NONE },
	{ CLASS_WARRIOR, 700, -1, -1, false, ELEMENT_NONE },
	{ CLASS_MAGE|CLASS_CLERIC, 650, -1, -1, false, ELEMENT_NONE },
	{ CLASS_WARRIOR, 650, -1, -1, false, ELEMENT_NONE }
};

struct nameAndAnims {
	std::string name;
	AnimationSet *anim;
};

std::string weaponSoundNames[] = {
	"melee_woosh.ogg",
	"melee_woosh.ogg",
	"melee_woosh.ogg",
	"melee_woosh.ogg",
	"pistol.ogg",
	"",
	"melee_woosh.ogg",
	"melee_woosh.ogg",
	"melee_woosh.ogg",
	"melee_woosh.ogg",
	"melee_woosh.ogg",
	"melee_woosh.ogg",
	"melee_woosh.ogg",
	"melee_woosh.ogg",
	"melee_woosh.ogg",
	"melee_woosh.ogg",
	"melee_woosh.ogg",
	"melee_woosh.ogg",
	"melee_woosh.ogg",
	"melee_woosh.ogg"
};

ArmorItem helmets[] = {
	{ CLASS_WARRIOR, 5, 0 },
	{ CLASS_STEAMPUNK|CLASS_WARRIOR, 25, 0 },
	{ CLASS_WARRIOR, 50, 0 },
	{ CLASS_WARRIOR, 100, 0 },
	{ CLASS_MAGE|CLASS_CLERIC, 50, 0 },
	{ CLASS_WARRIOR, 150, 50 },
	{ CLASS_WARRIOR|CLASS_CLERIC|CLASS_MAGE, 350, 250 },
	{ CLASS_WARRIOR|CLASS_CLERIC|CLASS_MAGE, 300, 200 },
	{ CLASS_WARRIOR|CLASS_CLERIC|CLASS_MAGE, 500, 500 },
	{ CLASS_WARRIOR|CLASS_CLERIC|CLASS_MAGE, 450, 450 }
};

ArmorItem chestArmors[] = {
	{ CLASS_WARRIOR, 10, 0 },
	{ CLASS_MAGE|CLASS_CLERIC, 10, 15 },
	{ CLASS_STEAMPUNK|CLASS_WARRIOR, 30, 5 },
	{ CLASS_MAGE|CLASS_CLERIC, 40, 25 },
	{ CLASS_WARRIOR|CLASS_STEAMPUNK, 60, 0 },
	{ CLASS_MAGE|CLASS_CLERIC, 90, 50 },
	{ CLASS_MAGE|CLASS_CLERIC, 150, 100 },
	{ CLASS_WARRIOR, 150, 100 },
	{ CLASS_WARRIOR|CLASS_MAGE|CLASS_CLERIC, 350, 250 },
	{ CLASS_WARRIOR, 300, 200 },
	{ CLASS_MAGE|CLASS_CLERIC, 250, 150 },
	{ CLASS_WARRIOR|CLASS_MAGE|CLASS_CLERIC, 500, 500 },
	{ CLASS_WARRIOR|CLASS_MAGE|CLASS_CLERIC, 450, 450 }
};

ArmorItem feetArmors[] = {
	{ CLASS_MAGE|CLASS_CLERIC|CLASS_WARRIOR|CLASS_STEAMPUNK, 10, 10 },
	{ CLASS_WARRIOR, 75, 0 },
	{ CLASS_WARRIOR, 110, 40 },
	{ CLASS_WARRIOR|CLASS_MAGE|CLASS_CLERIC, 250, 150 },
	{ CLASS_WARRIOR|CLASS_MAGE|CLASS_CLERIC, 350, 350 }
};

StatusItem statusItems[] = {
	{ 50, 0, }, // CURE
	{ 0, }, // HOLY_WATER
	{ 0, }, // Heal
	{ 0, 0, 0, 0, 0, 100, 0, }, // Elixir
	{ 9999, 0, }, // Juice
	{ 250, 0, }, // CURE2
	{ 2000, 0, }, // CURE3
	{ 10, 0, } // Meatballs
};

SpecialItem specialItems[] = {
	{ },
	{ },
	{ },
	{ },
	{ },
	{ },
	{ },
	{ },
	{ },
	{ },
	{ },
	{ },
	{ },
	{ }
};


static std::string getItemSound(std::string name)
{
	if (name == "Cure" || name == "Cure2" || name == "Cure3" || name == "Meatballs")
		return "Cure.ogg";
	else if (name == "Juice") {
		return "juice.ogg";
	}
	else if (name == "Heal")
		return "Heal.ogg";
	else if (name == "Revive" || name == "Holy Water")
		return "HolyWater.ogg";
	else if (name == "Elixir")
		return "Elixir.ogg";
	else
		return "";
}

Inventory inventory[MAX_INVENTORY];

std::string getWeaponSound(CombatantInfo &info)
{
	int index = -1;
	int lhand = info.equipment.lhand;
	int rhand = info.equipment.rhand;
	if ((lhand >= 0 && weapons[items[lhand].id].needs >= 0) ||
		(rhand >= 0 && weapons[items[rhand].id].needs >= 0)) {
		if ((lhand >= 0) && !weapons[items[lhand].id].ammo) {
			if (rhand < 0)
				return "";
			else
				index = lhand;
		}
		else if ((rhand >= 0) && !weapons[items[rhand].id].ammo) {
			if (lhand < 0)
				return "";
			else
				index = rhand;
		}
	}
	else if (lhand >= 0)
		index = lhand;
	else if (rhand >= 0)
		index = rhand;
	if (index >= 0) {
		int weaponIndex = items[index].id;
		return weaponSoundNames[weaponIndex];
	}

	return "";
}


int findEmptyInventorySlot(void)
{
	for (int i = 0; i < MAX_INVENTORY; i++) {
		if (inventory[i].index < 0)
			return i;
	}

	return -1;
}

int findUnfullInventorySlot(int index)
{
	for (int i = 0; i < MAX_INVENTORY; i++) {
		if ((inventory[i].index == index) && (inventory[i].quantity < 99)) {
			return i;
		}
	}

	return findEmptyInventorySlot();
}

int findUsedInventorySlot(int index)
{
	for (int i = 0; i < MAX_INVENTORY; i++) {
		if (inventory[i].index == index)
			return i;
	}

	return -1;
}

void setInventory(int slot, int index, int quantity)
{
	inventory[slot].index = index;
	inventory[slot].quantity = quantity;
}


std::string getItemName(int index)
{
	if (index < 0)
		return std::string("");

	return items[index].name;
}

int armorDefense(ItemType type, int num)
{
	if (num < 0) return 0;

	// FIXME:
	switch (type) {
		case ITEM_TYPE_CHEST_ARMOR:
			return chestArmors[num].defense;
			break;
		case ITEM_TYPE_HEAD_ARMOR:
			return helmets[num].defense;
			break;
		case ITEM_TYPE_FEET_ARMOR:
			return feetArmors[num].defense;
			break;
		default:
			return 0;
	}

	return 0;
}

int armorMagicDefense(ItemType type, int num)
{
	if (num < 0) return 0;

	switch (type) {
		case ITEM_TYPE_CHEST_ARMOR:
			return chestArmors[num].magicDefense;
		case ITEM_TYPE_HEAD_ARMOR:
			return helmets[num].magicDefense;
		case ITEM_TYPE_FEET_ARMOR:
			return feetArmors[num].magicDefense;
		default:
			return 0;
	}

	return 0;
}

int weaponDamage(int num)
{
	if (num < 0) return 0;
	return weapons[num].attack;
}


std::string getItemIcon(int index)
{
	if (index < 0)
		return std::string("");

	char name[10];
	sprintf(name, "{%03d}", items[index].icon);
	return std::string(name);
}


void use(Combatant *c, int index, bool can_equip, int numTargets)
{
	if (index < 0 || inventory[index].index < 0) {
		playPreloadedSample("error.ogg");
		return;
	}

	if (c == NULL) {
		if (items[inventory[index].index].type == ITEM_TYPE_SPECIAL) {
			notify("You might", "need that!", "");
			return;
		}
		if (inventory[index].quantity > 1) {
			char buf[100];
			sprintf(buf, _t("Drop all %d?"), inventory[index].quantity);
			if (prompt(std::string(buf), "", 0, 0)) {
				inventory[index].quantity = 0;
				inventory[index].index = -1;
			}
			else if (prompt("Really drop 1?", "", 0, 0)) {
				inventory[index].quantity--;
				if (inventory[index].quantity <= 0) {
					inventory[index].index = -1;
				}
			}
		}
		else if (prompt("Really", "drop this?", 0, 0)) {
			inventory[index].quantity--;
			if (inventory[index].quantity <= 0) {
				inventory[index].index = -1;
			}
		}
		return;
	}

	if (c->getName() == "Tode" && inventory[index].index == ITEM_MEATBALLS) {
		std::list<CombatEntity *> &v = battle->getEntities();
		std::list<CombatEntity *>::iterator it;
		for (it = v.begin(); it != v.end(); it++) {
			CombatEntity *e = *it;
			if (e->getType() == COMBATENTITY_TYPE_PLAYER) {
				CombatPlayer *p = (CombatPlayer *)e;
				if (p->getInfo().condition == CONDITION_SWALLOWED) {
					loadPlayDestroy("Mmm.ogg");
					CombatEnemyTode *t = (CombatEnemyTode *)c;
					t->puke_next();
					return;
				}
			}
		}
		playPreloadedSample("error.ogg");
	}

	CombatantInfo *info = &c->getInfo();

	int slot = findEmptyInventorySlot();

	if (items[inventory[index].index].type == ITEM_TYPE_STATUS) {
		int maxhp = info->abilities.maxhp;
		int hp = info->abilities.hp;
		// If in menus, play sound now
		if (can_equip) {
			std::string s = getItemSound(items[inventory[index].index].name);
			if (s != "")
				loadPlayDestroy(s);

			if (hp > 0) {
				if (inventory[index].index == CURE_INDEX || inventory[index].index == CURE2_INDEX || inventory[index].index == CURE3_INDEX) {
					int amount = 
					statusItems[items[inventory[index].index].id].hp;
					char text[100];
					sprintf(text, "%d", MIN(amount, maxhp-hp));
					addOmnipotentText(std::string(text), 40, 40,
						m_map_rgb(0, 255, 0));
				}
				else if (inventory[index].index == HOLY_WATER_INDEX) {
					char text[100];
					sprintf(text, "%d", maxhp-hp);
					addOmnipotentText(std::string(text), 40, 40,
						m_map_rgb(0, 255, 0));
				}
				else if (inventory[index].index == HEAL_INDEX) {
					addOmnipotentText(std::string("HEAL!"), 40, 40,
						m_map_rgb(0, 255, 0));
				}
				else if (inventory[index].index == ELIXIR_INDEX) {
					int maxmp = info->abilities.maxmp;
					int mp = info->abilities.mp;
					int amount = 
					statusItems[items[inventory[index].index].id].mp;
					char text[100];
					sprintf(text, "%d", MIN(amount, maxmp-mp));
					addOmnipotentText(std::string(text), 40, 40,
						m_map_rgb(0xff, 0x7f, 0)); // orange
				}
			}
			if (inventory[index].index == HOLY_WATER_INDEX) {
				char text[100];
				strcpy(text, "LIFE!");
				addOmnipotentText(std::string(text), 40, 40,
						m_map_rgb(0, 255, 0));
			}
		}
		// special case for 'HOLY_WATER'
		if (info->abilities.hp <= 0) {
			if (inventory[index].index == HOLY_WATER_INDEX) {
				info->abilities.hp = (int)(info->abilities.maxhp * 0.2f);
			}
		}
		else {
			// special case for Heal
			if (inventory[index].index == HEAL_INDEX) {
				if (c->getType() == COMBATENTITY_TYPE_PLAYER && c->getAnimationSet()->getSubName() != "use") {
					c->getAnimationSet()->setSubAnimation("stand");
				}
				info->condition = CONDITION_NORMAL;
			}
			else {
				StatusItem si;
				memcpy(&si, &statusItems[items[inventory[index].index].id], sizeof(StatusItem));
				if (c->getName() == "Zombie"
					|| c->getName() == "Mummy") {
					if (si.hp > 0) {
						si.hp = -si.hp;
					}
				}
				if (numTargets > 1) {
					si.maxhp = MAX(1, (si.maxhp * 1.2) / numTargets);
					si.hp = MAX(1, (si.hp * 1.2) / numTargets);
					si.attack = MAX(1, (si.attack * 1.2) / numTargets);
					si.defense = MAX(1, (si.defense * 1.2) / numTargets);
					si.speed = MAX(1, (si.speed * 1.2) / numTargets);
					si.maxmp = MAX(1, (si.maxmp * 1.2) / numTargets);
					si.mp = MAX(1, (si.mp * 1.2) / numTargets);
					si.luck = MAX(1, (si.luck * 1.2) / numTargets);
					si.mdefense = MAX(1, (si.mdefense * 1.2) / numTargets);
				}
				if (inventory[index].index == HOLY_WATER_INDEX) {
					if (c->getName() == "Zombie"
						|| c->getName() == "Mummy") {
						/* Do 500 damage on Zombies and Mummies */
						si.hp = -1000;
					}
					else {
						info->abilities.hp = info->abilities.maxhp;
					}
				}
				info->abilities.maxhp += si.maxhp;
				info->abilities.hp += si.hp;
				// If player is not dead, holy water gives them
				// max HP
				if (info->abilities.hp > info->abilities.maxhp)
					info->abilities.hp = info->abilities.maxhp;
				info->abilities.attack += si.attack;
				info->abilities.defense += si.defense;
				info->abilities.speed += si.speed;
				info->abilities.maxmp += si.maxmp;
				info->abilities.mp += si.mp;
				if (info->abilities.mp > info->abilities.maxmp)
					info->abilities.mp = info->abilities.maxmp;
				info->abilities.mdefense += si.mdefense;
				info->abilities.luck += si.luck;
			}
		}
		inventory[index].quantity--;
		if (inventory[index].quantity <= 0) {
			inventory[index].index = -1;
		}
		return;
	}
	else if (items[inventory[index].index].type == ITEM_TYPE_SPECIAL) {
		if (inventory[index].index == LETTER_INDEX) {
			notify("You can't read that...", "It is addressed to", "someone else...");
		}
		else {
			notify("You can't use", "that now...", "");
		}
		return;
	}

	if (!can_equip) {
		if (battle) {
			playPreloadedSample("error.ogg");
		}
		else {
			notify("You can't use", "that now...", "");
		}
		return;
	}

	switch (items[inventory[index].index].type) {
		case ITEM_TYPE_WEAPON: {
			if (!(info->characterClass & weapons[items[inventory[index].index].id].classes)) {
				notify("Character cannot", "use that...", "");
				return;
			}
			bool is_ammo = 
				weapons[items[inventory[index].index].id].ammo;
			if (is_ammo) {
				if ((info->equipment.lhand == inventory[index].index ||
				 info->equipment.rhand == inventory[index].index)) {
					int *ammo;
					if (info->equipment.lhand == inventory[index].index) {
						ammo = &(info->equipment.lquantity);
					}
					else {
						ammo = &(info->equipment.rquantity);
					}
					int amount = 99 - *ammo;
					if (amount > inventory[index].quantity)
						amount = inventory[index].quantity;
					inventory[index].quantity -= amount;
					if (inventory[index].quantity <= 0) {
						inventory[index].quantity = 0;
						inventory[index].index = -1;
					}
					*ammo += amount;
				}
				else if (info->equipment.lhand < 0 && info->equipment.rhand < 0) {
					info->equipment.rhand = inventory[index].index;
					info->equipment.rquantity = inventory[index].quantity;
					inventory[index].quantity = 0;
					inventory[index].index = -1;
				}
				else {
					int *full, *empty, *q;
					if (info->equipment.lhand >= 0) {
						full = &info->equipment.lhand;
						empty = &info->equipment.rhand;
						q = &info->equipment.rquantity;
					}
					else {
						full = &info->equipment.rhand;
						empty = &info->equipment.lhand;
						q = &info->equipment.lquantity;
					}

					int wid = items[inventory[index].index].id;
					int fullid = items[*full].id;
					if (weapons[fullid].needs >= 0 && ((weapons[fullid].needs == weapons[wid].satisfies) || (weapons[fullid].satisfies == weapons[wid].needs))) {
						*empty = inventory[index].index;
						int max = 99 - *q;
						int n = MIN(max, inventory[index].quantity);
						*q += n;
						inventory[index].quantity -= n;
						if (inventory[index].quantity <= 0)
							inventory[index].index = -1;
					}
					else {
						notify("Wrong type", "of ammunition...", "");
					}
				}
			}
			else {
				if (info->equipment.lhand >= 0 && info->equipment.rhand >= 0) {
					notify("Unequip something", "first...", "");
				}
				else if (info->equipment.lhand < 0 && info->equipment.rhand < 0) {
					info->equipment.rhand = inventory[index].index;
					info->equipment.rquantity = 1;
					inventory[index].quantity--;
					if (inventory[index].quantity <= 0) {
						inventory[index].index = -1;
					}
				}
				else {
					int *full, *empty, *q;
					if (info->equipment.lhand >= 0) {
						full = &info->equipment.lhand;
						empty = &info->equipment.rhand;
						q = &info->equipment.rquantity;
					}
					else {
						full = &info->equipment.rhand;
						empty = &info->equipment.lhand;
						q = &info->equipment.lquantity;
					}

					int wid = items[inventory[index].index].id;
					int fullid = items[*full].id;
					if (weapons[fullid].needs >= 0 && ((weapons[fullid].needs == weapons[wid].satisfies) || (weapons[fullid].satisfies == weapons[wid].needs))) {
						*empty = inventory[index].index;
						*q = 1;
						inventory[index].quantity--;
						if (inventory[index].quantity <= 0) {
							inventory[index].index = -1;
						}
					}
					else {
						if (inventory[index].quantity > 1) {
							int slot = findEmptyInventorySlot();
							if (slot < 0) {
								notify("No room", "for swapping...", "");
							}
							else {
								inventory[slot].index = *full;
								inventory[slot].quantity = 1;
								*full = inventory[index].index;
								inventory[index].quantity--;
							}
						}
						else {
							int tmp = inventory[index].index;
							inventory[index].quantity = 1;
							inventory[index].index = *full;
							*full = tmp;
						}
					}
				}
			}
			break;
		}
		case ITEM_TYPE_HEAD_ARMOR:
			if (!(info->characterClass & helmets[items[inventory[index].index].id].classes)) {
				notify("Character cannot", "use that...", "");
				return;
			}
			if ((inventory[index].quantity > 1) && (slot < 0)
				&& (info->equipment.harmor >= 0)) {
				notify("You have no room", "in your inventory...", "");
			}
			else {
				if (info->equipment.harmor >= 0) {
					inventory[slot].index = info->equipment.harmor;
					inventory[slot].quantity = 1;
				}
				info->equipment.harmor = inventory[index].index;
				inventory[index].quantity--;
				if (inventory[index].quantity <= 0) {
					inventory[index].index = -1;
				}
			}
			break;
		case ITEM_TYPE_CHEST_ARMOR:
			if (!(info->characterClass & chestArmors[items[inventory[index].index].id].classes)) {
				notify("Character cannot", "use that...", "");
				return;
			}
			if ((inventory[index].quantity > 1) && (slot < 0)
				&& (info->equipment.carmor >= 0)) {
				notify("You have no room", "in your inventory...", "");
			}
			else {
				if (info->equipment.carmor >= 0) {
					inventory[slot].index = info->equipment.carmor;
					inventory[slot].quantity = 1;
				}
				info->equipment.carmor = inventory[index].index;
				inventory[index].quantity--;
				if (inventory[index].quantity <= 0) {
					inventory[index].index = -1;
				}
			}
			break;
		case ITEM_TYPE_FEET_ARMOR:
			if (!(info->characterClass & feetArmors[items[inventory[index].index].id].classes)) {
				notify("Character cannot", "use that...", "");
				return;
			}
			if ((inventory[index].quantity > 1) && (slot < 0)
				&& (info->equipment.farmor >= 0)) {
				notify("You have no room", "in your inventory...", "");
			}
			else {
				if (info->equipment.farmor >= 0) {
					inventory[slot].index = info->equipment.farmor;
					inventory[slot].quantity = 1;
				}
				info->equipment.farmor = inventory[index].index;
				inventory[index].quantity--;
				if (inventory[index].quantity <= 0) {
					inventory[index].index = -1;
				}
			}
			break;
		default:
			return;
	}
}


int getResaleValue(int itemIndex)
{
	lua_State *stack = area->getLuaState();
	callLua(stack, "get_item_resale_value", "i>i", itemIndex);
	int value = (int)lua_tonumber(stack, -1);
	lua_pop(stack, 1);
	return value;
}

