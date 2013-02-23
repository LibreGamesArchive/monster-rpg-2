#include "monster2.hpp"
#define ASSERT ALLEGRO_ASSERT
#include <allegro5/platform/aintiphone.h>
#include <allegro5/allegro_opengl.h>
#include <zlib.h>
#include "iphone.h"
#import <Foundation/Foundation.h>
#import <MobileCoreServices/MobileCoreServices.h>
#import <UIKit/UIKit.h>
#import <MediaPlayer/MediaPlayer.h>
#import <AudioToolbox/AudioToolbox.h>
#import <AVFoundation/AVAudioSession.h>
#import "mygamecentervc.h"
#include <allegro5/allegro_iphone.h>

#import "NetReachability.h"

extern "C" {
#include <allegro5/allegro_iphone_objc.h>
}

static MPMusicPlayerController *musicPlayer;

void openRatingSite(void)
{
	const char *url_ascii = "http://www.monster-rpg.com";
	NSString *u = [[NSString alloc] initWithCString:url_ascii encoding:NSUTF8StringEncoding];
	CFURLRef url = CFURLCreateWithString(NULL, (CFStringRef)u, NULL);
	[[UIApplication sharedApplication] openURL:(NSURL *)url];
	[u release];
	CFRelease(url);
	exit(0);
}

void openFeedbackSite(void)
{
	const char *url_ascii = "http://www.monster-rpg.com/feedback.html";
	NSString *u = [[NSString alloc] initWithCString:url_ascii encoding:NSUTF8StringEncoding];
	CFURLRef url = CFURLCreateWithString(NULL, (CFStringRef)u, NULL);
	[[UIApplication sharedApplication] openURL:(NSURL *)url];
	[u release];
	CFRelease(url);
	exit(0);
}


MPMusicPlayerController *getMusicPlayer(void)
{
	if (musicPlayer == nil) {
 		musicPlayer = [MPMusicPlayerController iPodMusicPlayer];
	}
	return musicPlayer;
}

struct Album {
	std::vector<std::string> songs;
	std::vector<int> song_overall_num;
};

struct Artist {
	std::map<std::string, Album> albums;
};

