//
//  NetworkViewController.h
//  iDiMP
//
//  Created by John Kooker on 12/10/08.
//  Copyright 2008 Apple Inc. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "NetworkController.h"

@interface NetworkViewController : UIViewController {
    IBOutlet UITableView *networkTableView;
    
    NetworkController *_networkController;
    NSMutableArray *services;
}

@end
