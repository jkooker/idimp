//
//  iDiMPAppDelegate.h
//  iDiMP
//
//  Created by John Kooker on 11/20/08.
//  Copyright Apple Inc 2008. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AudioEngine.h"
#import "NetworkController.h"

@class RootViewController;

@interface iDiMPAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    RootViewController *rootViewController;
    
    AudioEngine* _audioEngine;
    AudioEffect* _masterAmpEffect;
    AudioEffect* _recordingAmpEffect;
    AudioEffect* _synthAmpEffect;
    AudioEffect* _networkAmpEffect;
    
    NetworkController *_networkController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet RootViewController *rootViewController;

- (AudioEffect*) masterAudioAmpEffect;
- (AudioEffect*) recordingAudioAmpEffect;
- (AudioEffect*) synthAudioAmpEffect;
- (AudioEffect*) networkAudioAmpEffect;

@end

