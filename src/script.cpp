#include "monster2.hpp"

#ifdef ALLEGRO_IPHONE
#include "iphone.h"
#endif

static Player *pushed_players[MAX_PARTY] = { NULL, };
static Player **strategy_players;
bool player_scripted = false;
static bool lander_success = false;

bool shouldDoMap = false;
std::string mapStartPlace;
std::string mapPrefix;
int battleStartDirection;


// Note: lua starts counting at 1, in C++ we start at 0, that's why you
// see -1 and +1  here and there

unsigned char *slurp_file(std::string filename, int *ret_size)
{
	ALLEGRO_FILE *f = al_fopen(filename.c_str(), "rb");

	if (!f)
		return NULL;

	long size = al_fsize(f);
	unsigned char *bytes;

	if (size < 0) {
		std::vector<char> v;
		int c;
		while ((c = al_fgetc(f)) != EOF) {
			v.push_back(c);
		}
		size = v.size();
		bytes = new unsigned char[size+1];
		for (int i = 0; i < size; i++) {
			bytes[i] = v[i];
		}
	}
	else {
		bytes = new unsigned char[size+1];
		al_fread(f, bytes, size);
	}
	al_fclose(f);

	bytes[size] = 0;

	if (ret_size)
		*ret_size = size;

	return bytes;
}


int getNumberFromScript(lua_State *state, std::string name)
{
	lua_getglobal(state, name.c_str());
	int d = (int)lua_tonumber(state, -1);
	lua_pop(state, 1);
	return d;
}


/* This basically calls data/scripts/start.lua
 */
void startNewGame(const char *name)
{

	/* Reset game info */
	for (int i = 0; i < MAX_MILESTONES; i++) {
		gameInfo.milestones[i] = false;
	}
	party[heroSpot]->setInfo(enyStartInfo);
	for (int i = 0; i < MAX_INVENTORY; i++) {
		inventory[i].index = -1;
		inventory[i].quantity = 0;
	}

	/* Start tutorial down 50 hp */
	if (std::string(name) == "tutorial") {
		party[heroSpot]->getInfo().abilities.hp -= 50;
	}


	/* Run start script */

	lua_State *luaState = lua_open();

	openLuaLibs(luaState);

	registerCFunctions(luaState);

	runGlobalScript(luaState);

	unsigned char *bytes;
	int file_size;

	debug_message("Loading global area script...\n");
	bytes = slurp_file(getResource("area_scripts/global.%s", getScriptExtension().c_str()), &file_size);
	if (!bytes) native_error("Couldn't load area_scripts/global.lua.");
	if (luaL_loadbuffer(luaState, (char *)bytes, file_size, "chunk")) {
		dumpLuaStack(luaState);
		throw ReadError();
	}
	delete[] bytes;

	debug_message("Running global area script...\n");
	if (lua_pcall(luaState, 0, 0, 0)) {
		dumpLuaStack(luaState);
		lua_close(luaState);
		throw ScriptError();
	}

	debug_message("Loading start script...\n");
	bytes = slurp_file(getResource("scripts/%s.%s", name, getScriptExtension().c_str()), &file_size);
	if (!bytes) native_error((std::string("Couldn't load scripts/") + name + ".lua.").c_str());
	if (luaL_loadbuffer(luaState, (char *)bytes, file_size, "chunk")) {
		dumpLuaStack(luaState);
		lua_close(luaState);
		throw ReadError();
	}
	delete[] bytes;

	debug_message("Running start script...\n");
	if (lua_pcall(luaState, 0, 0, 0)) {
		dumpLuaStack(luaState);
		lua_close(luaState);
		throw ScriptError();
	}

	debug_message("Returning control from startNewGame\n");

	lua_close(luaState);
}


void runGlobalScript(lua_State *luaState)
{
	unsigned char *bytes;
	int file_size;

	ALLEGRO_DEBUG("Running global script");

	debug_message("Loading global script...\n");
	bytes = slurp_file(getResource("scripts/global.%s", getScriptExtension().c_str()), &file_size);
	if (!bytes) native_error("Couldn't load scripts/global.lua.");
	ALLEGRO_DEBUG("slurped %d bytes", file_size);
	if (luaL_loadbuffer(luaState, (char *)bytes, file_size, "chunk")) {
		dumpLuaStack(luaState);
		throw ReadError();
	}
	ALLEGRO_DEBUG("loaded buffer");
	delete[] bytes;
	ALLEGRO_DEBUG("deleted bytes");

	debug_message("Running global script...\n");
	if (lua_pcall(luaState, 0, 0, 0)) {
		dumpLuaStack(luaState);
		lua_close(luaState);
		throw ScriptError();
	}
}


/*
 * Call a Lua function, leaving the results on the stack.
 */
void callLua(lua_State* luaState, const char *func, const char *sig, ...)
{
	va_list vl;
	int narg, nres;  /* number of arguments and results */

	va_start(vl, sig);
	lua_getglobal(luaState, func);  /* get function */
	
	if (!lua_isfunction(luaState, -1)) {
		lua_pop(luaState, 1);
		const char *s = strstr(sig, ">");
		if (s != NULL) {
			s++;
			while (*s != 0) {
				lua_pushnil(luaState);
				s++;
			}
		}
		return;
	}

	/* push arguments */
	narg = 0;
	while (*sig) {  /* push arguments */
		switch (*sig++) {
			case 'b':  /* int argument */
				lua_pushboolean(luaState, va_arg(vl, int));
				break;

			case 'i':  /* int argument */
				lua_pushnumber(luaState, va_arg(vl, int));
				break;

			case 's':  /* string argument */
				lua_pushstring(luaState, va_arg(vl, char *));
				break;
			case '>':
				goto endwhile;
			default:
				break;
		}
		narg++;
		luaL_checkstack(luaState, 1, "too many arguments");
	}
endwhile:

	/* do the call */
	nres = strlen(sig);  /* number of expected results */
	if (lua_pcall(luaState, narg, nres /*LUA_MULTRET*/, 0) != 0) {
		debug_message("error running function `f': %s", lua_tostring(luaState, 1));
	}
	
	va_end(vl);
}

/*
static const luaL_Reg lualibs[] = {
	{ "", luaopen_base },
	{ LUA_LOADLIBNAME, luaopen_package },
	{ LUA_TABLIBNAME, luaopen_table },
	{ LUA_IOLIBNAME, luaopen_io },
	{ LUA_OSLIBNAME, luaopen_os },
	{ LUA_STRLIBNAME, luaopen_string },
	{ LUA_MATHLIBNAME, luaopen_math },
	{ LUA_DBLIBNAME, luaopen_debug },
	{ 0, 0 }
};
*/

void openLuaLibs(lua_State *L) 
{
	luaL_openlibs(L);
/*
	const luaL_Reg *lib = lualibs;
	for (; lib->func; lib++) {
		lua_pushcfunction(L, lib->func);
		lua_pushstring(L, lib->name);
		lua_call(L, 1, 0);
	}
*/
}

void dumpLuaStack(lua_State *l) 
{
        int i;
        int top = lua_gettop(l);

        printf ("--- stack ---\n");
        printf ("top=%u   ...   ", top);

        for (i = 1; i <= top; i++) {  /* repeat for each level */
                int t = lua_type(l, i);
                switch (t) {

                case LUA_TSTRING:  /* strings */
                        printf("`%s'", lua_tostring(l, i));
                        break;

                case LUA_TBOOLEAN:  /* booleans */
                        printf(lua_toboolean(l, i) ? "true" : "false");
                        break;

                case LUA_TNUMBER:  /* numbers */
                        printf("%f", lua_tonumber(l, i));
                        break;

                case LUA_TTABLE:   /* table */
                        printf("table");
                        break;

                default:  /* other values */
                        printf("%s", lua_typename(l, t));
                        break;

                }
                printf("  ");  /* put a separator */
        }
        printf("\n");  /* end the listing */

        printf ("-------------\n");
}

std::string getScriptExtension()
{
	if (config.useCompiledScripts())
		return std::string("scr");
	else
		return std::string("lua");
}


static int CStartArea(lua_State* stack)
{
	const char* name = lua_tostring(stack, 1);

	startArea(std::string(name));

	return 0;
}


static int CFadeIn(lua_State *stack)
{
	int r = (int)lua_tonumber(stack, 1);
	int g = (int)lua_tonumber(stack, 2);
	int b = (int)lua_tonumber(stack, 3);

	MCOLOR color = m_map_rgb(r, g, b);
	fadeIn(color);

	return 0;
}


static int CFadeOut(lua_State *stack)
{
	int r = (int)lua_tonumber(stack, 1);
	int g = (int)lua_tonumber(stack, 2);
	int b = (int)lua_tonumber(stack, 3);

	MCOLOR color = m_map_rgb(r, g, b);
	fadeOut(color);

	if (!lua_isnil(stack, 4)) {
		int d = (int)lua_tonumber(stack, 4);
		m_rest(d);
	}

	return 0;
}

static int CTransitionIn(lua_State *stack)
{
	transitionIn();
	return 0;
}


static int CTransitionOut(lua_State *stack)
{
	transitionOut();
	return 0;
}

static int CSetObjectPosition(lua_State *stack)
{
	int id = (int)lua_tonumber(stack, 1);
	int x = (int)lua_tonumber(stack, 2);
	int y = (int)lua_tonumber(stack, 3);

	Object *o = area->findObject(id);
	o->setPosition(x, y);

	return 0;
}


void setObjectDirection(Object *o, int direction)
{
	try {
		Input *i = o->getInput();
		i->setDirection(direction);
		InputDescriptor in = i->getDescriptor();
		AnimationSet *as = o->getAnimationSet();
		if (as) {
			switch (in.direction) {
				case DIRECTION_NORTH:
					as->setSubAnimation("stand_n");
					break;
				case DIRECTION_SOUTH:
					as->setSubAnimation("stand_s");
					break;
				case DIRECTION_EAST:
					as->setSubAnimation("stand_e");
					break;
				case DIRECTION_WEST:
					as->setSubAnimation("stand_w");
					break;
				default:
					break;
			}
		}
	}
	catch (...) {
		// do nothing
	}
}


static int CSetObjectDirection(lua_State *stack)
{
	int id = (int)lua_tonumber(stack, 1);
	int direction = ((int)lua_tonumber(stack, 2)) - 1; // lua starts at 1

	Object *o = area->findObject(id);
	if (o) {
		setObjectDirection(o, direction);
	}

	return 0;
}



static int CAddObject(lua_State *stack)
{
	Object *o = new Object();
	int id = o->getId();
	area->addObject(o);

	lua_pushnumber(stack, id);

	return 1;
}


static int CRemoveObject(lua_State *stack)
{
	int id = (int)lua_tonumber(stack, 1);
	area->removeObject(id);
	return 0;
}


static int CSetObjectAnimationSet(lua_State *stack)
{
	int id = (int)lua_tonumber(stack, 1);
	const char *name = lua_tostring(stack, 2);

	Object *o = area->findObject(id);

	if (o) {
		std::string nameS = getResource("objects/%s.png", name);
		o->setAnimationSet(nameS);
	}
	else {
		debug_message("Couldn't find object with id %d (0)\n", id);
	}

	return 0;
}



static int CSetObjectInputToScriptControlled(lua_State *stack)
{
	int id = (int)lua_tonumber(stack, 1);

	Object *o = area->findObject(id);

	if (o) {
		al_lock_mutex(input_mutex);
		o->setInput(new ScriptInput());
		al_unlock_mutex(input_mutex);
	}
	else {
		debug_message("Couldn't find object with id %d (1)\n", id);
	}

	return 0;
}


static int CSetObjectInput(lua_State *stack)
{
	int id = (int)lua_tonumber(stack, 1);
	bool l = lua_toboolean(stack, 2);
	bool r = lua_toboolean(stack, 3);
	bool u = lua_toboolean(stack, 4);
	bool d = lua_toboolean(stack, 5);
	bool b1 = lua_toboolean(stack, 6);
	bool b2 = lua_toboolean(stack, 7);

	Object *o = area->findObject(id);

	if (o) {
		try {
			Input *i = o->getInput();
			i->set(l, r, u, d, b1, b2, false); // NOTE!!!!!
		}
		catch (...) {
			debug_message("Attempt to set input of object with no input handler set.\n");
		}
	}
	else {
		debug_message("Couldn't find object with id %d (2)\n", id);
	}

	return 0;
}



static int CGetObjectPosition(lua_State *stack)
{
	int id = (int)lua_tonumber(stack, 1);

	Object *o = area->findObject(id);

	if (o) {
		lua_pushnumber(stack, o->getX());
		lua_pushnumber(stack, o->getY());
	}
	else {
		debug_message("Attempt to get position of unknown object.\n");
		lua_pushnil(stack);
		lua_pushnil(stack);
	}

	return 2;
}


static int CGetObjectDirection(lua_State *stack)
{
	int id = (int)lua_tonumber(stack, 1);

	Object *o = area->findObject(id);

	if (o) {
		Input *i = o->getInput();
		if (i) {
			InputDescriptor id = i->getDescriptor();
			lua_pushnumber(stack, (int)id.direction+1);
		}
		else {
			lua_pushnil(stack);
		}
	}
	else {
		lua_pushnil(stack);
	}

	return 1;
}


static int CGetObjectOffset(lua_State *stack)
{
	int id = (int)lua_tonumber(stack, 1);

	Object *o = area->findObject(id);

	if (o) {
		int x, y;
		o->getOffset(&x, &y);
		lua_pushnumber(stack, x);
		lua_pushnumber(stack, y);
	}
	else {
		debug_message("Attempt to get offset of unknown object.\n");
		lua_pushnil(stack);
		lua_pushnil(stack);
	}

	return 2;
}


static int CGetMilestone(lua_State *stack)
{
	int num = (int)lua_tonumber(stack, 1)-1;

	if (num >= 0 && num < MAX_MILESTONES) {
		lua_pushboolean(stack, gameInfo.milestones[num]);
	}
	else {
		debug_message("getMilestone: milestone out of bounds (%d).\n", num);
		lua_pushnil(stack);
	}

	return 1;
}


