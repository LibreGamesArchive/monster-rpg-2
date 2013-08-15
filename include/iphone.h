#ifndef IPHONE_H
#define IPHONE_H

#ifdef __cplusplus
extern "C" {
#endif

void addImageView();
void removeImageView();
void openRatingSite();
void openFeedbackSite();
void showIpod();
void shutdownIpod();
bool iPodIsPlaying();
void iPodPrevious();
void iPodNext();
void iPodPlay();
void iPodPause();
void iPodStop();
bool isIpodPresented();
bool get_clipboard(char *buf, int len);
void set_clipboard(char *buf);
float getBatteryLevel();
bool isMultitaskingSupported();
float getScreenScale();
void nslog_message(const char *fmt, ...);
void vibrate();
bool wifiConnected();
void disableMic();
void initiOSKeyboard();
void switchiOSKeyboardIn();

extern ALLEGRO_KEYBOARD_STATE icade_keyboard_state;

#ifdef __cplusplus
}
#endif

#endif
