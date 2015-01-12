#ifndef GAMECENTER_H
#define GAMECENTER_H

#define NOTYET 2

#define MS_REMOVED_SQUEEKY_BOARDS_3 @"grp.M2.0001"
#define MS_RIDER_JOINED_15 @"grp.M2.0002"
#define MS_DEFEATED_MONSTER_20 @"grp.M2.0003"
#define MS_GOT_BADGE_26 @"grp.M2.0004"
#define MS_RIOS_JOINED_30 @"grp.M2.0005"
//#define MS_SAILED_PERIOD_39 @"grp.M2.0006"
#define MS_GUNNAR_JOINED_40 @"grp.M2.0007"
#define MS_GOT_RING_43 @"grp.M2.0008"
#define MS_BEAT_WITCH_48 @"grp.M2.0009"
#define MS_KILLED_GOLEMS_56 @"grp.M2.0010"
#define MS_GOT_KEY_59 @"grp.M2.0011"
#define MS_GOT_MEDALLION_65 @"grp.M2.0012"
#define MS_BEACH_BATTLE_DONE_67 @"grp.M2.0013"
#define MS_GOT_MILK_74 @"grp.M2.0014"
#define MS_SUB_SCENE_DONE_76 @"grp.M2.0015"
#define MS_GOT_LOOKING_SCOPE_89 @"grp.M2.0016"
#define MS_DRAINED_POOL_87 @"grp.M2.0017"
#define MS_BEAT_TIGGY_96 @"grp.M2.0018"
#define MS_FREED_PRISONER_102 @"grp.M2.0019"
#define MS_BEAT_ARCHERY_98 @"grp.M2.0020"
#define MS_GOT_STAFF_123 @"grp.M2.0021"
#define MS_FOREST_GOLD_180 @"grp.M2.0022"
#define MS_BEAT_TREE_135 @"grp.M2.0023"
#define MS_BEAT_GIRL_DRAGON_149 @"grp.M2.0024"
#define MS_ON_MOON_153 @"grp.M2.0025"
#define MS_TIPPER_JOINED_154 @"grp.M2.0026"
#define MS_GOT_ORB_167 @"grp.M2.0027"
#define MS_MRBIG_CHEST_168 @"grp.M2.0028"
#define MS_BEAT_TODE_171 @"grp.M2.0029"
#define MS_SUN_SCENE_176 @"grp.M2.0030"
#define MS_DONE_CREDITS_177 @"grp.M2.0031"

bool isGameCenterAPIAvailable();
void authenticatePlayer(void);
void resetAchievements(void);
void do_milestone(int num, bool visual);
void showAchievements(void);

extern volatile bool modalViewShowing;

#endif
