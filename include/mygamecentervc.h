//
//  MyGameCenterVC.h
//  Ballz
//
//  Created by Trent Gamblin on 11-06-19.
//  Copyright 2011 Nooskewl. All rights reserved.
//

#import <GameKit/GameKit.h>

#ifdef ALLEGRO_IPHONE
#import <UIKit/UIKit.h>

@interface MyGameCenterVC : UIViewController <GKAchievementViewControllerDelegate> {
    
}

- (void) showAchievements;

@end

UIAlertView *alert;     

@interface MyUIAlertViewDelegate : NSObject <UIActionSheetDelegate, UIAlertViewDelegate>   
{  
	int result;  
}  

-(int)getResult;  

@end;

int CreateModalDialog(NSString *title,  
                      NSString *msg,   
                      NSString *ok,   
                      NSString *cancel);
#else

@interface MyGameCenterVC : NSObject <GKAchievementViewControllerDelegate> {
}
- (void) showAchievements;
- (void)achievementViewControllerDidFinish:(GKAchievementViewController *)viewController;
@end

#endif