static int CSetMilestone(lua_State *stack)
{
	int num = (int)lua_tonumber(stack, 1)-1;
	bool value = (bool)lua_toboolean(stack, 2);

	if (num >= 0 && num < MAX_MILESTONES) {
#if defined(ALLEGRO_IPHONE) && !defined(LITE)
		if (value == true && gameInfo.milestones[num] == false)
			do_milestone(num);
#endif
		gameInfo.milestones[num] = value;
	}
	else {
		debug_message("setMilestone: milestone out of bounds.\n");
	}

	return 0;
}



static int CSetObjectHigh(lua_State *stack)
{
	int id = (int)lua_tonumber(stack, 1);
	bool high = (bool)lua_toboolean(stack, 2);

	Object *o = area->findObject(id);

	if (o) {
		o->setHigh(high);
	}
	else {
		debug_message("Attempt to call setHigh on unknown object %d.\n", id);
	}

	return 0;
}



static int CSetObjectLow(lua_State *stack)
{
	int id = (int)lua_tonumber(stack, 1);
	bool low = (bool)lua_toboolean(stack, 2);

	Object *o = area->findObject(id);

	if (o) {
		o->setLow(low);
	}
	else {
		debug_message("Attempt to call setObjectLow on unknown object %d.\n", id);
	}

	return 0;
}



static int CSetObjectHidden(lua_State *stack)
{
	int id = (int)lua_tonumber(stack, 1);
	bool hidden = (bool)lua_toboolean(stack, 2);

	Object *o = area->findObject(id);

	if (o) {
		o->setHidden(hidden);
	}
	else {
		debug_message("Attempt to call setHidden on unknown object %d.\n", id);
	}

	return 0;
}



static int CScriptifyPlayer(lua_State *stack)
{
	dpad_off();
	al_lock_mutex(input_mutex);
	player_scripted = true;
	party[heroSpot]->getObject()->setInput(new ScriptInput());
	pan_centered_x = pan_centered_y = false;
	area_panned_x = floor(area_panned_x);
	area_panned_y = floor(area_panned_y);
	area->center_view = true;
	al_unlock_mutex(input_mutex);
	return 0;
}


int CDeScriptifyPlayer(lua_State *stack)
{
	dpad_on();
	al_lock_mutex(input_mutex);
	player_scripted = false;
	Input *i = party[heroSpot]->getObject()->getInput();
	delete i;
	party[heroSpot]->getObject()->setInput(getInput());
	astar_stop();
	area->center_view = false;
	area->resetInput();
	al_unlock_mutex(input_mutex);
	clear_input_events();
	return 0;
}


static int CPlayerIsScripted(lua_State *stack)
{
	lua_pushboolean(stack, player_scripted);
	return 1;
}

static int CDoDialogue(lua_State *stack)
{
	const char *text = lua_tostring(stack, 1);
	std::string textS(text);
	bool wait = (bool)lua_toboolean(stack, 2);
	bool top = (bool)lua_toboolean(stack, 3);
	bool bottom = (bool)lua_toboolean(stack, 4);
	bool ret = false;

	doDialogue(textS, top, 4, 10, bottom);

	if (!wait) { lua_pushboolean(stack, false); return 1; }

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
			// WARNING
			if (break_main_loop) {
				goto done;
			}
			if (timer_on) {
				timer_time -= LOGIC_MILLIS;
				if (timer_time <= 0) {
					dont_draw_now = true;
					ret = false;
					goto done;
				}
			}
			// update gui
			mainWidget = tguiUpdate();
			bool b = false;
			if ((speechDialog && mainWidget == speechDialog) ||
				(area && area->getName() == "tutorial" && (b = iphone_shaken(0.1)))) {
				iphone_clear_shaken();
				ret = b;
				goto done;
			}
			if (!battle) {
				Area *oldArea = area;
				area->update(LOGIC_MILLIS);
				if (area != oldArea) {
					delete oldArea;
				}
			}
		}

		if (draw_counter) {
			draw_counter = 0;
			m_set_target_bitmap(buffer);
			MCOLOR color = black;
			m_clear(color);
			/* draw the Area */
			area->draw();
			// Draw the GUI
			if (timer_on) {
				int minutes = (timer_time/1000) / 60;
				int seconds = (timer_time/1000) % 60;
				char text[10];
				sprintf(text, "%d:%02d", minutes, seconds);
				int tw = m_text_length(huge_font, "5:55") + 10;
				int th = m_text_height(huge_font);
				mTextout(huge_font, text, BW-(tw/2)-10, th/2+5,
					white, black,
					WGT_TEXT_DROP_SHADOW, true);
			}
			tguiDraw();
			drawBufferToScreen();
			m_flip_display();
		}
	}

done:

	tguiDeleteWidget(speechDialog);
	delete speechDialog;
	speechDialog = NULL;
	dpad_on();

	lua_pushboolean(stack, ret);

	return 1;
}


static int CDoShakeDialogue(lua_State *stack)
{
	const char *text = lua_tostring(stack, 1);
	std::string textS(text);
	bool wait = (bool)lua_toboolean(stack, 2);
	bool top = (bool)lua_toboolean(stack, 3);
	bool bottom = (bool)lua_toboolean(stack, 4);

	doDialogue(textS, top, 4, 10, bottom);

	if (!wait) { lua_pushboolean(stack, false); return 1; }

	int sx = area_panned_x;
	int sy = area_panned_y;
	
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
			// WARNING
			if (break_main_loop) {
				lua_pushboolean(stack, false);
				tguiDeleteWidget(speechDialog);
				delete speechDialog;
				speechDialog = NULL;
				dpad_on();
				area_panned_x = sx;
				area_panned_y = sy;
				return 1;
			}
		
			if (timer_on) {
				timer_time -= LOGIC_MILLIS;
				if (timer_time < 0) {
					timer_time = 0;
					if (saveFilename) saveTime(saveFilename);
					// do menu
					exit(0);
				}
			}
			// update gui
			mainWidget = tguiUpdate();
			bool b = false;
			if ((speechDialog && mainWidget == speechDialog) ||
				(area && area->getName() == "tutorial" && (b = iphone_shaken(0.1)))) {
				lua_pushboolean(stack, b);
				iphone_clear_shaken();
				tguiDeleteWidget(speechDialog);
				delete speechDialog;
				speechDialog = NULL;
				dpad_on();
				area_panned_x = sx;
				area_panned_y = sy;
				return 1;
			}
		}
		if (area_panned_y < sy)
			area_panned_y = sy+1;
		else
			area_panned_y = sy-1;
		
		if (draw_counter > 0) {
			draw_counter = 0;
			m_set_target_bitmap(buffer);
			MCOLOR color = black;
			m_clear(color);
			/* draw the Area */
			area->draw();
			// Draw the GUI
			tguiDraw();
			drawBufferToScreen();
			m_flip_display();
		}
	}

	area_panned_x = sx;
	area_panned_y = sy;

	dpad_on();

	lua_pushboolean(stack, false);
	return 1;
}


// returns true if shaken
bool anotherDoDialogue(const char *text, bool clearbuf, bool top)
{
	bool ret = false;

	std::string textS(text);
	
	tguiPush();

	doDialogue(textS, top, 4, 10, false);

	int flags = al_get_new_bitmap_flags();
	al_set_new_bitmap_flags(flags & ~ALLEGRO_NO_PRESERVE_TEXTURE);
	MBITMAP *tmp = m_create_bitmap(BW, BH); // check
	al_set_new_bitmap_flags(flags);
	m_set_target_bitmap(tmp);
	m_draw_bitmap(buffer, 0, 0, 0);
	
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
			// WARNING
			if (break_main_loop) {
				goto done;
			}
		
			// update gui
			mainWidget = tguiUpdate();
			if (speechDialog && mainWidget == speechDialog) {
				goto done;
			}
	
			INPUT_EVENT ie = get_next_input_event();
			if (iphone_shaken(0.1) || (use_dpad && ie.button2 == DOWN)) {
				use_input_event();
				iphone_clear_shaken();
				ret = true;
				goto done;
			}
		}

		if (draw_counter > 0) {
			draw_counter = 0;
			m_set_target_bitmap(buffer);
			if (clearbuf) {
				m_clear(black);
			}
			else {
				m_draw_bitmap(tmp, 0, 0, 0);
			}
			// Draw the GUI
			tguiDraw();
			drawBufferToScreen();
			m_flip_display();
		}
	}

done:

	tguiDeleteWidget(speechDialog);
	delete speechDialog;
	speechDialog = NULL;
	dpad_on();
	m_destroy_bitmap(tmp);

	tguiPop();

	return ret;
}


static int CIsSpeechDialogActive(lua_State *stack)
{
	lua_pushboolean(stack, speechDialog != NULL);
	return 1;
}



static int CSetObjectSolid(lua_State *stack)
{
	int id = (int)lua_tonumber(stack, 1);
	bool solid = (bool)lua_toboolean(stack, 2);

	Object *o = area->findObject(id);
	if (o) {
		o->setSolid(solid);
	}
	else {
		debug_message("Couldn't find object %d to set solid\n", id);
	}

	return 0;
}


static int CSetObjectAnimationSetPrefix(lua_State *stack)
{
	int id = (int)lua_tonumber(stack, 1);
	const char *prefix = lua_tostring(stack, 2);

	Object *o = area->findObject(id);
	if (o) {
		std::string prefixS(prefix);
		o->getAnimationSet()->setPrefix(prefixS);
	}
	else {
		debug_message("Couldn't find object %d to set animation set prefix.\n", id);
	}

	return 0;
}


static int CSetCombatantAnimationSetPrefix(lua_State *stack)
{
	int id = (int)lua_tonumber(stack, 1);
	const char *prefix = lua_tostring(stack, 2);


	CombatEntity *e = battle->getEntity(id);
	if (e) {
		Combatant *c = (Combatant *)e;
		c->getAnimationSet()->setPrefix(std::string(prefix));
		if (!use_programmable_pipeline) {
			c->getWhiteAnimationSet()->setPrefix(std::string(prefix));
		}
	}
	else {
		debug_message("Combatant %d not found to set animationset prefix\n", id);
	}

	return 0;
}


static int CSetObjectSubAnimation(lua_State *stack)
{
	int id = (int)lua_tonumber(stack, 1);
	const char *name = lua_tostring(stack, 2);

	Object *o = area->findObject(id);
	if (o) {
		std::string nameS(name);
		o->getAnimationSet()->setSubAnimation(nameS);
	}
	else {
		debug_message("Couldn't find object %d to set sub animation.\n", id);
	}

	return 0;
}


static int CTint(lua_State *stack)
{
	int r = (int)lua_tonumber(stack, 1);
	int g = (int)lua_tonumber(stack, 2);
	int b = (int)lua_tonumber(stack, 3);
	int a = (int)lua_tonumber(stack, 4);

	MCOLOR color = m_map_rgba(r, g, b, a);
	area->tint(&color);

	return 0;
}


static int CSetObjectIsPerson(lua_State *stack)
{
	int id = (int)lua_tonumber(stack, 1);
	bool person = (bool)lua_toboolean(stack, 2);

	Object *o = area->findObject(id);

	if (o) {
		o->setPerson(person);
	}
	else {
		printf("Error: object %d not find (setObjectIsPerson)\n", id);
	}

	return 0;
}


static int CAddEnemy(lua_State *stack)
{
	if (!battle)
		return 0;

	const char *name = lua_tostring(stack, 1);
	int x = (int)lua_tonumber(stack, 2);
	int y = (int)lua_tonumber(stack, 3);
	bool force_pos;

	if (lua_isboolean(stack, 4))
		force_pos  = (bool)lua_toboolean(stack, 4);
	else
		force_pos = false;

	CombatEnemy *enemy;

	if (!strcmp(name, "Tode"))
		enemy = new CombatEnemyTode(name, x, y);
	else
		enemy = new CombatEnemy(name, x, y);

	enemy->setLoyalty(LOYALTY_EVIL);

	Combatant *player = findRandomPlayer();

	if (player->getLocation() == LOCATION_RIGHT) {
		enemy->setDirection(DIRECTION_EAST);
		enemy->setLocation(LOCATION_LEFT);
	}
	else {
		enemy->setDirection(DIRECTION_WEST);
		enemy->setLocation(LOCATION_RIGHT);
	}

	battle->addEntity(enemy, force_pos);

	enemy->start();

	return 0;
}


static int CGetRandomPlayer(lua_State *stack)
{
	std::list<CombatEntity *> &entities = battle->getEntities();
	std::list<CombatEntity *>::iterator it;

	int numPlayers = 0;
	std::vector<int> ids;
	int i;
	for (i = 0, it = entities.begin(); it != entities.end(); it++, i++) {
		CombatEntity *e = *it;
		if ((e->getType() == COMBATENTITY_TYPE_PLAYER)
				&& (!e->isDead())) {
			CombatPlayer *p = (CombatPlayer *)e;
			if (!(p->getInfo().condition == CONDITION_SWALLOWED)) {
				ids.push_back(e->getId());
				numPlayers++;
			}
		}
	}

	if (numPlayers <= 0) {
		lua_pushnumber(stack, -1);
		return 1;
	}

	int r = rand() % numPlayers;
	int num = ids[r];
	ids.clear();

	lua_pushnumber(stack, num);

	return 1;
}


static int CGetWeakestPlayer(lua_State *stack)
{
	Combatant *c = findWeakestPlayer();
	lua_pushnumber(stack, c->getId());
	return 1;
}


static int CBattleKillCombatant(lua_State *stack)
{
	int id = (int)lua_tonumber(stack, 1);

	CombatEntity *e = battle->getEntity(id);

	if (e) {
		Combatant *c = (Combatant *)e;
		c->getInfo().abilities.hp = 0;
	}
	else {
		debug_message("Can't find entity %d (KillCombatant)\n", id);
	}

	return 0;
}


