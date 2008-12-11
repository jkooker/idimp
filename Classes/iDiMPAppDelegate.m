//
//  iDiMPAppDelegate.m
//  iDiMP
//
//  Created by John Kooker on 11/20/08.
//  Copyright Apple Inc 2008. All rights reserved.
//

#import "iDiMPAppDelegate.h"
#import "RootViewController.h"

@implementation iDiMPAppDelegate


@synthesize window;
@synthesize rootViewController;

- (AudioEffect*) masterAudioAmpEffect
{
    return _masterAmpEffect;
}

- (AudioEffect*) recordingAudioAmpEffect
{
    return _recordingAmpEffect;
}

- (AudioEffect*) synthAudioAmpEffect
{
    return _synthAmpEffect;
}

- (AudioEffect*) networkAudioAmpEffect
{
    return _networkAmpEffect;
}

- (void)applicationDidFinishLaunching:(UIApplication *)application 
{    
    // init audio engine
    _audioEngine = AudioEngine::getInstance();
    
    _masterAmpEffect = NULL;
    _masterAmpEffect = new AmplitudeScale();
    _audioEngine->addMasterEffect(_masterAmpEffect);
    _recordingAmpEffect = NULL;
    _recordingAmpEffect = new AmplitudeScale();
    _audioEngine->addRecordingEffect(_recordingAmpEffect);
    _synthAmpEffect = NULL;
    _synthAmpEffect = new AmplitudeScale();
    _audioEngine->addSynthesisEffect(_synthAmpEffect);
    _networkAmpEffect = NULL;
    _networkAmpEffect = new AmplitudeScale();
    _audioEngine->addNetworkEffect(_networkAmpEffect);    
    
    [window addSubview:[rootViewController view]];
    [window makeKeyAndVisible];
}

- (void)dealloc 
{
    // dont need to delete the AudioEngine - we don't own it

    if (_masterAmpEffect != NULL)
    {
        _audioEngine->removeMasterEffect(_masterAmpEffect);
        delete _masterAmpEffect;
        _masterAmpEffect = NULL;
    }
    if (_recordingAmpEffect != NULL)
    {
        _audioEngine->removeRecordingEffect(_recordingAmpEffect);
        delete _recordingAmpEffect;
        _recordingAmpEffect = NULL;
    }
    if (_synthAmpEffect != NULL)
    {
        _audioEngine->removeSynthesisEffect(_synthAmpEffect);
        delete _synthAmpEffect;
        _synthAmpEffect = NULL;
    }
    if (_networkAmpEffect != NULL)
    {
        _audioEngine->removeNetworkEffect(_networkAmpEffect);
        delete _networkAmpEffect;
        _networkAmpEffect = NULL;
    }

    [rootViewController release];
    [window release];
    [super dealloc];
}

@end
