#include <cstdio>
#include <string>
#include <zlib.h>
#include <iostream>
#include <cstring>
#include <cstdlib>

const int ITEM_CURE = 2;
const int ITEM_HOLY_WATER = 6;
const int ITEM_HEAL = 7;
const int ITEM_ELIXIR = 9;
const int ITEM_CURE2 = 21;
const int ITEM_CURE3 = 43;

static int value(char c)
{
	if (isdigit(c))
		return c - '0';
	else
		return c - 'A' + 10;
}

static char value2(int i)
{
	if (i < 10)
		return i + '0';
	else
		return (i-10) + 'A';
}

static void save_url(const char *filename, const char *buf)
{
	FILE *f = fopen(filename, "wb");

	int half = strlen(buf)/2;

	for (int i = 0; i < half; i++) {
		char b1 = *buf;
		buf++;
		char b2 = *buf;
		buf++;
		int high = value(b1);
		int low = value(b2);
		int result = (high << 4) + low;
		fputc(result, f);
	}

	fclose(f);
}

char *create_url(unsigned char *bytes, int len)
{
	static char store[5000*3];
	
	int j = 0;
	for (int i = 0; i < len; i++) {
		int h = (bytes[i] & 0xf0) >> 4;
		int l = bytes[i] & 0x0f;
		store[j++] = value2(h);
		store[j++] = value2(l);
	}
	store[j] = 0;
	return store;
}

const int MAX_SPELLS = 256;
const int MAX_INVENTORY = 32;
const int MAX_PARTY = 4;

static std::string mapArea;
static char *areaName;
static bool milestones[8000];
static int heroSpot;
static int runtime;
static int gold;
int hero_x, hero_y;

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
};

struct INFO
{
	CombatantInfo info;
	std::string name;
	CombatFormation formation;
};

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

struct Inventory {
	int index;
	int quantity;
};

static INFO **infos;
static Inventory inventory[MAX_INVENTORY];

/*
 * Write 32 bits, little endian.
 */
void iputl(long l, gzFile f)
{
	gzputc(f, (int)(l & 0xFF));
	gzputc(f, (int)((l >> 8) & 0xFF));
	gzputc(f, (int)((l >> 16) & 0xFF));
	gzputc(f, (int)((l >> 24) & 0xFF));
}


/*
 * Read 32 bits, little endian
 */
long igetl(gzFile f)
{
	int c1 = gzgetc(f);
	int c2 = gzgetc(f);
	int c3 = gzgetc(f);
	int c4 = gzgetc(f);
	return (long)c1 | ((long)c2 << 8) | ((long)c3 << 16) | ((long)c4 << 24);
}

void my_pack_putc(int c, gzFile f)
{
	gzputc(f, c);
}


int my_pack_getc(gzFile f)
{
	int c = gzgetc(f);
	return c;
}


const char* readString(gzFile f)
{
	static char buf[1024];
	int length = igetl(f);
	int i;

	for (i = 0; i < length; i++)
		buf[i] = my_pack_getc(f);
	buf[i] = 0;

	return buf;
}

bool readMilestones(bool* ms, int num, gzFile f)
{
	for (int i = 0; i < num/8; i++) {
		int c = gzgetc(f);
		if (c == EOF) {
			return false;
		}
		for (int j = 0; j < 8; j++) {
			if (i*8+j >= num)
				break;
			ms[i*8+j] = c & 0x80;
			c <<= 1;
		}
	}
	return true;
}

void readStats(int who, gzFile f)
{
	int exists = my_pack_getc(f);

	if (!exists) {
		infos[who] = NULL;
		return;
	}

	const char *name = readString(f);
	infos[who]->name = std::string(name);

	infos[who]->formation = (CombatFormation)igetl(f);

	CombatantInfo &info = infos[who]->info;
	
	// abilities
	info.abilities.hp = igetl(f);
	info.abilities.maxhp = igetl(f);
	info.abilities.attack = igetl(f);
	info.abilities.defense = igetl(f);
	info.abilities.speed = igetl(f);
	info.abilities.mp = igetl(f);
	info.abilities.maxmp = igetl(f);
	info.abilities.mdefense = igetl(f);
	info.abilities.luck = igetl(f);
	
	// equipment
	info.equipment.lhand = igetl(f);
	info.equipment.rhand = igetl(f);

	info.equipment.harmor = igetl(f);
	info.equipment.carmor = igetl(f);
	info.equipment.farmor = igetl(f);
	info.equipment.lquantity = igetl(f);
	info.equipment.rquantity = igetl(f);
	
	int num_spells = igetl(f);

	for (int i = 0; i < num_spells; i++) {
		int exists = my_pack_getc(f);
		if (exists) {
			info.spells[i] = std::string(readString(f));
		}
		else {
			info.spells[i] = "";
		}
	}

	info.experience = igetl(f);
	info.characterClass = igetl(f);
	info.condition = (CombatCondition)igetl(f);
}


