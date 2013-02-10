#ifndef IPHONE_H
#define IPHONE_H

#ifdef __cplusplus
extern "C" {
#endif

void addImageView(void);
void removeImageView(void);
void openRatingSite(void);
void openFeedbackSite(void);
void showIpod(void);
void shutdownIpod(void);
bool iPodIsPlaying(void);
void iPodPrevious(void);
void iPodNext(void);
void iPodPlay(void);
void iPodPause(void);
void iPodStop(void);
bool isIpodPresented(void);
bool get_clipboard(char *buf, int len);
void set_clipboard(char *buf);
float getBatteryLevel(void);
bool isMultitaskingSupported(void);
float getScreenScale(void);
void nslog_message(const char *fmt, ...);
void vibrate(void);
bool wifiConnected(void);
void disableMic(void);
void initiOSKeyboard();

#ifdef __cplusplus
}
#endif

#endif