static int CGetRandomEnemy(lua_State *stack)
{
	std::list<CombatEntity *> &entities = battle->getEntities();
	std::list<CombatEntity *>::iterator it;

	int numEnemies = 0;
	std::vector<int> ids;
	int i;
	for (i = 0, it = entities.begin(); it != entities.end(); i++, it++) {
		CombatEntity *e = *it;
		if ((e->getType() == COMBATENTITY_TYPE_ENEMY)
				&& (!e->isDead())) {
			ids.push_back(e->getId());
			numEnemies++;
		}
	}

	int r = rand() % numEnemies;
	int num = ids[r];
	ids.clear();

	lua_pushnumber(stack, num);

	return 1;
}


static int CBattleLost(lua_State *stack)
{
	lua_pushboolean(stack, battle_lost);
	return 1;
}


static int CStartBattle(lua_State *stack)
{
	// FIXME: make sure this works. It's a hack to avoid
	// activating a chest or person and then immediately
	// going into battle.
	if (speechDialog)
		return 0;

	int nargs = lua_gettop(stack);

	const char *name = lua_tostring(stack, 1);

	if (nargs > 1)
		battle_must_win = (bool)lua_toboolean(stack, 2);
	else
		battle_must_win = true;

	bool can_run;
	if (nargs > 2)
		can_run = (bool)lua_toboolean(stack, 3);
	else
		can_run = true;
	
	Object *obj = area->findObject(0);
	if (!obj) {
		printf("findObject(0) FAILED!~\n");
		return 0;
	}
	obj->stop();
	// No background, no battle
	std::string t = area->getTerrain();
	if (t == "") {
		return 0;
	}

	if (use_dpad) {
		InputDescriptor ie = getInput()->getDescriptor();
		battleStartDirection = ie.direction;
	}

	if (area->getName() == "jungle") {
		playAmbience("");
	}

	loadPlayDestroy("battle.ogg");
	
	battle = new Battle(name, can_run);
	debug_message("created battle object\n");
	battle->start();
	astar_stop();
	
	debug_message("battle->start called\n");

	battleTransition();

	clear_input_events();

	if (use_dpad) {
		getInput()->set(false, false, false, false, false, false, false);
	}

	return 0;
}


static int CInBattle(lua_State *stack)
{
	if (battle) {
		lua_pushboolean(stack, 1);
	}
	else {
		lua_pushboolean(stack, 0);
	}

	return 1;
}




static int CDrawArea(lua_State *stack)
{
	m_set_target_bitmap(buffer);
	area->draw();
	return 0;
}


static int CUpdateArea(lua_State *stack)
{
	area->update(1);
	return 0;
}


static int CClearScreen(lua_State *stack)
{
	int r = (int)lua_tonumber(stack, 1);
	int g = (int)lua_tonumber(stack, 2);
	int b = (int)lua_tonumber(stack, 3);

	m_clear(m_map_rgb(r, g, b));

	return 0;
}



static int CClearBuffer(lua_State *stack)
{
	int r = (int)lua_tonumber(stack, 1);
	int g = (int)lua_tonumber(stack, 2);
	int b = (int)lua_tonumber(stack, 3);
	
	ALLEGRO_BITMAP *oldTarget = al_get_target_bitmap();
	m_set_target_bitmap(buffer);
	m_clear(m_map_rgb(r, g, b));
	al_set_target_bitmap(oldTarget);

	return 0;
}



static int CSetBufferTarget(lua_State *stack)
{
	m_set_target_bitmap(buffer);

	return 0;
}



static int CSetObjectDest(lua_State *stack)
{
	int id = (int)lua_tonumber(stack, 1);
	int dx = (int)lua_tonumber(stack, 2);
	int dy = (int)lua_tonumber(stack, 3);

	Object *o = area->findObject(id);

	o->setDest(dx, dy);

	return 0;
}


static int CFindEmptyInventorySlot(lua_State *stack)
{
	int slot = findEmptyInventorySlot();
	lua_pushnumber(stack, slot);
	return 1;
}


static int CFindUnfullInventorySlot(lua_State *stack)
{
	int index = (int)lua_tonumber(stack, 1);
	int slot = findUnfullInventorySlot(index);
	lua_pushnumber(stack, slot);
	return 1; 
}


static int CGetInventory(lua_State *stack)
{
	int slot = (int)lua_tonumber(stack, 1);

	lua_pushnumber(stack, inventory[slot].index);
	lua_pushnumber(stack, inventory[slot].quantity);

	return 2;
}


static int CSetInventory(lua_State *stack)
{
	int slot = (int)lua_tonumber(stack, 1);
	int index = (int)lua_tonumber(stack, 2);
	int quantity = (int)lua_tonumber(stack, 3);

	setInventory(slot, index, quantity);

	return 0;
}


static int CFindUsedInventorySlot(lua_State *stack)
{
	int index = (int)lua_tonumber(stack, 1);

	int slot = findUsedInventorySlot(index);
	lua_pushnumber(stack, slot);
	lua_pushnumber(stack, inventory[slot].quantity);

	return 2;
}


static int CGetItemName(lua_State *stack)
{
	int index = (int)lua_tonumber(stack, 1);

	std::string name = getItemName(index);

	lua_pushstring(stack, name.c_str());

	return 1;
}

static int CGetItemIcon(lua_State *stack)
{
	int index = (int)lua_tonumber(stack, 1);
	lua_pushstring(stack, getItemIcon(index).c_str());
	return 1;
}


static int CGetRandomNumber(lua_State *stack)
{
	int max = (int)lua_tonumber(stack, 1);
	lua_pushnumber(stack, rand() % max);
	return 1;
}


static int CLoadPlayDestroy(lua_State *stack)
{
	const char *name = lua_tostring(stack, 1);

	loadPlayDestroy(std::string(name));

	return 0;
}


static int CPlayPreloadedSample(lua_State *stack)
{
	const char *name = lua_tostring(stack, 1);

	playPreloadedSample(std::string(name));

	return 0;
}


static int CSetObjectIsFloater(lua_State *stack)
{
	int id = (int)lua_tonumber(stack, 1);
	bool floater = (bool)lua_toboolean(stack, 2);

	Object *o = area->findObject(id);

	if (o) {
		o->setFloater(floater);
	}
	else {
		printf("Attempt to call setFloater on unknown object %d\n", id);
	}

	return 0;
}


static int CAddPartyMember(lua_State *stack)
{
	const char *name = lua_tostring(stack, 1);

	Player *player = new Player(std::string(name)); // auto put in party

	if (std::string(name) == "Rider") {
		player->setInfo(riderStartInfo);
	}
	else if (std::string(name) == "Rios") {
		player->setInfo(riosStartInfo);
	}
	else if (std::string(name) == "Gunnar") {
		player->setInfo(gunnarStartInfo);
	}
	else if (std::string(name) == "Faelon") {
		player->setInfo(faelonStartInfo);
	}
	else if (std::string(name) == "Mel") {
		player->setInfo(melStartInfo);
	}
	else if (std::string(name) == "Tiggy") {
		player->setInfo(tiggyStartInfo);
	}
	else if (std::string(name) == "Tipper") {
		player->setInfo(tipperStartInfo);
	}
	else
		return 0;

	loadPlayDestroy("new_party_member.ogg");

	return 0;
}

static int CRemovePartyMember(lua_State *stack)
{
	const char *name = lua_tostring(stack, 1);

	for (int i = 0; i < MAX_PARTY; i++) {
		if (party[i] && party[i]->getName() == std::string(name)) {
			delete party[i];
			party[i] = NULL;
			break;
		}
	}

	return 0;
}


static int CStopObject(lua_State *stack)
{
	int id = (int)lua_tonumber(stack, 1);

	Object *o = area->findObject(id);

	if (o) {
		o->stop();
	}
	else {
		printf("Attempt to stop unknown object %d\n", id);
	}

	return 0;
}

static int CAstarStop(lua_State *stack)
{
	astar_stop();
	return 0;
}


static int CReviveAllPlayers(lua_State *stack)
{
	for (int i = 0; i < MAX_PARTY; i++) {
		Player *p = party[i];
		if (!p)
			continue;
		CombatantInfo &info = p->getInfo();

		info.abilities.hp = info.abilities.maxhp;
		info.abilities.mp = info.abilities.maxmp;
		info.condition = CONDITION_NORMAL;
	}

	return 0;
}


static int CAddSmoke(lua_State *stack)
{
	int x = (int)lua_tonumber(stack, 1)*TILE_SIZE;
	int y = (int)lua_tonumber(stack, 2)*TILE_SIZE;
	Smoke *ss = new Smoke(x, y);
	area->addObject(ss);

	return 0;
}


static int CAddSparklySpiralAroundPlayer(lua_State *stack)
{
	Object *o = area->findObject(0);
	int x, y;
	o->getPosition(&x, &y);
	x = (x*TILE_SIZE+(TILE_SIZE/2));
	y = (y*TILE_SIZE+(TILE_SIZE/2));
	SparklySpiral *ss = new SparklySpiral(x, y);
	area->addObject(ss);

	return 0;
}

static int CAddSparklySpiralAroundObject(lua_State *stack)
{
	int number = (int)lua_tonumber(stack, 1);

	Object *o = area->findObject(number);

	if (o) {
		int x, y;
		o->getPosition(&x, &y);
		x = (x*TILE_SIZE+(TILE_SIZE/2));
		y = (y*TILE_SIZE+(TILE_SIZE/2));
		SparklySpiral *ss = new SparklySpiral(x, y);
		area->addObject(ss);
	}

	return 0;
}


static int CDoMap(lua_State *stack)
{	
	int nargs = lua_gettop(stack);

	mapStartPlace = std::string(lua_tostring(stack, 1));

	if (nargs > 1) {
		mapPrefix = std::string(lua_tostring(stack, 2));
	}
	else {
		mapPrefix = "<none>";
	}
	
	shouldDoMap = true;

	return 0;
}


static int CDoShop(lua_State *stack)
{
	const char *name = lua_tostring(stack, 1);
	const char *img = lua_tostring(stack, 2);
	int num = (int)lua_tonumber(stack, 3);

	int *itmz = new int[num];
	int *costs = new int[num];

	for (int i = 0; i < num; i++) {
		int item = (int)lua_tonumber(stack, 4+(i*2));
		int cost = (int)lua_tonumber(stack, 5+(i*2));
		itmz[i] = item;
		costs[i] = cost;
	}

	fadeOut(black);

	static char buf[100];
	strcpy(buf, getResource("media/%s.png", img));

	doShop(std::string(name), buf, num, itmz, costs);

	delete[] itmz;
	delete[] costs;

	return 0;
}


static int CUpdateObject(lua_State *stack)
{
	int id = (int)lua_tonumber(stack, 1);
	int step = (int)lua_tonumber(stack, 2);

	Object *o = area->findObject(id);
	if (o) {
		o->update(area, step);
	}
	else {
		debug_message("Can't find object %d to update\n", id);
	}

	return 0;
}


static int CSetObjectDimensions(lua_State *stack)
{
	int id = (int)lua_tonumber(stack, 1);
	int x = (int)lua_tonumber(stack, 2);
	int y = (int)lua_tonumber(stack, 3);

	Object *o = area->findObject(id);
	if (o) {
		o->setDimensions(x, y);
	}
	else {
		debug_message("setObjectDimensions failed");
	}

	return 0;
}

static int CBattleGetX(lua_State *stack)
{
	int id = (int)lua_tonumber(stack, 1);

	CombatEntity *e = battle->getEntity(id);
	if (!e) {
		debug_message("CBattleGetX: invalid id\n");
		return 0;
	}

	lua_pushnumber(stack, e->getX());

	return 1;
}


static int CBattleGetY(lua_State *stack)
{
	int id = (int)lua_tonumber(stack, 1);

	CombatEntity *e = battle->getEntity(id);
	if (!e) {
		debug_message("CBattleGetY: invalid id\n");
		return 0;
	}

	lua_pushnumber(stack, e->getY());

	return 1;
}


static int CBattleSetX(lua_State *stack)
{
	int id = (int)lua_tonumber(stack, 1);
	int x = (int)lua_tonumber(stack, 2);

	CombatEntity *e = battle->getEntity(id);
	if (!e) {
		debug_message("CBattleSetX: invalid id\n");
		return 0;
	}

	e->setX(x);

	return 0;
}


static int CBattleSetY(lua_State *stack)
{
	int id = (int)lua_tonumber(stack, 1);
	int y = (int)lua_tonumber(stack, 2);

	CombatEntity *e = battle->getEntity(id);
	if (!e) {
		debug_message("CBattleSetY: invalid id\n");
		return 0;
	}

	e->setY(y);

	return 0;
}


static int CBattleResortEntity(lua_State *stack)
{
	int id = (int)lua_tonumber(stack, 1);

	CombatEntity *e = battle->getEntity(id);
	battle->resortEntity(e);

	return 0;
}


static int CBattleSetOx(lua_State *stack)
{
	int id = (int)lua_tonumber(stack, 1);
	int ox = (int)lua_tonumber(stack, 2);

	CombatEntity *e = battle->getEntity(id);
	if (!e) {
		debug_message("CBattleSetOx: invalid id\n");
		return 0;
	}

	e->setOx(ox);

	return 0;
}

static int CBattleSetOy(lua_State *stack)
{
	int id = (int)lua_tonumber(stack, 1);
	int oy = (int)lua_tonumber(stack, 2);

	CombatEntity *e = battle->getEntity(id);
	if (!e) {
		debug_message("CBattleSetOy: invalid id\n");
		return 0;
	}

	e->setOy(oy);

	return 0;
}


static int CBattleGetEntityType(lua_State *stack)
{
	int id = (int)lua_tonumber(stack, 1);

	CombatEntity *e = battle->getEntity(id);
	if (!e) {
		debug_message("CBattleGetEntityType: invalid id\n");
		lua_pushnil(stack);
		return 1;
	}

	lua_pushnumber(stack, (int)e->getType());

	return 1;
}


