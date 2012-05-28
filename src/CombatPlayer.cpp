#include "monster2.hpp"
#include <allegro5/internal/aintern_bitmap.h>

// FIXME?!?!
#include "allegro5/internal/aintern_pixels.h"

static bool end_this_battle = false;
static BattleResult battleResult;

enum ChooseReason {
	CHOOSE_ATTACK = 0,
	CHOOSE_ITEM,
	CHOOSE_MAGIC
};

enum WalkReason {
	WALK_BACK = 0,
	WALK_FORWARD
};

enum ConfirmReason {
	CONFIRM_DEFEND = 0,
	CONFIRM_RUN,
	CONFIRM_FORM
};

ActionHandler *createMainHandler(CombatPlayer *p, std::string name);


static std::vector<Combatant *> attacked;
static int itemIndex = -1;
static Combatant *itemTarget;

int spellIndex = -1;
static int numSpellTargets;
static Combatant **spellTargets;


class ConfirmHandler : public ActionHandler {
public:
	ActionHandler *act(int step, Battle *b);
	void init(void);
	void shutdown(void);
	ConfirmHandler(CombatPlayer *p, ConfirmReason r);
	
protected:
	MFrame *frame;
	MTextButton *button;
	ConfirmReason reason;
};



class MainHandler : public ActionHandler {
public:
	ActionHandler *act(int step, Battle *b) {
		if (player->getInfo().condition == CONDITION_CHARMED
		|| player->getInfo().condition == CONDITION_SHADOW)
			return createMainHandler(player, "Attack");
		for (int i = 0; i < 5 && buttons[i]; i++) {
			if (mainWidget == buttons[i]) {
				//playPreloadedSample("select.ogg");
				ActionHandler *newHandler =
					createMainHandler(player, std::string(actions[i]));
				return newHandler;
			}
		}
		if (area && area->getName() == "tutorial") {
			if (iphone_shaken(0.1)) {
				iphone_clear_shaken();
				battle->run(true);
				return NULL;
			}
		}

		for (int i = 0; i < MAX_PARTY; i++) {
			FakeWidget *formWidget = formWidgets[i];
			if (formWidget && formWidget->getHoldStart() > 0 &&
					formWidget->getHoldStart()+600UL < tguiCurrentTimeMillis()) {
				if (use_dpad) {
					InputDescriptor _id = getInput()->getDescriptor();
					while (_id.button1) {
						al_rest(0.001);
						_id = getInput()->getDescriptor();
					}
				}
				playPreloadedSample("select.ogg");
				showPlayerInfo(i);
				formWidget->reset();
				return NULL;
			}
			else if (formWidget && mainWidget == formWidget
				&& player == battle->findPlayer(i)) {
				return new ConfirmHandler(player, CONFIRM_FORM);
			}
		}
		return NULL;
	}

	void init(void) {
		frame = new MFrame(3, BH-47, (BW*2)/3-6, 45);
		shadow = new MShadow();
		int x = 5;
		int y;
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
		if (!use_dpad)
			y = BH-45;
		else
#endif
			y = BH - 47;
		memset(buttons, 0, sizeof(buttons));
		for (int i = 0; i < 5 && actions[i]; i++) {
			bool disabled = false;
			if (std::string(actions[i]) == "Item") {
				bool inv_empty = true;
				for (int j = 0; j < MAX_INVENTORY; j++) {
					if (inventory[j].index >= 0) {
						inv_empty = false;
						break;
					}
				}
				if (inv_empty) {
					disabled = true;
				}
			}
			else if (std::string(actions[i]) == "Magic") {
				int remaining = player->getInfo().abilities.mp;
				bool enough = false;
				for (int i = 0; i < MAX_SPELLS_IN_THIS_GAME; i++) {
					int cost = getSpellCost(player->getInfo().spells[i]);
					if (cost <= remaining) {
						enough = true;
						break;
					}
				}
				if (!enough) {
					disabled = true;
				}
			}
                        const int start = (((BW*2)/3) - (30*5))/2;
			if (use_dpad) {
				MTextButton *b = new MTextButton(x, y, std::string(actions[i]), disabled);
				buttons[i] = b;
				//y += m_text_height(game_font);
				y += 9;
			}
			else {
				MIcon *b = NULL;
				MCOLOR greyed_out = m_map_rgb(100, 100, 100);
				if (std::string(actions[i]) == "Attack") {
					b = new MIcon(start, 123,
						getResource("combat_media/battle_icons/attack.png"), disabled ? greyed_out : white, true, actions[i], true);
				}
				else if (std::string(actions[i]) == "Item") {
					b = new MIcon(start+30, 123,
						getResource("combat_media/battle_icons/item.png"), disabled ? greyed_out : white, true, actions[i], true);
				}
				else if (std::string(actions[i]) == "Magic") {
					b = new MIcon(start+120, 123,
						getResource("combat_media/battle_icons/magic.png"), disabled ? greyed_out : white, true, actions[i], true);
				}
				else if (std::string(actions[i]) == "Defend") {
					b = new MIcon(start+60, 123,
						getResource("combat_media/battle_icons/defend.png"), disabled ? greyed_out : white, true, actions[i], true);
				}
				else if (std::string(actions[i]) == "Run") {
					b = new MIcon(start+90, 123,
						getResource("combat_media/battle_icons/run.png"), disabled ? greyed_out : white, true, actions[i], true);
				}
				buttons[i] = b;
			}
		}

		for (int i = 0; i < MAX_PARTY; i++) {
			CombatPlayer *p = battle->findPlayer(i);
			if (p) {
				formWidgets[i] = new FakeWidget(p->getX()-10, p->getY()-32/*playerheight=32*/, 20, 32, true, true);
			}
			else {
				formWidgets[i] = NULL;
			}
		}
		tguiSetParent(0);
		tguiAddWidget(frame);
		tguiAddWidget(shadow);
		tguiSetParent(frame);
		for (int i = 0; i < 5 && buttons[i]; i++) {
			tguiAddWidget(buttons[i]);
		}
		for (int i = 0; i < MAX_PARTY; i++) {
			if (formWidgets[i])
				tguiAddWidget(formWidgets[i]);
		}
		tguiSetFocus(buttons[0]);
	}

