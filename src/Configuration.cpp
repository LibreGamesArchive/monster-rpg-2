#include "monster2.hpp"

#include <sys/stat.h>

#ifdef ALLEGRO_WINDOWS
#define mkdir(a, b) mkdir(a)
#endif

Configuration config;

#if UNUSED_NOW
static int get_joy_button(void)
{
	if (!config.getGamepadAvailable()) {
		notify("", "No gamepad attached", "");
		return -1;
	}

	m_rest(1);

	m_set_target_bitmap(buffer);

	mDrawFrame(BW/2-100, BH/2-50, 200, 100);
	mTextout(game_font, _t("Press a button"), BW/2, BH/2-30, white, black,
		WGT_TEXT_DROP_SHADOW, true);
	mTextout(game_font, _t("on your Gamepad"), BW/2, BH/2-10, white, black,
		WGT_TEXT_DROP_SHADOW, true);
	mTextout(game_font, _t("Press Escape to cancel"), BW/2, BH/2+30, white, black,
		WGT_TEXT_DROP_SHADOW, true);

	drawBufferToScreen();
	m_flip_display();

	while  (1) {
#ifdef ALLEGRO4
		if (key[KEY_ESC])
			return -1;
		poll_joystick();
		for (int i = 0; i < joy[0].num_buttons; i++) {
			if (joy[0].button[i].b)
				return i;
		}
#else
		ALLEGRO_KEYBOARD_STATE kbstate;
		al_get_keyboard_state(&kbstate);
		if (al_key_down(&kbstate, ALLEGRO_KEY_ESCAPE))
			return -1;
		ALLEGRO_JOYSTICK *joystick = al_get_joystick(0);
		ALLEGRO_JOYSTICK_STATE joystate;
		al_get_joystick_state(joystick, &joystate);
		for (int i = 0; i < al_get_joystick_num_buttons(joystick); i++) {
			if (joystate.button[i])
				return i;
		}
#endif
		m_rest(0.001);
	}
}

#endif

bool Configuration::getLowCpuUsage(void)
{
	return lowCpuUsage;
}


void Configuration::setLowCpuUsage(bool low)
{
	lowCpuUsage = low;
}


ScreenDescriptor* Configuration::getWantedGraphicsMode()
{
	return &wantedMode;
}

void Configuration::setWantedGraphicsMode(ScreenDescriptor& sd)
{
	wantedMode.width = sd.width;
	wantedMode.height = sd.height;
	wantedMode.fullscreen = sd.fullscreen;
}

bool Configuration::showDebugMessages()
{
	return debugMessages;
}

void Configuration::setShowDebugMessages(bool show)
{
	debugMessages = show;
}

bool Configuration::useCompiledScripts()
{
	return compiledScripts;
}

bool Configuration::getWaitForVsync()
{
	return waitForVsync;
}

void Configuration::setWaitForVsync(bool vs)
{
	waitForVsync = vs;
}

bool Configuration::getGamepadAvailable()
{
	return gamepadAvailable;
}

void Configuration::setGamepadAvailable(bool available)
{
	gamepadAvailable = available;
}

bool Configuration::useSmoothScaling()
{
	return smoothScaling;
}

void Configuration::setUseSmoothScaling(bool smooth)
{
	smoothScaling = smooth;
}

int Configuration::getSFXVolume()
{
	return sfxVolume;
}

void Configuration::setSFXVolume(int volume)
{
	sfxVolume = volume;
}

int Configuration::getMusicVolume()
{
	return musicVolume;
}

void Configuration::setMusicVolume(int volume)
{
	int _mv = musicVolume;
	musicVolume = volume;
	if (sound_inited) {
		if (volume == 0) {
			playMusic("");
			playAmbience("");
		}
		else if (_mv == 0 && volume != 0) {
			unmuteMusic();
			unmuteAmbience();
		}
	}
}

int Configuration::getJoyButton1()
{
	return joyB1;
}

void Configuration::setJoyButton1(int b1)
{
	joyB1 = b1;
}

int Configuration::getJoyButton2()
{
	return joyB2;
}

void Configuration::setJoyButton2(int b2)
{
	joyB2 = b2;
}

int Configuration::getJoyButton3()
{
	return joyB3;
}

void Configuration::setJoyButton3(int b3)
{
	joyB3 = b3;
}

int Configuration::getKey1(void)
{
	return key1;
}

void Configuration::setKey1(int k)
{
	key1 = k;
}

int Configuration::getKey2(void)
{
	return key2;
}

void Configuration::setKey2(int k)
{
	key2 = k;
}

