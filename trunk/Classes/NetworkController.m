// Copyright (c) 2009 Michelle Daniels and John Kooker
// 
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
// 
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.

//
//  NetworkController.m
//  iDiMP
//
//  Created by John Kooker on 12/10/08.
//

#import "NetworkController.h"

#define kBonjourServiceType @"_idimp._udp"
#define kiDiMPSocketPort 23711
#define kSendDataTimeout 1.0 // in seconds
#define kNetBufferLatency 4  // play four silent buffers before playing network audio

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
        
        // Prepare Bonjour service
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
        browserIsSearching = NO;
    }
    return self;
}

- (void)dealloc
{
    NSLog(@"%@ %s", [self class], _cmd);

    [socket close];
    [socket release];
    [services release];
    [netService release];
    [browser release];
    
    [super dealloc];
}

- (void)sendAudioBuffer:(short*)buffer length:(int)length channels:(int)numChannels
{
    // chop down to mono
    int numSamplesMono = length / numChannels;
    for (int i = 0; i < numSamplesMono; i++) {
        savedPacket.slices[oldestSlice].data[i] = buffer[numChannels * i];
    }
    
    // tag the latest slice with a new index (don't worry about currentSendBufferIndex wrapping around)
    savedPacket.slices[oldestSlice].index = currentSendBufferIndex++;
    
    // increment oldestSlice
    oldestSlice = (oldestSlice + 1) % kNumSlicesPerPacket;
    
    [self performSelectorOnMainThread:@selector(sendSavedPacket) withObject:nil waitUntilDone:NO];
}

- (void)sendSavedPacket
{
    if (savedAddress)
    {
        //NSLog(@"sending data!");
        [socket sendData:[NSData dataWithBytes:&savedPacket length:sizeof(savedPacket)] toAddress:savedAddress withTimeout:kSendDataTimeout tag:0];
    }
}

- (void)fillAudioBuffer:(short*)buffer samplesPerChannel:(int)samplesPerChannel channels:(int)numChannels
{
    // TODO: wait <latency> before starting through our cache
    
    // grab from big receive buffer
    for (int i = 0; i < samplesPerChannel; i++) {
        // copy into buffer, mirror into all channels
        for (int j = 0; j < numChannels; j++) {
            buffer[(numChannels * i) + j] = audioReceiveBuffers[currentReceiveBufferIndex].data[i];
        }
    }
    
    currentReceiveBufferIndex = (currentReceiveBufferIndex + 1) % kNumCachedReceiveBuffers;
}

- (void)startAudioServer
{
    // Initialize UDP socket
    socket = [[AsyncUdpSocket alloc] initWithDelegate:self];

    NSError *error = nil;
    if (![socket bindToPort:kiDiMPSocketPort error:&error])
    {
        NSLog(@"could not bindToPort. %@", error);
    }
    NSLog(@"created socket. host: %@, port: %d", [socket localHost], [socket localPort]);
    [socket receiveWithTimeout:-1 tag:0];

    [self startBonjourPublishing];
    [self startBonjourSearch];
}

- (void)stopAudioServer
{
    [self stopBonjourSearch];
    [self stopBonjourPublishing];
    [socket close];
    [socket release];
    socket = nil;
}

#pragma mark Bonjour Controls
- (void)startBonjourPublishing
{
    if (!serviceIsPublishing)
    {
        [netService publish];
        serviceIsPublishing = YES;
    }
}

- (void)stopBonjourPublishing
{
    if (serviceIsPublishing)
    {
        // Note: you must send a 'stop' for every 'publish' message, or it won't work.
        [netService stop];
        serviceIsPublishing = NO;
    }
}

- (void)startBonjourSearch
{
    if (!browserIsSearching)
    {
        [browser searchForServicesOfType:kBonjourServiceType inDomain:@"local."];
        browserIsSearching = YES;
        // reset found services
        [services removeAllObjects];
    }
}

- (void)stopBonjourSearch
{
    if (browserIsSearching)
    {
        [browser stop];
        browserIsSearching = NO;
    }
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

- (void)netServiceDidStop:(NSNetService *)sender
{
	NSLog(@"%@ %s", [self class], _cmd);
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
    //NSLog(@"%@ %s", [self class], _cmd);
}

- (void)onUdpSocket:(AsyncUdpSocket *)sock didNotSendDataWithTag:(long)tag dueToError:(NSError *)error
{
    NSLog(@"%@ %s", [self class], _cmd);
}

- (BOOL)onUdpSocket:(AsyncUdpSocket *)sock didReceiveData:(NSData *)data withTag:(long)tag fromHost:(NSString *)host port:(UInt16)port
{
    //NSLog(@"%@ %s", [self class], _cmd);
    
    DMPDataPacket *packet = (DMPDataPacket *)[data bytes];
    
    // throw each buffer into the big received buffer at index % kNumCachedReceiveBuffers
    int bufferIndexToWrite = 0;
    for (int i = 0; i < kNumSlicesPerPacket; i++)
    {
        bufferIndexToWrite = packet->slices[i].index % kNumCachedReceiveBuffers;
        
        // don't overwrite the same data
        if (packet->slices[i].index != audioReceiveBuffers[bufferIndexToWrite].index)
        {
            memcpy(&audioReceiveBuffers[bufferIndexToWrite] ,
                &(packet->slices[i]),
                sizeof(DMPDataPacketSlice));
        }
    }

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
