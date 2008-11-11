/*
 *  AudioQueueWrapper.h
 *  AudioButtonTest
 *
 *  Created by Michelle Daniels on 11/2/08.
 *  Copyright 2008 UCSD. All rights reserved.
 *
 */

#ifndef AUDIO_QUEUE_WRAPPER_H
#define AUDIO_QUEUE_WRAPPER_H

#import <math.h>
#import <stdlib.h>
#import <AudioToolbox/AudioToolbox.h>
#import "Oscillator.h"
#import "AudioEffect.h"

static const int NUM_AUDIO_DATA_BUFFERS = 3;
static const int BYTES_PER_SAMPLE = 2;

class AudioQueueWrapper 
{
public:
    Oscillator m_osc;
    AmplitudeScale* m_effect;
    
    AudioQueueWrapper();
    virtual ~AudioQueueWrapper();
        
    int InitPlayback();    
    void InitDebugFile();    
    void PlaybackCallback(AudioQueueRef inQ, AudioQueueBufferRef outQB);
    
    bool isRunning() const;    
    void play();    
    void stop();    
    void pause();    
    void resume();
        
protected:
    AudioQueueRef m_audioQueueOutput;
    AudioStreamBasicDescription m_dataFormat;
    UInt32 m_samplesPerFramePerChannel;
    bool m_audioPlayerShouldStopImmediately;
    AudioQueueBufferRef m_buffers[NUM_AUDIO_DATA_BUFFERS];
    AudioFileID m_debugFileID;
    SInt64 m_debugFileByteOffset;
};

#endif // AUDIO_QUEUE_WRAPPER_H