static int CBattleGetEntityCondition(lua_State *stack)
{
	int id = (int)lua_tonumber(stack, 1);

	CombatEntity *e = battle->getEntity(id);
	if (!e) {
		debug_message("CBattleGetEntityCondition: invalid id\n");
		lua_pushnil(stack);
		return 1;
	}

	Combatant *c = (Combatant *)e;
	lua_pushnumber(stack, (int)c->getInfo().condition);

	return 1;
}


static int CBattleSetEntityCondition(lua_State *stack)
{
	int id = (int)lua_tonumber(stack, 1);
	CombatCondition condition = (CombatCondition)((int)lua_tonumber(stack, 2));

	CombatEntity *e = battle->getEntity(id);
	if (!e) {
		debug_message("CBattleSetEntityCondition: invalid id\n");
		return 0;
	}

	Combatant *c = (Combatant *)e;
	c->getInfo().condition = condition;

	return 0;
}


static int CBattleGetEntityHP(lua_State *stack)
{
	int id = (int)lua_tonumber(stack, 1);

	CombatEntity *e = battle->getEntity(id);
	if (!e) {
		debug_message("CBattleGetEntityHP: invalid id\n");
		lua_pushnil(stack);
		return 1;
	}

	Combatant *c = (Combatant *)e;
	lua_pushnumber(stack, (int)c->getInfo().abilities.hp);

	return 1;
}


static int CBattleSetEntityHP(lua_State *stack)
{
	int id = (int)lua_tonumber(stack, 1);
	int hp = (int)lua_tonumber(stack, 2);

	CombatEntity *e = battle->getEntity(id);
	if (!e) {
		debug_message("CBattleSetEntityHP: invalid id\n");
		return 0;
	}

	Combatant *c = (Combatant *)e;
	c->getInfo().abilities.hp = hp;

	return 0;
}


static int CBattleSetAngle(lua_State *stack)
{
	int id = (int)lua_tonumber(stack, 1);
	float a = lua_tonumber(stack, 2);

	CombatEntity *e = battle->getEntity(id);
	if (!e) {
		debug_message("CBattleSetAngle: invalid id\n");
		return 0;
	}

	e->setAngle(a);

	return 0;
}

static int CBattleGetLocation(lua_State *stack)
{
	int id = (int)lua_tonumber(stack, 1);

	CombatEntity *e = battle->getEntity(id);
	if (!e) {
		debug_message("CBattleGetLocation: invalid id\n");
		lua_pushnil(stack);
		return 0;
	}

	if (e->getType() != COMBATENTITY_TYPE_PLAYER && e->getType() != COMBATENTITY_TYPE_ENEMY) {
		debug_message("CBattleGetLocation: not a Combatant\n");
	}

	Combatant *c = (Combatant *)e;

	lua_pushnumber(stack, c->getLocation());
	
	return 1;
}

static int CAreaIsSolid(lua_State *stack)
{
	int objId = (int)lua_tonumber(stack, 1);
	int x = (int)lua_tonumber(stack, 2);
	int y = (int)lua_tonumber(stack, 3);

	lua_pushboolean(stack, area->isOccupied((unsigned)objId, x, y, false));

	return 1;
}


static int CReverseTint(lua_State *stack)
{
	int r = (int)lua_tonumber(stack, 1);
	int g = (int)lua_tonumber(stack, 2);
	int b = (int)lua_tonumber(stack, 3);
	int a = (int)lua_tonumber(stack, 4);

	MCOLOR color = m_map_rgba(r, g, b, a);
	area->tint(&color, true);

	return 0;
}


static int CSetMusicVolume(lua_State *stack)
{
	double vol = lua_tonumber(stack, 1);

	setMusicVolume(vol);

	return 0;
}


static int CSetAmbienceVolume(lua_State *stack)
{
	double vol = lua_tonumber(stack, 1);

	setAmbienceVolume(vol);

	return 0;
}


static int CBattleGetEntityIndex(lua_State *stack)
{
	int id = (int)lua_tonumber(stack, 1);

	lua_pushnumber(stack, battle->getEntityIndex(id));

	return 1;
}

static int CBattleGetNextEntity(lua_State *stack)
{
	int id = (int)lua_tonumber(stack, 1);

	lua_pushnumber(stack, battle->getNextEntity(id));

	return 1;
}

static int CBattleMoveEntity(lua_State *stack)
{
	int which = (int)lua_tonumber(stack, 1);
	int where = (int)lua_tonumber(stack, 2);

	battle->moveEntity(which, where);

	return 0;
}

static int CBattleFindCombatant(lua_State *stack)
{
	const char *name = lua_tostring(stack, 1);

	std::list<CombatEntity *> &v = battle->getEntities();
	std::list<CombatEntity *>::iterator it;

	unsigned int i;
	for (i = 0, it = v.begin(); it != v.end(); i++, it++) {
		CombatEntity *e = *it;
		if (e->getType() == COMBATENTITY_TYPE_PLAYER || e->getType() == COMBATENTITY_TYPE_ENEMY) {
			Combatant *c = (Combatant *)e;
			if (c->getName() == std::string(name)) {
				lua_pushnumber(stack, c->getId());
				return 1;
			}
		}
	}
	
	lua_pushnumber(stack, -1);
	return 1;
}


static int CFillEllipse(lua_State *stack)
{
	int dx = (int)lua_tonumber(stack, 1);
	int dy = (int)lua_tonumber(stack, 2);
	int rx = (int)lua_tonumber(stack, 3);
	int ry = (int)lua_tonumber(stack, 4);
	int r = (int)lua_tonumber(stack, 5);
	int g = (int)lua_tonumber(stack, 6);
	int b = (int)lua_tonumber(stack, 7);
	int a = (int)lua_tonumber(stack, 8);

	MCOLOR color = m_map_rgba(r, g, b, a);

	m_fill_ellipse(dx, dy, rx, ry, color);

	return 0;
}


static int CBattleDoAttack(lua_State *stack)
{
	int attackerId = (int)lua_tonumber(stack, 1);
	int attackedId = (int)lua_tonumber(stack, 2);

	Combatant *attacker = (Combatant *)battle->getEntity(attackerId);
	Combatant *attacked = (Combatant *)battle->getEntity(attackedId);

	doAttack(attacker, attacked);

	return 0;
}


static int CBattleAddSlimeEffect(lua_State *stack)
{
	int id = (int)lua_tonumber(stack, 1);

	Combatant *c = (Combatant *)battle->getEntity(id);

	battle->addEntity(new SlimeEffect(c));

	return 0;
}


static int CBattleAddFire3Effect(lua_State *stack)
{
	int id = (int)lua_tonumber(stack, 1);

	Combatant *c = (Combatant *)battle->getEntity(id);

	battle->addEntity(new Fire3Effect(c));

	return 0;
}

static int CBattleAddExplodeEffect(lua_State *stack)
{
	loadPlayDestroy("enemy_explosion.ogg");
	int id = (int)lua_tonumber(stack, 1);

	Combatant *c = (Combatant *)battle->getEntity(id);

	battle->addEntity(new ExplodeEffect(c));

	return 0;
}

static int CBattleSetInfoUnionI(lua_State *stack)
{
	int id = (int)lua_tonumber(stack, 1);
	int n = (int)lua_tonumber(stack, 2);

	Combatant *c = (Combatant *)battle->getEntity(id);

	if (!c) {
		debug_message("GetBattleSetInfoUnionI: Can't find combatant with id %d\n", id);
		return 0;
	}

	c->getInfo().i = n;

	return 0;
}


static int CBattleGetNumEnemies(lua_State *stack)
{
	lua_pushnumber(stack, battle->getNumEnemies());
	return 1;
}


static int CSetTimer(lua_State *stack)
{
	timer_time = 1000*(int)lua_tonumber(stack, 1);
	timer_on = (bool)lua_toboolean(stack, 2);
	return 0;
}


static int CGetTimer(lua_State *stack)
{
	lua_pushnumber(stack, timer_time/1000);
	lua_pushboolean(stack, timer_on);
	return 2;
}


static int CPlayMusic(lua_State *stack)
{
	const char *name = lua_tostring(stack, 1);
	playMusic(std::string(name));
	return 0;
}


static int CAddLight(lua_State *stack)
{
	int x = (int)lua_tonumber(stack, 1);
	int y = (int)lua_tonumber(stack, 2);
	int dir = (int)(lua_tonumber(stack, 3)-1);
	int topw = (int)lua_tonumber(stack, 4);
	int bottomw = (int)lua_tonumber(stack, 5);
	int length = (int)lua_tonumber(stack, 6);
	int r = (int)lua_tonumber(stack, 7);
	int g = (int)lua_tonumber(stack, 8);
	int b = (int)lua_tonumber(stack, 9);
	int a = (int)lua_tonumber(stack, 10);

	MCOLOR color = m_map_rgba(r, g, b, a);
	Object *o = new Light(x, y, dir, topw, bottomw, length, color);
	area->addObject(o);

	return 0;
}

static int CGetPlayerNames(lua_State *stack)
{
	for (int i = 0; i < 4; i++) {
		if (party[i]) {
			lua_pushstring(stack, party[i]->getName().c_str());
		}
		else {
			lua_pushnil(stack);
		}
	}

	return 4;
}


static int CAddFish(lua_State *stack)
{
	int x = (int)lua_tonumber(stack, 1);
	int y = (int)lua_tonumber(stack, 2);

	Fish *f = new Fish(x, y);

	area->addObject(f);

	return 0;
}


static int CFollowPlayer(lua_State *stack)
{
	bool f = (bool)lua_toboolean(stack, 1);

	area->followPlayer(f);

	return 0;
}

static int CAddManChooser(lua_State *stack)
{
	dpad_off();

	int num = (int)lua_tonumber(stack, 1);

	std::vector<MMan> mans;

	for (int i = 0; i < num; i++) {
		int x = (int)lua_tonumber(stack, i*3+2);
		int y = (int)lua_tonumber(stack, i*3+3);
		bool go = (bool)lua_toboolean(stack, i*3+4);
		MMan m;
		m.x = x;
		if (go) y = 6;
		m.y = y;
		m.go = go;
		m.used = false;
		m.dead = false;
		mans.push_back(m);
	}

	manChooser = new MManSelector(mans);

	astar_stop();

	tguiSetParent(0);
	tguiAddWidget(manChooser);
	tguiSetFocus(manChooser);

	return 0;
}


static int CDestroyManChooser(lua_State *stack)
{
	delete manChooser;
	manChooser = NULL;
	dpad_on();
	return 0;
}


static int CBattleSetSubAnimation(lua_State *stack)
{
	int id = (int)lua_tonumber(stack, 1);
	const char *name = lua_tostring(stack, 2);

	CombatEnemy *e = (CombatEnemy *)battle->getEntity(id);

	if (e) {
		e->getAnimationSet()->setSubAnimation(name);
	}

	return 0;
}


static int CBattleGetWidth(lua_State *stack)
{
	int id = (int)lua_tonumber(stack, 1);

	Combatant *c = (Combatant *)battle->getEntity(id);

	lua_pushnumber(stack, c->getAnimationSet()->getWidth());

	return 1;
}


static int CBattleGetHeight(lua_State *stack)
{
	int id = (int)lua_tonumber(stack, 1);

	Combatant *c = (Combatant *)battle->getEntity(id);

	lua_pushnumber(stack, c->getAnimationSet()->getHeight());

	return 1;
}


static int CMarkMan(lua_State *stack)
{
	int index = (int)lua_tonumber(stack, 1);
	bool used = (bool)lua_toboolean(stack, 2);
	bool dead = (bool)lua_toboolean(stack, 3);

	manChooser->mark(index, used, dead);

	return 0;
}


static int CPushPlayers(lua_State *stack)
{
	for (int i = 0; i < MAX_PARTY; i++) {
		pushed_players[i] = party[i];
	}

	return 0;
}



static int CPopPlayers(lua_State *stack)
{
	for (int i = 0; i < MAX_PARTY; i++) {
		party[i] = pushed_players[i];
		pushed_players[i] = NULL;
	}

	return 0;
}


static int CPopPlayersNoClear(lua_State *stack)
{
	for (int i = 0; i < MAX_PARTY; i++) {
		party[i] = pushed_players[i];
	}

	return 0;
}


static int CClearPushedPlayers(lua_State *stack)
{
	for (int i = 0; i < MAX_PARTY; i++) {
		pushed_players[i] = NULL;
	}

	return 0;
}


static int CClearPlayers(lua_State *stack)
{
	for (int i = 0; i < MAX_PARTY; i++)
		party[i] = NULL;
	return 0;
}



static int CAddPlayer(lua_State *stack)
{
	const char *name = lua_tostring(stack, 1);

	for (int i = 0; i < MAX_PARTY; i++) {
		Player *p = pushed_players[i];
		if (p && p->getName() == std::string(name)) {
			for (int j = 0; j < MAX_PARTY; j++) {
				if (!party[j]) {
					party[j] = p;
					return 0;
				}
			}
		}
	}

	printf("AddPlayer falling through\n");

	return 0;
}


static int CCreateGuards(lua_State *stack)
{
	strategy_players = new Player *[8];

	for (int i = 0; i < 8; i++) {
		Player * p = new Player("Guard", false);
		p->setInfo(guardStartInfo);
		strategy_players[i] = p;
	}

	return 0;
}


static int CDestroyGuards(lua_State *stack)
{
	for (int i = 0; i < 8; i++) {
		delete strategy_players[i];
	}

	delete[] strategy_players;

	return 0;
}


static int CAddGuard(lua_State *stack)
{
	int n = (int)lua_tonumber(stack, 1);

	for (int i = 0; i < MAX_PARTY; i++) {
		if (!party[i]) {
			party[i] = strategy_players[n];
			return 0;
		}
	}

	return 0;
}


static int CHideManChooser(lua_State *stack)
{
	tguiDeleteWidget(manChooser);
	return 0;
}


