#ifndef NO_JOYPAD

#import "joypad_handler.h"

#include "Area.hpp"
#include "Battle.hpp"
#include "Configuration.hpp"
#include "init.hpp"
extern bool center_button_pressed;

/*
extern "C" {
	void lock_joypad_mutex(void);
	void unlock_joypad_mutex(void);

	void connect_external_controls(void);
	void disconnect_external_controls(void);

	void joy_b1_down(bool skip_initial_event = false, bool long_delay = false);
	void joy_b2_down(bool skip_initial_event = false, bool long_delay = false);
	void joy_b3_down(bool skip_initial_event = false, bool long_delay = false);
	void joy_b1_up();
	void joy_b2_up();
	void joy_b3_up();
	void joy_l_down(bool skip_initial_event = false, bool long_delay = false);
	void joy_r_down(bool skip_initial_event = false, bool long_delay = false);
	void joy_u_down(bool skip_initial_event = false, bool long_delay = false);
	void joy_d_down(bool skip_initial_event = false, bool long_delay = false);
	void joy_l_up();
	void joy_r_up();
	void joy_u_up();
	void joy_d_up();
}
*/

@implementation joypad_handler

- (id)init
{
    self = [super init];
    if (self) {
    }
    
    return self;
}

-(void)start
{
	joypadManager = [JPManager sharedManager];
	[joypadManager setDelegate:self];
	[joypadManager setMaxPlayerCount:1];
	JPControllerLayout *l = [JPControllerLayout snesLayout];
	[l setName:@"Monster RPG 2"];
        [joypadManager setControllerLayout:l];

	connected = left = right = up = down = ba = bb = bx = by = bl = br = false;
}

-(void)find_devices
{
	[[JPManager sharedManager] setGameState:kJPGameStateMenu];
}

-(void)stop_finding_devices
{
	[[JPManager sharedManager] setGameState:kJPGameStateGameplay];
}

-(void)joypadManager:(JPManager *)manager didFindDevice:(JPDevice *)device previouslyConnected:(BOOL)prev
{
	[self stop_finding_devices];
	[device setDelegate:self];
}

-(void)joypadManager:(JPManager *)manager didLoseDevice:(JPDevice *)device;
{
}

-(void)joypadManager:(JPManager *)manager deviceDidConnect:(JPDevice *)device
{
	[device setDelegate:self];  // Use self to have the same delegate object as the joypad manager.
	connected = true;
	connect_external_controls();
}

-(BOOL)joypadManager:(JPManager *)manager deviceShouldConnect:(JPDevice *)device
{
  return YES;
}


-(void)joypadManager:(JPManager *)manager deviceDidDisconnect:(JPDevice *)device
{
	connected = false;
	left = right = up = down = ba = bb = bx = by = bl = br = false;
	disconnect_external_controls();
}

-(void)joypadDevice:(JPDevice *)device didAccelerate:(JPAcceleration)accel
{
}

-(void)joypadDevice:(JPDevice *)device dPad:(JPInputIdentifier)dpad buttonUp:(JPDpadButton)dpadButton
{
	lock_joypad_mutex();
	
	if (dpadButton == kJPDpadButtonUp)
	{
		up = false;
		joy_u_up();
	}
	else if (dpadButton == kJPDpadButtonDown)
	{
		down = false;
		joy_d_up();
	}
	else if (dpadButton == kJPDpadButtonLeft)
	{
		left = false;
		joy_l_up();
	}
	else if (dpadButton == kJPDpadButtonRight)
	{
		right = false;
		joy_r_up();
	}

	unlock_joypad_mutex();
}

-(void)joypadDevice:(JPDevice *)device dPad:(JPInputIdentifier)dpad buttonDown:(JPDpadButton)dpadButton
{
	lock_joypad_mutex();
	
	if (dpadButton == kJPDpadButtonUp)
	{
		up = true;
		joy_u_down();
	}
	else if (dpadButton == kJPDpadButtonDown)
	{
		down = true;
		joy_d_down();
	}
	else if (dpadButton == kJPDpadButtonLeft)
	{
		left = true;
		joy_l_down();
	}
	else if (dpadButton == kJPDpadButtonRight)
	{
		right = true;
		joy_r_down();
	}
	
	unlock_joypad_mutex();
}

-(void)joypadDevice:(JPDevice *)device buttonUp:(JPInputIdentifier)button
{
	lock_joypad_mutex();
	
	if (button == kJPInputAButton)
	{
		ba = false;
		if (area && !battle && !in_pause && config.getAlwaysCenter() == PAN_HYBRID) {
			area_panned_x = floor(area_panned_x);
			area_panned_y = floor(area_panned_y);
			area->center_view = true;
			center_button_pressed = true;
		}
		joy_b1_up();
	}
	else if (button == kJPInputBButton)
	{
		bb = false;
		joy_b2_up();
	}
	else if (button == kJPInputXButton)
	{
		bx = false;
		joy_b3_up();
	}
	else if (button == kJPInputYButton)
	{
		by = false;
	}
	else if (button == kJPInputLButton)
	{
		bl = false;
	}
	else if (button == kJPInputRButton)
	{
		br = false;
	}

	unlock_joypad_mutex();
}

-(void)joypadDevice:(JPDevice *)device buttonDown:(JPInputIdentifier)button
{
	lock_joypad_mutex();
	
	if (button == kJPInputAButton)
	{
		ba = true;
		joy_b1_down();
	}
	else if (button == kJPInputBButton)
	{
		bb = true;
		joy_b2_down();
	}
	else if (button == kJPInputXButton)
	{
		bx = true;
		joy_b3_down();
	}
	else if (button == kJPInputYButton)
	{
		by = true;
	}
	else if (button == kJPInputLButton)
	{
		bl = true;
	}
	else if (button == kJPInputRButton)
	{
		br = true;
	}

	unlock_joypad_mutex();
}

-(void)joypadDevice:(JPDevice *)device analogStick:(JPInputIdentifier)stick didMove:(JPStickPosition)newPosition
{
}

@end

#endif // NO_JOYPAD
