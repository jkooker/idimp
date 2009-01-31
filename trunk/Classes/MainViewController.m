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

//
//  MainViewController.m
//  iDiMP
//
//  Created by John Kooker on 11/20/08.
//

#import "MainViewController.h"
#import "MainView.h"
#import "AudioEngine.h"

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
    // change amplitude based on rotation around X axis
    double angleZY = atan2(acceleration.y, acceleration.z);
    float amp = (angleZY / PI) >= 0 ? (angleZY / PI) : -(angleZY / PI);
    AudioEffect* e = AudioEngine::getInstance()->getMasterEffect(0);
    if (e != NULL)
    {
        e->getParameter(0)->setValue(amp);
    }
    else
    {
        NSLog(@"MainViewController::accelerometer master effect was NULL!!");
    }
    
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
