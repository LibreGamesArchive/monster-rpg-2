#include "monster2.hpp"

#ifdef ALLEGRO_IPHONE
#include <CoreFoundation/CoreFoundation.h>
#include <CFNetwork/CFNetwork.h>
#endif
			
#if defined ALLEGRO_IPHONE
#include "iphone.h"
#endif

#ifdef ALLEGRO_ANDROID
#include "java.h"
#endif

bool on_title_screen = false;

bool fairy_used = false;

bool in_map = false;
MMap *mapWidget = NULL;
bool in_pause = false;

enum PauseSection {
	MAIN = 0,
	ITEM,
	MAGIC,
	STATS,
	CHOOSER
};


static void draw_tiny_tri(int x, int y, int height, int dir, ALLEGRO_COLOR c)
{
	for (int i = 0; i < height; i++) {
		int yy = y + (dir * i);
		int w = 1 + (i*2);
		int xx = x - w/2;
		for (int j = 0; j < w; j++) {
			m_draw_pixel(xx+j, yy, c);
		}
	}
}


static const char *strings[] = {
"00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "0A", "0B", "0C", "0D", "0E", "0F", 
"10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "1A", "1B", "1C", "1D", "1E", "1F", 
"20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "2A", "2B", "2C", "2D", "2E", "2F", 
"30", "31", "32", "33", "34", "35", "36", "37", "38", "39", "3A", "3B", "3C", "3D", "3E", "3F", 
"40", "41", "42", "43", "44", "45", "46", "47", "48", "49", "4A", "4B", "4C", "4D", "4E", "4F", 
"50", "51", "52", "53", "54", "55", "56", "57", "58", "59", "5A", "5B", "5C", "5D", "5E", "5F", 
"60", "61", "62", "63", "64", "65", "66", "67", "68", "69", "6A", "6B", "6C", "6D", "6E", "6F", 
"70", "71", "72", "73", "74", "75", "76", "77", "78", "79", "7A", "7B", "7C", "7D", "7E", "7F", 
"80", "81", "82", "83", "84", "85", "86", "87", "88", "89", "8A", "8B", "8C", "8D", "8E", "8F", 
"90", "91", "92", "93", "94", "95", "96", "97", "98", "99", "9A", "9B", "9C", "9D", "9E", "9F", 
"A0", "A1", "A2", "A3", "A4", "A5", "A6", "A7", "A8", "A9", "AA", "AB", "AC", "AD", "AE", "AF", 
"B0", "B1", "B2", "B3", "B4", "B5", "B6", "B7", "B8", "B9", "BA", "BB", "BC", "BD", "BE", "BF", 
"C0", "C1", "C2", "C3", "C4", "C5", "C6", "C7", "C8", "C9", "CA", "CB", "CC", "CD", "CE", "CF", 
"D0", "D1", "D2", "D3", "D4", "D5", "D6", "D7", "D8", "D9", "DA", "DB", "DC", "DD", "DE", "DF", 
"E0", "E1", "E2", "E3", "E4", "E5", "E6", "E7", "E8", "E9", "EA", "EB", "EC", "ED", "EE", "EF", 
"F0", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "FA", "FB", "FC", "FD", "FE", "FF"
};


char *create_url(unsigned char *bytes, int len)
{
	static char store[5000*3];
	
	int j = 0;
	for (int i = 0; i < len; i++) {
		store[j++] = strings[bytes[i]][0];
		store[j++] = strings[bytes[i]][1];
	}
	store[j] = 0;
	return store;
}

void save_url(const char *filename, const char *buf)
{
	FILE *f = fopen(filename, "wb");
	if (!f) {
		return;
	}

	int half = (int)strlen(buf)/2;

	for (int i = 0; i < half; i++) {
		char b1 = *buf;
		buf++;
		char b2 = *buf;
		buf++;
		int index = 0;
		for (int j = 0; j < 256; j++) {
			if (strings[j][0] == b1 && strings[j][1] == b2) {
				index = j;
				break;
			}
		}
		fputc(index, f);
	}

	fclose(f);
}


static std::string getTimeString(uint32_t runtime)
{
	char s[15];

	uint32_t rt = runtime;
	int days = rt / 60 / 60 / 24;
	rt -= days * 24 * 60 * 60;
	int hours = rt / 60 / 60;
	rt -= hours * 60 * 60;
	int minutes = rt / 60;

	sprintf(s, "%02d:%02d:%02d", days, hours, minutes);
	return std::string(s);
}

static void maybeShowItemHelp(void)
{
	// Give help message the first time
	if (!area || in_map) {
		return;
	}
	lua_State *state = area->getLuaState();
	lua_getglobal(state, "MS_ITEM_CHOOSER_HELP");
	int ms = lua_tonumber(state, -1)-1;
	lua_pop(state, 1);
	if (!gameInfo.milestones[ms]) {
#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_ANDROID
		notify("Press the action button twice in one", "spot to use. Press in", "different spots to arrange.");
#endif
		gameInfo.milestones[ms] = true;
	}
}

void showSaveStateInfo(const char *basename)
{
	bool first = true;

	dpad_off();
	
	bool delayed = false;
	
	int w = 220;
	int h = 140;
	int x = (BW-w)/2;
	int y = (BH-h)/2;

#ifndef NO_PHYSFS
	al_set_standard_file_interface();
#endif
	ALLEGRO_BITMAP *ss = al_load_bitmap(getUserResource("%s.bmp", basename));
#ifndef NO_PHYSFS
	al_set_physfs_file_interface();
#endif

	char d[100];
	strcpy(d, file_date(getUserResource("%s.save", basename)));

	tguiPush();

	FakeWidget *w1 = new FakeWidget(BW/2-8, 138 , 16, 16);
	tguiSetParent(0);
	tguiAddWidget(w1);
	tguiSetFocus(w1);

	double started = al_get_time();
	bool updating = false;

	while (1) {
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

			if (!first) {
				if (!updating) {
					if (al_get_time() > started+0.5) {
						clear_input_events();
						updating = true;
					}
				}
				else {
					if (use_dpad) {
						INPUT_EVENT ie = get_next_input_event();
						if (ie.button1 == DOWN || ie.button2 == DOWN) {
							use_input_event();
							playPreloadedSample("select.ogg");
							goto done;
						}
					}
					if (tguiUpdate() == w1 || iphone_shaken(0.1)) {
						iphone_clear_shaken();
						playPreloadedSample("select.ogg");
						goto done;
					}
				}
			}
		}
		
		if (draw_counter > 0 || first) {
			draw_counter = 0;
		
			set_target_backbuffer();
	
			m_draw_bitmap_identity_view(tmpbuffer, 0, 0, 0);

			// Draw frame
			mDrawFrame(x, y, w, h, true);
			// Draw info

			// draw ss and date
			if (ss) {
				quick_draw(ss, BW/2-BW/4, BH/3-BH/4, 0);
			}
			else {
				mTextout(game_font, _t("<No Screenshot>"), BW/2, BH/3,
					grey, black, WGT_TEXT_NORMAL, true);
			}

			mTextout(game_font, d, BW/2, BH/3+BH/4+20, white, black,
				WGT_TEXT_NORMAL, true);

			mTextout(game_font, _t("OK"), BW/2, 140,
				grey, black,
				WGT_TEXT_NORMAL, true);

			// Draw "cursor"
			int tick = (unsigned)tguiCurrentTimeMillis() % 1000;
			if (tick < 800) {
				int size = m_get_bitmap_width(cursor);
				int rx = BW/2-m_text_length(game_font, _t("OK"))/2-size-2;
				int ry = 140+cursor_offset(true);
				m_draw_bitmap(cursor, rx, ry, 0);
			}
			drawBufferToScreen();
			m_flip_display();
			
			if (!delayed) {
				delayed = true;
				m_rest(0.25);
			}
		}

		if (first) {
			if (!getInput()->getDescriptor().button1) {
				first = false;
			}
		}
	}
	
done:

	dpad_on();

	if (ss) {
		al_destroy_bitmap(ss);
	}
	
	tguiDeleteWidget(w1);
	delete w1;
	
	tguiPop();

	waitForRelease(4);
	waitForRelease(5);
	clear_input_events();

	return;
}

static void draw_item_desc(lua_State *luaState, int index, int y)
{
	std::string name = getItemName(index);
	mTextout(game_font, _t(name.c_str()), BW/2, y,
		grey, black,
		WGT_TEXT_NORMAL, true);
	callLua(luaState, "get_item_description", "i>s", index);
	const char *desc = lua_tostring(luaState, -1);

	start_text();
	
	mTextout(game_font, _t(desc), BW/2, y+m_text_height(game_font),
		grey, black,
		WGT_TEXT_NORMAL, true);
	lua_pop(luaState, 1);

	switch (items[index].type) {
		case ITEM_TYPE_WEAPON: {
			char buf[100];
			sprintf(buf, _t("Attack: %d"), weapons[items[index].id].attack);
			mTextout(game_font, buf,
				BW/2, y+m_text_height(game_font)*3,
				grey, black,
				WGT_TEXT_NORMAL, true);
			break;
		}
		case ITEM_TYPE_HEAD_ARMOR:
		case ITEM_TYPE_FEET_ARMOR:
		case ITEM_TYPE_CHEST_ARMOR: {
			ArmorItem *a;
			switch (items[index].type) {
				case ITEM_TYPE_HEAD_ARMOR:
					a = (ArmorItem *)helmets;
					break;
				case ITEM_TYPE_FEET_ARMOR:
					a = (ArmorItem *)feetArmors;
					break;
				default:
					a = (ArmorItem *)chestArmors;
					break;
			}
			char buf[100];
			sprintf(buf, _t("Defense: %d"), a[items[index].id].defense);
			mTextout(game_font, buf,
				BW/2, y+m_text_height(game_font)*3,
				grey, black,
				WGT_TEXT_NORMAL, true);
			sprintf(buf, _t("MDefense: %d"), a[items[index].id].magicDefense);
			mTextout(game_font, buf,
				BW/2, y+m_text_height(game_font)*4,
				grey, black,
				WGT_TEXT_NORMAL, true);
			break;
		}
		default:
			break;
	}

	end_text();
}

static void draw_item_users(MBITMAP *partyBmps[MAX_PARTY], lua_State *luaState, int index, int y)
{
	bool can_use[MAX_PARTY] = { false, };
	int diff[MAX_PARTY] = { 0, }; // better, worse, or equal to current item

	ItemType type = items[index].type;

	if (type == ITEM_TYPE_STATUS) {
		for (int i = 0; i < MAX_PARTY; i++) {
			if (!party[i])
				continue;
			can_use[i] = true;
		}
	}
	else if (type == ITEM_TYPE_WEAPON) {
		for (int i = 0; i < MAX_PARTY; i++) {
			if (!party[i])
				continue;
			CombatantInfo &info = party[i]->getInfo();
			if (info.characterClass & weapons[items[index].id].classes) {
				can_use[i] = true;
				if (weapons[items[index].id].satisfies < 0) {
					int curr_power;
					if (info.equipment.lhand >= 0 && weapons[items[info.equipment.lhand].id].satisfies < 0) {
						curr_power = weapons[items[info.equipment.lhand].id].attack;
						
					}
					else {
						if (info.equipment.rhand < 0) {
							curr_power = 0;
						}
						else {
							curr_power = weapons[items[info.equipment.rhand].id].attack;
						}
					}
					int selected_power = weapons[items[index].id].attack;
					diff[i] = selected_power - curr_power;
				}
			}
		}
	}
	else if (type == ITEM_TYPE_HEAD_ARMOR) {
		for (int i = 0; i < MAX_PARTY; i++) {
			if (!party[i])
				continue;
			CombatantInfo &info = party[i]->getInfo();
			if (info.characterClass & helmets[items[index].id].classes) {
				can_use[i] = true;
				if (info.equipment.harmor < 0) {
					diff[i] = 1;
				}
				else {
					diff[i] = helmets[items[index].id].defense - helmets[items[info.equipment.harmor].id].defense;
				}
			}
		}
	}
	else if (type == ITEM_TYPE_CHEST_ARMOR) {
		for (int i = 0; i < MAX_PARTY; i++) {
			if (!party[i])
				continue;
			CombatantInfo &info = party[i]->getInfo();
			if (info.characterClass & chestArmors[items[index].id].classes) {
				can_use[i] = true;
				if (info.equipment.carmor < 0) {
					diff[i] = 1;
				}
				else {
					diff[i] = chestArmors[items[index].id].defense - chestArmors[items[info.equipment.carmor].id].defense;
				}
			}
		}
	}
	else if (type == ITEM_TYPE_FEET_ARMOR) {
		for (int i = 0; i < MAX_PARTY; i++) {
			if (!party[i])
				continue;
			CombatantInfo &info = party[i]->getInfo();
			if (info.characterClass & feetArmors[items[index].id].classes) {
				can_use[i] = true;
				if (info.equipment.farmor < 0) {
					diff[i] = 1;
				}
				else {
					diff[i] = feetArmors[items[index].id].defense - feetArmors[items[info.equipment.farmor].id].defense;
				}
			}
		}
	}

	for (int i = 0; i < MAX_PARTY; i++) {
		if (can_use[i] && partyBmps[i]) {
			int x = BW/2+(i-2)*20;
			int w = al_get_bitmap_width(partyBmps[i]->bitmap);
			int h = al_get_bitmap_height(partyBmps[i]->bitmap);
			m_draw_bitmap(partyBmps[i], BW/2+(i-2)*20, y, 0);
			if (diff[i] < 0) {
				draw_tiny_tri(
					x+w-2,
					y+h-1,
					3,
					-1,
					al_map_rgb_f(1, 0, 0)
				);
			}
			else if (diff[i] > 0) {
				draw_tiny_tri(
					x+w-2,
					y+h-6,
					3,
					1,
					al_map_rgb_f(0, 1, 0)
				);
			}
		}
	}
}

void showItemInfo(int index, bool preserve_buffer)
{
	bool first = true;

	dpad_off();
	
	bool delayed = false;
	
	int w = 200;
	int h = 120;
	int x = (BW-w)/2;
	int y = (BH-h)/2;

	MBITMAP *partyBmps[MAX_PARTY];

	for (int i = 0; i < MAX_PARTY; i++) {
		if (party[i]) {
			partyBmps[i] = m_load_bitmap(getResource("objects/%s_front.png", party[i]->getName().c_str()));
		}
		else {
			partyBmps[i] = NULL;
		}
	}

	lua_State *luaState;
	luaState = lua_open();
	openLuaLibs(luaState);
	runGlobalScript(luaState);

	tguiPush();

	MRectangle *fullscreenRect = new MRectangle(0, 0, BW, BH,
		m_map_rgba(0, 0, 0, 0), 0);
	FakeWidget *w1 = new FakeWidget(BW/2-8, 128 , 20, 20);
	tguiSetParent(0);
	tguiAddWidget(fullscreenRect);
	tguiSetParent(fullscreenRect);
	tguiAddWidget(w1);
	tguiSetFocus(w1);

	double started = al_get_time();
	bool updating = false;

	while (1) {
		al_wait_cond(wait_cond, wait_mutex);
		int tmp_counter = logic_counter;
		logic_counter = 0;
		if (tmp_counter > 10)
			tmp_counter = 1;
		while (tmp_counter > 0) {
			next_input_event_ready = true;

			tmp_counter--;
			if (is_close_pressed()) {
				do_close();
				close_pressed = false;
			}
			if (break_main_loop) {
				goto done;
			}

			if (!first) {
				if (!updating) {
					if (al_get_time() > started+0.5) {
						clear_input_events();
						updating = true;
					}
				}
				else {
					INPUT_EVENT ie = get_next_input_event();
					if (ie.button1 == DOWN || ie.button2 == DOWN) {
						use_input_event();
						playPreloadedSample("select.ogg");
						goto done;
					}
					if (iphone_shaken(0.1) || tguiUpdate() == w1) {
						iphone_clear_shaken();
						playPreloadedSample("select.ogg");
						goto done;
					}
				}
			}
		}

		if (draw_counter > 0 || first) {
			draw_counter = 0;

			set_target_backbuffer();

			if (preserve_buffer) {
				m_clear(m_map_rgb(0, 0, 0));
				m_draw_bitmap_identity_view(tmpbuffer, 0, 0, 0);
			}

			// Draw frame
			mDrawFrame(x, y, w, h, true);

			draw_item_desc(luaState, index, y+5+m_text_height(game_font)/2);
			draw_item_users(partyBmps, luaState, index, 95);

			mTextout(game_font, _t("OK"), BW/2, 130,
				grey, black,
				WGT_TEXT_NORMAL, true);

			// Draw "cursor"
			int tick = (unsigned)tguiCurrentTimeMillis() % 1000;
			if (tick < 800) {
				int size = m_get_bitmap_width(cursor);
				int rx = BW/2-m_text_length(game_font, _t("OK"))/2-size-2;
				int ry = 130+cursor_offset(true);
				m_draw_bitmap(cursor, rx, ry, 0);
			}

			drawBufferToScreen();
			m_flip_display();
			
			if (!delayed) {
				delayed = true;
				m_rest(0.25);
			}
		}

		if (first) {
			if (!getInput()->getDescriptor().button1) {
				first = false;
			}
		}
		
		m_rest(0.001);
	}
	
done:

	dpad_on();
	
	for (int i = 0; i < MAX_PARTY; i++) {
		if (partyBmps[i])
			m_destroy_bitmap(partyBmps[i]);
	}

	lua_close(luaState);
	tguiDeleteWidget(fullscreenRect);
	delete fullscreenRect;
	delete w1;

	tguiPop();

	waitForRelease(4);
	waitForRelease(5);
	clear_input_events();

	return;
}

