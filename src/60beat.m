#include <SBJoystick.h>

#include "60beat.h"

void joy_b1_down(void);
void joy_b2_down(void);
void joy_b3_down(void);
void joy_b1_up(void);
void joy_b2_up(void);
void joy_b3_up(void);
void joy_l_down(void);
void joy_r_down(void);
void joy_u_down(void);
void joy_d_down(void);
void joy_l_up(void);
void joy_r_up(void);
void joy_u_up(void);
void joy_d_up(void);

static bool sb_l, sb_r, sb_u, sb_d, sb_1, sb_2, sb_3, sb_4, sb_l1, sb_r1, sb_on;

@interface SB_delegate : UIViewController<SBJoystickDelegate> {
}

-(void) joystickStatusChanged:(SBJoystick *)joystick;
-(void) controlUpdated:(SBJoystick *)joystick;
@end

@implementation SB_delegate
-(void) joystickStatusChanged:(SBJoystick *)joystick
{
	sb_on = joystick.joystickConnected;
	if (sb_on) {
		connect_external_controls();
	}
	else {
		disconnect_external_controls();
	}
	if (!sb_on) {
		sb_l = sb_r = sb_u = sb_d = sb_1 = sb_2 = sb_3 = sb_4 = sb_l1 = sb_r1 = false;
	}
}

-(void) controlUpdated:(SBJoystick *)joystick
{
	bool _l = sb_l;
	bool _r = sb_r;
	bool _u = sb_u;
	bool _d = sb_d;
	bool _1 = sb_1;
	bool _2 = sb_2;
	bool _3 = sb_3;

	sb_l = joystick.buttonLState;
	sb_r = joystick.buttonRState;
	sb_u = joystick.buttonUState;
	sb_d = joystick.buttonDState;
	sb_1 = joystick.button1State;
	sb_2 = joystick.button2State;
	sb_3 = joystick.button3State;
	sb_4 = joystick.button4State;
	sb_l1 = joystick.buttonL1State;
	sb_r1 = joystick.buttonR1State;

	if (_l && !sb_l) { joy_l_up(); } else if (!_l && sb_l) { joy_l_down(); }
	if (_r && !sb_r) { joy_r_up(); } else if (!_r && sb_r) { joy_r_down(); }
	if (_u && !sb_u) { joy_u_up(); } else if (!_u && sb_u) { joy_u_down(); }
	if (_d && !sb_d) { joy_d_up(); } else if (!_d && sb_d) { joy_d_down(); }
	if (_1 && !sb_1) { joy_b1_up(); } else if (!_1 && sb_1) { joy_b1_down(); }
	if (_2 && !sb_2) { joy_b2_up(); } else if (!_2 && sb_2) { joy_b2_down(); }
	if (_3 && !sb_3) { joy_b3_up(); } else if (!_3 && sb_3) { joy_b3_down(); }
}
@end

static SB_delegate *sb_delegate;

void sb_start(void)
{
	[SBJoystick sharedInstance].enabled = YES;
	sb_delegate = [[SB_delegate alloc] init];
	[SBJoystick sharedInstance].delegate = sb_delegate;
}

void sb_stop(void)
{
	sb_on = false;
	[SBJoystick sharedInstance].enabled = NO;
	[SBJoystick sharedInstance].delegate = nil;
	[sb_delegate release];
}

bool is_sb_connected(void)
{
	return sb_on;
}

void get_sb_state(bool *l, bool *r, bool *u, bool *d, bool *b1, bool *b2, bool *b3)
{
	if (sb_on == false) {
		*u = *d = *l = *r = *b1 = *b2 = *b3 = false;
		return;
	}
	
	*u = sb_u;
	*d = sb_d;
	*l = sb_l;
	*r = sb_r;
	*b1 = sb_1;
	*b2 = sb_2;
	*b3 = sb_3;
}

