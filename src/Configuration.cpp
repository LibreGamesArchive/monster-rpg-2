#include "monster2.hpp"

#include <sys/stat.h>

#ifdef ALLEGRO_WINDOWS
#define mkdir(a, b) _mkdir(a)
#endif

Configuration config;

static bool keynames_gotten = false;
std::map<std::string, int> keyname_to_keycode_map;

static int keyname_to_keycode(const char *name)
{
	if (!keynames_gotten) {
		keynames_gotten = true;
		std::string phony = ".";
		for (int i = 0; i < ALLEGRO_KEY_MAX; i++) {
			std::string s = al_keycode_to_name(i);
			if (keyname_to_keycode_map.find(s) != keyname_to_keycode_map.end()) {
				s = phony;
				phony += ".";
			}
			keyname_to_keycode_map[s] = i;
		}

	}

	if (keyname_to_keycode_map.find(name) != keyname_to_keycode_map.end()) {
		return keyname_to_keycode_map[name];
	}

	return 0;
}

#ifdef ALLEGRO_ANDROID
bool zeemote_should_autoconnect(void)
{
	return config.getAutoconnectToZeemote();
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

int Configuration::getKeySettings(void)
{
	return keySettings;
}

void Configuration::setKeySettings(int k)
{
	keySettings = k;
}

int Configuration::getKeyFullscreen(void)
{
	return keyFullscreen;
}

void Configuration::setKeyFullscreen(int k)
{
	keyFullscreen = k;
}

int Configuration::getKeySFXUp(void)
{
	return keySFXUp;
}

void Configuration::setKeySFXUp(int k)
{
	keySFXUp = k;
}

int Configuration::getKeySFXDown(void)
{
	return keySFXDown;
}

void Configuration::setKeySFXDown(int k)
{
	keySFXDown = k;
}

int Configuration::getKeyMusicUp(void)
{
	return keyMusicUp;
}

void Configuration::setKeyMusicUp(int k)
{
	keyMusicUp = k;
}

int Configuration::getKeyMusicDown(void)
{
	return keyMusicDown;
}

void Configuration::setKeyMusicDown(int k)
{
	keyMusicDown = k;
}

int Configuration::getKeyQuit()
{
	return keyQuit;
}

void Configuration::setKeyQuit(int k)
{
	keyQuit = k;
}

int Configuration::getKeySortItems()
{
	return keySortItems;
}

void Configuration::setKeySortItems(int k)
{
	keySortItems = k;
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

int Configuration::getLanguage(void)
{
	return language;
}

void Configuration::setLanguage(int l)
{
	language = l;
}

bool Configuration::getAutoconnectToZeemote(void)
{
	return autoconnect_to_zeemote;
}

void Configuration::setAutoconnectToZeemote(bool ac)
{
	autoconnect_to_zeemote = ac;
}

bool Configuration::read()
{
	char buf[1000];
	sprintf(buf, "%s", getUserResource(""));
	debug_message("0");
	
	mkdir(buf, 0755);
	debug_message("1");

	while (buf[strlen(buf)-1] == '\\' || buf[strlen(buf)-1] == '/')
		buf[strlen(buf)-1] = 0;
	
	mkdir(buf, 0755);
	
	debug_message("2");

	// load

	debug_message("cfgfn='%s'\n", getUserResource("config"));
	XMLData* xml = new XMLData(getUserResource("config"), 0);
	if (xml->getFailed()) {
	   debug_message("couldn't read config");
	   delete xml;
#ifdef ALLEGRO_RASPBERRYPI
	   last_session_was_x = getenv("DISPLAY") != NULL;
#endif
	   return false;
	}
	debug_message("3");

	XMLData* monster = xml->find("monster2");
	if (!monster) { delete xml; throw ReadError(); }

	XMLData *hacks = monster->find("hacks");

	XMLData* game = monster->find("game");
	if (!game) { delete xml; throw ReadError(); }
	XMLData* auto_zeemote = game->find("autoconnect_to_zeemote");
	if (auto_zeemote) {
		setAutoconnectToZeemote(atoi(auto_zeemote->getValue().c_str()));
	}
	else {
		setAutoconnectToZeemote(0);
	}
#ifdef ALLEGRO_RASPBERRYPI
	XMLData* last_was_x = game->find("last_session_was_x");
	if (last_was_x) {
		last_session_was_x = atoi(last_was_x->getValue().c_str());
	}
#endif
	XMLData* joyb1 = game->find("joyb1");
	XMLData* joyb2 = game->find("joyb2");
	XMLData* joyb3 = game->find("joyb3");

	XMLData *key1 = NULL;
	XMLData *key2 = NULL;
	XMLData *key3 = NULL;
	XMLData *keyLeft = NULL;
	XMLData *keyRight = NULL;
	XMLData *keyUp = NULL;
	XMLData *keyDown = NULL;
	XMLData *keySettings = NULL;
	XMLData *keyFullscreen = NULL;
	XMLData *keySFXUp = NULL;
	XMLData *keySFXDown = NULL;
	XMLData *keyMusicUp = NULL;
	XMLData *keyMusicDown = NULL;
	XMLData *keyQuit = NULL;
	XMLData *keySortItems = NULL;
	XMLData *fpsOn = NULL;

#ifdef ALLEGRO_RASPBERRYPI
	const char *disp = getenv("DISPLAY");
	if ((last_session_was_x && disp) || (!last_session_was_x && !disp)) {
#endif
		key1 = game->find("key1");
		key2 = game->find("key2");
		key3 = game->find("key3");
		keyLeft = game->find("keyLeft");
		keyRight = game->find("keyRight");
		keyUp = game->find("keyUp");
		keyDown = game->find("keyDown");
		keySettings = game->find("keySettings");
		keyFullscreen = game->find("keyFullscreen");
		keySFXUp = game->find("keySFXUp");
		keySFXDown = game->find("keySFXDown");
		keyMusicUp = game->find("keyMusicUp");
		keyMusicDown = game->find("keyMusicDown");
		keyQuit = game->find("keyQuit");
		keySortItems = game->find("keySortItems");
		fpsOn = game->find("fpsOn");
#ifdef ALLEGRO_RASPBERRYPI
	}
	last_session_was_x = getenv("DISPLAY") != NULL;
#endif

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
		if (xml_auto_rotate) {
			setAutoRotation(atoi(xml_auto_rotate->getValue().c_str()));
		}
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

	XMLData* sound = monster->find("sound");
	if (!sound) { delete xml; throw ReadError(); }
	XMLData* music_volume = sound->find("music_volume");
	if (!music_volume) { delete xml; throw ReadError(); }
	XMLData* sfx_volume = sound->find("sfx_volume");
	if (!sfx_volume) { delete xml; throw ReadError(); }

	if (joyb1) setJoyButton1(atoi(joyb1->getValue().c_str()));
	if (joyb2) setJoyButton2(atoi(joyb2->getValue().c_str()));
	if (joyb3) setJoyButton3(atoi(joyb3->getValue().c_str()));
#if !defined ALLEGRO_IPHONE
	if (key1) setKey1(keyname_to_keycode(key1->getValue().c_str()));
	if (key2) setKey2(keyname_to_keycode(key2->getValue().c_str()));
	if (key3) setKey3(keyname_to_keycode(key3->getValue().c_str()));
	if (keyLeft) setKeyLeft(keyname_to_keycode(keyLeft->getValue().c_str()));
	if (keyRight) setKeyRight(keyname_to_keycode(keyRight->getValue().c_str()));
	if (keyUp) setKeyUp(keyname_to_keycode(keyUp->getValue().c_str()));
	if (keyDown) setKeyDown(keyname_to_keycode(keyDown->getValue().c_str()));
	if (keySettings) setKeySettings(keyname_to_keycode(keySettings->getValue().c_str()));
	if (keyFullscreen) setKeyFullscreen(keyname_to_keycode(keyFullscreen->getValue().c_str()));
	if (keySFXUp) setKeySFXUp(keyname_to_keycode(keySFXUp->getValue().c_str()));
	if (keySFXDown) setKeySFXDown(keyname_to_keycode(keySFXDown->getValue().c_str()));
	if (keyMusicUp) setKeyMusicUp(keyname_to_keycode(keyMusicUp->getValue().c_str()));
	if (keyMusicDown) setKeyMusicDown(keyname_to_keycode(keyMusicDown->getValue().c_str()));
	if (keyQuit) setKeyQuit(keyname_to_keycode(keyQuit->getValue().c_str()));
	if (keySortItems) setKeySortItems(keyname_to_keycode(keySortItems->getValue().c_str()));
#endif
	if (fpsOn) fps_on = atoi(fpsOn->getValue().c_str());

	ScreenDescriptor sd;
	sd.width = atoi(width->getValue().c_str());
	sd.height = atoi(height->getValue().c_str());
	sd.fullscreen = atoi(fullscreen->getValue().c_str());
	setWantedGraphicsMode(sd);
	setWaitForVsync(atoi(vsync->getValue().c_str()));
	setSFXVolume(atoi(sfx_volume->getValue().c_str()));
	setMusicVolume(atoi(music_volume->getValue().c_str()));

	delete xml;

	loaded = true;

	return true;
}

void Configuration::write()
{
	XMLData* monster = new XMLData("monster2", "");

	XMLData* game = new XMLData("game", "");
	XMLData* auto_zeemote = new XMLData("autoconnect_to_zeemote", my_itoa(getAutoconnectToZeemote()));
#ifdef ALLEGRO_RASPBERRYPI
	XMLData* last_was_x = new XMLData("last_session_was_x", my_itoa(last_session_was_x));
#endif
	XMLData* joyb1 = new XMLData("joyb1", my_itoa(getJoyButton1()));
	XMLData* joyb2 = new XMLData("joyb2", my_itoa(getJoyButton2()));
	XMLData* joyb3 = new XMLData("joyb3", my_itoa(getJoyButton3()));
#if !defined ALLEGRO_IPHONE
	XMLData* key1 = new XMLData(
		"key1", al_keycode_to_name(getKey1()));
	XMLData* key2 = new XMLData(
		"key2", al_keycode_to_name(getKey2()));
	XMLData* key3 = new XMLData(
		"key3", al_keycode_to_name(getKey3()));
	XMLData* keyLeft = new XMLData(
		"keyLeft", al_keycode_to_name(getKeyLeft()));
	XMLData* keyRight = new XMLData(
		"keyRight", al_keycode_to_name(getKeyRight()));
	XMLData* keyUp = new XMLData(
		"keyUp", al_keycode_to_name(getKeyUp()));
	XMLData* keyDown = new XMLData(
		"keyDown", al_keycode_to_name(getKeyDown()));
	XMLData* keySettings = new XMLData(
		"keySettings", al_keycode_to_name(getKeySettings()));
	XMLData* keyFullscreen = new XMLData(
		"keyFullscreen", al_keycode_to_name(getKeyFullscreen()));
	XMLData* keySFXUp = new XMLData(
		"keySFXUp", al_keycode_to_name(getKeySFXUp()));
	XMLData* keySFXDown = new XMLData(
		"keySFXDown", al_keycode_to_name(getKeySFXDown()));
	XMLData* keyMusicUp = new XMLData(
		"keyMusicUp", al_keycode_to_name(getKeyMusicUp()));
	XMLData* keyMusicDown = new XMLData(
		"keyMusicDown", al_keycode_to_name(getKeyMusicDown()));
	XMLData* keyQuit = new XMLData(
		"keyQuit", al_keycode_to_name(getKeyQuit()));
	XMLData* keySortItems = new XMLData(
		"keySortItems", al_keycode_to_name(getKeySortItems()));
#endif
	XMLData* fpsOn = new XMLData(
		"fpsOn", my_itoa(fps_on));

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

	game->add(auto_zeemote);
#ifdef ALLEGRO_RASPBERRYPI
	game->add(last_was_x);
#endif

	game->add(joyb1);
	game->add(joyb2);
	game->add(joyb3);
#if !defined ALLEGRO_IPHONE
	game->add(key1);
	game->add(key2);
	game->add(key3);
	game->add(keyLeft);
	game->add(keyRight);
	game->add(keyUp);
	game->add(keyDown);
	game->add(keySettings);
	game->add(keyFullscreen);
	game->add(keySFXUp);
	game->add(keySFXDown);
	game->add(keyMusicUp);
	game->add(keyMusicDown);
	game->add(keyQuit);
	game->add(keySortItems);
#endif
	game->add(fpsOn);

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
	XMLData *width = new XMLData("width", my_itoa(config_save_width));
	XMLData *height = new XMLData("height", my_itoa(config_save_height));
	XMLData* fullscreen = new XMLData("fullscreen", my_itoa(sd->fullscreen));
	XMLData* vsync = new XMLData("vsync", my_itoa(getWaitForVsync()));
	XMLData *xml_filter_type = new XMLData("filter_type", my_itoa(getFilterType()));
	XMLData *xml_aspect = new XMLData("maintain_aspect_ratio", my_itoa(getMaintainAspectRatio()));
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

	key1(ALLEGRO_KEY_SPACE),
	key2(ALLEGRO_KEY_ESCAPE),
	key3(ALLEGRO_KEY_V),
	keyLeft(ALLEGRO_KEY_LEFT),
	keyRight(ALLEGRO_KEY_RIGHT),
	keyUp(ALLEGRO_KEY_UP),
	keyDown(ALLEGRO_KEY_DOWN),
	keySettings(ALLEGRO_KEY_S),
	keyFullscreen(ALLEGRO_KEY_F),
	keySFXUp(ALLEGRO_KEY_F4),
	keySFXDown(ALLEGRO_KEY_F3),
	keyMusicUp(ALLEGRO_KEY_F2),
	keyMusicDown(ALLEGRO_KEY_F1),
	keyQuit(ALLEGRO_KEY_Q),
	keySortItems(ALLEGRO_KEY_F8),

	gamepadAvailable(true),
	onlyMemoryBitmaps(false),
	lowCpuUsage(false),
	stick(0),
	adapter(0),
	axis(0),
	xbox360(false)
	,cfg_tuning(CFG_TUNING_BALANCED)
	,cfg_difficulty(CFG_DIFFICULTY_NORMAL)
	,cfg_filter_type(FILTER_NONE)
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
#ifdef ALLEGRO_ANDROID
	,autoconnect_to_zeemote(false)
#endif
#ifdef ALLEGRO_RASPBERRYPI
	,last_session_was_x(-1)
#endif
{
#ifdef EDITOR
	wantedMode.width = 800;
	wantedMode.height = 500;
#endif
	wantedMode.fullscreen = false;
}
