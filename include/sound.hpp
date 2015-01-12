#ifndef SOUND_HPP
#define SOUND_HPP

#if defined KCM_AUDIO
typedef ALLEGRO_SAMPLE *MSAMPLE;
typedef ALLEGRO_SAMPLE_ID MSAMPLE_ID;
#elif defined ALLEGRO_ANDROID || defined NO_BASS
typedef int MSAMPLE;
typedef int MSAMPLE_ID;
#else
typedef HSAMPLE MSAMPLE;
typedef int MSAMPLE_ID;
#endif

extern std::string musicName;
extern std::string ambienceName;
extern bool sound_inited;

void unmuteMusic(void);
void unmuteAmbience(void);
void playMusic(std::string name, float vol = 1.0f, bool force = false);
void playAmbience(std::string name, float vol = 1.0f);
void playPreloadedSample(std::string name);
void destroySound(void);
void initSound(void);
MSAMPLE loadSample(std::string name);
void destroySample(MSAMPLE sample);
void loadPlayDestroy(std::string name);
void stopAmbience(void);
void destroyAmbience(void);
void playSample(MSAMPLE sample, MSAMPLE_ID *id = NULL);
void stopAllSamples(void);
void restartMusic(void);
void restartAmbience(void);
void playMusicVolumeOff(std::string name);
void setStreamVolume(MSAMPLE stream, float volume);
MSAMPLE streamSample(std::string name, float vol);
void destroyStream(MSAMPLE stream);

bool loadSamples(void (*cb)(int, int));
std::string check_music_name(std::string name, bool *is_flac);

// Called from JNI
#ifdef __cplusplus
extern "C" {
#endif
	void setMusicVolume(float volume);
	void setAmbienceVolume(float volume);
	float getMusicVolume(void);
	float getAmbienceVolume(void);
#ifdef __cplusplus
}
#endif

#endif