void readInventory(gzFile f)
{
	for (int i = 0; i < MAX_INVENTORY; i++) {
		inventory[i].index = igetl(f);
		inventory[i].quantity = igetl(f);
	}
}

void writeString(const char* s, gzFile f)
{
	int length = strlen(s);

	iputl(length, f);

	for (int i = 0; i < length; i++)
		my_pack_putc(s[i], f);
}

bool writeMilestones(bool* ms, int num, gzFile f)
{
	int i;
	int count = 0;
	for (i = 0; i < num/8; i++) {
		int c = 0;
		for (int j = 0; j+i*8 < num && j < 8; j++) {
			c |= (ms[j+i*8] << (7-j));
		}
		if (gzputc(f, c) == EOF)
			return false;
		else
			count++;
	}
	return true;
}

void writeStats(INFO *i, gzFile f)
{
	if (!i) {
		my_pack_putc(0, f);
		return;
	}
	else {
		my_pack_putc(1, f);
	}

	// name
	writeString(i->name.c_str(), f);

	// formation
	iputl((int)i->formation, f);

	CombatantInfo &info = i->info;

	// abilities
	iputl(info.abilities.hp, f);
	iputl(info.abilities.maxhp, f);
	iputl(info.abilities.attack, f);
	iputl(info.abilities.defense, f);
	iputl(info.abilities.speed, f);
	iputl(info.abilities.mp, f);
	iputl(info.abilities.maxmp, f);
	iputl(info.abilities.mdefense, f);
	iputl(info.abilities.luck, f);
	
	// equipment
	iputl(info.equipment.lhand, f);
	iputl(info.equipment.rhand, f);
	iputl(info.equipment.harmor, f);
	iputl(info.equipment.carmor, f);
	iputl(info.equipment.farmor, f);
	iputl(info.equipment.lquantity, f);
	iputl(info.equipment.rquantity, f);

	int num_spells = MAX_SPELLS;
	iputl(num_spells, f);

	for (int i = 0; i < num_spells; i++) {
		if (info.spells[i] != "") {
			my_pack_putc(1, f);
			writeString(info.spells[i].c_str(), f);
		}
		else {
			my_pack_putc(0, f);
		}
	}

	iputl(info.experience, f);
	iputl(info.characterClass, f);
	iputl(info.condition, f);
}


void writeInventory(gzFile f)
{
	for (int i = 0; i < MAX_INVENTORY; i++) {
		iputl(inventory[i].index, f);
		iputl(inventory[i].quantity, f);
	}
}



void saveGame(const char* filename)
{
	gzFile f = NULL;
	f = gzopen(filename, "wb9");
	if (!f)
		return;

	int x = hero_x;
	int y = hero_y;

	if (!writeMilestones(milestones, 8000, f)) {
		gzclose(f);
		return;
	}

	iputl(heroSpot, f);

	try {
		writeString(areaName, f);
		iputl(x, f);
		iputl(y, f);
		for (int i = 0; i < MAX_PARTY; i++) {
			writeStats(infos[i], f);
		}
		writeInventory(f);

		iputl(runtime, f);
		iputl(gold, f);

		if (mapArea == "") {
			iputl(0, f);
		}
		else {
			writeString(mapArea.c_str(), f);
		}
	}
	catch (...) {
		gzclose(f);
		return;
	}

	gzclose(f);
}

bool loadGame(const char* filename)
{
	gzFile f = NULL;
	f = gzopen(filename, "rb");
	if (!f)
		return false;

	readMilestones(milestones, 8000, f);

	heroSpot = igetl(f);

	std::string mapArea = "";

	try {
		areaName = strdup(readString(f));
		hero_x = igetl(f);
		hero_y = igetl(f);

		for (int i = 0; i < MAX_PARTY; i++) {
			readStats(i, f);
		}
	
		readInventory(f);

		runtime = igetl(f);
		gold = igetl(f);

		mapArea = std::string(readString(f));
	}
	catch (...) {
		gzclose(f);
		return false;
	}

	gzclose(f);

	return true;
}

