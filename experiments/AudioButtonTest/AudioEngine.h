/*
 *  AudioEngine.h
 *  AudioButtonTest
 *
 *  Created by Michelle Daniels on 11/15/08.
 *  Copyright 2008 UCSD. All rights reserved.
 *
 *  Note: use of RemoteIO audio unit based on code from here:
 *  http://michael.tyson.id.au/2008/11/04/using-remoteio-audio-unit/
 */

#ifndef AUDIO_ENGINE_H
#define AUDIO_ENGINE_H

#define kOutputBus 0
#define kInputBus 1

class AudioEngine
{
public:
    AudioEngine()
    {
        // Describe audio component
        AudioComponentDescription desc;
        desc.componentType = kAudioUnitType_Output;
        desc.componentSubType = kAudioUnitSubType_RemoteIO;
        desc.componentFlags = 0;
        desc.componentFlagsMask = 0;
        desc.componentManufacturer = kAudioUnitManufacturer_Apple;
        
        // Get component
        AudioComponent inputComponent = AudioComponentFindNext(NULL, &desc);
        
        // Get audio units
        OSStatus status = AudioComponentInstanceNew(inputComponent, &m_audioUnit);
        if (status != noErr)
        {
            printf("AudioEngine::AudioEngine could not create new audio component: status = %d\n", status);
        }
        
        // Enable IO for recording
        UInt32 flag = 1;
        status = AudioUnitSetProperty(m_audioUnit, 
                                      kAudioOutputUnitProperty_EnableIO, 
                                      kAudioUnitScope_Input, 
                                      kInputBus,
                                      &flag, 
                                      sizeof(flag));
        if (status != noErr)
        {
            printf("AudioEngine::AudioEngine could not could not enable recording: status = %d\n", status);
        }
        
        // Enable IO for playback
        status = AudioUnitSetProperty(m_audioUnit, 
                                      kAudioOutputUnitProperty_EnableIO, 
                                      kAudioUnitScope_Output, 
                                      kOutputBus,
                                      &flag, 
                                      sizeof(flag));
        if (status != noErr)
        {
            printf("AudioEngine::AudioEngine could not enable playback: status = %d\n", status);
        }
        
        // Describe format
        AudioStreamBasicDescription audioFormat;
        audioFormat.mSampleRate			= 44100.00;
        audioFormat.mFormatID			= kAudioFormatLinearPCM;
        audioFormat.mFormatFlags		= kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
        audioFormat.mFramesPerPacket	= 1;
        audioFormat.mChannelsPerFrame	= 1;
        audioFormat.mBitsPerChannel		= 16;
        audioFormat.mBytesPerPacket		= 2;
        audioFormat.mBytesPerFrame		= 2;
        
        // Apply (output?) format
        status = AudioUnitSetProperty(m_audioUnit, 
                                      kAudioUnitProperty_StreamFormat, 
                                      kAudioUnitScope_Output, 
                                      kInputBus, 
                                      &audioFormat, 
                                      sizeof(audioFormat));
        if (status != noErr)
        {
            printf("AudioEngine::AudioEngine could not set output format: status = %d\n", status);
        }
        
        // Apply (input?) format
        status = AudioUnitSetProperty(m_audioUnit, 
                                      kAudioUnitProperty_StreamFormat, 
                                      kAudioUnitScope_Input, 
                                      kOutputBus, 
                                      &audioFormat, 
                                      sizeof(audioFormat));
        if (status != noErr)
        {
            printf("AudioEngine::AudioEngine could not set input format: status = %d\n", status);
        }
        
        
        // Set input callback
        AURenderCallbackStruct callbackStruct;
        callbackStruct.inputProc = recordingCallback;
        callbackStruct.inputProcRefCon = this;
        status = AudioUnitSetProperty(m_audioUnit, 
                                      kAudioOutputUnitProperty_SetInputCallback, 
                                      kAudioUnitScope_Global, 
                                      kInputBus, 
                                      &callbackStruct, 
                                      sizeof(callbackStruct));
        if (status != noErr)
        {
            printf("AudioEngine::AudioEngine could not set input callback: status = %d\n", status);
        }

        
        // Set output callback
        callbackStruct.inputProc = playbackCallback;
        callbackStruct.inputProcRefCon = this;
        status = AudioUnitSetProperty(m_audioUnit, 
                                      kAudioUnitProperty_SetRenderCallback, 
                                      kAudioUnitScope_Global, 
                                      kOutputBus,
                                      &callbackStruct, 
                                      sizeof(callbackStruct));
        if (status != noErr)
        {
            printf("AudioEngine::AudioEngine could not set output callback: status = %d\n", status);
        }
        
        /*// Disable buffer allocation for the recorder (optional - do this if we want to pass in our own)
        flag = 0;
        status = AudioUnitSetProperty(m_audioUnit, 
                                      kAudioUnitProperty_ShouldAllocateBuffer,
                                      kAudioUnitScope_Output, 
                                      kInputBus,
                                      &flag, 
                                      sizeof(flag));
        
        // TODO: Allocate our own buffers if we want*/
        
        // Initialise
        status = AudioUnitInitialize(m_audioUnit);
        if (status != noErr)
        {
            printf("AudioEngine::AudioEngine could not initialize audio unit: status = %d\n", status);
        }    
    }
    
