#include "monster2.hpp"

Player *party[MAX_PARTY] = {
	NULL,
};

int heroSpot = 0;


CombatantInfo guardStartInfo = {
	{
		200,	// hp
		200,	// maxhp
		50,	// attack
		55,	// defense
		10,	// speed
		0,	// mp
		0,	// maxmp
		0,	// mdefense
		15	// luck
	},
	{
		-1,     // lhand
		23,	// rhand
		28,	// harmor
		26,	// carmor
		-1,	// farmor
		0,      // lquantity
		1,      // rquantity
	},
	{
		"<nil>",	// spells
	},
	0,
	CLASS_WARRIOR,
	CONDITION_NORMAL
};

CombatantInfo enyStartInfo = {
	{
		200,	// hp
		200,	// maxhp
		30,	// attack
		15,	// defense
		20,	// speed
		0,	// mp
		0,	// maxmp
		10,	// mdefense
		5	// luck
	},
	{
		-1,     // lhand
		-1,	// rhand
		-1,	// harmor
		-1,	// carmor
		-1,	// farmor
		0,      // lquantity
		0,      // rquantity
	},
	{
		"<nil>",	// spells
	},
	0,
	CLASS_WARRIOR|CLASS_ENY,
	CONDITION_NORMAL
};


CombatantInfo riderStartInfo = {
	{
		150,	// hp
		150,	// maxhp
		40,	// attack
		25,	// defense
		25,	// speed
		20,	// mp
		20,	// maxmp
		20,	// mdefense
		6	// luck
	},
	{
		-1,     // lhand
		5,	// rhand
		-1,	// harmor
		-1,	// carmor
		-1,	// farmor
		0,      // lquantity
		1,      // rquantity
	},
	{
		"Bolt1", "Fire1", "Ice1", "<nil>",	// spells
	},
	500,
	CLASS_MAGE,
	CONDITION_NORMAL
};


CombatantInfo riosStartInfo = {
	{
		120,	// hp
		120,	// maxhp
		50,	// attack
		20,	// defense
		20,	// speed
		30,	// mp
		30,	// maxmp
		30,	// mdefense
		12	// luck
	},
	{
		-1,     // lhand
		5,	// rhand
		-1,	// harmor
		-1,	// carmor
		-1,	// farmor
		0,      // lquantity
		1,      // rquantity
	},
	{
		"Cure1", "Heal", "<nil>",	// spells
	},
	500,
	CLASS_CLERIC,
	CONDITION_NORMAL
};


CombatantInfo gunnarStartInfo = {
	{
		200,	// hp
		200,	// maxhp
		50,	// attack
		20,	// defense
		30,	// speed
		0,	// mp
		0,	// maxmp
		10,	// mdefense
		12	// luck
	},
	{
		17,     // lhand
		16,	// rhand
		-1,	// harmor
		-1,	// carmor
		-1,	// farmor
		20,      // lquantity
		1,      // rquantity
	},
	{
		"<nil>",	// spells
	},
	1000,
	CLASS_STEAMPUNK,
	CONDITION_NORMAL
};


CombatantInfo faelonStartInfo = {
	{
		300,	// hp
		300,	// maxhp
		100,	// attack
		100,	// defense
		35,	// speed
		40,	// mp
		40,	// maxmp
		40,	// mdefense
		20	// luck
	},
	{
		-1,     // lhand
		29,	// rhand
		30,	// harmor
		26,	// carmor
		31,	// farmor
		0,      // lquantity
		1,      // rquantity
	},
	{
		"Heal", "Cure2", "Revive", "<nil>",	// spells
	},
	10000,
	CLASS_WARRIOR|CLASS_CLERIC,
	CONDITION_NORMAL
};


CombatantInfo melStartInfo = {
	{
		200,	// hp
		200,	// maxhp
		50,	// attack
		50,	// defense
		35,	// speed
		100,	// mp
		100,	// maxmp
		50,	// mdefense
		20	// luck
	},
	{
		-1,     // lhand
		32,	// rhand
		34,	// harmor
		33,	// carmor
		13,	// farmor
		0,      // lquantity
		1,      // rquantity
	},
	{
		"Cure1", "Cure2", "Cure3", "Heal",	// spells
		"Revive", "Stun", "Slow", "Quick", "Charm", "<nil>",
	},
	8000,
	CLASS_MAGE|CLASS_CLERIC,
	CONDITION_NORMAL
};


