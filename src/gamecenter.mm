#import <AppKit/AppKit.h>

// Game Center stuff
#define NOSOUND
#include "monster2.hpp"
#undef NOSOUND

#if defined ALLEGRO_MACOSX
#include <allegro5/allegro_osx.h>
#elif defined ALLEGRO_IPHONE
#include <allegro5/allegro_iphone_objc.h>
#endif

int is_authenticated = NOTYET;

#ifndef NO_GAMECENTER
#include "mygamecentervc.h"

NSMutableDictionary *achievementsDictionary;
#endif

BOOL isGameCenterAPIAvailable()
{
#ifdef NO_GAMECENTER
	return FALSE;
#else
	// Check for presence of GKLocalPlayer class.
	BOOL localPlayerClassAvailable = (NSClassFromString(@"GKLocalPlayer")) != nil;

	BOOL osVersionSupported = FALSE;

#ifdef ALLEGRO_IPHONE
	// The device must be running iOS 4.1 or later.
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	NSString *reqSysVer = @"4.1";
	NSString *currSysVer = [[UIDevice currentDevice] systemVersion];
	osVersionSupported = ([currSysVer compare:reqSysVer options:NSNumericSearch] != NSOrderedAscending);
	[pool drain];
#else
	OSErr err;
	SInt32 systemVersion;
	if ((err = Gestalt(gestaltSystemVersion, &systemVersion)) == noErr) {
		if (systemVersion >= 0x1080) {
			osVersionSupported = TRUE;
		}
	}
#endif

	return (localPlayerClassAvailable && osVersionSupported);
#endif
}

#define NUM_ACHIEVEMENTS 30

#ifndef NO_GAMECENTER
void reportAchievementIdentifier(NSString* identifier, bool notification);
#endif

void authenticatePlayer(void)
{
	if (!isGameCenterAPIAvailable()) {
		is_authenticated = 0;
		return;
	}

#ifndef NO_GAMECENTER
	GKLocalPlayer *localPlayer = [GKLocalPlayer localPlayer];

#ifdef ALLEGRO_IPHONE
	BOOL osVersionSupported;
	NSString *reqSysVer = @"6.0";
	NSString *currSysVer = [[UIDevice currentDevice] systemVersion];
	osVersionSupported = ([currSysVer compare:reqSysVer options:NSNumericSearch] != NSOrderedAscending);

	if (osVersionSupported) {
		[localPlayer setAuthenticateHandler:^(UIViewController *viewController, NSError *error) {
			if (viewController != nil) {
				al_iphone_set_statusbar_orientation(ALLEGRO_IPHONE_STATUSBAR_ORIENTATION_PORTRAIT);
				UIWindow *win = al_iphone_get_window(display);
				[win.rootViewController presentModalViewController:viewController animated:TRUE];
			}
			else if (localPlayer.isAuthenticated) {
				is_authenticated = true;
			}
			else {
				is_authenticated = false;
			}
		}];
	}
	else
#endif
	{
		[localPlayer authenticateWithCompletionHandler:^(NSError *error) {
			if (localPlayer.isAuthenticated)
			{
				// Perform additional tasks for the authenticated player.
				is_authenticated = 1;
			}
			else {
				printf("Game Center authentication error: code %ld\n", (long)[error code]);
				is_authenticated = 0;
			}
		}];
	}
#endif
}

bool reset_complete = false;

void resetAchievements(void)
{
	if (!isGameCenterAPIAvailable() || !is_authenticated)
		return;

#ifndef NO_GAMECENTER
	// Clear all locally saved achievement objects.
	achievementsDictionary = [[NSMutableDictionary alloc] init];

	// Clear all progress saved on Game Center
	[GKAchievement resetAchievementsWithCompletionHandler:^(NSError *error)
	 {
		 if (error != nil) {
			 // handle errors
		 }
		 reset_complete = true;
	 }];
#endif
}

#ifndef NO_GAMECENTER
void reportAchievementIdentifier(NSString* identifier, bool notification)
{
	if (!isGameCenterAPIAvailable() || !is_authenticated)
		return;

	if ([achievementsDictionary objectForKey:identifier] != nil) {
		return;
	}
	
	float percent = 100;
	
	GKAchievement *achievement = [[[GKAchievement alloc] initWithIdentifier: identifier] autorelease];
	if (achievement)
	{
		[achievementsDictionary setObject:achievement forKey:identifier];
		achievement.percentComplete = percent;
		[achievement reportAchievementWithCompletionHandler:^(NSError *error)
		 {
			 if (error != nil)
			 {
			 }
		 }];
	}
}

