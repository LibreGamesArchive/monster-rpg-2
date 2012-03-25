#include "monster2.hpp"
#define ASSERT ALLEGRO_ASSERT
#include <allegro5/platform/aintiphone.h>
#include <allegro5/allegro_opengl.h>
#include <zlib.h>
//#import "MyMusicPlayer.h"
#include "iphone.h"
#import <Foundation/Foundation.h>
#import <MobileCoreServices/MobileCoreServices.h>
#import <UIKit/UIKit.h>
#import <MediaPlayer/MediaPlayer.h>
#import <AudioToolbox/AudioToolbox.h>
#import "MyUIViewController.h"
#include <allegro5/allegro_iphone.h>

extern "C" {
#include <allegro5/allegro_iphone_objc.h>
}

static MPMusicPlayerController *musicPlayer;

void openRatingSite(void)
{
	NSAutoreleasePool *p = [[NSAutoreleasePool alloc] init];
	const char *url_ascii = "http://www.monster-rpg.com";
	NSString *u = [[NSString alloc] initWithCString:url_ascii encoding:NSUTF8StringEncoding];
	CFURLRef url = CFURLCreateWithString(NULL, (CFStringRef)u, NULL);
	[[UIApplication sharedApplication] openURL:(NSURL *)url];
	[u release];
	CFRelease(url);
	[p drain];
	exit(0);
}

void openFeedbackSite(void)
{
	NSAutoreleasePool *p = [[NSAutoreleasePool alloc] init];
	const char *url_ascii = "http://www.monster-rpg.com/feedback.html";
	NSString *u = [[NSString alloc] initWithCString:url_ascii encoding:NSUTF8StringEncoding];
	CFURLRef url = CFURLCreateWithString(NULL, (CFStringRef)u, NULL);
	[[UIApplication sharedApplication] openURL:(NSURL *)url];
	[u release];
	CFRelease(url);
	[p drain];
	exit(0);
}


MPMusicPlayerController *getMusicPlayer(void)
{
	NSAutoreleasePool *p = [[NSAutoreleasePool alloc] init];

	if (musicPlayer == nil) {
 		musicPlayer = [MPMusicPlayerController iPodMusicPlayer];
	}
	[p drain];
	return musicPlayer;
}

/*
bool isIpodPresented(void)
{
	if (the_controller == nil) return false;
	return the_controller.iPodIsPresented;
}
 */

struct Album {
	std::vector<std::string> songs;
	std::vector<int> song_overall_num;
};

struct Artist {
	std::map<std::string, Album> albums;
};

