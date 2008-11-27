//
//  AudioButtonTestViewController.mm
//  AudioButtonTest
//
//  Created by Michelle Daniels on 11/2/08.
//  Copyright UCSD 2008. All rights reserved.
//

#import "AudioButtonTestViewController.h"

static const float kAccelerometerInterval = 0.01;

@implementation AudioButtonTestViewController

@synthesize _playButton;
@synthesize _playIsOn;
@synthesize _recordIsOn;
@synthesize _ringModFreqSlider;
@synthesize _ringModFreqTextField;
@synthesize _ampSlider;
@synthesize _ampTextField;
@synthesize _waveformSelector;
@synthesize _recordedInputSwitch;
@synthesize _synthInputSwitch;

- (void) myInit
{   
    NSLog(@"AudioButtonTestViewController myInit");
    _audioEngine = NULL;
    _audioEngine = AudioEngine::getInstance();
    
    // init effects and map their parameters to sliders
    // at processing time, they will be called in the order they are added to the engine
    _ringModEffect = NULL;
    _ringModEffect = new RingMod();
    _ampEffect = NULL;
    _ampEffect = new AmplitudeScale();
    _audioEngine->addRecordingEffect(_ringModEffect);
    _audioEngine->addRecordingEffect(_ampEffect);
    _ringModFreqParam = _ringModEffect->getParameter(0);
    _ampParam = _ampEffect->getParameter(0);
}

- (void) dealloc
{   
    NSLog(@"AudioButtonTestViewController dealloc");
    
    // dont need to delete the AudioEngine - we don't own it

    if (_ringModEffect != NULL)
    {
        _audioEngine->removeRecordingEffect(_ringModEffect);
        delete _ringModEffect;
        _ringModEffect = NULL;
    }
    if (_ampEffect != NULL)
    {
        _audioEngine->removeRecordingEffect(_ampEffect);
        delete _ampEffect;
        _ampEffect = NULL;
    }
    [super dealloc];
}

/*
// Override initWithNibName:bundle: to load the view using a nib file then perform additional customization that is not appropriate for viewDidLoad.
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil]) {
        // Custom initialization
    }
    return self;
}
*/

/*
// Implement loadView to create a view hierarchy programmatically.
- (void)loadView {
}
*/


// Implement viewDidLoad to do additional setup after loading the view.
- (void)viewDidLoad {
    NSLog(@"AudioButtonTestViewController viewDidLoad");
    [self myInit];
        
    float modFreq = _ringModFreqParam->getValue();
    [_ringModFreqTextField setText:[NSString stringWithFormat:@"%d", (int)modFreq]];
    [_ringModFreqSlider setValue:modFreq animated:NO];
    [_ringModFreqSlider setMinimumValue:_ringModFreqParam->getMinValue()];
    [_ringModFreqSlider setMaximumValue:_ringModFreqParam->getMaxValue()];
    
    float amp = _ampParam->getValue();
    [_ampTextField setText:[NSString stringWithFormat:@"%f", amp]];
    [_ampSlider setValue:amp animated:NO];
    [_ampSlider setMinimumValue:_ampParam->getMinValue()];
    [_ampSlider setMaximumValue:_ampParam->getMaxValue()];

    // Set up accelerometer
    [[UIAccelerometer sharedAccelerometer] setUpdateInterval:kAccelerometerInterval]; // in seconds
    [[UIAccelerometer sharedAccelerometer] setDelegate:self];
    
    // init switch states
    [_recordedInputSwitch setOn:!_audioEngine->getMuteRecording()];
    [_synthInputSwitch setOn:!_audioEngine->getMuteSynth()];
    
    // init play/stop button
    if (_audioEngine->isStarted())
    {
        [_playButton setTitle: @"Stop" forState: UIControlStateNormal ];
        [_playButton setTitle: @"Stop" forState: UIControlStateHighlighted ];
    }
    else
    {
        [_playButton setTitle: @"Play" forState: UIControlStateNormal ];
        [_playButton setTitle: @"Play" forState: UIControlStateHighlighted ];
    }
    
    [super viewDidLoad];
}

// respond to a tap on the Play button. If stopped, start playing. If playing, stop.
- (IBAction) playOrStop: (id) sender {

	NSLog(@"playOrStop called:");
    
    if (_audioEngine->isStarted())
    {
        [_playButton setTitle: @"Play" forState: UIControlStateNormal ];
        [_playButton setTitle: @"Play" forState: UIControlStateHighlighted ];
        _audioEngine->stop();
    }
    else
    {
        [_playButton setTitle: @"Stop" forState: UIControlStateNormal ];
        [_playButton setTitle: @"Stop" forState: UIControlStateHighlighted ];
        _audioEngine->start();
    }
}

- (IBAction) recordedInputSwitchChanged: (id) sender 
{
	bool recordingIsOn = [_recordedInputSwitch isOn];
    if (recordingIsOn)
    {
        NSLog(@"recordedInputSwitchChanged unmuting recording");
        _audioEngine->setMuteRecording(false);
    }
    else
    {
        NSLog(@"recordedInputSwitchChanged muting recording");
        _audioEngine->setMuteRecording(true);
    }
}

- (IBAction) synthInputSwitchChanged: (id) sender 
{
	bool synthIsOn = [_synthInputSwitch isOn];
    if (synthIsOn)
    {
        NSLog(@"synthInputSwitchChanged unmuting synth");
        _audioEngine->setMuteSynth(false);
    }
    else
    {
        NSLog(@"synthInputSwitchChanged muting synth");
        _audioEngine->setMuteSynth(true);
    }
}

- (IBAction) ringModFreqSliderChanged: (id) sender
{
    int freq = (int)[_ringModFreqSlider value]; // for simplicity right now, use only integer values
    [_ringModFreqTextField setText:[NSString stringWithFormat:@"%d", freq]];
    _ringModFreqParam->setValue((float)freq);
}

- (IBAction) ampSliderChanged: (id) sender
{
    float amp = [_ampSlider value];
    [_ampTextField setText:[NSString stringWithFormat:@"%f", amp]];
    _ampParam->setValue(amp);
}

- (IBAction) waveformSelected: (id) sender
{
    NSLog(@"waveformSelected called:");
    _audioEngine->getSynth()->setWaveform((Oscillator:: Waveform)[_waveformSelector selectedSegmentIndex]);
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation 
{
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}


- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning]; // Releases the view if it doesn't have a superview
    // Release anything that's not essential, such as cached data
}

- (void)accelerometer:(UIAccelerometer *)accelerometer didAccelerate:(UIAcceleration *)acceleration
{
    double angleZY = atan2(acceleration.y, acceleration.z);
    //double angleXZ = atan2(acceleration.z, acceleration.x);
    //NSLog(@"accel: x = %2.2f, y = %2.2f, z = %2.2f, angleXZ = %2.2f, angleZY = %2.2f", acceleration.x, acceleration.y, acceleration.z, angleZY, angleXZ);
    
    // change amplitude based on rotation around X axis
    float amp = (angleZY / PI) >= 0 ? (angleZY / PI) : -(angleZY / PI);
    _ampParam->setValue(amp);
    [_ampTextField setText:[NSString stringWithFormat:@"%f", amp]];
    [_ampSlider setValue:amp animated:NO];
}

@end
