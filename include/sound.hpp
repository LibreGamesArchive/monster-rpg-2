#ifndef SOUND_HPP
#define SOUND_HPP

#ifdef ALLEGRO_ANDROID
typedef int MSAMPLE;
typedef int MSAMPLE_ID;
#else
#if !defined KCM_AUDIO
typedef HSAMPLE MSAMPLE;
typedef int MSAMPLE_ID;
#else
typedef ALLEGRO_SAMPLE *MSAMPLE;
typedef ALLEGRO_SAMPLE_ID MSAMPLE_ID;
#endif
#endif

extern std::string musicName;
extern std::string ambienceName;
extern bool sound_inited;

void unmuteMusic(void);
void unmuteAmbience(void);
void playMusic(std::string name, float vol = 1.0f, bool force = false);
void playAmbience(std::string name, float vol = 1.0f);
void setMusicVolume(float volume);
void setAmbienceVolume(float volume);
void playPreloadedSample(std::string name);
void destroySound(void);
void initSound(void);
MSAMPLE loadSample(std::string name);
void destroySample(MSAMPLE sample);
void loadPlayDestroy(std::string name);
void stopAmbience(void);
void destroyAmbience(void);
void playSample(MSAMPLE sample, MSAMPLE_ID *id = NULL);
float getMusicVolume(void);
float getAmbienceVolume(void);
void stopAllSamples(void);
void restartMusic(void);
void restartAmbience(void);
void playMusicVolumeOff(std::string name);
void setStreamVolume(MSAMPLE stream, float volume);
MSAMPLE streamSample(std::string name, float vol);
void destroyStream(MSAMPLE stream);

bool loadSamples(void (*cb)(int, int));
std::string check_music_name(std::string name, bool *is_flac);

#endif

