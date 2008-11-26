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
    Voice* _voices;
    int _numVoices;
    AudioEngine* _audioEngine;
}

@end