struct Holder
{
	int num;
	NSString *ident;
};
#endif

void do_milestone(int num, bool visual)
{
#ifdef NO_GAMECENTER
	return;
#else
	num++;

	Holder holders[NUM_ACHIEVEMENTS] = {
		{ 3, MS_REMOVED_SQUEEKY_BOARDS_3 },
		{ 15, MS_RIDER_JOINED_15 },
		{ 20, MS_DEFEATED_MONSTER_20 },
		{ 26, MS_GOT_BADGE_26 },
		{ 30, MS_RIOS_JOINED_30 },
		{ 40, MS_GUNNAR_JOINED_40 },
		{ 43, MS_GOT_RING_43 },
		{ 48, MS_BEAT_WITCH_48 },
		{ 56, MS_KILLED_GOLEMS_56 },
		{ 59, MS_GOT_KEY_59 },
		{ 65, MS_GOT_MEDALLION_65 },
		{ 67, MS_BEACH_BATTLE_DONE_67 },
		{ 74, MS_GOT_MILK_74 },
		{ 76, MS_SUB_SCENE_DONE_76 },
		{ 89, MS_GOT_LOOKING_SCOPE_89 },
		{ 87, MS_DRAINED_POOL_87 },
		{ 96, MS_BEAT_TIGGY_96 },
		{ 102, MS_FREED_PRISONER_102 },
		{ 98, MS_BEAT_ARCHERY_98 },
		{ 123, MS_GOT_STAFF_123 },
		{ 180, MS_FOREST_GOLD_180 },
		{ 135, MS_BEAT_TREE_135 },
		{ 149, MS_BEAT_GIRL_DRAGON_149 },
		{ 153, MS_ON_MOON_153 },
		{ 154, MS_TIPPER_JOINED_154 },
		{ 167, MS_GOT_ORB_167 },
		{ 168, MS_MRBIG_CHEST_168 },
		{ 171, MS_BEAT_TODE_171 },
		{ 176, MS_SUN_SCENE_176 },
		{ 177, MS_DONE_CREDITS_177 }
	};

	for (int i = 0; i < NUM_ACHIEVEMENTS; i++)
	{
		if (holders[i].num == num) {
			reportAchievementIdentifier(holders[i].ident, true);
			if (visual) {
				achievement_time = al_get_time();
				achievement_show = true;
			}
			return;
		}
	}
#endif
}

volatile bool modalViewShowing = false;

extern bool show_item_info_on_flip; // FIXME

void showAchievements(void)
{
#ifndef NO_GAMECENTER
	set_target_backbuffer();
	m_clear(black);
	m_flip_display();

#ifdef ALLEGRO_IPHONE
	MyGameCenterVC *uv = [[MyGameCenterVC alloc] initWithNibName:nil bundle:nil];
	[uv performSelectorOnMainThread: @selector(showAchievements) withObject:nil waitUntilDone:YES];
	while (modalViewShowing) {
		al_rest(0.001);
	}
	[uv release];
	
	UIWindow *window = al_iphone_get_window(display);
	UIView *view = al_iphone_get_view(display);
	[window bringSubviewToFront:view];
	clear_input_events();
#else
	ScreenDescriptor *sd = config.getWantedGraphicsMode();
	bool was_fullscreen = sd->fullscreen;
	if (sd->fullscreen) {
		toggle_fullscreen();
		set_target_backbuffer();
		m_clear(black);
		m_flip_display();
	}

	modalViewShowing = true;
	MyGameCenterVC *vc = [[MyGameCenterVC alloc] init];
	[vc performSelectorOnMainThread: @selector(showAchievements) withObject:nil waitUntilDone:FALSE];
	while (modalViewShowing) {
		al_rest(0.001);
	}
	[vc release];

	al_rest(1.0);

	if (was_fullscreen) {
		toggle_fullscreen();
	}

	clear_input_events();
#endif
#endif
}

