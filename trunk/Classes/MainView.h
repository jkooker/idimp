//
//  MainView.h
//  iDiMP
//
//  Created by John Kooker on 11/20/08.
//  Copyright Apple Inc 2008. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface MainView : UIView {
    NSMutableArray* _touchPoints;
}

@property (readwrite, retain) NSMutableArray* _touchPoints;

@end
