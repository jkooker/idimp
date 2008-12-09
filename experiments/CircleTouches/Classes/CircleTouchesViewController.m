//
//  CircleTouchesViewController.m
//  CircleTouches
//
//  Created by John Kooker on 11/10/08.
//  Copyright Apple Inc 2008. All rights reserved.
//

#import "CircleTouchesViewController.h"

static const float kAccelerometerInterval = 0.1;

@implementation CircleTouchesViewController

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
    [super viewDidLoad];
    
    // Set up accelerometer
    [[UIAccelerometer sharedAccelerometer] setUpdateInterval:kAccelerometerInterval]; // in seconds
    [[UIAccelerometer sharedAccelerometer] setDelegate:self];
}



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

- (void)accelerometer:(UIAccelerometer *)accelerometer didAccelerate:(UIAcceleration *)acceleration
{
    // change view's background color based on z value.
    //NSLog(@"z accel: %f", acceleration.z);
    
    self.view.backgroundColor = [UIColor colorWithRed:(acceleration.z + 1.2)/1.2
                                                green:0
                                                 blue:0 
                                                alpha:1];
}


@end