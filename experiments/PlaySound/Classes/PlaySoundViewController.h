//
//  PlaySoundViewController.h
//  PlaySound
//
//  Created by John Kooker on 11/4/08.
//  Copyright Apple Inc 2008. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <AudioToolbox/AudioToolbox.h>

@interface PlaySoundViewController : UIViewController {
    SystemSoundID _soundID;
}

- (IBAction) playSound:(id)sender;

@end