void showIpod(void)
{
	NSAutoreleasePool *p = [[NSAutoreleasePool alloc] init];

	global_draw_red = false;

	MBITMAP *bg = m_load_bitmap(getResource("media/options_bg.png"));
	m_set_target_bitmap(buffer);
	m_clear(m_map_rgb(0, 0, 0));

	mTextout_simple("Please wait...",
		BW/2-m_text_length(game_font, "Please wait...")/2,
		BH/2-m_text_height(game_font)/2, m_map_rgb(255, 255, 255));
	
	drawBufferToScreen();
	m_flip_display();

	std::vector<std::string> all_names;
	std::vector<bool> all_toggled;

	std::map<std::string, Artist> artists;

	MPMediaQuery *query = [MPMediaQuery albumsQuery];
	NSArray *array = [query items];
	int num = [array count];
	for (int i = 0; i < num; i++) {
		MPMediaItem *m = [array objectAtIndex:i];
		NSNumber *track_o = [m valueForProperty:MPMediaItemPropertyAlbumTrackNumber];
		char track[10];
		sprintf(track, "%02d", [track_o intValue]);
		NSString *album = [m valueForProperty:MPMediaItemPropertyAlbumTitle];
		const char *album_c = [album UTF8String];
		NSString *artist = [m valueForProperty:MPMediaItemPropertyArtist];
		const char *artist_c = [artist UTF8String];
		NSString *title = [m valueForProperty:MPMediaItemPropertyTitle];
		const char *title_c = [title UTF8String];

		all_names.push_back(std::string(title_c));
		all_toggled.push_back(false);

		if (!artists.count(std::string(artist_c))) {
			Artist a;
			artists[std::string(artist_c)] = a;
		}
		if (!artists[std::string(artist_c)].albums.count(std::string(album_c))) {
			Album a;
			artists[std::string(artist_c)].albums[std::string(album_c)] = a;
		}
		artists[std::string(artist_c)].albums[std::string(album_c)].songs.push_back(std::string(track) + " " + std::string(title_c));
		artists[std::string(artist_c)].albums[std::string(album_c)].song_overall_num.push_back(i);
	}
	
	// Sort songs per album
	for (std::map<std::string, Artist>::iterator it = artists.begin();
		 it != artists.end(); it++) {
		for (std::map<std::string, Album>::iterator it2 = artists[(*it).first].albums.begin(); it2 != artists[(*it).first].albums.end(); it2++) {
			std::vector<std::string> &vs = artists[(*it).first].albums[(*it2).first].songs;
			std::vector<int> &vi = artists[(*it).first].albums[(*it2).first].song_overall_num;
			for (int i = 0; i < (int)vs.size(); i++) {
				for (int j = i; j < (int)vs.size(); j++) {
					if (vs[i] > vs[j]) {
						std::string tmps;
						int tmpi;
						tmps = vs[i];
						tmpi = vi[i];
						vs[i] = vs[j];
						vi[i] = vi[j];
						vs[j] = tmps;
						vi[j] = tmpi;
					}
				}
			}
		}
	}

	// The full list is already sorted
	/*
	std::vector<int> order;

	for (int i = 0; i < num; i++) {
		order.push_back(i);
	}

	for (int i = 0; i < num; i++) {
		for (int j = i+1; j < num; j++) {
			if (all_names[i] > all_names[j]) {
				std::string tmp_s = all_names[i];
				int tmp_i = order[i];
				all_names[i] = all_names[j];
				order[i] = order[j];
				all_names[j] = tmp_s;
				order[j] = tmp_i;
			}
		}
	}
	*/

	std::vector< std::vector<bool> > bools;
	std::string curr_artist = "";
	std::string curr_album = "";
	int curr_artist_i = 0;
	int curr_album_i = 0;
	int curr_album_all_inclusive = 0;
	std::vector<std::string> artist_names;
	std::vector< std::vector<std::string> > album_names;
	std::vector< std::vector<std::string> > song_names;
	int artist_index = 0;
	std::vector< int > album_index;
	std::vector< std::vector< int > > song_index;
	int all_songs_index = 0;
	int i = 0, j = 0;
	for (std::map<std::string, Artist>::iterator it = artists.begin();
			it != artists.end(); it++) {
		artist_names.push_back((*it).first);
		album_names.push_back(std::vector<std::string>());
		for (std::map<std::string, Album>::iterator it2 = artists[(*it).first].albums.begin(); it2 != artists[(*it).first].albums.end(); it2++) {
			album_names[j].push_back((*it2).first);
			bools.push_back(std::vector<bool>());
			song_names.push_back(std::vector<std::string>());
			for (std::vector<std::string>::iterator it3 = artists[(*it).first].albums[(*it2).first].songs.begin(); it3 != artists[(*it).first].albums[(*it2).first].songs.end(); it3++) {
				song_names[i].push_back((*it3));
				bools[i].push_back(false);
			}
			i++;
		}
		j++;
	}

	
	for (int i = 0; i < (int)album_names.size(); i++) {
		song_index.push_back(std::vector<int>());
		for (int j = 0; j < (int)album_names[i].size(); j++) {
			song_index[i].push_back(0);
		}
	}
	for (int i = 0; i < (int)artist_names.size(); i++) {
		album_index.push_back(0);
	}

	int toplevel_choice = -1;
	int section = 0;

	std::vector<std::string> toplevel_choices;
	toplevel_choices.push_back("All");
	toplevel_choices.push_back("Artists");

	MScrollingList *list = new MScrollingList(3, 35, BW-10, BH-30-50, NULL, 0, 0, NULL, NULL, false);
	list->setItems(toplevel_choices);

	MToggleList *song_list = new MToggleList(3, 35, BW-10, BH-30-50, false);

	MIcon *check = new MIcon(BW/2-20, BH-42,
							 getResource("media/check.png"), m_map_rgb(255, 255, 255),
							 true, NULL, false, true);
	MIcon *backarrow = new MIcon(3, BH-32,
		getResource("media/backarrow.png"), m_map_rgb(255, 255, 255),
		true, NULL, false, true);
	
	tguiPush();
	
	tguiSetParent(0);
	tguiAddWidget(backarrow);
	tguiAddWidget(list);
	tguiAddWidget(check);
	tguiSetFocus(list);

	NSMutableArray *picked = NULL;

	int count = 0;
	int k = 0;

	if (dpad_type == DPAD_TOTAL_1 || dpad_type == DPAD_TOTAL_2) {
		myTguiIgnore(TGUI_MOUSE);
		getInput()->reset();
	}

	while (1) {
		int tmp_counter = logic_counter;
		logic_counter = 0;
		if (tmp_counter > 10)
			tmp_counter = 1;
		while  (tmp_counter > 0) {
			next_input_event_ready = true;
			
			tmp_counter--;

			if (close_pressed) {
				do_close();
				close_pressed = false;
			}
			
			TGUIWidget *w = tguiUpdate();

			if (w && w == check) {
				if (toplevel_choice == 1) {
					std::vector<bool> b = song_list->getToggled();
					bools[curr_album_all_inclusive].clear();
					for (int i = 0; i < (int)b.size(); i++) {
						bools[curr_album_all_inclusive].push_back(b[i]);
					}
				}
				else if (toplevel_choice == 0) {
					std::vector<bool> b = song_list->getToggled();
					for (int i = 0; i < (int)b.size(); i++) {
						all_toggled[i] = b[i];
					}
				}
				goto done;
			}
			else if (w && w == list) {
				int sel = list->getSelected();
				if (toplevel_choice == -1) {
					if (sel >= 0) {
						toplevel_choice = sel;
						if (toplevel_choice == 0) {
							tguiDeleteWidget(list);
							tguiDeleteWidget(check);
							tguiDeleteWidget(backarrow);
							song_list->setItems(all_names);
							song_list->setTop(all_songs_index);
							tguiAddWidget(backarrow);
							tguiAddWidget(song_list);
							tguiAddWidget(check);
							tguiSetFocus(song_list);
						}
						else {
							list->setItems(artist_names);
							list->setTop(artist_index);
						}
					}
				}
				else if (toplevel_choice == 1) {
					if (section == 0) {
						curr_artist_i = sel;
						curr_artist = artist_names[sel];
						artist_index = list->getTop();
						list->setItems(album_names[sel]);
						list->setTop(album_index[curr_artist_i]);
						section++;
					}
					else if (section == 1) {
						curr_album_i = sel;
						curr_album = album_names[curr_artist_i][curr_album_i];
						curr_album_all_inclusive = 0;
						for (int i = 0; i < (int)album_names.size(); i++) {
							for (int j = 0; j < (int)album_names[i].size(); j++) {
								if (i == curr_artist_i && album_names[i][j] == curr_album) {
									goto END;
								}
								curr_album_all_inclusive++;
							}
						}
					END:;
						section++;
						tguiDeleteWidget(list);
						tguiDeleteWidget(check);
						tguiDeleteWidget(backarrow);
						album_index[curr_artist_i] = list->getTop();
						song_list->setItems(song_names[curr_album_all_inclusive]);
						song_list->setTop(song_index[curr_artist_i][curr_album_i]);
						song_list->setToggled(bools[curr_album_all_inclusive]);
						tguiAddWidget(backarrow);
						tguiAddWidget(song_list);
						tguiAddWidget(check);
						tguiSetFocus(song_list);
					}
				}
			}
			else if (w && w == backarrow) {
				if (toplevel_choice == -1) {
					goto superdone;
				}
				else if (toplevel_choice == 0) {
					toplevel_choice = -1;
					section = 0;
					all_songs_index = song_list->getTop();
					tguiDeleteWidget(song_list);
					tguiDeleteWidget(check);
					tguiDeleteWidget(backarrow);
					tguiAddWidget(backarrow);
					tguiAddWidget(list);
					tguiAddWidget(check);
					tguiSetFocus(backarrow);
					list->reset();
				}
				else if (toplevel_choice == 1) {
					if (section == 2) {
						song_index[curr_artist_i][curr_album_i] = song_list->getTop();
						tguiDeleteWidget(song_list);
						tguiDeleteWidget(check);
						tguiDeleteWidget(backarrow);
						tguiAddWidget(backarrow);
						tguiAddWidget(list);
						tguiAddWidget(check);
						tguiSetFocus(backarrow);
						section--;
						list->setTop(album_index[curr_artist_i]);
						std::vector<bool> b = song_list->getToggled();
						for (int i = 0; i < (int)b.size(); i++) {
							bools[curr_album_all_inclusive][i] = b[i];
						}
					}
					else if (section == 1) {
						album_index[curr_artist_i] = list->getTop();
						list->setItems(artist_names);
						list->setTop(artist_index);
						section--;
					}
					else {
						toplevel_choice = -1;
						artist_index = list->getTop();
						list->setItems(toplevel_choices);
						list->reset();
						section = 0;
						for (int i = 0; i < (int)bools.size(); i++) {
							for (int j = 0; j < (int)bools[i].size(); j++) {
								bools[i][j] = false;
							}
						}
					}
				}
			}

			if (close_pressed) {
				do_close();
				close_pressed = false;
			}
		}

		if (draw_counter > 0) {
			draw_counter = 0;

			al_set_target_bitmap(buffer);

//			m_clear(m_map_rgb(0, 0, 0));
			
			al_draw_tinted_bitmap(bg, al_map_rgba(64, 64, 64, 255), 0, 0, 0);

			if (section >= 1) {
				mTextout_simple(curr_artist.c_str(),
					BW/2-m_text_length(game_font,
					curr_artist.c_str())/2, 6, m_map_rgb(100, 100, 100));
			}
			if (section >= 2) {
				mTextout_simple(curr_album.c_str(),
					BW/2-m_text_length(game_font,
					curr_album.c_str())/2, 18, m_map_rgb(100, 100, 100));
			}

			tguiDraw();

			drawBufferToScreen();
			
			m_flip_display();
		}

		m_rest(0.001);
	}
done:
	picked = [NSMutableArray array];

	if (toplevel_choice == 1) {
		for (int i = 0; i < (int)song_names.size(); i++) {
			for (int j = 0; j < (int)song_names[i].size(); j++) {
				if (bools[i][j]) {
					int n = 0;
					for (std::map<std::string, Artist>::iterator it = artists.begin();
						 it != artists.end(); it++) {
						for (std::map<std::string, Album>::iterator it2 = artists[(*it).first].albums.begin(); it2 != artists[(*it).first].albums.end(); it2++) {
							std::vector<std::string> &vs = artists[(*it).first].albums[(*it2).first].songs;
							std::vector<int> &vi = artists[(*it).first].albums[(*it2).first].song_overall_num;
							for (int l = 0; l < (int)vs.size(); l++) {
								if (n == k) {
									//printf("l=%d vi[l]=%d n=%d k=%d\n", l, vi[l], n, k);
									[picked addObject:[array objectAtIndex:vi[l]]];
									count++;
								}
								n++;
							}
						}
					}
				}
				k++;
			}
		}
	}
	else if (toplevel_choice == 0) {
		for (int i = 0; i < all_toggled.size(); i++) {
			if (all_toggled[i]) {
				[picked addObject:[array objectAtIndex:i]];
				count++;
			}
		}
	}
	
superdone:

	iPodPause();

	if (count) {
		MPMediaItemCollection *coll = [[MPMediaItemCollection alloc] initWithItems:picked];

		[getMusicPlayer() setQueueWithItemCollection:coll];
		[getMusicPlayer() play];
		[coll release];
	}

	if (section < 2) {
		tguiDeleteWidget(list);
	}
	else
		tguiDeleteWidget(song_list);
	tguiDeleteWidget(check);
	tguiDeleteWidget(backarrow);
	
	delete list;
	delete check;
	delete song_list;
	delete backarrow;

	tguiPop();

	global_draw_red = true;
	
	[p drain];
	
/*
	if (the_controller == nil) {
		the_controller = [[myController2 alloc] init];
	}
	else if (the_controller.iPodIsPresented) {
		return;
	}
	[[UIApplication sharedApplication] setStatusBarHidden:NO animated:NO];
	the_controller.iPodIsPresented = true;
	[[the_delegate window] addSubview:the_controller.view];
	[the_controller performSelectorOnMainThread: @selector(runPicker)
		withObject:nil waitUntilDone:YES];
		*/
}