int Configuration::getKey3(void)
{
	return key3;
}

void Configuration::setKey3(int k)
{
	key3 = k;
}

int Configuration::getKeyLeft(void)
{
	return keyLeft;
}

void Configuration::setKeyLeft(int k)
{
	keyLeft = k;
}

int Configuration::getKeyRight(void)
{
	return keyRight;
}

void Configuration::setKeyRight(int k)
{
	keyRight = k;
}

int Configuration::getKeyUp(void)
{
	return keyUp;
}

void Configuration::setKeyUp(int k)
{
	keyUp = k;
}

int Configuration::getKeyDown(void)
{
	return keyDown;
}

void Configuration::setKeyDown(int k)
{
	keyDown = k;
}

int Configuration::getTargetFPS(void)
{
	switch (getTuning()) {
		case (CFG_TUNING_BATTERY):
			return 20;
		case (CFG_TUNING_PERFORMANCE):
			return 60;
	}
	return 30;
}

bool Configuration::getUseOnlyMemoryBitmaps(void)
{
	return onlyMemoryBitmaps;
}


void Configuration::setUseOnlyMemoryBitmaps(bool onlyMem)
{
	onlyMemoryBitmaps = onlyMem;
}


int Configuration::getStick(void)
{
	return stick;
}

void Configuration::setStick(int s)
{
	stick = s;
}


int Configuration::getAxis(void)
{
	return axis;
}

void Configuration::setAxis(int s)
{
	axis = s;
}


bool Configuration::getXbox360(void)
{
	return xbox360;
}

void Configuration::setXbox360(bool x)
{
	xbox360 = x;
}


int Configuration::getAdapter(void)
{
	return adapter;
}

void Configuration::setAdapter(int a)
{
	adapter = a;
}

#ifdef WIZ
int Configuration::getGFXDriver(void)
{
	return gfxDriver;
}

void Configuration::setGFXDriver(int gd)
{
	gfxDriver = gd;
}
#endif

int Configuration::getLanguage(void)
{
	return language;
}

void Configuration::setLanguage(int l)
{
	language = l;
}