#ifndef ALLEGRO_IPHONE
#define delete_file remove
#endif

static bool choose_save_slot(int num, bool exists, void *data)
{
	std::string map_name = *((std::string *)data);
	if (num >= 0) {
		if (exists) {
			if (prompt("Overwrite?", "", 0, 0)) {
				saveGame(getUserResource("%d.save", num), map_name);
				if (screenshot) {
#ifndef NO_PHYSFS
					al_set_standard_file_interface();
#endif
					al_save_bitmap(getUserResource("%d.bmp", num), screenshot->bitmap);
#ifndef NO_PHYSFS
					al_set_physfs_file_interface();
#endif
				}
				else {
					delete_file(getUserResource("%d.bmp", num));
					delete_file(getUserResource("%d.png", num));
				}

				force_auto_save(map_name);

				notify("Your game", "has been saved...", "");
			}
		}
		else {
			saveGame(getUserResource("%d.save", num), map_name);
			if (screenshot) {
#ifndef NO_PHYSFS
				al_set_standard_file_interface();
#endif
				al_save_bitmap(getUserResource("%d.bmp", num), screenshot->bitmap);
#ifndef NO_PHYSFS
				al_set_physfs_file_interface();
#endif
			}
			else {
				delete_file(getUserResource("%d.png", num));
				delete_file(getUserResource("%d.bmp", num));
			}

			force_auto_save(map_name);

			notify("Your game", "has been saved...", "");
		}
	}
	
	return true;
}

