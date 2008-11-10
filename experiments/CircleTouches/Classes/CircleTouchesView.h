//
//  CircleTouchesView.h
//  CircleTouches
//
//  Created by John Kooker on 11/10/08.
//  Copyright 2008 Apple Inc. All rights reserved.
//

#import <UIKit/UIKit.h>


@interface CircleTouchesView : UIView {
    NSMutableArray *touchPoints;
}

@property (readwrite, retain) NSMutableArray *touchPoints;

@end
