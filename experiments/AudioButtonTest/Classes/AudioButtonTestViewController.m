//
//  AudioButtonTestViewController.m
//  AudioButtonTest
//
//  Created by Michelle Daniels on 11/2/08.
//  Copyright UCSD 2008. All rights reserved.
//

#import "AudioButtonTestViewController.h"
//#import "AudioQueueWrapper.h"

static const float kAccelerometerInterval = 0.01;

@implementation AudioButtonTestViewController

@synthesize _playButton;
@synthesize _recordButton;
@synthesize _playIsOn;
@synthesize _recordIsOn;
@synthesize _frequencySlider;
@synthesize _frequencyTextField;
@synthesize _ringModFreqSlider;
@synthesize _ringModFreqTextField;
@synthesize _ampSlider;
@synthesize _ampTextField;
@synthesize _waveformSelector;

- (void) myInit
{   
    NSLog(@"AudioButtonTestViewController myInit");
    _audioQueue = NULL;
    _audioQueue = new AudioQueueWrapper();}

- (void) dealloc
{   
    NSLog(@"AudioButtonTestViewController dealloc");
    if (_audioQueue != NULL)
    {
        delete _audioQueue;
        _audioQueue = NULL;
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
    float freq = _audioQueue->m_osc.getFreq();
    [_frequencyTextField setText:[[NSString alloc] initWithFormat:@"%d", (int)freq]];
    [_frequencySlider setValue:freq animated:NO];
    
    float modFreq = _audioQueue->m_ringModEffect->getModFreq();
    [_ringModFreqTextField setText:[[NSString alloc] initWithFormat:@"%d", (int)modFreq]];
    [_ringModFreqSlider setValue:modFreq animated:NO];
    
    float amp = _audioQueue->m_ampEffect->getAmp();
    [_ampTextField setText:[[NSString alloc] initWithFormat:@"%f", amp]];
    [_ampSlider setValue:amp animated:NO];
    
     // Set up accelerometer
    [[UIAccelerometer sharedAccelerometer] setUpdateInterval:kAccelerometerInterval]; // in seconds
    [[UIAccelerometer sharedAccelerometer] setDelegate:self];
    
    [super viewDidLoad];
}

// respond to a tap on the Play button. If stopped, start playing. If playing, stop.
- (IBAction) playOrStop: (id) sender {

	NSLog(@"playOrStop called:");
    
    if (_playIsOn)
    {
        [_playButton setTitle: @"Play" forState: UIControlStateNormal ];
        [_playButton setTitle: @"Play" forState: UIControlStateHighlighted ];
        _audioQueue->pause();
    }
    else
    {
        [_playButton setTitle: @"Stop" forState: UIControlStateNormal ];
        [_playButton setTitle: @"Stop" forState: UIControlStateHighlighted ];
        _audioQueue->play();
    }
    
    _playIsOn = !_playIsOn;
}

// respond to a tap on the Record button. If stopped, start recording. If recording, stop.
- (IBAction) recordOrStop: (id) sender {

	NSLog(@"recordOrStop called:");
    
    if (_recordIsOn)
    {
        [_recordButton setTitle: @"Record" forState: UIControlStateNormal ];
        [_recordButton setTitle: @"Record" forState: UIControlStateHighlighted ];
    }
    else
    {
        [_recordButton setTitle: @"Stop" forState: UIControlStateNormal ];
        [_recordButton setTitle: @"Stop" forState: UIControlStateHighlighted ];
    }
    
    _recordIsOn = !_recordIsOn;
}

- (IBAction) frequencySliderChanged: (id) sender
{
    int freq = (int)[_frequencySlider value]; // for simplicity right now, use only integer values
    [_frequencyTextField setText:[[NSString alloc] initWithFormat:@"%d", freq]];
    _audioQueue->m_osc.setFreq((float)freq);
}

- (IBAction) ringModFreqSliderChanged: (id) sender
{
    int freq = (int)[_ringModFreqSlider value]; // for simplicity right now, use only integer values
    [_ringModFreqTextField setText:[[NSString alloc] initWithFormat:@"%d", freq]];
    _audioQueue->m_ringModEffect->setModFreq((float)freq);
}

- (IBAction) ampSliderChanged: (id) sender
{
    float amp = [_ampSlider value];
    [_ampTextField setText:[[NSString alloc] initWithFormat:@"%f", amp]];
    _audioQueue->m_ampEffect->setAmp(amp);
}

- (IBAction) waveformSelected: (id) sender
{
    NSLog(@"waveformSelected called:");
    _audioQueue->m_osc.setWaveform((Oscillator:: Waveform)[_waveformSelector selectedSegmentIndex]);
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
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
    _audioQueue->m_ampEffect->setAmp(amp);
    [_ampTextField setText:[[NSString alloc] initWithFormat:@"%f", amp]];
    [_ampSlider setValue:amp animated:NO];
    
    //_audioQueue->m_ringModEffect->setModAmp((angleZY / PI) >= 0 ? (angleZY / PI) : -(angleZY / PI));
    
    //_audioQueue->m_ringModEffect->setModFreq(1000 * ((angleXZ / PI) >= 0 ? (angleXZ / PI) : -(angleXZ / PI)));
}

@end
