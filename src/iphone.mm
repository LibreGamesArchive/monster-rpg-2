#import <Foundation/Foundation.h>
#import <MobileCoreServices/MobileCoreServices.h>
#import <UIKit/UIKit.h>
#import <MediaPlayer/MediaPlayer.h>
#import <AudioToolbox/AudioToolbox.h>
#import <AVFoundation/AVAudioSession.h>
#import "mygamecentervc.h"

#include <allegro5/allegro.h>
#include <allegro5/allegro_iphone.h>
#include <allegro5/allegro_iphone_objc.h>
#include <allegro5/internal/aintern_keyboard.h>


#define NO_BASS
#include "monster2.hpp"

extern bool center_button_pressed;

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

void disableMic(void)
{
	[[AVAudioSession sharedInstance] setCategory:AVAudioSessionCategoryPlayback error:NULL];
}

double my_last_shake_time = 0.0;

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

ALLEGRO_KEYBOARD_STATE icade_keyboard_state;

@interface KBDelegate : NSObject<UITextViewDelegate>
- (void)start;
- (void)switch_in;
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

- (void)switch_in
{
	[text_view removeFromSuperview];
	UIWindow *window = al_iphone_get_window(display);
	[window addSubview:text_view];
	[text_view becomeFirstResponder];
}

- (void)textViewDidChange:(UITextView *)textView
{
	while ([textView.text length] > 0) {
		NSString *first = [textView.text substringToIndex:1];
		NSString *remain = [textView.text substringFromIndex:1];
		textView.text = remain;
		const char *txt = [first UTF8String];
		ALLEGRO_EVENT *e = (ALLEGRO_EVENT *)malloc(sizeof(ALLEGRO_EVENT));
		ALLEGRO_EVENT *e2 = NULL;
		bool emit = false;
		if (gen_event(e, toupper(txt[0]))) {
			TripleInput *i = getInput();
			if (i) {
				i->handle_event(e);
			}
			if (e->type == USER_KEY_DOWN) {
				e2 = (ALLEGRO_EVENT *)malloc(sizeof(ALLEGRO_EVENT));
				e2->user.type = USER_KEY_CHAR;
				e2->keyboard.keycode = e->keyboard.keycode;
				_AL_KEYBOARD_STATE_SET_KEY_DOWN(icade_keyboard_state, e->keyboard.keycode);

				if (e->keyboard.keycode == config.getKey1()) {
					joy_b1_down();
				}
				else if (e->keyboard.keycode == config.getKey2()) {
					joy_b2_down();
				}
				else if (e->keyboard.keycode == config.getKey3()) {
					joy_b3_down();
				}
				else if (e->keyboard.keycode == config.getKeyLeft()) {
					joy_l_down();
				}
				else if (e->keyboard.keycode == config.getKeyRight()) {
					joy_r_down();
				}
				else if (e->keyboard.keycode == config.getKeyUp()) {
					joy_u_down();
				}
				else if (e->keyboard.keycode == config.getKeyDown()) {
					joy_d_down();
				}
				else {
					emit = true;
				}
			}
			else {
				_AL_KEYBOARD_STATE_CLEAR_KEY_DOWN(icade_keyboard_state, e->keyboard.keycode);

				if (e->keyboard.keycode == config.getKey1()) {
					if (area && !battle && !in_pause && config.getAlwaysCenter() == PAN_HYBRID) {
						area_panned_x = floor(area_panned_x);
						area_panned_y = floor(area_panned_y);
						area->center_view = true;
						center_button_pressed = true;
					}
					joy_b1_up();
				}
				else if (e->keyboard.keycode == config.getKey2()) {
					joy_b2_up();
				}
				else if (e->keyboard.keycode == config.getKey3()) {
					joy_b3_up();
				}
				else if (e->keyboard.keycode == config.getKeyLeft()) {
					joy_l_up();
				}
				else if (e->keyboard.keycode == config.getKeyRight()) {
					joy_r_up();
				}
				else if (e->keyboard.keycode == config.getKeyUp()) {
					joy_u_up();
				}
				else if (e->keyboard.keycode == config.getKeyDown()) {
					joy_d_up();
				}
				else {
					emit = true;
				}
			}
			if (emit) {
				al_emit_user_event(&user_event_source, e, destroy_event);
				if (e2) {
					al_emit_user_event(&user_event_source, e2, destroy_event);
				}
			}
		}
		if (!emit) {
			free(e);
			free(e2);
		}
	}
}
@end

static KBDelegate *text_delegate;

void initiOSKeyboard()
{
	text_delegate = [[KBDelegate alloc] init];
	[text_delegate performSelectorOnMainThread: @selector(start) withObject:nil waitUntilDone:YES];
	memset(&icade_keyboard_state, 0, sizeof icade_keyboard_state);
}

void switchiOSKeyboardIn()
{
	[text_delegate performSelectorOnMainThread: @selector(switch_in) withObject:nil waitUntilDone:YES];
}
