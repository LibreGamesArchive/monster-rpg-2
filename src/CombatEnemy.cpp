#include "monster2.hpp"

void CombatEnemy::attacked(void)
{
	if (name == "Creep")
		swingTime = 1000;
}


lua_State *CombatEnemy::getLuaState(void)
{
	return luaState;
}


void CombatEnemy::initId(int id)
{
	lua_getglobal(luaState, "initId");
	if (lua_isfunction(luaState, -1)) {
		lua_pop(luaState, 1);
		callLua(luaState, "initId", "i>", id);
	}
	else
		lua_pop(luaState, 1);
}

void CombatEnemy::added(void)
{
	lua_getglobal(luaState, "added");
	if (lua_isfunction(luaState, -1)) {
		lua_pop(luaState, 1);
		callLua(luaState, "added", ">");
	}
	else
		lua_pop(luaState, 1);
}

bool CombatEnemy::spellHasNoEffect(std::string name)
{
	lua_getglobal(luaState, "spell_has_no_effect");
	if (lua_isfunction(luaState, -1)) {
		lua_pop(luaState, 1);
		callLua(luaState, "spell_has_no_effect", "s>b", name.c_str());
		bool noEffect = lua_toboolean(luaState, -1);
		lua_pop(luaState, 1);
		return noEffect;

	}

	lua_pop(luaState, 1);
	return false;
}

Element CombatEnemy::getStrength(void)
{
	return strength;
}

Element CombatEnemy::getWeakness(void)
{
	return weakness;
}

