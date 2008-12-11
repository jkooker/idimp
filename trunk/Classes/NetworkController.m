//
//  NetworkController.m
//  iDiMP
//
//  Created by John Kooker on 12/10/08.
//  Copyright 2008 Apple Inc. All rights reserved.
//

#import "NetworkController.h"

#define kBonjourServiceType @"_idimp._udp"

@implementation NetworkController

@synthesize services;

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
        
        // Advertise over Bonjour
        netService = [[NSNetService alloc] initWithDomain:@"local." type:kBonjourServiceType name:@"" port:23711];
        if (netService == nil)
        {
            NSLog(@"error: could not initialize NetService");
        }
        else
        {
            [netService scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSRunLoopCommonModes];
            [netService publish];
            [netService setDelegate:self];
        }
    }
    return self;
}

- (void)dealloc
{
    [services release];
    [netService release];
    
    [super dealloc];
}

- (void)sendAudioBuffer:(short*)buffer length:(int)length
{
    // stub
}

- (void)fillAudioBuffer:(short*)buffer samplesPerChannel:(int)samplesPerChannel channels:(int)numChannels
{
    // stub
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


@end
