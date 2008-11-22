//
//  FlipsideViewController.m
//  iDiMP
//
//  Created by John Kooker on 11/20/08.
//  Copyright Apple Inc 2008. All rights reserved.
//

#import "FlipsideViewController.h"


@implementation FlipsideViewController

@synthesize tabBarController;


- (void)viewDidLoad {
    [super viewDidLoad];
    self.view = tabBarController.view;
    self.view.backgroundColor = [UIColor viewFlipsideBackgroundColor];
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
