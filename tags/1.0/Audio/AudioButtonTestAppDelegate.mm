//
//  AudioButtonTestAppDelegate.m
//  AudioButtonTest
//
//  Created by Michelle Daniels on 11/2/08.
//  Copyright UCSD 2008. All rights reserved.
//

#import "AudioButtonTestAppDelegate.h"
#import "AudioButtonTestViewController.h"

@implementation AudioButtonTestAppDelegate

@synthesize window;
@synthesize viewController;


- (void)applicationDidFinishLaunching:(UIApplication *)application {    
    
    //AudioButtonTestViewController* tempController = [[AudioButtonTestViewController alloc] init];
    //self.viewController = tempController;
    //[tempController release];
    
    [viewController myInit];
    
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