void Configuration::read()
{
	char buf[1000];
	sprintf(buf, "%s", getUserResource(""));
	
	mkdir(buf, 0755);

	while (buf[strlen(buf)-1] == '\\' || buf[strlen(buf)-1] == '/')
		buf[strlen(buf)-1] = 0;
	
	mkdir(buf, 0755);

	// load

	printf("buf='%s'\n", buf);
	
	XMLData* xml = new XMLData(getUserResource("config"));

	debug_message("cfgfn='%s'\n", getUserResource("config"));

	XMLData* monster = xml->find("monster2");
	if (!monster) { delete xml; throw ReadError(); }

	XMLData *hacks = monster->find("hacks");

	XMLData* game = monster->find("game");
	if (!game) { delete xml; throw ReadError(); }
	XMLData* joyb1 = game->find("joyb1");
	if (!joyb1) { delete xml; throw ReadError(); }
	XMLData* joyb2 = game->find("joyb2");
	if (!joyb2) { delete xml; throw ReadError(); }
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
	XMLData *xml_dpad_type = game->find("dpad_type");
	if (xml_dpad_type) {
		setDpadType(atoi(xml_dpad_type->getValue().c_str()));
	}
	XMLData *xml_tellusertousedpad = game->find("tell_user_to_use_dpad");
	if (xml_tellusertousedpad) {
		setTellUserToUseDpad(atoi(xml_tellusertousedpad->getValue().c_str()));
	}
	XMLData *xml_swap_buttons = game->find("swap_buttons");
	if (xml_swap_buttons) {
		setSwapButtons(atoi(xml_swap_buttons->getValue().c_str()));
	}
#endif
	XMLData *xml_difficulty = game->find("difficulty");
	if (xml_difficulty) {
		setDifficulty(atoi(xml_difficulty->getValue().c_str()));
	}
	XMLData *xml_tuning = game->find("tuning");
	if (xml_tuning) {
		setTuning(atoi(xml_tuning->getValue().c_str()));
	}
#if defined ALLEGRO_IPHONE
	XMLData *xml_shake_action = game->find("shake_action");
	if (xml_shake_action) {
		setShakeAction(atoi(xml_shake_action->getValue().c_str()));
	}
	XMLData *xml_flip_screen = game->find("flip_screen");
	if (xml_flip_screen) {
		XMLData *xml_auto_rotate = game->find("auto_rotate");
		bool flip = (bool)atoi(xml_flip_screen->getValue().c_str());
		bool auto_rot = (bool)atoi(xml_auto_rotate->getValue().c_str());
		if (flip) {
			setAutoRotation(1);
		}
		else if (auto_rot) {
			setAutoRotation(2);
		}
		else {
			setAutoRotation(0);
		}
	}
	else {
		XMLData *xml_auto_rotate = game->find("auto_rotate");
		setAutoRotation(atoi(xml_auto_rotate->getValue().c_str()));
	}
#endif
	XMLData *xml_language = game->find("language");
	if (xml_language) {
		bool langfixed = false;
		if (hacks) {
			XMLData *langfix = hacks->find("rm_french_canada");
			langfixed = atoi(langfix->getValue().c_str());
		}
		// 1.10 was the first version to have "version" in the config
		// file. if version is NULL, French-Europe translation and up
		// should be decreased by 1 (French-Canada removed)
		int lang = atoi(xml_language->getValue().c_str());
		if (!langfixed && lang > 2) {
			setLanguage(lang-1);
		}
		else
			setLanguage(lang);
	}

	XMLData* gfx = monster->find("gfx");
	if (!gfx) { delete xml; throw ReadError(); }
	XMLData* width = gfx->find("width");
	if (!width) { delete xml; throw ReadError(); }
	XMLData* height = gfx->find("height");
	if (!height) { delete xml; throw ReadError(); }
	XMLData* fullscreen = gfx->find("fullscreen");
	if (!fullscreen) { delete xml; throw ReadError(); }
	XMLData* vsync = gfx->find("vsync");
	if (!vsync) { delete xml; throw ReadError(); }
	XMLData *xml_filter_type = gfx->find("filter_type");
	if (xml_filter_type) {
		int ft = atoi(xml_filter_type->getValue().c_str());
		setFilterType(ft >= NUM_FILTER_TYPES ? 0 : ft);
	}
	XMLData *xml_aspect = gfx->find("maintain_aspect_ratio");
	if (xml_aspect) {
		setMaintainAspectRatio(atoi(xml_aspect->getValue().c_str()));
	}
#ifdef WIZ
	XMLData *gfxdrv = gfx->find("gfxDriver");
#endif

	XMLData* sound = monster->find("sound");
	if (!sound) { delete xml; throw ReadError(); }
	XMLData* music_volume = sound->find("music_volume");
	if (!music_volume) { delete xml; throw ReadError(); }
	XMLData* sfx_volume = sound->find("sfx_volume");
	if (!sfx_volume) { delete xml; throw ReadError(); }

	setJoyButton1(atoi(joyb1->getValue().c_str()));
	setJoyButton2(atoi(joyb2->getValue().c_str()));
	ScreenDescriptor sd;
	sd.width = atoi(width->getValue().c_str());
	sd.height = atoi(height->getValue().c_str());
	sd.fullscreen = atoi(fullscreen->getValue().c_str());
	setWantedGraphicsMode(sd);
	setWaitForVsync(atoi(vsync->getValue().c_str()));
#ifdef WIZ
	setGFXDriver(atoi(gfxdrv->getValue().c_str()));
#endif
	setSFXVolume(atoi(sfx_volume->getValue().c_str()));
	setMusicVolume(atoi(music_volume->getValue().c_str()));

	delete xml;

	loaded = true;
}

