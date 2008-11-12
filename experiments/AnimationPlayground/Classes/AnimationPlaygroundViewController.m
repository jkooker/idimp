//
//  AnimationPlaygroundViewController.m
//  AnimationPlayground
//
//  Created by John Kooker on 11/11/08.
//  Copyright Apple Inc 2008. All rights reserved.
//

#import "AnimationPlaygroundViewController.h"

@implementation AnimationPlaygroundViewController

- (IBAction)animate:(id)sender
{
    // clean up last animation
    [myViewIWantToAnimate removeFromSuperview];
    [myViewIWantToAnimate release];

    CGRect rect = CGRectMake(0.0f, 0.0f, 320.0f, 100.0f);
    myViewIWantToAnimate = [ [ UIView alloc ] initWithFrame: rect ]; // Make the view.
    [self.view addSubview:myViewIWantToAnimate];
    myViewIWantToAnimate.backgroundColor = [ UIColor blueColor ]; // Set the background color blue.

    /* Animate the view down 60 pixels */
    [ UIView beginAnimations: nil context: nil ]; // Tell UIView we're ready to start animations.
    [ UIView setAnimationCurve: UIViewAnimationCurveEaseInOut ];
    [ UIView setAnimationDuration: 1.0f ]; // Set the duration to 1 second.
    myViewIWantToAnimate.frame = CGRectMake(rect.origin.x, rect.origin.y + 60.0f, rect.size.width, rect.size.height); // Set the frame 60 pixels down.
    [ UIView commitAnimations ]; // Animate!
    
    // probably need to wait here until the last animation is complete.

    /* Animate the view back to it's original spot on the screen */
    [ UIView beginAnimations: nil context: nil ]; // Tell UIView we're ready to start animations.
    [ UIView setAnimationCurve: UIViewAnimationCurveEaseInOut ];
    [ UIView setAnimationDuration: 1.0f ]; // Set the duration to 1 second.
    myViewIWantToAnimate.frame = rect; // Set the frame to the original frame.
    [ UIView commitAnimations ]; // Animate!
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

@end
