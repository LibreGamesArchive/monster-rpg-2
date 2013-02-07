#include "monster2.hpp"

Battle *battle = NULL;

static std::string nextSpeech = "";

static std::map<std::string, MSAMPLE> preloaded_samples;
void preloadSFX(std::string name)
{
	if (preloaded_samples.find(name) != preloaded_samples.end()) {
		return;
	}
	preloaded_samples[name] = loadSample(name);
}
void preloadSpellSFX(std::string spellName)
{
	if (spellName == "Acorns") {
		preloadSFX("Acorns.ogg");
	}
	else if (spellName == "Arc") {
		preloadSFX("Arc.ogg");
	}
	else if (spellName == "Banana" || spellName == "Boulder") {
		preloadSFX("Banana.ogg");
	}
	else if (spellName == "Beam" || spellName == "Blaze") {
		preloadSFX("Beam.ogg");
	}
	else if (spellName == "Bolt1") {
		preloadSFX("bolt.ogg");
	}
	if (spellName == "Bolt2") {
		preloadSFX("Bolt2.ogg");
	}
	if (spellName == "Bolt3") {
		preloadSFX("Bolt3.ogg");
	}
	if (spellName == "Breath of Fire") {
		preloadSFX("BoF.ogg");
	}
	else if (spellName == "Claw") {
		preloadSFX("Meow.ogg");
	}
	else if (spellName == "Daisy") {
		preloadSFX("Daisy.ogg");
	}
	else if (spellName == "Drop") {
		preloadSFX("jump.ogg");
	}
	else if (spellName == "Darkness1") {
		preloadSFX("Darkness1.ogg");
	}
	else if (spellName == "Darkness2") {
		preloadSFX("Darkness2.ogg");
	}
	else if (spellName == "Darkness3") {
		preloadSFX("Darkness3.ogg");
	}
	else if (spellName == "Fireball") {
		preloadSFX("Fireball.ogg");
	}
	else if (spellName == "Fire1") {
		preloadSFX("fire1.ogg");
	}
	else if (spellName == "Fire2") {
		preloadSFX("Fire2.ogg");
	}
	else if (spellName == "Fire3") {
		preloadSFX("Fire3.ogg");
	}
	else if (spellName == "Ice1") {
		preloadSFX("ice1.ogg");
	}
	else if (spellName == "Ice3") {
		preloadSFX("Ice3.ogg");
	}
	else if (spellName == "Kiss of Death") {
		preloadSFX("high_cackle.ogg");
	}
	else if (spellName == "Laser") {
		preloadSFX("Laser.ogg");
	}
	else if (spellName == "MachineGun") {
		preloadSFX("Machine_Gun.ogg");
	}
	else if (spellName == "Orbit") {
		preloadSFX("Orbit.ogg");
	}
	else if (spellName == "Puke") {
		preloadSFX("Puke.ogg");
	}
	else if (spellName == "Punch") {
		preloadSFX("Punch.ogg");
	}
	else if (spellName == "Rend" || spellName == "Talon") {
		preloadSFX("Rend.ogg");
	}
	else if (spellName == "Sludge" || spellName == "Acid" || spellName == "BellyAcid") {
		preloadSFX("slime.ogg");
	}
	else if (spellName == "Spray") {
		preloadSFX("Spray.ogg");
	}
	else if (spellName == "Stomp") {
		preloadSFX("Stomp.ogg");
	}
	else if (spellName == "Stone") {
		preloadSFX("Stone.ogg");
	}
	else if (spellName == "Swallow") {
		preloadSFX("Mmm.ogg");
		preloadSFX("Swallow.ogg");
	}
	else if (spellName == "Touch of Death") {
		preloadSFX("TouchofDeath.ogg");
	}
	else if (spellName == "Twister") {
		preloadSFX("Twister.ogg");
	}
	else if (spellName == "UFO") {
		preloadSFX("UFO.ogg");
	}
	else if (spellName == "Wave") {
		preloadSFX("Wave.ogg");
	}
	else if (spellName == "Web") {
		preloadSFX("Web.ogg");
	}
	else if (spellName == "Weep" || spellName == "Torrent") {
		preloadSFX("Weep.ogg");
	}
	else if (spellName == "Whirlpool") {
		preloadSFX("Whirlpool.ogg");
	}
	else if (spellName == "Whip") {
		preloadSFX("Whip.ogg");
	}
}
bool playBattlePreload(std::string name)
{
	if (preloaded_samples.find(name) == preloaded_samples.end())
		return false;
	playSample(preloaded_samples[name]);
	return true;
}

std::string getConditionName(CombatCondition cc)
{
	if (cc == CONDITION_POISONED) {
		return "Poisoned";
	}
	else if (cc == CONDITION_PARALYZED) {
		return "Paralyzed";
	}
	else if (cc == CONDITION_STONED) {
		return "Stone";
	}
	else if (cc == CONDITION_SLOW) {
		return "Slow";
	}
	else if (cc == CONDITION_QUICK) {
		return "Quick";
	}
	else if (cc == CONDITION_CHARMED) {
		return "Charmed";
	}
	else if (cc == CONDITION_MUSHROOM) {
		return "Mushroom";
	}
	else if (cc == CONDITION_SHADOW) {
		return "Shadow";
	}
	else if (cc == CONDITION_WEBBED) {
		return "Webbed";
	}
	else if (cc == CONDITION_SWALLOWED) {
		return "Swallowed";
	}
	else
		return "";
}


