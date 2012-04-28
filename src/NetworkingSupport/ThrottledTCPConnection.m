//
//  ThrottledTCPConnection.m
//  Podcopter
//
//  Created by Aaron Thompson on 3/30/10.
//  Copyright 2010 Vanderbilt University. All rights reserved.
//

#import "ThrottledTCPConnection.h"

@interface TCPConnection(ProtectedMethods)

- (BOOL)_writeData:(NSData *)data;

@end


@implementation ThrottledTCPConnection

- (BOOL)sendData:(NSData *)data throttledToBandwidthLimit:(float)kilobytesPerSecond
{
	if(![self isValid] || !data)
		return NO;
	
	// Need to rework this calculation
//	float targetTimeToSend = [data length] * 1024.0f / kilobytesPerSecond;
	float packetSpacing = 1;
//	int numPackets = 5;
	NSUInteger packetSize = ceilf(packetSpacing * 1024.0f * kilobytesPerSecond);
//	float packetWidth = 5000;
	float timeInterval = packetSpacing;

	const UInt8 *dataBytes = [data bytes];
	int offset = 0;
	
	while (offset < [data length])
	{
        if ([data length] - packetSize - offset < 0) {
            packetSize = [data length] - offset;
        }
        
		NSData *partialData = [NSData dataWithBytesNoCopy:(void *)&dataBytes[offset] length:packetSize freeWhenDone: NO];
		if(![super _writeData:partialData]) {
			[super invalidate];
			return NO;
		}
		
		offset += packetSize;
		
		[NSThread sleepForTimeInterval:timeInterval];
	}
	
	return YES;
}

- (NSData *)receiveData
{
	NSData *data;
	
	if (![self isValid])
		return nil;
	
	data = [self _readData];
	if(data == nil)
		[self invalidate];
	else if(data == (id)kCFNull)
		data = nil;
	
	return data;
}

- (NSLock *)sendLock {
	if (_sendLock == nil) {
		_sendLock = [[NSLock alloc] init];
	}
	
	return _sendLock;
}

@end
