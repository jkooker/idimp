//
//  NetworkController.m
//  iDiMP
//
//  Created by John Kooker on 12/10/08.
//  Copyright 2008 Apple Inc. All rights reserved.
//

#import "NetworkController.h"

#define kBonjourServiceType @"_idimp._udp"
#define kiDiMPSocketPort 23711

@implementation NetworkController

@synthesize services;
@synthesize savedAddress;
@synthesize clientTableView;

#pragma mark Singleton implementation

static NetworkController *sharedNetworkController = nil;
 
+ (NetworkController *)sharedInstance
{
    @synchronized(self) {
        if (sharedNetworkController == nil) {
            [[self alloc] init]; // assignment not done here
        }
    }
    return sharedNetworkController;
}
 
+ (id)allocWithZone:(NSZone *)zone
{
    @synchronized(self) {
        if (sharedNetworkController == nil) {
            sharedNetworkController = [super allocWithZone:zone];
            return sharedNetworkController;  // assignment and return on first allocation
        }
    }
    return nil; //on subsequent allocation attempts return nil
}
 
- (id)copyWithZone:(NSZone *)zone
{
    return self;
}
 
- (id)retain
{
    return self;
}
 
- (unsigned)retainCount
{
    return UINT_MAX;  //denotes an object that cannot be released
}
 
- (void)release
{
    //do nothing
}
 
- (id)autorelease
{
    return self;
}

#pragma mark -

- (id)init
{
    if (self = [super init])
    {
        services = [[NSMutableArray array] retain];
        
        // Initialize UDP socket
        socket = [[AsyncUdpSocket alloc] initWithDelegate:self];
        NSError *error = nil;
        if (![socket bindToPort:kiDiMPSocketPort error:&error])
        {
            NSLog(@"could not bindToPort. %@", error);
        }
        NSLog(@"created socket. host: %@, port: %d", [socket localHost], [socket localPort]);
        [socket receiveWithTimeout:-1 tag:0];
        
        // Advertise over Bonjour
        netService = [[NSNetService alloc] initWithDomain:@"local." type:kBonjourServiceType name:@"" port:kiDiMPSocketPort];
        if (netService == nil)
        {
            NSLog(@"error: could not initialize NetService");
        }
        else
        {
            [netService scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSRunLoopCommonModes];
            [netService setDelegate:self];
        }
        
        // Prepare Bonjour browser
        browser = [[NSNetServiceBrowser alloc] init];
        [browser setDelegate:self];
    }
    return self;
}

- (void)dealloc
{
    [services release];
    [netService release];
    [browser release];
    
    [super dealloc];
}

- (void)sendAudioBuffer:(short*)buffer length:(int)length
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    // send the data out
    if (savedAddress)
    {
        NSLog(@"sending data!");
        [socket sendData:[NSData dataWithBytes:buffer length:length] toAddress:savedAddress withTimeout:2 tag:0];
    }
    [pool release];
}

- (void)fillAudioBuffer:(short*)buffer samplesPerChannel:(int)samplesPerChannel channels:(int)numChannels
{
    // stub
}

#pragma mark Bonjour Controls
- (void)startBonjourPublishing
{
    [netService publish];
}

- (void)stopBonjourPublishing
{
    [netService stop];
}

- (void)startBonjourSearch
{
    [browser searchForServicesOfType:kBonjourServiceType inDomain:@"local."];
}

- (void)stopBonjourSearch
{
    [browser stop];
}


#pragma mark NSNetService Delegate Methods

- (void)netService:(NSNetService *)sender didNotPublish:(NSDictionary *)errorDict
{
	NSLog(@"%@ %s", [self class], _cmd);
}

- (void)netServiceDidPublish:(NSNetService *)sender
{
	NSLog(@"%@ %s", [self class], _cmd);
}

- (void)netServiceDidResolveAddress:(NSNetService *)sender
{
    const char *firstAddressData = (const char *)[[[sender addresses] objectAtIndex:0] bytes];
    NSLog(@"address resolved. %@ = %@ (%hhu.%hhu.%hhu.%hhu) (1 of %d)",
        [sender name],
        [[sender addresses] objectAtIndex:0],
        firstAddressData[4], firstAddressData[5], firstAddressData[6], firstAddressData[7],
        [[sender addresses] count]);
}

- (void)netService:(NSNetService *)sender didNotResolve:(NSDictionary *)errorDict
{
    NSLog(@"did not resolve address for %@.", [sender name]);
}

#pragma mark NSNetServiceBrowser Delegate Methods

- (void)netServiceBrowserWillSearch:(NSNetServiceBrowser *)aNetServiceBrowser
{
    NSLog(@"%@ %s", [self class], _cmd);
}

- (void)netServiceBrowser:(NSNetServiceBrowser *)aNetServiceBrowser didFindService:(NSNetService *)aNetService moreComing:(BOOL)moreComing
{
	//NSLog(@"netservice found: %@ %@", [aNetService name], moreComing ? @"(moreComing)" : @"");
    
    [services addObject:aNetService];
    if (clientTableView)
    {
        [clientTableView reloadData];
    }
    [aNetService setDelegate:self];
    [aNetService resolveWithTimeout:5];
}

- (void)netServiceBrowser:(NSNetServiceBrowser *)aNetServiceBrowser didRemoveService:(NSNetService *)aNetService moreComing:(BOOL)moreComing
{
    BOOL foundService = NO;
    
    for (NSNetService *currentService in services)
    {
        if ([[currentService name] isEqual:[aNetService name]] &&
            [[currentService type] isEqual:[aNetService type]] &&
            [[currentService domain] isEqual:[aNetService domain]])
        {
            [services removeObject:currentService];
            if (clientTableView)
            {
                [clientTableView reloadData];
            }
            foundService = YES;
            break;
        }
    }
    
	NSLog(@"netservice removed: %@ (%@)", [aNetService name], foundService ? @"successfully" : @"unsuccessfully");
}

#pragma mark AsyncUdpSocket Delegate Methods
- (void)onUdpSocket:(AsyncUdpSocket *)sock didSendDataWithTag:(long)tag
{
    NSLog(@"%@ %s", [self class], _cmd);
}

- (void)onUdpSocket:(AsyncUdpSocket *)sock didNotSendDataWithTag:(long)tag dueToError:(NSError *)error
{
    NSLog(@"%@ %s", [self class], _cmd);
}

- (BOOL)onUdpSocket:(AsyncUdpSocket *)sock didReceiveData:(NSData *)data withTag:(long)tag fromHost:(NSString *)host port:(UInt16)port
{
    NSLog(@"%@ %s", [self class], _cmd);
    
    [sock receiveWithTimeout:-1 tag:0];
    
    return YES;
}

- (void)onUdpSocket:(AsyncUdpSocket *)sock didNotReceiveDataWithTag:(long)tag dueToError:(NSError *)error
{
    NSLog(@"%@ %s", [self class], _cmd);
}

- (void)onUdpSocketDidClose:(AsyncUdpSocket *)sock
{
    NSLog(@"%@ %s", [self class], _cmd);
}



@end
