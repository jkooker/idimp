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

    IBOutlet UISlider* _ringModFreqSlider;
    IBOutlet UITextField* _ringModFreqTextField;
    IBOutlet UISlider* _ampSlider;
    IBOutlet UITextField* _ampTextField;
    IBOutlet UISegmentedControl* _waveformSelector;
    AudioEngine* _audioEngine;
    AudioEffectParameter* _ringModFreqParam;
    AudioEffectParameter* _ampParam;
    AudioEffect* _ringModEffect;
    AudioEffect* _ampEffect;
}

@property (nonatomic, retain) IBOutlet UISlider* _ringModFreqSlider;
@property (nonatomic, retain) IBOutlet UITextField* _ringModFreqTextField;
@property (nonatomic, retain) IBOutlet UISlider* _ampSlider;
@property (nonatomic, retain) IBOutlet UITextField* _ampTextField;
@property (nonatomic, retain) IBOutlet UISegmentedControl* _waveformSelector;

- (void) myInit;
- (void) dealloc;
- (IBAction) ringModFreqSliderChanged: (id) sender;
- (IBAction) ampSliderChanged: (id) sender;
- (IBAction) waveformSelected: (id) sender;
- (void) accelerometer:(UIAccelerometer *)accelerometer didAccelerate:(UIAcceleration *)acceleration;


@end