void Battle::removeEnemyName(std::string name)
{
	// remove name
	std::map<std::string, int>::iterator it2;
	for (it2 = enemyNames.begin(); it2 != enemyNames.end(); it2++) {
		if (name == it2->first) {
			it2->second--;
			if (it2->second <= 0) {
				enemyNames.erase(it2);
				break;
			}
		}
	}
}

void Battle::addEnemyName(std::string name)
{
	enemyNames[name]++;
}


void Battle::resortEntity(CombatEntity *e)
{
	int id = e->getId();
	std::list<CombatEntity *>::iterator it;
	int i = 0;

	for (it = entities.begin(); it != entities.end(); it++, i++) {
		if (*it == e) {
			break;
		}
	}

	if (entityLock) {
		deleteItentities = std::find(entities.begin(), entities.end(), e);
		deleteItzsorted_entities = std::find(zsorted_entities.begin(), zsorted_entities.end(), e);
		deleteIts = true;
	}
	else {
		entities.erase(std::find(entities.begin(), entities.end(), e));
		zsorted_entities.erase(std::find(zsorted_entities.begin(), zsorted_entities.end(), e));
	}

	addEntity(e, false, id, i);
}


int Battle::getNumEnemies(void)
{
	int n = 0;

	std::list<CombatEntity *>::iterator it;
	for (it = entities.begin(); it != entities.end(); it++) {
		if ((*it)->getType() == COMBATENTITY_TYPE_ENEMY) {
			n++;
		}
	}

	return n;
}


void Battle::setNextEntity(int n)
{
	nextEntity = n;
}


CombatEntity *Battle::getEntity(int id)
{
	std::list<CombatEntity *>::iterator it;
	for (it = entities.begin(); it != entities.end(); it++) {
		CombatEntity *e = *it;
		if (e->getId() == id)
			return e;
	}
	return NULL;
}


void Battle::end(BattleResult r)
{
	shortcircuit = r;
}


bool Battle::getAttackedFromBehind(void)
{
	return attackedFromBehind;
}


bool Battle::isRunning(void)
{
	return running;
}


bool Battle::run(bool force)
{
	if (!force) {
		if (boss_fight || (!can_run))
			return false;

		bool success = rand() % 2;

		if (!success)
			return false;
	}
	
	running = true;

	messages.clear();

	if (!force) {
		lua_getglobal(luaState, "gold");
		int battle_gold = (int)lua_tonumber(luaState, -1);
		lua_pop(luaState, 1);
		int dropped = rand() % (battle_gold * 2);
		if (dropped > gold)
			dropped = gold;
		if (dropped > 0) {
			gold -= dropped;
			char msg[100];
			sprintf(msg, _t("Dropped %d gold..."), dropped);
			addMessage(MESSAGE_TOP, std::string(msg), 3000);
		}
		else {
			addMessage(MESSAGE_TOP, _t("Ran away..."), 3000);
		}
	}

	return true;
}


void Battle::drawStatus(void)
{
	const int height = 45;
	int separator = BW/2-6;

	if (draw_enemy_status) {
		mDrawFrame(3, BH-height-2, separator, height);
	}

	if (draw_player_status) {
		mDrawFrame(separator+9, BH-height-2, BW-(separator+9)-3, height);
	}

	bool held = al_is_bitmap_drawing_held();
	al_hold_bitmap_drawing(true);
	
	if (draw_enemy_status) {
		// print enemies 
		
		std::map<std::string, int>::iterator enemyIt;

		int x = 5;
		int y = BH-height;

		for (enemyIt = enemyNames.begin(); enemyIt != enemyNames.end(); enemyIt++) {
			std::string name = enemyIt->first;
			int num = enemyIt->second;
			if (num <= 0) continue;
			mTextout(game_font, name.c_str(), x, y,
				grey,
				black,
				WGT_TEXT_DROP_SHADOW, false);
			mTextout(game_font, _t(my_itoa(num)), separator-10, y,
				grey,
				black,
				WGT_TEXT_DROP_SHADOW, false);
			y += m_text_height(game_font);
		}
	}

	if (draw_player_status) {
		// print players + hp

		int x = separator + 11;
		int y = BH-height;
		char buf[100];
		
		for (int i = 0; i < MAX_PARTY; i++) {
			CombatPlayer *p = findPlayer(i);
			std::list<CombatEntity *>::iterator it  = std::find(acting_entities.begin(), acting_entities.end(), p);
			MCOLOR c;
			if (it != acting_entities.end())
				c = m_map_rgb(255, 255, 0);
			else
				c = grey;
			if (p != NULL) {
				mTextout(game_font, 
					_t(party[p->getNumber()]->getName().c_str()), x, y,
					c,
					black,
					WGT_TEXT_DROP_SHADOW, false);
				sprintf(buf, "%d/%d", MAX(0, p->getInfo().abilities.hp),
					p->getInfo().abilities.maxhp);
				mTextout(game_font, buf,
					BW-10-m_text_length(game_font, buf), y,
					c,
					black,
					WGT_TEXT_DROP_SHADOW, false);
			}
			y += m_text_height(game_font);
		}
	}

	al_hold_bitmap_drawing(held);
}


