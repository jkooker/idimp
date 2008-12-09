//
//  NetThrashViewController.m
//  NetThrash
//
//  Created by John Kooker on 11/25/08.
//  Copyright Apple Inc 2008. All rights reserved.
//

#import "NetThrashViewController.h"
#import "AsyncUdpSocket.h"
#import "NetThrashAppDelegate.h"

NSString *headers[] = {
    @"Network",
    @"Pairing",
    @""
};

enum NetworkTableViewSections {
    NetworkSection = 0,
    PairingSection,
    NumberOfSections
};


// The Bonjour application protocol, which must:
// 1) be no longer than 14 characters
// 2) contain only lower-case letters, digits, and hyphens
// 3) begin and end with lower-case letter or digit
// It should also be descriptive and human-readable
// See the following for more information:
// http://developer.apple.com/networking/bonjour/faq.html
#define kDMPIdentifier      @"idimp"


@implementation NetThrashViewController

@synthesize savedAddress;

- (void) _showAlert:(NSString*)title
{
	UIAlertView* alertView = [[UIAlertView alloc] initWithTitle:title message:@"Check your networking configuration." delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
	[alertView show];
	[alertView release];
}

/*
// Override initWithNibName:bundle: to load the view using a nib file then perform additional customization that is not appropriate for viewDidLoad.
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil]) {
        // Custom initialization
    }
    return self;
}
*/

/*
// Implement loadView to create a view hierarchy programmatically.
- (void)loadView {
}
*/


// Implement viewDidLoad to do additional setup after loading the view.
- (void)viewDidLoad {
    [super viewDidLoad];
    
    //[self setupServer];
    
    // Advertise over Bonjour
    netService = [[NSNetService alloc] initWithDomain:@"local." type:@"_idimp._udp." name:@"" port:23711];
    if (netService == nil)
    {
        NSLog(@"error: could not initialize NetService");
    }
    else
    {
        [netService scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSRunLoopCommonModes];
        [netService publish];
        [netService setDelegate:self];
    }
    
    // Start searching for Bonjour services
    browser = [[NSNetServiceBrowser alloc] init];
    services = [[NSMutableArray array] retain];
    [browser setDelegate:self];
    [browser searchForServicesOfType:@"_idimp._udp." inDomain:@"local."];
}


- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}


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
    return 2;
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
        
    [tableView deselectRowAtIndexPath:indexPath animated:YES];
}

#pragma mark NSNetServiceBrowser Delegate Methods

- (void)netServiceBrowserWillSearch:(NSNetServiceBrowser *)aNetServiceBrowser
{
	//NSLog(@"%s", _cmd);
}

- (void)netServiceBrowser:(NSNetServiceBrowser *)aNetServiceBrowser didFindService:(NSNetService *)aNetService moreComing:(BOOL)moreComing
{
	//NSLog(@"netservice found: %@ %@", [aNetService name], moreComing ? @"(moreComing)" : @"");
    
    [services addObject:aNetService];
    [networkingTableView reloadData];
    [aNetService setDelegate:self];
    [aNetService resolveWithTimeout:5];
}

- (void)netServiceBrowser:(NSNetServiceBrowser *)aNetServiceBrowser didRemoveService:(NSNetService *)aNetService moreComing:(BOOL)moreComing
{
    BOOL foundService = NO;
    
    for (NSNetService *currentService in services)
    {
        if ([[currentService name] isEqual:[aNetService name]] &&
            [[currentService type] isEqual:[aNetService type]] &&
            [[currentService domain] isEqual:[aNetService domain]])
        {
            [services removeObject:currentService];
            [networkingTableView reloadData];
            foundService = YES;
            break;
        }
    }
    
	NSLog(@"netservice removed: %@ (%@)", [aNetService name], foundService ? @"successfully" : @"unsuccessfully");
}

#pragma mark NSNetService Delegate Methods

- (void)netService:(NSNetService *)sender didNotPublish:(NSDictionary *)errorDict
{
	NSLog(@"%s", _cmd);
}

- (void)netServiceDidPublish:(NSNetService *)sender
{
	NSLog(@"%s", _cmd);
}

- (void)netServiceDidResolveAddress:(NSNetService *)sender
{
    const char *firstAddressData = (const char *)[[[sender addresses] objectAtIndex:0] bytes];
    NSLog(@"address resolved. %@ = %@ (%hhu.%hhu.%hhu.%hhu) (1 of %d)",
        [sender name],
        [[sender addresses] objectAtIndex:0],
        firstAddressData[4], firstAddressData[5], firstAddressData[6], firstAddressData[7],
        [[sender addresses] count]);
}

- (void)netService:(NSNetService *)sender didNotResolve:(NSDictionary *)errorDict
{
    NSLog(@"did not resolve address for %@.", [sender name]);
}

@end