// return false on quit
bool pause(bool can_save, bool change_music_volume, std::string map_name)
{
	/* Save a screenshot in case user saves their game */
	if (in_map) {
		al_lock_mutex(ss_mutex);
		ALLEGRO_BITMAP *old_target = al_get_target_bitmap();
		m_set_target_bitmap(screenshot);
		ALLEGRO_TRANSFORM t;
		al_identity_transform(&t);
		al_scale_transform(&t, 0.5f, 0.5f);
		al_use_transform(&t);
		m_clear(black);
		tguiDraw();
		al_set_target_bitmap(old_target);
		al_unlock_mutex(ss_mutex);
	}
	else if (area) {
		al_lock_mutex(ss_mutex);
		ALLEGRO_BITMAP *old_target = al_get_target_bitmap();
		m_set_target_bitmap(screenshot);
		ALLEGRO_TRANSFORM t;
		al_identity_transform(&t);
		al_scale_transform(&t, 0.5f, 0.5f);
		al_use_transform(&t);
		area->draw();
		al_set_target_bitmap(old_target);
		al_unlock_mutex(ss_mutex);
	}

	in_pause = true;
	
	dpad_off();
	
	if (timer_on)
		can_save = false;
	else
		can_save = true;

	if (!global_can_save)
		can_save = false;

	Input *input = getInput();
	int dir = input->getDirection();

	tguiPush();

	if (change_music_volume) {
		setMusicVolume(0.5f);
		setAmbienceVolume(0.5f);
	}

	bool ret = true;

	// Main widgets
	MRectangle *fullscreenRect = new MRectangle(0, 0, BW, BH,
		blue, M_FILLED);
	MCorner *corner = new MCorner(157-80, 3);
	std::vector<int> splits;
	splits.push_back(100);
	splits.push_back(127);
	MSplitFrame *mainFrame = new MSplitFrame(162, 3, 75, BH-5, splits);

	int yy = 6;
	int yinc;

#if defined ALLEGRO_ANDROID || defined ALLEGRO_IPHONE
	yinc = 14;
#else
	if (use_dpad) yinc = 14;
	else yinc = 16;
#endif

	int yyy = 6;

#if defined ALLEGRO_IPHONE || defined ALLEGRO_MACOSX
	MIcon *game_center = NULL;
#if !defined NO_GAMECENTER
	if (isGameCenterAPIAvailable()) {
		game_center = new MIcon(128, yyy, getResource("game_center.png"), al_map_rgb(255, 255, 255), true, NULL, false, true, true, true, false);
		yyy += 26;
	}
#endif
#elif defined AMAZON
	MIcon *game_center = NULL;
	if (amazon_initialized() == 1) {
		game_center = new MIcon(128, yyy, getResource("gamecircle.png"), al_map_rgb(255, 255, 255), true, NULL, false, true, true, true, false);
		yyy += 26;
	}
#endif
	
	MIcon *fairy = new MIcon(128, yyy, getResource("fairy.png"), al_map_rgb(255, 255, 255), true, NULL, false, true, true, true, false);

	// add fairy to menu if you have < 40 "pts" worth of items, or 0 heals
	bool add_fairy;
	bool heal_only;
	bool need_gold = false;
	int pts = 0;
	int item_index;
	item_index = findUsedInventorySlot(CURE_INDEX);
	if (item_index >= 0)
		pts += 2 * inventory[item_index].quantity;
	item_index = findUsedInventorySlot(CURE2_INDEX);
	if (item_index >= 0)
		pts += 4 * inventory[item_index].quantity;
	item_index = findUsedInventorySlot(CURE3_INDEX);
	if (item_index >= 0)
		pts += 5 * inventory[item_index].quantity;
	item_index = findUsedInventorySlot(JUICE_INDEX);
	if (item_index >= 0)
		pts += 5 * inventory[item_index].quantity;
	item_index = findUsedInventorySlot(ELIXIR_INDEX);
	if (item_index >= 0)
		pts += 10 * inventory[item_index].quantity;
	item_index = findUsedInventorySlot(HOLY_WATER_INDEX);
	if (item_index >= 0)
		pts += 10 * inventory[item_index].quantity;
	if (pts < 40) { // arbitrary
		add_fairy = true;
		heal_only = false;
		if (gold < 250) {
			need_gold = true;
		}
		else
			need_gold = false;
	}
	else {
		item_index = findUsedInventorySlot(HEAL_INDEX);
		if (item_index == -1) {
			add_fairy = true;
			heal_only = true;
		}
		else {
			add_fairy = false;
			heal_only = false;
		}
	}

#if defined ALLEGRO_IPHONE || defined ALLEGRO_MACOSX || defined AMAZON
	TGUIWidget *left_widget = game_center;
#else
	TGUIWidget *left_widget = NULL;
	if (add_fairy && !fairy_used)
		left_widget = fairy;
#endif

	MTextButton *mainItem = new MTextButton(162, yy, "Item", false, left_widget, NULL, false);

	yy += yinc;
	MTextButton *mainMagic = new MTextButton(162, yy, "Magic", false, left_widget, NULL, false);
	yy += yinc;
	MTextButton *mainForm = new MTextButton(162, yy, "Form", false, left_widget, NULL, false);
	yy += yinc;
	MTextButton *mainStats = new MTextButton(162, yy, "Stats", false, left_widget, NULL, false);
	yy += yinc;
	TGUIWidget *mainSave;
	if (can_save)
		mainSave = new MTextButton(162, yy, "Save", false, left_widget, NULL, false);
	else
		mainSave = new MLabel(162+m_text_height(game_font)/2+2, yy, "Save", m_map_rgb(128, 128, 128));
	yy += yinc;
	MTextButton *mainResume = new MTextButton(162, yy, "Play", false, left_widget, NULL, false);

#if defined ALLEGRO_ANDROID || defined ALLEGRO_IPHONE
	MTextButton *mainMusic = new MTextButton(162, yy, "Options", false, left_widget, NULL, false);
	yy += yinc;
#endif

	MTextButton *mainLevelUp = new MTextButton(162, yy, "Cheat", false, left_widget, NULL, false);
	MTextButton *mainQuit = new MTextButton(162, yy, "Quit", false, left_widget, NULL, false);
#if defined ALLEGRO_IPHONE || defined ALLEGRO_MACOSX || defined AMAZON
	if (game_center)
		game_center->set_right_widget(mainItem);
#endif

	fairy->set_right_widget(mainItem);

	MDragNDropForm *dndForm = NULL;
	if (!use_dpad)
		dndForm = new MDragNDropForm();

	MParty *partyStats = new MParty();
	MLabel *mainTimeLabel = new MLabel(164, 107, "Time", grey);
	MLabel *mainTime = new MLabel(164, 116, getTimeString(runtime), grey);
	MLabel *mainGoldLabel = new MLabel(164, 133, "Gold", grey);
	char goldS[15];
	sprintf(goldS, "%d", gold);
	MLabel *mainGold = new MLabel(164, 142, std::string(goldS), grey);


	//MRectangle *fullscreenRect2 = new MRectangle(0, 0, BW, BH,
	MRectangle *fullscreenRect2 = new MRectangle(0, 0, 0, 0,
		blue, M_FILLED);
	MPartySelector *partySelectorTop = new MPartySelector(3, 0, true);
	MItemSelector *itemSelector = new MItemSelector(73, BH-2, 0, 0, true);
	std::vector<MultiPoint> equipPoints;
	int oy = 5 + m_text_height(game_font)/2;
	for (int i = 0; i < 5; i++) {
		MultiPoint p;
		p.x = BW/2+10;
		p.y = oy;
		oy += m_text_height(game_font) + 2;
		p.west = false;
		equipPoints.push_back(p);
	}
	MMultiChooser *equipChooser = new MMultiChooser(equipPoints, false);

	std::vector<MultiPoint> formPoints;
	std::vector<MultiPoint> formPoints_target;
	for (int i = 0; i < MAX_PARTY; i++) {
		MultiPoint p;
		p.x = 140;
		p.y = 10+(i*37);
		p.west = true;
		formPoints.push_back(p);
	}
	MMultiChooser *formChooser = new MMultiChooser(formPoints, false);
	MMultiChooser *formChooser_target = NULL;
	
	MMultiChooser *formChooser2 = new MMultiChooser(formPoints, false);

	MStats *stats = new MStats(3, BH-5, 0, true);

	MRectangle *fullscreenRect3 = new MRectangle(0, 0, 0, 0,
		blue, M_FILLED);
	MPartySelector *partySelectorTop2 = new MPartySelector(3, 0, false);
	CombatantInfo unused;
	MSpellSelector *spellSelector = new MSpellSelector(73, BH-2, 0, 0, true, partySelectorTop2, unused);

	// Add widgets
	tguiSetParent(0);
	tguiAddWidget(fullscreenRect);
	tguiSetParent(fullscreenRect);
	tguiAddWidget(corner);
	tguiAddWidget(mainFrame);
	tguiAddWidget(mainItem);
	tguiAddWidget(mainMagic);
	tguiAddWidget(mainForm);
	tguiAddWidget(mainStats);
	tguiAddWidget(mainSave);
#ifdef ALLEGRO_IPHONE
#ifdef DEBUG_XXX
	tguiAddWidget(mainLevelUp);
#endif
#endif

#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
	tguiAddWidget(mainMusic);
#endif

	tguiAddWidget(mainQuit);

#if defined ALLEGRO_IPHONE || defined ALLEGRO_MACOSX || defined AMAZON
	if (game_center)
		tguiAddWidget(game_center);
#endif

	if (add_fairy && !fairy_used)
		tguiAddWidget(fairy);

	tguiAddWidget(partyStats);
	tguiAddWidget(mainTimeLabel);
	tguiAddWidget(mainTime);
	tguiAddWidget(mainGoldLabel);
	tguiAddWidget(mainGold);
	if (dndForm)
		tguiAddWidget(dndForm);
	tguiSetFocus(mainItem);

	prepareForScreenGrab1();
	tguiDraw();
	drawBufferToScreen(false);
	prepareForScreenGrab2();
	fadeIn(black);

	PauseSection section = MAIN;
	std::string spellName;

	int who = 0;

	bool break_for_fade_after_draw = false;

	clear_input_events();

	for (;;) {
		al_wait_cond(wait_cond, wait_mutex);
		// Logic
		int tmp_counter = logic_counter;
		logic_counter = 0;
		if (tmp_counter > 10)
			tmp_counter = 1;
		while  (!break_for_fade_after_draw && tmp_counter > 0) {
			next_input_event_ready = true;

			tmp_counter--;
			if (is_close_pressed()) {
				do_close();
				close_pressed = false;
			}
			if (break_main_loop) {
				if (section == CHOOSER) {
					tguiDeleteWidget(formChooser_target);
					tguiSetFocus(formChooser_target);
					if (formChooser_target) {
						tguiDeleteWidget(formChooser_target);
						delete formChooser_target;
						formChooser_target = NULL;
					}
				}
				else if (section != MAIN) {
					tguiPop();
					tguiSetFocus(mainItem);
					section = MAIN;
				}
				goto done;
			}
		
			// update gui
			TGUIWidget *widget = tguiUpdate();
			if (widget == mainItem) {
				tguiPush();
				tguiSetParent(0);
				tguiAddWidget(fullscreenRect2);
				tguiSetParent(fullscreenRect2);
				/* Party selector must be before
				 * itemSelector (and spellSelector etc below)
				 */
				partySelectorTop->setSelected(0);
				tguiAddWidget(partySelectorTop);
				itemSelector->setSelected(0);
				itemSelector->setTop(0);
				tguiAddWidget(itemSelector);
				prepareForScreenGrab1();
				tguiDraw();
				prepareForScreenGrab2();
				maybeShowItemHelp();
				tguiSetFocus(partySelectorTop);
				section = ITEM;
				if (!use_dpad) {
					if (!gameInfo.milestones[MS_DRAG_TO_USE])
						onscreen_drag_to_use = true;
				}
			}
			else if (widget == itemSelector) {
				onscreen_drag_to_use = false;
				int sel = itemSelector->getSelected();
				if (sel < 0) {
					tguiDeleteWidget(fullscreenRect2);
					tguiPop();
					tguiSetFocus(mainItem);
					section = MAIN;

					// REMOVE OMNI
					stopAllOmni();
				}
				else {
					int drop_x, drop_y;
					itemSelector->getDropLocation(&drop_x, &drop_y);
					if ((drop_x < 0 && drop_y < 0) || (drop_x >= 0 && drop_y < itemSelector->getY())) {
						if (section == ITEM) {
							if (!use_dpad) {
								gameInfo.milestones[MS_DRAG_TO_USE] = true;
							}
							int index = partySelectorTop->getSelected();
							if (index >= MAX_PARTY) {
								prepareForScreenGrab1();
								m_clear(black);
								tguiDraw();
								drawBufferToScreen(false);
								prepareForScreenGrab2();
								use(NULL, sel, true);
							}
							else if (party[index]) {
								Combatant *c = party[index]->makeCombatant(index, false);
								prepareForScreenGrab1();
								m_clear(black);
								tguiDraw();
								drawBufferToScreen(false);
								prepareForScreenGrab2();
								use(c, sel, true);
								memcpy(&party[index]->getInfo(), &c->getInfo(), sizeof(CombatantInfo));
								delete c;
							}
							else
								playPreloadedSample("error.ogg");
						}
						itemSelector->reset();
					}
				}
			}
			else if (widget == partySelectorTop) {
				int who = partySelectorTop->getSelected();
				if (who < 0) {
					onscreen_drag_to_use = false;
					tguiDeleteWidget(fullscreenRect2);
					tguiPop();
					tguiSetFocus(mainItem);
					section = MAIN;
				}
				else if (who < MAX_PARTY) {
					InputDescriptor id = getInput()->getDescriptor();

					bool handled = false;
					if (!id.button1) {
						int chosen = partySelectorTop->getEquipIndex();
						int *toUnequip = 0;
						int *toUnequip_quantity = 0;
						int unequipQuantity = 1;
						switch (chosen) {
							case 0:
								toUnequip = &party[who]->getInfo().equipment.lhand;
								toUnequip_quantity = &party[who]->getInfo().equipment.lquantity;
								unequipQuantity = party[who]->getInfo().equipment.lquantity;
								break;
							case 1:
								toUnequip = &party[who]->getInfo().equipment.rhand;
								toUnequip_quantity = &party[who]->getInfo().equipment.rquantity;
								unequipQuantity = party[who]->getInfo().equipment.rquantity;
								break;
							case 2:
								toUnequip = &party[who]->getInfo().equipment.harmor;
								break;
							case 3:
								toUnequip = &party[who]->getInfo().equipment.carmor;
								break;
							case 4:
								toUnequip = &party[who]->getInfo().equipment.farmor;
								break;
						}
						if (*toUnequip >= 0) {
							if (section == ITEM) {
								playPreloadedSample("select.ogg");
								int slot = findUsedInventorySlot(*toUnequip);
								if (slot < 0) {
									slot = findEmptyInventorySlot();
									if (slot >= 0) {
										inventory[slot].index = *toUnequip;
										inventory[slot].quantity = unequipQuantity;
										*toUnequip = -1;
										if (toUnequip_quantity != 0) {
											*toUnequip_quantity = 0;
										}
									}
								}
								else if (inventory[slot].quantity+unequipQuantity <= 99) {
									inventory[slot].quantity += unequipQuantity;
									*toUnequip = -1;
									if (toUnequip_quantity != 0) {
										*toUnequip_quantity = 0;
									}
								}
								else {
									prepareForScreenGrab1();
									m_clear(black);
									tguiDraw();
									drawBufferToScreen(false);
									prepareForScreenGrab2();
									notify("You have no", "room in your", "inventory.");
								}
							}
							handled = true;
						}
					}
					if (!handled) {
						std::vector<int> i;
						i.push_back(0);
						equipChooser->setSelected(i);
						i.clear();
						tguiSetParent(0);
						tguiAddWidget(equipChooser);
						tguiSetFocus(equipChooser);
					}
				}
			}
			else if (widget == partySelectorTop2) {
				tguiDeleteWidget(fullscreenRect3);
				tguiPop();
				tguiSetFocus(mainMagic);
				section = MAIN;
			}
			else if (widget == equipChooser) {
				std::vector<int> &chosen = equipChooser->getSelected();
				if (chosen.size() > 0 && chosen[0] >= 0) {
					int who = partySelectorTop->getSelected();
					if (party[who]) {
						int *toUnequip = 0;
						int *toUnequip_quantity = 0;
						int unequipQuantity = 1;
						switch (chosen[0]) {
							case 0:
								toUnequip = &party[who]->getInfo().equipment.lhand;
								toUnequip_quantity = &party[who]->getInfo().equipment.lquantity;
								unequipQuantity = party[who]->getInfo().equipment.lquantity;
								break;
							case 1:
								toUnequip = &party[who]->getInfo().equipment.rhand;
								toUnequip_quantity = &party[who]->getInfo().equipment.rquantity;
								unequipQuantity = party[who]->getInfo().equipment.rquantity;
								break;
							case 2:
								toUnequip = &party[who]->getInfo().equipment.harmor;
								break;
							case 3:
								toUnequip = &party[who]->getInfo().equipment.carmor;
								break;
							case 4:
								toUnequip = &party[who]->getInfo().equipment.farmor;
								break;
						}
						if (*toUnequip >= 0) {
							if (section == ITEM) {
								int slot = findEmptyInventorySlot();
								if (slot >= 0) {
									inventory[slot].index = *toUnequip;
									inventory[slot].quantity = unequipQuantity;
									*toUnequip = -1;
									if (toUnequip_quantity != 0) {
										*toUnequip_quantity = 0;
									}
								}
								else {
									prepareForScreenGrab1();
									m_clear(black);
									tguiDraw();
									drawBufferToScreen(false);
									prepareForScreenGrab2();
									notify("You have no", "room in your", "inventory.");
								}
							}
						}
					}
					tguiDeleteWidget(equipChooser);
					tguiSetFocus(partySelectorTop);
				}
				else {
					tguiDeleteWidget(equipChooser);
					tguiSetFocus(partySelectorTop);
					for (int i = 0; i < (int)chosen.size(); i++) {
						chosen[i] = -chosen[i] - 1;
					}
				}
			}
			else if (widget == mainMagic) {
				tguiPush();
				tguiSetParent(0);
				tguiAddWidget(fullscreenRect3);
				tguiSetParent(fullscreenRect3);
				partySelectorTop2->setSelected(0);
				tguiAddWidget(partySelectorTop2);
				spellSelector->setSelected(0);
				spellSelector->setTop(0);
				tguiAddWidget(spellSelector);
				set_target_backbuffer();
				tguiDraw();
				// draw
				tguiSetFocus(partySelectorTop2);
				section = MAGIC;
			}
			else if (widget == spellSelector) {
				int sel = spellSelector->getSelected();
				if (sel < 0) {
					tguiDeleteWidget(fullscreenRect3);
					tguiPop();
					tguiSetFocus(mainMagic);
					section = MAIN;
				}
				else {
					who = partySelectorTop2->getSelected();
					if (party[who] && party[who]->getInfo().abilities.hp > 0) {
						spellName = party[who]->getInfo().spells[sel];
						if (spellName != "" && getSpellCost(spellName) <= party[who]->getInfo().abilities.mp) {
							if (getSpellAlignment(spellName) == SPELL_WHITE) {
								formPoints_target.clear();
								for (int i = 0; i < MAX_PARTY; i++) {
									if (party[i]) {
										MultiPoint p;
										p.x = 140;
										p.y = 10+(i*37);
										p.west = true;
										formPoints_target.push_back(p);
									}
								}
								if (formChooser_target) {
									delete formChooser_target;
								}
								formChooser_target = new MMultiChooser(formPoints_target, true);
								// FIXME: choose target and cast
								tguiDeleteWidget(fullscreenRect3);
								tguiPop();
								tguiSetFocus(mainMagic);
								section = CHOOSER;
								std::vector<int> v;
								v.push_back(0);
								formChooser_target->setSelected(v);
								tguiAddWidget(formChooser_target);
								tguiSetFocus(formChooser_target);
							}
							else {
								playPreloadedSample("error.ogg");
							}
						}
						else {
							playPreloadedSample("error.ogg");
						}
					}
					else {
						playPreloadedSample("error.ogg");
					}
				}
			}
			else if (widget == mainForm) {
				std::vector<int> v;
				v.push_back(0);
				formChooser->setSelected(v);
				tguiAddWidget(formChooser);
				tguiSetFocus(formChooser);
				section = CHOOSER;
			}
			else if (widget == mainStats) {
				stats->setSelected(0);
				tguiPush();
				tguiSetParent(0);
				tguiAddWidget(stats);
				tguiSetFocus(stats);
				section = STATS;
			}
			else if (widget == mainSave) {
				std::string prevMusicName = musicName;
				std::string prevAmbienceName = ambienceName;
				playAmbience("");
				playMusic("loadsave.ogg");
				m_rest(0.1);
				setMusicVolume(0.5f);

				choose_savestate_old("Choose a save state slot:", true, false, choose_save_slot, (void *)&map_name);
				playMusic(prevMusicName);
				playAmbience(prevAmbienceName);
				m_rest(0.1);
				setMusicVolume(0.5f);
				setAmbienceVolume(0.5f);
			}
			else if (widget == mainResume || iphone_shaken(0.1)) {
				iphone_clear_shaken();
				ret = true;
				goto done;
			}
			else if (widget == mainQuit) {
				prepareForScreenGrab1();
				m_clear(black);
				tguiDraw();
				drawBufferToScreen(false);
				prepareForScreenGrab2();
				if (prompt("Really quit?", "", 0, 0)) {
					ret = false;
					goto done;
				}
			}
			else if (widget == mainLevelUp) {
				for (int i = 0; i < MAX_PARTY; i++) {
					if (party[i]) {
						levelUp(party[i], 50);
					}
				}
				tguiSetFocus(mainLevelUp);
				section = MAIN;
			}
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
			else if (widget == mainMusic) {
				getInput()->set(false, false, false, false, false, false, false);
				config_menu(false);
				tguiSetFocus(mainMusic);
				section = MAIN;
			}
#endif
			else if (widget == formChooser && !formChooser->getInset()) {
				std::vector<int> &v = formChooser->getSelected();
				if (v.size() > 0) {
					if (v[0] < 0) {
						for (int i = 0; i < (int)v.size(); i++) {
							v[i] = -v[i] - 1;
						}
					}
					else {
						std::vector<int> v;
						v.push_back(0);
						formChooser2->setSelected(v);
						tguiAddWidget(formChooser2);
						tguiSetFocus(formChooser2);
						formChooser->setInset(true);
					}
				}
				else {
					tguiDeleteWidget(formChooser);
					tguiSetFocus(mainForm);
					section = MAIN;
				}
			}
			else if (formChooser_target && widget == formChooser_target) {
				std::vector<int> &v = formChooser_target->getSelected();
				if (v.size() > 0 && v[0] >= 0) {
					bool sound = true;
					for (int i = 0; i < (int)v.size(); i++) {
						int cx, cy;
						cx = 30;
						cy = (BH/8)+(BH/4)*v[i];
						applyWhiteMagicSpell(spellName, party[v[i]]->getInfo(), (int)v.size(), sound, NULL, cx, cy);
						sound = false;
					}
					party[who]->getInfo().abilities.mp -= getSpellCost(spellName);
					// HERE
					if (party[who]->getInfo().abilities.mp < getSpellCost(spellName)) {
						tguiDeleteWidget(formChooser_target);
						delete formChooser_target;
						formChooser_target = NULL;
						tguiSetFocus(mainMagic);
						section = MAIN;
					}
				}
				else if (v.size() > 0) {
					for (int i = 0; i < (int)v.size(); i++) {
						v[i] = -v[i] - 1;
					}
				}
				else {
					tguiDeleteWidget(formChooser_target);
					delete formChooser_target;
					formChooser_target = NULL;
					tguiSetFocus(mainMagic);
					section = MAIN;
				}
			}
			else if (widget == formChooser2) {
				std::vector<int> &v1 = formChooser->getSelected();
				std::vector<int> &v2 = formChooser2->getSelected();
				if (v2.size() > 0 && v2[0] >= 0) {
					int p1 = v1[0];
					int p2 = v2[0];
					if (p1 == p2 && party[p1] && party[p2]) {
						CombatFormation f = party[p1]->getFormation();
						if (f == FORMATION_FRONT) {
							f = FORMATION_BACK;
						}
						else {
							f = FORMATION_FRONT;
						}
						party[p1]->setFormation(f);
					}
					else {
						if (p1 == heroSpot)
							heroSpot = p2;
						else if (p2 == heroSpot)
							heroSpot = p1;
						Player *p = party[p1];
						party[p1] = party[p2];
						party[p2] = p;
						player = party[heroSpot];
					}
					tguiDeleteWidget(formChooser);
					tguiDeleteWidget(formChooser2);
					tguiSetFocus(mainForm);
					section = MAIN;
				}
				else {
					if (v2.size() > 0) {
						for (size_t i = 0; i < v2.size(); i++) {
							v2[i] = -v2[i] - 1;
						}
					}
					tguiDeleteWidget(formChooser);
					tguiDeleteWidget(formChooser2);
					tguiSetFocus(mainForm);
					section = MAIN;
				}
				formChooser->setInset(false);
			}
			else if (widget == stats) {
				tguiDeleteWidget(stats);
				tguiPop();
				tguiSetFocus(mainStats);
				section = MAIN;
			}
			else if (widget == fairy) {
				prepareForScreenGrab1();
				m_clear(black);
				tguiDraw();
				drawBufferToScreen(false);
				prepareForScreenGrab2();
				if (need_gold) {
					gold = 250;
					anotherDoDialogue(_t("Fairy: I see you're low on gold. I can help a bit...\n"), false, true, false);
				}
				else {
					anotherDoDialogue(_t("Fairy: You look like you're in need of supplies...\n"), false, true, false);
				}

				char imgname[1000];
				strcpy(imgname, getResource("media/fairy_shopkeeper.png"));

				tguiPush();

				if (heal_only) {
					int indexes[1] = { HEAL_INDEX };
					int costs[1] = { 50 };
					doShop("Fairy", imgname, 1, indexes, costs);
				}
				else {
					int indexes[5] = { HEAL_INDEX, CURE_INDEX, CURE2_INDEX, HOLY_WATER_INDEX, ELIXIR_INDEX };
					int costs[5] = { 50, 50, 100, 250, 250 };
					doShop("Fairy", imgname, 5, indexes, costs);
				}
				
				tguiPop();
	
				char goldS[15];
				sprintf(goldS, "%d", gold);
				mainGold->setString(goldS);
				
				tguiDeleteWidget(fairy);
				tguiSetFocus(mainItem);
				
				fairy_used = true;

				TGUIWidget *lwidget;

#if ((defined ALLEGRO_IPHONE || defined ALLEGRO_MACOSX) && !defined NO_GAMECENTER) || defined AMAZON
				lwidget = game_center;
#else
				lwidget = NULL;
#endif

				mainItem->set_left_widget(lwidget);
				mainMagic->set_left_widget(lwidget);
				mainForm->set_left_widget(lwidget);
				mainStats->set_left_widget(lwidget);
				if (dynamic_cast<MTextButton *>(mainSave))
					((MTextButton *)mainSave)->set_left_widget(lwidget);
				mainResume->set_left_widget(lwidget);
				mainQuit->set_left_widget(lwidget);
			}
#ifndef NO_GAMECENTER
#if defined ALLEGRO_IPHONE || defined ALLEGRO_MACOSX
			else if (game_center && widget == game_center)
			{
				al_stop_timer(logic_timer);
				al_stop_timer(draw_timer);
				showAchievements();
				al_start_timer(logic_timer);
				al_start_timer(draw_timer);
#ifdef ALLEGRO_IPHONE
				switchiOSKeyboardIn();
#endif
			}
#endif
#endif
#if defined AMAZON
			else if (game_center && widget == game_center) {
				al_stop_timer(logic_timer);
				al_stop_timer(draw_timer);
				show_achievements();
				al_start_timer(logic_timer);
				al_start_timer(draw_timer);
			}
#endif

			INPUT_EVENT ie = get_next_input_event();
			if (section == MAIN && ie.button2 == DOWN) {
				use_input_event();
				break_for_fade_after_draw = true;
				prepareForScreenGrab1();
				break;
			}
		}

		if (break_for_fade_after_draw || draw_counter > 0) {
			draw_counter = 0;
			if (!break_for_fade_after_draw) {
				set_target_backbuffer();
			}
			m_clear(black);
			tguiDraw();
			drawBufferToScreen();
			if (break_for_fade_after_draw) {
				break;
			}
			m_flip_display();
		}
	}

done:
	
	if (break_for_fade_after_draw) {
		break_for_fade_after_draw = false;
		prepareForScreenGrab2();
		fadeOut(black);
	}

	stopAllOmni();

	tguiDeleteActive();

	delete fullscreenRect;
	delete corner;
	delete mainFrame;
	delete mainItem;
	delete mainMagic;
	delete mainForm;
	delete mainStats;
	delete mainSave;
	delete mainResume;
	delete mainQuit;
	delete mainLevelUp;
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
	delete mainMusic;
#endif
	if (dndForm)
		delete dndForm;
	delete partyStats;
	delete mainTimeLabel;
	delete mainTime;
	delete mainGoldLabel;
	delete mainGold;
	delete stats;
	delete formChooser2;
	delete spellSelector;

	delete fullscreenRect2;
	delete fullscreenRect3;
	delete partySelectorTop;
	delete partySelectorTop2;
	delete itemSelector;
	delete equipChooser;
	equipPoints.clear();
	delete formChooser;
	formPoints.clear();

	if (formChooser_target) {
		delete formChooser_target;
	}
	
	delete fairy;
#if defined ALLEGRO_IPHONE || defined ALLEGRO_MACOSX
	delete game_center;
#elif defined AMAZON
	delete game_center;
#endif

	setMusicVolume(1);
	setAmbienceVolume(1);

	input->set(false, false, false, false, false, false, false);
	input->setDirection(dir);

	dpad_on();
	
	tguiPop();

	waitForRelease(4);
	waitForRelease(5);
	clear_input_events();

	in_pause = false;

	return ret;
}