	void shutdown(void) {
		tguiDeleteWidget(frame);
		delete frame;
		tguiDeleteWidget(shadow);
		delete shadow;
		for (int i = 0; i < 5; i++) {
			delete buttons[i];
			free(actions[i]);
		}
		for (int i = 0; i < MAX_PARTY; i++) {
			if (formWidgets[i])
				delete formWidgets[i];
		}

		battle->drawWhichStatus(true, true); // restore
	}

	MainHandler(CombatPlayer *p) :
		ActionHandler(p)
	{
		playerInput = true;

		lua_State *luaState = lua_open();

		openLuaLibs(luaState);

		registerCFunctions(luaState);

		runGlobalScript(luaState);

		std::string realName = party[player->getNumber()]->getName();

		debug_message("Real name = %s\n", realName.c_str());
	
		unsigned char *bytes;
		int file_size;

		debug_message("Loading player script...\n");
		bytes = slurp_file(getResource("combat_players/%s.%s", realName.c_str(), getScriptExtension().c_str()), &file_size);
	if (luaL_loadbuffer(luaState, (char *)bytes, file_size, "chunk")) {
			dumpLuaStack(luaState);
			throw ReadError();
		}
		delete[] bytes;

		debug_message("Running player script...\n");
		if (lua_pcall(luaState, 0, 0, 0)) {
			dumpLuaStack(luaState);
			lua_close(luaState);
			throw ScriptError();
		}

		if (area && area->getName() == "tutorial") {
			memset(actions, 0, sizeof(actions));
			actions[0] = strdup("Attack");
			actions[1] = strdup("Item");
			actions[2] = strdup("Defend");
			actions[3] = strdup("Run");
		}
		else {
			callLua(luaState, "get_actions", ">sssss");

			memset(actions, 0, sizeof(actions));
			for (int i = 0; i < 5; i++) {
				const char *action = lua_tostring(luaState, -5+i);
				if (action && action[0])
					actions[i] = strdup(action);
			}

			lua_pop(luaState, 5);
		}

		lua_close(luaState);

		battle->drawWhichStatus(false, true); // draw only player status
	}

protected:
	MFrame *frame;
	char *actions[5];
	TGUIWidget *buttons[5];
	FakeWidget *formWidgets[MAX_PARTY];
	MShadow *shadow;
};


class EndHandler : public ActionHandler {
public:
	bool isEnd(void) { return true; }
	EndHandler(CombatPlayer *p) :
		ActionHandler(p)
	{
		playerInput = false;
	}
};



class SkipHandler : public ActionHandler {
public:
	ActionHandler *act(int step, Battle *b) {
		return new EndHandler(player);
	}
	SkipHandler(CombatPlayer *p) :
		ActionHandler(p)
	{
		playerInput = false;
	}
};


class AttackHandler : public ActionHandler {
public:
	ActionHandler *act(int step, Battle *b);
	void init(void) {}
	void shutdown(void) {}
	AttackHandler(CombatPlayer *p);
protected:
	int count;
};


class UseItemHandler : public ActionHandler {
public:
	ActionHandler *act(int step, Battle *b);
	void init(void);
	void shutdown(void);
	void draw(void);
	UseItemHandler(CombatPlayer *p);

protected:
	ItemEffect *effect;
	int lifetime;
	int count;
};

	
class UseMagicHandler : public ActionHandler {
public:
	ActionHandler *act(int step, Battle *b);
	void init(void);
	void shutdown(void);
	void draw(void);
	UseMagicHandler(CombatPlayer *p);

protected:
	Spell *spell;
	bool spellInited;
};

	
class RunHandler : public ActionHandler {
public:
	ActionHandler *act(int step, Battle *b);
	void init(void);
	void shutdown(void);
	void draw(void);
	RunHandler(CombatPlayer *p);

protected:
	bool success;
	float speed;
};
	
class WalkHandler : public ActionHandler {
public:
	ActionHandler *act(int step, Battle *b) {
		if (dir == DIRECTION_EAST) {
			x += 0.1f * step;
		}
		else {
			x -= 0.1f * step;
		}

		if (fabs(x - initialX) >= 24) {
			x = dx;
		}

		if (fabs(x - initialX) >= 24) {
			player->setX(initialX + 
				(dir == DIRECTION_EAST ? 1 : -1) * 24);					
			switch (reason) {
				case WALK_BACK:
					if (dir == DIRECTION_EAST) {
						player->setDirection(DIRECTION_WEST);
					}
					else {
						player->setDirection(DIRECTION_EAST);
					}
					player->getAnimationSet()->setSubAnimation("stand");
					if (!use_programmable_pipeline) {
						player->getWhiteAnimationSet()->setSubAnimation("stand");
					}
					return new EndHandler(player);
				case WALK_FORWARD:
					if (dir == DIRECTION_EAST) {
						player->setDirection(DIRECTION_EAST);
					}
					else {
						player->setDirection(DIRECTION_WEST);
					}
					player->getAnimationSet()->setSubAnimation("stand");
					if (!use_programmable_pipeline) {
						player->getWhiteAnimationSet()->setSubAnimation("stand");
					}
					return new MainHandler(player);
			}
			// done, choose based on reason
		}

		player->setX(x);

		return NULL;
	}

	void init(void) {
	}

	void shutdown(void) {
	}

	WalkHandler(CombatPlayer *p, Direction dir, WalkReason reason) :
		ActionHandler(p)
	{
		playerInput = false;
		this->dir = dir;
		this->reason = reason;
		initialX = (int)p->getX();
		x = initialX;
		player->getAnimationSet()->setSubAnimation("walk");
		if (!use_programmable_pipeline) {
			player->getWhiteAnimationSet()->setSubAnimation("walk");
		}
		if (reason == WALK_BACK) {
			if (player->getLocation() == LOCATION_RIGHT) {
				player->setDirection(DIRECTION_EAST);
			}
			else {
				player->setDirection(DIRECTION_WEST);
			}
		}
		if (player->getDirection() == DIRECTION_EAST) {
			dx = x + 24;
		}
		else {
			dx = x - 24;
		}
	}
protected:
	Direction dir;
	WalkReason reason;
	float x;
	float dx;
	int initialX;
};
	

