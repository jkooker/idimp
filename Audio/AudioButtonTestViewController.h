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
 *  @file AudioButtonTestViewController.h
 *  iDiMP
 *
 *  Created by Michelle Daniels on 11/2/08.
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

