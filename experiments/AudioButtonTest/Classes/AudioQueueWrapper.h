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
    AmplitudeScale* m_ampEffect;
    RingMod* m_ringModEffect;
    
    AudioQueueWrapper();
    virtual ~AudioQueueWrapper();
        
    int InitPlayback();    
    void InitRecording();
    void InitDebugFile();    
    void PlaybackCallback(AudioQueueRef inQ, AudioQueueBufferRef outQB);
    void RecordingCallback(AudioQueueRef inAudioQueue,
                           AudioQueueBufferRef inBuffer,
                           const AudioTimeStamp *inStartTime,
                           UInt32 inNumPackets,
                           const AudioStreamPacketDescription *inPacketDesc);
    
    bool outputIsRunning() const;    
    bool inputIsRunning() const;    
    void play();    
    void stop();    
    void pause();    
    void resume();
    void recordStart();
    void recordStop();
    void recordPause();
        
protected:

    bool is_running(AudioQueueRef queue) const;    
    
    AudioQueueRef m_audioQueueOutput;
    AudioQueueRef m_audioQueueInput;
    AudioStreamBasicDescription m_dataFormat;
    UInt32 m_samplesPerFramePerChannel;
    bool m_audioPlayerShouldStopImmediately;
    AudioQueueBufferRef m_outputBuffers[NUM_AUDIO_DATA_BUFFERS];
    AudioFileID m_debugFileID;
    SInt64 m_debugFileByteOffset;
};

#endif // AUDIO_QUEUE_WRAPPER_H