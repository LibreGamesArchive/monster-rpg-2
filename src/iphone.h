#ifndef IPHONE_H
#define IPHONE_H

#define NOTYET 2

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

#define MS_REMOVED_SQUEEKY_BOARDS_3 @"M2.0001"
#define MS_RIDER_JOINED_15 @"M2.0002"
#define MS_DEFEATED_MONSTER_20 @"M2.0003"
#define MS_GOT_BADGE_26 @"M2.0004"
#define MS_RIOS_JOINED_30 @"M2.0005"
//#define MS_SAILED_PERIOD_39 @"M2.0006"
#define MS_GUNNAR_JOINED_40 @"M2.0007"
#define MS_GOT_RING_43 @"M2.0008"
#define MS_BEAT_WITCH_48 @"M2.0009"
#define MS_KILLED_GOLEMS_56 @"M2.0010"
#define MS_GOT_KEY_59 @"M2.0011"
#define MS_GOT_MEDALLION_65 @"M2.0012"
#define MS_BEACH_BATTLE_DONE_67 @"M2.0013"
#define MS_GOT_MILK_74 @"M2.0014"
#define MS_SUB_SCENE_DONE_76 @"M2.0015"
#define MS_GOT_LOOKING_SCOPE_89 @"M2.0016"
#define MS_DRAINED_POOL_87 @"M2.0017"
#define MS_BEAT_TIGGY_96 @"M2.0018"
#define MS_FREED_PRISONER_102 @"M2.0019"
#define MS_BEAT_ARCHERY_98 @"M2.0020"
#define MS_GOT_STAFF_123 @"M2.0021"
#define MS_FOREST_GOLD_180 @"M2.0022"
#define MS_BEAT_TREE_135 @"M2.0023"
#define MS_BEAT_GIRL_DRAGON_149 @"M2.0024"
#define MS_ON_MOON_153 @"M2.0025"
#define MS_TIPPER_JOINED_154 @"M2.0026"
#define MS_GOT_ORB_167 @"M2.0027"
#define MS_MRBIG_CHEST_168 @"M2.0028"
#define MS_BEAT_TODE_171 @"M2.0029"
#define MS_SUN_SCENE_176 @"M2.0030"
#define MS_DONE_CREDITS_177 @"M2.0031"

BOOL isGameCenterAPIAvailable();
void authenticatePlayer(void);
void resetAchievements(void);
void do_milestone(int num);
void showAchievements(void);

extern bool modalViewShowing;

#ifdef __cplusplus
}
#endif

#endif
