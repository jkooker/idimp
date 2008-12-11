//
//  NetworkController.h
//  iDiMP
//
//  Created by John Kooker on 12/10/08.
//  Copyright 2008 Apple Inc. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "AsyncUdpSocket.h"

/**
 * The NetworkController class is a singleton, and it provides the following services:
 * - Bonjour advertising and browsing
 * - Sending and receiving audio data over UDP
 */
@interface NetworkController : NSObject {
    AsyncUdpSocket *socket;
    NSNetService *netService;
    NSNetServiceBrowser *browser;
    NSMutableArray *services; // Active NSNetServices
    NSData *savedAddress; // Current destination of network audio
    UITableView *clientTableView;
}

@property (nonatomic, retain) NSMutableArray *services;
@property (nonatomic, retain) UITableView *clientTableView;

+ (NetworkController *)sharedInstance;

- (void)sendAudioBuffer:(short*)buffer length:(int)length;
- (void)fillAudioBuffer:(short*)buffer samplesPerChannel:(int)samplesPerChannel channels:(int)numChannels;

- (void)startBonjourSearch;
- (void)stopBonjourSearch;

@end