bool CombatEnemy::act(int step, Battle *b)
{
	lua_getglobal(luaState, "skip");
	bool _nil = lua_isnil(luaState, -1);
	if (!_nil) {
		bool skip = lua_toboolean(luaState, -1);
		lua_pop(luaState, 1);
		if (skip)
			return true;
	}
	else
		lua_pop(luaState, 1);

	if (status.type == COMBAT_WAITING) {
		swingTime = 0;
		status.type = COMBAT_DECIDING;
		thinkCount = 0;
		defending = false;
		if (name != "Relic" && name != "Lava") {
			if (name == "Rider") {
				animSet->setSubAnimation("stand");
				if (!use_programmable_pipeline) {
					whiteAnimSet->setSubAnimation("stand");
				}
			}
			else {
				animSet->setSubAnimation(0);
				if (!use_programmable_pipeline) {
					whiteAnimSet->setSubAnimation(0);
				}
			}
		}
	}

	if (status.type == COMBAT_DECIDING) {
		thinkCount += step;
		if (thinkCount < 1000) {
			return false;
		}

		callLua(luaState, "get_action", "ii>iiiiiiiiii", step, info.condition);
		CombatStatusType t = (CombatStatusType)((int)lua_tonumber(luaState, -10));
		if (t != COMBAT_BUSY)
			status.type = (CombatStatusType)((int)lua_tonumber(luaState, -10));
		switch (status.type) {
			case COMBAT_CASTING: {
				spellInited = false;
				const char *spellName = lua_tostring(luaState, -9);
				numTargets = (int)lua_tonumber(luaState, -8);
				std::vector<int> vt;
				std::list<CombatEntity *> &entities = b->getEntities();
				// if numTargets < 0, targets == all players
				if (numTargets < 0) {
					numTargets = 0;
					std::list<CombatEntity *>::iterator it;
					for (it = entities.begin(); it != entities.end(); it++) {
						CombatEntity *e = *it;
						if (e->getType() == COMBATENTITY_TYPE_PLAYER) {
							CombatPlayer *p = (CombatPlayer *)e;
							if (p->getInfo().abilities.hp > 0) {
								vt.push_back(p->getId());
								numTargets++;
							}
						}
					}
				}
				else {
					for (int i = 0; i < numTargets; i++) {
						int num = (int)lua_tonumber(luaState, -7+i);
						vt.push_back(num);
					}
				}
				targets = new Combatant *[numTargets];
				for (int i = 0; i < numTargets; i++) {
					int num = vt[i];
					std::list<CombatEntity *>::iterator it;
					for (it = entities.begin(); it != entities.end(); it++) {
						CombatEntity *e = *it;
						if (e->getId() == num) {
							targets[i] = (Combatant *)e;
							break;
						}
					}
				}
				vt.clear();
				spell = createSpell(std::string(spellName));
				if (printableName == std::string(_t("Girl"))) {
					if (std::string(spellName) == "Fireball") {
						animSet->setSubAnimation("cast1");
						if (!use_programmable_pipeline) {
							whiteAnimSet->setSubAnimation("cast1");
						}
					}
					else {
						animSet->setSubAnimation("cast2");
						if (!use_programmable_pipeline) {
							whiteAnimSet->setSubAnimation("cast2");
						}
					}
				}
				else if (printableName == std::string(_t("Dragon"))) {
					if (std::string(spellName) == "Talon") {
						animSet->setSubAnimation("talon");
						if (!use_programmable_pipeline) {
							whiteAnimSet->setSubAnimation("talon");
						}
					}
					else {
						animSet->setSubAnimation("bof");
						if (!use_programmable_pipeline) {
							whiteAnimSet->setSubAnimation("bof");
						}
					}
				}
				else if (printableName == std::string(_t("Tode"))) {
					if (std::string(spellName) == "BellyAcid") {
						animSet->setSubAnimation("acid");
						if (!use_programmable_pipeline) {
							whiteAnimSet->setSubAnimation("acid");
						}
					}
					else if (std::string(spellName) == "Swallow") {
						animSet->setSubAnimation("swallow");
						if (!use_programmable_pipeline) {
							whiteAnimSet->setSubAnimation("swallow");
						}
					}
					else if (std::string(spellName) == "Puke") {
						animSet->setSubAnimation("puke");
						if (!use_programmable_pipeline) {
							whiteAnimSet->setSubAnimation("puke");
						}
					}
				}
				else {
					animSet->setSubAnimation("cast");
					if (!use_programmable_pipeline) {
						whiteAnimSet->setSubAnimation("cast");
					}
				}
				// add message
				std::string msg = "{008}" + std::string(_t(spellName));
				MessageLocation loc;
				if (getLocation() == LOCATION_RIGHT)
					loc = MESSAGE_RIGHT;
				else
					loc = MESSAGE_LEFT;
				if (std::string(spellName) != "MachineGun")
					battle->addMessage(loc, msg, 1500);
				ALLEGRO_DEBUG("End of cast?");
				break;
			}
			case COMBAT_ATTACKING: {
				animSet->setSubAnimation("attack");
				if (!use_programmable_pipeline) {
					whiteAnimSet->setSubAnimation("attack");
				}
				animSet->reset();
				numTargets = (int)lua_tonumber(luaState, -9);
				std::list<CombatEntity *> &e = battle->getEntities();
				for (int i = 0; i < numTargets; i++) {
					int num = (int)lua_tonumber(luaState, -8+i);
					std::list<CombatEntity *>::iterator it;
					for (it = e.begin(); it != e.end(); it++) {
						CombatEntity *ent = *it;
						if (ent->getId() == num) {
							status.attacking.who.push_back((Combatant *)ent);
							break;
						}
					}
				}
				break;
			}
			case COMBAT_DEFENDING: {
				defending = true;
				status.type = COMBAT_WAITING;
				animSet->setSubAnimation("defend");
				if (!use_programmable_pipeline) {
					whiteAnimSet->setSubAnimation("defend");
				}
				animSet->reset();
				lua_pop(luaState, 10);
				return true;
			}
			case COMBAT_SKIP: {
				lua_pop(luaState, 10);
				status.type = COMBAT_WAITING;
				return true;
			}
			default:
				break;
		}
		lua_pop(luaState, 10);
		if (t == COMBAT_BUSY)
			return false;
	}

	if (status.type == COMBAT_CASTING) {
		bool animComplete = false;
		if (animSet->getCurrentAnimation()->hasTag("DoNotWaitForAnimationToFinish")) {
			animComplete = true;
		}
		else if (printableName == std::string(_t("Girl")) || printableName == std::string(_t("Dragon"))) {
			if (animSet->getCurrentAnimation()->isFinished())
				animComplete = true;
		}
		else {
			if (!animSet->checkSubAnimationExists("cast")
				|| animSet->getCurrentAnimation()->isFinished()) {
				animComplete = true;
			}
		}
		if (animComplete) {
			if (!spellInited) {
				spell->init(this, targets, numTargets);
				spellInited = true;
			}
			ALLEGRO_DEBUG("CALLING SPELL->UPDATE");
			if (spell->update(step)) {
				ALLEGRO_DEBUG("Spell->update true");
				animSet->reset();
				spell->apply();
				delete spell;
				spell = NULL;
				status.type = COMBAT_WAITING;
				if (name != "Relic" && name != "Lava") {
					animSet->setSubAnimation("stand");
					if (!use_programmable_pipeline) {
						whiteAnimSet->setSubAnimation("stand");
					}
				}
				return true;
			}
			ALLEGRO_DEBUG("Spell->update false");
		}
	}
	else if (status.type == COMBAT_ATTACKING) {
		if (!animSet->checkSubAnimationExists("attack") || animSet->getCurrentAnimation()->isFinished()) {
			for (int i = 0; i < (int)status.attacking.who.size(); i++) {
				doAttack(this, status.attacking.who[i]);
				// Some attacks are poisonous etc
				lua_getglobal(luaState, "get_attack_condition");
				if (lua_isfunction(luaState, -1)) {
					lua_pop(luaState, 1);
					callLua(luaState, "get_attack_condition",
						">i");
					CombatCondition cc =
						(CombatCondition)((int)lua_tonumber(luaState, -1));
					lua_pop(luaState, 1);
					if (cc != CONDITION_NORMAL) {
						Combatant *c = status.attacking.who[i];
						CombatantInfo &info = c->getInfo();
						info.condition = cc;
						MessageLocation loc;
						if (c->getLocation() == LOCATION_RIGHT) {
							loc = MESSAGE_RIGHT;
						}
						else {
							loc = MESSAGE_LEFT;
						}
						battle->addMessage(loc, getConditionName(cc),
							3000);
						if (cc == CONDITION_SHADOW) {
							info.charmedCount = 3;
						}
					}
				}
				else {
					lua_pop(luaState, 1);
				}
			}
			status.attacking.who.clear();
			status.type = COMBAT_WAITING;
			return true;
		}
	}

	ALLEGRO_DEBUG("Done act");
	return false;
}


void CombatEnemy::die(int attackerId)
{
	callLua(luaState, "die", "ii>", _id, attackerId);
}


bool CombatEnemy::update(int step)
{
	animSet->update(step);

	if (info.abilities.hp <= 0) {
		if (!sample_played) {
			if (explosionCircles)
				loadPlayDestroy("enemy_explosion.ogg");
			else
				playPreloadedSample("enemy_die.ogg");
			sample_played = true;
		}
		deadCount += step;
		if (deadCount >= DIE_TIME) {
			return true;
		}
	}

	if (info.condition == CONDITION_CHARMED)
		charmAnim->update(step);

	int w = getAnimationSet()->getWidth();
	int h = getAnimationSet()->getHeight();

	for (int i = 0; i < numExplosionCircles; i++) {
		explosionCircles[i].count += step;
		explosionCircles[i].color.a = 255 * (1 - (float)explosionCircles[i].count / explosionCircles[i].lifetime);
		if (explosionCircles[i].count > explosionCircles[i].lifetime) {
			explosionCircles[i].count = 0;
			explosionCircles[i].x = x + ((rand() % w) - (w/2));
			explosionCircles[i].y = y - rand() % h;
		}
	}

	if (swingTime > 0)
		swingTime -= step;

	return false;
}


