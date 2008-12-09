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
    UDPServer *server;
    NSNetService *netService;
    NSNetServiceBrowser *browser;
    NSMutableArray *services;
    
    IBOutlet UITableView *networkingTableView;
}

- (void)setupServer;

@end

