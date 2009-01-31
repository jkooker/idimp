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
//  iDiMPAppDelegate.m
//  iDiMP
//
//  Created by John Kooker on 11/20/08.
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
    
    _networkController = [NetworkController sharedInstance];
    
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