void Battle::draw(void)
{
	m_set_blender(M_ONE, M_ZERO, white);
	if (superpower && use_programmable_pipeline) {
		al_set_shader(display, tinter);
		al_set_shader_float(tinter, "ratio", 1);
		al_set_shader_float(tinter, "r", 0.8);
		al_set_shader_float(tinter, "g", 0);
		al_set_shader_float(tinter, "b", 0);
		al_use_shader(tinter, true);
		m_draw_bitmap(bg, 0, 0, 0);
		al_use_shader(tinter, false);
		al_set_shader(display, default_shader);
	}
	else {
		m_draw_bitmap(bg, 0, 0, 0);
	}

	m_set_blender(M_ONE, M_INVERSE_ALPHA, white);
		
	bool player_acting = false;
	for (int i = 0; i < MAX_PARTY; i++) {
		CombatPlayer *p = findPlayer(i);
		std::list<CombatEntity *>::iterator it  = std::find(acting_entities.begin(), acting_entities.end(), p);
		if (it != acting_entities.end()) {
			player_acting = true;
			break;
		}
	}
	
	std::list<CombatEntity *>::iterator it;
	for (it = zsorted_entities.begin(); it != zsorted_entities.end(); it++) {
		CombatEntity *e = *it;
		if (e->getType() == COMBATENTITY_TYPE_ENEMY) {
			((CombatEnemy *)e)->draw_shadow();
		}
	}

	// draw combatants
	for (it = zsorted_entities.begin(); it != zsorted_entities.end(); it++) {
		CombatEntity *e = *it;
		e->draw();
	}

	if (player_acting && !boss_fight) {
		for (it = zsorted_entities.begin(); it != zsorted_entities.end(); it++) {
			CombatEntity *e = *it;
			if (e->getType() == COMBATENTITY_TYPE_ENEMY) {
				CombatEnemy *enemy = (CombatEnemy *)e;
				int x = enemy->getX();
				int y = enemy->getY();
				int h = enemy->getAnimationSet()->getHeight();
				y -= h;
				y -= 4;
				if (y < 4)
					y = 4;
				CombatantInfo &info = enemy->getInfo();
				float p;
				if (info.abilities.hp <= 0)
					p = 0;
				else
					p = info.abilities.hp / (float)info.abilities.maxhp;
				const int BAR_W = 16;
				const int BAR_H = 2;
				int bar_width = p * BAR_W;
				if (bar_width > BAR_W)
					bar_width = BAR_W;
				else if (bar_width <= 0)
					bar_width = 1;
				m_draw_rectangle((x-BAR_W/2)-2, (y-BAR_H/2)-2,
						(x+BAR_W/2)+2, (y+BAR_H/2)+2, white,
						M_FILLED);
				m_draw_rectangle((x-BAR_W/2)-1, (y-BAR_H/2)-1,
						(x+BAR_W/2)+1, (y+BAR_H/2)+1, black,
						M_FILLED);
				if (p > 0) {
					MCOLOR colors[4];
					colors[0] = m_map_rgb(255, 0, 0);
					colors[1] = m_map_rgb(255, 155, 0);
					colors[2] = m_map_rgb(255, 255, 0);
					colors[3] = m_map_rgb(0, 255, 0);
					MCOLOR c = colors[(bar_width-1)/(BAR_W/4)];
					m_draw_rectangle((x-BAR_W/2), y-BAR_H/2,
							(x-BAR_W/2)+bar_width, y+BAR_H/2, c,
							M_FILLED);
				}
			}
		}
	}

	// draw status
	drawStatus();

	// draw messages
	if (messages.size() > 0) {
		int x, y;
		int w, h;
		if ((*messages.begin()).location == MESSAGE_TOP) {
			w = BW-TILE_SIZE*2;
			h = 15;
			x = (BW-w)/2;
			y = 3;
		}
		else {
			y = 10;
			w = 10 + m_text_length(game_font, _t((*messages.begin()).text.c_str()));
			h = 15;
			if ((*messages.begin()).location == MESSAGE_LEFT) {
				x = 10;
			}
			else {
				x = BW-w-10;
			}
		}
		mDrawFrame(x, y, w, h, true);
		mTextout(game_font, _t((*messages.begin()).text.c_str()), x+5, y+3,
			white, black,
			WGT_TEXT_DROP_SHADOW, false);
	}
}



bool Battle::allDead(CombatEntityType type)
{
	if (type == COMBATENTITY_TYPE_ENEMY && shortcircuit == BATTLE_PLAYER_WIN)
		return true;

	bool living = false;

	std::list<CombatEntity *>::iterator it;
	for (it = entities.begin(); it != entities.end(); it++) {
		CombatEntity *e = *it;
		if (e->getType() == type) {
			Combatant *c = (Combatant *)e;
			if (c->getInfo().abilities.hp > 0 || c->getName() == "Girl") {
				living = true;
			}
		}
	}

	return (living == false);
}


void Battle::storeStats(bool awardExperience)
{
	for (int i = 0; i < MAX_PARTY; i++) {
		CombatPlayer *p = findPlayer(i);
		if (p) {
			CombatFormation f = p->getFormation();
			if (attackedFromBehind) {
				if (f == FORMATION_FRONT)
					f = FORMATION_BACK;
				else
					f = FORMATION_FRONT;
			}
			party[i]->setFormation(f);
			party[i]->getInfo().equipment.lquantity =
				p->getInfo().equipment.lquantity;
			party[i]->getInfo().equipment.rquantity =
				p->getInfo().equipment.rquantity;
			party[i]->getInfo().equipment.lhand =
				p->getInfo().equipment.lhand;
			party[i]->getInfo().equipment.rhand =
				p->getInfo().equipment.rhand;
			memcpy(&party[i]->getInfo().abilities,
				&p->getInfo().abilities,
				sizeof(CombatantAbilities));
			if (awardExperience) {
				party[i]->getInfo().experience =
					p->getInfo().experience;
			}
			if (p->getInfo().condition == CONDITION_POISONED) {
				party[i]->getInfo().condition =
					p->getInfo().condition;
			}
			else {
				party[i]->getInfo().condition =
					CONDITION_NORMAL;
			}
		}
	}
}