static int dragon_x;
static int dragon_y;
static int dragon_flags;


static void dragon_blackAnd0(AnimationSet *a, AnimationSet *a2)
{
	char animName[100];
	sprintf(animName, "transform0");
	a->setSubAnimation(std::string(animName));
	if (!use_programmable_pipeline) {
		a2->setSubAnimation(std::string(animName));
	}

	long start = tguiCurrentTimeMillis();
	while (tguiCurrentTimeMillis() < (unsigned long)start+1000) {
		long elapsed = tguiCurrentTimeMillis() - start;
		int i = elapsed / 100;
		MCOLOR c;
		if (i % 2 == 1)
			c = black;
		else
			c = white;

		m_set_target_bitmap(buffer);

		m_clear(c);
		a->draw(dragon_x, dragon_y-a->getHeight(), dragon_flags);

		drawBufferToScreen();
		m_flip_display();
		
		m_rest(0.001);
	}
}

static void dragon_normal(AnimationSet *a, AnimationSet *a2, int frame)
{
	m_set_target_bitmap(buffer);
	char animName[100];
	sprintf(animName, "transform%d", frame);
	a->setSubAnimation(std::string(animName));
	if (!use_programmable_pipeline) {
		a2->setSubAnimation(std::string(animName));
	}
	
	m_clear(black);
	a->draw(dragon_x, dragon_y-a->getHeight(), dragon_flags);
	
	drawBufferToScreen();
	m_flip_display();

	long start = tguiCurrentTimeMillis();
	while (tguiCurrentTimeMillis() < (unsigned long)start+125) {
		m_rest(0.001);
	}
}

static void dragon_flash(AnimationSet *a, AnimationSet *a2, int frame, float startAlpha)
{
	char animName[100];
	sprintf(animName, "transform%d", frame);
	a->setSubAnimation(std::string(animName));
	if (!use_programmable_pipeline) {
		a2->setSubAnimation(std::string(animName));
	}
	
	long start = tguiCurrentTimeMillis();
	while (tguiCurrentTimeMillis() < (unsigned long)start+125) {
		long elapsed = tguiCurrentTimeMillis() - start;
		if (elapsed > 125) elapsed = 125;
		float alpha = 1.0 - ((1.0/6.0) * ((float)elapsed/125));
		alpha += startAlpha;

		m_set_target_bitmap(buffer);

		m_clear(m_map_rgba(255, 255, 255, 255*alpha));
		a->draw(dragon_x, dragon_y-a->getHeight(), dragon_flags);
		
		drawBufferToScreen();
		m_flip_display();
		
		m_rest(0.001);
	}
}

static void dragon_fade(AnimationSet *a, AnimationSet *a2, int fullframe, int fadeframe)
{
	char animName[100];
	sprintf(animName, "transform%d", fullframe);
	a->setSubAnimation(std::string(animName));
	if (!use_programmable_pipeline) {
		a2->setSubAnimation(std::string(animName));
	}
	MBITMAP *full = a->getCurrentAnimation()->getCurrentFrame()->getImage()->getBitmap();
	sprintf(animName, "transform%d", fadeframe);
	a->setSubAnimation(std::string(animName));
	if (!use_programmable_pipeline) {
		a2->setSubAnimation(std::string(animName));
	}
	MBITMAP *fade = a->getCurrentAnimation()->getCurrentFrame()->getImage()->getBitmap();
	
	long start = tguiCurrentTimeMillis();
	while (tguiCurrentTimeMillis() < (unsigned long)start+125) {
		long elapsed = tguiCurrentTimeMillis() - start;
		if (elapsed > 125) elapsed = 125;
		float alpha = 1.0 - (elapsed/125.0);

		m_set_target_bitmap(buffer);
		m_clear(black);

		m_save_blender();
		m_set_blender(M_ONE, M_INVERSE_ALPHA, m_map_rgba(255*alpha, 255*alpha, 255*alpha, 255*alpha));
		m_draw_bitmap(fade, dragon_x, dragon_y-a->getHeight(), dragon_flags);
		m_restore_blender();

		m_draw_bitmap(full, dragon_x, dragon_y-a->getHeight(), dragon_flags);
		
		drawBufferToScreen();
		m_flip_display();
		
		m_rest(0.001);
	}
}

static void dragon_players(AnimationSet *a, AnimationSet *a2)
{
	a->setSubAnimation("stand");
	if (!use_programmable_pipeline) {
		a2->setSubAnimation("stand");
	}

	CombatPlayer *players[MAX_PARTY] = { NULL };

	for (int i = 0; i < MAX_PARTY; i++) {
		players[i] = battle->findPlayer(i);
	}

	long start = tguiCurrentTimeMillis();
	while (tguiCurrentTimeMillis() < (unsigned long)start+2000) {
		m_set_target_bitmap(buffer);
		m_clear(black);
		a->draw(dragon_x, dragon_y-a->getHeight(), dragon_flags);
		int elapsed = (int)(tguiCurrentTimeMillis()-start);
		if (elapsed > 2000) elapsed = 2000;
		elapsed /= 100;
		if (elapsed % 2 == 1) {
			for (int i = 0; i < MAX_PARTY; i++) {
				if (players[i])
					players[i]->draw();
			}
		}

		drawBufferToScreen();
		m_flip_display();
		
		m_rest(0.001);
	}
}

