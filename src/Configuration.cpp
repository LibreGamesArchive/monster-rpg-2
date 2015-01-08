#include "monster2.hpp"

#include <sys/stat.h>

#ifdef ALLEGRO_ANDROID
#include "java.h"
#endif

#ifdef ALLEGRO_WINDOWS
#define mkdir(a, b) _mkdir(a)
#endif

Configuration config;

std::map<std::string, int> keyname_to_keycode_map;

const char *keynames[] =
{
	"NULL",
	"A",
	"B",
	"C",
	"D",
	"E",
	"F",
	"G",
	"H",
	"I",
	"J",
	"K",
	"L",
	"M",
	"N",
	"O",
	"P",
	"Q",
	"R",
	"S",
	"T",
	"U",
	"V",
	"W",
	"X",
	"Y",
	"Z",
	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"PAD_0",
	"PAD_1",
	"PAD_2",
	"PAD_3",
	"PAD_4",
	"PAD_5",
	"PAD_6",
	"PAD_7",
	"PAD_8",
	"PAD_9",
	"F1",
	"F2",
	"F3",
	"F4",
	"F5",
	"F6",
	"F7",
	"F8",
	"F9",
	"F10",
	"F11",
	"F12",
	"ESCAPE",
	"TILDE",
	"MINUS",
	"EQUALS",
	"BACKSPACE",
	"TAB",
	"OPENBRACE",
	"CLOSEBRACE",
	"ENTER",
	"SEMICOLON",
	"QUOTE",
	"BACKSLASH",
	"BACKSLASH2",
	"COMMA",
	"FULLSTOP",
	"SLASH",
	"SPACE",
	"INSERT",
	"DELETE",
	"HOME",
	"END",
	"PGUP",
	"PGDN",
	"LEFT",
	"RIGHT",
	"UP",
	"DOWN",
	"PAD_SLASH",
	"PAD_ASTERISK",
	"PAD_MINUS",
	"PAD_PLUS",
	"PAD_DELETE",
	"PAD_ENTER",
	"PRINTSCREEN",
	"PAUSE",
	"ABNT_C1",
	"YEN",
	"KANA",
	"CONVERT",
	"NOCONVERT",
	"AT",
	"CIRCUMFLEX",
	"COLON2",
	"KANJI",
	"PAD_EQUALS",
	"BACKQUOTE",
	"SEMICOLON2",
	"COMMAND",
	"BACK",
	"VOLUME_UP",
	"VOLUME_DOWN",
	"SEARCH",
	"DPAD_CENTER",
	"BUTTON_X",
	"BUTTON_Y",
	"DPAD_UP",
	"DPAD_DOWN",
	"DPAD_LEFT",
	"DPAD_RIGHT",
	"SELECT",
	"START",
	"L1",
	"R1",
	"122",
	"123",
	"124",
	"125",
	"126",
	"127",
	"128",
	"129",
	"130",
	"131",
	"132",
	"133",
	"134",
	"135",
	"136",
	"137",
	"138",
	"139",
	"140",
	"141",
	"142",
	"143",
	"144",
	"145",
	"146",
	"147",
	"148",
	"149",
	"150",
	"151",
	"152",
	"153",
	"154",
	"155",
	"156",
	"157",
	"158",
	"159",
	"160",
	"161",
	"162",
	"163",
	"164",
	"165",
	"166",
	"167",
	"168",
	"169",
	"170",
	"171",
	"172",
	"173",
	"174",
	"175",
	"176",
	"177",
	"178",
	"179",
	"180",
	"181",
	"182",
	"183",
	"184",
	"185",
	"186",
	"187",
	"188",
	"189",
	"190",
	"191",
	"192",
	"193",
	"194",
	"195",
	"196",
	"197",
	"198",
	"199",
	"200",
	"201",
	"202",
	"203",
	"204",
	"205",
	"206",
	"207",
	"208",
	"209",
	"210",
	"211",
	"212",
	"213",
	"214",
	"LSHIFT",
	"RSHIFT",
	"LCTRL",
	"RCTRL",
	"ALT",
	"ALTGR",
	"LWIN",
	"RWIN",
	"MENU",
	"SCROLLLOCK",
	"NUMLOCK",
	"CAPSLOCK",
	"227",
	"228",
	"229",
	"230",
	"231",
	"232",
	"233",
	"234",
	"235",
	"236",
	"237",
	"238",
	"239",
	"240",
	"241",
	"242",
	"243",
	"244",
	"245",
	"246",
	"247",
	"248",
	"249",
	"250",
	"251",
	"252",
	"253",
	"254",
	"255"
};

static int keyname_to_keycode(const char *name)
{
	for (int i = 0; i < ALLEGRO_KEY_MAX; i++) {
		if (!strcasecmp(name, keynames[i])) {
			return i;
		}
	}

	return 0;
}