int findItemIndex(int id)
{
	for (int i = 0; i < MAX_INVENTORY; i++) {
		if (inventory[i].index >= 0) {
			if (inventory[i].index == id)
				return i;
		}
	}

	for (int i = 0; i < MAX_INVENTORY; i++) {
		if (inventory[i].index < 0)
			return i;
	}

	return -1;
}

int main(int argc, char **argv)
{
	int pos;
	
#ifdef __APPLE__
	char dir[1000];
	strcpy(dir, argv[0]);
	printf("before dir=%s\n", dir);
	pos = strlen(dir)-1;
	while (pos >= 0 && dir[pos] != '/')
		pos--;
	if (dir[pos] == '/')
		dir[pos] = 0;
	printf("after dir=%s\n", dir);
	chdir(dir);
#endif

	infos = new INFO*[MAX_PARTY];
	for (int i = 0; i < MAX_PARTY; i++) {
		infos[i] = new INFO;
	}

	const char *filename = "save.txt";

	if (argc > 1)
		filename = strdup(argv[1]);

	char buf[100000];
	pos = 0;
	int c;
	FILE *f = fopen(filename, "r");
	while ((c = fgetc(f)) != EOF)
		buf[pos++] = c;
	buf[pos] = 0;
	fclose(f);

	save_url("__tmp.save__", buf);

	bool ret = loadGame("__tmp.save__");
	printf("%s", ret ? "Load OK.\n" : "Load failed.\n");
	if (!ret) return 1;



	// UI
	for (int i = 0; i < MAX_PARTY; i++) {
		if (infos[i]) {
			infos[i]->info.abilities.hp = infos[i]->info.abilities.maxhp;
			infos[i]->info.abilities.mp = infos[i]->info.abilities.maxmp;
		}
	}

	int indexes[6] = {
		ITEM_CURE,
		ITEM_CURE2,
		ITEM_CURE3,
		ITEM_HEAL,
		ITEM_HOLY_WATER,
		ITEM_ELIXIR,
	};

	int nums[6];
	pos = 0;

	char input[1000];

	std::cout << "Extra Cure (0-99): ";
	std::cin.getline(input, 1000);;
	nums[pos++] = atoi(input);
	std::cout << "Extra Cure2 (0-99): ";
	std::cin.getline(input, 1000);;
	nums[pos++] = atoi(input);
	std::cout << "Extra Cure3 (0-99): ";
	std::cin.getline(input, 1000);;
	nums[pos++] = atoi(input);
	std::cout << "Extra Heal (0-99): ";
	std::cin.getline(input, 1000);;
	nums[pos++] = atoi(input);
	std::cout << "Extra Holy Water (0-99): ";
	std::cin.getline(input, 1000);;
	nums[pos++] = atoi(input);
	std::cout << "Extra Elixir (0-99): ";
	std::cin.getline(input, 1000);;
	nums[pos++] = atoi(input);
	std::cout << "Enter new x position (-1 means no move. Use carefully!): ";
	std::cin.getline(input, 1000);
	int new_hero_x = atoi(input);
	std::cout << "Enter new y position (-1 means no move. Use carefully!): ";
	std::cin.getline(input, 1000);
	int new_hero_y = atoi(input);

	if (new_hero_x != -1)
		hero_x = new_hero_x;
	if (new_hero_y != -1)
		hero_y = new_hero_y;

	for (int i = 0; i < 6; i++) {
		int j = findItemIndex(indexes[i]);
		if (j < 0)
			printf("Error: No room for item %d.\n", i);
		else {
			inventory[j].index = indexes[i];
			inventory[j].quantity += nums[i];
			if (inventory[j].quantity > 99)
				inventory[j].quantity = 99;
		}
	}

	saveGame("__tmp.save__");
	
	pos = 0;
	f = fopen("__tmp.save__", "rb");
	while ((c = fgetc(f)) != EOF)
		buf[pos++] = c;
	fclose(f);

	char *result = create_url((unsigned char *)buf, pos);

	f = fopen("out.txt", "w");
	fprintf(f, "%s", result);
	fclose(f);

	remove("__tmp.save__");

	printf("Done. Saved to 'out.txt'\n");
	system("sleep 5");
}

