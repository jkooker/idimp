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
    TouchSynth* _synth; // TODO: maybe the synth should be owned by this class and added to the audio engine explicitly?
    AudioEngine* _audioEngine;
}

@end