void CombatEnemy::draw_shadow(void)
{
	if (name == "Relic" || name == "Lava") {
		if (info.abilities.hp < info.abilities.maxhp/2) {
			animSet->setSubAnimation("hurt");
			if (!use_programmable_pipeline) {
				whiteAnimSet->setSubAnimation("hurt");
			}
		}
		else {
			animSet->setSubAnimation("stand");
			if (!use_programmable_pipeline) {
				whiteAnimSet->setSubAnimation("stand");
			}
		}
	}

	if (ox == 0 && oy == 0 && name != "Creep") {
		MBITMAP *b = animSet->getCurrentAnimation()->getCurrentFrame()->getImage()->getBitmap();
		::draw_shadow(b, x, y-2, location == LOCATION_LEFT);
	}
}


void CombatEnemy::draw(void)
{
	if (name == "Relic" || name == "Lava") {
		if (info.abilities.hp < info.abilities.maxhp/2) {
			animSet->setSubAnimation("hurt");
			if (!use_programmable_pipeline) {
				whiteAnimSet->setSubAnimation("hurt");
			}
		}
		else {
			animSet->setSubAnimation("stand");
			if (!use_programmable_pipeline) {
				whiteAnimSet->setSubAnimation("stand");
			}
		}
	}

	// Allow enemy to draw stuff before they're drawn
	lua_getglobal(luaState, "pre_draw");
	if (!lua_isnil(luaState, -1)) {
		callLua(luaState, "pre_draw", ">");
	}
	lua_pop(luaState, 1);


	int flags;

	if (direction == DIRECTION_EAST) {
		flags = M_FLIP_HORIZONTAL;
	}
	else {
		flags = 0;
	}

	if (info.abilities.hp <= 0 && name == "Girl") {
		// Girl transforms to Dragon
		battle->addEnemyName(std::string(_t("Dragon")));
		battle->removeEnemyName(std::string(_t("Girl")));
		name = "Dragon";
		printableName = std::string(_t("Dragon"));
		AnimationSet *tmp = oldAnim;
		oldAnim = animSet;
		animSet = tmp;
		if (!use_programmable_pipeline) {
			tmp = oldWhiteAnim;
			oldWhiteAnim = whiteAnimSet;
			whiteAnimSet = tmp;
		}

		dragon_x = x-animSet->getWidth()/2;
		dragon_y = y;
		dragon_flags = flags;

		/* Transition into dragon from girl */
		dragon_blackAnd0(animSet, whiteAnimSet);
		dragon_normal(animSet, whiteAnimSet, 1);
		dragon_normal(animSet, whiteAnimSet, 2);
		dragon_normal(animSet, whiteAnimSet, 3);
		dragon_normal(animSet, whiteAnimSet, 4);
		dragon_normal(animSet, whiteAnimSet, 3);
		dragon_normal(animSet, whiteAnimSet, 2);
		dragon_normal(animSet, whiteAnimSet, 3);
		dragon_normal(animSet, whiteAnimSet, 4);
		dragon_normal(animSet, whiteAnimSet, 3);
		dragon_normal(animSet, whiteAnimSet, 2);
		dragon_normal(animSet, whiteAnimSet, 3);
		dragon_normal(animSet, whiteAnimSet, 4);
		dragon_normal(animSet, whiteAnimSet, 5);
		playPreloadedSample("low_cackle.ogg");
		dragon_flash(animSet, whiteAnimSet, 6, 1.0);
		dragon_flash(animSet, whiteAnimSet, 7, 0.8333);
		dragon_flash(animSet, whiteAnimSet, 8, 0.6666);
		dragon_flash(animSet, whiteAnimSet, 9, 0.5);
		dragon_flash(animSet, whiteAnimSet, 10, 0.3333);
		dragon_flash(animSet, whiteAnimSet, 11, 0.1666);
		dragon_fade(animSet, whiteAnimSet, 12, 11);
		dragon_fade(animSet, whiteAnimSet, 13, 12);
		dragon_fade(animSet, whiteAnimSet, 14, 13);
		dragon_fade(animSet, whiteAnimSet, 15, 14);
		dragon_normal(animSet, whiteAnimSet, 16);
		dragon_normal(animSet, whiteAnimSet, 17);
		dragon_normal(animSet, whiteAnimSet, 16);
		dragon_normal(animSet, whiteAnimSet, 17);
		dragon_normal(animSet, whiteAnimSet, 16);
		dragon_normal(animSet, whiteAnimSet, 17);
		dragon_normal(animSet, whiteAnimSet, 18);
		dragon_players(animSet, whiteAnimSet);

		animSet->setSubAnimation("stand");
		if (!use_programmable_pipeline) {
			whiteAnimSet->setSubAnimation("stand");
		}
		sample_played = false;
		deadCount = 0;
		// change stats
		info.abilities.hp = 10000;
		info.abilities.attack = 400;
		info.abilities.defense = 9999;
		info.abilities.speed = 70;
		info.abilities.mdefense = 300;
		info.abilities.luck = 100;
	}
	// Draw fading out purple waving
	else if (info.abilities.hp <= 0 && !explosionCircles) {
		int w = getAnimationSet()->getWidth();
		int h = getAnimationSet()->getHeight();
		int firstLinesTop = (int)((float)deadCount / DIE_TIME * (h / 2));
		int count = deadCount - (DIE_TIME/3);
		if (count < 0) count = 0;
		int secondLinesTop = (int)((float)count / (DIE_TIME/3*2) * (h / 2));
		// fixme modify this horizontal sine
		Animation *a = animSet->getCurrentAnimation();
		Frame *f = a->getCurrentFrame();
		Image *i = f->getImage();
		MBITMAP *bmp = i->getBitmap();
		float fl = ((float)deadCount / DIE_TIME) * (M_PI*2);
#if (defined A5_OGL || A5_D3D)
		if (use_programmable_pipeline) {
			al_set_shader(display, tinter);
			al_set_shader_float(tinter, "ratio", 1);
			al_set_shader_float(tinter, "r", 0.8);
			al_set_shader_float(tinter, "g", 0);
			al_set_shader_float(tinter, "b", 0.8);
			al_use_shader(tinter, true);
			int dx = (int)(x - (w/2));
			int dy = (int)(y - h);
			for (int i = 0; i < h; i++, dy++) {
				float f = fl + (((float)i/h) * (M_PI*2));
				int new_dx = (int)(dx + (sin(f)*6));
				if (i % 2) {
					if ((h - i) > firstLinesTop*2) {
						m_draw_bitmap_region(bmp,
							0, i, w, 1, new_dx, dy, flags);
					}
				}
				else {
					if ((h - i) > secondLinesTop*2) {
						m_draw_bitmap_region(bmp,
							0, i, w, 1, new_dx, dy, flags);
					}
				}
			}
			al_use_shader(tinter, false);
			al_set_shader(display, default_shader);
		}
		else {
			int dx = (int)(x - (w/2));
			int dy = (int)(y - h);
			for (int i = 0; i < h; i++, dy++) {
				float f = fl + (((float)i/h) * (M_PI*2));
				int dxo = sin(f)*6;
				if (i % 2) {
					if ((h - i) > firstLinesTop*2) {
						m_draw_bitmap_region(work,
							0, i, w, 1, dx+dxo, dy,
							0);
					}
				}
				else {
					if ((h - i) > secondLinesTop*2) {
						m_draw_bitmap_region(work,
							0, i, w, 1, dx+dxo, dy,
							0);
					}
				}
			}
		}
#endif
	}
	else {
		int w = animSet->getWidth();
		int h = animSet->getHeight();

		bool bright;
		bright = ((unsigned)tguiCurrentTimeMillis() % 500 < 250);
		if (thinkCount < 1000 && bright) {
			if (use_programmable_pipeline) {
				al_set_shader(display, brighten);
				al_set_shader_float(brighten, "brightness", 0.7);
				al_use_shader(brighten, true);
				animSet->draw(x+ox-(w/2), y+oy-h, flags);
				al_use_shader(brighten, false);
				al_set_shader(display, default_shader);
			}
			else {
				MBITMAP *bmp = whiteAnimSet->getCurrentAnimation()->getCurrentFrame()->getImage()->getBitmap();
				m_draw_bitmap(bmp, ox+x-(w/2), oy+y-h, flags);
			}
		}
		else {
			if (info.condition == CONDITION_PARALYZED && ((unsigned)tguiCurrentTimeMillis() % 200 < 100)) {
				if (use_programmable_pipeline) {
					al_set_shader(display, tinter);
					al_set_shader_float(tinter, "ratio", 1);
					al_set_shader_float(tinter, "r", 0.5);
					al_set_shader_float(tinter, "g", 0.5);
					al_set_shader_float(tinter, "b", 0.5);
					al_use_shader(tinter, true);
					if (angle == 0)
						animSet->draw(x-(w/2), y-h, flags);
					else
						animSet->drawRotated(x+ox, y+oy-h/2, angle, flags);
					al_use_shader(tinter, false);
					al_set_shader(display, default_shader);
				}
				else {
					m_save_blender();
					float  r, g, b;
					r = 0.5f;
					g = 0.5f;
					b = 0.5f;
					m_set_blender(ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA, al_map_rgb_f(r, g, b));
					if (angle == 0) {
						MBITMAP *bmp = animSet->getCurrentAnimation()->getCurrentFrame()->getImage()->getBitmap();
						m_draw_bitmap(bmp, x-(w/2), y-h, flags);
					}
					else
						animSet->drawRotated(x+ox, y+oy-h/2, angle, flags);
					m_restore_blender();
				}
			}
			else {
				if (name == "Creep" && swingTime > 0) {
					float max = M_PI/16;
					int t = 500 - (swingTime % 500);
					float angle;
					if (t < 125)
						angle = t/125.0f * -(max);
					else if (t < 250)
						angle = -(max) - ((t-125)/125.0f * -(max));
					else if (t < 375)
						angle = (t-250)/125.0f * (max);
					else
						angle = (max) - ((t-375)/125.0f * (max));
					MBITMAP *bmp = animSet->getCurrentAnimation()->getCurrentFrame()->getImage()->getBitmap();
					if (location == LOCATION_LEFT) {
						m_draw_rotated_bitmap(bmp, 13, 1, x-8.5, y-h, angle, M_FLIP_HORIZONTAL);
					}
					else {
						angle = -angle;
						m_draw_rotated_bitmap(bmp, 30, 1, x+8.5, y-h, angle, 0);
					}
				}
				else if (angle == 0)
					animSet->draw(x+ox-(w/2), y+oy-h, flags);
				else
					animSet->drawRotated(x+ox, y+oy-h/2, angle, flags);
				if (info.condition == CONDITION_QUICK || info.condition == CONDITION_SLOW
						|| info.condition == CONDITION_CHARMED) {
					int cx = location == LOCATION_LEFT ? x+((w/2))+8 : x-(w/2)-8;
					int clock_y = y+oy-(h*3)/4;
					if (clock_y < 10) clock_y = 10;
					if (info.condition == CONDITION_QUICK) {
						draw_clock(cx+ox, clock_y, 7, false);
					}
					else if (info.condition == CONDITION_SLOW) {
						draw_clock(cx+ox, clock_y, 7, true);
					}
					else if (info.condition == CONDITION_CHARMED) {
						charmAnim->draw(cx+ox-10, y+oy-(h*3)/4, 0);
					}
				}
			}
		}
	}

	if (explosionCircles && info.abilities.hp <= 0) {
		for (int i = 0; i < numExplosionCircles; i++) {
			int r = explosionCircles[i].radius * (float)explosionCircles[i].count / explosionCircles[i].lifetime;
			m_draw_circle(explosionCircles[i].x, explosionCircles[i].y,
				r, explosionCircles[i].color, M_FILLED);
		}
	}

	// Allow enemy to draw stuff after they're drawn
	lua_getglobal(luaState, "post_draw");
	if (!lua_isnil(luaState, -1)) {
		callLua(luaState, "post_draw", ">");
	}
	lua_pop(luaState, 1);

}

