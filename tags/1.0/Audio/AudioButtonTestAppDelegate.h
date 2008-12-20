//
//  AudioButtonTestAppDelegate.h
//  AudioButtonTest
//
//  Created by Michelle Daniels on 11/2/08.
//  Copyright UCSD 2008. All rights reserved.
//

#import <UIKit/UIKit.h>

@class AudioButtonTestViewController;

@interface AudioButtonTestAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    AudioButtonTestViewController *viewController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet AudioButtonTestViewController *viewController;

@end

