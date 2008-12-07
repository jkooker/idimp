//
//  NetThrashViewController.m
//  NetThrash
//
//  Created by John Kooker on 11/25/08.
//  Copyright Apple Inc 2008. All rights reserved.
//

#import "NetThrashViewController.h"

NSString *headers[] = {
    @"Network",
    @"Pairing",
    @""
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

- (void) _showAlert:(NSString*)title
{
	UIAlertView* alertView = [[UIAlertView alloc] initWithTitle:title message:@"Check your networking configuration." delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
	[alertView show];
	[alertView release];
}

- (void)setupServer
{
    [server release];
    server = nil;
    
    server = [UDPServer new];
    [server setDelegate:self];
    
    NSError* error;
	if(server == nil || ![server start:&error]) {
		NSLog(@"Failed creating server: %@", error);
		[self _showAlert:@"Failed creating server"];
		return;
	}
	
	//Start advertising to clients, passing nil for the name to tell Bonjour to pick use default name
	if(![server enableBonjourWithDomain:@"local" applicationProtocol:[UDPServer bonjourTypeFromIdentifier:kDMPIdentifier] name:nil]) {
		[self _showAlert:@"Failed advertising server"];
		return;
	}

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
    
    [self setupServer];
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
    return 3;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return 2;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    UITableViewCell *theCell = [tableView dequeueReusableCellWithIdentifier:@"junk"];
    if (!theCell)
    {
        theCell = [[UITableViewCell alloc] initWithFrame:CGRectMake(0, 0, 0, 0) reuseIdentifier:@"junk"];
        theCell.text = @"hi there.";
        theCell.accessoryType = UITableViewCellAccessoryCheckmark;
    }
    return theCell;
}

- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section
{
    return headers[section];
}

#pragma mark UDPServer Delegate Methods

- (void) serverDidEnableBonjour:(UDPServer*)server withName:(NSString*)string
{
	NSLog(@"%s", _cmd);
}

- (void)didAcceptConnectionForServer:(UDPServer*)server inputStream:(NSInputStream *)istr outputStream:(NSOutputStream *)ostr
{
	NSLog(@"%s", _cmd);
}


@end
