//
//  MainViewController.m
//  iDiMP
//
//  Created by John Kooker on 11/20/08.
//  Copyright Apple Inc 2008. All rights reserved.
//

#import "MainViewController.h"
#import "MainView.h"

static const int kAccelerometerFrequency        = 25; //Hz
static const int kFilteringFactor               = 0.1;
static const int kMinShakeInterval              = 1;
static const int kShakeAccelerationThreshold    = 2.0;

@implementation MainViewController


- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil]) {
        // Custom initialization
        [[UIAccelerometer sharedAccelerometer] setUpdateInterval:(1.0 / kAccelerometerFrequency)];
        [[UIAccelerometer sharedAccelerometer] setDelegate:self];
    }
    return self;
}


/*
// Implement viewDidLoad to do additional setup after loading the view.
- (void)viewDidLoad {
    [super viewDidLoad];
}
*/


- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}


- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning]; // Releases the view if it doesn't have a superview
    // Release anything that's not essential, such as cached data
}


- (void)dealloc {
    [super dealloc];
}

#pragma mark Acceleration
// This can be moved if we decide that more than just the main view needs to detect shaking
- (void)accelerometer:(UIAccelerometer*)accelerometer didAccelerate:(UIAcceleration*)acceleration
{
    if ([self didShake:acceleration])
    {
        NSLog(@"Detected shake gesture.");
        // Cycle through waveforms
        TouchSynth *mainSynth = AudioEngine::getInstance()->getSynth();
        mainSynth->incrementWaveform();
    }
}

- (BOOL)didShake:(UIAcceleration*)acceleration
{
    UIAccelerationValue length, x, y, z;
    BOOL detectedShakeGesture = NO;
	
	// Use a basic high-pass filter to remove the influence of gravity
	_savedAcceleration[0] = acceleration.x * kFilteringFactor + _savedAcceleration[0] * (1.0 - kFilteringFactor);
	_savedAcceleration[1] = acceleration.y * kFilteringFactor + _savedAcceleration[1] * (1.0 - kFilteringFactor);
	_savedAcceleration[2] = acceleration.z * kFilteringFactor + _savedAcceleration[2] * (1.0 - kFilteringFactor);

	// Compute values for the three axes of the acceleromater
	x = acceleration.x - _savedAcceleration[0];
	y = acceleration.y - _savedAcceleration[0];
	z = acceleration.z - _savedAcceleration[0];
	
	// Compute the intensity of the current acceleration 
	length = sqrt(x * x + y * y + z * z);
    
	// If above a given threshold, return true
	if((length >= kShakeAccelerationThreshold) && (CFAbsoluteTimeGetCurrent() > _lastShakeTime + kMinShakeInterval)) {
        detectedShakeGesture = YES;
		_lastShakeTime = CFAbsoluteTimeGetCurrent();
	}
    
    return detectedShakeGesture;
}

@end