class ChooseTargetHandler : public ActionHandler {
public:
	ActionHandler *act(int step, Battle *b) {
		if ((applyFrame && tguiActiveWidget == applyFrame) || (applyButton && tguiActiveWidget == applyButton) || tguiActiveWidget == NULL) {
			if (iphone_shaken(0.1)) {
				iphone_clear_shaken();
				return new MainHandler(player);
			}
		}

		if (mainWidget == chooser || player->getInfo().condition == CONDITION_CHARMED || player->getInfo().condition == CONDITION_SHADOW || (mainWidget && (mainWidget == applyButton))) {

			// Don't accept negative values (back line)
			if (mainWidget == chooser && chooser->getSelected().size() > 0 && chooser->getSelected()[0] < 0) {
				std::vector<int>& v = chooser->getSelected();
				for (int i = 0; i < (int)v.size(); i++) {
					v[i] = -v[i] - 1;
				}
				return NULL;
			}

			player->itemIndex_display = -1;
			player->spellIndex_display = -1;

			std::vector<int> &c = chooser->getSelected();

			if ((c.size() <= 0 && !(player->getInfo().condition == CONDITION_CHARMED || player->getInfo().condition == CONDITION_SHADOW))) {
				onscreen_swipe_to_attack = false;
				return new MainHandler(player);
			}
			else {
				if (reason == CHOOSE_ATTACK) {
					if (player->getInfo().condition == CONDITION_CHARMED || player->getInfo().condition == CONDITION_SHADOW) {
						attacked.push_back(findRandomPlayer());
					}
					else {
						for (int i = 0; i < (int)c.size(); i++) {
							attacked.push_back((Combatant *)points[c[i]].data);
						}
						if (!use_dpad) {
							onscreen_swipe_to_attack = false;
							gameInfo.milestones[MS_SWIPE_TO_ATTACK] = true;
						}
					}
					return new AttackHandler(player);
				}
				else if (reason == CHOOSE_ITEM) {
					itemTarget = (Combatant *)points[c[0]].data;
					return new UseItemHandler(player);
				}
				else if (reason == CHOOSE_MAGIC) {
					numSpellTargets = c.size();
					spellTargets = new Combatant *[numSpellTargets];
					for (int i = 0; i < numSpellTargets; i++) {
						spellTargets[i] = (Combatant *)points[c[i]].data;
					}
					return new UseMagicHandler(player);
				}
			}
		}

		return NULL;
	}

	ChooseTargetHandler(CombatPlayer *p, ChooseReason reason) :
		ActionHandler(p)
	{
		playerInput = true;
		this->reason = reason;

		if (!use_dpad) {
			if (reason == CHOOSE_ATTACK && !gameInfo.milestones[MS_SWIPE_TO_ATTACK]) {
				onscreen_swipe_to_attack = true;
			}
		}

		std::list<CombatEntity *> &entities = battle->getEntities();


		std::list<CombatEntity *>::iterator it;
		int player_x;
		if (battle->getAttackedFromBehind()) {
			player_x = 0;
		}
		else {
			player_x = BW;
		}
		for (it = entities.begin(); it != entities.end(); it++) {
			CombatEntity *e = *it;
			bool add = false;
			if (e->getType() == COMBATENTITY_TYPE_PLAYER) {
				CombatPlayer *p = (CombatPlayer *)e;
				if (p->getInfo().condition == CONDITION_SWALLOWED) {
					continue;
				}
			}
			if (reason == CHOOSE_ITEM) {
				if (e->getType() == COMBATENTITY_TYPE_PLAYER
						|| e->getType() == COMBATENTITY_TYPE_ENEMY) {
					add = true;
				}
			}
			else if (reason == CHOOSE_ATTACK) {
				if (e->getType() == COMBATENTITY_TYPE_ENEMY) {
					Combatant *c = (Combatant *)e;
					if (c->getInfo().abilities.hp > 0)
						add = true;
				}
			}
			else { // CHOOSE_MAGIC
				bool black = getSpellAlignment(
					player->getInfo().spells[spellIndex])
					== SPELL_BLACK;
				if (black) {
					if (/*e->getType() == COMBATENTITY_TYPE_PLAYER ||*/
							e->getType() == COMBATENTITY_TYPE_ENEMY) {
						Combatant *c = (Combatant *)e;
						if (c->getInfo().abilities.hp > 0)
							add = true;
					}
				}
				else { // white magic
					if (e->getType() == COMBATENTITY_TYPE_PLAYER || e->getType() == COMBATENTITY_TYPE_ENEMY) {
						add = true;
					}
				}
				//add = true;
			}
			if (add) {
				Combatant *c = (Combatant *)e;
				AnimationSet *a = c->getAnimationSet();
				MultiPoint point;
				if (c->getType() == COMBATENTITY_TYPE_PLAYER) {
					if (battle->getAttackedFromBehind()) {
						point.west = true;
					}
					else {
						point.west = false;
					}
				}
				else {
					if (battle->getAttackedFromBehind()) {
						point.west = false;
					}
					else {
						point.west = true;
					}
				}
				if (c->getType() == COMBATENTITY_TYPE_PLAYER) {
					if (point.west) {
						point.y = c->getY() - 20;
						point.x = c->getX() + (a->getWidth()/2) - 5;
						if (player_x < point.x) {
							player_x = point.x;
						}
					}
					else {
						point.y = c->getY() - 20;
						point.x = c->getX() - (a->getWidth()/2) + 5;
						if (player_x > point.x) {
							player_x = point.x;
						}
					}
				}
				else {
					if (point.west) {
						point.y = c->getY() - a->getHeight() + 10;
						if (point.y < 10) point.y = 10;
						point.x = c->getX() + (a->getWidth()/2) - 5;
					}
					else {
						point.y = c->getY() - a->getHeight() + 10;
						if (point.y < 10) point.y = 10;
						point.x = c->getX() - (a->getWidth()/2) + 5;
					}
				}
				point.data = e;
				points.push_back(point);
			}
		}

		for (unsigned int i = 0; i < points.size(); i++) {
			if (battle->getAttackedFromBehind()) {
				if (points[i].x < 60) {
					points[i].x = player_x;
				}
			}
			else {
				if (points[i].x > BW-60) {
					points[i].x = player_x;
				}
			}
		}

		bool black = false;
		if (reason == CHOOSE_MAGIC)
			black = getSpellAlignment(
				player->getInfo().spells[spellIndex])
				== SPELL_BLACK;

		std::vector<int> sel;
		bool get_west = false;
		bool choosing_player = false;

		if (reason == CHOOSE_ATTACK) {
			get_west = !battle->getAttackedFromBehind();
		}
		else if (reason == CHOOSE_ITEM) {
			get_west = battle->getAttackedFromBehind();
			choosing_player = true;
		}
		else { // CHOOSE_MAGIC
			if (black) {
				if (battle->getAttackedFromBehind()) {
					get_west = false;
				}
				else {
					get_west = true;
				}
			}
			else {
				choosing_player = true;
				if (battle->getAttackedFromBehind()) {
					get_west = true;
				}
				else {
					get_west = false;
				}
			}
		}

		// FIXME: true for spells
		bool can_multi;
		if (reason == CHOOSE_MAGIC) {
			// some spells are for mutilple people some aren't
			std::string sn = player->getInfo().spells[spellIndex];
			if (sn == "Revive" || sn == "Stun" || sn == "Slow"
				|| sn == "Quick" || sn == "Charm") {
				can_multi = false;
			}
			else
				can_multi = true;
		}
		else
			can_multi = false;

		// Choose default selected dude
		int index = 0;
		int xchoice = get_west ? 0 : BW;
		int ychoice = BH;
		for (int i = 0; i < (int)points.size(); i++) {
			if (points[i].west != get_west) {
				continue;
			}
			if (choosing_player) {
				if (points[i].y < ychoice) {
					index = i;
					ychoice = (int)points[i].y;
				}
			}
			else {
				if (get_west) {
					if (points[i].x > xchoice) {
						index = i;
						xchoice = (int)points[i].x;
					}
				}
				else {
					if (points[i].x < xchoice) {
						index = i;
						xchoice = (int)points[i].x;
					}
				}
			}
		}

		sel.push_back(index);
		chooser = new MMultiChooser(points, can_multi);

		chooser->setSelected(sel);
		sel.clear();

		if (have_mouse || !use_dpad) {
			applyFrame = new MFrame(BW/3-20, 110+(50/2)-6-5, 112, 22, true);
			applyButton = new MTextButton(BW/3-20+6, 110+(50/2)-6+2,
									 "Apply");
				/*
			applyFrame = new MFrame(BW/3-20, 110+7, 112, 18, true);
			applyButton = new MTextButton(BW/3-20+6, 121,
				"Apply");
				 */
		}

		tguiSetParent(0);
		tguiAddWidget(chooser);
		if (have_mouse || !use_dpad) {
			tguiAddWidget(applyFrame);
			tguiAddWidget(applyButton);
		}
		tguiSetFocus(chooser);
	}

