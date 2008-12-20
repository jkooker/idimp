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

/**
 * Holds one buffer from Core Audio, along with a tag byte.
 */
typedef struct DMPDataPacketSlice
{
    uint8_t index; // maybe this should be 'tag'?
    short data[kNumSamplesPerChannel];
} DMPDataPacketSlice;

/**
 * For data redundancy, holds multiple DMPDataPacketSlice structs.
 */
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

/**
 * A list of available Bonjour iDiMP NetServices.
 * Used to populate the Audio Destination table section.
 */
@property (nonatomic, retain) NSMutableArray *services;
/**
 * The selected destination for networked audio.
 */
@property (nonatomic, retain) NSData *savedAddress;
/**
 * Used to provide update notifications, instead of implementing an entire delegate protocol.
 */
@property (nonatomic, retain) UITableView *clientTableView;

/**
 * Obtains the shared instance of the singleton class.
 */
+ (NetworkController *)sharedInstance;

/**
 * Called by AudioEngine to queue a buffer for network transmit.
 */
- (void)sendAudioBuffer:(short*)buffer length:(int)length channels:(int)numChannels;
/**
 * Called by AudioEngine to obtain received network audio.
 */
- (void)fillAudioBuffer:(short*)buffer samplesPerChannel:(int)samplesPerChannel channels:(int)numChannels;

/**
 * Creates and binds the UDP socket, and starts Bonjour publishing and searching.
 */
- (void)startAudioServer;
/**
 * Closes and destroys the UDP socket, and stops Bonjour publishing and searching.
 */
- (void)stopAudioServer;

/**
 * Advertises the Bonjour service.
 */
- (void)startBonjourPublishing;
/**
 * Stops advertising the Bonjour service.
 */
- (void)stopBonjourPublishing;

/**
 * Searches for iDiMP UDP Bonjour services.
 */
- (void)startBonjourSearch;
/**
 * Stops searching for iDiMP UDP Bonjour services.
 */
- (void)stopBonjourSearch;

@end