void CombatEnemy::initLua(void)
{
	luaState = lua_open();

	openLuaLibs(luaState);

	registerCFunctions(luaState);

	runGlobalScript(luaState);

	unsigned char *bytes;
	int file_size;

	debug_message("Loading global enemy script...\n");
	bytes = slurp_file(getResource("combat_enemies/global.%s", getScriptExtension().c_str()), &file_size);
	if (!bytes) native_error("Load Error.", "combat_enemies/global.lua");
	if (luaL_loadbuffer(luaState, (char *)bytes, file_size, "chunk")) {
		dumpLuaStack(luaState);
		throw ReadError();
	}
	delete[] bytes;

	debug_message("Running global enemy script...\n");
	if (lua_pcall(luaState, 0, 0, 0)) {
		dumpLuaStack(luaState);
		lua_close(luaState);
		throw ScriptError();
	}

	debug_message("Loading enemy script...\n");
	bytes = slurp_file(getResource("combat_enemies/%s.%s", name.c_str(), getScriptExtension().c_str()), &file_size);
	if (!bytes) native_error("Load Error.", ((std::string("combat_enemies/") + name + ".lua").c_str()));
	if (luaL_loadbuffer(luaState, (char *)bytes, file_size, "chunk")) {
		dumpLuaStack(luaState);
		throw ReadError();
	}
	delete[] bytes;

	debug_message("Running enemy script...\n");
	if (lua_pcall(luaState, 0, 0, 0)) {
		dumpLuaStack(luaState);
		lua_close(luaState);
		throw ScriptError();
	}
	
	explosionCircles = NULL;
	numExplosionCircles = 0;

	lua_getglobal(luaState, "explodes");
	if (lua_isboolean(luaState, -1)) {
		if (lua_toboolean(luaState, -1)) {
			int w = getAnimationSet()->getWidth();
			int h = getAnimationSet()->getHeight();
			numExplosionCircles = (w*h)/64;
			explosionCircles = new ExplosionCircle[numExplosionCircles];
			for (int i = 0; i < numExplosionCircles; i++) {
				explosionCircles[i].x = x + ((rand() % w) -  (w/2));
				explosionCircles[i].y = y - rand() % h;
				int n = rand() % 4;
				switch (n) {
					case 0:
						explosionCircles[i].color = m_map_rgb(230, 230, 230);
						break;
					case 1:
						explosionCircles[i].color = m_map_rgb(250, 40, 0);
						break;
					case 2:
						explosionCircles[i].color = m_map_rgb(200, 250, 0);
						break;
					case 3:
						explosionCircles[i].color = m_map_rgb(30, 30, 30);
						break;
				}
				explosionCircles[i].radius = 5 + rand() % 10;
				explosionCircles[i].count = 0;
				explosionCircles[i].lifetime = 250 + rand() % 400;
			}
		}
		lua_pop(luaState, 1);
	}
	else
		lua_pop(luaState, 1);

	float_height = getNumberFromScript(luaState, "float");
}

