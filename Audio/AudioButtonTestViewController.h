//
//  AudioButtonTestViewController.h
//  AudioButtonTest
//
//  Created by Michelle Daniels on 11/2/08.
//  Copyright UCSD 2008. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AudioEngine.h"

@interface AudioButtonTestViewController : UIViewController <UIAccelerometerDelegate> {

    IBOutlet UIButton* _playButton;
    IBOutlet UISlider* _frequencySlider;
    IBOutlet UITextField* _frequencyTextField;
    IBOutlet UISlider* _ringModFreqSlider;
    IBOutlet UITextField* _ringModFreqTextField;
    IBOutlet UISlider* _ampSlider;
    IBOutlet UITextField* _ampTextField;
    IBOutlet UISegmentedControl* _waveformSelector;
    IBOutlet UISwitch* _recordedInputSwitch;
    IBOutlet UISwitch* _synthInputSwitch;
    BOOL _playIsOn;
    BOOL _recordIsOn;
    AudioEngine* _audioEngine;
    AudioEffectParameter* _ringModFreqParam;
    AudioEffectParameter* _ampParam;
    AudioEffect* _ringModEffect;
    AudioEffect* _ampEffect;
}

@property (nonatomic, retain) IBOutlet UIButton* _playButton;
@property (nonatomic, retain) IBOutlet UISlider* _frequencySlider;
@property (nonatomic, retain) IBOutlet UITextField* _frequencyTextField;
@property (nonatomic, retain) IBOutlet UISlider* _ringModFreqSlider;
@property (nonatomic, retain) IBOutlet UITextField* _ringModFreqTextField;
@property (nonatomic, retain) IBOutlet UISlider* _ampSlider;
@property (nonatomic, retain) IBOutlet UITextField* _ampTextField;
@property (nonatomic, retain) IBOutlet UISegmentedControl* _waveformSelector;
@property (nonatomic, retain) IBOutlet UISwitch* _recordedInputSwitch;
@property (nonatomic, retain) IBOutlet UISwitch* _synthInputSwitch;
@property (readwrite) BOOL _playIsOn;
@property (readwrite) BOOL _recordIsOn;

- (void) myInit;
- (void) dealloc;
- (IBAction) playOrStop: (id) sender;
- (IBAction) recordedInputSwitchChanged: (id) sender;
- (IBAction) synthInputSwitchChanged: (id) sender;
- (IBAction) frequencySliderChanged: (id) sender;
- (IBAction) ringModFreqSliderChanged: (id) sender;
- (IBAction) ampSliderChanged: (id) sender;
- (IBAction) waveformSelected: (id) sender;
- (void) accelerometer:(UIAccelerometer *)accelerometer didAccelerate:(UIAcceleration *)acceleration;


@end
