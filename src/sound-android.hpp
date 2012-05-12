#ifndef SOUND_ANDROID_HPP
#define SOUND_ANDROID_HPP

std::string check_music_name(std::string name, bool *is_flac);

void unmuteMusic_oldandroid(void);
void unmuteAmbience_oldandroid(void);
float getMusicVolume_oldandroid(void);
float getAmbienceVolume_oldandroid(void);
void playMusic_oldandroid(std::string name, float vol = 1.0f, bool force = false);
void playAmbience_oldandroid(std::string name, float vol = 1.0f);
void setMusicVolume_oldandroid(float volume);
void setAmbienceVolume_oldandroid(float volume);
void destroySound_oldandroid(void);
void initSound_oldandroid(void);
MSAMPLE loadSample_oldandroid(std::string name);
void destroySample_oldandroid(MSAMPLE sample);
void loadPlayDestroy_oldandroid(std::string name);
void stopAmbience_oldandroid(void);
void destroyAmbience_oldandroid(void);
void playSample_oldandroid(MSAMPLE sample, MSAMPLE_ID *id = NULL);
void stopAllSamples_oldandroid(void);
void restartMusic_oldandroid(void);
void restartAmbience_oldandroid(void);
void playMusicVolumeOff_oldandroid(std::string name);

#endif