BattleResult Battle::update(int step)
{
	if (!manChooser && !(name == "1Golem")) {
		if (musicFadeCount < 2000) {
			bool go = false;
			musicFadeCount += step;
			if (musicFadeCount > 2000) {
				musicFadeCount = 2000;
				go = true;
			}
			else {
				fade_regulator++;
				if (fade_regulator >= 10) {
					fade_regulator = 0;
					go = true;
				}
			}
			if (go) {
				float vol = musicFadeCount / 2000.0f;
				setMusicVolume(vol);
			}
		}
	}

	// Check for a win
	if (!playersAllDead && allDead(COMBATENTITY_TYPE_PLAYER)) {
		playersAllDead = true;
		playersAllDeadCount = 0;
		if (speechDialog) {
			dpad_on();
			tguiDeleteWidget(speechDialog);
			delete speechDialog;
			speechDialog = NULL;
		}
		messages.clear();
		storeStats(false);
	}
	else if (!enemiesAllDead && allDead(COMBATENTITY_TYPE_ENEMY)) {
		enemiesAllDead = true;
		enemiesAllDeadStage = 0;
		// award experience
		lua_getglobal(luaState, "experience");
		int exp = (int)lua_tonumber(luaState, -1);
		lua_pop(luaState, 1);
		// FIXME FIXME FIXME:
		//exp = 1500;
		for (int i = 0; i < MAX_PARTY; i++) {
			CombatPlayer *p = findPlayer(i);
			if (p && p->getInfo().abilities.hp > 0) {
				p->getInfo().experience += exp;
			}
		}
		char msg[100];
		char msg2[100];
		sprintf(msg2, _t("Gained %d experience\n"), exp);
		msg2[strlen(msg2)-1] = 0;
		sprintf(msg, "%s!\n", msg2);
		doDialogue(std::string(msg), true, 1, 3);
	}

	if (running && messages.size() <= 0) {
		storeStats(false);
		fadeOut(black);
		return BATTLE_PLAYER_RUN;
	}

	if (playersAllDead) {
		playersAllDeadCount += step;
		if (playersAllDeadCount > 2000) {
			debug_message("player loses\n");
			fadeOut(m_map_rgb(255, 0, 0));
			if (name != "first_battle" && name != "2Statues" && !manChooser && !(area && area->getName() == "tutorial")) {
				m_set_target_bitmap(buffer);
				m_clear(m_map_rgb(255, 0, 0));
				anotherDoDialogue("You were defeated in battle...\nRestore your game and save the world!\n", false, true);
			}
			return BATTLE_ENEMY_WIN;
		}
	}

	if (enemiesAllDead && !playersAllDead) {
		if (!speechDialog) {
			enemiesAllDeadStage++;
			if (enemiesAllDeadStage == 1) {
				messages.clear();
				lua_getglobal(luaState, "gold");
				int g = (int)lua_tonumber(luaState, -1);
				lua_pop(luaState, 1);
				gold += g;
				if (gold > MAX_GOLD)
					gold = MAX_GOLD;
				char msg[100];
				sprintf(msg, "%s%d %s!\n", _t("Found "), g, _t(" gold"));
				doDialogue(std::string(msg), true, 1, 3);
			}
			else if (enemiesAllDeadStage == 2) {
				callLua(luaState, "get_item", ">i");
				int item = (int)lua_tonumber(luaState, -1);
				lua_pop(luaState, 1);
				if (item >= 0) {
					int slot = findUnfullInventorySlot(item);
					if (slot >= 0) {
						//a dd it
						if (inventory[slot].index >= 0) {
							inventory[slot].quantity++;
						}
						// use empty slot
						else {
							inventory[slot].index = item;
							inventory[slot].quantity = 1;
						}
						char msg[100];
						if (isVowel(*getItemName(item).c_str())) {
							sprintf(msg, _t("Found an %s!\n"), _t(getItemName(item).c_str()));
						}
						else {
							sprintf(msg, _t("Found a %s!\n"), _t(getItemName(item).c_str()));
						}
						doDialogue(std::string(msg), true, 1, 3);
					}
				}
			}
			else {
				// Keep battle stats
				storeStats(true);
				fadeOut(black);
				return BATTLE_PLAYER_WIN;
			}
		}
	}

	if (messages.size() > 0) {
		messageCount += step;
		if (messageCount >= (*messages.begin()).lifetime) {
			messageCount = 0;
			messages.erase(messages.begin());
		}
	}

	entityLock = true;

	std::list<CombatEntity *>::iterator it;
	int count = 0;
	for (it = entities.begin(); it != entities.end(); count++) {
		CombatEntity *e = *it;
		if (e->update(step)) {
			std::list<CombatEntity *>::iterator it2;
			for (it2 = zsorted_entities.begin(); it2 != zsorted_entities.end(); it2++) {
				if (*it2 == e)
					break;
			}
			if (it2 != zsorted_entities.end()) {
				it2 = zsorted_entities.erase(it2);
			}
			for (it2 = acting_entities.begin(); it2 != acting_entities.end(); it2++) {
				if (*it2 == e)
					break;
			}
			if (it2 != acting_entities.end()) {
				it2 = acting_entities.erase(it2);
			}
			if (e->getType() == COMBATENTITY_TYPE_ENEMY) {
				// remove name
				std::map<std::string, int>::iterator it2;
				CombatEnemy *ce = (CombatEnemy *)e;
				for (it2 = enemyNames.begin(); it2 != enemyNames.end(); it2++) {
					if (std::string(_t(ce->getName().c_str())) == it2->first) {
						it2->second--;
						if (it2->second <= 0) {
							enemyNames.erase(it2);
							break;
						}
					}
				}
				if (count < currentEntity)
					currentEntity--;
			}
			delete e;
			it = entities.erase(it);
		}
		else {
			it++;
		}
	}

	if (!speechDialog && acting_entities.size() <= 0 &&
			!allDead(COMBATENTITY_TYPE_PLAYER) &&
			!allDead(COMBATENTITY_TYPE_ENEMY) &&
			!running) {
		if (nextEntity >= 0) {
			int i = 0;
			std::list<CombatEntity *>::iterator it;
			for (it = entities.begin(); it != entities.end(); it++) {
				if (nextEntity == i)
					break;
				i++;
			}

			acting_entities.push_back(*it);
			nextEntity = -1;
		}
		else {
			if (nextSpeech != "") {
				doDialogue(nextSpeech, true, 1, 3);
				nextSpeech = "";
			}
			else {
				callLua(luaState, "get_speech", ">s");
				bool sp = !lua_isnil(luaState, -1);
				if (sp) {
					const char *speech = lua_tostring(luaState, -1);
					doDialogue(std::string(speech), true, 1, 3);
				}
				else {
					clear_input_events(); // HOOOO
					CombatEntity *ce;
					for (;;) {
						currentEntity++;
						ce = nth(entities, currentEntity);
						if (currentEntity >= (int)entities.size()) {
							currentEntity = -1;
							continue;
						}
						else if ((currentEntity >= 0) &&
								ce->isDead()) {
							continue;
						}
						else if (ce->getType() == COMBATENTITY_TYPE_FRILL) {
							continue;
						}
						break;
					}
					acting_entities.push_back(ce);
				}
				lua_pop(luaState, 1);
			}
		}
	}

	for (it = acting_entities.begin(); it != acting_entities.end();) {
		CombatEntity *ce = *it;
		if (ce->getType() == COMBATENTITY_TYPE_PLAYER || ce->getType() == COMBATENTITY_TYPE_ENEMY) {
			Combatant *c = (Combatant *)ce;
			if (c->getInfo().condition == CONDITION_PARALYZED || c->getInfo().condition == CONDITION_WEBBED) {
				c->getInfo().paralyzeCount--;
				if (c->getInfo().paralyzeCount <= 0) {
					c->getInfo().paralyzeCount = 0;
					c->getInfo().condition = CONDITION_NORMAL;
				}
				it = acting_entities.erase(it);
				continue;
			}
			else if (c->getInfo().condition == CONDITION_STONED || c->getInfo().condition == CONDITION_MUSHROOM || c->getInfo().condition == CONDITION_SWALLOWED) {
				it = acting_entities.erase(it);
				continue;
			}
			else if (c->getInfo().condition == CONDITION_SLOW) {
				if (c->getInfo().missed_extra == false) {
					c->getInfo().missed_extra = true;
					it = acting_entities.erase(it);
					continue;
				}
			}
		}
quick_label:

		if (ce->act(step, this)) {
			if (ce->getType() == COMBATENTITY_TYPE_PLAYER ||
				ce->getType() == COMBATENTITY_TYPE_ENEMY) {
				// Handle poison
				Combatant *c = (Combatant *)ce;
				CombatantInfo &info = c->getInfo();
				if (info.abilities.hp > 1 &&
					info.condition == CONDITION_POISONED) {
					int damage = info.abilities.maxhp * 0.05f;
					if (damage < 1)
						damage = 1;
					info.abilities.hp -= damage;
					if (info.abilities.hp <= 0) {
						info.abilities.hp = 1;
					}

					// add damage notifiction
					int x, y;
					getTextPos(c, &x, &y);
					char s[20];
					sprintf(s, "%d", damage);
					TemporaryText *tt = new TemporaryText(x, y, std::string(s),
						white);
					addEntity(tt);
				}
			}
			if (ce->getType() == COMBATENTITY_TYPE_PLAYER || ce->getType() == COMBATENTITY_TYPE_ENEMY) {
				Combatant *c = (Combatant *)ce;
				if (c->getInfo().condition == CONDITION_QUICK) {
					if (c->getInfo().missed_extra == false && !allDead(COMBATENTITY_TYPE_ENEMY)) {
						c->getInfo().missed_extra = true;
						goto quick_label;
					}
					else {
						c->getInfo().missed_extra = false;
					}
				}
				else if (c->getInfo().condition == CONDITION_SLOW) {
					c->getInfo().missed_extra = false;
				}
				else if (c->getInfo().condition == CONDITION_CHARMED || c->getInfo().condition == CONDITION_SHADOW) {
					c->getInfo().charmedCount--;
					if (c->getInfo().charmedCount <= 0) {
						c->getInfo().charmedCount = 0;
						c->getInfo().condition = CONDITION_NORMAL;
					}
				}
			}
			it = acting_entities.erase(it);
		}
		else {
			it++;
		}
	}

	entityLock = false;

	if (entitiesToAddLater.size() > 0) {
		if (deleteIts) {
			entities.erase(deleteItentities);
			zsorted_entities.erase(deleteItzsorted_entities);
			deleteIts = false;
		}
		for (int i = 0; i < (int)entitiesToAddLater.size(); i++) {
			addEntity(entitiesToAddLater[i], force_posesToAddLater[i], idsToAddLater[i], indicesToAddLater[i]);
		}
		entitiesToAddLater.clear();
		force_posesToAddLater.clear();
		idsToAddLater.clear();
		indicesToAddLater.clear();
	}
	
	// If has a bad condition, change animation
	for (it = entities.begin(); it != entities.end(); it++) {
		CombatEntity *e = *it;
		if (e->getType() == COMBATENTITY_TYPE_PLAYER) {
			CombatPlayer *p = (CombatPlayer *)e;
			if (p->getInfo().condition != CONDITION_NORMAL
					&& !p->isActing() && !running) {
				// FIXME FILL ME IN
				std::string animNames[] = {
					"poisoned",
					"paralyzed",
					"", // Stoned
					"", // slow
					"", // quick
					"", // charmed
					"", // mushroom
					"", //shadow
					"", // webbed
					"" // swallowed
				};
				if (animNames[p->getInfo().condition] != "") {
					p->getAnimationSet()->setSubAnimation(
						animNames[p->getInfo().condition]);
				}
			}
		}
	}

	return BATTLE_CONTINUE;
}