CombatantInfo tiggyStartInfo = {
	{
		350,	// hp
		350,	// maxhp
		100,	// attack
		120,	// defense
		40,	// speed
		40,	// mp
		40,	// maxmp
		60,	// mdefense
		25	// luck
	},
	{
		-1,     // lhand
		29,	// rhand
		30,	// harmor
		26,	// carmor
		31,	// farmor
		0,      // lquantity
		1,      // rquantity
	},
	{
		"Bolt3", "Bolt2", "Bolt1",	// spells
		"Fire3", "Fire2", "Fire1",
		"Ice3", "Ice2", "Ice1", "<nil>",
	},
	11000,
	CLASS_WARRIOR|CLASS_MAGE,
	CONDITION_NORMAL
};


CombatantInfo tipperStartInfo = {
	{
		400,	// hp
		400,	// maxhp
		110,	// attack
		135,	// defense
		80,	// speed
		120,	// mp
		120,	// maxmp
		100,	// mdefense
		80	// luck
	},
	{
		-1,     // lhand
		60,	// rhand
		62,	// harmor
		61,	// carmor
		-1,	// farmor
		0,      // lquantity
		1,      // rquantity
	},
	{
		"Cure3", "Revive", "Heal",
		"Bolt3", "Fire3", "Ice3", "<nil>",
	},
	35000,
	CLASS_MAGE|CLASS_CLERIC,
	CONDITION_NORMAL
};



int gold = 0;


static int findSpotForPlayer(void)
{
	for (int i = 0; i < MAX_PARTY; i++) {
		if (party[i] == NULL) {
			return i;
		}
	}

	return -1;
}


MBITMAP *Player::getIcon(void)
{
	return icon;
}


Object *Player::getObject(void)
{
	return object;
}


CombatantInfo &Player::getInfo(void)
{
	return info;
}


CombatFormation Player::getFormation(void)
{
	return formation;
}


std::string Player::getName(void)
{
	return name;
}


void Player::setObject(Object *o)
{
	object = o;
}


void Player::setInfo(CombatantInfo &i)
{
	copyInfo(info, i);
}


void Player::setFormation(CombatFormation f)
{
		formation = f;
}


void Player::setName(std::string name)
{
	this->name = name;
}


Combatant *Player::makeCombatant(int number)
{
	//AnimationSet *banim = findBattleAnim(name, this);
	CombatPlayer *c = new CombatPlayer(name, number/*, banim*/);

	copyInfo(c->getInfo(), info);

	c->setLoyalty(LOYALTY_GOOD);
	c->setFormation(formation);

	int lid = info.equipment.lhand;
	int rid = info.equipment.rhand;
		
	c->getAnimationSet()->setPrefix("");
	if (!use_programmable_pipeline) {
		c->getWhiteAnimationSet()->setPrefix("");
	}

	if ((lid < 0) && (rid < 0)) {
		c->getAnimationSet()->setPrefix("noweapon_");
		if (!use_programmable_pipeline) {
			c->getWhiteAnimationSet()->setPrefix("noweapon_");
		}
	}
	else if ((lid >= 0 && weapons[items[lid].id].needs >= 0) ||
		(rid >= 0 && weapons[items[rid].id].needs >= 0)) {
		if ((lid >= 0) && !weapons[items[lid].id].ammo) {
			if (rid < 0) {
				c->getAnimationSet()->setPrefix("noweapon_");
				if (!use_programmable_pipeline) {
					c->getWhiteAnimationSet()->setPrefix("noweapon_");
				}
			}
		}
		else if ((rid >= 0) && !weapons[items[rid].id].ammo) {
			if (lid < 0) {
				c->getAnimationSet()->setPrefix("noweapon_");
				if (!use_programmable_pipeline) {
					c->getWhiteAnimationSet()->setPrefix("noweapon_");
				}
			}
		}
	}

	return c;
}


Player::Player(std::string name, bool putInParty) :
	name(name)
{
	object = NULL;

	if (putInParty) {
		int i = findSpotForPlayer();
		party[i] = this;
	}

	icon = m_load_bitmap(getResource("media/%s_profile.png", name.c_str()));

	formation = FORMATION_FRONT;

	//referenceBattleAnim(name, this);
}