static int CShowManChooser(lua_State *stack)
{
	tguiSetParent(0);
	tguiAddWidget(manChooser);
	tguiSetFocus(manChooser);
	return 0;
}



static int CBattleWon(lua_State *stack)
{
	lua_pushboolean(stack, battle_won);
	return 1;
}


static int CBattleIsDead(lua_State *stack)
{
	int n = (int)lua_tonumber(stack, 1);

	lua_pushboolean(stack, party[n]->getInfo().abilities.hp <= 0);

	return 1;
}


static int CRemovePlayer(lua_State *stack)
{
	const char *name = lua_tostring(stack, 1);

	for (int i = 0; i < MAX_PARTY; i++) {
		if (party[i] && party[i]->getName() == std::string(name)) {
			delete party[i];
			party[i] = NULL;
			break;
		}
	}
	
	for (int j = 0; j < MAX_PARTY-1; j++) {
		if (party[j] == NULL) {
			for (int k = j+1; k < MAX_PARTY; k++) {
				if (party[k] != NULL) {
					party[j] = party[k];
					party[k] = NULL;
					break;
				}
			}
		}
	}

	heroSpot = -1;

	for (int i = 0; i < MAX_PARTY; i++) {
		if (party[i] && party[i]->getName() == "Eny") {
			heroSpot = i;
			break;
		}
	}

	return 0;
}



static int CAllPointsUp(lua_State *stack)
{
	int points = (int)lua_tonumber(stack, 1);

	for (int i = 0; i < MAX_PARTY; i++) {
		if (party[i])
			while (levelUp(party[i], points))
				;
	}

	return 0;
}


static int CDoShooter(lua_State *stack)
{
	bool for_points = lua_toboolean(stack, 1);
	lua_pushboolean(stack, shooter(for_points));
	return 1;
}

static int CDoArchery(lua_State *stack)
{
	bool for_points = lua_toboolean(stack, 1);
	lua_pushboolean(stack, archery(for_points));
	return 1;
}

static int CPrompt(lua_State *stack)
{
	const char *s1 = lua_tostring(stack, 1);
	const char *s2 = lua_tostring(stack, 2);
	int shake_def = (int)lua_tonumber(stack, 3);
	int def = (int)lua_tonumber(stack, 4);

	bool result = prompt(std::string(s1), std::string(s2), shake_def, def);

	lua_pushboolean(stack, result);

	return 1;
}


static int CTriplePrompt(lua_State *stack)
{
	const char *s1 = lua_tostring(stack, 1);
	const char *s2 = lua_tostring(stack, 2);
	const char *s3 = lua_tostring(stack, 3);
	const char *b1 = lua_tostring(stack, 4);
	const char *b2 = lua_tostring(stack, 5);
	const char *b3 = lua_tostring(stack, 6);
	bool shake_def = (bool)lua_toboolean(stack, 7);

	int result = triple_prompt(s1, s2, s3, b1, b2, b3, shake_def);

	lua_pushnumber(stack, result);

	return 1;
}


static int CNotify(lua_State *stack)
{
	const char *s1 = lua_tostring(stack, 1);
	const char *s2 = lua_tostring(stack, 2);
	const char *s3 = lua_tostring(stack, 3);

	notify(std::string(s1), std::string(s2), std::string(s3));

	return 0;
}


static int CSetTileSolid(lua_State *stack)
{
	int x = (int)lua_tonumber(stack, 1);
	int y = (int)lua_tonumber(stack, 2);
	bool solid = (bool)lua_toboolean(stack, 3);

	Tile *t = area->getTile(x, y);
	t->setSolid(solid);

	return 0;
}


static int CSetTileLayer(lua_State *stack)
{
	int x = (int)lua_tonumber(stack, 1);
	int y = (int)lua_tonumber(stack, 2);
	int l = (int)lua_tonumber(stack, 3);
	int v = (int)lua_tonumber(stack, 4);

	Tile *t = area->getTile(x, y);
	t->setAnimationNum(l, v);
	if (v != -1) {
		std::vector<int>& tileAnimationNums = area->getAnimationNums();
		int n = area->newmap[tileAnimationNums[v]];
		t->setTU(l, (n % area->tm_w) * TILE_SIZE);
		t->setTV(l, (n / area->tm_w) * TILE_SIZE);
	}

	return 0;
}

static int CGetTileLayer(lua_State *stack)
{
	int x = (int)lua_tonumber(stack, 1);
	int y = (int)lua_tonumber(stack, 2);
	int layer = (int)lua_tonumber(stack, 3);

	Tile *t = area->getTile(x, y);
	lua_pushnumber(stack, t->getAnimationNum(layer));
	
	return 1;
}

static int CGetAnimationNum(lua_State *stack)
{
	int tileNum = (int)lua_tonumber(stack, 1);

	std::vector<int> &animNums = area->getAnimationNums();

	int ret = -1;

	for (unsigned int i = 0; i < animNums.size(); i++) {
		if (animNums[i] == tileNum) {
			ret = i;
			break;
		}
	}

	lua_pushnumber(stack, ret);

	return 1;
}


static int CAreaTileToTilemap(lua_State *stack)
{
	int tileNum = (int)lua_tonumber(stack, 1);

	std::vector<int> &animNums = area->getAnimationNums();

	if (tileNum < (int)animNums.size()) {
		lua_pushnumber(stack, -1);
	}
	else {
		lua_pushnumber(stack, animNums[tileNum]);
	}

	return 1;
}


static int CRest(lua_State *stack)
{
	double seconds = lua_tonumber(stack, 1);
	m_rest(seconds);
	return 0;
}


static int CFlip(lua_State *stack)
{
	drawBufferToScreen();
	m_flip_display();
	return 0;
}


static int CGetPlayerHP(lua_State *stack)
{
	int who = (int)lua_tonumber(stack, 1);

	if (party[who]) {
		lua_pushnumber(stack, party[who]->getInfo().abilities.hp);
	}
	else {
		lua_pushnumber(stack, -1);
	}

	return 1;
}


static int CSetPlayerHP(lua_State *stack)
{
	int who = (int)lua_tonumber(stack, 1);
	int amount = (int)lua_tonumber(stack, 2);

	if (party[who]) {
		party[who]->getInfo().abilities.hp = amount;
	}

	return 0;
}


static int CGiveGold(lua_State *stack)
{
	int n = (int)lua_tonumber(stack, 1);

	gold += n;
	if (gold > MAX_GOLD)
		gold = MAX_GOLD;

	return 0;
}


static int CGameOver(lua_State *stack)
{
	if (saveFilename) saveTime(saveFilename);
	fadeOut(m_map_rgb(255, 0, 0));
	break_main_loop = true;
	return 0;
}


static int CGameOverNoFade(lua_State *stack)
{
	if (saveFilename) saveTime(saveFilename);
	break_main_loop = true;
	return 0;
}


static int CBattleAddMessage(lua_State *stack)
{
	MessageLocation loc = (MessageLocation)((int)lua_tonumber(stack, 1));
	const char *text = lua_tostring(stack, 2);
	int timeout = (int)lua_tonumber(stack, 3);

	battle->addMessage(loc, std::string(text), timeout);

	return 0;
}


static int CDoVolcanoEndScene(lua_State *stack)
{
	volcano_scene();
	return 0;
}


static int CStartRocket(lua_State *stack)
{
	int id = (int)lua_tonumber(stack, 1);

	Rocket *r = (Rocket *)area->findObject(id);
	r->start();

	return 0;
}


static int CAddRocket(lua_State *stack)
{
	int x = (int)lua_tonumber(stack, 1);
	int y = (int)lua_tonumber(stack, 2);
	const char *spriteName = lua_tostring(stack, 3);

	Rocket *r = new Rocket(x, y, std::string(spriteName));
	int id = r->getId();
	area->addObject(r);

	lua_pushnumber(stack, id);

	return 1;
}



static int CDoLander(lua_State *stack)
{
	lander_success = do_lander();
	return 0;
}

static int CLanderSuccess(lua_State *stack)
{
	lua_pushboolean(stack, lander_success);
	return 1;
}


static int Cinto_the_sun(lua_State *stack)
{
	into_the_sun();
	return 0;
}


static int Ccredits(lua_State *stack)
{
	credits();
	return 0;
}

static int CGetPlatform(lua_State *stack)
{
#if defined ALLEGRO_IPHONE
	lua_pushstring(stack, "iphone");
#elif defined ALLEGRO_ANDROID
	lua_pushstring(stack, "android");
#elif defined ALLEGRO_MACOSX
	lua_pushstring(stack, "mac");
#else
	lua_pushstring(stack, "pc");
#endif

	return 1;
}

static int CGetPlayerDest(lua_State *stack)
{
	if ((have_mouse && path_head) || !use_dpad) {
		Node *tail = get_path_tail();
		if (tail) {
			lua_pushnumber(stack, tail->x);
			lua_pushnumber(stack, tail->y);
			return 2;
		}
	}
	if (use_dpad) {
		if (area) {
			Object *o = area->findObject(0);
			if (o) {
				lua_pushnumber(stack, o->getX());
				lua_pushnumber(stack, o->getY());
				return 2;
			}
		}
	}

	lua_pushnil(stack);
	lua_pushnil(stack);
	return 2;
}


static int CGetAreaPan(lua_State *stack)
{
	lua_pushnumber(stack, area_panned_x);
	lua_pushnumber(stack, area_panned_y);
	return 2;
}

static int CSetAreaPan(lua_State *stack)
{
	int xpan = lua_tonumber(stack, 1);
	int ypan = lua_tonumber(stack, 2);
	area_panned_x = xpan;
	area_panned_y = ypan;
	return 0;
}

