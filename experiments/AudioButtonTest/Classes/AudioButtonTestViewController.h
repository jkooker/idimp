//
//  AudioButtonTestViewController.h
//  AudioButtonTest
//
//  Created by Michelle Daniels on 11/2/08.
//  Copyright UCSD 2008. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AudioQueueWrapper.h"

@interface AudioButtonTestViewController : UIViewController {

    IBOutlet UIButton* playButton;
    IBOutlet UIButton* recordButton;
    IBOutlet UISlider* _frequencySlider;
    IBOutlet UITextField* _frequencyTextField;
    IBOutlet UISegmentedControl* _waveformSelector;
    BOOL playIsOn;
    BOOL recordIsOn;
    AudioQueueWrapper* m_audioQueue;
}

@property (nonatomic, retain) IBOutlet UIButton* playButton;
@property (nonatomic, retain) IBOutlet UIButton* recordButton;
@property (nonatomic, retain) IBOutlet UISlider* _frequencySlider;
@property (nonatomic, retain) IBOutlet UITextField* _frequencyTextField;
@property (nonatomic, retain) IBOutlet UISegmentedControl* _waveformSelector;
@property (readwrite) BOOL playIsOn;
@property (readwrite) BOOL recordIsOn;

- (void) myInit;
- (void) dealloc;
- (IBAction) playOrStop: (id) sender;
- (IBAction) recordOrStop: (id) sender;
- (IBAction) frequencySliderChanged: (id) sender;
- (IBAction) waveformSelected: (id) sender;

@end

