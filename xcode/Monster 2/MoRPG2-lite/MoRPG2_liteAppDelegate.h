//
//  MoRPG2_liteAppDelegate.h
//  MoRPG2-lite
//
//  Created by Trent Gamblin on 11-04-23.
//  Copyright 2011 Nooskewl. All rights reserved.
//

#import <UIKit/UIKit.h>

@class MoRPG2_liteViewController;

@interface MoRPG2_liteAppDelegate : NSObject <UIApplicationDelegate> {

}

@property (nonatomic, retain) IBOutlet UIWindow *window;

@property (nonatomic, retain) IBOutlet MoRPG2_liteViewController *viewController;

@end
