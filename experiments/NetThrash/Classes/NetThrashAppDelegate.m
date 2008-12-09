//
//  NetThrashAppDelegate.m
//  NetThrash
//
//  Created by John Kooker on 11/25/08.
//  Copyright Apple Inc 2008. All rights reserved.
//

#import "NetThrashAppDelegate.h"
#import "NetThrashViewController.h"

@implementation NetThrashAppDelegate

@synthesize window;
@synthesize viewController;
@synthesize socket;


- (void)applicationDidFinishLaunching:(UIApplication *)application {    
    
    // Override point for customization after app launch
    self.socket = [[AsyncUdpSocket alloc] initWithDelegate:self];
    NSError *error = nil;
    if (![self.socket bindToPort:23711 error:&error])
    {
        NSLog(@"could not bindToPort. %@", error);
    }
    NSLog(@"created socket. host: %@, port: %d", [self.socket localHost], [self.socket localPort]);
    [self.socket receiveWithTimeout:-1 tag:0];
    
    [window addSubview:viewController.view];
    [window makeKeyAndVisible];
}


- (void)dealloc {
    [viewController release];
    [window release];
    [super dealloc];
}

#pragma mark AsyncUdpSocket Delegate Methods

- (void)onUdpSocket:(AsyncUdpSocket *)sock didSendDataWithTag:(long)tag
{
	NSLog(@"%s", _cmd);
}

- (void)onUdpSocket:(AsyncUdpSocket *)sock didNotSendDataWithTag:(long)tag dueToError:(NSError *)error
{
	NSLog(@"%s", _cmd);
}

- (BOOL)onUdpSocket:(AsyncUdpSocket *)sock didReceiveData:(NSData *)data withTag:(long)tag fromHost:(NSString *)host port:(UInt16)port
{
	NSLog(@"%s", _cmd);
    
    // assumption: incoming data is a null-terminated string
    NSLog(@"received data: %s", [data bytes]);
    
    [sock receiveWithTimeout:-1 tag:0];
    
    return YES;
}

- (void)onUdpSocket:(AsyncUdpSocket *)sock didNotReceiveDataWithTag:(long)tag dueToError:(NSError *)error
{
	NSLog(@"%s", _cmd);
}

- (void)onUdpSocketDidClose:(AsyncUdpSocket *)sock
{
	NSLog(@"%s", _cmd);
}

@end