// Combatants with the same speed get randomized in start() (not anymore!)
void Battle::addEntity(CombatEntity *add, bool force_pos, int id, int index)
{
	if (id < 0) {
		add->setId(currId++);
	
		if (add->getType() == COMBATENTITY_TYPE_ENEMY) {
			CombatEnemy *e = (CombatEnemy *)add;
			e->initId(currId-1);
		}
	}

	if (entityLock) {
		entitiesToAddLater.push_back(add);
		force_posesToAddLater.push_back(force_pos);
		idsToAddLater.push_back(id);
		indicesToAddLater.push_back(index);
		return;
	}

	// What is this case for??
	if (entities.size() <= 0 || add->getType() == COMBATENTITY_TYPE_FRILL || index >= 0) {
		if (index >= 0) {
			std::list<CombatEntity *>::iterator it = entities.begin();
			for (int i = 0; i < index && it != entities.end(); i++) {
				it++;
			}
			entities.insert(it, add);
		}
		else {
			entities.push_back(add);
		}
	}
	else {
		Combatant *c = (Combatant *)add;
		CombatantInfo &cInfo = c->getInfo();
		std::list<CombatEntity *>::iterator it;
		if (preemptive) {
			 it = entities.end();
		}
		else {
			// find one with lower speed
			for (it = entities.begin(); it != entities.end(); it++) {
				Combatant *curr = (Combatant *)(*it);
				if (curr->getType() != COMBATENTITY_TYPE_ENEMY &&
					curr->getType() != COMBATENTITY_TYPE_PLAYER) {
					continue;
				}
				CombatantInfo &currInfo = curr->getInfo();
				if (currInfo.abilities.speed < cInfo.abilities.speed) {
					break;
				}
			}
		}
		// insert before the one with lower speed
		entities.insert(it, c);
		if (add->getType() == COMBATENTITY_TYPE_ENEMY) {
			enemyNames[c->getPrintableName()]++;
		}
		if (add->getType() == COMBATENTITY_TYPE_ENEMY && attackedFromBehind) {
			if (!force_pos) {
				c->setX(BW-c->getX());
			}
			c->setLocation(LOCATION_RIGHT);
			c->setDirection(DIRECTION_WEST);
		}
	}

	std::list<CombatEntity *>::iterator it = zsorted_entities.begin();

	while (it != zsorted_entities.end()) {
		CombatEntity *c = *it;
		if (c->getY() > add->getY())
			break;
		it++;
	}

	zsorted_entities.insert(it, add);

	if (index < 0) {
		if (add->getType() == COMBATENTITY_TYPE_ENEMY) {
			CombatEnemy *e = (CombatEnemy *)add;
			e->added();
			e->mkdeath();
		}
	}
}


