#ifndef SOUND_HPP
#define SOUND_HPP

#if !defined KCM_AUDIO
#include <bass.h>
typedef HSAMPLE MSAMPLE;
typedef int MSAMPLE_ID;
#else
typedef ALLEGRO_SAMPLE * MSAMPLE;
typedef ALLEGRO_SAMPLE_ID MSAMPLE_ID;
#endif

extern std::string musicName;
extern std::string ambienceName;
extern bool sound_inited;

void unmuteMusic(void);
void unmuteAmbience(void);
void playMusic(std::string name, bool setLoopStart = false, unsigned int loopStart = 0, bool force = false);
void setMusicVolume(float volume);
void setAmbienceVolume(float volume);
void playPreloadedSample(std::string name);
void destroySound(void);
void initSound(void);
MSAMPLE loadSample(std::string name);
void destroySample(MSAMPLE sample);
void loadPlayDestroy(std::string name);
void playAmbience(std::string name);
void stopAmbience(void);
void destroyAmbience(void);
void playSample(MSAMPLE sample, MSAMPLE_ID *id = NULL);
float getMusicVolume(void);
float getAmbienceVolume(void);
void stopAllSamples(void);
void restartMusic(void);
void restartAmbience(void);
void playMusicVolumeOff(std::string name);
	
#endif