void CombatEnemy::start(void)
{
	callLua(luaState, "start", ">");
}


void CombatEnemy::construct(std::string name, int x, int y, bool alpha)
{
	spell = NULL;
	thinkCount = 9999;
	deadCount = 0;
	sample_played = false;

	this->x = x;
	this->y = y;

	type = COMBATENTITY_TYPE_ENEMY;

	initLua();

	direction = DIRECTION_EAST;
	location = LOCATION_LEFT;
	loyalty = LOYALTY_EVIL;

	info.abilities.hp = (int)getNumberFromScript(luaState, "hp");
	info.abilities.maxhp = info.abilities.hp;
	info.abilities.attack = (int)getNumberFromScript(luaState, "attack");
	info.abilities.defense = (int)getNumberFromScript(luaState, "defense");
	info.abilities.speed = (int)getNumberFromScript(luaState, "speed");
	info.abilities.mdefense = (int)getNumberFromScript(luaState, "mdefense");
	info.abilities.luck = (int)getNumberFromScript(luaState, "luck");

	strength = (Element)((int)getNumberFromScript(luaState, "strength"));
	weakness = (Element)((int)getNumberFromScript(luaState, "weakness"));

	info.equipment.lhand = -1;
	info.equipment.rhand = -1;
	info.equipment.harmor = -1;
	info.equipment.carmor = -1;
	info.equipment.farmor = -1;

	info.condition = CONDITION_NORMAL;

	status.type = COMBAT_WAITING;

	if (printableName == std::string(_t("Girl"))) {
		oldAnim = new AnimationSet(getResource("combat_media/Dragon.png"));
		if (!use_programmable_pipeline) {
			oldWhiteAnim = oldAnim->clone(CLONE_ENEMY);
			oldWhiteAnim->setSubAnimation("stand");
		}
	}
	else {
		oldAnim = NULL;
		if (!use_programmable_pipeline) {
			oldWhiteAnim = NULL;
		}
	}

	charmAnim = new AnimationSet(getResource("combat_media/Charm.png"));

	swingTime = 0;

	spellInited = false;

	work = NULL;

	if (!use_programmable_pipeline) {
		whiteAnimSet = animSet->clone(CLONE_ENEMY);
		whiteAnimSet->setSubAnimation("stand");

		mkdeath();
	}
}

