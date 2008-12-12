//
//  NetworkViewController.h
//  iDiMP
//
//  Created by John Kooker on 12/10/08.
//  Copyright 2008 Apple Inc. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "NetworkController.h"

/**
 * NetworkViewController is the view controller for NetworkView.
 * It interfaces between the Network Tab and the NetworkController.
 */
@interface NetworkViewController : UIViewController {
    IBOutlet UITableView *networkTableView;
    UISwitch *serverSwitch;
    
    NetworkController *_networkController;
    NSMutableArray *services;
    NSInteger selectedServiceIndex;
}

@end
