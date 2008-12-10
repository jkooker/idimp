//
//  MasterAudioControlsViewController.mm
//  iDiMP
//
//  Created by Michelle Daniels on 12/10/08.
//  Copyright 2008 UCSD. All rights reserved.
//

#import "MasterAudioControlsViewController.h"

@implementation MasterAudioControlsViewController

@synthesize _masterAudioLevelSlider;
@synthesize _masterAudioMuteSwitch;
@synthesize _synthAudioLevelSlider;
@synthesize _synthAudioMuteSwitch;
@synthesize _recordedAudioLevelSlider;
@synthesize _recordedAudioMuteSwitch;
@synthesize _networkAudioLevelSlider;
@synthesize _networkAudioMuteSwitch;

// Implement viewDidLoad to do additional setup after loading the view.
- (void)viewDidLoad 
{
    NSLog(@"MasterAudioControlsViewController viewDidLoad");
        
    // init audio engine
    _audioEngine = AudioEngine::getInstance();

    // init switch states
    [_recordedAudioMuteSwitch setOn:!_audioEngine->getMuteRecording()];
    [_synthAudioMuteSwitch setOn:!_audioEngine->getMuteSynth()];
    [_networkAudioMuteSwitch setOn:!_audioEngine->getMuteNetwork()];
    [_masterAudioMuteSwitch setOn:_audioEngine->isStarted()];
    
    [super viewDidLoad];
}

- (IBAction) masterAudioLevelSliderChanged: (id) sender
{
    NSLog(@"MasterAudioControlsViewController masterAudioLevelSliderChanged");
}

- (IBAction) masterAudioMuteSwitchChanged: (id) sender
{
	bool isOn = [_masterAudioMuteSwitch isOn];
    if (isOn)
    {
        NSLog(@"masterAudioMuteSwitchChanged starting all audio");
        _audioEngine->start();
    }
    else
    {
        NSLog(@"masterAudioMuteSwitchChanged stopping all audio");
        _audioEngine->stop();
    }
}

- (IBAction) synthAudioLevelSliderChanged: (id) sender
{
    NSLog(@"MasterAudioControlsViewController synthAudioLevelSliderChanged");
}

- (IBAction) synthAudioMuteSwitchChanged: (id) sender
{
	bool isOn = [_synthAudioMuteSwitch isOn];
    if (isOn)
    {
        NSLog(@"synthAudioMuteSwitchChanged unmuting synth");
        _audioEngine->setMuteSynth(false);
    }
    else
    {
        NSLog(@"synthAudioMuteSwitchChanged muting synth");
        _audioEngine->setMuteSynth(true);
    }
}

- (IBAction) recordedAudioLevelSliderChanged: (id) sender
{
    NSLog(@"MasterAudioControlsViewController recordedAudioLevelSliderChanged");
}

- (IBAction) recordedAudioMuteSwitchChanged: (id) sender
{
	bool isOn = [_recordedAudioMuteSwitch isOn];
    if (isOn)
    {
        NSLog(@"recordedAudioMuteSwitchChanged unmuting recording");
        _audioEngine->setMuteRecording(false);
    }
    else
    {
        NSLog(@"recordedAudioMuteSwitchChanged muting recording");
        _audioEngine->setMuteRecording(true);
    }
}

- (IBAction) networkAudioLevelSliderChanged: (id) sender
{
    NSLog(@"MasterAudioControlsViewController networkAudioLevelSliderChanged");
}

- (IBAction) networkAudioMuteSwitchChanged: (id) sender
{
	bool isOn = [_networkAudioMuteSwitch isOn];
    if (isOn)
    {
        NSLog(@"networkAudioMuteSwitchChanged unmuting network");
        _audioEngine->setMuteNetwork(false);
    }
    else
    {
        NSLog(@"networkAudioMuteSwitchChanged muting network");
        _audioEngine->setMuteNetwork(true);
    }
}

@end
