//
//  AudioButtonTestViewController.h
//  AudioButtonTest
//
//  Created by Michelle Daniels on 11/2/08.
//  Copyright UCSD 2008. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AudioEngine.h"

@interface AudioButtonTestViewController : UIViewController {

    IBOutlet UISlider* _ringModFreqSlider;
    IBOutlet UITextField* _ringModFreqTextField;
    IBOutlet UISegmentedControl* _waveformSelector;
    AudioEngine* _audioEngine;
    AudioEffectParameter* _ringModFreqParam;
    AudioEffect* _ringModEffect;
}

@property (nonatomic, retain) IBOutlet UISlider* _ringModFreqSlider;
@property (nonatomic, retain) IBOutlet UITextField* _ringModFreqTextField;
@property (nonatomic, retain) IBOutlet UISegmentedControl* _waveformSelector;

- (void) myInit;
- (void) dealloc;
- (IBAction) ringModFreqSliderChanged: (id) sender;
- (IBAction) waveformSelected: (id) sender;

@end