	virtual ~ChooseTargetHandler(void) {
		tguiDeleteWidget(chooser);
		if (have_mouse || !use_dpad) {
			tguiDeleteWidget(applyFrame);
			tguiDeleteWidget(applyButton);
			delete applyFrame;
			delete applyButton;
		}
		delete chooser;
		points.clear();
	}

protected:
	ChooseReason reason;
	MMultiChooser *chooser;
	std::vector<MultiPoint> points;
	MFrame *applyFrame;
	MTextButton *applyButton;
};

ActionHandler *AttackHandler::act(int step, Battle *b)
{
	/*
	AnimationSet *wAnim = getWeaponAnimation(player->getInfo());
	if (wAnim) {
		wAnim->update(step);
	}
	*/

	count += step;
	if (count < player->getAnimationSet()->getCurrentAnimation()->getLength())
		return NULL;
	
	//player->setDrawWeapon(false);

	if (attacked.size() == 1 && attacked[0]->getName() == "EvilTig_ending") {
		MBITMAP *bmp;
		bool win;
		// 38 == mystic sword
		if (player->getInfo().equipment.lhand == 38 || player->getInfo().equipment.rhand == 38) {
			bmp = m_load_bitmap(getResource("media/slice.png"));
			loadPlayDestroy("slice.ogg");
			win = true;
			end_this_battle = true;
			battleResult = BATTLE_PLAYER_WIN;
		}
		else {
			bmp = m_load_bitmap(getResource("media/ching.png"));
			loadPlayDestroy("ching.ogg");
			win = false;
		}
		m_set_target_bitmap(buffer);
		m_draw_bitmap(bmp, 0, 0, 0);
		dpad_off();
		drawBufferToScreen();
		m_flip_display();
		m_rest(5);
		dpad_on();
		clear_input_events();
		//fadeOut(black);
		if (win) {
			attacked[0]->getAnimationSet()->setSubAnimation("broken");
			if (!use_programmable_pipeline) {
				attacked[0]->getWhiteAnimationSet()->setSubAnimation("broken");
			}
		}
		//m_set_target_bitmap(buffer);
		//battle->draw();
		//fadeIn(black);
		m_destroy_bitmap(bmp);
		//return new EndHandler(player);
	}
	else {
		for (int i = 0; i < (int)attacked.size(); i++) {
			doAttack(player, attacked[i]);
		}
	}
	
	attacked.clear();

	// walk back
	Direction dir;
	if (player->getLocation() == LOCATION_RIGHT) {
		dir = DIRECTION_EAST;
	}
	else {
		dir = DIRECTION_WEST;
	}
	ActionHandler *ah = new WalkHandler(player, dir, WALK_BACK);
	return ah;
}

AttackHandler::AttackHandler(CombatPlayer *p) :
	ActionHandler(p)
{
	playerInput = false;
	count = 0;

	CombatantInfo &info = p->getInfo();

	int lid = -1;
	int rid = -1;

	if (info.equipment.lhand >= 0)
		lid = items[info.equipment.lhand].id;
	if (info.equipment.rhand >= 0)
		rid = items[info.equipment.rhand].id;

	player->getAnimationSet()->setSubAnimation("attack");
	if (!use_programmable_pipeline) {
		player->getWhiteAnimationSet()->setSubAnimation("attack");
	}
	
	player->getAnimationSet()->getCurrentAnimation()->reset();

	std::string soundName = getWeaponSound(player->getInfo());
	if (soundName != "") {
		loadPlayDestroy(soundName);
	}
}


