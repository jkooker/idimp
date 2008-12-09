//
//  NetThrashViewController.h
//  NetThrash
//
//  Created by John Kooker on 11/25/08.
//  Copyright Apple Inc 2008. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "UDPServer.h"

@interface NetThrashViewController : UIViewController <UDPServerDelegate> {
    NSNetService *netService;
    NSNetServiceBrowser *browser;
    NSMutableArray *services;
    NSData *savedAddress;
    
    IBOutlet UITableView *networkingTableView;
}

@property (nonatomic, retain) NSData *savedAddress;

@end

