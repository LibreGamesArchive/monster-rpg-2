//
//  joypad_handler.h
//  Ballz
//
//  Created by Trent Gamblin on 11-09-28.
//  Copyright 2011 Nooskewl. All rights reserved.
//

#import "JPSDK.h"

@interface joypad_handler : NSObject<JPManagerDelegate, JPDeviceDelegate>
{
	JPManager *joypadManager;

@public
	bool connected, left, right, up, down, ba, bb, bx, by, bl, br;
}

-(void)start;
-(void)find_devices;
-(void)stop_finding_devices;

@end