void shutdownIpod(void)
{
	/*if (the_controller != nil)
		[the_controller release];*/
	if (musicPlayer != nil)
		[musicPlayer release];
}

bool iPodIsPlaying(void)
{
	NSAutoreleasePool *p = [[NSAutoreleasePool alloc] init];
	return getMusicPlayer().playbackState & MPMusicPlaybackStatePlaying;
	[p drain];
}

void iPodPrevious(void)
{
	NSAutoreleasePool *p = [[NSAutoreleasePool alloc] init];
	[getMusicPlayer() skipToPreviousItem];
	[p drain];
}

void iPodNext(void)
{
	NSAutoreleasePool *p = [[NSAutoreleasePool alloc] init];
	[getMusicPlayer() skipToNextItem];
	[p drain];
}

void iPodPlay(void)
{
	NSAutoreleasePool *p = [[NSAutoreleasePool alloc] init];
	[getMusicPlayer() play];
	[p drain];
}

void iPodPause(void)
{
	NSAutoreleasePool *p = [[NSAutoreleasePool alloc] init];
	[getMusicPlayer() pause];
	[p drain];
}

void iPodStop(void)
{
	NSAutoreleasePool *p = [[NSAutoreleasePool alloc] init];
	[getMusicPlayer() stop];
	[p drain];
}

