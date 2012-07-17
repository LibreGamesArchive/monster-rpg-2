//
//  MyUIViewController.h
//  Ballz
//
//  Created by Trent Gamblin on 11-06-19.
//  Copyright 2011 Nooskewl. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <GameKit/GameKit.h>


@interface MyUIViewController : UIViewController <GKAchievementViewControllerDelegate> {
    
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
