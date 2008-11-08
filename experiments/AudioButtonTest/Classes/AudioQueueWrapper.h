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

static const int NUM_AUDIO_DATA_BUFFERS = 3;
static const int BYTES_PER_SAMPLE = 2;

class AudioQueueWrapper 
{
public:

    AudioQueueWrapper();
    virtual ~AudioQueueWrapper();
        
    int InitPlayback();    
    void AQBufferCallback(AudioQueueRef inQ, AudioQueueBufferRef outQB);
    
    bool isRunning() const;    
    void play();    
    void stop();    
    void pause();    
    void resume();
        
protected:
    AudioQueueRef m_audioQueue;
    AudioStreamBasicDescription m_dataFormat;
    UInt32 m_samplesPerFramePerChannel;
    bool m_audioPlayerShouldStopImmediately;
    AudioQueueBufferRef m_buffers[NUM_AUDIO_DATA_BUFFERS];
    Oscillator m_osc;
};

#endif // AUDIO_QUEUE_WRAPPER_H