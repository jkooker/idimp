//
//  NetworkViewController.m
//  iDiMP
//
//  Created by John Kooker on 12/10/08.
//  Copyright 2008 Apple Inc. All rights reserved.
//

#import "NetworkViewController.h"

NSString *headers[] = {
    @"",             // NetworkSection
    @"Audio Destination",   // PairingSection
    @""
};

enum NetworkTableViewSections {
    NetworkSection = 0,
    PairingSection,
    NumberOfSections
};


@implementation NetworkViewController

/*
// The designated initializer. Override to perform setup that is required before the view is loaded.
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil]) {
        // Custom initialization
    }
    return self;
}
*/

/*
// Implement loadView to create a view hierarchy programmatically, without using a nib.
- (void)loadView {
}
*/


// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
    [super viewDidLoad];
    
    selectedServiceIndex = -1;
    
    _networkController = [NetworkController sharedInstance];
    services = _networkController.services;
    _networkController.clientTableView = networkTableView;
    [_networkController startBonjourSearch];
    
    serverSwitch = [[UISwitch alloc] initWithFrame:CGRectZero];
    [serverSwitch addTarget:self action:@selector(serverDidSwitch) forControlEvents:UIControlEventValueChanged];
}


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
    [serverSwitch release];
    [super dealloc];
}

- (void)serverDidSwitch
{
    NSLog(@"%@ %s", [self class], _cmd);
    
    // get new value of serverswitch
    if ([serverSwitch isOn])
    {
        [_networkController startAudioServer];
    }
    else
    {
        [_networkController stopAudioServer];
    }
}

- (void)viewWillAppear:(BOOL)animated
{
    [[NetworkController sharedInstance] startBonjourSearch];
}

- (void)viewWillDisappear:(BOOL)animated
{
    [[NetworkController sharedInstance] stopBonjourSearch];
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
                theCell.text = @"Network";
                theCell.accessoryType = UITableViewCellAccessoryNone;
                
                // add switch
                theCell.accessoryView = serverSwitch;
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
                // put in the "waiting" label
                // TODO: change this to "searching" when appropriate
                theCell.text = @"Waiting...";
                theCell.textColor = [UIColor colorWithWhite:0.5 alpha:0.5];
            }
            else
            {
                // show all the available services
                theCell.text = [[services objectAtIndex:indexPath.row] name];
                theCell.textColor = ([theCell.text isEqualToString:[[UIDevice currentDevice] name]]) ? [UIColor colorWithWhite:0.5 alpha:0.5] : [UIColor blackColor];
                theCell.accessoryType = (selectedServiceIndex == indexPath.row) ? UITableViewCellAccessoryCheckmark : UITableViewCellAccessoryNone;
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
    NSLog(@"%@ %s", [self class], _cmd);
        
    if (![[[services objectAtIndex:indexPath.row] addresses] count])
    {
        NSLog(@"oops, can't send because we haven't resolved addresses for this service");
    }
    else
    {
        _networkController.savedAddress = [[[services objectAtIndex:indexPath.row] addresses] objectAtIndex:0];
        // add a checkmark to this one
        if (selectedServiceIndex != -1)
        {
            [tableView cellForRowAtIndexPath:[NSIndexPath indexPathForRow:selectedServiceIndex inSection:indexPath.section]].accessoryType = UITableViewCellAccessoryNone;
        }
        selectedServiceIndex = indexPath.row;
        [tableView cellForRowAtIndexPath:indexPath].accessoryType = UITableViewCellAccessoryCheckmark;
        
#if 1
        // debug: send a packet (this seems to be necessary to kick off the socket)
        short *buffer = new short[10];
        [_networkController sendAudioBuffer:buffer length:10 channels:1];
        delete buffer;
#endif
    }
    
    //[sharedSocket connectToAddress:[[[services objectAtIndex:indexPath.row] addresses] objectAtIndex:0] error:&error];
    
    [tableView deselectRowAtIndexPath:indexPath animated:YES];
}


@end