class create_death_blit_data : public RecreateData
{
public:
	AnimationSet *as;
	CombatLocation location;
	~create_death_blit_data() {}
};

static void create_death_blit(MBITMAP *bitmap, RecreateData *data)
{
	create_death_blit_data *d = (create_death_blit_data *)data;

	AnimationSet *as = d->as;
	CombatLocation location = d->location;

	std::string old = as->getSubName();

	if (as->checkSubAnimationExists("hurt")) {
		as->setSubAnimation("hurt");
	}
	else {
		as->setSubAnimation("stand");
	}

	Animation *a = as->getCurrentAnimation();
	Frame *f = a->getCurrentFrame();
	Image *i = f->getImage();
	MBITMAP *bmp = i->getBitmap();
	int w = as->getWidth();
	int h = as->getHeight();

	m_push_target_bitmap();
	m_set_target_bitmap(bitmap);
	m_clear(al_map_rgba(0, 0, 0, 0));
	death_blit_region(bmp, 0, 0, w, h, 0, 0, al_map_rgb_f(0.8, 0, 0.8), location == LOCATION_RIGHT ? 0 : ALLEGRO_FLIP_HORIZONTAL);
	m_pop_target_bitmap();
	
	as->setSubAnimation(old);
}

void CombatEnemy::mkdeath(void)
{
	if (use_programmable_pipeline)
		return;

	if (work) {
		m_destroy_bitmap(work);
	}
	
	AnimationSet *as;
	if (printableName == std::string(_t("Girl"))) {
		as = oldAnim;
	}
	else {
		as = animSet;
	}

	int w = as->getWidth();
	int h = as->getHeight();

	create_death_blit_data *data = new create_death_blit_data;
	data->as = as;
	data->location = location;

	work = m_create_alpha_bitmap(w, h, create_death_blit, data, NULL, true); // check
}

CombatEnemy::CombatEnemy(std::string name, int x, int y, bool alpha) :
	Combatant(name, alpha)
{
	std::string s = name;
	std::string::size_type loc;
	loc = s.find("_", 0);
	s = s.substr(0, loc);

	//referenceBattleAnim(s, this);
	//animSet = findBattleAnim(s, this);
	animSet = new AnimationSet(getResource("combat_media/%s.png", s.c_str()));
	animSet->setSubAnimation("stand");

	construct(name, x, y, alpha);
}

CombatEnemy::CombatEnemy(std::string name, int x, int y) :
	Combatant(name, false)
{
	std::string s = name;
	std::string::size_type loc;
	loc = s.find("_", 0);
	s = s.substr(0, loc);

	//referenceBattleAnim(s, this);
	//animSet = findBattleAnim(s, this);
	animSet = new AnimationSet(getResource("combat_media/%s.png", s.c_str()));
	animSet->setSubAnimation("stand");

	construct(name, x, y, false);
}


CombatEnemy::~CombatEnemy(void)
{
	lua_close(luaState);
	if (spell)
		delete spell;
#ifndef LITE
	delete charmAnim;
#endif
	if (explosionCircles)
		delete[] explosionCircles;
	if (!use_programmable_pipeline) {
		if (work) {
			m_destroy_bitmap(work);
		}	
	}
	if (oldAnim) {
		delete oldAnim;
		if (!use_programmable_pipeline) {
			delete oldWhiteAnim;
		}
	}
	
	std::string s = name;
	std::string::size_type loc;
	loc = s.find("_", 0);
	s = s.substr(0, loc);

	//unreferenceBattleAnim(s, this);
	delete animSet;
	if (!use_programmable_pipeline) {
		delete whiteAnimSet;
	}
}

void CombatEnemyTode::draw_shadow(void)
{
	bool full = false;
	for (int i = 0; i < MAX_PARTY; i++) {
		if (swallowed[i]) {
			full = true;
			break;
		}
	}

	if (full && animSet->getSubName() == "stand") {
		animSet->setSubAnimation("full");
		if (!use_programmable_pipeline) {
			whiteAnimSet->setSubAnimation("full");
		}
	}

	if (ox == 0 && oy == 0) {
		MBITMAP *b = animSet->getCurrentAnimation()->getCurrentFrame()->getImage()->getBitmap();
		::draw_shadow(b, x, y-2, location == LOCATION_LEFT);
	}
}



