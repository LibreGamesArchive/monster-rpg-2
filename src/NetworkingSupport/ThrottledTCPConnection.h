//
//  ThrottledTCPConnection.h
//  Podcopter
//
//  Created by Aaron Thompson on 3/30/10.
//  Copyright 2010 Vanderbilt University. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "TCPConnection.h"

@interface ThrottledTCPConnection : TCPConnection {
	NSLock *_sendLock;
}

- (BOOL)sendData:(NSData *)data throttledToBandwidthLimit:(float)kilobytesPerSecond;

@end