Player::~Player(void)
{
	m_destroy_bitmap(icon);
	//unreferenceBattleAnim(name, this);
}



int getLevel(int experience)
{
	int level = 0;
	int dec = EXP_INFLATION;
	
	while (experience >= dec) {
		level++;
		experience -= dec;
		dec += EXP_INFLATION;
	}

	if (level > 99)
		level = 99;

	return level;
}


int getExperience(int level)
{
	int total = 0;
	int inc = EXP_INFLATION;

	for (int i = 0; i < level && i < 100; i++) {
		total += inc;
		inc += EXP_INFLATION;
	}

	return total;
}


static std::vector<std::string> getLearnedSpells(std::string who, int newLevel)
{
	std::vector<std::string> spells;

	// FIXME:!
	//spells.push_back("Fire2");
	//return spells;

	if (who == "Rider") {
		if (newLevel == 3) {
			spells.push_back("Fire2");
			spells.push_back("Ice2");
			spells.push_back("Bolt2");
		}
	}
	else if (who == "Rios") {
		if (newLevel == 2) {
			spells.push_back("Cure2");
		}
	}
	else if (who == "Mel") {
		if (newLevel == 9) {
			spells.push_back("Vampire");
		}
	}

	return spells;
}


static std::string ability_labels[] = {
	"<unused>",
	"MaxHP:",
	"Attack:",
	"Defense:",
	"Speed:",
	"<unused>",
	"MaxMP:",
	"M.Def.:",
	"Luck:"
};


/* This used to be a callback, but it didn't work */

struct LEVEL_UP_CALLBACK_DATA {
	Player *player;
	int *points;
	CombatantInfo *origInfo;
};


