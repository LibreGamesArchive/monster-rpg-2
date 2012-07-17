//
//  joypad_handler.m
//  Ballz
//
//  Created by Trent Gamblin on 11-09-28.
//  Copyright 2011 Nooskewl. All rights reserved.
//

#import "joypad_handler.h"
#import "JoypadSDK.h"

void lock_joypad_mutex(void);
void unlock_joypad_mutex(void);

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

void connect_external_controls(void);
void disconnect_external_controls(void);

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
	joypadManager = [[JoypadManager alloc] init];
	[joypadManager setDelegate:self];
	[joypadManager setMaxPlayerCount:1];
	JoypadControllerLayout *l = [JoypadControllerLayout snesLayout];
	[l setName:@"Monster RPG 2"];
        [joypadManager setControllerLayout:l];

	connected = left = right = up = down = ba = bb = bx = by = bl = br = false;
}

-(void)find_devices
{
	[joypadManager startFindingDevices];
}

-(void)joypadManager:(JoypadManager *)manager didFindDevice:(JoypadDevice *)device previouslyConnected:(BOOL)prev
{
	[manager stopFindingDevices];
	[device setDelegate:self];
}

-(void)joypadManager:(JoypadManager *)manager didLoseDevice:(JoypadDevice *)device;
{
}

-(void)joypadManager:(JoypadManager *)manager deviceDidConnect:(JoypadDevice *)device player:(unsigned int)player
{
	[device setDelegate:self];  // Use self to have the same delegate object as the joypad manager.
	connected = true;
	connect_external_controls();
}

-(BOOL)joypadManager:(JoypadManager *)manager deviceShouldConnect:(JoypadDevice *)device
{
  return YES;
}


-(void)joypadManager:(JoypadManager *)manager deviceDidDisconnect:(JoypadDevice *)device player:(unsigned int)player
{
	connected = false;
	left = right = up = down = ba = bb = bx = by = bl = br = false;
	disconnect_external_controls();
}

-(void)joypadDevice:(JoypadDevice *)device didAccelerate:(JoypadAcceleration)accel
{
}

-(void)joypadDevice:(JoypadDevice *)device dPad:(JoyInputIdentifier)dpad buttonUp:(JoyDpadButton)dpadButton
{
	lock_joypad_mutex();
	
	if (dpadButton == kJoyDpadButtonUp)
	{
		up = false;
		joy_u_up();
	}
	else if (dpadButton == kJoyDpadButtonDown)
	{
		down = false;
		joy_d_up();
	}
	else if (dpadButton == kJoyDpadButtonLeft)
	{
		left = false;
		joy_l_up();
	}
	else if (dpadButton == kJoyDpadButtonRight)
	{
		right = false;
		joy_r_up();
	}

	unlock_joypad_mutex();
}

-(void)joypadDevice:(JoypadDevice *)device dPad:(JoyInputIdentifier)dpad buttonDown:(JoyDpadButton)dpadButton
{
	lock_joypad_mutex();
	
	if (dpadButton == kJoyDpadButtonUp)
	{
		up = true;
		joy_u_down();
	}
	else if (dpadButton == kJoyDpadButtonDown)
	{
		down = true;
		joy_d_down();
	}
	else if (dpadButton == kJoyDpadButtonLeft)
	{
		left = true;
		joy_l_down();
	}
	else if (dpadButton == kJoyDpadButtonRight)
	{
		right = true;
		joy_r_down();
	}
	
	unlock_joypad_mutex();
}

-(void)joypadDevice:(JoypadDevice *)device buttonUp:(JoyInputIdentifier)button
{
	lock_joypad_mutex();
	
	if (button == kJoyInputAButton)
	{
		ba = false;
		joy_b1_up();
	}
	else if (button == kJoyInputBButton)
	{
		bb = false;
		joy_b2_up();
	}
	else if (button == kJoyInputXButton)
	{
		bx = false;
		joy_b3_up();
	}
	else if (button == kJoyInputYButton)
	{
		by = false;
	}
	else if (button == kJoyInputLButton)
	{
		bl = false;
	}
	else if (button == kJoyInputRButton)
	{
		br = false;
	}

	unlock_joypad_mutex();
}

-(void)joypadDevice:(JoypadDevice *)device buttonDown:(JoyInputIdentifier)button
{
	lock_joypad_mutex();
	
	if (button == kJoyInputAButton)
	{
		ba = true;
		joy_b1_down();
	}
	else if (button == kJoyInputBButton)
	{
		bb = true;
		joy_b2_down();
	}
	else if (button == kJoyInputXButton)
	{
		bx = true;
		joy_b3_down();
	}
	else if (button == kJoyInputYButton)
	{
		by = true;
	}
	else if (button == kJoyInputLButton)
	{
		bl = true;
	}
	else if (button == kJoyInputRButton)
	{
		br = true;
	}

	unlock_joypad_mutex();
}

-(void)joypadDevice:(JoypadDevice *)device analogStick:(JoyInputIdentifier)stick didMove:(JoypadStickPosition)newPosition
{
}

@end
