//
//  AudioButtonTestViewController.m
//  AudioButtonTest
//
//  Created by Michelle Daniels on 11/2/08.
//  Copyright UCSD 2008. All rights reserved.
//

#import "AudioButtonTestViewController.h"
//#import "AudioQueueWrapper.h"

@implementation AudioButtonTestViewController

@synthesize playButton;
@synthesize recordButton;
@synthesize playIsOn;
@synthesize recordIsOn;

- (void) myInit
{   
    NSLog(@"AudioButtonTestViewController init");
    //if ((self = [super init])) 
    //{
    m_audioQueue = NULL;
    m_audioQueue = new AudioQueueWrapper();
    m_audioQueue->InitPlayback();
    //}
    //return self;
    
}

- (void) dealloc
{   
    NSLog(@"AudioButtonTestViewController dealloc");
    if (m_audioQueue != NULL)
    {
        delete m_audioQueue;
        m_audioQueue = NULL;
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


/*
// Implement viewDidLoad to do additional setup after loading the view.
- (void)viewDidLoad {
    [super viewDidLoad];
}
*/

// respond to a tap on the Play button. If stopped, start playing. If playing, stop.
- (IBAction) playOrStop: (id) sender {

	NSLog (@"playOrStop called:");
    
    if (playIsOn)
    {
        [self.playButton setTitle: @"Play" forState: UIControlStateNormal ];
        [self.playButton setTitle: @"Play" forState: UIControlStateHighlighted ];
        m_audioQueue->pause();
    }
    else
    {
        [self.playButton setTitle: @"Stop" forState: UIControlStateNormal ];
        [self.playButton setTitle: @"Stop" forState: UIControlStateHighlighted ];
        m_audioQueue->play();
    }
    
    playIsOn = !playIsOn;
}

// respond to a tap on the Record button. If stopped, start recording. If recording, stop.
- (IBAction) recordOrStop: (id) sender {

	NSLog (@"recordOrStop called:");
    
    if (recordIsOn)
    {
        [self.recordButton setTitle: @"Record" forState: UIControlStateNormal ];
        [self.recordButton setTitle: @"Record" forState: UIControlStateHighlighted ];
    }
    else
    {
        [self.recordButton setTitle: @"Stop" forState: UIControlStateNormal ];
        [self.recordButton setTitle: @"Stop" forState: UIControlStateHighlighted ];
    }
    
    recordIsOn = !recordIsOn;
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}


- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning]; // Releases the view if it doesn't have a superview
    // Release anything that's not essential, such as cached data
}

@end
