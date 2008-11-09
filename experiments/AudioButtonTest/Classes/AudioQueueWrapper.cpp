/*
 *  AudioQueueWrapper.cpp
 *  AudioButtonTest
 *
 *  Created by Michelle Daniels on 11/2/08.
 *  Copyright 2008 UCSD. All rights reserved.
 *
 */

#import "AudioQueueWrapper.h"
#import <stdio.h>
#import <stdlib.h>
#import <unistd.h>
#import <sys/types.h>

static const int NUM_CHANNELS = 2;
static const int MAX_AMPLITUDE = 32767;

static void AQBufferCallbackFunction(void* in, AudioQueueRef inQ, AudioQueueBufferRef outQB)
{
    //printf("AQBufferCallbackFunction\n");
    AudioQueueWrapper* aq = (AudioQueueWrapper*) in;
    aq->AQBufferCallback(inQ, outQB);
}

AudioQueueWrapper::AudioQueueWrapper() :
    m_samplesPerFramePerChannel(512),
    m_audioPlayerShouldStopImmediately(true)
{
    printf("AudioQueueWrapper::AudioQueueWrapper\n");
    m_dataFormat.mSampleRate = SAMPLE_RATE;
    m_dataFormat.mFormatID = kAudioFormatLinearPCM;
    m_dataFormat.mFormatFlags = kLinearPCMFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
    m_dataFormat.mBytesPerPacket = 4;
    m_dataFormat.mFramesPerPacket = 1;
    m_dataFormat.mBytesPerFrame = 4; // 2 2-byte samples per channel
    m_dataFormat.mChannelsPerFrame = 2;
    m_dataFormat.mBitsPerChannel = 16;
}

AudioQueueWrapper::~AudioQueueWrapper() 
{
    printf("AudioQueueWrapper::~AudioQueueWrapper\n");
    if (m_audioQueue != NULL)
    {
        AudioQueueDispose(m_audioQueue, TRUE);
    }
}

int AudioQueueWrapper::InitPlayback()
{
    printf("AudioQueueWrapper::InitPlayback\n");
    UInt32 err = AudioQueueNewOutput(&m_dataFormat, 
                                     AQBufferCallbackFunction,
                                     this,
                                     NULL,
                                     kCFRunLoopCommonModes,
                                     0,
                                     &m_audioQueue);
    if (err) return err;
    
    UInt32 bufferSize = m_samplesPerFramePerChannel * m_dataFormat.mBytesPerFrame;
    for (int i = 0; i < NUM_AUDIO_DATA_BUFFERS; i++)
    {
        err = AudioQueueAllocateBuffer(m_audioQueue, bufferSize, &m_buffers[i]);
        printf("allocated buffer %d\n", i);
        if (err) return err;
        AQBufferCallback(m_audioQueue, m_buffers[i]);
    }
    printf("AudioQueueWrapper::InitPlayback finished\n");
    return 0;
}

void AudioQueueWrapper::AQBufferCallback(AudioQueueRef inQ, AudioQueueBufferRef outQB)
{
    //printf("AudioQueueWrapper::AQBufferCallback\n");
    short* coreAudioBuffer = (short*) outQB->mAudioData;

    if (m_samplesPerFramePerChannel > 0)
    {
        outQB->mAudioDataByteSize = 4 * m_samplesPerFramePerChannel;
        m_osc.nextSampleBuffer(coreAudioBuffer, m_samplesPerFramePerChannel, NUM_CHANNELS, MAX_AMPLITUDE);
    }
    AudioQueueEnqueueBuffer(inQ, outQB, 0, NULL);
}

bool AudioQueueWrapper::isRunning() const
{
    UInt32 isRunning;
    UInt32 propertySize = sizeof(UInt32);
    OSStatus result = AudioQueueGetProperty( m_audioQueue, 
                                             kAudioQueueProperty_IsRunning, 
                                             &isRunning, 
                                             &propertySize);
    
    if (result != noErr)
    {
        return false;
    }
    else 
    {
        return isRunning;
    }
}
    
void AudioQueueWrapper::play()
{
    AudioQueueStart(m_audioQueue, NULL);
    printf("Audio queue started\n");
}

void AudioQueueWrapper::stop()
{
    AudioQueueStop(m_audioQueue, m_audioPlayerShouldStopImmediately);
}

void AudioQueueWrapper::pause()
{
    AudioQueuePause(m_audioQueue);
}

void AudioQueueWrapper::resume()
{
    AudioQueueStart(m_audioQueue, NULL); // NULL start time means ASAP
}
