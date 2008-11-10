//
//  CircleTouchesAppDelegate.m
//  CircleTouches
//
//  Created by John Kooker on 11/10/08.
//  Copyright Apple Inc 2008. All rights reserved.
//

#import "CircleTouchesAppDelegate.h"
#import "CircleTouchesViewController.h"

@implementation CircleTouchesAppDelegate

@synthesize window;
@synthesize viewController;


- (void)applicationDidFinishLaunching:(UIApplication *)application {    
    
    // Override point for customization after app launch    
    [window addSubview:viewController.view];
    [window makeKeyAndVisible];
}


- (void)dealloc {
    [viewController release];
    [window release];
    [super dealloc];
}


@end
