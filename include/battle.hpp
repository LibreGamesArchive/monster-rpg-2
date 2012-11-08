#ifndef BATTLE_HPP
#define BATTLE_HPP



static inline CombatEntity *nth(std::list<CombatEntity *> &l, int n)
{
	int i = 0;
	std::list<CombatEntity *>::iterator it = l.begin();
	for (i = 0; i < n; i++) {
		it++;
	}
	return *it;
}


enum BattleResult {
	BATTLE_PLAYER_WIN = 0,
	BATTLE_ENEMY_WIN,
	BATTLE_PLAYER_RUN,
	BATTLE_CONTINUE // not done yet
};


enum BattleStatus {
   BATTLE_STATUS_SELECTING = 0,
   BATTLE_STATUS_WAITING,
   BATTLE_STATUS_ACTING
};


enum MessageLocation {
	MESSAGE_LEFT = 0,
	MESSAGE_TOP,
	MESSAGE_RIGHT
};


struct BattleMessage {
	MessageLocation location;
	std::string text;
	int lifetime;
};


class Battle {
public:
	bool isInWater(void);

	std::string getName(void) { return name; }
	void removeEnemyName(std::string name);
	void addEnemyName(std::string name);
	void end(BattleResult result);
	int getEntityIndex(int id);
	int getNextEntity(int id);
	void moveEntity(int toMove, int where);

	void setNextEntity(int n);
	std::list<CombatEntity *> &getEntities(void);
	CombatEntity *getEntity(int id);
	bool getAttackedFromBehind(void);

	void addEntity(CombatEntity *c, bool force_pos = false, int id = -1, int index = -1);
	void addMessage(MessageLocation loc, std::string msg, int lifetime);

	void start(void);
	BattleResult update(int step);
	void draw(void);
	void initLua(void);
	bool run(bool force = false);
	CombatPlayer *findPlayer(int number);
	bool isRunning(void);
	void storeStats(bool awardExperience);

	int getNumEnemies(void);

	void resortEntity(CombatEntity *e);

	void drawWhichStatus(bool enemy, bool player);

	Battle(std::string name, bool can_run = true);
	~Battle(void);

private:
	bool allDead(CombatEntityType type);
	void drawStatus(void);

	std::string name;
	// not z sorted
	std::list<CombatEntity *> entities; // no frills
	// z sorted
	std::list<CombatEntity *> zsorted_entities;
	int currentEntity;
	// these change position of players/enemies
	bool attackedFromBehind;
	bool preemptive;
	lua_State *luaState;
	int numEnemies;
	std::list<CombatEntity *> acting_entities;
        BattleStatus status;
	std::map<std::string, int> enemyNames;
	MBITMAP *bg;
	bool entityLock;
	std::vector<CombatEntity *> entitiesToAddLater;
	std::vector<bool> force_posesToAddLater;
	std::vector<int> idsToAddLater;
	std::vector<int> indicesToAddLater;
	bool playersAllDead;
	int playersAllDeadCount;
	std::list<BattleMessage> messages;
	int messageCount;
	bool enemiesAllDead;
	int enemiesAllDeadStage;
	bool running;
	bool boss_fight;
	bool inWater;
	int musicFadeCount;
	int currId;
	bool can_run; // can never run from boss fight
	BattleResult shortcircuit;
	int nextEntity;
	bool deleteIts;
	std::list<CombatEntity *>::iterator deleteItentities;
	std::list<CombatEntity *>::iterator deleteItzsorted_entities;
	std::list<CombatEntity *>::iterator insertPoint;
	bool draw_enemy_status;
	bool draw_player_status;
	int fade_regulator;
};


extern Battle *battle;


std::string getConditionName(CombatCondition cc);
Combatant *findRandomPlayer(void);
Combatant *findWeakestPlayer(void);

void preloadSFX(std::string name);
void preloadSpellSFX(std::string spellName);
bool playBattlePreload(std::string name);

#endif

