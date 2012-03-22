#import <objc/objc.h>
#include "joypad_handler.h"
#include "joypad.hpp"
#include "monster2.hpp"

static joypad_handler *joypad = NULL;
static bool b_is_32_or_later;

extern "C" {
void lock_joypad_mutex(void);
void unlock_joypad_mutex(void);
}

void find_joypads(void)
{
	if (b_is_32_or_later && !joypad_connected())
	{
		[joypad performSelectorOnMainThread: @selector(find_devices) withObject:nil waitUntilDone:YES];
	}
}

bool is_32_or_later(void)
{
	return b_is_32_or_later;
}

void init_joypad(void)
{
#ifdef IPHONE
	NSAutoreleasePool *p = [[NSAutoreleasePool alloc] init];
	NSString *s = [[UIDevice currentDevice] systemVersion];
	float v = [s floatValue];
	[p drain];
	b_is_32_or_later = v > 3.19;
#else
	b_is_32_or_later = true;
#endif

	if (is_32_or_later()) {
		joypad = [[joypad_handler alloc] init];
		[joypad performSelectorOnMainThread: @selector(start) withObject:nil waitUntilDone:YES];
	}
}

InputDescriptor get_joypad_state(void)
{
	InputDescriptor i;

	lock_joypad_mutex();
	
	if (!b_is_32_or_later || !joypad || !joypad->connected)
	{
		memset(&i, 0, sizeof(i));
	}
	else {
		i.left = joypad->left;
		i.right = joypad->right;
		i.up = joypad->up;
		i.down = joypad->down;
		i.button1 = joypad->ba;
		i.button2 = joypad->bb;
		i.button3 = joypad->bx;
	}

	unlock_joypad_mutex();
	
	return i;
}

bool joypad_connected(void)
{
	if (!b_is_32_or_later)
		return false;

	return joypad && joypad->connected;
}