class ChooseItemHandler : public ActionHandler {
public:
	ActionHandler *act(int step, Battle *b);
	void init(void);
	void shutdown(void);
	ChooseItemHandler(CombatPlayer *p);

protected:
	MItemSelector *itemSelector;
};


ActionHandler *ChooseItemHandler::act(int step, Battle *b)
{
	if (mainWidget == itemSelector) {
		itemIndex = itemSelector->getSelected();
		if (itemIndex < 0) {
			return new MainHandler(player);
		}
		else {
			// FIXME: can't use tents
			if (items[inventory[itemIndex].index].type == ITEM_TYPE_STATUS) {
				player->itemIndex_display = itemIndex;
				return new ChooseTargetHandler(player, CHOOSE_ITEM);
			}
			else {
				playPreloadedSample("error.ogg");
				return new MainHandler(player);
			}
		}
	}
	return NULL;
}

void ChooseItemHandler::init(void)
{
	tguiSetParent(0);
	tguiAddWidget(itemSelector);
	tguiSetFocus(itemSelector);
	battle->drawWhichStatus(false, false);
}

void ChooseItemHandler::shutdown(void)
{
	tguiDeleteWidget(itemSelector);
	delete itemSelector;
	battle->drawWhichStatus(true, true);
}

ChooseItemHandler::ChooseItemHandler(CombatPlayer *p) :
	ActionHandler(p)
{
	playerInput = true;
	itemSelector = new MItemSelector(113, BH-2, 0, 0, false);
}


class ChooseSpellHandler : public ActionHandler {
public:
	ActionHandler *act(int step, Battle *b);
	void init(void);
	void shutdown(void);
	ChooseSpellHandler(CombatPlayer *p);

protected:
	MSpellSelector *spellSelector;
};



ActionHandler *ChooseSpellHandler::act(int step, Battle *b)
{
	if (mainWidget == spellSelector) {
		spellIndex = spellSelector->getSelected();
		if (spellIndex < 0) {
			return new MainHandler(player);
		}
		else {
			CombatantInfo &info = player->getInfo();
			if (info.spells[spellIndex] != "" &&
				getSpellCost(info.spells[spellIndex]) <= info.abilities.mp) {
				player->spellIndex_display = spellIndex;
				return new ChooseTargetHandler(player, CHOOSE_MAGIC);
			}
			else {
				playPreloadedSample("error.ogg");
				return new MainHandler(player);
			}
		}
	}
	return NULL;
}

void ChooseSpellHandler::init(void)
{
	tguiSetParent(0);
	tguiAddWidget(spellSelector);
	tguiSetFocus(spellSelector);
	battle->drawWhichStatus(false, false);
}

void ChooseSpellHandler::shutdown(void)
{
	tguiDeleteWidget(spellSelector);
	delete spellSelector;
	battle->drawWhichStatus(true, true);
}

ChooseSpellHandler::ChooseSpellHandler(CombatPlayer *p) :
	ActionHandler(p)
{
	playerInput = true;
	spellSelector = new MSpellSelector(113, BH-2, 0, 0, false, NULL, p->getInfo());
}


ActionHandler *UseMagicHandler::act(int step, Battle *b)
{
	static bool spellComplete = false;

	if (!spellInited) {
		spell->init(player, spellTargets, numSpellTargets);
		spellInited = true;
	}
	if (!spellComplete && spell->update(step)) {
		spell->apply();
		player->getInfo().abilities.mp -=
			getSpellCost(player->getInfo().spells[spellIndex]);
		spellComplete = true;
	}
	if (spellComplete && player->getAnimationSet()->getCurrentAnimation()->isFinished()) {
		Direction dir;
		if (battle->getAttackedFromBehind()) {
			dir = DIRECTION_WEST;
		}
		else {
			dir = DIRECTION_EAST;
		}
		spellComplete = false;
		return new WalkHandler(player, dir, WALK_BACK);
	}
	return NULL;
}

void UseMagicHandler::init(void)
{
	player->getAnimationSet()->setSubAnimation("cast");
	if (!use_programmable_pipeline) {
		player->getWhiteAnimationSet()->setSubAnimation("cast");
	}
	player->getAnimationSet()->reset();
	
	spell = createSpell(player->getInfo().spells[spellIndex]);
	//battle->addEntity(spell);
	//lifetime = spell->getLifetime();

	std::string msg = "{008}" + std::string(_t(player->getInfo().spells[spellIndex].c_str()));
	MessageLocation loc;
	if (player->getLocation() == LOCATION_RIGHT)
		loc = MESSAGE_RIGHT;
	else
		loc = MESSAGE_LEFT;
	battle->addMessage(loc, msg, 1500);
}

void UseMagicHandler::shutdown(void)
{
	delete spell;
}

void UseMagicHandler::draw(void)
{
}

UseMagicHandler::UseMagicHandler(CombatPlayer *p) :
	ActionHandler(p)
{
	spellInited = false;

	playerInput = false;
}


ActionHandler *UseItemHandler::act(int step, Battle *b)
{
	count += step;

	if (count >= lifetime && player->getAnimationSet()->getCurrentAnimation()->isFinished()) {
		Direction dir;
		if (battle->getAttackedFromBehind()) {
			dir = DIRECTION_WEST;
		}
		else {
			dir = DIRECTION_EAST;
		}
		return new WalkHandler(player, dir, WALK_BACK);
	}

	return NULL;
}

void UseItemHandler::init(void)
{
	player->getAnimationSet()->setSubAnimation("use");
	if (!use_programmable_pipeline) {
		player->getWhiteAnimationSet()->setSubAnimation("use");
	}
	player->getAnimationSet()->reset();

	effect = createItemEffect(itemIndex);
	Combatant **targets = new Combatant*[1];
	targets[0] = itemTarget;
	effect->init(player, targets, 1);
	battle->addEntity(effect);
	lifetime = effect->getLifetime();

	int index = inventory[itemIndex].index;
	std::string msg = getItemIcon(index) + std::string(_t(getItemName(index).c_str()));
	MessageLocation loc;
	if (player->getLocation() == LOCATION_RIGHT)
		loc = MESSAGE_RIGHT;
	else
		loc = MESSAGE_LEFT;
	battle->addMessage(loc, msg, 1500);
}

