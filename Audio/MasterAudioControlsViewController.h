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

/**
 *  @file MasterAudioControlsViewController.h
 *  iDiMP 
 *
 *  Created by Michelle Daniels on 12/10/08.
 *
 *  This file defines the interface for the MasterAudioControlsViewController class.
 */

#import <UIKit/UIKit.h>
#import "iDiMPAppDelegate.h"

/**
  * MasterAudioControlsViewController class.
  * This class is the UIViewController for MasterAudioControls.xib
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