void showIpod(void)
{
	global_draw_red = false;

	MBITMAP *bg = m_load_bitmap(getResource("media/options_bg.png"));
	al_set_target_backbuffer(display);
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
		char *album_a;
		if (album_c)
		   album_a = strdup(album_c);
		else
		   album_a = strdup("???");
		NSString *artist = [m valueForProperty:MPMediaItemPropertyArtist];
		const char *artist_c = [artist UTF8String];
		char *artist_a;
		if (artist_c)
		   artist_a = strdup(artist_c);
		else
		   artist_a = strdup("???");
		NSString *title = [m valueForProperty:MPMediaItemPropertyTitle];
		const char *title_c = [title UTF8String];
		char *title_a;
		if (title_c)
		   title_a = strdup(title_c);
		else
		   title_a = strdup("???");

		all_names.push_back(std::string(title_a));
		all_toggled.push_back(false);

		if (!artists.count(std::string(artist_a))) {
			Artist a;
			artists[std::string(artist_a)] = a;
		}
		if (!artists[std::string(artist_a)].albums.count(std::string(album_a))) {
			Album a;
			artists[std::string(artist_a)].albums[std::string(album_a)] = a;
		}
		artists[std::string(artist_a)].albums[std::string(album_a)].songs.push_back(std::string(track) + " " + std::string(title_a));
		artists[std::string(artist_a)].albums[std::string(album_a)].song_overall_num.push_back(i);
		free(album_a);
		free(artist_a);
		free(title_a);
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
			// WARNING
			if (break_main_loop) {
				goto done;
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

			/* Really? Here too?
			if (close_pressed) {
				do_close();
				close_pressed = false;
			}
			*/
		}

		if (draw_counter > 0) {
			draw_counter = 0;

			al_set_target_backbuffer(display);

			al_draw_tinted_bitmap(bg->bitmap, al_map_rgba(64, 64, 64, 255), 0, 0, 0);

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
}

void shutdownIpod(void)
{
	if (musicPlayer != nil)
		[musicPlayer release];
}

bool iPodIsPlaying(void)
{
	return getMusicPlayer().playbackState & MPMusicPlaybackStatePlaying;
}

void iPodPrevious(void)
{
	[getMusicPlayer() skipToPreviousItem];
}

void iPodNext(void)
{
	[getMusicPlayer() skipToNextItem];
}

void iPodPlay(void)
{
	[getMusicPlayer() play];
}

void iPodPause(void)
{
	[getMusicPlayer() pause];
}

void iPodStop(void)
{
	[getMusicPlayer() stop];
}

// return true on success
bool get_clipboard(char *buf, int len)
{
	NSString *d = [[UIPasteboard generalPasteboard] string];
	if (d == nil)
		return false;
	strcpy(buf, [d UTF8String]);
	return true;
}

void set_clipboard(char *buf)
{
	NSData *d = [NSData dataWithBytes:buf length:strlen(buf)];
	[[UIPasteboard generalPasteboard] setData:d forPasteboardType:(NSString *)kUTTypeUTF8PlainText];
}

float getBatteryLevel(void)
{
	return al_iphone_get_battery_level();
}

bool isMultitaskingSupported(void)
{
	char buf[100];
	strcpy(buf, [[[UIDevice currentDevice] systemVersion] UTF8String]);
	if (atof(buf) < 4.0) return false;
	bool b = [[UIDevice currentDevice] isMultitaskingSupported];
	return b;
}

void vibrate(void)
{
	AudioServicesPlaySystemSound (kSystemSoundID_Vibrate);
}

bool wifiConnected(void)
{
	const bool wifiOnly = true;

	//Make sure we have a WiFi network up & running
	NetReachability * _reachability = [[NetReachability alloc] initWithDefaultRoute:NO];

	bool result = [_reachability isReachable];

	if (wifiOnly)
		result = result && ![_reachability isUsingCell];

	[_reachability release];

	return result;
}

void disableMic(void)
{
	[[AVAudioSession sharedInstance] setCategory:AVAudioSessionCategoryPlayback error:NULL];
}

static UITextView *text_view;
ALLEGRO_EVENT_SOURCE user_event_source;

static void destroy_event(ALLEGRO_USER_EVENT *u)
{
}

const char *downs = "WDXAYUIOHJKL";
const char *ups   = "ECZQTFMGRNPV";

static int event_type(char c, int *index)
{
	int i;

	for (i = 0; downs[i]; i++) {
		if (c == downs[i]) {
			*index = i;
			return USER_KEY_DOWN;
		}
	}

	for (i = 0; ups[i]; i++) {
		if (c == ups[i]) {
			*index = i;
			return USER_KEY_UP;
		}
	}

	return -1;
}

static bool gen_event(ALLEGRO_EVENT *e, char c)
{
	int index;
	int type = event_type(c, &index);
	if (type < 0) {
		return false;
	}

	c = (type == USER_KEY_DOWN) ? c : downs[index];
	c = (c-'A') + ALLEGRO_KEY_A;

	e->user.type = type;
	e->keyboard.keycode = c;

	return true;
}

@interface KBDelegate : NSObject<UITextViewDelegate>
- (void)start;
- (void)textViewDidChange:(UITextView *)textView;
@end
@implementation KBDelegate
- (void)start
{
	UIWindow *window = al_iphone_get_window(display);

	CGRect r = CGRectMake(0, 0, 0, 0);
	text_view = [[UITextView alloc] initWithFrame:r];
	text_view.delegate = self;
	text_view.hidden = YES;
	
	CGRect r2 = CGRectMake(0, 0, 0, 0);
	UIView *blank = [[UIView alloc] initWithFrame:r2];
	blank.hidden = YES;
	
	text_view.inputView = blank;

	[window addSubview:text_view];
	[text_view becomeFirstResponder];
}
- (void)textViewDidChange:(UITextView *)textView
{
printf("TVDC!!!!!!!!!\n");
	while ([textView.text length] > 0) {
		NSString *first = [textView.text substringToIndex:1];
		NSString *remain = [textView.text substringFromIndex:1];
		textView.text = remain;
		const char *txt = [first UTF8String];
		ALLEGRO_EVENT *e = (ALLEGRO_EVENT *)malloc(sizeof(ALLEGRO_EVENT));
		ALLEGRO_EVENT *e2 = NULL;
		if (gen_event(e, toupper(txt[0]))) {
			if (e->type == USER_KEY_DOWN) {
				e2 = (ALLEGRO_EVENT *)malloc(sizeof(ALLEGRO_EVENT));
				e2->user.type = USER_KEY_CHAR;
				e2->keyboard.keycode = e->keyboard.keycode;
			}
			al_emit_user_event(&user_event_source, e, destroy_event);
			if (e2) {
				al_emit_user_event(&user_event_source, e2, destroy_event);
			}
		}
		else {
			delete e;
		}
	}
}
@end

static KBDelegate *text_delegate;

void initiOSKeyboard()
{
	text_delegate = [[KBDelegate alloc] init];
	[text_delegate performSelectorOnMainThread: @selector(start) withObject:nil waitUntilDone:YES];
}

