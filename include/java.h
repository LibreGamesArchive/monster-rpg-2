#ifndef __java_h
#define __java_h

#ifdef __cplusplus
extern "C" {
#endif

#include <bass.h>

#include "input_descriptor.h"

void bass_initSound();
HSAMPLE bass_loadSample(const char *name);;
HSAMPLE bass_loadSampleLoop(const char *name);
void bass_playSampleVolumePan(HSAMPLE s, float vol, float pan);
void bass_playSampleVolume(HSAMPLE s, float vol);
void bass_playSample(HSAMPLE s);
void bass_stopSample(HSAMPLE s);
HMUSIC bass_loadMusic(const char *name);
void bass_playMusic(HMUSIC music);
void bass_stopMusic(HMUSIC music);
void bass_destroyMusic(HMUSIC music);
void bass_shutdownBASS();
void bass_destroySample(HSAMPLE s);
void bass_setMusicVolume(HMUSIC music, float vol);

void openURL(const char *url);

bool get_clipboard(char *buf, int len);
void set_clipboard(char *buf);

const char *get_sdcarddir();

bool wifiConnected();

struct InputDescriptor get_zeemote_state();
extern bool zeemote_connected;
void find_zeemotes();
void autoconnect_zeemote();
void goHome();
void logString(const char *s);

#if defined OUYA
int isPurchased();
void queryPurchased();
void doIAP();
int checkPurchased();
#endif

#ifdef __cplusplus
}
#endif

#endif
