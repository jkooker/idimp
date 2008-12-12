//
//  FlipsideViewController.h
//  iDiMP
//
//  Created by John Kooker on 11/20/08.
//  Copyright Apple Inc 2008. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface FlipsideViewController : UIViewController <UITabBarControllerDelegate> {
    UITabBarController *tabBarController;
    UIViewController *currentViewController;
}

@property (nonatomic, retain) IBOutlet UITabBarController *tabBarController;

@end