static int CDoItemTutorial(lua_State *stack)
{
	dpad_off();

	bool equipChooserAdded = false;

	bool ret = false;

	int stage = 0;

	inventory[0].index = 0;
	inventory[0].quantity = 1;

	MRectangle *fullscreenRect = new MRectangle(0, 0, 0, 0,
		blue, M_FILLED);
	MPartySelector *partySelectorTop = new MPartySelector(3, 0, true);
	MItemSelector *itemSelector = new MItemSelector(73, BH-2, 0, 0, true);

	std::vector<MultiPoint> equipPoints;
	for (int i = 0; i < 5; i++) {
		MultiPoint p;
		p.x = BW/2+10;
		p.y = 2 + (m_text_height(game_font)/2) + (m_text_height(game_font)*i) + (i*4);
		p.west = false;
		equipPoints.push_back(p);
	}
	MMultiChooser *equipChooser = new MMultiChooser(equipPoints, false);

	// Add widgets
	tguiSetParent(0);
	tguiAddWidget(fullscreenRect);
	partySelectorTop->setSelected(0);
	tguiAddWidget(partySelectorTop);
	itemSelector->setSelected(0);
	itemSelector->setTop(0);
	tguiAddWidget(itemSelector);
	tguiSetFocus(partySelectorTop);

	m_set_target_bitmap(buffer);
	tguiDraw();
	drawBufferToScreen();
	m_flip_display();

	#define DLG(t, c) anotherDoDialogue(t, c, !c)

	if (use_dpad) {
		if (DLG("This is the inventory screen.\nFirst let's equip an item.\nMove to the sword in the bottom pane and select it by pressing twice.\n", false))
			{ if (prompt("Really exit", "tutorial?", 0, 1)) { ret = true; goto done; }}
	}
	else {
		if (DLG("This is the inventory screen.\nFirst let's equip an item.\nDrag the sword into the player selector at the top.\n", false))
			{ if (prompt("Really exit", "tutorial?", 0, 1)) { ret = true; goto done; }}
	}

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
			// WARNING
			if (break_main_loop) {
				goto done;
			}
		
			// update gui
			TGUIWidget *widget = tguiUpdate();

			if (stage == 0) {
				if (widget == itemSelector) {
					int sel = itemSelector->getSelected();
					int who = partySelectorTop->getSelected();
					if (sel >= 0 && who == 0) {
						player->getInfo().equipment.rhand = inventory[sel].index;
						player->getInfo().equipment.rquantity = 1;
						inventory[sel].index = -1;
						inventory[sel].quantity = 0;
					}
					else if (sel < 0) {
						if (prompt("Really exit", "tutorial?", 0, 1))
						{ ret = true; goto done; }
						itemSelector->setSelected(0);
					}
				}
				else if (widget == partySelectorTop && partySelectorTop->getSelected() == -1) {
					if (prompt("Really exit", "tutorial?", 0, 1))
					{ ret = true; goto done; }
					partySelectorTop->setSelected(0);
				}
				if (player->getInfo().equipment.rhand != -1) {
					stage++;
									if (use_dpad) {
						if (DLG("Excellent!\nNow let's learn how to un-equip an item.\nMove to the top frame, press the action button, then move to and select the sword.\n", false))
							{ if (prompt("Really exit", "tutorial?", 0, 1)) { ret = true; goto done; }}
					}
					else {
						if (DLG("Excellent!\nNow let's learn how to un-equip an item.\nDrag the sword from the top frame to the bottom frame.\n", false))
							{ if (prompt("Really exit", "tutorial?", 0, 1)) { ret = true; goto done; }}
					}
								}
			}
			else if (stage == 1) {
				if (widget == itemSelector && itemSelector->getSelected() == -1) {
					if (prompt("Really exit", "tutorial?", 0, 1))
					{ ret = true; goto done; }
					itemSelector->setSelected(0);
				}
				else if (widget == partySelectorTop) {
					int sel = partySelectorTop->getSelected();
					if (sel == -1) {
						if (prompt("Really exit", "tutorial?", 0, 1))
						{ ret = true; goto done; }
						partySelectorTop->setSelected(0);
					}
					else if (sel != MAX_PARTY) {
						if (use_dpad) {
							if (!equipChooserAdded) {
								std::vector<int> i;
								i.push_back(0);
								equipChooser->setSelected(i);
								i.clear();
								tguiSetParent(0);
								tguiAddWidget(equipChooser);
								tguiSetFocus(equipChooser);
								equipChooserAdded = true;
							}
						}
						else {
							player->getInfo().equipment.rhand = -1;
							player->getInfo().equipment.rquantity = 0;
							stage++;
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
							if (DLG("Great!\nNow I will show you how to use some other basic items...\nYou use an item just like you use a weapon...\nDrag the Cure to your player at the top to use it.\nYou can also press and hold an item name to get a description.\n", false)) { if (prompt("Really exit", "tutorial?", 0, 1)) { ret = true; goto done; }}
#else
							if (DLG("Great!\nNow I will show you how to use some other basic items...\nYou use an item just like you use a weapon...\nDrag the Cure to your player at the top to use it.\nYou can also press button 3 (default \"V\") to get a description.\n", false)) { if (prompt("Really exit", "tutorial?", 0, 1)) { ret = true; goto done; }}
#endif
													inventory[0].index = CURE_INDEX;
							inventory[0].quantity = 1;
						}
					}
				}
				if (widget == equipChooser) {
					std::vector<int> &chosen = equipChooser->getSelected();
					if (chosen.size() > 0 && chosen[0] >= 0) {
						int who = partySelectorTop->getSelected();
						if (party[who]) {
							if (chosen[0] == 1) {
								stage++;
								player->getInfo().equipment.rhand = -1;
								player->getInfo().equipment.rquantity = 0;
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
								if (DLG("Great!\nNow I will show you how to use some other basic items...\nYou use an item just like you use a weapon...\nPress twice on the Cure to use it.\nYou can also press and hold an item name to get a description.\n", false)) { if (prompt("Really exit", "tutorial?", 0, 1)) { ret = true; goto done; }}
#else
								if (DLG("Great!\nNow I will show you how to use some other basic items...\nYou use an item just like you use a weapon...\nPress twice on the Cure to use it.\nYou can also press button 3 (default \"V\") to get a description.\n", false)) { if (prompt("Really exit", "tutorial?", 0, 1)) { ret = true; goto done; }}
#endif
								inventory[0].index = CURE_INDEX;
								inventory[0].quantity = 1;
								tguiDeleteWidget(equipChooser);
								tguiSetFocus(itemSelector);
							}
						}
						else {
							tguiDeleteWidget(equipChooser);
							tguiSetFocus(partySelectorTop);
						}
					}
					else {
						tguiDeleteWidget(equipChooser);
						tguiSetFocus(partySelectorTop);
					}
					equipChooserAdded = false;
				}
			}
			else if (stage == 2) {
				if (widget == itemSelector) {
					int sel = itemSelector->getSelected();
					int who = partySelectorTop->getSelected();
					if (sel >= 0 && inventory[sel].index >= 0 && who == 0) {
						loadPlayDestroy("Cure.ogg");
						inventory[sel].index = -1;
						inventory[sel].quantity = 0;
						stage++;
											if (use_dpad) {
							if (DLG("Ok, you're almost done.\nLet's try one battle just to get our feet wet...\nAttacking works by moving to an arrow next to the enemy you wish to attack.\nThen you press the action button.\nUsing items works the same way.\nYou have several other options in battle... Play around with them to make yourself familiar!\n", false)) { if (prompt("Really exit", "tutorial?", 0, 1)) { ret = true; goto done; }}
						}
						else {
							if (DLG("Ok, you're almost done.\nLet's try one battle just to get our feet wet...\nAttacking works by pressing an arrow next to the enemy you wish to attack.\nThen you draw an invisible line in the direction of the arrow.\nUsing items works the same way.\nYou have several other options in battle... Play around with them to make yourself familiar!\n", false)) { if (prompt("Really exit", "tutorial?", 0, 1)) { ret = true; goto done; }}
							if (DLG("Remember, draw an invisible line in the direction of the arrows to perform an action!\nThat includes attacking, using items, casting spells, etc.\nHere we go!\n", false)) { if (prompt("Really exit", "tutorial?", 0, 1)) { ret = true; goto done; }}
						}
											goto done;
					}
					else if (sel < 0) {
						if (prompt("Really exit", "tutorial?", 0, 1))
						{ ret = true; goto done; }
						itemSelector->setSelected(0);
					}
				}
				else if (widget == partySelectorTop && partySelectorTop->getSelected() == -1) {
					if (prompt("Really exit", "tutorial?", 0, 1))
					{ ret = true; goto done; }
					partySelectorTop->setSelected(0);
				}
			}
		}

		if (draw_counter > 0) {
			draw_counter = 0;
			m_set_target_bitmap(buffer);
			m_clear(black);
			tguiDraw();
			
			drawBufferToScreen();
			m_flip_display();
		}
	}

done:
	if (equipChooserAdded) {
		tguiDeleteWidget(equipChooser);
		tguiSetFocus(partySelectorTop);
	}

	tguiDeleteWidget(fullscreenRect);
	tguiDeleteWidget(partySelectorTop);
	tguiDeleteWidget(itemSelector);

	delete fullscreenRect;
	delete partySelectorTop;
	delete itemSelector;
	delete equipChooser;
	equipPoints.clear();

	if (getInput()) {
		getInput()->set(false, false, false, false, false, false, false);
		getInput()->setDirection(DIRECTION_SOUTH);
	}

	lua_pushboolean(stack, ret);

	dpad_on();
	
	return 1;
}

static int CStartsWithVowel(lua_State *stack)
{
	const char *str = lua_tostring(stack, 1);

	lua_pushboolean(stack, isVowel(str[0]));

	return 1;
}

static int CDoMapTutorial(lua_State *stack)
{
	dpad_off();

	bool ret = false;

	GameInfo backup;
	memcpy(backup.milestones, gameInfo.milestones, sizeof(GameInfo));
	memset(gameInfo.milestones, 0, sizeof(GameInfo));
	gameInfo.milestones[MS_MET_HORSE] = true;

	Input *input = getInput();
	int dir = input->getDirection();

	MMap *mapWidget = new MMap("forest", "map");

	tguiSetParent(0);
	tguiAddWidget(mapWidget);
	tguiSetFocus(mapWidget);

	m_set_target_bitmap(buffer);
	tguiDraw();
	drawBufferToScreen();
	m_flip_display();

	
	#define DLG(t, c) anotherDoDialogue(t, c, !c)

	if (use_dpad) {
		if (DLG("Each area on the map has branches coming off of it.\nUse the arrows to move between areas. If you press on the icon with the arrow, you will enter that area.\nMove the cursor to Seaside town and then press the action button.\n", false))  { if (prompt("Really exit", "tutorial?", 0, 1)) { ret = true; goto done; }};
	}
	else {
		if (DLG("Each area on the map has branches coming off of it.\nPress an icon to move in that direction. If you press on the icon with the arrow, you will enter that area.\nMove the cursor to Seaside town and then press the icon.\n", false))  { if (prompt("Really exit", "tutorial?", 0, 1)) { ret = true; goto done; }};
	}


	mapWidget->flash();

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
			// WARNING
			if (break_main_loop) {
				goto done;
			}
			// update gui
			std::string startingName = mapWidget->getSelected();

			TGUIWidget *widget = tguiUpdate();
			if (widget == mapWidget) {
				ALLEGRO_DEBUG("tguiUpdate returned");
				if (mapWidget->getSelected() == "seaside") {
					goto done;
				}
			}

			if (iphone_shaken(0.1)) {
				iphone_clear_shaken();
				if (prompt("Really exit", "tutorial?", 0, 1)) {
					ret = true;
					goto done;
				}
			}
		}

		if (draw_counter > 0) {
			draw_counter = 0;
			m_set_target_bitmap(buffer);
			MCOLOR color = black;
			m_clear(color);
			// Draw the GUI
			tguiDraw();
			drawBufferToScreen();
			 // FIXME
			m_flip_display();
		}
	}

done:

	tguiDeleteWidget(mapWidget);
	delete mapWidget;

	input->setDirection(dir);
	input->set(false, false, false, false, false, false, false);

	runtime_start = tguiCurrentTimeMillis();
	
	memcpy(gameInfo.milestones, backup.milestones, sizeof(GameInfo));

	lua_pushboolean(stack, ret);

	dpad_on();

	m_set_target_bitmap(buffer);
	m_clear(black);
	area->draw();
	drawBufferToScreen();
	m_flip_display();

	return 1;
}


static int CGetAreaWidth(lua_State *stack)
{
	lua_pushnumber(stack, area->getWidth());
	return 1;
}


static int CGetAreaHeight(lua_State *stack)
{
	lua_pushnumber(stack, area->getHeight());
	return 1;
}


static int CSetSpecialWalkable(lua_State *stack)
{
	int id = (int)lua_tonumber(stack, 1);
	bool special = lua_toboolean(stack, 2);

	Object *o = area->findObject(id);
	if (o) {
		o->setSpecialWalkable(special);
	}

	return 0;
}


static int CAnotherDoDialogue(lua_State *stack)
{
	const char *text = lua_tostring(stack, 1);
	bool clear_buf = lua_toboolean(stack, 2);

	lua_pushboolean(stack, anotherDoDialogue(text, clear_buf, !clear_buf));

	return 1;
}


static int CEnyHasWeapon(lua_State *stack)
{
	Player *eny = NULL;

	for (int i = 0; i < MAX_PARTY; i++) {
		Player *p = party[i];
		if (p && p->getName() == "Eny") {
			eny = p;
			break;
		}
	}

	if (eny) {
		lua_pushboolean(stack, eny->getInfo().equipment.rhand >= 0 ||
			eny->getInfo().equipment.lhand >= 0);
	}
	else {
		lua_pushboolean(stack, false);
	}

	return 1;
}


static int CDoKingKingAlbertLook(lua_State *stack)
{
	dpad_off();

	FakeWidget *widget = new FakeWidget(0, 0, BW, BH);
	tguiSetParent(0);
	tguiAddWidget(widget);
	tguiSetFocus(widget);

	doDialogue("It's a picture of a past King...\n... with some kind of monster?!\nHe looks friendly though!\n", false, 4, 10, true);

	bool dialogue_dismissed = false;

	MBITMAP *bmp = m_load_bitmap(getResource("media/kingkingalbert.png"));

	while (1) {
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
			// WARNING
			if (break_main_loop) {
				goto done;
			}
			
			TGUIWidget *w = tguiUpdate();
			if (!dialogue_dismissed) {
				if (w == speechDialog) {
					dialogue_dismissed = true;
					tguiDeleteWidget(speechDialog);
					delete speechDialog;
					speechDialog = NULL;
					tguiSetFocus(widget);
					dpad_on();
				}
			}
			else {
				if (w == widget) {
					if (use_dpad) {
						InputDescriptor in = getInput()->getDescriptor();
						while (in.button1) {
							in = getInput()->getDescriptor();
						}
						clear_input_events();
					}
					playPreloadedSample("select.ogg");
					goto done;
				}
			}
		}
		
		if (draw_counter > 0) {
			draw_counter = 0;
			m_set_target_bitmap(buffer);
			m_draw_bitmap(bmp, 0, 0, 0);
			tguiDraw();
			drawBufferToScreen();
			m_flip_display();
		}
	}
done:
	tguiDeleteWidget(widget);
	delete widget;

	m_destroy_bitmap(bmp);

	dpad_on();

	return 0;
}

static void get_shiney_stuff(int *x, int *y, ALLEGRO_COLOR *c)
{
	static long lastCall = -1;
	
	if (lastCall != -1) {
		if ((unsigned long)lastCall+100 > tguiCurrentTimeMillis())
			return;
	}
	
	lastCall = tguiCurrentTimeMillis();
	
	for (int i = 0; i < 7; i++) {
		// FIXME: hardcoded
		x[i] = 165 + (rand() % 7 - 3);
		y[i] = 65 + (rand() % 5 - 2);
		float a = (100 + rand() % 100) / 255.0;
		c[i] = m_map_rgba(
			(200 + rand() % 55)*a,
			(200 + rand() % 55)*a,
			0,
			a*255
		);
	}
}

static int CDoKeepLook(lua_State *stack)
{
	dpad_off();

	bool show_shine = !gameInfo.milestones[MS_FOREST_GOLD];
	
	int x[7];
	int y[7];
	ALLEGRO_COLOR c[7];
	
	get_shiney_stuff(x, y, c);
	
	FakeWidget *widget = new FakeWidget(0, 0, BW, BH);
	tguiSetParent(0);
	tguiAddWidget(widget);
	tguiSetFocus(widget);

	if (show_shine) {
		doDialogue("Eny: Look over there... there's something shining in the forest, behind those trees...\n", false, 4, 10, true);
	}

	bool dialogue_dismissed = false;

	MBITMAP *bmp = m_load_bitmap(getResource("media/forest_treasure.png"));

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
			// WARNING
			if (break_main_loop) {
				goto done;
			}

			TGUIWidget *w = tguiUpdate();
			if (show_shine && !dialogue_dismissed) {
				if (w == speechDialog) {
					dialogue_dismissed = true;
					tguiDeleteWidget(speechDialog);
					delete speechDialog;
					speechDialog = NULL;
					tguiSetFocus(widget);
					dpad_on();
				}
			}
			else {
				if (w == widget) {
					if (use_dpad) {
						InputDescriptor in = getInput()->getDescriptor();
						while (in.button1) {
							in = getInput()->getDescriptor();
						}
						clear_input_events();
					}
					playPreloadedSample("select.ogg");
					goto done;
				}
			}
			get_shiney_stuff(x, y, c);
		}

		if (draw_counter > 0) {
			draw_counter = 0;

			m_set_target_bitmap(buffer);
			m_draw_bitmap(bmp, 0, 0, 0);
			if (show_shine) {
				m_save_blender();
				m_set_blender(M_ALPHA, M_ONE, white);
				for (int i = 0; i < 7; i++) {
					m_draw_pixel(x[i], y[i], c[i]);
				}
				m_restore_blender();
			}
			tguiDraw();
			drawBufferToScreen();
			m_flip_display();
		}
	}
done:
	tguiDeleteWidget(widget);
	delete widget;

	m_destroy_bitmap(bmp);

	dpad_on();

	return 0;
}


