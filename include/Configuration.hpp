#ifndef M2_CONFIG_H
#define M2_CONFIG_H

#include "monster2.hpp"

enum ASPECT_RATIO
{
	ASPECT_FILL_SCREEN,
	ASPECT_INTEGER,
	ASPECT_MAINTAIN_RATIO
};

enum DPAD_TYPE 
{
	DPAD_NONE = 0,
	DPAD_HYBRID_1,
	DPAD_HYBRID_2,
	DPAD_TOTAL_1,
	DPAD_TOTAL_2
};

enum CFG_DIFFICULTY
{
	CFG_DIFFICULTY_EASY = 0,
	CFG_DIFFICULTY_NORMAL = 1,
	CFG_DIFFICULTY_HARD = 2
};

enum CFG_TUNING
{
	CFG_TUNING_BATTERY = 0,
	CFG_TUNING_BALANCED = 1,
	CFG_TUNING_PERFORMANCE = 2
};

enum CFG_SHAKE_ACTION
{
	CFG_SHAKE_CANCEL = 0,
	CFG_SHAKE_CHANGE_SONG = 1
};

class Configuration {
public:
	ScreenDescriptor* getWantedGraphicsMode();
	void setWantedGraphicsMode(ScreenDescriptor& sd);
	bool showDebugMessages();
	void setShowDebugMessages(bool show);
	bool useCompiledScripts();
	bool getWaitForVsync();
	void setWaitForVsync(bool vs);
	bool getGamepadAvailable();
	void setGamepadAvailable(bool available);
	bool useSmoothScaling();
	void setUseSmoothScaling(bool smooth);
	int getSFXVolume();
	void setSFXVolume(int volume);
	int getMusicVolume();
	void setMusicVolume(int volume);
	int getJoyButton1();
	void setJoyButton1(int b1);
	int getJoyButton2();
	void setJoyButton2(int b2);
	int getJoyButton3();
	void setJoyButton3(int b3);
	int getKey1();
	void setKey1(int k1);
	int getKey2();
	void setKey2(int k2);
	int getKey3();
	void setKey3(int k2);
	int getKeyLeft();
	void setKeyLeft(int kl);
	int getKeyRight();
	void setKeyRight(int kr);
	int getKeyUp();
	void setKeyUp(int ku);
	int getKeyDown();
	void setKeyDown(int kd);
	int getKeySettings();
	int getKeyFullscreen();
	int getKeySFXUp();
	int getKeySFXDown();
	int getKeyMusicUp();
	int getKeyMusicDown();
	void setKeySettings(int k);
	void setKeyFullscreen(int k);
	void setKeySFXUp(int k);
	void setKeySFXDown(int k);
	void setKeyMusicUp(int k);
	void setKeyMusicDown(int k);
	int getKeyQuit();
	void setKeyQuit(int k);
	int getKeySortItems();
	void setKeySortItems(int k);
	int getTargetFPS();
	bool getUseOnlyMemoryBitmaps();
	void setUseOnlyMemoryBitmaps(bool onlyMem);
	bool getLowCpuUsage();
	void setLowCpuUsage(bool low);
	int getStick();
	void setStick(int s);
	int getAxis();
	void setAxis(int a);
	bool getXbox360();
	void setXbox360(bool x);
	int getAdapter();
	void setAdapter(int a);
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
	int getDpadType() { return cfg_dpad_type; }
	void setDpadType(int b) { cfg_dpad_type = b; }
	bool getTellUserToUseDpad() { return cfg_tellusertousedpad; }
	void setTellUserToUseDpad(bool b) { cfg_tellusertousedpad = b; }
	bool getSwapButtons() { return cfg_swap_buttons; }
	void setSwapButtons(bool b) { cfg_swap_buttons = b; }
#endif
	int getDifficulty() { return cfg_difficulty; }
	void setDifficulty(int d) { cfg_difficulty = d; }
	int getTuning() { return cfg_tuning; }
	void setTuning(int t) { cfg_tuning = t; }
#ifdef ALLEGRO_IPHONE
	int getShakeAction() { return cfg_shake_action; }
	void setShakeAction(int t) { cfg_shake_action = t; }
#endif
#ifdef ALLEGRO_IPHONE
	int getAutoRotation() {
		return cfg_auto_rotation;
	}
	void setAutoRotation(int auto_rot) {
		cfg_auto_rotation = auto_rot;
		if (display) {
			if (auto_rot == 0)
			{
				al_set_display_option(display, ALLEGRO_SUPPORTED_ORIENTATIONS, ALLEGRO_DISPLAY_ORIENTATION_270_DEGREES);
			}
			else if (auto_rot == 1)
			{
				al_set_display_option(display, ALLEGRO_SUPPORTED_ORIENTATIONS, ALLEGRO_DISPLAY_ORIENTATION_90_DEGREES);
			}
			else
			{
				al_set_display_option(display, ALLEGRO_SUPPORTED_ORIENTATIONS, ALLEGRO_DISPLAY_ORIENTATION_LANDSCAPE);
			}
		}
	}
#endif
	int getMaintainAspectRatio() {
		return cfg_maintain_aspect_ratio;
	}
	void setMaintainAspectRatio(int m) {
		cfg_maintain_aspect_ratio = m;
	}

	int getDepthBits() {
		return cfg_depth_bits;
	}
	void setDepthBits(int b) {
		cfg_depth_bits = b;
	}

	void setLanguage(int l);
	int getLanguage();
	bool getAutoconnectToZeemote();
	void setAutoconnectToZeemote(bool ac);
	// returns true if there was a config file
	bool read();
	void write();
	Configuration();
private:
	ScreenDescriptor wantedMode;	// wanted screen resolution & depth
	bool debugMessages;		// show debugging messages?
	bool compiledScripts;		// use compiled scripts?
	bool waitForVsync;
	bool loaded;	// loaded from file or default?
	bool smoothScaling;
	int sfxVolume;
	int musicVolume;
	int joyB1;
	int joyB2;
	int joyB3;
	int key1;
	int key2;
	int key3;
	int keyLeft;
	int keyRight;
	int keyUp;
	int keyDown;
	int keySettings;
	int keyFullscreen;
	int keySFXUp;
	int keySFXDown;
	int keyMusicUp;
	int keyMusicDown;
	int keyQuit;
	int keySortItems;
	bool gamepadAvailable;
	bool onlyMemoryBitmaps;
	bool lowCpuUsage;
	int stick;
	int adapter;
	int axis;
	bool xbox360;
	int cfg_tuning;
	int cfg_difficulty;
	int cfg_maintain_aspect_ratio;
	int language;
	bool fixed_language;
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
	int cfg_dpad_type;
	bool cfg_tellusertousedpad;
	int cfg_swap_buttons;
#endif
#ifdef ALLEGRO_IPHONE
	int cfg_shake_action;
	int cfg_auto_rotation;
#endif
	int cfg_depth_bits;
};

extern Configuration config;

bool do_config();
const char *keycode_to_keyname(int i);

#endif