// return true on success
bool get_clipboard(char *buf, int len)
{
	NSAutoreleasePool *p = [[NSAutoreleasePool alloc] init];
	//NSData *d = [[UIPasteboard generalPasteboard] dataForPasteboardType:(NSString *)kUTTypeUTF8PlainText];
	NSString *d = [[UIPasteboard generalPasteboard] string];
	if (d == nil)
		return false;
	strcpy(buf, [d UTF8String]);
	[p drain];
	return true;
}

void set_clipboard(char *buf)
{
	NSAutoreleasePool *p = [[NSAutoreleasePool alloc] init];
	NSData *d = [NSData dataWithBytes:buf length:strlen(buf)];
	[[UIPasteboard generalPasteboard] setData:d forPasteboardType:(NSString *)kUTTypeUTF8PlainText];
	[p drain];
}

float getBatteryLevel(void)
{
	return al_iphone_get_battery_level();
}

bool isMultitaskingSupported(void)
{
	NSAutoreleasePool *p = [[NSAutoreleasePool alloc] init];
	char buf[100];
	strcpy(buf, [[[UIDevice currentDevice] systemVersion] UTF8String]);
	if (atof(buf) < 4.0) return false;
	bool b = [[UIDevice currentDevice] isMultitaskingSupported];
	[p drain];
	return b;
}