void CombatEnemyTode::draw(void)
{
	int nswallowed = 0;
	for (int i = 0; i < MAX_PARTY; i++) {
		if (swallowed[i] != NULL)
			nswallowed++;
	}

	if (animSet->getSubName() == "stand" && nswallowed > 0) {
		animSet->setSubAnimation("full");
		if (!use_programmable_pipeline) {
			whiteAnimSet->setSubAnimation("full");
		}
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

	// Draw fading out purple waving
	if (info.abilities.hp <= 0) {
		int w = getAnimationSet()->getWidth();
		int h = getAnimationSet()->getHeight();
		int firstLinesTop = (int)((float)deadCount / DIE_TIME * (h / 2));
		int count = deadCount - (DIE_TIME/3);
		if (count < 0) count = 0;
		int secondLinesTop = (int)((float)count / (DIE_TIME/3*2) * (h / 2));
		// fixme modify this horizontal sine
		Animation *a = animSet->getCurrentAnimation();
		Frame *f = a->getCurrentFrame();
		Image *i = f->getImage();
		MBITMAP *bmp = i->getBitmap();
		float fl = ((float)deadCount / DIE_TIME) * (M_PI*2);
		if (use_programmable_pipeline) {
			al_set_shader(display, tinter);
			al_set_shader_float(tinter, "ratio", 1);
			al_set_shader_float(tinter, "r", 0.8);
			al_set_shader_float(tinter, "g", 0);
			al_set_shader_float(tinter, "b", 0.8);
			al_use_shader(tinter, true);
			int dx = (int)(x - w/2);
			int dy = (int)(y - h);
			for (int i = 0; i < h; i++, dy++) {
				float f = fl + (((float)i/h) * (M_PI*2));
				int new_dx = (int)(dx + (sin(f)*6));
				if (i % 2) {
					if ((h - i) > firstLinesTop*2) {
						m_draw_bitmap_region(bmp,
							0, i, w, 1, new_dx, dy, flags);
					}
				}
				else {
					if ((h - i) > secondLinesTop*2) {
						m_draw_bitmap_region(bmp,
							0, i, w, 1, new_dx, dy, flags);
					}
				}
			}
			al_use_shader(tinter, false);
			al_set_shader(display, default_shader);
		}
		else {
			{
			m_save_blender();
			float  r, g, b;
			r = 0.8f;
			g = 0.0f;
			b = 0.8f;
			m_set_blender(ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA, al_map_rgb_f(r, g, b));
			int dx = (int)(x - w/2);
			int dy = (int)(y - h);
			for (int i = 0; i < h; i++, dy++) {
				float f = fl + (((float)i/h) * (M_PI*2));
				int new_dx = (int)(dx + (sin(f)*6));
				if (i % 2) {
					if ((h - i) > firstLinesTop*2) {
						m_draw_bitmap_region(bmp,
							0, i, w, 1, new_dx, dy, flags);
					}
				}
				else {
					if ((h - i) > secondLinesTop*2) {
						m_draw_bitmap_region(bmp,
							0, i, w, 1, new_dx, dy, flags);
					}
				}
			}
			m_restore_blender();
			}
		}
	}
	else {
		bool bright;
		bright = ((unsigned)tguiCurrentTimeMillis() % 500 < 250);
		if (thinkCount < 1000 && bright) {
			if (use_programmable_pipeline) {
				al_set_shader(display, brighten);
				al_set_shader_float(brighten, "brightness", 0.7);
				al_use_shader(brighten, true);
				animSet->draw(x+ox-(w/2), y+oy-h, flags);
				al_use_shader(brighten, false);
				al_set_shader(display, default_shader);
			}
			else {
				MBITMAP *bmp = whiteAnimSet->getCurrentAnimation()->getCurrentFrame()->getImage()->getBitmap();
				m_draw_bitmap(bmp, ox+x-(w/2), oy+y-h, flags);
			}
		}

		else {
			int w = animSet->getWidth();
			int h = animSet->getHeight();
			MBITMAP *bitmap = animSet->getCurrentAnimation()->getCurrentFrame()->getImage()->getBitmap();
			m_draw_alpha_bitmap(bitmap, x-w/2, y-h, flags);
			if (animSet->getSubName() == "full") {
				for (int i = 0; i < 4; i++) {
					if (swallowed[i]) {
						int bw = m_get_bitmap_width(swallowed[i]);
						int bh = m_get_bitmap_height(swallowed[i]);

						m_draw_rotated_bitmap(swallowed[i],
							bw/2, bh/2, x+5, y-h/2+10, angles[i], 0);
					}
				}
			}
			m_draw_alpha_bitmap(bitmap, x-w/2, y-h, flags);
		}
	}
}


void CombatEnemyTode::puke()
{
	for (int i = 0; i < 4; i++) {
		if (swallowed[i]) {
			m_destroy_bitmap(swallowed[i]);
			swallowed[i] = NULL;
		}
	}
}


void CombatEnemyTode::swallow(MBITMAP *b, float angle)
{
	for (int i = 0; i < 4; i++) {
		if (!swallowed[i]) {
			swallowed[i] = b;
			angles[i] = angle;
			break;
		}
	}
}

void CombatEnemyTode::puke_next(void)
{
	pukenext = true;
}

bool CombatEnemyTode::act(int step, Battle *b)
{
	static Spell *spell = NULL;

	if (pukenext) {
		spell = createSpell(std::string("Puke"));
		spell->init(this, NULL, 0);
		battle->addMessage(MESSAGE_LEFT, "{008} Puke", 1500);
		pukenext = false;
		animSet->setSubAnimation("puke");
		if (!use_programmable_pipeline) {
			whiteAnimSet->setSubAnimation("puke");
		}
	}

	if (spell) {
		if (spell->update(step)) {
			animSet->reset();
			spell->apply();
			delete spell;
			spell = NULL;
			status.type = COMBAT_WAITING;
			animSet->setSubAnimation("hurt");
			if (!use_programmable_pipeline) {
				whiteAnimSet->setSubAnimation("hurt");
			}
			if (animSet->getSubName() != "hurt") {
				animSet->setSubAnimation("stand");
				if (!use_programmable_pipeline) {
					whiteAnimSet->setSubAnimation("stand");
				}
			}
			return true;
		}
		return false;
	}

	return CombatEnemy::act(step, b);
}

CombatEnemyTode::CombatEnemyTode(std::string name, int x, int y) :
	CombatEnemy(name, x, y, true)
{
	for (int i = 0; i < 4; i++)
		swallowed[i] = NULL;
	pukenext = false;
}

CombatEnemyTode::~CombatEnemyTode()
{
	for (int i = 0; i < 4; i++) {
		if (swallowed[i])
			m_destroy_bitmap(swallowed[i]);
	}
}
