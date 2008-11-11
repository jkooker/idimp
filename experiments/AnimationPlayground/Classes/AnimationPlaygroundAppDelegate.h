//
//  AnimationPlaygroundAppDelegate.h
//  AnimationPlayground
//
//  Created by John Kooker on 11/11/08.
//  Copyright Apple Inc 2008. All rights reserved.
//

#import <UIKit/UIKit.h>

@class AnimationPlaygroundViewController;

@interface AnimationPlaygroundAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    AnimationPlaygroundViewController *viewController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet AnimationPlaygroundViewController *viewController;

@end