static int CSetPlayerFormation(lua_State *stack)
{
	int spot = lua_tonumber(stack, 1);
	CombatFormation form = (CombatFormation)((int)lua_tonumber(stack, 2));

	party[spot]->setFormation(form);

	return 0;
}


static Player *get_beach_battle_player(int n)
{
	if (n < 0 || n >= MAX_PARTY)
		return NULL;

	std::string names[MAX_PARTY] = {
		"Eny", "Rider", "Rios", "Gunnar"
	};

	Player *found = NULL;

	for (int i = 0; i < MAX_PARTY; i++) {
		Player *pl = party[i];
		if (!pl)
			continue;
		if (pl->getName() == names[n]) {
			found = pl;
			break;
		}
	}

	return found;
}


static int CShowBeachBattleInfo(lua_State *stack)
{
	int who = lua_tonumber(stack, 1);

	if (who < 4) {
		showPlayerInfo_ptr(strategy_players[who]);
	}
	else if (who >= 8) {
		showPlayerInfo_ptr(strategy_players[who-4]);
	}
	else if (who >= 4 && who < 8) {
		int n = who - 4;
		Player *p = get_beach_battle_player(n);
		if (p)
			showPlayerInfo_ptr(p);
	}

	return 0;
}


static int CSetGlobalCanSave(lua_State *stack)
{
	bool gcs = (bool)lua_toboolean(stack, 1);

	global_can_save = gcs;

	return 0;
}


static int CDpadOff(lua_State *stack)
{
	dpad_off();
	return 0;
}


static int CDpadOn(lua_State *stack)
{
	dpad_on();
	return 0;
}

static int CGetUseDpad(lua_State *stack)
{
	lua_pushboolean(stack, use_dpad);
	return 1;
}

static int CGetBreakMainLoop(lua_State *stack)
{
	lua_pushboolean(stack, break_main_loop);
	return 1;
}


static int CSetGonnaFadeInRed(lua_State *stack)
{
	gonna_fade_in_red = lua_toboolean(stack, 1);
	return 0;
}

static int CSetAreaFocus(lua_State *state)
{
	int x = lua_tonumber(state, 1);
	int y = lua_tonumber(state, 2);
	area->setFocus(x, y);
	return 0;
}

static int CSetGlobalDrawControls(lua_State *stack)
{
	bool v = (bool)lua_toboolean(stack, 1);

	global_draw_controls = v;

	return 0;
}

static int CSetGlobalDrawRed(lua_State *stack)
{
	bool v = (bool)lua_toboolean(stack, 1);

	global_draw_red = v;

	return 0;
}

static int CSetRedOffPressOn(lua_State *stack)
{
	bool v = (bool)lua_toboolean(stack, 1);

	red_off_press_on = v;

	return 0;
}

static int CGetDpadType(lua_State *stack)
{
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
	lua_pushnumber(stack, config.getDpadType());
#else
	lua_pushnumber(stack, DPAD_TOTAL_1);
#endif
	return 1;
}

static int C_t(lua_State *stack)
{
	const char *s = lua_tostring(stack, 1);

	lua_pushstring(stack, _t(s));

	return 1;
}

static int C_extra_battle_chances(lua_State *stack)
{
	switch (config.getDifficulty()) {
		case CFG_DIFFICULTY_EASY:
			lua_pushnumber(stack, 10);
			break;
		case CFG_DIFFICULTY_HARD:
			lua_pushnumber(stack, -5);
			break;
		default:
			lua_pushnumber(stack, 0);
			break;
	}

	return 1;
}

static int C_dbg(lua_State *stack)
{
	const char *t = lua_tostring(stack, 1);

	ALLEGRO_DEBUG("%s", t);

	return 0;
}

static int C_BonusPoints(lua_State *stack)
{
	increaseGold(5000);
	for (int i = 0; i < MAX_PARTY; i++) {
		if (party[i])
			while (levelUp(party[i], 50))
				;
	}

	return 0;
}

struct SFX {
	int id;
	MSAMPLE sample;
};
static int id_counter = 1;
static std::vector<SFX> sfx;

static int CLoadSample(lua_State *stack)
{
	const char *name = lua_tostring(stack, 1);

	SFX s;
	s.id = id_counter++;
	s.sample = loadSample(name);

	sfx.push_back(s);

	lua_pushnumber(stack, s.id);

	return 1;
}

static int CDestroySample(lua_State *stack)
{
	int id = lua_tonumber(stack, 1);

	for (size_t i = 0; i < sfx.size(); i++) {
		if (sfx[i].id == id) {
			destroySample(sfx[i].sample);
			sfx.erase(sfx.begin() + i);
			break;
		}
	}

	return 0;
}

static int CPlaySample(lua_State *stack)
{
	int id = lua_tonumber(stack, 1);

	for (size_t i = 0; i < sfx.size(); i++) {
		if (sfx[i].id == id) {
			playSample(sfx[i].sample);
			break;
		}
	}

	return 0;
}

static int CPreloadSFX(lua_State *stack)
{
	const char *name = lua_tostring(stack, 1);

	preloadSFX(name);

	return 0;
}

static int CPreloadSpellSFX(lua_State *stack)
{
	const char *name = lua_tostring(stack, 1);

	preloadSpellSFX(name);

	return 0;
}

int CDebug(lua_State *stack)
{
	printf("%s\n", lua_tostring(stack, 1));
	return 0;
}

/*
 * This registers all the required C/C++ functions
 * with the Lua interpreter, so they can be called
 * from scripts.
 */