//extern bool is_muted(void);

void vibrate(void)
{
//	if (is_muted())
//		return;

	NSAutoreleasePool *p = [[NSAutoreleasePool alloc] init];
	AudioServicesPlaySystemSound (kSystemSoundID_Vibrate);
	[p drain];
}

// Game Center stuff
#ifndef LITE
int is_authenticated = NOTYET;
NSMutableDictionary *achievementsDictionary;

BOOL isGameCenterAPIAvailable()
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	
	// Check for presence of GKLocalPlayer class.
	BOOL localPlayerClassAvailable = (NSClassFromString(@"GKLocalPlayer")) != nil;
	
	// The device must be running iOS 4.1 or later.
	NSString *reqSysVer = @"4.1";
	NSString *currSysVer = [[UIDevice currentDevice] systemVersion];
	BOOL osVersionSupported = ([currSysVer compare:reqSysVer options:NSNumericSearch] != NSOrderedAscending);
	
	[pool drain];
	
	return (localPlayerClassAvailable && osVersionSupported);
}

void loadAchievements(void)
{
	if (!isGameCenterAPIAvailable() || !is_authenticated)
		return;

	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

	achievementsDictionary = [[NSMutableDictionary alloc] init];
	[GKAchievement loadAchievementsWithCompletionHandler:^(NSArray *achievements, NSError *error)
	 {
		 if (error == nil)
		 {
			 for (GKAchievement* achievement in achievements) {
				 [achievementsDictionary setObject: achievement forKey: achievement.identifier];
			 }
		 }
	 }];
	 
	 [pool drain];
}