void doMap(std::string startPlace, std::string prefix)
{
	dpad_off();

	if (area) {
		was_in_map = true;
	}

	in_map = true;
	
	playMusic("");
	playAmbience("");

	if (prefix == "map2") {
		playMusic("map2.ogg");
	}
	else {
		playMusic("map.ogg");
	}

	mapWidget = new MMap(startPlace, prefix);

	tguiSetParent(0);
	tguiAddWidget(mapWidget);
	tguiSetFocus(mapWidget);

	prepareForScreenGrab1();
	tguiDraw();
	drawBufferToScreen(false);
	prepareForScreenGrab2();
	fadeIn(black);

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
			// update gui
			std::string startingName = mapWidget->getSelected();

			TGUIWidget *widget = tguiUpdate();
			if (widget == mapWidget) {
				goto done;
			}
		}

		if (draw_counter > 0) {
			draw_counter = 0;
			set_target_backbuffer();
			m_clear(black);
			// Draw the GUI
			tguiDraw();
			drawBufferToScreen();
			m_flip_display();
		}
	}

done:

	dpad_on();

	tguiDeleteWidget(mapWidget);
	delete mapWidget;
	mapWidget = NULL;

	if (party[heroSpot]) {
		Input *obj_in = party[heroSpot]->getObject()->getInput();
		Input *i = getInput();
		if (obj_in && i) {
			i->setDirection(obj_in->getDescriptor().direction);
		}
	}

	in_map = false;

	runtime_start = tguiCurrentTimeMillis();
}

// return false on quit
void doShop(std::string name, const char *imageName, int nItems,
	int *indexes, int *costs)
{
#define DRAW \
	m_clear(black); \
	mDrawFrame(3, 3, BW-6, 60-6); \
	m_draw_bitmap(face, 5, 20-16, 0); \
	char s[100]; \
	sprintf(s, _t("%d gold"), gold); \
	mTextout_simple(s, BW-5-m_text_length(game_font, s), \
		20-m_text_height(game_font)-2, grey); \
	s[0] = 0; \
	if (!(shop->getSelected() == -1 && isel->getSelected() == -1)) { \
		int selected = shop->getSelected(); \
		if (tguiActiveWidget == shop && selected >= 0 && shop_inventory[selected].index >= 0) { \
			sprintf(s, _t("Cost: %d"), costs[selected]); \
		} \
		else { \
			strcpy(s, "-"); \
		} \
		if (tguiActiveWidget == shop && selected >= 0 && shop_inventory[selected].index >= 0) { \
			draw_item_desc(luaState, shop_inventory[selected].index, 4+m_get_bitmap_height(face)+2); \
			draw_item_users(partyBmps, luaState, shop_inventory[selected].index, 8); \
		} \
		else if (tguiActiveWidget == isel) { \
			int sel = isel->getSelected(); \
			if (sel >= 0 && inventory[sel].index >= 0) { \
				draw_item_desc(luaState, inventory[sel].index, 4+m_get_bitmap_height(face)+2); \
				draw_item_users(partyBmps, luaState, inventory[sel].index, 8); \
			} \
		} \
		mTextout_simple(s, BW-5-m_text_length(game_font, s), \
			22, grey); \
	} \
	tguiDraw();

	dpad_off();

	Inventory shop_inventory[MAX_INVENTORY];

	Input *input = getInput();
	int dir = input->getDirection();

	setMusicVolume(0.5f);

	for (int i = 0; i < MAX_INVENTORY; i++) {
		if (i < nItems) {
			shop_inventory[i].index = indexes[i];
			shop_inventory[i].quantity = 1;
		}
		else {
			shop_inventory[i].index = -1;
			shop_inventory[i].quantity = 0;
		}
	}

	// Main widgets
	MItemSelector *shop = new MItemSelector(62, 62+50-4, 0, 0, false);
	shop->setShop();
	shop->setInventory(shop_inventory);
	shop->setRaiseOnFocus(true);
	MItemSelector *isel = new MItemSelector(113, 113+50-5, 0, 0, false);
	isel->setShop();
	isel->setRaiseOnFocus(true);

	MBITMAP *face = m_load_bitmap(imageName);

	MBITMAP *partyBmps[MAX_PARTY];

	for (int i = 0; i < MAX_PARTY; i++) {
		if (party[i]) {
			partyBmps[i] = m_load_bitmap(getResource("objects/%s_front.png", party[i]->getName().c_str()));
		}
		else {
			partyBmps[i] = NULL;
		}
	}

	lua_State *luaState;
	luaState = lua_open();
	openLuaLibs(luaState);
	runGlobalScript(luaState);

	// Add widgets
	tguiSetParent(0);
	tguiAddWidget(shop);
	tguiAddWidget(isel);
	tguiSetFocus(shop);

	prepareForScreenGrab1();
	DRAW
	drawBufferToScreen(false);
	prepareForScreenGrab2();
	fadeIn(black);
	
	bool break_for_fade_after_draw = false;

	clear_input_events();

	for (;;) {
		al_wait_cond(wait_cond, wait_mutex);
		int tmp_counter = logic_counter;
		logic_counter = 0;
		if (tmp_counter > 10)
			tmp_counter = 1;
		while  (!break_for_fade_after_draw && tmp_counter > 0) {
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
			int drop_x, drop_y;
			if (widget == shop) {
				int sel = shop->getSelected();
				if (sel < 0) {
					break_for_fade_after_draw = true;
					prepareForScreenGrab1();
					break;
				}
				shop->getDropLocation(&drop_x, &drop_y);
				if (use_dpad || drop_y < 0 || drop_y >= 110) {
					// buy
					if (shop_inventory[sel].index >= 0) {
						char s1[100];
						char s2[100];
						char s3[100];
						strcpy(s1, "Buy how many?");
						sprintf(s2, "1: %d", costs[sel]);
						sprintf(s3, "5: %d", 5*costs[sel]);
						int choice;
						prepareForScreenGrab1();
						DRAW
						prepareForScreenGrab2();
						if ((choice = triple_prompt(s1, s2, s3, "Buy 1", "Buy 5", "Cancel", 2)) != 2) {
							int q = choice == 0 ? 1 : 5;
							int total_cost = costs[sel]*q;
							if (total_cost > gold) {
								playPreloadedSample("error.ogg");
								prepareForScreenGrab1();
								DRAW
								prepareForScreenGrab2();
								notify("You don't have", "enough gold to buy that!", "");
							}
							else {
								// check for room in inventory
								int slot = -1;
								int slot_part = -1;
								int tmp;
								tmp = findUsedInventorySlot(shop_inventory[sel].index, 0);
								while (tmp >= 0 && inventory[tmp].quantity >= 99) {
									tmp = findUsedInventorySlot(shop_inventory[sel].index, tmp+1);
								}
								if (tmp >= 0) {
									if (inventory[tmp].quantity+q <= 99) {
										slot = tmp;
									}
									else {
										slot_part = tmp;
									}
								}
								if (slot < 0) {
									tmp = findEmptyInventorySlot();
									if (tmp >= 0) {
										slot = tmp;
									}
								}
								if (slot < 0) {
									prepareForScreenGrab1();
									DRAW
									prepareForScreenGrab2();
									notify("You don't have room", "for that!", "");
								}
								else {
									gold -= total_cost;
									if (slot_part >= 0) {
										inventory[slot_part].index = shop_inventory[sel].index;
										q = q - (99 - inventory[slot_part].quantity);
										inventory[slot_part].quantity = 99;
									}
									inventory[slot].index = shop_inventory[sel].index;
									inventory[slot].quantity += q;
									loadPlayDestroy("chest.ogg");
									prepareForScreenGrab1();
									DRAW
									prepareForScreenGrab2();
									notify("", "Thank you!", "");
								}
							}
						}
					}
				}
			}
			else if (widget == isel) {
				int sel = isel->getSelected();
				if (sel < 0) {
					break_for_fade_after_draw = true;
					prepareForScreenGrab1();
					break;
				}
				isel->getDropLocation(&drop_x, &drop_y);
				if (use_dpad || drop_y < 0 || (drop_y >= 50 && drop_y < 110)) {
					// sell
					if (inventory[sel].index >= 0 &&
						inventory[sel].quantity > 0 && 
						items[inventory[sel].index].type != ITEM_TYPE_SPECIAL) {
						int sell_quantity = 1;
						char s1[100];
						char s2[100];
						if (inventory[sel].quantity > 1) {
							sprintf(s1, _t("Sell all %d?"), inventory[sel].quantity);
							prepareForScreenGrab1();
							DRAW
							prepareForScreenGrab2();
							if (prompt(s1, "", 0, 0)) {
								sell_quantity = inventory[sel].quantity;
							}
						}
						int total_cost = getResaleValue(inventory[sel].index) * sell_quantity;
						sprintf(s1, _t("Really sell %d"), sell_quantity);
						sprintf(s2, _t("for %d gold?"), total_cost);
						prepareForScreenGrab1();
						DRAW
						prepareForScreenGrab2();
						if (prompt(s1, s2, 0, 0)) {
							inventory[sel].quantity -= sell_quantity;
							if (inventory[sel].quantity <= 0) {
								inventory[sel].quantity = 0;
								inventory[sel].index = -1;
							}
							loadPlayDestroy("chest.ogg");
							prepareForScreenGrab1();
							DRAW
							prepareForScreenGrab2();
							notify("", "Thank you!", "");
							gold += total_cost;
						}
					}
					else {
						playPreloadedSample("error.ogg");
						if (items[inventory[sel].index].type == ITEM_TYPE_SPECIAL) {
							prepareForScreenGrab1();
							DRAW
							prepareForScreenGrab2();
							notify("You may", "need that...", "");
						}
					}
				}
			}
		}
		
		if (break_for_fade_after_draw || draw_counter > 0) {
			draw_counter = 0;
			if (!break_for_fade_after_draw) {
				set_target_backbuffer();
			}
			DRAW
			drawBufferToScreen();
			if (break_for_fade_after_draw) {
				break;
			}
			m_flip_display();
		}
	}

done:

	shop->setSelected(-1);
	isel->setSelected(-1);
	
	setMusicVolume(1);
	
	if (break_for_fade_after_draw) {
		break_for_fade_after_draw = false;
		prepareForScreenGrab2();
		fadeOut(black);
	}

	tguiDeleteWidget(shop);
	tguiDeleteWidget(isel);

	delete shop;
	delete isel;

	m_destroy_bitmap(face);

	for (int i = 0; i < MAX_PARTY; i++) {
		if (partyBmps[i])
			m_destroy_bitmap(partyBmps[i]);
	}

	lua_close(luaState);

	clear_input_events();

	input->setDirection(dir);
	input->set(false, false, false, false, false, false, false);
	
	dpad_on();

#undef DRAW
}


void into_the_sun(void)
{
	dpad_off();

	MBITMAP *bg = m_load_bitmap(getResource("media/sun.png"));

	MBITMAP *bg_locked = m_load_bitmap(getResource("media/sun.png"));
	m_lock_bitmap(bg_locked, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READONLY);

	AnimationSet *explosion = new_AnimationSet(getResource("media/explosion.png"));

	prepareForScreenGrab1();
	m_draw_bitmap(bg, 0, 0, 0);
	drawBufferToScreen(false);
	prepareForScreenGrab2();
	fadeIn(black);

	int count = 0;
	const int length = 3000;

	int sx = BW;
	int sy = BH;
	int dx = 99;
	int dy = 96;

	clear_input_events();

	while (true) {
		set_target_backbuffer();

		al_wait_cond(wait_cond, wait_mutex);
		int tmp_counter = logic_counter;
		logic_counter = 0;
		if (tmp_counter > 10)
			tmp_counter = 1;
		while  (tmp_counter > 0) {
			next_input_event_ready = true;

			tmp_counter--;
			
			count += LOGIC_MILLIS;

			if (is_close_pressed()) {
				do_close();
				close_pressed = false;
			}
			if (break_main_loop) {
				goto done;
			}
		}

		if (draw_counter > 0) {
			draw_counter = 0;
			m_draw_bitmap(bg, 0, 0, 0);
			if (count > length) {
				explosion->update(LOGIC_MILLIS);
				explosion->draw(dx-8, dy-8, 0);
				if (count > length+1400)  //explosion->getCurrentAnimation()->getLength())
					goto done;
			}
			else {
				int cx = sx + ((float)count/length)*(dx-sx);
				int cy = sy + ((float)count/length)*(dy-sy);
				MCOLOR c = m_get_pixel(bg_locked, cx, cy);
				unsigned char r, g, b, a;
				m_unmap_rgba(c, &r, &g, &b, &a);
				ALLEGRO_VERTEX v[1];
				v[0].x = cx;
				v[0].y = cy;
				v[0].z = 0;
				if (r+g+b < (255*3)/2) {
					v[0].color = white;
				}
				else {
					v[0].color = black;
				}
				m_draw_prim(v, NULL, NULL, 0, 1, ALLEGRO_PRIM_POINT_LIST);
			}

			drawBufferToScreen(false);
			m_flip_display();
		}
	}
done:
	set_target_backbuffer();
	m_draw_bitmap(bg, 0, 0, 0);
	drawBufferToScreen(false);
	m_flip_display();
	m_rest(5);
	
	prepareForScreenGrab1();
	m_draw_bitmap(bg, 0, 0, 0);
	drawBufferToScreen(false);
	prepareForScreenGrab2();
	fadeOut(black);

	m_destroy_bitmap(bg);
	m_unlock_bitmap(bg_locked);
	m_destroy_bitmap(bg_locked);
	delete explosion;

	dpad_on();
}


