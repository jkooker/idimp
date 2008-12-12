//
//  FlipsideViewController.h
//  iDiMP
//
//  Created by John Kooker on 11/20/08.
//  Copyright Apple Inc 2008. All rights reserved.
//

#import <UIKit/UIKit.h>

/**
 * View Controller for FlipsideView.
 * Holds a UITabBarController, which comprises the main part of the view.
 * Provides viewDidAppear/Disappear notifications to tabs' view controllers.
 */
@interface FlipsideViewController : UIViewController <UITabBarControllerDelegate> {
    UITabBarController *tabBarController;
    UIViewController *currentViewController;
}

/**
 * Tab Bar Controller instantiated in XIB.
 */
@property (nonatomic, retain) IBOutlet UITabBarController *tabBarController;

@end