static void levelUpCallback(int selected, LEVEL_UP_CALLBACK_DATA *d)
{
	dpad_off();

	const int width = 200;
	const int height = 50;
	int x = (BW-width)/2;
	int y = (BH-height)/2;
	char charBuffer[100];

	CombatantInfo &info =
		d->player->getInfo();
	
	bool useMP = (info.characterClass & CLASS_MAGE) || (info.characterClass & CLASS_CLERIC);

	int **values, **orig_values;
	int *v1[] = {
		&info.abilities.maxhp,
		&info.abilities.attack,
		&info.abilities.defense,
		&info.abilities.speed,
		&info.abilities.maxmp,
		&info.abilities.mdefense,
		&info.abilities.luck
	};
	int *v2[] = {
		&info.abilities.maxhp,
		&info.abilities.attack,
		&info.abilities.defense,
		&info.abilities.speed,
		&info.abilities.mdefense,
		&info.abilities.luck
	};
	int *ov1[] = {
		&d->origInfo->abilities.maxhp,
		&d->origInfo->abilities.attack,
		&d->origInfo->abilities.defense,
		&d->origInfo->abilities.speed,
		&d->origInfo->abilities.maxmp,
		&d->origInfo->abilities.mdefense,
		&d->origInfo->abilities.luck
	};
	int *ov2[] = {
		&d->origInfo->abilities.maxhp,
		&d->origInfo->abilities.attack,
		&d->origInfo->abilities.defense,
		&d->origInfo->abilities.speed,
		&d->origInfo->abilities.mdefense,
		&d->origInfo->abilities.luck
	};
	if (useMP) {
		values = v1;
		orig_values = ov1;
	}
	else {
		values = v2;
		orig_values = ov2;
	}

	int dx, dy, dw, dh;
	get_screen_offset_size(&dx, &dy, &dw, &dh);
	m_draw_scaled_backbuffer(dx, dy, dw, dh, 0, 0, dw, dh, tmpbuffer);

	MFrame_NormalDraw *frame = new MFrame_NormalDraw(x, y, width, height, true);

	std::vector<std::vector<MTableData> > tableData;
	std::vector<MTableData> column;
	MTableData cell;

	cell.text = _t("Stat");
	cell.justify = JUSTIFY_LEFT;
	cell.color = grey;
	cell.width = 60;
	cell.height = 14;
	column.push_back(cell);
	for (int i = 0, j = 0; j < 9; j++) {
		if (ability_labels[j] == "<unused>")
			continue;
		else if (!useMP && ability_labels[j] == "MaxMP:")
			continue;
		if (i == selected) {
			cell.text = std::string(_t(ability_labels[j].c_str()));
			cell.text.replace(cell.text.begin()+cell.text.find(':'),
				cell.text.end(), "");
			break;
		}
		i++;
	}
	cell.justify = JUSTIFY_LEFT;
	cell.color = grey;
	cell.width = 60;
	cell.height = 14;
	column.push_back(cell);
	tableData.push_back(column);
	column.clear();

	cell.text = _t("Remain");
	cell.justify = JUSTIFY_RIGHT;
	cell.color = grey;
	cell.width = 45;
	cell.height = 14;
	column.push_back(cell);
	sprintf(charBuffer, "%d", *d->points);
	cell.text = std::string(charBuffer);
	cell.justify = JUSTIFY_RIGHT;
	cell.color = white;
	cell.width = 45;
	cell.height = 14;
	column.push_back(cell);
	tableData.push_back(column);
	column.clear();

	cell.text = "#";
	cell.justify = JUSTIFY_RIGHT;
	cell.color = grey;
	cell.width = 40;
	cell.height = 14;
	column.push_back(cell);
	sprintf(charBuffer, "%d", *(values[selected]));
	cell.text = std::string(charBuffer);
	cell.justify = JUSTIFY_RIGHT;
	cell.color = white;
	cell.width = 40;
	cell.height = 14;
	column.push_back(cell);
	tableData.push_back(column);
	column.clear();

	cell.text = _t("Adjust");
	cell.justify = JUSTIFY_RIGHT;
	cell.color = grey;
	cell.width = 45;
	cell.height = 14;
	column.push_back(cell);
	cell.text = "";
	cell.justify = JUSTIFY_RIGHT;
	cell.color = grey;
	cell.width = 45;
	cell.height = 14;
	column.push_back(cell);
	tableData.push_back(column);
	column.clear();

	MTable *table = new MTable(x+5, y+5, tableData, 
		m_map_rgb_f(blue.r+0.1, blue.g+0.1, blue.b+0.1));

	MIcon *down = new MIcon(x+195-38, y+5+28-10, getResource("media/down.png"), white, false, NULL, false, false, false, true, true);
	MIcon *up = new MIcon(x+195-10, y+5+28-10, getResource("media/up.png"), white, false, NULL, false, false, false, true, true);

	MTextButton *ok = new MTextButton(BW/2-m_text_length(game_font, "OK***")/2, y+37,
		"OK");

	MRectangle *fullscreenRect = new MRectangle(0, 0, BW, BH,
		m_map_rgba(0, 0, 0, 0), 0);

	tguiPush();
	
	tguiSetParent(0);
	tguiAddWidget(fullscreenRect);
	tguiSetParent(fullscreenRect);
	tguiAddWidget(frame);
	tguiAddWidget(table);
	tguiAddWidget(down);
	tguiAddWidget(up);
	tguiAddWidget(ok);
	tguiSetFocus(ok);

	clear_input_events();

	for (;;) {
		al_wait_cond(wait_cond, wait_mutex);
		int tmp_counter = logic_counter;
		logic_counter = 0;
		if (tmp_counter > 10)
			tmp_counter = 1;
		while  (tmp_counter > 0) {
			next_input_event_ready = true;

			tmp_counter--;
			if (is_close_pressed()) {
				do_close();
				close_pressed = false;
			}
			if (break_main_loop) {
				goto done;
			}

			// Logic
			INPUT_EVENT ie = get_next_input_event();
			// update gui
			TGUIWidget *widget = tguiUpdate();
			if (widget == ok || iphone_shaken(0.1)) {
				iphone_clear_shaken();
				goto done;
			}
			else if (widget == up || widget == down
				|| ie.up == DOWN || ie.down == DOWN) {
				use_input_event();
				if (ie.up == DOWN || ie.down == DOWN)
					playPreloadedSample("select.ogg");
				int sel = selected;
				int inc;
				if (widget == up || ie.up == DOWN) {
					inc = 1;
				}
				else {
					inc = -1;
				}
				if (inc > 0 || *(values[sel]) + inc >= *(orig_values[sel])) {
					*(values[sel]) += inc;
					*(d->points) -= inc;
					if (*(d->points) == 0) {
						goto done;
					}
					sprintf(charBuffer, "%d", *d->points);
					cell.text = std::string(charBuffer);
					cell.justify = JUSTIFY_RIGHT;
					cell.color = white;
					cell.width = 45;
					cell.height = 14;
					tableData[1][1] = cell;

					sprintf(charBuffer, "%d", *(values[sel]));
					cell.text = std::string(charBuffer);
					cell.justify = JUSTIFY_RIGHT;
					cell.color = white;
					cell.width = 40;
					cell.height = 14;
					tableData[2][1] = cell;

					table->setData(tableData);
				}
			}
		}

		if (draw_counter > 0) {
			draw_counter = 0;
			al_set_target_backbuffer(display);
			get_screen_offset_size(&dx, &dy, &dw, &dh);
			m_draw_bitmap_identity_view(tmpbuffer, dx, dy, 0);
			m_set_blender(M_ONE, M_INVERSE_ALPHA, white);
			// Draw the GUI
			tguiDraw();
			drawBufferToScreen();
			m_flip_display();
		}
	}

done:

	tguiDeleteWidget(fullscreenRect);
	delete fullscreenRect;
	delete frame;
	delete table;
	delete up;
	delete down;
	delete ok;

	tguiPop();

	dpad_on();
}

