#ifndef NO_JOYPAD

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

#endif // NO_JOYPAD
