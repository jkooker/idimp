//
//  NetworkController.h
//  iDiMP
//
//  Created by John Kooker on 12/10/08.
//  Copyright 2008 Apple Inc. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "AsyncUdpSocket.h"

#define kNumSamplesPerChannel 1024
#define kNumSlicesPerPacket 2
// Make this a multiple of 2 for fast % computations
#define kNumCachedReceiveBuffers 8

typedef struct DMPDataPacketSlice
{
    uint8_t index; // maybe this should be 'tag'?
    short data[kNumSamplesPerChannel];
} DMPDataPacketSlice;

typedef struct DMPDataPacket {
    DMPDataPacketSlice slices[kNumSlicesPerPacket];
} DMPDataPacket;


/**
 * The NetworkController class is a singleton, and it provides the following services:
 * - Bonjour advertising and browsing
 * - Sending and receiving audio data over UDP
 */
@interface NetworkController : NSObject {
    AsyncUdpSocket *socket;
    NSNetService *netService;
    BOOL serviceIsPublishing;
    NSNetServiceBrowser *browser;
    BOOL browserIsSearching;
    NSMutableArray *services; // Active NSNetServices
    NSData *savedAddress; // Current destination of network audio
    UITableView *clientTableView;
    
    DMPDataPacket savedPacket;
    uint8_t oldestSlice;
    uint8_t currentSendBufferIndex;
    DMPDataPacketSlice audioReceiveBuffers[kNumCachedReceiveBuffers];
    NSUInteger currentReceiveBufferIndex;
}

@property (nonatomic, retain) NSMutableArray *services;
@property (nonatomic, retain) NSData *savedAddress;
@property (nonatomic, retain) UITableView *clientTableView;

+ (NetworkController *)sharedInstance;

- (void)sendAudioBuffer:(short*)buffer length:(int)length channels:(int)numChannels;
- (void)fillAudioBuffer:(short*)buffer samplesPerChannel:(int)samplesPerChannel channels:(int)numChannels;

- (void)startAudioServer;
- (void)stopAudioServer;

- (void)startBonjourPublishing;
- (void)stopBonjourPublishing;

- (void)startBonjourSearch;
- (void)stopBonjourSearch;

@end
