//
//  MasterAudioControlsViewController.h
//  iDiMP
//
//  Created by Michelle Daniels on 12/10/08.
//  Copyright 2008 UCSD. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AudioEngine.h"

@interface MasterAudioControlsViewController : UIViewController {
    IBOutlet UISlider* _masterAudioLevelSlider;
    IBOutlet UISwitch* _masterAudioMuteSwitch;
    IBOutlet UISlider* _synthAudioLevelSlider;
    IBOutlet UISwitch* _synthAudioMuteSwitch;
    IBOutlet UISlider* _recordedAudioLevelSlider;
    IBOutlet UISwitch* _recordedAudioMuteSwitch;
    IBOutlet UISlider* _networkAudioLevelSlider;
    IBOutlet UISwitch* _networkAudioMuteSwitch;
    
    AudioEngine* _audioEngine;
}

@property (nonatomic, retain) IBOutlet UISlider* _masterAudioLevelSlider;
@property (nonatomic, retain) IBOutlet UISwitch* _masterAudioMuteSwitch;
@property (nonatomic, retain) IBOutlet UISlider* _synthAudioLevelSlider;
@property (nonatomic, retain) IBOutlet UISwitch* _synthAudioMuteSwitch;
@property (nonatomic, retain) IBOutlet UISlider* _recordedAudioLevelSlider;
@property (nonatomic, retain) IBOutlet UISwitch* _recordedAudioMuteSwitch;
@property (nonatomic, retain) IBOutlet UISlider* _networkAudioLevelSlider;
@property (nonatomic, retain) IBOutlet UISwitch* _networkAudioMuteSwitch;

- (IBAction) masterAudioLevelSliderChanged: (id) sender;
- (IBAction) masterAudioMuteSwitchChanged: (id) sender;
- (IBAction) synthAudioLevelSliderChanged: (id) sender;
- (IBAction) synthAudioMuteSwitchChanged: (id) sender;
- (IBAction) recordedAudioLevelSliderChanged: (id) sender;
- (IBAction) recordedAudioMuteSwitchChanged: (id) sender;
- (IBAction) networkAudioLevelSliderChanged: (id) sender;
- (IBAction) networkAudioMuteSwitchChanged: (id) sender;

@end
