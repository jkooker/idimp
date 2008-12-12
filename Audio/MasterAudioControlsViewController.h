//
//  MasterAudioControlsViewController.h
//  iDiMP
//
//  Created by Michelle Daniels on 12/10/08.
//  Copyright 2008 UCSD. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "iDiMPAppDelegate.h"

/**
  * MasterAudioControlsViewController class.
  */
@interface MasterAudioControlsViewController : UIViewController {
    IBOutlet UISwitch* _masterAudioMuteSwitch;
    IBOutlet UISlider* _synthAudioLevelSlider;
    IBOutlet UISwitch* _synthAudioMuteSwitch;
    IBOutlet UITextField* _synthAudioLevelTextField;
    IBOutlet UISlider* _recordedAudioLevelSlider;
    IBOutlet UISwitch* _recordedAudioMuteSwitch;
    IBOutlet UITextField* _recordedAudioLevelTextField;
    IBOutlet UISlider* _networkAudioLevelSlider;
    IBOutlet UISwitch* _networkAudioMuteSwitch;
    IBOutlet UITextField* _networkAudioLevelTextField;
    
    iDiMPAppDelegate* _appDelegate;
    AudioEngine* _audioEngine;
}

@property (nonatomic, retain) IBOutlet UISwitch* _masterAudioMuteSwitch;
@property (nonatomic, retain) IBOutlet UISlider* _synthAudioLevelSlider;
@property (nonatomic, retain) IBOutlet UISwitch* _synthAudioMuteSwitch;
@property (nonatomic, retain) IBOutlet UITextField* _synthAudioLevelTextField;
@property (nonatomic, retain) IBOutlet UISlider* _recordedAudioLevelSlider;
@property (nonatomic, retain) IBOutlet UISwitch* _recordedAudioMuteSwitch;
@property (nonatomic, retain) IBOutlet UITextField* _recordedAudioLevelTextField;
@property (nonatomic, retain) IBOutlet UISlider* _networkAudioLevelSlider;
@property (nonatomic, retain) IBOutlet UISwitch* _networkAudioMuteSwitch;
@property (nonatomic, retain) IBOutlet UITextField* _networkAudioLevelTextField;

/**
 * This callback is called when the UISwitch for muting master audio is toggled.
 * @param sender the caller of the callback
 */
- (IBAction) masterAudioMuteSwitchChanged: (id) sender;

/**
 * This callback is called when the slider for changing the synthesized audio level is moved.
 * @param sender the caller of the callback
 */
- (IBAction) synthAudioLevelSliderChanged: (id) sender;

/**
 * This callback is called when the UISwitch for muting synthesized audio is toggled.
 * @param sender the caller of the callback
 */
- (IBAction) synthAudioMuteSwitchChanged: (id) sender;

/**
 * This callback is called when the slider for changing the recorded audio level is moved.
 * @param sender the caller of the callback
 */
- (IBAction) recordedAudioLevelSliderChanged: (id) sender;

/**
 * This callback is called when the UISwitch for muting recorded audio is toggled.
 * @param sender the caller of the callback
 */
- (IBAction) recordedAudioMuteSwitchChanged: (id) sender;

/**
 * This callback is called when the slider for changing the level of the streaming network input audio is moved.
 * @param sender the caller of the callback
 */
- (IBAction) networkAudioLevelSliderChanged: (id) sender;

/**
 * This callback is called when the UISwitch for muting streaming network input audio is toggled.
 * @param sender the caller of the callback
 */
- (IBAction) networkAudioMuteSwitchChanged: (id) sender;

@end