void credits(void)
{
	hide_mouse_cursor();

	dpad_off();

	enum SECTION_STATE {
		S_IN = 0,
		S_WAIT,
		S_OUT,
		S_COUNT
	};

	SECTION_STATE state = S_IN;
	int count = -1000;

	const int times[S_COUNT] = { 1000, 5000, 2000 };
	
	int section = 0;
	const int sections = 14;

	const char *section_text[sections][16] = {
		{
			"STORY",
			" ",
			"MICHAEL CROFT",
			"JACOB DAWID",
			"TRENT GAMBLIN",
			NULL
		},
		{
			"GRAPHICS",
			" ",
			"JACOB DAWID",
			"TRENT GAMBLIN",
			"TONY HUISMAN",
			"RAMON FREDERICK RABANG",
			"MATTHEW SMITH",
			"PAVEL SOUNTSOV",
			NULL
		},
		{
			"SOUND",
			" ",
			"TRENT GAMBLIN",
			"DAVID VITAS",
			NULL
		},
		{
			"MUSIC",
			" ",
			"JOaO BRUNO",
			"JACOB DAWID",
			"DAN \"DASSE\" EVERTSSON",
			"TRENT GAMBLIN",
			"DAVID VITAS",
			NULL
		},
		{
			"LEVEL DESIGN",
			" ",
			"TRENT GAMBLIN",
			NULL
		},
		{
			"PROGRAMMING",
			" ",
			"TRENT GAMBLIN",
			NULL
		},
		{
			"TESTING",
			" ",
			"ATTILIO CAROTENUTO",
			"ARMANDO M CERASOLI",
			"MICHAL CICHON",
			"MICHAEL CROFT",
			"KUMAR DARYANANI",
			"STEVEN PAGAN",
			NULL
		},
		{
			"TESTING",
			" ",
			"PATRICK DAVID",
			"MONCOMBLE FLORIAN",
			"CANTIDIO O FONTES",
			"TRENT GAMBLIN",
			"JOAO GONCALVES",
			NULL
		},
		{
			"TESTING",
			" ",
			"TONY HUISMAN",
			"SERGEY LATYSHEV",
			"FABIO NOBREGA",
			"JACOB NUTTING",
			"ELIAS PSCHERNIG",
			NULL
		},
		{
			"TRANSLATIONS",
			" ",
			"EDUARDO \"RYUKEN\" ARAUJO",
			"ATTILIO CAROTENUTO",
			"ARMANDO M CERASOLI",
			"MICHAL CICHON",
			"PATRICK DAVID",
			"THOMAS FAUST",
			NULL
		},
		{
			"TRANSLATIONS",
			" ",
			"MONCOMBLE FLORIAN",
			"CANTIDIO O FONTES",
			"JOAO GONCALVES",
			"LORENZ JOLLING",
			"KIMMYB",
			"SERGEY LATYSHEV",
			NULL
		},
		{
			"TRANSLATIONS",
			" ",
			"SERGIO M. A. PEREIRA",
			"ANDREA PROVASI",
			"IRAKLIS PSAROUDAKIS",
			"ELIAS PSCHERNIG",
			"SEAWASP MESH AND MUSIC",
			"SAMUEL VAN DER WAA",
			NULL
		},
		{
			"SPECIAL THANKS",
			" ",
			"TANYA GAMBLIN-BECKER",
			"NATASHA HUISMAN",
			"DANICA CRISTOBAL-RABANG",
			"WILL TICE",
			"TRIXIE",
			NULL
		},
		{
			"- A NOOSKEWL GAME -",
			NULL
		}
	};

	int line_counts[sections];

	for (int s = 0; s < sections; s++) {
		int n;
		for (n = 0; section_text[s][n] != NULL; n++)
			;
		line_counts[s] = n;
	}
	/* Point 1: target position (in place for reading
	 * point 2: start or current position
	 */
	std::vector<std::vector<std::vector<std::pair<MPoint, MPoint> > > > lettersPos;

	for (int s = 0; s < sections; s++) {
		int n = line_counts[s];
		int y = BH/2-(10*n)/2;
		std::vector<std::vector<std::pair<MPoint, MPoint> > > lines;
		for (int j = 0; j < n; j++) {
			int x = BW/2-(10*(int)strlen(section_text[s][j]))/2;
			std::vector<std::pair<MPoint, MPoint> > string_points;
			for (unsigned int i = 0; i < strlen(section_text[s][j]); i++) {
				MPoint start, dest;
				float angle = ((rand() % RAND_MAX) / (float)RAND_MAX) * M_PI * 2;
				start.x = BW/2+cos(angle)*200;
				start.y = BH/2+sin(angle)*200;
				dest.x = x;
				dest.y = y;
				std::pair<MPoint, MPoint> p = std::pair<MPoint, MPoint>(start, dest);
				string_points.push_back(p);
				x += 10;
			}
			lines.push_back(string_points);
			y += 10;
		}
		lettersPos.push_back(lines);
	}


	ALLEGRO_VERTEX *verts = new ALLEGRO_VERTEX[5000];

	for (int i = 0; i < 5000; i++)
		verts[i].z = 0;

	int vcount;

	float offset = 0;
	float amplitude = 0;

	playMusic("credits.ogg");

	MBITMAP *bg = m_load_bitmap(getResource("media/credit_bg.png"));
	MBITMAP *font = m_load_bitmap(getResource("media/credit_font.png"));

	long start = tguiCurrentTimeMillis();

	clear_input_events();

	bool break_for_fade_after_draw = false;

	for (;;) {
		if (is_close_pressed()) {
			do_close();
			close_pressed = false;
		}
		if (break_main_loop) {
			goto done;
		}
		long now = tguiCurrentTimeMillis();
		int step = (int)(now - start);
		start = now;
		offset += 0.01f * step;

		if (!break_for_fade_after_draw) {
			set_target_backbuffer();
		}

		m_draw_bitmap(bg, 0, 0, 0);

		if (count < 0) {
			count += step;
			if (count > 0)
				count = 0;
		}
		else {
			count += step;
			/* draw section */
			if (state == S_IN) {
				vcount = 0;
				float p = (float)count/times[0];
				if (p > 1) p = 1;
				for (int line = 0; line < line_counts[section]; line++) {
					for (int c = 0; c < (int)lettersPos[section][line].size(); c++) {
						int letter = section_text[section][line][c];
						int dest_x = lettersPos[section][line][c].second.x;
						int dest_y = lettersPos[section][line][c].second.y;
						int start_x = lettersPos[section][line][c].first.x;
						int start_y = lettersPos[section][line][c].first.y;
						int dx = dest_x - start_x;
						int dy = dest_y - start_y;
						int x = (dx*p)+start_x;
						int y = (dy*p)+start_y;
						int u, v;
						ALLEGRO_COLOR col = white;
						if (letter == ' ') {
							col = al_map_rgba(0, 0, 0, 0);
							u = 0;
						}
						else if (letter == '-') {
							u = 260;
						}
						else if (letter == 'a') {
							u = 270;
						}
						else if (letter == '"') {
							u = 280;
						}
						else if (letter == '.') {
							u = 290;
						}
						else
							u = (letter-'A')*10;
						v = 1;
						u += 1 + (u / 10)*2;
						verts[vcount].x = x;
						verts[vcount].y = y;
						verts[vcount].u = u;
						verts[vcount].v = v;
						verts[vcount].color = col;
						vcount++;
						verts[vcount].x = x;
						verts[vcount].y = y+10;
						verts[vcount].u = u;
						verts[vcount].v = v+10;
						verts[vcount].color = col;
						vcount++;
						verts[vcount].x = x+10;
						verts[vcount].y = y;
						verts[vcount].u = u+10;
						verts[vcount].v = v;
						verts[vcount].color = col;
						vcount++;
						verts[vcount].x = x;
						verts[vcount].y = y+10;
						verts[vcount].u = u;
						verts[vcount].v = v+10;
						verts[vcount].color = col;
						vcount++;
						verts[vcount].x = x+10;
						verts[vcount].y = y;
						verts[vcount].u = u+10;
						verts[vcount].v = v;
						verts[vcount].color = col;
						vcount++;
						verts[vcount].x = x+10;
						verts[vcount].y = y+10;
						verts[vcount].u = u+10;
						verts[vcount].v = v+10;
						verts[vcount].color = col;
						vcount++;
					}
				}

				m_draw_prim(verts, 0, font, 0, vcount, ALLEGRO_PRIM_TRIANGLE_LIST);
				if (count >= times[0]) {
					state = (SECTION_STATE)((int)state+1);
					count = 0;
				}
			}
			else if (state == S_WAIT) {
				vcount = 0;
				for (int line = 0; line < line_counts[section]; line++) {
					for (int c = 0; c < (int)lettersPos[section][line].size(); c++) {
						int letter = section_text[section][line][c];
						int x = lettersPos[section][line][c].second.x;
						int y = lettersPos[section][line][c].second.y;
						int u, v;
						ALLEGRO_COLOR col = white;
						if (letter == ' ') {
							col = al_map_rgba(0, 0, 0, 0);
							u = 0;
						}
						else if (letter == '-') {
							u = 260;
						}
						else if (letter == 'a') {
							u = 270;
						}
						else if (letter == '"') {
							u = 280;
						}
						else if (letter == '.') {
							u = 290;
						}
						else
							u = (letter-'A')*10;
						v = 1;
						u += 1 + (u / 10)*2;
						verts[vcount].x = x;
						verts[vcount].y = y;
						verts[vcount].u = u;
						verts[vcount].v = v;
						verts[vcount].color = col;
						vcount++;
						verts[vcount].x = x;
						verts[vcount].y = y+10;
						verts[vcount].u = u;
						verts[vcount].v = v+10;
						verts[vcount].color = col;
						vcount++;
						verts[vcount].x = x+10;
						verts[vcount].y = y;
						verts[vcount].u = u+10;
						verts[vcount].v = v;
						verts[vcount].color = col;
						vcount++;
						verts[vcount].x = x;
						verts[vcount].y = y+10;
						verts[vcount].u = u;
						verts[vcount].v = v+10;
						verts[vcount].color = col;
						vcount++;
						verts[vcount].x = x+10;
						verts[vcount].y = y;
						verts[vcount].u = u+10;
						verts[vcount].v = v;
						verts[vcount].color = col;
						vcount++;
						verts[vcount].x = x+10;
						verts[vcount].y = y+10;
						verts[vcount].u = u+10;
						verts[vcount].v = v+10;
						verts[vcount].color = col;
						vcount++;
					}
				}

				m_draw_prim(verts, 0, font, 0, vcount, ALLEGRO_PRIM_TRIANGLE_LIST);
				if (count >= times[1]) {
					state = (SECTION_STATE)((int)state+1);
					count = 0;
					offset = 0;
					amplitude = 0;
				}
			}
			else {
				vcount = 0;
				amplitude += 0.05f * step;
				for (int line = 0; line < line_counts[section]; line++) {
					float first = lettersPos[section][line][0].second.x;
					float last = lettersPos[section][line][strlen(section_text[section][line])-1].second.x;
					int width = last - first;
					for (int c = 0; c < (int)lettersPos[section][line].size(); c++) {
						int letter = section_text[section][line][c];
						int x = lettersPos[section][line][c].second.x;
						float y = lettersPos[section][line][c].second.y;
						y += sin(((x-first)/width)*2+offset)*amplitude;
						int alpha = MIN(255, MAX(0, (float)(2000-count)/2000.0*255));
						MCOLOR alpha_white = m_map_rgba(
							alpha, alpha, alpha, alpha
						);
						int u, v;
						ALLEGRO_COLOR col = alpha_white;
						if (letter == ' ') {
							col = al_map_rgba(0, 0, 0, 0);
							u = 0;
						}
						else if (letter == '-') {
							u = 260;
						}
						else if (letter == 'a') {
							u = 270;
						}
						else if (letter == '"') {
							u = 280;
						}
						else if (letter == '.') {
							u = 290;
						}
						else
							u = (letter-'A')*10;
						v = 1;
						u += 1 + (u / 10)*2;
						verts[vcount].x = x;
						verts[vcount].y = y;
						verts[vcount].u = u;
						verts[vcount].v = v;
						verts[vcount].color = col;
						vcount++;
						verts[vcount].x = x;
						verts[vcount].y = y+10;
						verts[vcount].u = u;
						verts[vcount].v = v+10;
						verts[vcount].color = col;
						vcount++;
						verts[vcount].x = x+10;
						verts[vcount].y = y;
						verts[vcount].u = u+10;
						verts[vcount].v = v;
						verts[vcount].color = col;
						vcount++;
						verts[vcount].x = x;
						verts[vcount].y = y+10;
						verts[vcount].u = u;
						verts[vcount].v = v+10;
						verts[vcount].color = col;
						vcount++;
						verts[vcount].x = x+10;
						verts[vcount].y = y;
						verts[vcount].u = u+10;
						verts[vcount].v = v;
						verts[vcount].color = col;
						vcount++;
						verts[vcount].x = x+10;
						verts[vcount].y = y+10;
						verts[vcount].u = u+10;
						verts[vcount].v = v+10;
						verts[vcount].color = col;
						vcount++;
					}
				}

				m_draw_prim(verts, 0, font, 0, vcount, ALLEGRO_PRIM_TRIANGLE_LIST);
				if (count >= times[2]) {
					state = S_IN;
					count = 0;
					section++;
				}
			}
		}

		drawBufferToScreen(false);

		if (break_for_fade_after_draw) {
			break;
		}

		if (section >= sections) {
			section--;
			break_for_fade_after_draw = true;
			prepareForScreenGrab1();
			continue;
		}
		m_flip_display();
		m_rest(0.001);
	}

	if (break_for_fade_after_draw) {
		break_for_fade_after_draw = false;
		prepareForScreenGrab2();
		fadeOut(black);
	}

done:
	
	m_destroy_bitmap(bg);

	if (break_main_loop) {
		goto done2;
	}

	{
		/* Now roll the Indiegogo sponsor credits! */

		const char *lines[] = {
			"A VERY SPECIAL",
			"SHOUT OUT TO OUR",
			"INDIEGOGO SPONSORS",
			"",
			"",
			"",
			"TREVOR BEKOLAY",
			"WILLIAM HENSLEY",
			"WILLIAMGEORGEBROWNE",
			"CHRISTOPHER VALORE",
			"GAMINGONLINUX.COM",
			"JAIME ROBERTSON",
			"GIACOMO RUSSO",
			"MICHAEL TERVOORT",
			"PAWEL ZUBRBYCKI",
			"JOSH GUNDERSON",
			"JONAH LIBSTER",
			"CHRISTOPHER WEIGLE",
			"JAN SIMEK",
			"BIGNIC",
			"PAUL PRIDHAM",
			"THOMAS FJELLSTROM",
			"ELIAS PSCHERNIG",
			"NAMIDA",
			"XERANAS",
			"RUSTY.BOLTS",
			"FERNANDO",
			"MATTHEW BROOKS",
			"TODD B.",
			"GELE SEBASTIEN",
			"J.W. GLAZE",
			"DABASCHT",
			"SIMON DWYER",
			"JOSH BUSH",
			"MATEHACKERS",
			"MATTHEW.KEE",
			"GUSTAV TIGER",
			"JENNIFER ROGER",
			"BERNARDO COMPAGNONI",
			"STEPHANE GUILLOU",
			"HARD ROCK",
			"JON RAFKIND",
			"PHILIPPE",
			"MARK SMITH",
			"O.KORFF",
			"LEFTIUM",
			NULL
		};

		int nlines = 0;
		for (; lines[nlines] != NULL; nlines++)
			;

		int scroll_size = nlines*10 + BH*1.2;
		float scroll_amount = 0;

		long start = tguiCurrentTimeMillis();

		clear_input_events();

		for (;;) {
			if (is_close_pressed()) {
				do_close();
				close_pressed = false;
			}
			if (break_main_loop) {
				goto done2;
			}
			long now = tguiCurrentTimeMillis();
			int step = (int)(now - start);
			start = now;

			scroll_amount += 0.01 * step;

			vcount = 0;
			int y = BH-scroll_amount;
			for (int line = 0; line < nlines; line++, y += 10) {
				int x = BW/2 - (int)strlen(lines[line])*10/2;
				for (int c = 0; lines[line][c]; c++, x += 10) {
					int letter = lines[line][c];
					int u, v;
					ALLEGRO_COLOR col = white;
					if (letter == ' ') {
						col = al_map_rgba(0, 0, 0, 0);
						u = 0;
					}
					else if (letter == '-') {
						u = 260;
					}
					else if (letter == 'a') {
						u = 270;
					}
					else if (letter == '"') {
						u = 280;
					}
					else if (letter == '.') {
						u = 290;
					}
					else
						u = (letter-'A')*10;
					v = 1;
					u += 1 + (u / 10)*2;
					verts[vcount].x = x;
					verts[vcount].y = y;
					verts[vcount].u = u;
					verts[vcount].v = v;
					verts[vcount].color = col;
					vcount++;
					verts[vcount].x = x;
					verts[vcount].y = y+10;
					verts[vcount].u = u;
					verts[vcount].v = v+10;
					verts[vcount].color = col;
					vcount++;
					verts[vcount].x = x+10;
					verts[vcount].y = y;
					verts[vcount].u = u+10;
					verts[vcount].v = v;
					verts[vcount].color = col;
					vcount++;
					verts[vcount].x = x;
					verts[vcount].y = y+10;
					verts[vcount].u = u;
					verts[vcount].v = v+10;
					verts[vcount].color = col;
					vcount++;
					verts[vcount].x = x+10;
					verts[vcount].y = y;
					verts[vcount].u = u+10;
					verts[vcount].v = v;
					verts[vcount].color = col;
					vcount++;
					verts[vcount].x = x+10;
					verts[vcount].y = y+10;
					verts[vcount].u = u+10;
					verts[vcount].v = v+10;
					verts[vcount].color = col;
					vcount++;
				}
			}
			
			if (!break_for_fade_after_draw) {
				set_target_backbuffer();
			}

			al_clear_to_color(al_map_rgb_f(0, 0, 0));

			m_draw_prim(verts, 0, font, 0, vcount, ALLEGRO_PRIM_TRIANGLE_LIST);

			drawBufferToScreen(false);
			
			if (break_for_fade_after_draw) {
				break;
			}
			
			if (scroll_amount > scroll_size) {
				break_for_fade_after_draw = true;
				prepareForScreenGrab1();
				continue;
			}

			m_flip_display();
			m_rest(0.001);
		}
	}
	
	if (break_for_fade_after_draw) {
		break_for_fade_after_draw = false;
		prepareForScreenGrab2();
		fadeOut(black);
	}
	
done2:
	delete[] verts;

	m_destroy_bitmap(font);

	dpad_on();

	show_mouse_cursor();
}


void choose_savestate_old(std::string caption, bool paused, bool autosave, bool (*callback)(int n, bool exists, void *data), void *data)
{
	dpad_off();

	SaveStateInfo infos[10];

	try {
		for (int i = 0; i < 10; i++) {
			getSaveStateInfo(i, infos[i], autosave);
		}
	}
	catch (ReadError e) {
		(void)e;
		prepareForScreenGrab1();
		m_clear(black);
		prepareForScreenGrab2();
		notify("Corrupt save state found.", "Delete it and", "Try again...");
		callback(-1, false, data);
		return;
	}

	MIcon *bg = new MIcon(0, 0, std::string(getResource("media/savebg.png")), white, false, "", false, false, false);

	MTextButton *buttons[10];
	for (int i = 0; i < 10; i++) {
		char buf[100];
		if (infos[i].exp == 0 && infos[i].gold == 0 && infos[i].time == 0) {
			sprintf(buf, "(Empty)");
		}
		else {
			sprintf(buf, "E:%d G:%d, %s",
				infos[i].exp,
				infos[i].gold,
				getTimeString(infos[i].time).c_str()
			);
		}
		buttons[i] = new MTextButton(10, 18+i*14, std::string(buf), false);
	}

	MLabel *lcaption = new MLabel(0, 1, caption, m_map_rgb(220, 220, 220));

	tguiPush();

	tguiSetParent(0);
	tguiAddWidget(bg);
	tguiSetParent(bg);
	tguiAddWidget(lcaption);
	for (int i = 0; i < 10; i++) {
		tguiAddWidget(buttons[i]);
	}
	tguiSetFocus(buttons[0]);

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

			TGUIWidget *widget = tguiUpdate();

			for (int i = 0; i < 10; i++) {
				if (widget == buttons[i]) {
					prepareForScreenGrab1();
					tguiDraw();
					drawBufferToScreen(false);
					prepareForScreenGrab2();
					if (callback(i, (infos[i].exp == 0 && infos[i].gold == 0 && infos[i].time == 0) ? false : true, data)) {
						goto done;
					}
				}
			}
			
			INPUT_EVENT ie = get_next_input_event();
			if (ie.button2 == DOWN || iphone_shaken(0.1)) {
				use_input_event();
				playPreloadedSample("select.ogg");
				iphone_clear_shaken();
				m_rest(0.5);
				callback(-1, false, data);
				goto done;
			}

		}

		if (draw_counter > 0) {
			draw_counter = 0;
			set_target_backbuffer();

			tguiDraw();
			
			drawBufferToScreen();
			m_flip_display();
		}
	}