// returns index not id
int Battle::getEntityIndex(int id)
{
	std::list<CombatEntity *>::iterator it;
	int i = 0;
	for (it = zsorted_entities.begin(); it != zsorted_entities.end(); it++) {
		CombatEntity *e = *it;
		if (e->getId() == id) {
			return i;
		}
		i++;
	}
	return -1;
}


// returns index not id
int Battle::getNextEntity(int id)
{
	std::list<CombatEntity *>::iterator it;
	unsigned int i;
	for (i = 0, it = zsorted_entities.begin(); it != zsorted_entities.end(); i++, it++) {
		CombatEntity *e = *it;
		if (e->getId() == id) {
			if (i == zsorted_entities.size()-1) {
				if (entitiesToAddLater.size() > 0) {
					return (*entitiesToAddLater.begin())->getId();
				}
				else {
					return (*zsorted_entities.begin())->getId();
				}
			}
			else {
				it++;
				return (*it)->getId();
			}
		}
	}

	for (unsigned int i = 0; i < entitiesToAddLater.size(); i++) {
		CombatEntity *e = entitiesToAddLater[i];
		if (e->getId() == id) {
			if (i == entitiesToAddLater.size()-1) {
				return (*zsorted_entities.begin())->getId();
			}
			else {
				return entitiesToAddLater[i+1]->getId();
			}
		}
	}

	return -1;
}


