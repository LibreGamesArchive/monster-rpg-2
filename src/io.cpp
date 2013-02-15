#include "monster2.hpp"


unsigned char memory_save[20000];
int memory_save_offset = 0;
bool using_memory_save = false;
bool memory_saved = false;


/*
 * Write 32 bits, little endian.
 */
void iputl(long l, gzFile f)
{
	if (using_memory_save) {
		memory_save[memory_save_offset++] = (l & 0xFF);
		memory_save[memory_save_offset++] = ((l >> 8) & 0xFF);
		memory_save[memory_save_offset++] = ((l >> 16) & 0xFF);
		memory_save[memory_save_offset++] = ((l >> 24) & 0xFF);
	}
	else {
		if (gzputc(f, (int)(l & 0xFF)) == -1) {
			throw WriteError();
		}
		if (gzputc(f, (int)((l >> 8) & 0xFF)) == -1) {
			throw WriteError();
		}
		if (gzputc(f, (int)((l >> 16) & 0xFF)) == -1) {
			throw WriteError();
		}
		if (gzputc(f, (int)((l >> 24) & 0xFF)) == -1) {
			throw WriteError();
		}
	}
}


/*
 * Read 32 bits, little endian
 */
long igetl(gzFile f)
{
	int c1 = gzgetc(f);
	if (c1 == -1) throw ReadError();
	int c2 = gzgetc(f);
	if (c2 == -1) throw ReadError();
	int c3 = gzgetc(f);
	if (c3 == -1) throw ReadError();
	int c4 = gzgetc(f);
	if (c4 == -1) throw ReadError();
	return (long)c1 | ((long)c2 << 8) | ((long)c3 << 16) | ((long)c4 << 24);
}

