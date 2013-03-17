#ifndef COMBAT_PLAYER_HPP
#define COMBAT_PLAYER_HPP

class Player;
class CombatPlayer;

class ActionHandler {
public:
	virtual ActionHandler *act(int step, Battle *b) { return NULL; };
	virtual void init(void) {};
	virtual void shutdown(void) {};
	virtual bool isEnd(void) { return false; }
	virtual void draw(void) {};

	bool requiresPlayerInput(void) {
		return playerInput;
	}

	ActionHandler(CombatPlayer *p) {
		player = p;
	}

	virtual ~ActionHandler() {};

protected:
	bool playerInput;
	CombatPlayer *player;
};



class CombatPlayer : public Combatant {
public:
	/* Oh my dear Lord: publicly visible */
	int itemIndex_display;
	int spellIndex_display;

	bool act(int step, Battle *b);
	void draw(void);
	bool update(int step);

	CombatFormation getFormation(void);
	int getNumber(void);
	bool isActing(void);

	void setFormation(CombatFormation f);
	void setNumber(int number);
	void setDrawWeapon(bool dw);
	void setRunning(bool r) { running = r; }
	
	void createStoneBmp();
	void showRect(bool show);

	CombatPlayer(std::string name, int number, std::string prefix = "", bool loadImages = true);
	virtual ~CombatPlayer(void);

private:
	CombatFormation formation;
	int number;
	ActionHandler *handler;
	bool choosing;
	bool acting;
	bool drawWeapon;
	MBITMAP *work;
	AnimationSet *charmAnim;
	bool running;
	MBITMAP *stone_bmp;
	bool show_rect;
};

extern bool show_player_info_on_flip;
extern int player_number_to_show_on_flip;
extern Player *player_to_show_on_flip;

#endif

