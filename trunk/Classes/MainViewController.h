//
//  MainViewController.h
//  iDiMP
//
//  Created by John Kooker on 11/20/08.
//  Copyright Apple Inc 2008. All rights reserved.
//

#import <UIKit/UIKit.h>

/**
 * View controller for MainView. Handles accelerometer data.
 */
@interface MainViewController : UIViewController <UIAccelerometerDelegate> {
    UIAccelerationValue	_savedAcceleration[3]; // in three dimensions
    CFTimeInterval		_lastShakeTime;
}

/**
 * Tests accelerometer input to determine if a shake gesture occurred.
 * @return Whether a shake gesture occurred.
 */
- (BOOL) didShake:(UIAcceleration*)acceleration;

@end
