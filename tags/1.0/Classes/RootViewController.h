//
//  RootViewController.h
//  iDiMP
//
//  Created by John Kooker on 11/20/08.
//  Copyright Apple Inc 2008. All rights reserved.
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