void UseItemHandler::shutdown(void)
{
}

void UseItemHandler::draw(void)
{
}

UseItemHandler::UseItemHandler(CombatPlayer *p) :
	ActionHandler(p),
	count(0)
{
	playerInput = false;
}


ActionHandler *RunHandler::act(int step, Battle *b)
{
	if (!success) {
		// walk back
		Direction dir;
		if (player->getLocation() == LOCATION_RIGHT) {
			dir = DIRECTION_EAST;
		}
		else {
			dir = DIRECTION_WEST;
		}
		ActionHandler *ah = new WalkHandler(player, dir, WALK_BACK);
		return ah;
	}

	bool done = true;

	for (int i = 0; i < MAX_PARTY; i++) {
		CombatPlayer *p = battle->findPlayer(i);
		if (p && p->getInfo().abilities.hp > 0) {
			p->setX(p->getX() + speed*step);
			int w = p->getAnimationSet()->getWidth();
			if (p->getX() > -(w*2) && p->getX() < (BW+w*2)) {
				done = false;
			}
		}
	}

	if (done) {
		return new EndHandler(player);
	}

	return NULL;
}


void RunHandler::init(void)
{
	success = battle->run();
	if (!success) {
		battle->addMessage(MESSAGE_TOP, "Couldn't run!", 1000);
		return;
	}

	if (player->getLocation() == LOCATION_RIGHT) {
		speed = 0.1f;
	}
	else {
		speed = -0.1f;
	}

	for (int i = 0; i < MAX_PARTY; i++) {
		CombatPlayer *p = battle->findPlayer(i);
		if (p && p->getInfo().abilities.hp > 0) {
			p->setRunning(true);
			p->getAnimationSet()->setSubAnimation("walk");
			if (!use_programmable_pipeline) {
				p->getWhiteAnimationSet()->setSubAnimation("walk");
			}
			if (p->getLocation() == LOCATION_RIGHT) {
				p->setDirection(DIRECTION_EAST);
			}
			else {
				p->setDirection(DIRECTION_WEST);
			}
		}
	}
}


void RunHandler::shutdown(void)
{
}


void RunHandler::draw(void)
{
}


RunHandler::RunHandler(CombatPlayer *p) :
	ActionHandler(p)
{
}

ActionHandler *ConfirmHandler::act(int step, Battle *b)
{
	if (use_dpad) {
		INPUT_EVENT ie = get_next_input_event();
		if (ie.button2 == DOWN) {
			use_input_event();
			return new MainHandler(player);
		}
	}
	if (have_mouse || !use_dpad) {
		if (iphone_shaken(0.1)) {
			iphone_clear_shaken();
			return new MainHandler(player);
		}
	}
	if (mainWidget == button) {
		if (reason == CONFIRM_DEFEND) {
			player->setDefending(true);
			player->getAnimationSet()->setSubAnimation("defend");
			if (!use_programmable_pipeline) {
				player->getWhiteAnimationSet()->setSubAnimation("defend");
			}
			// walk back
			Direction dir;
			if (player->getLocation() == LOCATION_RIGHT) {
				dir = DIRECTION_EAST;
			}
			else {
				dir = DIRECTION_WEST;
			}
			ActionHandler *ah = new WalkHandler(player, dir, WALK_BACK);
			return ah;
		}
		else if (reason == CONFIRM_RUN) {
			return new RunHandler(player);
		}
		else if (reason == CONFIRM_FORM) {
			CombatFormation f = player->getFormation();
			if (f == FORMATION_FRONT)
				f = FORMATION_BACK;
			else
				f = FORMATION_FRONT;
			player->setFormation(f);
			{
				int x = player->getX();
				if (player->getLocation() == LOCATION_LEFT) {
					if (f == FORMATION_FRONT) {
						x += 16;
					}
					else {
						x -= 16;
					}
				}
				else {
					if (f == FORMATION_FRONT) {
						x -= 16;
					}
					else {
						x += 16;
					}
				}
				player->setX(x);
			}
			// walk back
			Direction dir;
			if (player->getLocation() == LOCATION_RIGHT) {
				dir = DIRECTION_EAST;
			}
			else {
				dir = DIRECTION_WEST;
			}
			ActionHandler *ah = new WalkHandler(player, dir, WALK_BACK);
			return ah;
		}
	}
	return NULL;
}
	
void ConfirmHandler::init(void) {
	frame = new MFrame(BW/3-20, 110+(50/2)-6-5, 125, 22, true);
	if (reason == CONFIRM_DEFEND) {
		button = new MTextButton(62, 110+(50/2)-6+2,
										 "Defend?");
	}
	else if (reason == CONFIRM_RUN) {
		button = new MTextButton(62, 110+(50/2)-6+2,
										 "Run?");
	}
	else if (reason == CONFIRM_FORM) {
		button = new MTextButton(62, 110+(50/2)-6+2,
										 "Change stance?");
	}
	tguiSetParent(0);
	tguiAddWidget(frame);
	tguiSetParent(frame);
	tguiAddWidget(button);
	tguiSetFocus(button);
}
	
void ConfirmHandler::shutdown(void) {
	tguiDeleteWidget(frame);
	delete frame;
	delete button;
}
	
ConfirmHandler::ConfirmHandler(CombatPlayer *p, ConfirmReason r) :
ActionHandler(p)
{
	playerInput = true;
	reason = r;
}


ActionHandler *createMainHandler(CombatPlayer *p, std::string name)
{
	if (name == "Skip") {
		return new SkipHandler(p);
	}
	else if (name == "Attack") {
		return new ChooseTargetHandler(p, CHOOSE_ATTACK);
	}
	else if (name == "Item") {
		return new ChooseItemHandler(p);
	}
	else if (name == "Run") {
		return new ConfirmHandler(p, CONFIRM_RUN);
	}
	else if (name == "Magic") {
		return new ChooseSpellHandler(p);
	
	}
	else if (name == "Defend") {
		return new ConfirmHandler(p, CONFIRM_DEFEND);
	}

	return NULL;
}