done:
	tguiDeleteWidget(bg);
	delete bg;

	for (int i = 0; i < 10; i++) {
		delete buttons[i];
	}
	delete lcaption;

	tguiPop();

	dpad_on();

	clear_input_events();
}

static SaveStateInfo save_info[10];
static MScrollingList *save_list;
static SaveStateInfo auto_info[10];
static MScrollingList *auto_list;

static void load_save_info(void)
{
	for (int i = 0; i < 10; i++) {
		getSaveStateInfo(i, save_info[i], false);
		getSaveStateInfo(i, auto_info[i], true);
	}
	
	std::vector<std::string> save_strings;
	std::vector<std::string> auto_strings;

	for (int i = 0; i < 10; i++) {
		char buf[100];
		if (!(save_info[i].exp == 0 && save_info[i].gold == 0 && save_info[i].time == 0)) {
			sprintf(buf, "E:%d G:%d, %s",
				save_info[i].exp,
				save_info[i].gold,
				getTimeString(save_info[i].time).c_str()
			);
			save_strings.push_back(std::string(buf));
		}
		if (!(auto_info[i].exp == 0 && auto_info[i].gold == 0 && auto_info[i].time == 0)) {
			sprintf(buf, "E:%d G:%d, %s",
				auto_info[i].exp,
				auto_info[i].gold,
				getTimeString(auto_info[i].time).c_str()
			);
			auto_strings.push_back(std::string(buf));
		}
	}

	save_list->setItems(save_strings);
	auto_list->setItems(auto_strings);

	save_list->reset();
	auto_list->reset();
}

static int get_num(const char *name, int n)
{
	int i, j = 0;
	for (i = 0; i < 10; i++) {
		SaveStateInfo *ssi;
		if (name[0] == '\0') {
			ssi = save_info;
		}
		else {
			ssi = auto_info;
		}
		if (!(ssi[i].exp == 0 && ssi[i].gold == 0 && ssi[i].time == 0)) {
			if (j == n)
				break;
			j++;
		}
	}
	return i;
}

static void trash(const char *name, int n)
{
	if (prompt("Really delete", "this save state?", 0, 0)) {
		int i = get_num(name, n);
		if (i < 10) {
			char filename[1000];
			strcpy(filename, getUserResource("%s%d.save", name, i));
			delete_file(filename);
		}
	}
	load_save_info();
}

static void trash_save(int n)
{
	char s[10];
	strcpy(s, "");
	trash(s, n);
}

static void trash_auto(int n)
{
	char s[10];
	strcpy(s, "auto");
	trash(s, n);
}

static void show_savestate_info_callback(int n, const void *data)
{
	const char *p = (const char *)data;
	char buf[100];

	int i = get_num(p, n);
	sprintf(buf, "%s%d", p, i);

	showSaveStateInfo(buf);
}

struct CHOOSE_SAVESTATE_INFO {
	int num;
	bool existing;
	bool isAuto;
};

static bool choose_copy_state(int n, bool exists, void *data)
{
	(void)data;

	prepareForScreenGrab1();
	m_clear(black);
	prepareForScreenGrab2();

	if (n >= 0) {
		if (exists) {
#ifndef NO_PHYSFS
			al_set_standard_file_interface();
#endif
			int sz;
			unsigned char *bytes = slurp_file(getUserResource("%d.save", n), &sz);
#ifndef NO_PHYSFS
			al_set_physfs_file_interface();
#endif
			char *encoded = create_url(bytes, sz);
			al_set_clipboard_text(display, encoded);
			notify("", "Save state copied.", "");
		}
		else {
			notify("Can't copy", "empty save state!", "");
		}
	}
	else {
		notify("", "Cancelled", "");
	}
	
	return true;
}

static bool choose_paste_state(int n, bool exists, void *data)
{
	CHOOSE_SAVESTATE_INFO *i = (CHOOSE_SAVESTATE_INFO *)data;
	
	if (n >= 0) {
		bool saveit = true;
		if (exists) {
			if (!prompt("Really paste over", "this state?", 0, 0))
				saveit = false;
		}
		if (saveit) {
			char *buf;
			if ((buf = al_get_clipboard_text(display)) != NULL) {
				save_url(getUserResource("%d.save", n), buf);
				if (prompt("Run this game now?", "", 0, 1)) {
					i->num = n;
					i->existing = true;
					i->isAuto = false;
				}
				else {
					load_save_info();
				}
			}
			else {
				notify("", "Nothing in the pasteboard.", "");
			}
		}
	}
	else {
		notify("", "Cancelled.", "");
	}
	return true;
}

void choose_savestate(int *num, bool *existing, bool *isAuto)
{
	dpad_off();

	std::string prevMusicName = musicName;
	playMusic("loadsave.ogg");

	const int LIST_H = 100;

	MBITMAP *trashcan = m_load_bitmap(getResource("media/trashcan.png"));
	MIcon *trash_icon = new MIcon(
		BW-m_get_bitmap_width(trashcan)-5,
		BH-m_get_bitmap_height(trashcan)-5,
		std::string(getResource("media/trashcan.png")),
		white, false, "", false, false, false);

	MFrame *frame = new MFrame(10, 35, 220, 110);
	save_list = new MScrollingList(20, 45, 200, LIST_H, trash_save, BW-m_get_bitmap_width(trashcan)/2-5, BH-m_get_bitmap_height(trashcan)/2-5, show_savestate_info_callback, "");
	auto_list = new MScrollingList(20, 45, 200, LIST_H, trash_auto, BW-m_get_bitmap_width(trashcan)/2-5, BH-m_get_bitmap_height(trashcan)/2-5, show_savestate_info_callback, "auto");
	MTab *save_tab = new MTab("Save", 10, 20);
	int xx2 = 10 + m_text_length(game_font, _t("Save")) + 6;
	MTab *auto_tab = new MTab("Auto", xx2, 20);

	int xx3 = xx2 + m_text_length(game_font, _t("Auto")) + 6;
	MTab *copypaste_tab = new MTab("Copy & Paste", xx3, 20);

	MTextButton *new_game_button = new MTextButton(BW-m_text_length(game_font, _t("New Game..."))-20, 12-m_text_height(game_font)/2, "New Game...");

	MTextButton *copy_button = new MTextButton(30, 50, "Copy Save");
	MTextButton *paste_button = new MTextButton(30, 70, "Paste Save");

	load_save_info();

	tguiSetParent(0);
	tguiAddWidget(frame);
	tguiAddWidget(save_list);
	tguiAddWidget(save_tab);
	tguiAddWidget(auto_tab);

	tguiAddWidget(copypaste_tab);

	tguiAddWidget(new_game_button);
	tguiAddWidget(trash_icon);
	tguiSetFocus(new_game_button);

	save_tab->setSelected(true);
	auto_tab->setSelected(false);
	copypaste_tab->setSelected(false);
	int on = 0;

	tguiLowerWidget(save_tab);
	tguiLowerWidget(frame);
			
	prepareForScreenGrab1();
	m_clear(black);
	tguiDraw();
	drawBufferToScreen(false);
	prepareForScreenGrab2();
	fadeIn(black);

	bool break_for_fade_after_draw = false;

	clear_input_events();

	for (;;) {
		al_wait_cond(wait_cond, wait_mutex);
		int tmp_counter = logic_counter;
		logic_counter = 0;
		if (tmp_counter > 10)
			tmp_counter = 1;
		while  (!break_for_fade_after_draw && tmp_counter > 0) {
			next_input_event_ready = true;

			tmp_counter--;
			if (is_close_pressed()) {
				do_close();
				close_pressed = false;
			}
			if (break_main_loop) {
				*num = -1;
				goto done;
			}

			TGUIWidget *widget = tguiUpdate();

			if (widget == save_tab) {
				if (on != 0) {
					save_tab->setSelected(true);
					auto_tab->setSelected(false);
					copypaste_tab->setSelected(false);
					save_list->reset();
					tguiSetParent(0);
					tguiAddWidget(save_list);
					if (on == 1)
						tguiDeleteWidget(auto_list);
					else {
						tguiDeleteWidget(copy_button);
						tguiDeleteWidget(paste_button);
					}
					tguiLowerWidget(save_list);
					tguiLowerWidget(save_tab);
					tguiLowerWidget(frame);
					tguiSetFocus(save_tab);
					on = 0;
				}
			}
			else if (widget == auto_tab) {
				if (on != 1) {
					auto_tab->setSelected(true);
					save_tab->setSelected(false);
					copypaste_tab->setSelected(false);
					auto_list->reset();
					tguiSetParent(0);
					tguiAddWidget(auto_list);
					if (on == 0)
						tguiDeleteWidget(save_list);
					else {
						tguiDeleteWidget(copy_button);
						tguiDeleteWidget(paste_button);
					}
					tguiLowerWidget(auto_list);
					tguiLowerWidget(auto_tab);
					tguiLowerWidget(frame);
					tguiSetFocus(auto_tab);
					on = 1;
				}
			}
			else if (widget == copypaste_tab) {
				if (on != 2) {
					save_tab->setSelected(false);
					auto_tab->setSelected(false);
					copypaste_tab->setSelected(true);
					tguiSetParent(0);
					tguiAddWidget(copy_button);
					tguiAddWidget(paste_button);
					if (on == 0)
						tguiDeleteWidget(save_list);
					else
						tguiDeleteWidget(auto_list);
					tguiLowerWidget(paste_button);
					tguiLowerWidget(copy_button);
					tguiLowerWidget(copypaste_tab);
					tguiLowerWidget(frame);
					tguiSetFocus(copy_button);
					on = 2;
				}
			}
			else
			if (widget == save_list) {
				int sel = save_list->getSelected();
				if (sel >= 0) {
					int i, j = 0;
					for (i = 0; i < 10; i++) {
						if (!(save_info[i].exp == 0 && save_info[i].gold == 0 && save_info[i].time == 0)) {
							if (j == sel)
								break;
							j++;
						}
					}
					if (i < 10) {
						*num = i;
						*existing = true;
						*isAuto = false;
						break_for_fade_after_draw = true;
						prepareForScreenGrab1();
						break;
					}
				}
				else {
					*num = -1;
					break_for_fade_after_draw = true;
					prepareForScreenGrab1();
					break;
				}
			}
			else if (widget == auto_list) {
				int sel = auto_list->getSelected();
				if (sel >= 0) {
					int i, j = 0;
					for (i = 0; i < 10; i++) {
						if (!(auto_info[i].exp == 0 && auto_info[i].gold == 0 && auto_info[i].time == 0)) {
							if (j == sel)
								break;
							j++;
						}
					}
					if (i < 10) {
						*num = i;
						*existing = true;
						*isAuto = true;
						break_for_fade_after_draw = true;
						prepareForScreenGrab1();
						break;
					}
				}
				else {
					*num = -1;
					break_for_fade_after_draw = true;
					prepareForScreenGrab1();
					break;
				}
			}
			else if (widget == new_game_button) {
				*num = 0;
				*existing = false;
				break_for_fade_after_draw = true;
				prepareForScreenGrab1();
				break;
			}
			else if (widget == copy_button) {
				choose_savestate_old("Choose state to copy", true, false, choose_copy_state, NULL);
			}
			else if (widget == paste_button) {
				CHOOSE_SAVESTATE_INFO i;
				memset(&i, 0, sizeof(i));
				i.num = -1;
				choose_savestate_old("Choose state to paste into", true, false, choose_paste_state, &i);
				if (i.num >= 0) {
					*num = i.num;
					*existing = i.existing;
					*isAuto = i.isAuto;
					break_for_fade_after_draw = true;
					prepareForScreenGrab1();
					break;
				}
			}

			InputDescriptor id = getInput()->getDescriptor();

			if (iphone_shaken(0.1) || id.button2 == DOWN) {
				iphone_clear_shaken();
				*num = -1;
				break_for_fade_after_draw = true;
				prepareForScreenGrab1();
				break;
			}
		}


		if (break_for_fade_after_draw || draw_counter > 0) {
			draw_counter = 0;

			if (!break_for_fade_after_draw) {
				set_target_backbuffer();
			}

			m_clear(black);
			tguiDraw();

			drawBufferToScreen();

			if (break_for_fade_after_draw) {
				break;
			}

			m_flip_display();
		}
	}
done:
	playMusic(musicName);
	
	if (break_for_fade_after_draw) {
		break_for_fade_after_draw = false;
		prepareForScreenGrab2();
		fadeOut(black);
	}

	tguiDeleteWidget(frame);
	tguiDeleteWidget(save_tab);
	tguiDeleteWidget(new_game_button);
	tguiDeleteWidget(save_list);
	tguiDeleteWidget(trash_icon);

	tguiDeleteWidget(copypaste_tab);
	tguiDeleteWidget(copy_button);
	tguiDeleteWidget(paste_button);
	
	tguiDeleteWidget(auto_list);
	tguiDeleteWidget(auto_tab);

	delete frame;
	delete save_tab;
	delete new_game_button;
	delete trash_icon;
	delete save_list;

	delete copy_button;
	delete paste_button;
	delete copypaste_tab;

	delete auto_tab;
	delete auto_list;

	m_destroy_bitmap(trashcan);

	dpad_on();
}

