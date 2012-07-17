#include "monster2.hpp"

#include "java.h"

#include "sound-android.hpp"

static std::string shutdownMusicName = "";
static std::string shutdownAmbienceName = "";
static int total_samples = 0;
static int curr_sample = 0;
static std::string sample_name = "";
static int music = 0;
static int ambience = 0;
static float musicVolume = 1.0f;
static float ambienceVolume = 1.0f;

std::string check_music_name(std::string name, bool *is_flac);

static void destroyMusic_oldandroid(void)
{
	if (music) {
		bass_destroyMusic(music);
		music = 0;
	}
	if (ambience) {
		bass_destroyMusic(ambience);
		ambience = 0;
	}
	
	musicName = "";
	sample_name = "";
}

void initSound_oldandroid(void)
{
	bass_initSound();
}

void destroySound_oldandroid(void)
{
	if (!sound_inited) return;

	bass_shutdownBASS();
}

static void playSampleVolume_oldandroid(MSAMPLE s, float vol)
{
	if (!sound_inited) return;

	bass_playSampleVolume(s, vol);
}

MSAMPLE loadSample_oldandroid(std::string name)
{
	MSAMPLE s = 0;

	if (sound_inited) {
		ALLEGRO_PATH *p = al_get_standard_path(ALLEGRO_RESOURCES_PATH);
		char fn[1000];
		sprintf(fn, "%s/unpack/sfx/%s", al_path_cstr(p, '/'), name.c_str());
		al_destroy_path(p);
		s = bass_loadSample(fn);
	}

	return s;
}


void destroySample_oldandroid(MSAMPLE sample)
{
	if (!sound_inited) return;

	bass_destroySample(sample);
}


void playSample_oldandroid(MSAMPLE sample, MSAMPLE_ID *unused)
{
	(void)unused;
	if (!sound_inited) return;

	float vol = (float)config.getSFXVolume()/255.0;
	playSampleVolume_oldandroid(sample, vol);
}


void loadPlayDestroy_oldandroid(std::string name)
{
	if (!sound_inited) return;

	playPreloadedSample(name);
}

void stopAllSamples_oldandroid(void)
{
}

void playMusic_oldandroid(std::string name, float volume, bool force)
{
	if (!sound_inited) return;

	if (!force && musicName == name)
		return;

	if (name != "")
		musicName = name;

	if (music) {
		bass_destroyMusic(music);
	}

	if (name == "" || config.getMusicVolume() == 0) {
		music = 0;
		return;
	}

	bool is_flac;
	name = check_music_name(name, &is_flac);

	if (is_flac) {
		al_set_standard_file_interface();
	}
	music = bass_loadMusic(name.c_str());
	if (is_flac) {
		al_android_set_apk_file_interface();
	}
	bass_playMusic(music);
	setMusicVolume(volume);
}

void setMusicVolume_oldandroid(float volume)
{
	if (!sound_inited) return;

	musicVolume = volume;

	volume *= config.getMusicVolume()/255.0f;

	if (music) {
		bass_setMusicVolume(music, volume);
	}
}

void playAmbience_oldandroid(std::string name, float vol)
{
	if (!sound_inited) return;

	ambienceName = name;

	if (ambience) {
		bass_destroyMusic(ambience);
	}

	if (name == "" || config.getMusicVolume() == 0) {
		ambience = 0;
		return;
	}

	bool is_flac;
	name = check_music_name(name, &is_flac);

	if (is_flac) {
		al_set_standard_file_interface();
	}
	ambience = bass_loadMusic(name.c_str());
	if (is_flac) {
		al_android_set_apk_file_interface();
	}
	bass_playMusic(ambience);
	setAmbienceVolume(vol);
}

void setAmbienceVolume_oldandroid(float volume)
{
	if (!sound_inited) return;

	ambienceVolume = volume;

	volume *= config.getMusicVolume()/255.0f;

	if (ambience)
		bass_setMusicVolume(ambience, volume);
}

float getMusicVolume_oldandroid(void)
{
   return musicVolume;
}

float getAmbienceVolume_oldandroid(void)
{
   return ambienceVolume;
}

void unmuteMusic_oldandroid(void)
{
    playMusic_oldandroid(musicName, 1.0f, true);
}

void unmuteAmbience_oldandroid(void)
{
    playAmbience(ambienceName);
}

void restartMusic_oldandroid(void)
{
	playMusic_oldandroid(shutdownMusicName, 1.0f, true);
}

void restartAmbience_oldandroid(void)
{
	playAmbience_oldandroid(shutdownAmbienceName);
}

