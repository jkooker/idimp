/**
 *  @file AudioButtonTestViewController.h
 *  iDiMP
 *
 *  Created by Michelle Daniels on 11/2/08.
 *  Copyright UCSD 2008. All rights reserved.
 *
 *  This file defines the interface for the AudioButtonTestViewController class.
 */

#import <UIKit/UIKit.h>
#import "AudioEngine.h"

/**
 * AudioButtonTestViewController class.
 * Originally this class was the UIViewController for the AudioButtonTest single-view application.
 * Now this class only controls the "Audio Params" tab of the flipside view on iDiMP, so
 * "AudioButtonTestViewController" is a bit of a misnomer.
 */
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

/**
 * This method does custom initialization that has to be performed after this class is instantiated.
 * It is essentially a constructor and must be called before the view will be functional.
 * Memory allocated here must be freed in dealloc.
 * @see dealloc
 */
- (void) myInit;

/**
 * This method frees any memory allocated in myInit.
 * @see myInit
 */
- (void) dealloc;

/**
 * This callback is called when the slider controlling the ring modulation frequency for recorded audio is moved.
 * It updates the frequency accordingly in the associated AudioEffect object.
 * @param sender the caller of this callback
 */
- (IBAction) ringModFreqSliderChanged: (id) sender;

/** 
 * This callback is called when the selected segment of the UISegmentedControl controlling the current waveform for 
 * synthesis changes.
 * @param sender the caller of this callback
 */
- (IBAction) waveformSelected: (id) sender;

@end