bool config_menu(bool start_on_fullscreen)
{
	dpad_off();

	pause_f_to_toggle_fullscreen = true;

	tguiPush();

	MIcon *bg = new MIcon(0, 0, getResource("media/options_bg.png"), al_map_rgba_f(0.25f, 0.25f, 0.25f, 1.0f));

	int xx = 10;
	int y = 10;

	std::vector<std::string> music_choices;
	music_choices.push_back("{027} Sound on");
	music_choices.push_back("{027} SFX only");
	music_choices.push_back("{027} Silence");
	MSingleToggle *sound_toggle = new MSingleToggle(xx, y, music_choices);
	bool music_on = config.getMusicVolume();
	bool sound_on = config.getSFXVolume();
	if (music_on && sound_on) sound_toggle->setSelected(0);
	else if (sound_on) sound_toggle->setSelected(1);
	else sound_toggle->setSelected(2);
	int sound_start = sound_toggle->getSelected();
	
	y += 13;

#if defined ALLEGRO_IPHONE ||  defined ALLEGRO_ANDROID
	MSingleToggle *input_toggle = NULL;
	if (!isAndroidConsole()) {
		std::vector<std::string> input_choices;
		input_choices.push_back("{027} Tap-and-go");
		input_choices.push_back("{027} Hybrid input 1");
		input_choices.push_back("{027} Hybrid input 2");
		input_choices.push_back("{027} Total D-Pad 1");
		input_choices.push_back("{027} Total D-Pad 2");
		input_toggle = new MSingleToggle(xx, y, input_choices);
		input_toggle->setSelected(config.getDpadType());
		y += 13;
	}
#endif

	std::vector<std::string> difficulty_choices;
	difficulty_choices.push_back("{027} Easy difficulty");
	difficulty_choices.push_back("{027} Normal difficulty");
	difficulty_choices.push_back("{027} Hard difficulty!");
	MSingleToggle *difficulty_toggle = new MSingleToggle(xx, y, difficulty_choices);
	int curr_difficulty = config.getDifficulty();
	difficulty_toggle->setSelected(curr_difficulty);
	y += 13;

	MSingleToggle *tuning_toggle = NULL;
	int curr_tuning = 2;
	if (!isAndroidConsole()) {
		std::vector<std::string> tuning_choices;
		tuning_choices.push_back("{027} Tune for battery life");
		tuning_choices.push_back("{027} Balanced tuning");
		tuning_choices.push_back("{027} Tune for performance");
		tuning_toggle = new MSingleToggle(xx, y, tuning_choices);
		curr_tuning = config.getTuning();
		tuning_toggle->setSelected(curr_tuning);

		y += 13;
	}

#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
	MSingleToggle *swap_buttons_toggle = NULL;
	if (!isAndroidConsole()) {
		std::vector<std::string> swap_buttons_choices;
		swap_buttons_choices.push_back("{027} Normal buttons (select, cancel)");
		swap_buttons_choices.push_back("{027} Swap buttons (cancel, select)");
		swap_buttons_toggle = new MSingleToggle(xx, y, swap_buttons_choices);
		int curr_swap_buttons = config.getSwapButtons();
		swap_buttons_toggle->setSelected(curr_swap_buttons);

		y += 13;
	}
#endif

	#define aspect_real_to_option(c) (c == ASPECT_FILL_SCREEN ? 1 : (c == ASPECT_MAINTAIN_RATIO ? 2 : 0))
	#define aspect_option_to_real(c) (c == 0 ? ASPECT_INTEGER : (c == 1 ? ASPECT_FILL_SCREEN : ASPECT_MAINTAIN_RATIO))

	std::vector<std::string> aspect_choices;
	aspect_choices.push_back("{027} Integer scaling");
	aspect_choices.push_back("{027} Fill the screen");
	aspect_choices.push_back("{027} Maintain aspect ratio");
	MSingleToggle *aspect_toggle = new MSingleToggle(xx, y, aspect_choices);
	int curr_aspect = config.getMaintainAspectRatio();
	aspect_toggle->setSelected(aspect_real_to_option(curr_aspect));
	y += 13;

#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_ANDROID && !defined ALLEGRO_RASPBERRYPI
	std::vector<std::string> fullscreen_choices;
	fullscreen_choices.push_back("{027} Draw in a window");
	fullscreen_choices.push_back("{027} Draw fullscreen");
	MSingleToggle *fullscreen_toggle = new MSingleToggle(xx, y, fullscreen_choices);
	int start_fullscreen = config.getWantedGraphicsMode()->fullscreen;
	fullscreen_toggle->setSelected(start_fullscreen);
	y += 13;
#endif
	
	std::vector<std::string> alwaysCenter_choices;
	alwaysCenter_choices.push_back("{027} View: Centered");
	alwaysCenter_choices.push_back("{027} View: Moveable");
	alwaysCenter_choices.push_back("{027} View: Hybrid");
	MSingleToggle *alwaysCenter_toggle = new MSingleToggle(xx, y, alwaysCenter_choices);
	alwaysCenter_toggle->setSelected(config.getAlwaysCenter());
	int alwaysCenter = config.getAlwaysCenter();
	y += 13;
	
	std::vector<std::string> language_choices;
	for (int i = 0; i < 1000; i++) {
		std::string s = get_language_friendly_name(i);
		if (s == "")
			break;
		language_choices.push_back("{027} " + s);
	}
	MSingleToggle *language_toggle = new MSingleToggle(xx, y, language_choices);
	int curr_language = config.getLanguage();
	language_toggle->setSelected(curr_language);
	y += 13;
	
	MTextButton *reset_game_center = NULL;
#if (defined ALLEGRO_IPHONE || defined ALLEGRO_MACOSX) && (!defined NO_GAMECENTER)
	const char *reset_str = "Reset achievements";
	if (isGameCenterAPIAvailable()) {
		reset_game_center = new MTextButton(BW-2-(m_text_length(game_font, _t(reset_str))+m_get_bitmap_width(cursor)+1), BH-2-26, reset_str);
	}
#else
	const char *reset_str = "Controls";
#endif

	MTextButton *controls = new MTextButton(BW-2-(m_text_length(game_font, _t(reset_str))+m_get_bitmap_width(cursor)+1), BH-2-13, "Controls");
	
	tguiSetParent(0);
	tguiAddWidget(bg);
	tguiSetParent(bg);
	tguiAddWidget(sound_toggle);
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
	if (input_toggle) {
		tguiAddWidget(input_toggle);
	}
#endif
	tguiAddWidget(difficulty_toggle);
#ifdef ALLEGRO_IPHONE_XXX
	tguiAddWidget(shake_toggle);
#endif
	if (!isAndroidConsole()) {
		tguiAddWidget(tuning_toggle);
	}
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
	if (!isAndroidConsole()) {
		tguiAddWidget(swap_buttons_toggle);
	}
#endif

	tguiAddWidget(aspect_toggle);

#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_ANDROID && !defined ALLEGRO_RASPBERRYPI
	tguiAddWidget(fullscreen_toggle);
#endif
	
	tguiAddWidget(alwaysCenter_toggle);

	tguiAddWidget(language_toggle);
	
	if (reset_game_center) {
		tguiAddWidget(reset_game_center);
	}

	tguiAddWidget(controls);

#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_ANDROID && !defined ALLEGRO_RASPBERRYPI
	if (start_on_fullscreen) {
		tguiSetFocus(fullscreen_toggle);
	}
	else {
#endif
		tguiSetFocus(sound_toggle);
#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_ANDROID && !defined ALLEGRO_RASPBERRYPI
	}
#endif

	prepareForScreenGrab1();
	tguiDraw();
	drawBufferToScreen(false);
	prepareForScreenGrab2();
	fadeIn(black);

	bool break_for_fade_after_draw = false;

	clear_input_events();

	for (;;) {
		
		al_wait_cond(wait_cond, wait_mutex);
		
		// Logic
		int tmp_counter = logic_counter;
		logic_counter = 0;
		if (tmp_counter > 10)
			tmp_counter = 1;
		while  (!break_for_fade_after_draw && tmp_counter > 0) {
			next_input_event_ready = true;
			tmp_counter--;
			if (is_close_pressed()) {
				do_close();
				close_pressed = false;
			}
			if (break_main_loop) {
				goto done;
			}
			
			TGUIWidget *w = tguiUpdate();

#ifndef NO_GAMECENTER
#if defined ALLEGRO_IPHONE || defined ALLEGRO_MACOSX
			if (w && w == reset_game_center) {
				char buf[200];
				sprintf(buf, "%s?", _t("Reset achievements"));
				prepareForScreenGrab1();
				tguiDraw();
				drawBufferToScreen(false);
				prepareForScreenGrab2();
				if (prompt(buf, "", false, false)) {
					resetAchievements();
				}
			}
			else
#endif
#endif
			if (w && w == controls) {
#ifdef ALLEGRO_ANDROID
				int type = MInputGetter::TYPE_GAMEPAD;
#else
				int type = MInputGetter::TYPE_KB;
#endif
#if defined ALLEGRO_IPHONE
				prepareForScreenGrab1();
				tguiDraw();
				drawBufferToScreen(false);
				prepareForScreenGrab2();
				notify("These controls are for", "iCade only!", "");
#endif
				while (true) {
					type = config_input(type);
					if (type == 0) {
						prepareForScreenGrab1();
						tguiDraw();
						drawBufferToScreen(false);
						prepareForScreenGrab2();
						fadeIn(black);
						waitForRelease(5);
						clear_input_events();
						break;
					}
				}
			}

			INPUT_EVENT ie = get_next_input_event();

			if (ie.button2 == DOWN || iphone_shaken(0.1)) {
				use_input_event();
				iphone_clear_shaken();
				getInput()->waitForReleaseOr(5, 1000);
				break_for_fade_after_draw = true;
				prepareForScreenGrab1();
				break;
			}
		}
		
		if (language_toggle->getSelected() != curr_language) {
			curr_language = language_toggle->getSelected();
			config.setLanguage(curr_language);
			load_translation(get_language_name(config.getLanguage()).c_str());
			if (reset_game_center) {
				reset_game_center->setX(BW-2-(m_text_length(game_font, _t(reset_str))+m_get_bitmap_width(cursor)+1));
			}
			if (controls) {
				controls->setX(BW-2-(m_text_length(game_font, _t(reset_str))+m_get_bitmap_width(cursor)+1));
			}
			clear_input_events();
		}

		config.setAlwaysCenter(alwaysCenter_toggle->getSelected());
		
		int sel = sound_toggle->getSelected();
		if (sel != sound_start) {
			sound_start = sel;
			if (sel == 0) {
				config.setMusicVolume(255);
				config.setSFXVolume(255);
				setMusicVolume(1);
			}
			else if (sel == 1) {
				config.setMusicVolume(0);
				config.setSFXVolume(255);
				setMusicVolume(1);
			}
			else {
				config.setMusicVolume(0);
				config.setSFXVolume(0);
				setMusicVolume(1);
			}
		}

#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
		if (input_toggle) {
			sel = input_toggle->getSelected();
			if (config.getDpadType() != sel) {
				al_lock_mutex(dpad_mutex);
				getInput()->reset();
				config.setDpadType(sel);
				joystick_repeat_started[JOY_REPEAT_AXIS0] = false;
				joystick_repeat_started[JOY_REPEAT_AXIS1] = false;
				joystick_repeat_started[JOY_REPEAT_B1] = false;
				joystick_repeat_started[JOY_REPEAT_B2] = false;
				joystick_repeat_started[JOY_REPEAT_B3] = false;
				dpad_type = sel;
				al_unlock_mutex(dpad_mutex);
				
				if (sel == DPAD_TOTAL_1 || sel == DPAD_TOTAL_2) {
					dpad_on(false);
				}
				else {
					dpad_off(false);
				}
				if (use_dpad) {
					if (config.getTellUserToUseDpad()) {
						prepareForScreenGrab1();
						tguiDraw();
						drawBufferToScreen(false);
						prepareForScreenGrab2();
						notify("Use the on-screen directional", "pad and buttons to navigate.", "");
						config.setTellUserToUseDpad(false);
					}
				}
			}
		}
#endif

		sel = difficulty_toggle->getSelected();
		if (sel != curr_difficulty) {
			curr_difficulty = sel;
			config.setDifficulty(sel);
		}
		
		if (tuning_toggle) {
			sel = tuning_toggle->getSelected();
			if (sel != curr_tuning) {
				curr_tuning = sel;
				config.setTuning(sel);
				reinstall_timer = true;
			}
		}
		
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
		if (swap_buttons_toggle) {
			sel = swap_buttons_toggle->getSelected();
			config.setSwapButtons(sel);
		}
#endif
		
		sel = aspect_toggle->getSelected();
		if (aspect_real_to_option(config.getMaintainAspectRatio()) != sel) {
			config.setMaintainAspectRatio(aspect_option_to_real(sel));
			set_screen_params();
			if (reset_game_center) {
				reset_game_center->setX(BW-2-(m_text_length(game_font, _t(reset_str))+m_get_bitmap_width(cursor)+1));
			}
			if (controls) {
				controls->setX(BW-2-(m_text_length(game_font, _t(reset_str))+m_get_bitmap_width(cursor)+1));
			}
			clear_input_events();
		}

#if !defined ALLEGRO_ANDROID && !defined ALLEGRO_IPHONE && !defined ALLEGRO_RASPBERRYPI
		bool fs = fullscreen_toggle->getSelected();
		if (fs != (bool)start_fullscreen) {
			set_target_backbuffer();
			toggle_fullscreen();
			start_fullscreen = fs;
		}
#endif

		if (break_for_fade_after_draw || draw_counter > 0) {
			draw_counter = 0;
			if (!break_for_fade_after_draw) {
				set_target_backbuffer();
			}
			tguiDraw();
			drawBufferToScreen();
			if (break_for_fade_after_draw) {
				break;
			}
			m_flip_display();
		}
	}
done:
	if (break_for_fade_after_draw) {
		break_for_fade_after_draw = false;
		prepareForScreenGrab2();
		fadeOut(black);
	}

	tguiDeleteWidget(bg);

	delete bg;
	delete sound_toggle;
	delete difficulty_toggle;
	delete tuning_toggle;
	delete aspect_toggle;

#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
	if (input_toggle)
		delete input_toggle;
	delete swap_buttons_toggle;
#if defined ALLEGRO_IPHONE_XXX
	delete shake_toggle;
#endif
#if defined ALLEGRO_IPHONE || defined ALLEGRO_MACOSX
	if (reset_game_center)
		delete reset_game_center;	
#endif
	delete controls;
#elif !defined ALLEGRO_RASPBERRYPI
	delete fullscreen_toggle;
#endif
	delete language_toggle;
	delete alwaysCenter_toggle;
	
	tguiPop();

	dpad_on();

	pause_f_to_toggle_fullscreen = false;

	waitForRelease(4);
	waitForRelease(5);
	clear_input_events();

	config.write();

	if (area && alwaysCenter != config.getAlwaysCenter()) {
		if (config.getAlwaysCenter() != PAN_MANUAL) {
			area->center_view = true;
		}
		else {
			area->center_view = false;
		}
	}

	return false;
}

static void title_draw(MBITMAP *bg)
{
	m_draw_bitmap(bg, 0, 0, 0);

	tguiDraw();

	drawBufferToScreen();
}

int title_menu(void)
{
	on_title_screen = true;
	
	break_main_loop = false;
	
	dpad_off();
	
	bool gdr = global_draw_red;
	global_draw_red = false;

	MBITMAP *bg = m_load_bitmap(getResource("media/title.png"));
	
	debug_message("title bg loaded\n");

	std::vector<std::string> options;
	options.push_back("Continue");
	options.push_back("Start/Load Game");
	options.push_back("Tutorial");
	options.push_back("Options");
#if defined OUYA
	if (config.getPurchased() != 1) {
		options.push_back("BUY FULL VERSION");
	}
#endif

	MMainMenu *main_menu = new MMainMenu(BH-BH/5, options);
	
	tguiPush();

	tguiSetParent(0);

	tguiAddWidget(main_menu);

	tguiSetFocus(main_menu);

	debug_message("doing title loop\n");
	
	prepareForScreenGrab1();
	title_draw(bg);
	prepareForScreenGrab2();
	fadeIn(black);

	int selected = -1;

	bool break_for_fade_after_draw = false;

	clear_input_events();

	for (;;) {
		al_wait_cond(wait_cond, wait_mutex);
		// Logic
		int tmp_counter = logic_counter;
		logic_counter = 0;
		if (tmp_counter > 10)
			tmp_counter = 1;
		while  (!break_for_fade_after_draw && tmp_counter > 0) {
			next_input_event_ready = true;

			tmp_counter--;
			if (is_close_pressed()) {
				do_close();
				close_pressed = false;
			}

			TGUIWidget *widget = tguiUpdate();

			if (widget == main_menu && main_menu->getSelected() < 3) {
				selected = main_menu->getSelected();
				break_for_fade_after_draw = true;
				prepareForScreenGrab1();
				break;
			}
			else if (widget == main_menu && main_menu->getSelected() == 3) {
				prepareForScreenGrab1();
				title_draw(bg);
				prepareForScreenGrab2();
				fadeOut(black);
				tguiPush();
				on_title_screen = false;
				bool result = config_menu();
				on_title_screen = true;
				main_menu->setSelected(0);
				tguiPop();
				if (result == true) {
					selected = 0xDEAD;
					break_for_fade_after_draw = true;
					prepareForScreenGrab1();
					break;
				}
				prepareForScreenGrab1();
				title_draw(bg);
				prepareForScreenGrab2();
				fadeIn(black);
			}
#if defined OUYA
			else if (widget == main_menu && main_menu->getSelected() == 4) {
				int purchased = checkPurchased();
				if (purchased != 1) {
					doIAP();
					purchased = -1;
					while (purchased == -1) {
						purchased = isPurchased();
						al_rest(0.01);
						if (is_close_pressed()) {
							do_close();
							close_pressed = false;
						}
					}
				}
				if (purchased == 1) {
					prepareForScreenGrab1();
					title_draw(bg);
					prepareForScreenGrab2();
					notify("", "Thank you!", "");
					main_menu->setSelected(0);
					main_menu->removeOption(6);
					config.setPurchased(purchased);
				}
				al_rest(0.5);
				clear_input_events();
				getInput()->set(false, false, false, false, false, false, false);
				al_flush_event_queue(input_event_queue);
			}
#endif
			break_main_loop = false; // AGAIN (SEE ABOVE)
			INPUT_EVENT ie = get_next_input_event();
			// Back button on android is a shake. other shakes (hot corner) suppressed on this menu (see monster2.cpp)
			if (ie.button2 == DOWN || iphone_shaken(0.1)) {
				selected = 0xBEEF;
				goto done;
			}
		}

		if (break_for_fade_after_draw || draw_counter > 0) {
			draw_counter = 0;

			if (!break_for_fade_after_draw) {
				set_target_backbuffer();
			}

			title_draw(bg);

			if (break_for_fade_after_draw) {
				break;
			}

			m_flip_display();
		}
	}

done:

	if (break_for_fade_after_draw) {
		break_for_fade_after_draw = false;
		prepareForScreenGrab2();
		fadeOut(black);
	}

	m_destroy_bitmap(bg);

	tguiDeleteWidget(main_menu);
	delete main_menu;

	dpad_on();

	tguiPop();

	tguiEnableHotZone(true);

	global_draw_red = gdr;

	on_title_screen = false;

	return selected;
}