    ~AudioEngine()
    {
        AudioUnitUninitialize(m_audioUnit);
    }
    
    void start()
    {
        OSStatus status = AudioOutputUnitStart(m_audioUnit);
        if (status != noErr)
        {
            printf("AudioEngine::AudioEngine could not start audio unit: status = %d\n", status);
        }
    }
    
    void stop()
    {
        OSStatus status = AudioOutputUnitStop(m_audioUnit);
        if (status != noErr)
        {
            printf("AudioEngine::AudioEngine could not stop audio unit: status = %d\n", status);
        }
    }
    
    static OSStatus recordingCallback(void *inRefCon, 
                                      AudioUnitRenderActionFlags *ioActionFlags, 
                                      const AudioTimeStamp *inTimeStamp, 
                                      UInt32 inBusNumber, 
                                      UInt32 inNumberFrames, 
                                      AudioBufferList *ioData) 
    {
        printf("AudioEngine::recordingCallback\n");

        AudioEngine* engine = (AudioEngine*)inRefCon;
        return engine->recordingCallbackHelper(ioActionFlags,
                                               inTimeStamp,
                                               inBusNumber,
                                               inNumberFrames,
                                               ioData);
    }
    
    OSStatus recordingCallbackHelper(AudioUnitRenderActionFlags *ioActionFlags, 
                                     const AudioTimeStamp *inTimeStamp, 
                                     UInt32 inBusNumber, 
                                     UInt32 inNumberFrames, 
                                     AudioBufferList *ioData) 
    {
        printf("AudioEngine::recordingCallbackHelper\n");

        // TODO: Use inRefCon to access our interface object to do stuff
        // Then, use inNumberFrames to figure out how much data is available, and make
        // that much space available in buffers in an AudioBufferList.
        
        AudioBufferList *bufferList; // <- Fill this up with buffers (you will want to malloc it, as it's a dynamic-length list)
        
        // Then:
        // Obtain recorded samples
        
        OSStatus status = AudioUnitRender(m_audioUnit, 
                                          ioActionFlags, 
                                          inTimeStamp, 
                                          inBusNumber, 
                                          inNumberFrames, 
                                          bufferList);
        if (status != noErr)
        {
            printf("AudioEngine::recordingCallbackHelper could not render audio unit: status = %d\n", status);
        }
        
        // Now, we have the samples we just read sitting in buffers in bufferList
        //DoStuffWithTheRecordedAudio(bufferList);
        return noErr;
    }
    
    static OSStatus playbackCallback(void *inRefCon, 
                                     AudioUnitRenderActionFlags *ioActionFlags, 
                                     const AudioTimeStamp *inTimeStamp, 
                                     UInt32 inBusNumber, 
                                     UInt32 inNumberFrames, 
                                     AudioBufferList *ioData) 
    {    
        printf("AudioEngine::playbackCallback\n");

        // Notes: ioData contains buffers (may be more than one!)
        // Fill them up as much as you can. Remember to set the size value in each buffer to match how
        // much data is in the buffer.
        return noErr;
    }
    
private:
    AudioComponentInstance m_audioUnit;
    
};

#endif // AUDIO_ENGINE_H