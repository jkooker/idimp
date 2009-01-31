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
//  FlipsideViewController.m
//  iDiMP
//
//  Created by John Kooker on 11/20/08.
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
    [currentViewController viewWillAppear:animated];
}

- (void)viewWillDisappear:(BOOL)animated
{
    [currentViewController viewWillDisappear:animated];
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