// takes indexes not ids
void Battle::moveEntity(int toMove, int where)
{
	CombatEntity *e = nth(zsorted_entities, toMove);
	
	zsorted_entities.erase(std::find(zsorted_entities.begin(), zsorted_entities.end(), e));

	std::list<CombatEntity *>::iterator it = zsorted_entities.begin();
	int i = 0;
	while (i < where)
		it++;
	zsorted_entities.insert(it, e);
}


void Battle::addMessage(MessageLocation loc, std::string msg, int lifetime)
{
	BattleMessage bm;
	bm.location = loc;
	bm.text = msg;
	bm.lifetime = lifetime;
	messages.push_back(bm);
}


void Battle::start(void)
{
#ifdef ALLEGRO_IPHONE
	vibrate();
#endif
	std::string musicname;

	if (name == "1Tode")
		musicname = "final_boss.ogg";
	else if (boss_fight) {
		musicname = "boss.ogg";
	}
	else {
		musicname = "battle.ogg";
	}

	if (!(name == "1Golem") && !manChooser) {
		playMusic(musicname, 0.0f);
		musicFadeCount = 0;
	}
	// Add players and adjust enemy positions
	if (attackedFromBehind) {
		// add players
		for (int i = 0; i < MAX_PARTY; i++) {
			if (!party[i])
				continue;
			CombatPlayer *c = (CombatPlayer *)party[i]->makeCombatant(i);
			c->setId(currId++);
			// reverse formation
			if (c->getFormation() == FORMATION_FRONT) {
				c->setFormation(FORMATION_BACK);
			}
			else {
				c->setFormation(FORMATION_FRONT);
			}
			if (c->getFormation() == FORMATION_FRONT) {
				c->setX(20+16);
			}
			else {
				c->setX(20);
			}
			c->setY(60 + (i*16));
			c->setDirection(DIRECTION_EAST);
			c->setLocation(LOCATION_LEFT);
			addEntity(c);
		}
	}
	else {
		// add players
		for (int i = 0; i < MAX_PARTY; i++) {
			if (!party[i])
				continue;
			CombatPlayer *c = (CombatPlayer *)party[i]->makeCombatant(i);
			c->setId(currId++);
			if (c->getFormation() == FORMATION_FRONT) {
				c->setX(BW-(16+20));
			}
			else {
				c->setX(BW-(20));
			}
			c->setY(60 + (i*16));
			c->setDirection(DIRECTION_WEST);
			c->setLocation(LOCATION_RIGHT);;
			addEntity(c);
		}
	}


	callLua(luaState, "start", ">");
	
	std::list<CombatEntity *>::iterator it;
	for (it = entities.begin(); it != entities.end(); it++) {
		CombatEntity *e = *it;
		if (e->getType() == COMBATENTITY_TYPE_ENEMY) {
			((CombatEnemy *)e)->start();
		}
	}

	// if preemptive, don't change order
	if (preemptive) {
		return;
	}
	// if attacked from behind, invert order
	else if (attackedFromBehind) {
		entities.reverse();
	}
}


std::list<CombatEntity *> &Battle::getEntities(void)
{
	return entities;
}


void Battle::initLua(void)
{
	luaState = lua_open();

	openLuaLibs(luaState);

	registerCFunctions(luaState);

	runGlobalScript(luaState);
	
	unsigned char *bytes;
	int file_size;

	debug_message("Loading global combat script...\n");
	bytes = slurp_file(getResource("combat_scripts/global.%s", getScriptExtension().c_str()), &file_size);
	if (!bytes) native_error("Load Error.", "combat_scripts/global.lua");
	if (luaL_loadbuffer(luaState, (char *)bytes, file_size, "chunk")) {
		dumpLuaStack(luaState);
		throw ReadError();
	}
	delete[] bytes;

	debug_message("Running global combat script...\n");
	if (lua_pcall(luaState, 0, 0, 0)) {
		dumpLuaStack(luaState);
		lua_close(luaState);
		throw ScriptError();
	}
	
	debug_message("Loading combat script...\n");
	bytes = slurp_file(getResource("combat_scripts/%s.%s", name.c_str(), getScriptExtension().c_str()), &file_size);
	if (!bytes) native_error("Load Error.", ((std::string("combat_scripts/") + name + ".lua").c_str()));
	if (luaL_loadbuffer(luaState, (char *)bytes, file_size, "chunk")) {
		dumpLuaStack(luaState);
		throw ReadError();
	}
	delete[] bytes;

	debug_message("Running combat script...\n");
	if (lua_pcall(luaState, 0, 0, 0)) {
		dumpLuaStack(luaState);
		lua_close(luaState);
		throw ScriptError();
	}

	lua_getglobal(luaState, "water");
	if (lua_isboolean(luaState, -1)) {
		inWater = (bool)lua_toboolean(luaState, -1);
	}
	lua_pop(luaState, 1);
}


void Battle::drawWhichStatus(bool enemy, bool player)
{
	draw_enemy_status = enemy;
	draw_player_status = player;
}

bool Battle::isInWater(void)
{
	return inWater;
}


