//
//  NetThrashAppDelegate.h
//  NetThrash
//
//  Created by John Kooker on 11/25/08.
//  Copyright Apple Inc 2008. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AsyncUdpSocket.h"

@class NetThrashViewController;

@interface NetThrashAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    NetThrashViewController *viewController;
    
    AsyncUdpSocket *socket;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet NetThrashViewController *viewController;
@property (nonatomic, retain) AsyncUdpSocket *socket;

@end

