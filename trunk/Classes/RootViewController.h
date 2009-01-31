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
//  RootViewController.h
//  iDiMP
//
//  Created by John Kooker on 11/20/08.
//

#import <UIKit/UIKit.h>

@class MainViewController;
@class FlipsideViewController;

/**
 * Controls switching between MainView and FlipsideView.
 */
@interface RootViewController : UIViewController {

    UIButton *infoButton;
    MainViewController *mainViewController;
    FlipsideViewController *flipsideViewController;
    UINavigationBar *flipsideNavigationBar;
}

/**
 * Triggers the switch between MainView and FlipsideView.
 */
@property (nonatomic, retain) IBOutlet UIButton *infoButton;
/**
 * Instantiated by RootViewController.
 */
@property (nonatomic, retain) MainViewController *mainViewController;
/**
 * Instantiated by RootViewController, and tacked onto FlipsideView.
 */
@property (nonatomic, retain) UINavigationBar *flipsideNavigationBar;
/**
 * Instantiated by RootViewController.
 */
@property (nonatomic, retain) FlipsideViewController *flipsideViewController;

/**
 * Called by infoButton and Done button on the backside. Flips between the two views.
 */
- (IBAction)toggleView;

@end