bool CombatPlayer::update(int step)
{
	#ifndef LITE
	charmAnim->update(step);
	#endif

	if (!(info.condition == CONDITION_STONED) && !(info.condition == CONDITION_MUSHROOM)) {
		animSet->update(step);
		if (!use_programmable_pipeline) {
			whiteAnimSet->update(step);
		}
	}
	return false;
}



bool CombatPlayer::act(int step, Battle *b)
{
	if (handler == NULL) {
		if (info.abilities.hp <= 0) {
			return true;
		}
		defending = false;
		Direction d;
		if (getLocation() == LOCATION_RIGHT) {
			d = DIRECTION_WEST;
		}
		else {
			d = DIRECTION_EAST;
		}
		handler = new WalkHandler(this, d, WALK_FORWARD);
		handler->init();
		choosing = true;
		acting = true;
	}
	else {
		ActionHandler *a = handler->act(step, b);
		if (a != NULL) {
			handler->shutdown();
			delete handler;
			handler = a;
			handler->init();
			if (!handler->requiresPlayerInput()) {
				choosing = false;
			}
			else {
				choosing = true;
			}
			if (handler->isEnd()) {
				handler->shutdown();
				delete handler;
				handler = NULL;
				acting = false;
				//tguiMakeFresh();
				if (end_this_battle) {
					end_this_battle = false;
					battle->end(battleResult);
				}
				return true;
			}
		}
	}

	return false;
}


// note: only bright choosing and normal draws work with battle water
void CombatPlayer::draw(void)
{
	if (itemIndex_display >= 0) {
		char text[100];
		sprintf(text, "(%s)", _t(getItemName(inventory[itemIndex_display].index).c_str()));
		int w = m_text_length(game_font, _t(text));
		int h = m_text_height(game_font);
		int x, y;
		if (location == LOCATION_RIGHT) {
			x = BW-w-10;
		}
		else {
			x = 5;
		}
		y = 6;
		mDrawFrame(x, y, w+5, h+3, true);
		mTextout_simple(text, x+2, y+3, grey);
	}
	else if (spellIndex_display >= 0) {
		char text[100];
		sprintf(text, "(%s)", _t(info.spells[spellIndex_display].c_str()));
		int w = m_text_length(game_font, _t(text));
		int h = m_text_height(game_font);
		int x, y;
		if (location == LOCATION_RIGHT) {
			x = BW-w-10;
		}
		else {
			x = 5;
		}
		y = 5;
		mDrawFrame(x, y, w+5, h+3, true);
		mTextout_simple(text, x+2, y+3, grey);
	}


	if (info.condition == CONDITION_SWALLOWED)
		return;

	if (!acting && !running) {
		if (getInfo().abilities.hp < 1) {
			animSet->setSubAnimation("dead");
			info.condition = CONDITION_NORMAL;
		}
		else if ((!battle->isRunning() && (getInfo().abilities.hp <= (getInfo().abilities.maxhp/10))) &&
				!(info.condition == CONDITION_STONED || info.condition == CONDITION_POISONED || info.condition == CONDITION_PARALYZED || info.condition == CONDITION_MUSHROOM || info.condition == CONDITION_WEBBED)) {
			animSet->setSubAnimation("hurt");		
		}
		else if (getInfo().condition == CONDITION_NORMAL && animSet->getSubName() != "hit") {
			animSet->setSubAnimation("stand");
			if (!use_programmable_pipeline) {
				whiteAnimSet->setSubAnimation("stand");
			}
		}
	}

	int cx = getX()-getAnimationSet()->getWidth()/2;
	int cy = getY() - getAnimationSet()->getHeight();
	int cw = getAnimationSet()->getWidth();
	int ch = getAnimationSet()->getHeight() - 5;
	int savex, savey, savew, saveh;
	al_get_clipping_rectangle(&savex, &savey, &savew, &saveh);

	if (info.condition == CONDITION_MUSHROOM) {
		draw_shadow(mushroom, x, y, location == LOCATION_LEFT);
	}
	else {
		if (battle->isInWater())
			al_set_clipping_rectangle(0, cy, BW, ch);

		MBITMAP *b = animSet->getCurrentAnimation()->getCurrentFrame()->getImage()->getBitmap();
		draw_shadow(b, x, y, location == LOCATION_LEFT);

		if (battle->isInWater())
			al_set_clipping_rectangle(savex, savey, savew, saveh);
	}

	int flags;

	if (direction == DIRECTION_EAST) {
		flags = M_FLIP_HORIZONTAL;
	}
	else {
		flags = 0;
	}

	int w = animSet->getWidth();
	int h = animSet->getHeight();

	bool bright;
	bright = ((unsigned)tguiCurrentTimeMillis() % 500 < 250);
	if (choosing && bright) {
		int amount;
		float amountF;
		ALLEGRO_COLOR vcol;
		if (info.condition == CONDITION_SHADOW) {
			amountF = -1.0f;
			amount = 255*amountF;
			vcol = black;
		}
		else {
			amountF = 0.7f;
			amount = 255*0.7;
			vcol = white;
		}

#if (defined A5_OGL || defined A5_D3D) && !defined WIZ && !defined NO_SHADERS

		if (battle->isInWater())
			al_set_clipping_rectangle(cx, cy, cw, ch);

		if (use_programmable_pipeline) {
			al_set_shader(display, brighten);
			MBITMAP *sb = animSet->getCurrentAnimation()->getCurrentFrame()->getImage()->getBitmap();
			al_set_shader_sampler(brighten, "tex", sb->bitmap, 0);
			al_set_shader_float(brighten, "brightness", amountF);
			al_use_shader(brighten, true);
			m_draw_tinted_bitmap(sb, vcol, x-(w/2), y-h, flags);
			al_use_shader(brighten, false);
			al_set_shader(display, default_shader);
		}
		else {
			if (info.condition == CONDITION_SHADOW) {
				MBITMAP *bmp = animSet->getCurrentAnimation()->getCurrentFrame()->getImage()->getBitmap();
				m_draw_tinted_bitmap(bmp, black, x-(w/2), y-h, flags);
			}
			else {
				MBITMAP *bmp = whiteAnimSet->getCurrentAnimation()->getCurrentFrame()->getImage()->getBitmap();
				m_draw_bitmap(bmp, x-(w/2), y-h, flags);
			}
		}
		
		if (battle->isInWater())
			al_set_clipping_rectangle(savex, savey, savew, saveh);

#endif
	}
	else {
		if (info.condition != CONDITION_STONED) {
			if (stone_bmp) {
				m_destroy_bitmap(stone_bmp);
				stone_bmp = NULL;
			}
		}

		if (info.condition == CONDITION_STONED) {
			if (!stone_bmp) {
				work = animSet->getCurrentAnimation()->getCurrentFrame()->getImage()->getBitmap();
				int ww = m_get_bitmap_width(work);
				int wh = m_get_bitmap_height(work);
				stone_bmp = m_create_bitmap(ww, wh);
#ifdef ALLEGRO_ANDROID
				al_lock_bitmap(work->bitmap->parent, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READONLY);
#else
				m_lock_bitmap(work, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READONLY);
#endif
				m_lock_bitmap(stoneTexture, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READONLY);

				ALLEGRO_BITMAP *target = al_get_target_bitmap();
				
				m_set_target_bitmap(stone_bmp);
				m_clear(al_map_rgba_f(0, 0, 0, 0));

				m_lock_bitmap(stone_bmp, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READWRITE);
			
				for (int yy = 0; yy < wh; yy++) {
					for (int xx = 0; xx < ww; xx++) {
						MCOLOR c = m_get_pixel(work, xx, yy);
						if (c.a != 1.0f) continue;
						MCOLOR c2;
						c2 = m_get_pixel(stoneTexture, xx%m_get_bitmap_width(stoneTexture), yy%m_get_bitmap_width(stoneTexture));
						MCOLOR result;
						float avg = (c.r + c.g + c.b) / 6.0f;
						result.r = avg + c2.r/2;
						result.g = avg + c2.g/2;
						result.b = avg + c2.b/2;
						result.a = 1.0f;
						m_put_pixel(xx, yy, result);
					}
				}
#ifdef ALLEGRO_ANDROID
				al_unlock_bitmap(work->bitmap->parent);
#else
				m_unlock_bitmap(work);
#endif
				m_unlock_bitmap(stoneTexture);

				m_unlock_bitmap(stone_bmp);

				al_set_target_bitmap(target);
			}

			m_draw_bitmap(stone_bmp, x-(w/2), y-h, flags);
		}
		else if (info.condition == CONDITION_MUSHROOM) {
			m_draw_bitmap(mushroom, x-TILE_SIZE/2, y-TILE_SIZE, 0);
		}
		else if (info.condition == CONDITION_SHADOW) {
			int amount = -255;
			int amountF = -1.0f;
			(void)amount;
			(void)amountF;
#ifdef ALLEGRO4
			MBITMAP *bmp = animSet->getCurrentAnimation()->getCurrentFrame()->getImage()->getBitmap();
			brightened_blit(bmp, x-(w/2), y-h, amount, flags);
#else
			{
			m_save_blender();
			float  r, g, b;
			r = 0.0f;
			g = 0.0f;
			b = 0.0f;
			m_set_blender(ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA, al_map_rgb_f(r, g, b));
			MBITMAP *sb = animSet->getCurrentAnimation()->getCurrentFrame()->getImage()->getBitmap();
			m_draw_tinted_bitmap(sb, m_map_rgb(r, g, b), x-(w/2), y-h, flags);
			m_restore_blender();
			}
#endif
		}
		else {
			if (battle->isInWater())
				al_set_clipping_rectangle(cx, cy, cw, ch);
			animSet->draw(x-(w/2), y-h, flags);
			if (battle->isInWater())
				al_set_clipping_rectangle(savex, savey, savew, saveh);
			
			if (info.condition == CONDITION_WEBBED)
				m_draw_bitmap(webbed, x-16, y-32, 0);
			if (info.condition == CONDITION_QUICK || info.condition == CONDITION_SLOW
				|| info.condition == CONDITION_CHARMED) {
				int cx = location == LOCATION_LEFT ? x+w/2 : x-w/2;
				if (info.condition == CONDITION_QUICK) {
					draw_clock(cx, y-h/2, 7, false);
				}
				else if (info.condition == CONDITION_SLOW) {
					draw_clock(cx, y-h/2, 7, true);
				}
				else if (info.condition == CONDITION_CHARMED) {
					charmAnim->draw(cx-15, y-(h*3)/4-5, 0);
				}
			}
		}
	}

	if (handler)
		handler->draw();
}