Battle::Battle(std::string name, bool can_run) :
	name(name),
	currentEntity(-1),
	attackedFromBehind(false),
	preemptive(false),
        status(BATTLE_STATUS_SELECTING),
	playersAllDead(false),
	messageCount(0),
	enemiesAllDead(false),
	enemiesAllDeadStage(0),
	running(false),
	inWater(false),
	currId(0),
	shortcircuit(BATTLE_CONTINUE),
	nextEntity(-1),
	deleteIts(false),
	draw_enemy_status(true),
	draw_player_status(true),
	fade_regulator(9)
{
	if (area && !player_scripted) {
		save_memory(true);
	}

	dpad_off();

	this->can_run = can_run;

	entityLock = false;

	initLua();

	lua_getglobal(luaState, "num_enemies");
	numEnemies = (int)lua_tonumber(luaState, -1);
	lua_pop(luaState, 1);
	lua_getglobal(luaState, "boss_fight");
	if (lua_isnil(luaState, -1)) {
		boss_fight = false;
	}
	else {
		boss_fight = lua_toboolean(luaState, -1);
	}
	lua_pop(luaState, 1);
	lua_getglobal(luaState, "can_run");
	if (!lua_isnil(luaState, -1)) {
		this->can_run = lua_toboolean(luaState, -1);
	}
	lua_pop(luaState, 1);
	if (!boss_fight) {
		int r = (rand() % 40);

		if (r == 0) {
			attackedFromBehind = true;
		}
		else if (r == 1) {
			preemptive = true;
		}
	}

	// FIXME:
	//attackedFromBehind = true;
	//attackedFromBehind = rand() % 2;

	if (attackedFromBehind) {
		addMessage(MESSAGE_TOP, _t("The last shall be first!\n"), 2500);
	}
	else if (preemptive) {
		addMessage(MESSAGE_TOP, _t("Pre-emptive attack!\n"), 2500);
	}


	bg = m_load_bitmap(getResource("combat_bgs/%s.png", area->getTerrain().c_str()));
}

extern bool fairy_used;

Battle::~Battle(void)
{
	fairy_used = false;

	std::list<CombatEntity *>::iterator it;
	for (it = entities.begin(); it != entities.end(); it++) {
		delete *it;
	}

	entities.clear();

	zsorted_entities.clear();

	acting_entities.clear();

	enemyNames.clear();

	m_destroy_bitmap(bg);

	messages.clear();

	entitiesToAddLater.clear();
	force_posesToAddLater.clear();
	idsToAddLater.clear();
	indicesToAddLater.clear();

	lua_close(luaState);

	dpad_on();
	
	std::map<std::string, MSAMPLE>::iterator it2;
	for (it2 = preloaded_samples.begin(); it2 != preloaded_samples.end(); it2++) {
		MSAMPLE s = (MSAMPLE)it2->second;
		destroySample(s);
	}
	preloaded_samples.clear();
}


CombatPlayer *Battle::findPlayer(int number)
{
	std::list<CombatEntity *>::iterator it;
	for (it = entities.begin(); it != entities.end(); it++) {
		CombatEntity *e = *it;
		if (e->getType() == COMBATENTITY_TYPE_PLAYER) {
			CombatPlayer *p = (CombatPlayer *)e;
			if (p->getNumber() == number) {
				return p;
			}
		}
	}

	return NULL;
}


Combatant *findWeakestPlayer(void)
{
	if (!battle)
		return NULL;
	
	std::list<CombatEntity *> &v = battle->getEntities();

	int min = INT_MAX;
	Combatant *weakest = NULL;

	std::list<CombatEntity *>::iterator it;
	for (it = v.begin(); it != v.end(); it++) {
		CombatEntity *e = *it;
		if ((e->getType() == COMBATENTITY_TYPE_PLAYER)
			&& (!e->isDead())) {
				CombatPlayer *p = (CombatPlayer *)e;
				CombatantInfo &info = p->getInfo();
				int tot = 0;
				tot += info.abilities.hp * 3;
				tot += info.abilities.defense * 3;
				tot += armorDefense(ITEM_TYPE_HEAD_ARMOR, items[info.equipment.harmor].id);
				tot += armorDefense(ITEM_TYPE_CHEST_ARMOR, items[info.equipment.carmor].id);
				tot += armorDefense(ITEM_TYPE_FEET_ARMOR, items[info.equipment.farmor].id);
				if (tot < min) {
					min = tot;
					weakest = p;
				}
		}
	}

	return weakest;
}


Combatant *findRandomPlayer(void)
{
	if (!battle)
		return NULL;
	
	std::list<CombatEntity *> &v = battle->getEntities();
	std::list<CombatEntity *>::iterator it;

	int count = 0;

	for (it = v.begin(); it != v.end(); it++) {
		CombatEntity *e = *it;
		if ((e->getType() == COMBATENTITY_TYPE_PLAYER)
			&& (!e->isDead())) {
				Combatant *c = (Combatant *)e;
				if (!(c->getInfo().condition == CONDITION_SWALLOWED)) {
					count++;
				}
		}
	}

	if (count <= 0)
		return NULL;

	int r = rand() % count;

	for (it = v.begin(); it != v.end(); it++) {
		CombatEntity *e = *it;
		if ((e->getType() == COMBATENTITY_TYPE_PLAYER)
				&& (!e->isDead())) {
			Combatant *c = (Combatant *)e;
			if (!(c->getInfo().condition == CONDITION_SWALLOWED)) {
				if (r == 0) {
					return (Combatant *)e;
				}
				r--;
			}
		}
	}

	return NULL;
}