void registerCFunctions(lua_State* luaState)
{
	/* Area-related */

	lua_pushcfunction(luaState, CStartArea);
	lua_setglobal(luaState, "startArea");

	lua_pushcfunction(luaState, CGetMilestone);
	lua_setglobal(luaState, "getMilestone");

	lua_pushcfunction(luaState, CSetMilestone);
	lua_setglobal(luaState, "setMilestone");

	lua_pushcfunction(luaState, CDrawArea);
	lua_setglobal(luaState, "drawArea");

	lua_pushcfunction(luaState, CUpdateArea);
	lua_setglobal(luaState, "updateArea");

	lua_pushcfunction(luaState, CFindEmptyInventorySlot);
	lua_setglobal(luaState, "findEmptyInventorySlot");

	lua_pushcfunction(luaState, CFindUnfullInventorySlot);
	lua_setglobal(luaState, "findUnfullInventorySlot");

	lua_pushcfunction(luaState, CFindUsedInventorySlot);
	lua_setglobal(luaState, "findUsedInventorySlot");

	lua_pushcfunction(luaState, CSetInventory);
	lua_setglobal(luaState, "setInventory");

	lua_pushcfunction(luaState, CGetInventory);
	lua_setglobal(luaState, "getInventory");

	lua_pushcfunction(luaState, CGetItemName);
	lua_setglobal(luaState, "getItemName");

	lua_pushcfunction(luaState, CGetItemIcon);
	lua_setglobal(luaState, "getItemIcon");

	lua_pushcfunction(luaState, CDoMap);
	lua_setglobal(luaState, "doMap");

	lua_pushcfunction(luaState, CAreaIsSolid);
	lua_setglobal(luaState, "areaIsSolid");

	lua_pushcfunction(luaState, CFollowPlayer);
	lua_setglobal(luaState, "followPlayer");

	lua_pushcfunction(luaState, CAddManChooser);
	lua_setglobal(luaState, "addManChooser");

	lua_pushcfunction(luaState, CDestroyManChooser);
	lua_setglobal(luaState, "destroyManChooser");

	lua_pushcfunction(luaState, CSetTileSolid);
	lua_setglobal(luaState, "setTileSolid");

	lua_pushcfunction(luaState, CSetTileLayer);
	lua_setglobal(luaState, "setTileLayer");

	lua_pushcfunction(luaState, CGetTileLayer);
	lua_setglobal(luaState, "getTileLayer");

	lua_pushcfunction(luaState, CGetAnimationNum);
	lua_setglobal(luaState, "getAnimationNum");

	lua_pushcfunction(luaState, CAreaTileToTilemap);
	lua_setglobal(luaState, "areaTileToTilemap");

	/* Object-related */

	lua_pushcfunction(luaState, CAddObject);
	lua_setglobal(luaState, "addObject");
	
	lua_pushcfunction(luaState, CRemoveObject);
	lua_setglobal(luaState, "removeObject");
	
	lua_pushcfunction(luaState, CSetObjectPosition);
	lua_setglobal(luaState, "setObjectPosition");

	lua_pushcfunction(luaState, CSetObjectDirection);
	lua_setglobal(luaState, "setObjectDirection");

	lua_pushcfunction(luaState, CSetObjectAnimationSet);
	lua_setglobal(luaState, "setObjectAnimationSet");

	lua_pushcfunction(luaState, CSetObjectInputToScriptControlled);
	lua_setglobal(luaState, "setObjectInputToScriptControlled");

	lua_pushcfunction(luaState, CSetObjectInput);
	lua_setglobal(luaState, "setObjectInput");

	lua_pushcfunction(luaState, CGetObjectPosition);
	lua_setglobal(luaState, "getObjectPosition");

	lua_pushcfunction(luaState, CGetObjectDirection);
	lua_setglobal(luaState, "getObjectDirection");

	lua_pushcfunction(luaState, CSetObjectHigh);
	lua_setglobal(luaState, "setObjectHigh");

	lua_pushcfunction(luaState, CSetObjectLow);
	lua_setglobal(luaState, "setObjectLow");

	lua_pushcfunction(luaState, CSetObjectHidden);
	lua_setglobal(luaState, "setObjectHidden");

	lua_pushcfunction(luaState, CScriptifyPlayer);
	lua_setglobal(luaState, "scriptifyPlayer");

	lua_pushcfunction(luaState, CDeScriptifyPlayer);
	lua_setglobal(luaState, "descriptifyPlayer");

	lua_pushcfunction(luaState, CPlayerIsScripted);
	lua_setglobal(luaState, "playerIsScripted");

	lua_pushcfunction(luaState, CSetObjectSolid);
	lua_setglobal(luaState, "setObjectSolid");

	lua_pushcfunction(luaState, CSetObjectAnimationSetPrefix);
	lua_setglobal(luaState, "setObjectAnimationSetPrefix");
	
	lua_pushcfunction(luaState, CSetObjectSubAnimation);
	lua_setglobal(luaState, "setObjectSubAnimation");

	lua_pushcfunction(luaState, CSetObjectIsPerson);
	lua_setglobal(luaState, "setObjectIsPerson");

	lua_pushcfunction(luaState, CGetObjectOffset);
	lua_setglobal(luaState, "getObjectOffset");
	
	lua_pushcfunction(luaState, CSetObjectDest);
	lua_setglobal(luaState, "setObjectDest");
	
	lua_pushcfunction(luaState, CSetObjectIsFloater);
	lua_setglobal(luaState, "setObjectIsFloater");
	
	lua_pushcfunction(luaState, CStopObject);
	lua_setglobal(luaState, "stopObject");
	
	lua_pushcfunction(luaState, CUpdateObject);
	lua_setglobal(luaState, "updateObject");
	
	lua_pushcfunction(luaState, CSetObjectDimensions);
	lua_setglobal(luaState, "setObjectDimensions");
	
	lua_pushcfunction(luaState, CAddLight);
	lua_setglobal(luaState, "addLight");
	
	lua_pushcfunction(luaState, CGetPlayerNames);
	lua_setglobal(luaState, "getPlayerNames");
	
	lua_pushcfunction(luaState, CAddFish);
	lua_setglobal(luaState, "addFish");
	
	lua_pushcfunction(luaState, CRemovePlayer);
	lua_setglobal(luaState, "removePlayer");
	
	lua_pushcfunction(luaState, CGetPlayerHP);
	lua_setglobal(luaState, "getPlayerHP");
	
	lua_pushcfunction(luaState, CSetPlayerHP);
	lua_setglobal(luaState, "setPlayerHP");


	


	/* Battle functions */

	lua_pushcfunction(luaState, CAddEnemy);
	lua_setglobal(luaState, "addEnemy");

	lua_pushcfunction(luaState, CGetRandomPlayer);
	lua_setglobal(luaState, "getRandomPlayer");

	lua_pushcfunction(luaState, CGetWeakestPlayer);
	lua_setglobal(luaState, "getWeakestPlayer");

	lua_pushcfunction(luaState, CGetRandomEnemy);
	lua_setglobal(luaState, "getRandomEnemy");

	lua_pushcfunction(luaState, CStartBattle);
	lua_setglobal(luaState, "startBattle");

	lua_pushcfunction(luaState, CInBattle);
	lua_setglobal(luaState, "inBattle");

	lua_pushcfunction(luaState, CBattleGetX);
	lua_setglobal(luaState, "battleGetX");

	lua_pushcfunction(luaState, CBattleGetY);
	lua_setglobal(luaState, "battleGetY");

	lua_pushcfunction(luaState, CBattleSetOx);
	lua_setglobal(luaState, "battleSetOx");

	lua_pushcfunction(luaState, CBattleSetOy);
	lua_setglobal(luaState, "battleSetOy");

	lua_pushcfunction(luaState, CBattleSetAngle);
	lua_setglobal(luaState, "battleSetAngle");

	lua_pushcfunction(luaState, CBattleGetLocation);
	lua_setglobal(luaState, "battleGetLocation");

	lua_pushcfunction(luaState, CBattleGetEntityIndex);
	lua_setglobal(luaState, "battleGetEntityIndex");

	lua_pushcfunction(luaState, CBattleGetNextEntity);
	lua_setglobal(luaState, "battleGetNextEntity");

	lua_pushcfunction(luaState, CBattleMoveEntity);
	lua_setglobal(luaState, "battleMoveEntity");

	lua_pushcfunction(luaState, CBattleDoAttack);
	lua_setglobal(luaState, "battleDoAttack");

	lua_pushcfunction(luaState, CBattleAddSlimeEffect);
	lua_setglobal(luaState, "battleAddSlimeEffect");

	lua_pushcfunction(luaState, CBattleAddFire3Effect);
	lua_setglobal(luaState, "battleAddFire3Effect");

	lua_pushcfunction(luaState, CBattleAddExplodeEffect);
	lua_setglobal(luaState, "battleAddExplodeEffect");

	lua_pushcfunction(luaState, CBattleSetInfoUnionI);
	lua_setglobal(luaState, "battleSetInfoUnionI");

	lua_pushcfunction(luaState, CBattleGetNumEnemies);
	lua_setglobal(luaState, "battleGetNumEnemies");

	lua_pushcfunction(luaState, CBattleSetSubAnimation);
	lua_setglobal(luaState, "battleSetSubAnimation");

	lua_pushcfunction(luaState, CBattleGetWidth);
	lua_setglobal(luaState, "battleGetWidth");

	lua_pushcfunction(luaState, CBattleGetHeight);
	lua_setglobal(luaState, "battleGetHeight");

	lua_pushcfunction(luaState, CPushPlayers);
	lua_setglobal(luaState, "pushPlayers");

	lua_pushcfunction(luaState, CPopPlayers);
	lua_setglobal(luaState, "popPlayers");
	
	lua_pushcfunction(luaState, CPopPlayersNoClear);
	lua_setglobal(luaState, "popPlayersNoClear");
	
	lua_pushcfunction(luaState, CClearPushedPlayers);
	lua_setglobal(luaState, "clearPushedPlayers");
	
	lua_pushcfunction(luaState, CClearPlayers);
	lua_setglobal(luaState, "clearPlayers");

	lua_pushcfunction(luaState, CAddPlayer);
	lua_setglobal(luaState, "addPlayer");

	lua_pushcfunction(luaState, CAddGuard);
	lua_setglobal(luaState, "addGuard");

	lua_pushcfunction(luaState, CCreateGuards);
	lua_setglobal(luaState, "createGuards");

	lua_pushcfunction(luaState, CDestroyGuards);
	lua_setglobal(luaState, "destroyGuards");

	lua_pushcfunction(luaState, CHideManChooser);
	lua_setglobal(luaState, "hideManChooser");

	lua_pushcfunction(luaState, CShowManChooser);
	lua_setglobal(luaState, "showManChooser");

	lua_pushcfunction(luaState, CBattleWon);
	lua_setglobal(luaState, "battleWon");

	lua_pushcfunction(luaState, CBattleIsDead);
	lua_setglobal(luaState, "battleIsDead");

	lua_pushcfunction(luaState, CSetCombatantAnimationSetPrefix);
	lua_setglobal(luaState, "setCombatantAnimationSetPrefix");

	lua_pushcfunction(luaState, CBattleFindCombatant);
	lua_setglobal(luaState, "battleFindCombatant");

	lua_pushcfunction(luaState, CBattleLost);
	lua_setglobal(luaState, "battleLost");

	lua_pushcfunction(luaState, CBattleGetEntityType);
	lua_setglobal(luaState, "battleGetEntityType");

	lua_pushcfunction(luaState, CBattleGetEntityCondition);
	lua_setglobal(luaState, "battleGetEntityCondition");

	lua_pushcfunction(luaState, CBattleSetEntityCondition);
	lua_setglobal(luaState, "battleSetEntityCondition");

	lua_pushcfunction(luaState, CBattleGetEntityHP);
	lua_setglobal(luaState, "battleGetEntityHP");

	lua_pushcfunction(luaState, CBattleSetEntityHP);
	lua_setglobal(luaState, "battleSetEntityHP");

	lua_pushcfunction(luaState, CBattleSetX);
	lua_setglobal(luaState, "battleSetX");

	lua_pushcfunction(luaState, CBattleSetY);
	lua_setglobal(luaState, "battleSetY");

	lua_pushcfunction(luaState, CBattleResortEntity);
	lua_setglobal(luaState, "battleResortEntity");

	lua_pushcfunction(luaState, CBattleAddMessage);
	lua_setglobal(luaState, "battleAddMessage");

	lua_pushcfunction(luaState, CBattleKillCombatant);
	lua_setglobal(luaState, "battleKillCombatant");

	
	/* Miscellaneous */

	lua_pushcfunction(luaState, CFadeIn);
	lua_setglobal(luaState, "fadeIn");

	lua_pushcfunction(luaState, CFadeOut);
	lua_setglobal(luaState, "fadeOut");

	lua_pushcfunction(luaState, CTransitionIn);
	lua_setglobal(luaState, "transitionIn");

	lua_pushcfunction(luaState, CTransitionOut);
	lua_setglobal(luaState, "transitionOut");

	lua_pushcfunction(luaState, CDoDialogue);
	lua_setglobal(luaState, "doDialogue");

	lua_pushcfunction(luaState, CDoShakeDialogue);
	lua_setglobal(luaState, "doShakeDialogue");

	lua_pushcfunction(luaState, CIsSpeechDialogActive);
	lua_setglobal(luaState, "isSpeechDialogActive");

	lua_pushcfunction(luaState, CTint);
	lua_setglobal(luaState, "tint");

	lua_pushcfunction(luaState, CClearScreen);
	lua_setglobal(luaState, "clearScreen");

	lua_pushcfunction(luaState, CClearBuffer);
	lua_setglobal(luaState, "clearBuffer");

	lua_pushcfunction(luaState, CSetBufferTarget);
	lua_setglobal(luaState, "setBufferTarget");

	lua_pushcfunction(luaState, CGetRandomNumber);
	lua_setglobal(luaState, "getRandomNumber");

	lua_pushcfunction(luaState, CLoadPlayDestroy);
	lua_setglobal(luaState, "loadPlayDestroy");

	lua_pushcfunction(luaState, CPlayPreloadedSample);
	lua_setglobal(luaState, "playPreloadedSample");

	lua_pushcfunction(luaState, CAddPartyMember);
	lua_setglobal(luaState, "addPartyMember");

	lua_pushcfunction(luaState, CRemovePartyMember);
	lua_setglobal(luaState, "removePartyMember");

	lua_pushcfunction(luaState, CReviveAllPlayers);
	lua_setglobal(luaState, "reviveAllPlayers");

	lua_pushcfunction(luaState, CAddSparklySpiralAroundPlayer);
	lua_setglobal(luaState, "addSparklySpiralAroundPlayer");

	lua_pushcfunction(luaState, CAddSparklySpiralAroundObject);
	lua_setglobal(luaState, "addSparklySpiralAroundObject");

	lua_pushcfunction(luaState, CAddSmoke);
	lua_setglobal(luaState, "addSmoke");

	lua_pushcfunction(luaState, CDoShop);
	lua_setglobal(luaState, "doShop");

	lua_pushcfunction(luaState, CReverseTint);
	lua_setglobal(luaState, "reverse_tint");

	lua_pushcfunction(luaState, CSetMusicVolume);
	lua_setglobal(luaState, "set_music_volume");

	lua_pushcfunction(luaState, CSetAmbienceVolume);
	lua_setglobal(luaState, "set_ambience_volume");

	lua_pushcfunction(luaState, CFillEllipse);
	lua_setglobal(luaState, "fillEllipse");

	lua_pushcfunction(luaState, CSetTimer);
	lua_setglobal(luaState, "setTimer");

	lua_pushcfunction(luaState, CGetTimer);
	lua_setglobal(luaState, "getTimer");

	lua_pushcfunction(luaState, CPlayMusic);
	lua_setglobal(luaState, "playMusic");

	lua_pushcfunction(luaState, CMarkMan);
	lua_setglobal(luaState, "markMan");

	lua_pushcfunction(luaState, CAllPointsUp);
	lua_setglobal(luaState, "allPointsUp");

	lua_pushcfunction(luaState, CDoShooter);
	lua_setglobal(luaState, "doShooter");

	lua_pushcfunction(luaState, CDoArchery);
	lua_setglobal(luaState, "doArchery");

	lua_pushcfunction(luaState, CPrompt);
	lua_setglobal(luaState, "prompt");

	lua_pushcfunction(luaState, CTriplePrompt);
	lua_setglobal(luaState, "triple_prompt");

	lua_pushcfunction(luaState, CNotify);
	lua_setglobal(luaState, "notify");

	lua_pushcfunction(luaState, CRest);
	lua_setglobal(luaState, "rest");

	lua_pushcfunction(luaState, CFlip);
	lua_setglobal(luaState, "flip");

	lua_pushcfunction(luaState, CGiveGold);
	lua_setglobal(luaState, "giveGold");

	lua_pushcfunction(luaState, CGameOver);
	lua_setglobal(luaState, "gameOver");

	lua_pushcfunction(luaState, CGameOverNoFade);
	lua_setglobal(luaState, "gameOverNoFade");

	lua_pushcfunction(luaState, CDoVolcanoEndScene);
	lua_setglobal(luaState, "doVolcanoEndScene");

	lua_pushcfunction(luaState, CAddRocket);
	lua_setglobal(luaState, "addRocket");

	lua_pushcfunction(luaState, CStartRocket);
	lua_setglobal(luaState, "startRocket");

	lua_pushcfunction(luaState, CDoLander);
	lua_setglobal(luaState, "doLander");

	lua_pushcfunction(luaState, CLanderSuccess);
	lua_setglobal(luaState, "landerSuccess");

	lua_pushcfunction(luaState, Cinto_the_sun);
	lua_setglobal(luaState, "into_the_sun");

	lua_pushcfunction(luaState, Ccredits);
	lua_setglobal(luaState, "credits");

	lua_pushcfunction(luaState, CGetPlatform);
	lua_setglobal(luaState, "get_platform");

	lua_pushcfunction(luaState, CGetPlayerDest);
	lua_setglobal(luaState, "get_player_dest");

	lua_pushcfunction(luaState, CGetAreaPan);
	lua_setglobal(luaState, "getAreaPan");

	lua_pushcfunction(luaState, CSetAreaPan);
	lua_setglobal(luaState, "setAreaPan");

	lua_pushcfunction(luaState, CDoItemTutorial);
	lua_setglobal(luaState, "doItemTutorial");

	lua_pushcfunction(luaState, CStartsWithVowel);
	lua_setglobal(luaState, "startsWithVowel");

	lua_pushcfunction(luaState, CAstarStop);
	lua_setglobal(luaState, "astar_stop");

	lua_pushcfunction(luaState, CDoMapTutorial);
	lua_setglobal(luaState, "doMapTutorial");

	lua_pushcfunction(luaState, CGetAreaWidth);
	lua_setglobal(luaState, "getAreaWidth");

	lua_pushcfunction(luaState, CGetAreaHeight);
	lua_setglobal(luaState, "getAreaHeight");

	lua_pushcfunction(luaState, CSetSpecialWalkable);
	lua_setglobal(luaState, "setSpecialWalkable");

	lua_pushcfunction(luaState, CAnotherDoDialogue);
	lua_setglobal(luaState, "anotherDoDialogue");

	lua_pushcfunction(luaState, CEnyHasWeapon);
	lua_setglobal(luaState, "enyHasWeapon");

	lua_pushcfunction(luaState, CDoKeepLook);
	lua_setglobal(luaState, "doKeepLook");

	lua_pushcfunction(luaState, CDoKingKingAlbertLook);
	lua_setglobal(luaState, "doKingKingAlbertLook");

	lua_pushcfunction(luaState, CSetPlayerFormation);
	lua_setglobal(luaState, "setPlayerFormation");

	lua_pushcfunction(luaState, CShowBeachBattleInfo);
	lua_setglobal(luaState, "showBeachBattleInfo");

	lua_pushcfunction(luaState, CSetGlobalCanSave);
	lua_setglobal(luaState, "setGlobalCanSave");

	lua_pushcfunction(luaState, CDpadOff);
	lua_setglobal(luaState, "dpad_off");

	lua_pushcfunction(luaState, CDpadOn);
	lua_setglobal(luaState, "dpad_on");

	lua_pushcfunction(luaState, CGetUseDpad);
	lua_setglobal(luaState, "get_use_dpad");

	lua_pushcfunction(luaState, CGetBreakMainLoop);
	lua_setglobal(luaState, "getBreakMainLoop");

	lua_pushcfunction(luaState, CSetGonnaFadeInRed);
	lua_setglobal(luaState, "setGonnaFadeInRed");

	lua_pushcfunction(luaState, CSetAreaFocus);
	lua_setglobal(luaState, "setAreaFocus");

	lua_pushcfunction(luaState, CSetGlobalDrawControls);
	lua_setglobal(luaState, "setGlobalDrawControls");

	lua_pushcfunction(luaState, CSetGlobalDrawRed);
	lua_setglobal(luaState, "setGlobalDrawRed");

	lua_pushcfunction(luaState, CSetRedOffPressOn);
	lua_setglobal(luaState, "setRedOffPressOn");

	lua_pushcfunction(luaState, CGetDpadType);
	lua_setglobal(luaState, "getDpadType");
	
	lua_pushcfunction(luaState, C_t);
	lua_setglobal(luaState, "_t");

	lua_pushcfunction(luaState, C_extra_battle_chances);
	lua_setglobal(luaState, "extra_battle_chances");

	lua_pushcfunction(luaState, C_dbg);
	lua_setglobal(luaState, "dbg");

	lua_pushcfunction(luaState, C_BonusPoints);
	lua_setglobal(luaState, "bonusPoints");

	lua_pushcfunction(luaState, CLoadSample);
	lua_setglobal(luaState, "loadSample");

	lua_pushcfunction(luaState, CDestroySample);
	lua_setglobal(luaState, "destroySample");

	lua_pushcfunction(luaState, CPlaySample);
	lua_setglobal(luaState, "playSample");

	lua_pushcfunction(luaState, CPreloadSFX);
	lua_setglobal(luaState, "preloadSFX");

	lua_pushcfunction(luaState, CPreloadSpellSFX);
	lua_setglobal(luaState, "preloadSpellSFX");

	lua_pushcfunction(luaState, CDebug);
	lua_setglobal(luaState, "debug");
}

