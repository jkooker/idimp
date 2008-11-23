/*
 *  AudioQueueWrapper.cpp
 *  AudioButtonTest
 *
 *  Created by Michelle Daniels on 11/2/08.
 *  Copyright 2008 UCSD. All rights reserved.
 *
 */

#import "AudioQueueWrapper.h"
#import "AudioBasics.h"
#import <stdio.h>
#import <stdlib.h>
#import <unistd.h>
#import <sys/types.h>

//#define _DEBUG_

static const char* DEBUG_FILE_NAME = "debug.wav";
static const CFStringEncoding DEFAULT_STRING_ENCODING = kCFStringEncodingMacRoman;
static const int NUM_BUFFER_SAMPLES = 512; // values less than this seem to result in buffer underflows or no audio at all :(

static void PlaybackCallbackFunction(void* in, AudioQueueRef inQ, AudioQueueBufferRef outQB)
{
    AudioQueueWrapper* aq = (AudioQueueWrapper*) in;
    aq->PlaybackCallback(inQ, outQB);
}

static void RecordingCallbackFunction(void *in, 
                                      AudioQueueRef inAudioQueue,
                                      AudioQueueBufferRef inBuffer,
                                      const AudioTimeStamp *inStartTime,
                                      UInt32 inNumPackets,
                                      const AudioStreamPacketDescription *inPacketDesc)
{
    AudioQueueWrapper* aq = (AudioQueueWrapper*) in;
    aq->RecordingCallback(inAudioQueue, inBuffer, inStartTime, inNumPackets, inPacketDesc);
}

AudioQueueWrapper::AudioQueueWrapper() :
    m_audioQueueOutput(NULL),
    m_audioQueueInput(NULL),
    m_samplesPerFramePerChannel(NUM_BUFFER_SAMPLES),
    m_audioPlayerShouldStopImmediately(true),
    m_debugFileID(0),
    m_debugFileByteOffset(0)
{
    printf("AudioQueueWrapper::AudioQueueWrapper\n");
    PopulateAudioDescription(m_dataFormat);
    
#ifdef _DEBUG_
    InitDebugFile();
#endif
    InitPlayback();
    InitRecording();
}

AudioQueueWrapper::~AudioQueueWrapper() 
{
    printf("AudioQueueWrapper::~AudioQueueWrapper\n");
    if (m_audioQueueOutput != NULL)
    {
        AudioQueueDispose(m_audioQueueOutput, TRUE);
    }
    if (m_audioQueueInput != NULL)
    {
        AudioQueueDispose(m_audioQueueInput, TRUE);
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
        err = AudioQueueAllocateBuffer(m_audioQueueOutput, bufferSize, &m_outputBuffers[i]);
        printf("allocated buffer %d\n", i);
        if (err) return err;
        PlaybackCallback(m_audioQueueOutput, m_outputBuffers[i]);
    }
    printf("AudioQueueWrapper::InitPlayback finished\n");
    return 0;
}

void AudioQueueWrapper::InitRecording()
{
    printf("AudioQueueWrapper::InitRecording\n");
    AudioQueueNewInput(&m_dataFormat,
                       RecordingCallbackFunction,
                       this,
                       NULL,
                       NULL,
                       0,
                       &m_audioQueueInput);
                      
    // allocate and enqueue buffers
	int bufferByteSize = 65536;		// this is the maximum buffer size used by the player class
	
	for (int bufferIndex = 0; bufferIndex < NUM_AUDIO_DATA_BUFFERS; ++bufferIndex) 
    {
		AudioQueueBufferRef buffer;
		
		AudioQueueAllocateBuffer(m_audioQueueInput, bufferByteSize, &buffer);

		AudioQueueEnqueueBuffer(m_audioQueueInput, buffer, 0, NULL);
	}
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
    //printf("AudioQueueWrapper::PlaybackCallback inQ = %d, outQB = %d\n", inQ, outQB);
    short* coreAudioBuffer = (short*) outQB->mAudioData;

    if (m_samplesPerFramePerChannel > 0)
    {
        outQB->mAudioDataByteSize = 4 * m_samplesPerFramePerChannel;
        m_osc.nextSampleBuffer(coreAudioBuffer, m_samplesPerFramePerChannel, AUDIO_NUM_CHANNELS, MAX_AMPLITUDE_16_BITS);
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
    //printf("AudioQueueWrapper::PlaybackCallback FINISHED\n");
}

void AudioQueueWrapper::RecordingCallback(AudioQueueRef inAudioQueue,
                                          AudioQueueBufferRef inBuffer,
                                          const AudioTimeStamp *inStartTime,
                                          UInt32 inNumPackets,
                                          const AudioStreamPacketDescription *inPacketDesc)
{
    //printf("AudioQueueWrapper::RecordingCallback\n");
    
    if (inNumPackets > 0)
    {
        // write data to output
    }

	// if not stopping, re-enqueue the buffer so that it can be filled again
	if (inputIsRunning()) 
    {
		AudioQueueEnqueueBuffer (m_audioQueueInput, inBuffer, 0, NULL);
	}
    //printf("AudioQueueWrapper::RecordingCallback FINISHED\n");
}

bool AudioQueueWrapper::outputIsRunning() const
{
    return (is_running(m_audioQueueOutput));
}

bool AudioQueueWrapper::inputIsRunning() const
{
    return (is_running(m_audioQueueInput));
}

bool AudioQueueWrapper::is_running(AudioQueueRef queue) const
{
    UInt32 isRunning;
    UInt32 propertySize = sizeof(UInt32);
    OSStatus result = AudioQueueGetProperty( queue, 
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
    printf("Output audio queue started\n");
}

void AudioQueueWrapper::stop()
{
    AudioQueueStop(m_audioQueueOutput, m_audioPlayerShouldStopImmediately);
    printf("Output audio queue stopped\n");
}

void AudioQueueWrapper::pause()
{
    AudioQueuePause(m_audioQueueOutput);
    printf("Output audio queue paused\n");
}

void AudioQueueWrapper::resume()
{
    AudioQueueStart(m_audioQueueOutput, NULL); // NULL start time means ASAP
    printf("Output audio queue resumed\n");
}

void AudioQueueWrapper::recordStart()
{
    AudioQueueStart(m_audioQueueInput, NULL);
    printf("Input audio queue started\n");
}

void AudioQueueWrapper::recordStop()
{
    AudioQueueStop(m_audioQueueInput, m_audioPlayerShouldStopImmediately);
    printf("Input audio queue stopped\n");
}

void AudioQueueWrapper::recordPause()
{
    AudioQueuePause(m_audioQueueInput);
    printf("Input audio queue paused\n");

}