#define NUM_ACHIEVEMENTS 30

NSString *achievements_backlog[NUM_ACHIEVEMENTS];
int num_backlog_achievements = 0;

void reportAchievementIdentifier(NSString* identifier, bool notification);

void authenticatePlayer(void)
{
	if (!isGameCenterAPIAvailable()) {
		is_authenticated = 0;
		return;
	}

	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

	GKLocalPlayer *localPlayer = [GKLocalPlayer localPlayer];

	[localPlayer authenticateWithCompletionHandler:^(NSError *error) {
		if (localPlayer.isAuthenticated)
		{
			// Perform additional tasks for the authenticated player.
			is_authenticated = 1;
			loadAchievements();
			int i;
			int n = num_backlog_achievements;
			for (i = 0; i < n; i++) {
				NSString *s = achievements_backlog[0];
				int j;
				for (j = 1; j < num_backlog_achievements; j++) {
					achievements_backlog[j-1] = achievements_backlog[j];
				}
				num_backlog_achievements--;
				reportAchievementIdentifier(s, false);
				[s release];
			}
		}
		else
			is_authenticated = 0;
	}];
	
	[pool drain];
}

bool reset_complete = false;

void resetAchievements(void)
{
	if (!isGameCenterAPIAvailable() || !is_authenticated)
		return;

	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

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
	 
	 [pool drain];
}

void reportAchievementIdentifier(NSString* identifier, bool notification)
{
	if (!isGameCenterAPIAvailable() || !is_authenticated)
		return;

	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

	if ([achievementsDictionary objectForKey:identifier] != nil) {
		[pool drain];
		return;
	}
	// check the backlog!
	int i;
	for (i = 0; i < num_backlog_achievements; i++) {
		if (NSOrderedSame == [achievements_backlog[i] compare:identifier]) {
			// already there
			[pool drain];
			return;
		}
	}
	
	float percent = 100;
	
	if (notification) {
		// FIXME: show trophy
	}

	GKAchievement *achievement = [[[GKAchievement alloc] initWithIdentifier: identifier] autorelease];
	if (achievement)
	{
		[achievementsDictionary setObject:achievement forKey:identifier];
		achievement.percentComplete = percent;
		[achievement reportAchievementWithCompletionHandler:^(NSError *error)
		 {
			 if (error != nil)
			 {
				 // Retain the achievement object and try again later (not shown).
				 if (num_backlog_achievements < NUM_ACHIEVEMENTS) {
					 achievements_backlog[num_backlog_achievements] = [[NSString alloc] initWithString:identifier];
					 num_backlog_achievements++;
				 }
			 }
		 }];
	}
	
	[pool drain];
}

struct Holder
{
	int num;
	NSString *ident;
};

void do_milestone(int num)
{
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
			achievement_time = al_get_time();
			achievement_show = true;
			return;
		}
	}
}

bool modalViewShowing = false;

void showAchievements(void)
{
	MyUIViewController *uv = [[MyUIViewController alloc] initWithNibName:nil bundle:nil];
	[uv performSelectorOnMainThread: @selector(showAchievements) withObject:nil waitUntilDone:YES];
	while (modalViewShowing) {
		al_rest(0.001);
	}
	[uv release];
	
	ALLEGRO_DISPLAY *d = airplay_connected ? controller_display : display;
	
	UIWindow *window = al_iphone_get_window(d);
	UIView *view = al_iphone_get_view(d);
	[window bringSubviewToFront:view];
}

#endif
