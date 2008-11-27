//
//  MainViewController.h
//  iDiMP
//
//  Created by John Kooker on 11/20/08.
//  Copyright Apple Inc 2008. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface MainViewController : UIViewController <UIAccelerometerDelegate> {
    UIAccelerationValue	_savedAcceleration[3]; // in three dimensions
    CFTimeInterval		_lastShakeTime;
}

- (BOOL) didShake:(UIAcceleration*)acceleration;

@end