const char *keycode_to_keyname(int i)
{
	if (i < 0 || i >= ALLEGRO_KEY_MAX) {
		return 0;
	}

	return keynames[i];
}

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

int Configuration::getJoyButtonMusicUp()
{
	return joyMusicUp;
}

void Configuration::setJoyButtonMusicUp(int b)
{
	joyMusicUp = b;
}

int Configuration::getJoyButtonMusicDown()
{
	return joyMusicDown;
}

void Configuration::setJoyButtonMusicDown(int b)
{
	joyMusicDown = b;
}

int Configuration::getJoyButtonSFXUp()
{
	return joySFXUp;
}

void Configuration::setJoyButtonSFXUp(int b)
{
	joySFXUp = b;
}

int Configuration::getJoyButtonSFXDown()
{
	return joySFXDown;
}

void Configuration::setJoyButtonSFXDown(int b)
{
	joySFXDown = b;
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

void Configuration::setDefaultInputs()
{
#ifdef ALLEGRO_ANDROID
	joyB1 = 0;
	joyB2 = 1;
	joyB3 = 2;
	joyMusicUp = -1;
	joyMusicDown = -1;
	joySFXUp = -1;
	joySFXDown = -1;
#else
	joyB1 = 0;
	joyB2 = 1;
	joyB3 = 2;
	joyMusicUp = 4;
	joyMusicDown = 5;
	joySFXUp = 6;
	joySFXDown = 7;
#endif

#ifndef ALLEGRO_IPHONE
	key1 = ALLEGRO_KEY_SPACE;
	key2 = ALLEGRO_KEY_ESCAPE;
	key3 = ALLEGRO_KEY_V;
	keyLeft = ALLEGRO_KEY_LEFT;
	keyRight = ALLEGRO_KEY_RIGHT;
	keyUp = ALLEGRO_KEY_UP;
	keyDown = ALLEGRO_KEY_DOWN;
	keySettings = ALLEGRO_KEY_S;
	keyFullscreen = ALLEGRO_KEY_F;
	keySFXUp = ALLEGRO_KEY_F4;
	keySFXDown = ALLEGRO_KEY_F3;
	keyMusicUp = ALLEGRO_KEY_F2;
	keyMusicDown = ALLEGRO_KEY_F1;
	keyQuit = ALLEGRO_KEY_Q;
	keySortItems = ALLEGRO_KEY_F8;
#else
	key1 = ALLEGRO_KEY_K;
	key2 = ALLEGRO_KEY_L;
	key3 = ALLEGRO_KEY_O;
	keyLeft = ALLEGRO_KEY_A;
	keyRight = ALLEGRO_KEY_D;
	keyUp = ALLEGRO_KEY_W;
	keyDown = ALLEGRO_KEY_X;
	keySettings = ALLEGRO_KEY_Y;
	keyFullscreen = 0;
	keySFXUp = 0;
	keySFXDown = 0;
	keyMusicUp = ALLEGRO_KEY_J;
	keyMusicDown = ALLEGRO_KEY_H;
	keyQuit = ALLEGRO_KEY_U;
	keySortItems = ALLEGRO_KEY_I;
#endif
}

bool Configuration::read()
{
#ifdef OUYA
#ifdef NO_DRM
	bool unlocked = true;
#else
	bool unlocked = false;
#endif

	if (isAndroidConsole()) {
		purchased = unlocked;
	}
	else {
		purchased = 1;
	}
#else
	purchased = 1;
#endif

	always_center = PAN_HYBRID;

	setDefaultInputs();

	if (isAndroidConsole()) {
		cfg_maintain_aspect_ratio = ASPECT_INTEGER;
	}
	else {
		cfg_maintain_aspect_ratio = ASPECT_MAINTAIN_RATIO;
	}

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
#if defined OUYA
		int purchased = checkPurchased();
		if (purchased == 1) {
			config.setPurchased(1);
		}
#endif
		return false;
	}
	debug_message("3");

	XMLData* monster = xml->find("monster2");
	if (!monster) { delete xml; throw ReadError(); }

	XMLData *hacks = monster->find("hacks");

	if (hacks) {
		XMLData *ignorecfg = hacks->find("ignorecfg");
		if (!ignorecfg) {
			delete xml;

			loaded = true;

			return true;
		}
	}

	XMLData* game = monster->find("game");
	if (!game) { delete xml; throw ReadError(); }
	XMLData *joyb1 = game->find("joyb1");
	XMLData *joyb2 = game->find("joyb2");
	XMLData *joyb3 = game->find("joyb3");
	XMLData *joymusicup = game->find("joymusicup");
	XMLData *joymusicdown = game->find("joymusicdown");
	XMLData *joysfxup = game->find("joysfxup");
	XMLData *joysfxdown = game->find("joysfxdown");
	XMLData *purchXML = game->find("p");

	bool keysfixed = isAndroidConsole() ? false : true;
	if (isAndroidConsole() && hacks) {
		XMLData *keyfix = hacks->find("ouya_keyfix");
		if (keyfix) {
			keysfixed = atoi(keyfix->getValue().c_str());
		}
	}

	XMLData *key1 = game->find("key1");
	XMLData *key2 = game->find("key2");
	XMLData *key3 = game->find("key3");
	XMLData *keyLeft = game->find("keyLeft");
	XMLData *keyRight = game->find("keyRight");
	XMLData *keyUp = game->find("keyUp");
	XMLData *keyDown = game->find("keyDown");
	XMLData *keySettings = game->find("keySettings");
	XMLData *keyFullscreen = game->find("keyFullscreen");
	XMLData *keySFXUp = game->find("keySFXUp");
	XMLData *keySFXDown = game->find("keySFXDown");
	XMLData *keyMusicUp = game->find("keyMusicUp");
	XMLData *keyMusicDown = game->find("keyMusicDown");
	XMLData *keyQuit = game->find("keyQuit");
	XMLData *keySortItems = game->find("keySortItems");
	XMLData *fpsOn = game->find("fpsOn");
	XMLData *alwaysCenter = game->find("alwaysCenter");

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
	/*
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
	*/
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
	XMLData *xml_aspect = gfx->find("maintain_aspect_ratio");
	if (xml_aspect) {
		setMaintainAspectRatio(atoi(xml_aspect->getValue().c_str()));
	}
	XMLData *xml_depth = gfx->find("depth_bits");
	if (xml_depth) {
		setDepthBits(atoi(xml_depth->getValue().c_str()));
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
	if (joymusicup) setJoyButtonMusicUp(atoi(joymusicup->getValue().c_str()));
	if (joymusicdown) setJoyButtonMusicDown(atoi(joymusicdown->getValue().c_str()));
	if (joysfxup) setJoyButtonSFXUp(atoi(joysfxup->getValue().c_str()));
	if (joysfxdown) setJoyButtonSFXDown(atoi(joysfxdown->getValue().c_str()));
	if (purchXML) setPurchased(atoi(purchXML->getValue().c_str()));
	
	if (keysfixed) {
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
	}

	if (fpsOn) fps_on = atoi(fpsOn->getValue().c_str());
	if (alwaysCenter) always_center = atoi(alwaysCenter->getValue().c_str());

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
	XMLData* joyb1 = new XMLData("joyb1", my_itoa(getJoyButton1()));
	XMLData* joyb2 = new XMLData("joyb2", my_itoa(getJoyButton2()));
	XMLData* joyb3 = new XMLData("joyb3", my_itoa(getJoyButton3()));
	XMLData* joymusicup = new XMLData("joymusicup", my_itoa(getJoyButtonMusicUp()));
	XMLData* joymusicdown = new XMLData("joymusicdown", my_itoa(getJoyButtonMusicDown()));
	XMLData* joysfxup = new XMLData("joysfxup", my_itoa(getJoyButtonSFXUp()));
	XMLData* joysfxdown = new XMLData("joysfxdown", my_itoa(getJoyButtonSFXDown()));
	XMLData *purchXML = new XMLData("p", my_itoa(getPurchased()));
	XMLData* key1 = new XMLData(
		"key1", keycode_to_keyname(getKey1()));
	XMLData* key2 = new XMLData(
		"key2", keycode_to_keyname(getKey2()));
	XMLData* key3 = new XMLData(
		"key3", keycode_to_keyname(getKey3()));
	XMLData* keyLeft = new XMLData(
		"keyLeft", keycode_to_keyname(getKeyLeft()));
	XMLData* keyRight = new XMLData(
		"keyRight", keycode_to_keyname(getKeyRight()));
	XMLData* keyUp = new XMLData(
		"keyUp", keycode_to_keyname(getKeyUp()));
	XMLData* keyDown = new XMLData(
		"keyDown", keycode_to_keyname(getKeyDown()));
	XMLData* keySettings = new XMLData(
		"keySettings", keycode_to_keyname(getKeySettings()));
	XMLData* keyFullscreen = new XMLData(
		"keyFullscreen", keycode_to_keyname(getKeyFullscreen()));
	XMLData* keySFXUp = new XMLData(
		"keySFXUp", keycode_to_keyname(getKeySFXUp()));
	XMLData* keySFXDown = new XMLData(
		"keySFXDown", keycode_to_keyname(getKeySFXDown()));
	XMLData* keyMusicUp = new XMLData(
		"keyMusicUp", keycode_to_keyname(getKeyMusicUp()));
	XMLData* keyMusicDown = new XMLData(
		"keyMusicDown", keycode_to_keyname(getKeyMusicDown()));
	XMLData* keyQuit = new XMLData(
		"keyQuit", keycode_to_keyname(getKeyQuit()));
	XMLData* keySortItems = new XMLData(
		"keySortItems", keycode_to_keyname(getKeySortItems()));
	XMLData* fpsOn = new XMLData(
		"fpsOn", my_itoa(fps_on));
	XMLData* alwaysCenter = new XMLData(
		"alwaysCenter", my_itoa(always_center));

#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
	XMLData* xml_dpad_type = new XMLData("dpad_type", old_control_mode < 0 ? my_itoa(getDpadType()) : my_itoa(old_control_mode));
	XMLData* xml_tellusertousedpad = new XMLData("tell_user_to_use_dpad", my_itoa(getTellUserToUseDpad()));
	XMLData *xml_swap_buttons = new XMLData("swap_buttons", my_itoa(getSwapButtons()));
#endif
	XMLData *xml_difficulty = new XMLData("difficulty", my_itoa(getDifficulty()));
	XMLData *xml_tuning = new XMLData("tuning", my_itoa(getTuning()));
#ifdef ALLEGRO_IPHONE
	XMLData *xml_shake_action = new XMLData("shake_action", my_itoa(getShakeAction()));
	//XMLData *xml_auto_rotate = new XMLData("auto_rotate", my_itoa(getAutoRotation()));
#endif
	XMLData* xml_lang = new XMLData("language", my_itoa(getLanguage()));

	game->add(joyb1);
	game->add(joyb2);
	game->add(joyb3);
	game->add(joymusicup);
	game->add(joymusicdown);
	game->add(joysfxup);
	game->add(joysfxdown);
	game->add(purchXML);
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
	game->add(fpsOn);
	game->add(alwaysCenter);

#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
	game->add(xml_dpad_type);
	game->add(xml_tellusertousedpad);
	game->add(xml_swap_buttons);
#endif
	game->add(xml_difficulty);
	game->add(xml_tuning);
#ifdef ALLEGRO_IPHONE
	game->add(xml_shake_action);
	//game->add(xml_auto_rotate);
#endif
	game->add(xml_lang);

	XMLData* gfx = new XMLData("gfx", "");
	ScreenDescriptor* sd = getWantedGraphicsMode();
	XMLData *width = new XMLData("width", my_itoa(config_save_width));
	XMLData *height = new XMLData("height", my_itoa(config_save_height));
	XMLData* fullscreen = new XMLData("fullscreen", my_itoa(sd->fullscreen));
	XMLData* vsync = new XMLData("vsync", my_itoa(getWaitForVsync()));
	XMLData *xml_aspect = new XMLData("maintain_aspect_ratio", my_itoa(getMaintainAspectRatio()));
	XMLData *xml_depth = new XMLData("depth_bits", my_itoa(getDepthBits()));
	gfx->add(width);
	gfx->add(height);
	gfx->add(fullscreen);
	gfx->add(vsync);
	gfx->add(xml_aspect);
	gfx->add(xml_depth);

	XMLData* sound = new XMLData("sound", "");
	XMLData* music_volume = new XMLData("music_volume", my_itoa(getMusicVolume()));
	XMLData* sfx_volume = new XMLData("sfx_volume", my_itoa(getSFXVolume()));
	sound->add(sfx_volume);
	sound->add(music_volume);
	
	XMLData *hacks = new XMLData("hacks", "");
	XMLData *rm_french_canada = new XMLData("rm_french_canada", my_itoa(1));
	XMLData *ouya_keyfix = new XMLData("ouya_keyfix", my_itoa(1));
	XMLData *ignorecfg = new XMLData("ignorecfg", my_itoa(0));
	hacks->add(rm_french_canada);
	hacks->add(ouya_keyfix);
	hacks->add(ignorecfg);

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

	gamepadAvailable(true),
	onlyMemoryBitmaps(false),
	lowCpuUsage(false),
	stick(0),
	adapter(0),
	axis(0),
	xbox360(false)
	,cfg_tuning(CFG_TUNING_PERFORMANCE)
	,cfg_difficulty(CFG_DIFFICULTY_NORMAL)
	,language(0)
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
	,cfg_dpad_type(0)
	,cfg_tellusertousedpad(true)
	,cfg_swap_buttons(false)
#endif
#ifdef ALLEGRO_IPHONE
	,cfg_shake_action(CFG_SHAKE_CANCEL)
	//,cfg_auto_rotation(2)
#endif
	,cfg_depth_bits(-1)
{
#ifdef EDITOR
	wantedMode.width = 800;
	wantedMode.height = 500;
#endif
	wantedMode.fullscreen = false;
}
