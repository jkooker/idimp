//
//  MainView.h
//  iDiMP
//
//  Created by John Kooker on 11/20/08.
//  Copyright Apple Inc 2008. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "AudioEngine.h"
#import "Synth.h"

@interface MainView : UIView {
    NSMutableArray* _touchPoints;
    Voice _voices[NUM_VOICES];
    int _numVoices;
    //AudioEngine* _audioEngine;
}

@property (readwrite, retain) NSMutableArray* _touchPoints;

@end
