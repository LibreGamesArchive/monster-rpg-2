#include "monster2.hpp"


// Get position of "damage" text and stuff
void getTextPos(Combatant *attacked, int *x, int *y)
{
	*x = (int)attacked->getX();
	int extra = attacked->getAnimationSet()->getWidth()/2;
	if (attacked->getLocation() == LOCATION_LEFT)
		*x += extra;
	else
		*x -= extra;
	*y = (int)attacked->getY();
	*y -= attacked->getAnimationSet()->getHeight()/3*2;
}


void getCenter(Combatant *attacked, int *x, int *y)
{
	*x = (int)attacked->getX();
	*y = (int)(attacked->getY() - attacked->getAnimationSet()->getHeight()/2);
}


void doAttack(Combatant *attacker, Combatant *attacked, bool swoosh)
{
	CombatantInfo &attackerInfo = attacker->getInfo();
	CombatantInfo &attackedInfo = attacked->getInfo();


// determine where the attack hit
	int armor = -1;
	ItemType armorType = ITEM_TYPE_CHEST_ARMOR;
	if (attacked->getType() == COMBATENTITY_TYPE_PLAYER) {
		int r = rand() % 10;
		if (r < 5) {
         armor = attackedInfo.equipment.carmor < 0 ? -1 : items[attackedInfo.equipment.carmor].id;
			armorType = ITEM_TYPE_CHEST_ARMOR;
		}
		else if (r < 8) {
         armor = attackedInfo.equipment.harmor < 0 ? -1 : items[attackedInfo.equipment.harmor].id;
			armorType = ITEM_TYPE_HEAD_ARMOR;
		}
		else {
         armor = attackedInfo.equipment.farmor < 0 ? -1 : items[attackedInfo.equipment.farmor].id;
			armorType = ITEM_TYPE_FEET_ARMOR;
		}
	}

	int lhand = attackerInfo.equipment.lhand;
	int rhand = attackerInfo.equipment.rhand;

	int weapon_damage = 
      (lhand < 0 ? 10 : weaponDamage(items[lhand].id)) +
      (rhand < 0 ? 10 : weaponDamage(items[rhand].id));

	int damage = (attackerInfo.abilities.attack + weapon_damage);

	if (superpower) {
		CombatPlayer *p = dynamic_cast<CombatPlayer *>(attacker);
		if (p) {
			damage = 9999;
		}
		p = dynamic_cast<CombatPlayer *>(attacked);
		if (p) {
			damage = 1;
		}
	}

	/* Apply difficulty setting */
	if (config.getDifficulty() == CFG_DIFFICULTY_EASY) {
		if (attacker->getType() == COMBATENTITY_TYPE_PLAYER || attacker->getName() == "Minion")
			damage *= 1.15;
		else
			damage -= damage * 0.15;
	}
	else if (config.getDifficulty() == CFG_DIFFICULTY_HARD) {
		if (attacker->getType() == COMBATENTITY_TYPE_PLAYER)
			damage -= damage * 0.15;
		else
			damage *= 1.15;
	}

	int defense = attackedInfo.abilities.defense +
		armorDefense(armorType, armor);

	damage -= defense;

	// deplete ammo
	if ((lhand >= 0) && weapons[items[lhand].id].ammo) {
		attackerInfo.equipment.lquantity--;
		if (attackerInfo.equipment.lquantity <= 0) {
			attackerInfo.equipment.lquantity = 0;
			attackerInfo.equipment.lhand = -1;
			attacker->getAnimationSet()->setPrefix("noweapon_");
			if (!use_programmable_pipeline) {
				attacker->getWhiteAnimationSet()->setPrefix("noweapon_");
			}
		}
	}
	else if ((rhand >= 0) && weapons[items[rhand].id].ammo) {
		attackerInfo.equipment.rquantity--;
		if (attackerInfo.equipment.rquantity <= 0) {
			attackerInfo.equipment.rquantity = 0;
			attackerInfo.equipment.rhand = -1;
			attacker->getAnimationSet()->setPrefix("noweapon_");
			if (!use_programmable_pipeline) {
				attacker->getWhiteAnimationSet()->setPrefix("noweapon_");
			}
		}
	}


	if (lhand < 0 || rhand < 0) {
		// Players in the back row get hit less by attacks
		if (attacked->getType() == COMBATENTITY_TYPE_PLAYER ||
				attacker->getType() == COMBATENTITY_TYPE_PLAYER) {
			CombatPlayer *p;
			if (attacked->getType() == COMBATENTITY_TYPE_PLAYER) {
				p = (CombatPlayer *)attacked;
			}
			else {
				p = (CombatPlayer *)attacker;
			}
			if (p->getFormation() == FORMATION_BACK) {
				int reduction = (int)(damage * 0.2f);
				damage = damage - reduction;
			}
		}
	}


	// take luck into account
	int difference = attackerInfo.abilities.luck - attackedInfo.abilities.luck;
	if (difference > 0) {
		// handle critical hit
		if (difference > 20)
			difference = 20;
		int r = rand() % 100;
		bool critical = (r < difference) ? true : false;
		if (critical) {
			damage *= 2;
			battle->addMessage(MESSAGE_TOP, _t("Critical hit!"), 1500);
		}

	}
	else if (difference < 0) {
		// handle dodge
		difference = -difference;
		if (difference > 20)
			difference = 20;
		int r = rand() % 100;
		bool dodge = (r < difference) ? true : false;
		if (dodge) {
			damage /= 2;
			battle->addMessage(MESSAGE_TOP, _t("Blocked!"), 1500);
		}
	}
	
	if (lhand >= 0 && weapons[items[lhand].id].element != ELEMENT_NONE) {
		damage = applyStrengthsAndWeaknesses((CombatEnemy *)attacked,
			damage,
			weapons[items[lhand].id].element);
	}
	else 
	if (lhand >= 0 && weapons[items[rhand].id].element != ELEMENT_NONE) {
		damage = applyStrengthsAndWeaknesses((CombatEnemy *)attacked,
			damage,
			weapons[items[attackerInfo.equipment.rhand].id].element);
	}

	if (attacked->isDefending()) {
		if (attacked->getType() == COMBATENTITY_TYPE_PLAYER)
			attacked->setDefending(false);
		damage /= 2;
	}

	if (damage <= 0)
		damage = 1;
	
	if (attacked->getType() == COMBATENTITY_TYPE_ENEMY) {
		CombatEnemy *e = (CombatEnemy *)attacked;
		lua_State *luaState = e->getLuaState();
		lua_getglobal(luaState, "getDamage");
		if (lua_isfunction(luaState, -1)) {
			lua_pop(luaState, 1);
			callLua(luaState, "getDamage", "si>i", attacker->getName().c_str(), damage);
			damage = (int)lua_tonumber(luaState, -1);
			lua_pop(luaState, 1);
		}
		else
			lua_pop(luaState, 1);
	}
	

	attackedInfo.abilities.hp -= damage;

	if (attackedInfo.abilities.hp <= 0 &&
			attacked->getType() == COMBATENTITY_TYPE_ENEMY) {
		((CombatEnemy *)attacked)->die(attacker->getId());
	}

	int x;
	int y;

	getTextPos(attacked, &x, &y);

	char s[20];

	sprintf(s, "%d", damage);

	TemporaryText *tt = new TemporaryText(x, y, std::string(s),
		white);

	battle->addEntity(tt);

	getCenter(attacked, &x, &y);
	int dir = attacked->getLocation() == LOCATION_LEFT ?
		DIRECTION_WEST : DIRECTION_EAST;
	if (swoosh) {
		AttackSwoosh *as = new AttackSwoosh(x, y, dir, attacked, attacker);
		battle->addEntity(as);
	}

	playPreloadedSample("hit.ogg");

	if (attacked->getType() == COMBATENTITY_TYPE_ENEMY) {
		CombatEnemy *e = (CombatEnemy *)attacked;
		if (e->getInfo().abilities.hp > 0) {
			lua_State *L = e->getLuaState();
			lua_getglobal(L, "retaliate");
			/* for angry enemies */
			if (lua_isfunction(L, -1)) {
				lua_pop(L, 1);
				callLua(L, "retaliate", "i>", attacker->getId());
				std::list<CombatEntity *> &v = battle->getEntities();
				std::list<CombatEntity *>::iterator it;
				int i;
				for (it = v.begin(), i = 0; it != v.end(); it++, i++) {
					if (*it == attacked) {
						battle->setNextEntity(i);
						break;
					}
				}
			}
			else
				lua_pop(L, 1);
		}
		/* For prickly enemies */
		if (attacker->getType() == COMBATENTITY_TYPE_PLAYER && attacked->getName() == "Thornster") {
			int dmg;

			dmg = 65;

			int x;
			int y;

			getTextPos(attacker, &x, &y);

			char s[20];

			sprintf(s, "%d", dmg);

			TemporaryText *tt = new TemporaryText(x, y, std::string(s),
				white);

			battle->addEntity(tt);

			playPreloadedSample("hit.ogg");
			
			attacker->getInfo().abilities.hp -= dmg;
			if (attacker->getInfo().abilities.hp < 0)
				attacker->getInfo().abilities.hp = 0;
		}
	}

	attacked->attacked();
}
