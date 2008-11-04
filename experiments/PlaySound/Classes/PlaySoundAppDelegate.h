//
//  PlaySoundAppDelegate.h
//  PlaySound
//
//  Created by John Kooker on 11/4/08.
//  Copyright Apple Inc 2008. All rights reserved.
//

#import <UIKit/UIKit.h>

@class PlaySoundViewController;

@interface PlaySoundAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    PlaySoundViewController *viewController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet PlaySoundViewController *viewController;

@end