CombatFormation CombatPlayer::getFormation(void)
{
	return formation;
}

int CombatPlayer::getNumber(void)
{
	return number;
}


void CombatPlayer::setFormation(CombatFormation f)
{
	formation = f;
}

void CombatPlayer::setNumber(int number)
{
	this->number = number;
}

void CombatPlayer::setDrawWeapon(bool dw)
{
	drawWeapon = dw;
}


bool CombatPlayer::isActing(void)
{
	return acting;
}


CombatPlayer::CombatPlayer(std::string name, int number, AnimationSet *animSet, std::string prefix) :
	Combatant(name),
	number(number),
	handler(NULL),
	choosing(false),
	acting(false),
	drawWeapon(false),
	running(false),
	stone_bmp(NULL)
{
	this->animSet = animSet;

	if (!use_programmable_pipeline) {
		whiteAnimSet = animSet->clone(CLONE_PLAYER);
		whiteAnimSet->setSubAnimation("stand");
	}
	
	if (prefix != "") {
		animSet->setPrefix(prefix);
		if (!use_programmable_pipeline) {
			whiteAnimSet->setPrefix(prefix);
		}
	}

	type = COMBATENTITY_TYPE_PLAYER;
	formation = FORMATION_FRONT;

	itemIndex_display = -1;
	spellIndex_display = -1;

	#ifndef LITE
	charmAnim = new AnimationSet(getResource("combat_media/Charm.png"));
	#endif
}


CombatPlayer::~CombatPlayer(void)
{
	if (handler) {
		handler->shutdown();
		delete handler;
		handler = NULL;
	}

#ifndef LITE
	delete charmAnim;
#endif
	
	if (!use_programmable_pipeline) {
		delete whiteAnimSet;
	}
			
	if (stone_bmp) {
		m_destroy_bitmap(stone_bmp);
		stone_bmp = NULL;
	}
}