void Configuration::write()
{
	XMLData* monster = new XMLData("monster2", "");

	XMLData* game = new XMLData("game", "");
	XMLData* joyb1 = new XMLData("joyb1", my_itoa(getJoyButton1()));
	XMLData* joyb2 = new XMLData("joyb2", my_itoa(getJoyButton2()));
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
	XMLData* xml_dpad_type = new XMLData("dpad_type", old_control_mode < 0 ? my_itoa(getDpadType()) : my_itoa(old_control_mode));
	XMLData* xml_tellusertousedpad = new XMLData("tell_user_to_use_dpad", my_itoa(getTellUserToUseDpad()));
	XMLData *xml_swap_buttons = new XMLData("swap_buttons", my_itoa(getSwapButtons()));
#endif
	XMLData *xml_difficulty = new XMLData("difficulty", my_itoa(getDifficulty()));
	XMLData *xml_tuning = new XMLData("tuning", my_itoa(getTuning()));
#ifdef ALLEGRO_IPHONE
	XMLData *xml_shake_action = new XMLData("shake_action", my_itoa(getShakeAction()));
	XMLData *xml_auto_rotate = new XMLData("auto_rotate", my_itoa(getAutoRotation()));
#endif
	XMLData* xml_lang = new XMLData("language", my_itoa(getLanguage()));

	game->add(joyb1);
	game->add(joyb2);
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
	game->add(xml_dpad_type);
	game->add(xml_tellusertousedpad);
	game->add(xml_swap_buttons);
#endif
	game->add(xml_difficulty);
	game->add(xml_tuning);
#ifdef IPHONE
	game->add(xml_shake_action);
	game->add(xml_auto_rotate);
#endif
	game->add(xml_lang);

	XMLData* gfx = new XMLData("gfx", "");
	ScreenDescriptor* sd = getWantedGraphicsMode();
	XMLData* width = new XMLData("width", my_itoa(sd->width));
	XMLData* height = new XMLData("height", my_itoa(sd->height));
	XMLData* fullscreen = new XMLData("fullscreen", my_itoa(sd->fullscreen));
	XMLData* vsync = new XMLData("vsync", my_itoa(getWaitForVsync()));
	XMLData *xml_filter_type = new XMLData("filter_type", my_itoa(getFilterType()));
	XMLData *xml_aspect = new XMLData("maintain_aspect_ratio", my_itoa(getMaintainAspectRatio()));
#ifdef WIZ
	XMLData *gfxdrv = new XMLData("gfxDriver", my_itoa(getGFXDriver()));
	gfx->add(gfxdrv);
#endif
	gfx->add(width);
	gfx->add(height);
	gfx->add(fullscreen);
	gfx->add(vsync);
	gfx->add(xml_filter_type);
	gfx->add(xml_aspect);

	XMLData* sound = new XMLData("sound", "");
	XMLData* music_volume = new XMLData("music_volume", my_itoa(getMusicVolume()));
	XMLData* sfx_volume = new XMLData("sfx_volume", my_itoa(getSFXVolume()));
	sound->add(sfx_volume);
	sound->add(music_volume);
	
	XMLData *hacks = new XMLData("hacks", "");
	XMLData *rm_french_canada = new XMLData("rm_french_canada", my_itoa(1));
	hacks->add(rm_french_canada);

	monster->add(game);
	monster->add(gfx);
	monster->add(sound);
	monster->add(hacks);

	std::ofstream fout(getUserResource("config"), std::ios::out);
	monster->write(fout, 0);
	fout.close();

	delete monster;
}

Configuration::Configuration() :
#if defined MEMDBG || defined DEBUG
	debugMessages(true), 
#else
	debugMessages(false), 
#endif
	compiledScripts(false),
	waitForVsync(true),
	smoothScaling(false), 
	sfxVolume(255),
	musicVolume(255),
	joyB1(0),
	joyB2(1),
	joyB3(2),
#ifdef ALLEGRO4
	key1(KEY_SPACE),
	key2(KEY_ESC),
	keyLeft(KEY_LEFT),
	keyRight(KEY_RIGHT),
	keyUp(KEY_UP),
	keyDown(KEY_DOWN),
#else
	key1(ALLEGRO_KEY_SPACE),
	key2(ALLEGRO_KEY_ESCAPE),
	key3(ALLEGRO_KEY_V),
	keyLeft(ALLEGRO_KEY_LEFT),
	keyRight(ALLEGRO_KEY_RIGHT),
	keyUp(ALLEGRO_KEY_UP),
	keyDown(ALLEGRO_KEY_DOWN),
	/*
	keyLeft(ALLEGRO_KEY_H),
	keyRight(ALLEGRO_KEY_L),
	keyUp(ALLEGRO_KEY_K),
	keyDown(ALLEGRO_KEY_J),
	*/
#endif
	gamepadAvailable(true),
#ifndef WIZ
	onlyMemoryBitmaps(false),
#else
	onlyMemoryBitmaps(true),
#endif
	lowCpuUsage(false),
	stick(0),
	adapter(0),
	axis(0),
	xbox360(false)
	,cfg_tuning(CFG_TUNING_BALANCED)
	,cfg_difficulty(CFG_DIFFICULTY_NORMAL)
#ifdef ALLEGRO_ANDROID
	,cfg_filter_type(FILTER_LINEAR)
#else
	,cfg_filter_type(FILTER_NONE)
#endif
	,cfg_maintain_aspect_ratio(0)
	,language(0)
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
	,cfg_dpad_type(0)
	,cfg_tellusertousedpad(true)
	,cfg_swap_buttons(false)
#endif
#ifdef ALLEGRO_IPHONE
	,cfg_shake_action(CFG_SHAKE_CANCEL)
	,cfg_auto_rotation(2)
#endif
{
#ifdef EDITOR
	wantedMode.width = 800;
	wantedMode.height = 500;
#endif
	wantedMode.fullscreen = false;
}
