#ifndef COMBAT_STATUS
#define COMBAT_STATUS


enum CombatStatusType {
	COMBAT_WAITING = 0,
	COMBAT_DECIDING,
	COMBAT_DEAD,
	COMBAT_ATTACKING,
	COMBAT_USING,
	COMBAT_CASTING,
	COMBAT_RUNNING,
	COMBAT_DEFENDING,
	COMBAT_BUSY,
	COMBAT_SKIP
};


struct CombatStatusInfo {
	CombatStatusType type;
	struct {
	} waiting;
	struct {
	} deciding;
	struct {
		std::vector<Combatant *> who;
	} attacking;
	struct {
	} use;
	struct {
	} casting;
	struct {
	} running;
};


#endif

