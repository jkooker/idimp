//
//  FlipsideViewController.m
//  iDiMP
//
//  Created by John Kooker on 11/20/08.
//  Copyright Apple Inc 2008. All rights reserved.
//

#import "FlipsideViewController.h"
#import "AudioEngine.h"


@implementation FlipsideViewController

@synthesize tabBarController;

- (void)viewDidLoad {
    [super viewDidLoad];
    self.view = tabBarController.view;
    self.view.backgroundColor = [UIColor viewFlipsideBackgroundColor];
    
    currentViewController = nil;
    
    // This should be suitably late to connect to the network controller.
    AudioEngine::getInstance()->connectToNetworkController();
}

- (void)viewWillAppear:(BOOL)animated
{
}

- (void)viewWillDisappear:(BOOL)animated
{
}

- (void)tabBarController:(UITabBarController *)tabBarController didSelectViewController:(UIViewController *)viewController
{
    NSLog(@"%@ %s", [self class], _cmd);
    
    if (currentViewController)
    {
        [currentViewController viewWillDisappear:NO];
    }
    
    [viewController viewWillAppear:NO];
    currentViewController = viewController;
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


@end
