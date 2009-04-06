//
//  AudioButtonTestViewController.mm
//  iDiMP
//
//  Created by Michelle Daniels on 11/2/08.
//  Copyright UCSD 2008. All rights reserved.
//

#import "AudioButtonTestViewController.h"

@implementation AudioButtonTestViewController

@synthesize _ringModFreqSlider;
@synthesize _ringModFreqTextField;
@synthesize _waveformSelector;

- (void) myInit
{   
    NSLog(@"AudioButtonTestViewController myInit");
    _audioEngine = NULL;
    _audioEngine = AudioEngine::getInstance();
    
    // init effects and map their parameters to sliders
    // at processing time, they will be called in the order they are added to the engine
    _ringModEffect = NULL;
    _ringModEffect = new RingMod();
    _audioEngine->addRecordingEffect(_ringModEffect);
    _ringModFreqParam = _ringModEffect->getParameter(0);
    
    // init the appropriate segment for waveform selection UISegmentedControl
    [_waveformSelector setSelectedSegmentIndex:_audioEngine->getSynth()->getWaveform()];
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
    [super dealloc];
}

// Implement viewDidLoad to do additional setup after loading the view.
- (void) viewDidLoad 
{
    NSLog(@"AudioButtonTestViewController viewDidLoad");
    [self myInit];
        
    float modFreq = _ringModFreqParam->getValue();
    [_ringModFreqTextField setText:[NSString stringWithFormat:@"%d", (int)modFreq]];
    [_ringModFreqSlider setValue:modFreq animated:NO];
    [_ringModFreqSlider setMinimumValue:_ringModFreqParam->getMinValue()];
    [_ringModFreqSlider setMaximumValue:_ringModFreqParam->getMaxValue()];
    
    [super viewDidLoad];
}

- (void) viewWillAppear: (BOOL) animated
{
    NSLog(@"AudioButtonTestViewController::viewWillAppear");
    // init the appropriate segment for waveform selection UISegmentedControl
    [_waveformSelector setSelectedSegmentIndex:_audioEngine->getSynth()->getWaveform()];
}

- (IBAction) ringModFreqSliderChanged: (id) sender
{
    int freq = (int)[_ringModFreqSlider value]; // for simplicity right now, use only integer values
    [_ringModFreqTextField setText:[NSString stringWithFormat:@"%d", freq]];
    _ringModFreqParam->setValue((float)freq);
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

@end