void my_pack_putc(int c, gzFile f)
{
	if (using_memory_save) {
		memory_save[memory_save_offset++] = c;
	}
	else {
		if (gzputc(f, c) == EOF)
			throw WriteError();
	}
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


void writeString(const char* s, gzFile f)
{
	int length = strlen(s);

	iputl(length, f);

	for (int i = 0; i < length; i++)
		my_pack_putc(s[i], f);
}

/*
 * Read milestones from a file.
 * Milestones are stored as one big bit field, the
 * first bit in the file being the first milestone.
 */
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


static bool writeMilestones(bool* ms, int num, gzFile f)
{
	int i;
	int count = 0;
	for (i = 0; i < num/8; i++) {
		int c = 0;
		for (int j = 0; j+i*8 < num && j < 8; j++) {
			c |= (ms[j+i*8] << (7-j));
		}
		if (using_memory_save) {
			memory_save[memory_save_offset++] = c;
		}
		else {
			if (gzputc(f, c) == EOF)
				return false;
			else
				count++;
		}
	}
	return true;
}


static void writeStats(Player *p, gzFile f)
{
	if (!p) {
		my_pack_putc(0, f);
		return;
	}
	else {
		my_pack_putc(1, f);
	}

	// name
	writeString(p->getName().c_str(), f);

	// formation
	iputl((int)p->getFormation(), f);

	CombatantInfo &info = p->getInfo();

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


static void writeInventory(gzFile f)
{
	for (int i = 0; i < MAX_INVENTORY; i++) {
		iputl(inventory[i].index, f);
		iputl(inventory[i].quantity, f);
	}
}


static void readStats(int who, gzFile f)
{
	int exists = my_pack_getc(f);

	if (!exists) {
		party[who] = NULL;
		return;
	}

	const char *name = readString(f);

	
	Player *p = new Player(std::string(name), false);
	party[who] = p;

	if (who == heroSpot) {
		p->setObject(new Object());
		p->getObject()->setPerson(true);
		p->getObject()->setAnimationSet(getResource("objects/Eny.png"));
		al_lock_mutex(input_mutex);
		p->getObject()->setInput(getInput());
		al_unlock_mutex(input_mutex);
	}

	p->setFormation((CombatFormation)igetl(f));

	CombatantInfo &info = p->getInfo();

	
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
		std::string n = "";
		if (exists) {
			n = std::string(readString(f));
		}
		if (i < MAX_SPELLS_IN_THIS_GAME) {
			info.spells[i] = n;
		}
	}

	info.experience = igetl(f);
	info.characterClass = igetl(f);
	info.condition = (CombatCondition)igetl(f);
}


static void readInventory(gzFile f)
{
	for (int i = 0; i < MAX_INVENTORY; i++) {
		inventory[i].index = igetl(f);
		inventory[i].quantity = igetl(f);
	}
}


// TODO: if changing this change writeTime and loadGame too
void saveGame(const char* filename, std::string mapArea)
{
	gzFile f = NULL;
	if (!using_memory_save) {
		f = gzopen(filename, "wb9");
		if (!f)
			throw WriteError();
	}

	if (filename && strncmp(filename, "auto", 4)) {
		if (saveFilename) free(saveFilename);
		saveFilename = strdup(filename);
	}

	std::string areaName;
	if (area) {
		areaName = area->getName();
	}
	else {
		areaName = "";
	}
	debug_message("Saving in area %s\n", areaName.c_str());
	int x = party[heroSpot]->getObject()->getX();
	int y = party[heroSpot]->getObject()->getY();

	if (!writeMilestones(gameInfo.milestones, MAX_MILESTONES, f)) {
		gzclose(f);
		throw WriteError();
	}

	iputl(heroSpot, f);

	try {
		writeString(areaName.c_str(), f);
		iputl(x, f);
		iputl(y, f);
		for (int i = 0; i < MAX_PARTY; i++) {
			writeStats(party[i], f);
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
		throw WriteError();
	}

	if (!using_memory_save) {
		gzclose(f);
	}
}

// TODO: if changing this change writeTime and saveGame too
bool loadGame(const char* filename)
{
	gzFile f = NULL;
	f = gzopen(filename, "rb");
	if (!f)
		throw ReadError();

	if (saveFilename) free(saveFilename);
	saveFilename = strdup(filename);

	readMilestones(gameInfo.milestones, MAX_MILESTONES, f);

/*
	for (int i = 0; i < (int)forced_milestones.size(); i++) {
		std::pair<int, bool> &p = forced_milestones[i];
		gameInfo.milestones[p.first] = p.second;
	}
*/

	//gameInfo.milestones[MS_GOLEM_TIMER_STARTED] = false;

	heroSpot = igetl(f);

	debug_message("heroSpot = %d\n", heroSpot);
	
	char* areaName;
	std::string mapArea = "";

	try {
		areaName = strdup(readString(f));
		int x = igetl(f);
		int y = igetl(f);

		for (int i = 0; i < MAX_PARTY; i++) {
			readStats(i, f);
		}
	
		party[heroSpot]->getObject()->setPosition(x, y);
		party[heroSpot]->getObject()->getInput()->setDirection(DIRECTION_SOUTH);

		readInventory(f);

		runtime = igetl(f);
		gold = igetl(f);

		//  FIXME
		mapArea = std::string(readString(f));
	}
	catch (...) {
		gzclose(f);
		throw ReadError();
	}

	gzclose(f);

	bool ret;
	
	if (mapArea == "") {
		startArea(std::string(areaName));
		ret = false;
	}
	else {
		if (gameInfo.milestones[MS_ON_MOON]) {
			doMap(mapArea, "map2");
		}
		else
			doMap(mapArea, "map");
		ret = true;
	}

	free(areaName);
	
	
	return ret;
}

void saveTime(char *filename)
{
	#define STRLEN (bytes[offs] | (bytes[offs+1] << 8) | (bytes[offs+2] << 16) | (bytes[offs+3] << 24))

	std::vector<int> bytes;
	gzFile f = NULL;
	f = gzopen(filename, "rb");
	if (!f)
		return;
	
	while (1) {
		int i = my_pack_getc(f);
		if (i == EOF)
			break;
		bytes.push_back(i);
	}

	gzclose(f);

	// Calculate position of time
	
	// skip milestones+herospot
	int offs = MAX_MILESTONES/8 + 4;
	// skip area name and x,y
	offs += STRLEN + 4 + 8;
	// skip stats
	for (int i = 0; i < MAX_PARTY; i++) {
		bool pexists = bytes[offs++];
		if (pexists) {
			// skip name and stats
			offs += STRLEN + 4 + (17*4);
			// skip spells
			int numSpells = STRLEN;
			offs += 4;
			for (int j = 0; j < numSpells; j++) {
				bool sexists = bytes[offs++];
				if (sexists) {
					// skip spell name
					offs += STRLEN + 4;
				}
			}
			// skip last bit of stats
			offs += 4*3;
		}
	}
	// skip inventory
	offs += MAX_INVENTORY * 8;

	// all that for this:
	bytes[offs] = runtime & 0xff;
	bytes[offs+1] = (runtime >> 8) & 0xff;
	bytes[offs+2] = (runtime >> 16) & 0xff;
	bytes[offs+3] = (runtime >> 24) & 0xff;

	// write it
	f = gzopen(filename, "wb9");

	for (int i = 0; i < (int)bytes.size(); i++) {
		my_pack_putc(bytes[i], f);
	}

	gzclose(f);
	#undef STRLEN
}


void getSaveStateInfo(int num, SaveStateInfo &info, bool autosave)
{
	gzFile f = NULL;
	f = gzopen(getUserResource("%s%d.save", autosave ? "auto" : "", num), "rb");
	if (!f) {
		memset(&info, 0, sizeof(SaveStateInfo));
		return;
	}

	try {
		// skip milestones
		for (int i = 0; i < MAX_MILESTONES/8; i++) {
			my_pack_getc(f);
		}

		int hspot = igetl(f); // skip herospot

		readString(f); // skip area name
		igetl(f);
		igetl(f); // skip x, y

		for (int j = 0; j < MAX_PARTY; j++) {
			bool exists = my_pack_getc(f); // skip exists
			if (exists) {
				readString(f); // skip name
				igetl(f); // skip formation
				igetl(f); // hp
				igetl(f); // maxhp
				igetl(f); // attack
				igetl(f); // defense
				igetl(f); // speed
				igetl(f); // mp
				igetl(f); // maxmp
				igetl(f); // mdefense
				igetl(f); // luck
				igetl(f); // lhand
				igetl(f); // rhand
				igetl(f); // harmor
				igetl(f); // carmor
				igetl(f); // farmor
				igetl(f); // lquantity
				igetl(f); // rquantity

				// skip spells
				int num_spells = igetl(f);
				for (int i = 0; i < num_spells; i++) {
					int exists = my_pack_getc(f);
					if (exists) {
						readString(f);
					}
				}

				// read experience
				if (j == hspot)
					info.exp = igetl(f);
				else
					igetl(f);

				igetl(f); // skip class
				igetl(f); // skip condition
			}
		}

		// skip inventory
		for (int i = 0; i < MAX_INVENTORY; i++) {
			igetl(f);
			igetl(f);
		}

		info.time = igetl(f);
		info.gold = igetl(f);
	}
	catch (...) {
		memset(&info, 0, sizeof(info));
		gzclose(f);
		return;
	}

	gzclose(f);
}