/// returns true to signify the caller to call again
bool levelUp(Player *player, int bonus)
{
	dpad_off();

	CombatantInfo orig_info = player->getInfo();

	float currVol = (float)config.getMusicVolume() / 255.0f;
	if (currVol > 0.5f) {
		setMusicVolume(0.5f);
	}

	int points = bonus ? bonus : 10+(getLevel(player->getInfo().experience)/5*10);
	int start_points = points;
	
	std::vector<std::string> spells =
		getLearnedSpells(player->getName(),
		getLevel(player->getInfo().experience));

	// award spells (and get caption)
	std::string caption = "";
	if (!bonus) {
		for (int i = 0; i < (int)spells.size(); i++) {
			if (i == 0) {
				caption = std::string(_t("Learned ")) + std::string(_t(spells[i].c_str()));
			}
			else {
				caption += ", " + std::string(_t(spells[i].c_str()));
			}
			/* Don't award same spell twice, as in if player
			 * decides to reset stats and assign them again
			 */
			int j;
			for (j = 0; player->getInfo().spells[j] != ""; j++) {
				if (player->getInfo().spells[j] == spells[i])
					break;
			}
			if (player->getInfo().spells[j] == "")
				player->getInfo().spells[j] = spells[i];
		}
	}

	MLevelUpHeader *levelUpHeader = new MLevelUpHeader(&points, caption, bonus != 0);
	int number = 0;
	for (; number < MAX_PARTY; number++) {
		if (party[number] == player)
			break;
	}
	MStats *stats = new MStats(35, BH-6-16*2+1, number, false);

	MRectangle *fullscreenRect = new MRectangle(0, 0, BW, BH, blue, M_FILLED);
	
	std::vector<MultiPoint> mcPoints;

	int yy = 40;

	for (int i = 0; i < 9; i++) {
		if (ability_labels[i] == "<unused>") {
			continue;
		}
		if (!(player->getInfo().characterClass & CLASS_MAGE) && !(player->getInfo().characterClass & CLASS_CLERIC)
			&& ability_labels[i] == "MaxMP:") {
			yy += 12;
			continue;
		}
		MultiPoint p;
		p.x = BW/2+70;
		p.y = yy+(m_text_height(game_font)/2);
		p.west = false;
		mcPoints.push_back(p);
		yy += 12;
	}

	LEVEL_UP_CALLBACK_DATA levelUpData;
	levelUpData.player = player;
	levelUpData.points = &points;
	levelUpData.origInfo = &orig_info;
	
	MMultiChooser *multiChooser = new MMultiChooser(mcPoints, false);

	tguiPush();

	// Add widgets
	tguiSetParent(0);
	tguiAddWidget(fullscreenRect);
	tguiSetParent(fullscreenRect);
	tguiAddWidget(levelUpHeader);
	tguiAddWidget(stats);
	tguiAddWidget(multiChooser);
	tguiSetFocus(multiChooser);

	al_set_target_backbuffer(display);
	m_set_blender(M_ONE, M_INVERSE_ALPHA, white);
	tguiDraw();
	drawBufferToScreen();
	fadeIn(black);

	clear_input_events();

	for (;;) {
		al_wait_cond(wait_cond, wait_mutex);
		// Logic
		int tmp_counter = logic_counter;
		logic_counter = 0;
		if (tmp_counter > 10)
			tmp_counter = 1;
		while  (tmp_counter > 0) {
			next_input_event_ready = true;

			tmp_counter--;
			if (is_close_pressed()) {
				do_close();
				close_pressed = false;
			}
			if (break_main_loop) {
				goto done;
			}

			// update gui
			TGUIWidget *widget = tguiUpdate();
			if (widget == multiChooser) {
				std::vector<int> &v = multiChooser->getSelected();
				if (v.size() <= 0) {
					bool cancelled = false;
					al_set_target_backbuffer(display);
					m_clear(black);
					tguiDraw();
					drawBufferToScreen();
					bool prompt_ret = prompt("Reset and", "start over?", 0, 0, "", &cancelled);
					if (!cancelled) {
						if (prompt_ret == true) {
							CombatantInfo &info = player->getInfo();
							memcpy(&info.abilities, &orig_info.abilities, sizeof(CombatantAbilities));
							points = start_points;
						}
					}
					std::vector<int> v;
					v.push_back(0);
					multiChooser->setSelected(v);
					v.clear();
					tguiSetFocus(multiChooser);
				}
				else {
					if (v[0] < 0) {
						for (int i = 0; i < (int)v.size(); i++) {
							v[i] = -v[i] - 1;
						}
					}
				}
			}
			if (multiChooser->getTapped() >= 0) {
				al_set_target_backbuffer(display);
				m_clear(black);
				tguiDraw();
				drawBufferToScreen();
				levelUpCallback(multiChooser->getTapped(), &levelUpData);
				multiChooser->setTapped(false);

				if (points <= 0) {
					goto done;
				}
			}
		}

		if (draw_counter > 0) {
			draw_counter = 0;
			al_set_target_backbuffer(display);
			m_clear(black);
			tguiDraw();
			drawBufferToScreen();
			m_flip_display();
		}
	}

done:

	al_set_target_backbuffer(display);
	m_clear(black);
	m_set_blender(M_ONE, M_INVERSE_ALPHA, white);
	// Draw the GUI
	tguiDraw();
	drawBufferToScreen();
	m_flip_display();

	bool ret;
	bool prompt_ret;
	al_set_target_backbuffer(display);
	m_clear(black);
	tguiDraw();
	drawBufferToScreen();
	prompt_ret = prompt("Keep changes?", "Select no to reset...", 0, 0);
	if (prompt_ret) {
		ret = false;
	}
	else {
		CombatantInfo &info = player->getInfo();
		memcpy(&info.abilities, &orig_info.abilities, sizeof(CombatantAbilities));
		ret = true;
	}
	
	setMusicVolume(1);
			
	al_set_target_backbuffer(display);
	m_clear(black);
	m_set_blender(M_ONE, M_INVERSE_ALPHA, white);
	// Draw the GUI
	tguiDraw();
	drawBufferToScreen();
	fadeOut(black);

	tguiDeleteWidget(fullscreenRect);

	delete levelUpHeader;
	delete stats;
	delete multiChooser;
	delete fullscreenRect;
	mcPoints.clear();

	dpad_on();

	tguiPop();

	return ret;
}

void copyInfo(CombatantInfo &info, CombatantInfo &newinfo)
{
	memcpy(&info.abilities, &newinfo.abilities, sizeof(CombatantAbilities));
	memcpy(&info.equipment, &newinfo.equipment, sizeof(CombatantEquipment));
	bool done = false;
	for (int i = 0; i < MAX_SPELLS; i++) {
		if (done || newinfo.spells[i] == "<nil>") {
			done = true;
			info.spells[i] = "";
		}
		else
			info.spells[i] = newinfo.spells[i];
	}
	info.experience = newinfo.experience;
	info.characterClass = newinfo.characterClass;
	info.condition = newinfo.condition;
}
	
void increaseGold(int amount)
{
	gold += amount;
	if (gold > MAX_GOLD) {
		gold = MAX_GOLD;
	}
}

void giveSpell(std::string who, std::string spell)
{
	for (int i = 0; i < MAX_PARTY; i++) {
		if (party[i] && party[i]->getName() == who) {
			for (int j = 0; j < MAX_SPELLS_IN_THIS_GAME; j++) {
				if (party[i]->getInfo().spells[j] == "") {
					party[i]->getInfo().spells[j] = spell;
					break;
				}
			}
		}
	}
}

