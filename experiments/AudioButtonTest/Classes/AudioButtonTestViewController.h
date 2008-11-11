//
//  AudioButtonTestViewController.h
//  AudioButtonTest
//
//  Created by Michelle Daniels on 11/2/08.
//  Copyright UCSD 2008. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AudioQueueWrapper.h"

@interface AudioButtonTestViewController : UIViewController <UIAccelerometerDelegate> {

    IBOutlet UIButton* _playButton;
    IBOutlet UIButton* _recordButton;
    IBOutlet UISlider* _frequencySlider;
    IBOutlet UITextField* _frequencyTextField;
    IBOutlet UISegmentedControl* _waveformSelector;
    BOOL _playIsOn;
    BOOL _recordIsOn;
    AudioQueueWrapper* _audioQueue;
}

@property (nonatomic, retain) IBOutlet UIButton* _playButton;
@property (nonatomic, retain) IBOutlet UIButton* _recordButton;
@property (nonatomic, retain) IBOutlet UISlider* _frequencySlider;
@property (nonatomic, retain) IBOutlet UITextField* _frequencyTextField;
@property (nonatomic, retain) IBOutlet UISegmentedControl* _waveformSelector;
@property (readwrite) BOOL _playIsOn;
@property (readwrite) BOOL _recordIsOn;

- (void) myInit;
- (void) dealloc;
- (IBAction) playOrStop: (id) sender;
- (IBAction) recordOrStop: (id) sender;
- (IBAction) frequencySliderChanged: (id) sender;
- (IBAction) waveformSelected: (id) sender;
- (void) accelerometer:(UIAccelerometer *)accelerometer didAccelerate:(UIAcceleration *)acceleration;


@end

