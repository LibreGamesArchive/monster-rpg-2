#ifndef PLAYER_HPP
#define PLAYER_HPP


const int MAX_PARTY = 4;

// Exp. for level x == exp. for level (x-1) + EXP_INFLATION
const int EXP_INFLATION = 500;

const int MAX_GOLD = 999999;


class Player {
public:
	Object *getObject(void);
	CombatantInfo &getInfo(void);
	CombatFormation getFormation(void);
	std::string getName(void);
	MBITMAP *getIcon(void);

	void setObject(Object *o);
	void setInfo(CombatantInfo &i);
	void setFormation(CombatFormation f);
	void setName(std::string name);

	Combatant *makeCombatant(int number);

	Player(std::string name, bool putInParty = true);
	~Player();

	// FIXME: free stuff in destructor
protected:
	Object *object;
	CombatantInfo info;
	std::string name;
	CombatFormation formation;
	int number;
	MBITMAP *icon;
};


extern Player *party[MAX_PARTY];
extern int heroSpot; // when Eny is in the party array
extern int gold;

extern CombatantInfo guardStartInfo;
extern CombatantInfo enyStartInfo;
extern CombatantInfo riderStartInfo;
extern CombatantInfo riosStartInfo;
extern CombatantInfo gunnarStartInfo;
extern CombatantInfo faelonStartInfo;
extern CombatantInfo melStartInfo;
extern CombatantInfo tiggyStartInfo;
extern CombatantInfo tipperStartInfo;

int getLevel(int experience);
int getExperience(int level);
bool levelUp(Player *player, int bonus = 0);
void copyInfo(CombatantInfo &info, CombatantInfo &newinfo);

void increaseGold(int amount);
void giveSpell(std::string who, std::string spell);

std::string baseAnimName(std::string name);
AnimationSet *findBattleAnim(std::string name);
void referenceBattleAnim(std::string name);
void unreferenceBattleAnim(std::string name);

#endif