#ifdef DEBUG_XXX
class MNumGetter : public TGUIWidget {
public:
	void draw(void)
	{
		char n[10];
		sprintf(n, "%d", value);
		mTextout_simple(n, x, y, white);
		m_draw_bitmap(up_arrow, x+50, y, M_FLIP_VERTICAL);
		m_draw_bitmap(up_arrow, x+65, y, 0);
	}

	int getValue(void) {
		return value;
	}
	void mouseDown(int mx, int my, int b)
	{
		if (mx >= 50 && mx < 60)
			down = 1;
		else if (mx >= 65 && mx < 75)
			down = 2;
	}

	void mouseUpAbs(int x, int y, int b)
	{
		down = 0;
		count = 0;
	}

	int update(int step)
	{
		count += step;
		if (count < 100)
			return TGUI_CONTINUE;
		count = 0;

		if (down == 1 && value)
			value--;
		else if (down == 2)
			value++;
		return TGUI_CONTINUE;
	}

	MNumGetter(int x, int y, int initial)
	{
		this->x = x;
		this->y = y;
		this->width = 100;
		this->height = 10;
		this->hotkeys = NULL;
		this->value = initial;
		down = 0;
		up_arrow = m_load_bitmap(getResource("media/up.png"));
		count = 0;
	}

	virtual ~MNumGetter(void)
	{
		m_destroy_bitmap(up_arrow);
	}
private:
	MBITMAP *up_arrow;
	int value;
	int down;
	int count;
};


void debug_start(DEBUG_DATA *d)
{
	dpad_off();

	char *area_names[] = {
		"Keep_1",
		"Keep_2",
		"Keep_3",
		"Keep_4",
		"Keep_cells",
		"Keep_cells2",
		"Keep_outer",
		"Keep_t1-1",
		"Keep_t1-2",
		"Keep_t2-1",
		"Keep_t2-2",
		"Keep_t3-1",
		"Keep_t3-2",
		"Keep_t4-1",
		"Keep_t4-2",
		"Muttrace",
		"Underground_0",
		"Underground_1",
		"Underground_2",
		"Underground_start",
		"beach",
		"castle",
		"castle_l1",
		"castle_l2",
		"castle_tower0",
		"castle_tower1_1",
		"castle_tower1_2",
		"castle_tower1_3",
		"castle_tower2_1",
		"castle_tower2_2",
		"castle_tower2_3",
		"castle_tower3_1",
		"castle_tower3_2",
		"castle_tower4_1",
		"castle_tower4_2",
		"darkside",
		"eastern_dock",
		"farm",
		"farmhouse",
		"farmhouse2",
		"flowey",
		"flowey_barracks",
		"flowey_barracks2",
		"flowey_shop",
		"flowey_tavern",
		"forest",
		"fort1--1",
		"fort1-2",
		"fort1-3",
		"fort2--1",
		"fort2-2",
		"fort2-3",
		"fort3--1",
		"fort3-2",
		"fort3-3",
		"fort4--1",
		"fort4-2",
		"fort4-3",
		"fort_start",
		"hut1",
		"hut2",
		"hut3",
		"inn0",
		"inn1",
		"inn2",
		"jungle",
		"jungle_village",
		"launchpad",
		"lib",
		"libb",
		"library_downstairs",
		"monestary",
		"moon_landing",
		"moon_valley",
		"mountain_descent0",
		"mountain_descent1",
		"mountain_descent2",
		"mountain_descent3",
		"mountains",
		"mr_hut-1",
		"mr_hut-2",
		"mr_hut-3",
		"mr_hut-4",
		"mr_hut-5",
		"mrbig",
		"portal",
		"seaside",
		"seaside_repaired",
		"seaside_shop",
		"seaside_shop_repaired",
		"shyzu",
		"shyzu_in-1",
		"shyzu_in-2",
		"shyzu_in-3",
		"shyzu_in-4",
		"shyzu_in-5",
		"test",
		"tutorial",
		"volc_in-1",
		"volc_in-2",
		"volc_in-3",
		"volc_in-4",
		"volc_save",
		"volcano",
		NULL
	};
	
	MScrollingList *area_list = new MScrollingList(0, 20, 220, 120,
		NULL, 0, 0, NULL, NULL);

	std::vector<std::string> items;
	for (int i = 0; area_names[i]; i++) {
		items.push_back(std::string(area_names[i]));
	}

	area_list->setItems(items);

	tguiSetParent(0);
	tguiAddWidget(area_list);
	tguiSetFocus(area_list);

	while (1) {
		next_input_event_ready = true;
		TGUIWidget *w = tguiUpdate();
		if (w == area_list) {
			d->area = std::string(area_names[area_list->getSelected()]);
			break;
		}
		set_target_backbuffer();
		m_clear(m_map_rgb(0, 0, 0));

		tguiDraw();
		
		drawBufferToScreen();
		m_flip_display();
	}

	tguiDeleteWidget(area_list);
	delete area_list;

	MLabel *l1 = new MLabel(10, 10, "x:", white);
	MLabel *l2 = new MLabel(10, 30, "y:", white);
	MNumGetter *ng1 = new MNumGetter(25, 10, 10);
	MNumGetter *ng2 = new MNumGetter(25, 30, 10);
	MTextButton *button = new MTextButton(60, 60, "OK");

	tguiSetParent(0);
	tguiAddWidget(l1);
	tguiAddWidget(l2);
	tguiAddWidget(ng1);
	tguiAddWidget(ng2);
	tguiAddWidget(button);
	tguiSetFocus(button);

	while (1) {
		next_input_event_ready = true;

		TGUIWidget *w = tguiUpdate();
		if (w == button) {
			break;
		}
		set_target_backbuffer();
		m_clear(m_map_rgb(0, 0, 0));

		tguiDraw();

		drawBufferToScreen();
		m_flip_display();
	}

	d->x = ng1->getValue();
	d->y = ng2->getValue();

	tguiDeleteWidget(l1);
	tguiDeleteWidget(l2);
	tguiDeleteWidget(ng1);
	tguiDeleteWidget(ng2);
	tguiDeleteWidget(button);

	delete l1;
	delete l2;
	delete ng1;
	delete ng2;
	delete button;

	const char *ms_names[] = {
		"MS_OPENING_SCENE",
		"MS_CELL_SCENE",
		"MS_REMOVED_SQUEEKY_BOARDS",
		"MS_KEEP_CHEST_0",
		"MS_UNDERGROUND_CHEST_0",
		"MS_UNDERGROUND_CHEST_1",
		"MS_UNDERGROUND_CHEST_2",
		"MS_UNDERGROUND_CHEST_3",
		"MS_UNDERGROUND_CHEST_4",
		"MS_ITEM_CHOOSER_HELP",
		"MS_SAVE_SPOT_HELP",
		"MS_UNDERGROUND_0_CHEST",
		"MS_UNDERGROUND_0_CHEST_2",
		"MS_RIDER_APPEARED",
		"MS_RIDER_JOINED",
		"MS_UNDERGROUND_1_CHEST_0",
		"MS_UNDERGROUND_1_CHEST_1",
		"MS_UNDERGROUND_1_CHEST_2",
		"MS_UNDERGROUND_2_CHEST_1",
		"MS_DEFEATED_MONSTER",
		"MS_ENCOUNTERED_MONSTER",
		"MS_UNDERGROUND_2_CHEST_2",
		"MS_UNDERGROUND_2_CHEST_3",
		"MS_MET_HORSE",
		"MS_SEASIDE_REPAIRED",
		"MS_GOT_BADGE",
		"MS_BURNED_SEASIDE_CHEST_0",
		"MS_BURNED_SEASIDE_CHEST_1",
		"MS_BURNED_SEASIDE_CHEST_2",
		"MS_RIOS_JOINED",
		"MS_ACTIVATED_RIOS",
		"MS_TALKED_TO_RIOS",
		"MS_TALKED_TO_CAPTAIN",
		"MS_LEFT_SEASIDE_BY_BOAT",
		"MS_LEFT_EAST_BY_BOAT",
		"MS_GOT_FRUIT",
		"MS_GOT_FRUIT_JUICE",
		"MS_SAILORS_WORD",
		"MS_SAILED_PERIOD",
		"MS_GUNNAR_JOINED",
		"MS_STARTED_THRONE_SCENE",
		"MS_ENDED_THRONE_SCENE",
		"MS_GOT_RING",
		"MS_CASTLE_TOWER1_CHEST0",
		"MS_CASTLE_TOWER3_CHEST0",
		"MS_CASTLE_CHEST0",
		"MS_CASTLE_SLEEP_HINT",
		"MS_BEAT_WITCH",
		"MS_MOUNTAIN_CHEST_0",
		"MS_MOUNTAIN_CHEST_1",
		"MS_MOUNTAIN_CHEST_2",
		"MS_MOUNTAIN_CHEST_3",
		"MS_MOUNTAIN_CHEST_4",
		"MS_UNLOCKED_FLOWEY",
		"MS_GOLEM_TIMER_STARTED",
		"MS_KILLED_GOLEMS",
		"MS_KILLED_1_GOLEM",
		"MS_FLOWEY_WALLS_FIXED",
		"MS_GOT_KEY",
		"MS_INN1_CHEST",
		"MS_RIOS_TALKED_TO_LIBRARIAN",
		"MS_RIOS_LEFT",
		"MS_GUARDS_CHEST",
		"MS_TAVERN_JUICE",
		"MS_GOT_MEDALLION",
		"MS_BEACH_INTRO",
		"MS_BEACH_BATTLE_DONE",
		"MS_FARMERS_POT1",
		"MS_FARMERS_POT2",
		"MS_COW1",
		"MS_COW2",
		"MS_COW3",
		"MS_COW4",
		"MS_GOT_MILK",
		"MS_GUNNAR_LEFT",
		"MS_SUB_SCENE_DONE",
		"MS_DOCK_TO_FORT",
		"MS_FORT_TO_DOCK",
		"MS_CHEST_FLINT1",
		"MS_CHEST_FLINT2",
		"MS_CHEST_FLINT3",
		"MS_CHEST_FLINT4",
		"MS_LIT_ALTER1",
		"MS_LIT_ALTER2",
		"MS_LIT_ALTER3",
		"MS_LIT_ALTER4",
		"MS_DRAINED_POOL",
		"MS_GOT_KEY2",
		"MS_GOT_LOOKING_SCOPE",
		"MS_DROPPED_MEDALLION",
		"MS_FORT_TRAPPED_CHEST1",
		"MS_FORT_TRAPPED_CHEST2",
		"MS_FORT_TRAPPED_CHEST3",
		"MS_UNUSED__",
		"MS_UNUSED__",
		"MS_BEAT_TIGGY",
		"MS_GONE_TO_ARCHERY_TOWER",
		"MS_BEAT_ARCHERY",
		"MS_BEAT_KEEP_DOOR_GUARDS",
		"MS_KEEP_1_CHEST_0",
		"MS_KEEP_1_CHEST_1",
		"MS_FREED_PRISONER",
		"MS_KEEP_1_CHEST_2",
		"MS_KEEP_1_CHEST_3",
		"MS_HELPED_GOBLIN",
		"MS_KEEP_1_FIGHT",
		"MS_KEEP_2_CHEST_0",
		"MS_KEEP_2_CHEST_1",
		"MS_GONE_TO_TROLL_TOWER",
		"MS_BEAT_TROLL",
		"MS_KEEP_3_CHEST_0",
		"MS_KEEP_3_CHEST_1",
		"MS_KEEP_3_CHEST_2",
		"MS_KEEP_3_CHEST_3",
		"MS_KILLED_NANNER",
		"MS_NANNER_CHEST",
		"MS_KEEP_TOWER1_CHEST",
		"MS_KEEP_TOWER2_CHEST",
		"MS_KEEP_TOWER3_CHEST",
		"MS_KEEP_TOWER4_CHEST",
		"MS_KEEP_4_GOBLINS",
		"MS_BEAT_RIDER",
		"MS_GOT_STAFF",
		"MS_TALKED_TO_HELPED_GOBLIN",
		"MS_TIGGY_SEASIDE_SCENE",
		"MS_SPOKE_TO_KING_ABOUT_STAFF",
		"MS_JUNGLE_CHEST_1",
		"MS_JUNGLE_CHEST_2",
		"MS_JUNGLE_CHEST_3",
		"MS_JUNGLE_CHEST_4",
		"MS_JUNGLE_CHEST_5",
		"MS_JUNGLE_CHEST_6",
		"MS_JUNGLE_CHEST_7",
		"MS_JUNGLE_CHEST_8",
		"MS_BEAT_TREE",
		"MS_TREE_CHEST",
		"MS_PASSED_JUNGLE",
		"MS_JUNGLE_VILLAGE_CHEST",
		"MS_UNLOCKED_VOLCANO",
		"MS_VOLCANO_BRIDGE_LOWERED",
		"MS_VOLCANO_CHEST_0",
		"MS_VOLCANO_CHEST_1",
		"MS_VOLCANO_CHEST_2",
		"MS_VOLCANO_CHEST_3",
		"MS_VOLCANO_CHEST_4",
		"MS_VOLCANO_CHEST_5",
		"MS_VOLCANO_CHEST_6",
		"MS_VOLCANO_CHEST_7",
		"MS_BEAT_GIRL_DRAGON",
		"MS_FARMER_GONE_TO_GATE",
		"MS_FARMER_AT_LAUNCHPAD",
		"MS_ROCKET_LAUNCHED",
		"MS_ON_MOON",
		"MS_TIPPER_JOINED",
		"MS_SHYZU_CHEST_1",
		"MS_SHYZU_CHEST_2",
		"MS_MOON_VALLEY_CHEST_1",
		"MS_MOON_VALLEY_CHEST_2",
		"MS_MOON_VALLEY_CHEST_3",
		"MS_MOON_VALLEY_CHEST_4",
		"MS_PAST_MOON_VALLEY",
		"MS_MUTTRACE_LOWERED",
		"MS_MUTTRACE_CHEST_1",
		"MS_MUTTRACE_CHEST_2",
		"MS_MUTTRACE_CHEST_3",
		"MS_MUTTRACE_CHEST_4",
		"MS_GOT_ORB",
		"MS_MRBIG_CHEST",
		"MS_MUTTRACE_SCENE",
		"MS_LANDER_LAUNCHED",
		"MS_BEAT_TODE",
		"MS_DARKSIDE_CHEST_1",
		"MS_DARKSIDE_CHEST_2",
		"MS_DARKSIDE_CHEST_3",
		"MS_DARKSIDE_CHEST_4",
		"MS_SUN_SCENE",
		"MS_DONE_CREDITS",
		"MS_FIRST_POISON",
		"MS_SLEPT_IN_INN",
		"MS_FOREST_GOLD",
		"MS_VOLCANO_SAVE_CHEST",
		"MS_LOOKED_FROM_KEEP",
		"MS_ENY_CELL_OPENED",
		"MS_MATE_GAVE_ADVICE",
		"MS_FORT_UNDER_CHEST_1",
		"MS_FORT_UNDER_CHEST_2",
		"MS_FORT_UNDER_CHEST_3",
		"MS_FORT_UNDER_CHEST_4",
		"MS_MUTTRACE_ELIXIR",
		NULL
	};

	MToggleList *ms_list = new MToggleList(0, 20, 220, 100);

	items.clear();

	for (int i = 0; ms_names[i]; i++) {
		items.push_back(std::string(ms_names[i]));
	}

	ms_list->setItems(items);

	button = new MTextButton(10, 142, "OK");

	tguiSetParent(0);
	tguiAddWidget(ms_list);
	tguiAddWidget(button);
	tguiSetFocus(button);

	while (1) {
		next_input_event_ready = true;

		TGUIWidget *w = tguiUpdate();
		if (w == button) {
			break;
		}
		set_target_backbuffer();
		m_clear(m_map_rgb(0, 0, 0));

		tguiDraw();
		
		drawBufferToScreen();
		m_flip_display();
	}

	std::vector<bool> list = ms_list->getToggled();

	for (int i = 0; i < (int)list.size(); i++) {
		std::pair<int, bool> p;
		p.first = i;
		p.second = list[i];
		d->milestones.push_back(p);
	}

	tguiDeleteWidget(ms_list);
	tguiDeleteWidget(button);

	dpad_on();

	delete ms_list;
	delete button;
}
#endif
