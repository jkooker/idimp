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
@synthesize _masterAudioLevelTextField;
@synthesize _synthAudioLevelSlider;
@synthesize _synthAudioMuteSwitch;
@synthesize _synthAudioLevelTextField;
@synthesize _recordedAudioLevelSlider;
@synthesize _recordedAudioMuteSwitch;
@synthesize _recordedAudioLevelTextField;
@synthesize _networkAudioLevelSlider;
@synthesize _networkAudioMuteSwitch;
@synthesize _networkAudioLevelTextField;

// Implement viewDidLoad to do additional setup after loading the view.
- (void)viewDidLoad 
{
    NSLog(@"MasterAudioControlsViewController viewDidLoad");

    // store app delegate for future use
    _appDelegate = (iDiMPAppDelegate *)[UIApplication sharedApplication].delegate;

    // get instance of audio engine
    _audioEngine = AudioEngine::getInstance();

    // init switch states
    [_recordedAudioMuteSwitch setOn: !_audioEngine->getMuteRecording()];
    [_synthAudioMuteSwitch setOn: !_audioEngine->getMuteSynth()];
    [_networkAudioMuteSwitch setOn: !_audioEngine->getMuteNetwork()];
    [_masterAudioMuteSwitch setOn: _audioEngine->isStarted()];
    
    // enable/disable sliders
    [_masterAudioLevelSlider setEnabled: _audioEngine->isStarted()];
    [_recordedAudioLevelSlider setEnabled: !_audioEngine->getMuteRecording()];
    [_synthAudioLevelSlider setEnabled: !_audioEngine->getMuteSynth()];
    [_networkAudioLevelSlider setEnabled: !_audioEngine->getMuteNetwork()];
    
    // init text fields
    [_masterAudioLevelTextField setText:[NSString stringWithFormat:@"%f", [_masterAudioLevelSlider value]]];
    [_recordedAudioLevelTextField setText:[NSString stringWithFormat:@"%f", [_recordedAudioLevelSlider value]]];
    [_synthAudioLevelTextField setText:[NSString stringWithFormat:@"%f", [_synthAudioLevelSlider value]]];
    [_networkAudioLevelTextField setText:[NSString stringWithFormat:@"%f", [_networkAudioLevelSlider value]]];
    
    [super viewDidLoad];
}

- (IBAction) masterAudioLevelSliderChanged: (id) sender
{
    NSLog(@"MasterAudioControlsViewController masterAudioLevelSliderChanged");
    
    [_appDelegate masterAudioAmpEffect]->getParameter(0)->setValue([_masterAudioLevelSlider value]);
    [_masterAudioLevelTextField setText:[NSString stringWithFormat:@"%f", [_masterAudioLevelSlider value]]];
}

- (IBAction) masterAudioMuteSwitchChanged: (id) sender
{
	bool isOn = [_masterAudioMuteSwitch isOn];
    if (isOn)
    {
        NSLog(@"masterAudioMuteSwitchChanged starting all audio");
        [_masterAudioLevelSlider setEnabled:true];
        _audioEngine->start();
    }
    else
    {
        NSLog(@"masterAudioMuteSwitchChanged stopping all audio");
        [_masterAudioLevelSlider setEnabled:false];
        _audioEngine->stop();
    }
}

- (IBAction) synthAudioLevelSliderChanged: (id) sender
{
    NSLog(@"MasterAudioControlsViewController synthAudioLevelSliderChanged");
    
    [_appDelegate synthAudioAmpEffect]->getParameter(0)->setValue([_synthAudioLevelSlider value]);
    [_synthAudioLevelTextField setText:[NSString stringWithFormat:@"%f", [_synthAudioLevelSlider value]]];
}

- (IBAction) synthAudioMuteSwitchChanged: (id) sender
{
	bool isOn = [_synthAudioMuteSwitch isOn];
    if (isOn)
    {
        NSLog(@"synthAudioMuteSwitchChanged unmuting synth");
        [_synthAudioLevelSlider setEnabled:true];
        _audioEngine->setMuteSynth(false);
    }
    else
    {
        NSLog(@"synthAudioMuteSwitchChanged muting synth");
        [_synthAudioLevelSlider setEnabled:false];
        _audioEngine->setMuteSynth(true);
    }
}

- (IBAction) recordedAudioLevelSliderChanged: (id) sender
{
    NSLog(@"MasterAudioControlsViewController recordedAudioLevelSliderChanged");
    
    [_appDelegate recordingAudioAmpEffect]->getParameter(0)->setValue([_recordedAudioLevelSlider value]);
    [_recordedAudioLevelTextField setText:[NSString stringWithFormat:@"%f", [_recordedAudioLevelSlider value]]];
}

- (IBAction) recordedAudioMuteSwitchChanged: (id) sender
{
	bool isOn = [_recordedAudioMuteSwitch isOn];
    if (isOn)
    {
        NSLog(@"recordedAudioMuteSwitchChanged unmuting recording");
        [_recordedAudioLevelSlider setEnabled:true];
        _audioEngine->setMuteRecording(false);
    }
    else
    {
        NSLog(@"recordedAudioMuteSwitchChanged muting recording");
        [_recordedAudioLevelSlider setEnabled:false];
        _audioEngine->setMuteRecording(true);
    }
}

- (IBAction) networkAudioLevelSliderChanged: (id) sender
{
    NSLog(@"MasterAudioControlsViewController networkAudioLevelSliderChanged");
    
    [_appDelegate networkAudioAmpEffect]->getParameter(0)->setValue([_networkAudioLevelSlider value]);
    [_networkAudioLevelTextField setText:[NSString stringWithFormat:@"%f", [_networkAudioLevelSlider value]]];
}

- (IBAction) networkAudioMuteSwitchChanged: (id) sender
{
	bool isOn = [_networkAudioMuteSwitch isOn];
    if (isOn)
    {
        NSLog(@"networkAudioMuteSwitchChanged unmuting network");
        [_networkAudioLevelSlider setEnabled:true];
        _audioEngine->setMuteNetwork(false);
    }
    else
    {
        NSLog(@"networkAudioMuteSwitchChanged muting network");
        [_networkAudioLevelSlider setEnabled:false];
        _audioEngine->setMuteNetwork(true);
    }
}

@end
