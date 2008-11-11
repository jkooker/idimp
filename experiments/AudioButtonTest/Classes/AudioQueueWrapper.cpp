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

//#define _DEBUG_

static const int NUM_CHANNELS = 2;
static const int MAX_AMPLITUDE = 32767;
static const char* DEBUG_FILE_NAME = "debug.wav";
static const CFStringEncoding DEFAULT_STRING_ENCODING = kCFStringEncodingMacRoman;

static void PlaybackCallbackFunction(void* in, AudioQueueRef inQ, AudioQueueBufferRef outQB)
{
    AudioQueueWrapper* aq = (AudioQueueWrapper*) in;
    aq->PlaybackCallback(inQ, outQB);
}

AudioQueueWrapper::AudioQueueWrapper() :
    m_audioQueueOutput(NULL),
    m_samplesPerFramePerChannel(512),
    m_audioPlayerShouldStopImmediately(true),
    m_debugFileID(0),
    m_debugFileByteOffset(0)
{
    printf("AudioQueueWrapper::AudioQueueWrapper\n");
    m_dataFormat.mSampleRate = SAMPLE_RATE;
    m_dataFormat.mFormatID = kAudioFormatLinearPCM;
    m_dataFormat.mFormatFlags = kLinearPCMFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
    m_dataFormat.mBytesPerPacket = 4;
    m_dataFormat.mFramesPerPacket = 1;
    m_dataFormat.mBytesPerFrame = 4; // 2 2-byte samples per channel
    m_dataFormat.mChannelsPerFrame = NUM_CHANNELS;
    m_dataFormat.mBitsPerChannel = 16;
    
#ifdef _DEBUG_
    InitDebugFile();
#endif
    InitPlayback();
}

AudioQueueWrapper::~AudioQueueWrapper() 
{
    printf("AudioQueueWrapper::~AudioQueueWrapper\n");
    if (m_audioQueueOutput != NULL)
    {
        AudioQueueDispose(m_audioQueueOutput, TRUE);
    }
    if (m_debugFileID > 0)
    {
        AudioFileClose(m_debugFileID);
    }
}

int AudioQueueWrapper::InitPlayback()
{
    printf("AudioQueueWrapper::InitPlayback\n");
    UInt32 err = AudioQueueNewOutput(&m_dataFormat, 
                                     PlaybackCallbackFunction,
                                     this,
                                     NULL,
                                     kCFRunLoopCommonModes,
                                     0,
                                     &m_audioQueueOutput);
    if (err) return err;
    
    UInt32 bufferSize = m_samplesPerFramePerChannel * m_dataFormat.mBytesPerFrame;
    for (int i = 0; i < NUM_AUDIO_DATA_BUFFERS; i++)
    {
        err = AudioQueueAllocateBuffer(m_audioQueueOutput, bufferSize, &m_buffers[i]);
        printf("allocated buffer %d\n", i);
        if (err) return err;
        PlaybackCallback(m_audioQueueOutput, m_buffers[i]);
    }
    printf("AudioQueueWrapper::InitPlayback finished\n");
    return 0;
}

void AudioQueueWrapper::InitDebugFile()
{
    if (m_debugFileID == 0) 
    {
        // create file URL from file name
        CFStringRef filename = CFStringCreateWithCString(NULL, DEBUG_FILE_NAME, DEFAULT_STRING_ENCODING);
        printf("AudioQueueWrapper::InitRecording filename = %s\n", CFStringGetCStringPtr(filename, DEFAULT_STRING_ENCODING));
        
        CFURLRef urlRef = CFURLCreateWithString( NULL, filename, NULL);
        printf("AudioQueueWrapper::InitRecording urlRef = %s\n", CFStringGetCStringPtr(CFURLGetString(urlRef), DEFAULT_STRING_ENCODING));
                                                  
		// create the audio file (NOTE: this also opens the file)
		OSStatus result = noErr;
        result = AudioFileCreateWithURL( urlRef, 
                                         kAudioFileWAVEType,
                                         &m_dataFormat,
                                         kAudioFileFlags_EraseFile,
                                         &m_debugFileID);	
        if (result != noErr)
        {
            printf("Error creating debug file: %d\n", result);
        }
    
        // release memory allocated above
        CFRelease(urlRef);
        CFRelease(filename);
	}
}

void AudioQueueWrapper::PlaybackCallback(AudioQueueRef inQ, AudioQueueBufferRef outQB)
{
    //printf("AudioQueueWrapper::AQBufferCallback\n");
    short* coreAudioBuffer = (short*) outQB->mAudioData;

    if (m_samplesPerFramePerChannel > 0)
    {
        outQB->mAudioDataByteSize = 4 * m_samplesPerFramePerChannel;
        m_osc.nextSampleBuffer(coreAudioBuffer, m_samplesPerFramePerChannel, NUM_CHANNELS, MAX_AMPLITUDE);
    }
    AudioQueueEnqueueBuffer(inQ, outQB, 0, NULL);
    
#ifdef _DEBUG_  
    // write to debug file
    UInt32 numBytes = m_samplesPerFramePerChannel * m_dataFormat.mBytesPerFrame;
    OSStatus result = noErr;
    result = AudioFileWriteBytes( m_debugFileID,
                                  FALSE,
                                  m_debugFileByteOffset,
                                  &numBytes,
                                  coreAudioBuffer);
    if (result != noErr)
    {
        printf("Error writing to debug file: %d\n", result);
    }
    if (numBytes < (m_samplesPerFramePerChannel * m_dataFormat.mBytesPerFrame))
    {
        printf("Warning: some bytes were not written to the debug file\n");
    }
    m_debugFileByteOffset += numBytes;
#endif
}

bool AudioQueueWrapper::isRunning() const
{
    UInt32 isRunning;
    UInt32 propertySize = sizeof(UInt32);
    OSStatus result = AudioQueueGetProperty( m_audioQueueOutput, 
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
    AudioQueueStart(m_audioQueueOutput, NULL);
    printf("Audio queue started\n");
}

void AudioQueueWrapper::stop()
{
    AudioQueueStop(m_audioQueueOutput, m_audioPlayerShouldStopImmediately);
}

void AudioQueueWrapper::pause()
{
    AudioQueuePause(m_audioQueueOutput);
}

void AudioQueueWrapper::resume()
{
    AudioQueueStart(m_audioQueueOutput, NULL); // NULL start time means ASAP
}
