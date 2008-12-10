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

#define MAX_SENT_DATAGRAMS 200

- (void)onUdpSocket:(AsyncUdpSocket *)sock didSendDataWithTag:(long)tag
{
	NSLog(@"%s", _cmd);
    
    sentDatagramCount++;
    
    // if tag is "start" then reset our count
    if (tag == DMPDataPacketTagStart) sentDatagramCount = 1;
    
    // keep sending until we hit our count, then send an end packet
    if (sentDatagramCount < MAX_SENT_DATAGRAMS)
    {
        DMPDataPacket p;
        memset(&p, 0, sizeof(p));
        
        p.tag = (sentDatagramCount < MAX_SENT_DATAGRAMS - 1) ? DMPDataPacketTagNone : DMPDataPacketTagEnd;
        p.index = sentDatagramCount;
        
        [self.socket sendData:[NSData dataWithBytes:&p length:sizeof(p)]
            toAddress:viewController.savedAddress
            withTimeout:5
            tag:p.tag];
    }
}

- (void)onUdpSocket:(AsyncUdpSocket *)sock didNotSendDataWithTag:(long)tag dueToError:(NSError *)error
{
	NSLog(@"%s %@", _cmd, error);
}

- (BOOL)onUdpSocket:(AsyncUdpSocket *)sock didReceiveData:(NSData *)data withTag:(long)tag fromHost:(NSString *)host port:(UInt16)port
{
	//NSLog(@"%s", _cmd);
    
    receivedDatagramCount++;
    const DMPDataPacket *p = [data bytes];
    
    NSLog(@"received data. index %d. total %d. %@%@",
        p->index,
        receivedDatagramCount,
        (p->tag == DMPDataPacketTagStart) ? @"START" : @"",
        (p->tag == DMPDataPacketTagEnd) ? @"END" : @"");
    
    if (p->tag == DMPDataPacketTagStart)
    {
        receivedDatagramCount = 1;
    }
        
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
