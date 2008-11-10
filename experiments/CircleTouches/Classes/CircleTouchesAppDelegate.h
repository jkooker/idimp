//
//  CircleTouchesAppDelegate.h
//  CircleTouches
//
//  Created by John Kooker on 11/10/08.
//  Copyright Apple Inc 2008. All rights reserved.
//

#import <UIKit/UIKit.h>

@class CircleTouchesViewController;

@interface CircleTouchesAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    CircleTouchesViewController *viewController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet CircleTouchesViewController *viewController;

@end

