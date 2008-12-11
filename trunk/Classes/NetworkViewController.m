//
//  NetworkViewController.m
//  iDiMP
//
//  Created by John Kooker on 12/10/08.
//  Copyright 2008 Apple Inc. All rights reserved.
//

#import "NetworkViewController.h"

NSString *headers[] = {
    @"Network",
    @"Audio Destination",
    @""
};

enum NetworkTableViewSections {
    NetworkSection = 0,
    PairingSection,
    NumberOfSections
};


@implementation NetworkViewController

// The designated initializer. Override to perform setup that is required before the view is loaded.
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil]) {
        // Custom initialization
        services = [NetworkController sharedInstance].services;
    }
    return self;
}

/*
// Implement loadView to create a view hierarchy programmatically, without using a nib.
- (void)loadView {
}
*/

/*
// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
    [super viewDidLoad];
}
*/

/*
// Override to allow orientations other than the default portrait orientation.
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}
*/

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning]; // Releases the view if it doesn't have a superview
    // Release anything that's not essential, such as cached data
}


- (void)dealloc {
    [super dealloc];
}

#pragma mark TableView Data Source Methods

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return NumberOfSections;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    NSInteger retVal = 0;
    if (section == NetworkSection) retVal = 1;
    if (section == PairingSection) retVal = [services count] ? [services count] : 1;

    return retVal;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    static NSString *serverSwitchCellID = @"serverSwitch";
    static NSString *pairingItemCellID = @"pairingItem";
    
    UITableViewCell *theCell = nil;
    
    switch (indexPath.section) {
        case NetworkSection:
            theCell = [tableView dequeueReusableCellWithIdentifier:serverSwitchCellID];
            if (!theCell)
            {
                theCell = [[UITableViewCell alloc] initWithFrame:CGRectZero reuseIdentifier:serverSwitchCellID];
                theCell.text = @"Server";
                theCell.accessoryType = UITableViewCellAccessoryNone;
                
                // add switch
                UISwitch *serverSwitch = [[UISwitch alloc] initWithFrame:CGRectZero];
                theCell.accessoryView = serverSwitch;
                [serverSwitch release];
            }
            break;
        case PairingSection:
            theCell = [tableView dequeueReusableCellWithIdentifier:pairingItemCellID];
            if (!theCell)
            {
                theCell = [[UITableViewCell alloc] initWithFrame:CGRectZero reuseIdentifier:pairingItemCellID];
            }
            
            if (![services count])
            {
                // put in the "searching" label
                theCell.text = @"Searching...";
                theCell.textColor = [UIColor colorWithWhite:0.5 alpha:0.5];
            }
            else
            {
                // show all the available services
                theCell.text = [[services objectAtIndex:[indexPath row]] name];
                theCell.textColor = [UIColor blackColor];
                theCell.accessoryType = UITableViewCellAccessoryNone;
            }
            break;
        default:
            break;
    }

    return theCell;
}

- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section
{
    return headers[section];
}

#pragma mark TableView Delegate Methods

- (NSIndexPath *)tableView:(UITableView *)tableView willSelectRowAtIndexPath:(NSIndexPath *)indexPath {
	// Ignore the selection if there are no services.
	if ([services count] == 0)
		return nil;
    
    // The Network Section just contains the server switch. It doesn't allow selections.
    if (indexPath.section == NetworkSection)
        return nil;

	return indexPath;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
	NSLog(@"%s", _cmd);
    
#if 0
    
    if (![[[services objectAtIndex:indexPath.row] addresses] count])
        NSLog(@"oops, can't send because we haven't resolved addresses for this service");
    
    AsyncUdpSocket *sharedSocket = ((NetThrashAppDelegate *)[UIApplication sharedApplication].delegate).socket;
    //[sharedSocket connectToAddress:[[[services objectAtIndex:indexPath.row] addresses] objectAtIndex:0] error:&error];
    
    // send a starter packet
    DMPDataPacket p;
    memset(&p, 0, sizeof(p));
    p.tag = DMPDataPacketTagStart;
    
    self.savedAddress = [[[services objectAtIndex:indexPath.row] addresses] objectAtIndex:0];
    
    [sharedSocket sendData:[NSData dataWithBytes:&p length:sizeof(p)]
        toAddress:self.savedAddress
        withTimeout:5
        tag:DMPDataPacketTagStart];
#endif
        
    [tableView deselectRowAtIndexPath:indexPath animated:YES];
}


@end
