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
static const int NUM_RECORDING_BUFFERS = 1;
static const int NUM_CHANNELS = 1; // mono for now

class AudioEngine
{
public:
    AudioEngine() :
        m_inputBufferList(NULL),
        m_recordedData(NULL),
        m_recordedDataSizeInBytes(0)
    {
        printf("AudioEngine::AudioEngine\n");
        
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
        m_audioFormat.mSampleRate           = 44100.00;
        m_audioFormat.mFormatID			    = kAudioFormatLinearPCM;
        m_audioFormat.mFormatFlags          = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
        m_audioFormat.mFramesPerPacket      = 1;
        m_audioFormat.mChannelsPerFrame     = NUM_CHANNELS;
        m_audioFormat.mBitsPerChannel		= 16;
        m_audioFormat.mBytesPerPacket		= 2 * NUM_CHANNELS;
        m_audioFormat.mBytesPerFrame		= 2 * NUM_CHANNELS;
        
        // Apply (output?) format
        status = AudioUnitSetProperty(m_audioUnit, 
                                      kAudioUnitProperty_StreamFormat, 
                                      kAudioUnitScope_Output, 
                                      kInputBus, 
                                      &m_audioFormat, 
                                      sizeof(m_audioFormat));
        if (status != noErr)
        {
            printf("AudioEngine::AudioEngine could not set output format: status = %d\n", status);
        }
        
        // Apply (input?) format
        status = AudioUnitSetProperty(m_audioUnit, 
                                      kAudioUnitProperty_StreamFormat, 
                                      kAudioUnitScope_Input, 
                                      kOutputBus, 
                                      &m_audioFormat, 
                                      sizeof(m_audioFormat));
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
        
        print_audio_unit_properties();
    }
    
    ~AudioEngine()
    {
        printf("AudioEngine::~AudioEngine\n");
        AudioUnitUninitialize(m_audioUnit);
        if (m_inputBufferList != NULL)
        {
            for (UInt32 i = 0; i < m_inputBufferList->mNumberBuffers; i++)
            {
                free(m_inputBufferList->mBuffers[i].mData);
            }
            free(m_inputBufferList);
            m_inputBufferList = NULL;
        }
        if (m_recordedData != NULL)
        {
            free(m_recordedData);
            m_recordedData = NULL;
        }
    }
    
    void start()
    {
        printf("AudioEngine::start\n");
        OSStatus status = AudioOutputUnitStart(m_audioUnit);
        if (status != noErr)
        {
            printf("AudioEngine::AudioEngine could not start audio unit: status = %d\n", status);
        }
    }
    
    void stop()
    {
        printf("AudioEngine::stop\n");
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
        //printf("AudioEngine::recordingCallback\n");

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
        //printf("AudioEngine::recordingCallbackHelper: inBusNumber = %d, inNumberFrames = %d, ioData = %d\n", inBusNumber, inNumberFrames, ioData);
        //printf("inTimeStamp->mSampleTime = %d, mHostTime = %d, mRateScalar = %d, mFlags = %d\n", inTimeStamp->mSampleTime, inTimeStamp->mHostTime, inTimeStamp->mRateScalar, inTimeStamp->mFlags);

        // TODO: Use inRefCon to access our interface object to do stuff
        // Then, use inNumberFrames to figure out how much data is available, and make
        // that much space available in buffers in an AudioBufferList.
        
        //AudioBufferList *bufferList; // <- Fill this up with buffers (you will want to malloc it, as it's a dynamic-length list)
        
        if (m_inputBufferList == NULL)
        {
            allocate_input_buffers(inNumberFrames);
        }
        
        // Then:
        // Obtain recorded samples
        
        OSStatus status = AudioUnitRender(m_audioUnit, 
                                          ioActionFlags, 
                                          inTimeStamp, 
                                          inBusNumber, 
                                          inNumberFrames, 
                                          m_inputBufferList);
        if (status != noErr)
        {
            printf("AudioEngine::recordingCallbackHelper could not render audio unit: status = %d\n", status);
        }
        
        // Now, we have the samples we just read sitting in buffers in bufferList
        //DoStuffWithTheRecordedAudio(bufferList);
        memcpy(m_recordedData, m_inputBufferList->mBuffers[0].mData, m_inputBufferList->mBuffers[0].mDataByteSize);
        
        //printf("AudioEngine::recordingCallbackHelper FINISHED\n");
        
        return noErr;
    }
    
    static OSStatus playbackCallback(void *inRefCon, 
                                     AudioUnitRenderActionFlags *ioActionFlags, 
                                     const AudioTimeStamp *inTimeStamp, 
                                     UInt32 inBusNumber, 
                                     UInt32 inNumberFrames, 
                                     AudioBufferList *ioData) 
    {    
        //printf("AudioEngine::playbackCallback\n");

        AudioEngine* engine = (AudioEngine*)inRefCon;
        return engine->playbackCallbackHelper(ioActionFlags, 
                                              inTimeStamp, 
                                              inBusNumber, 
                                              inNumberFrames, 
                                              ioData);
       
        return noErr;
    }
    
    OSStatus playbackCallbackHelper(AudioUnitRenderActionFlags *ioActionFlags, 
                                    const AudioTimeStamp *inTimeStamp, 
                                    UInt32 inBusNumber, 
                                    UInt32 inNumberFrames, 
                                    AudioBufferList *ioData) 
    {    
        //printf("AudioEngine::playbackCallbackHelper: inBusNumber = %d, inNumberFrames = %d, ioData = %d\n", inBusNumber, inNumberFrames, ioData);
        //printf("ioData->mNumberBuffers = %d\n", ioData->mNumberBuffers);

        // Notes: ioData contains buffers (may be more than one!)
        // Fill them up as much as you can. Remember to set the size value in each buffer to match how
        // much data is in the buffer.
        
        for (int i = 0; i < ioData->mNumberBuffers; i++)
        {
            //printf("buffer %d: mNumberChannels = %d, mDataByteSize = %d, mData = %d\n", i, ioData->mBuffers[i].mNumberChannels, ioData->mBuffers[i].mDataByteSize, ioData->mBuffers[i].mData);

            //ioData->mBuffers[i].mNumberChannels = NUM_CHANNELS;
            if (m_recordedData != NULL && m_recordedDataSizeInBytes <= ioData->mBuffers[i].mDataByteSize)
            {
                memcpy(ioData->mBuffers[i].mData, m_recordedData, m_recordedDataSizeInBytes);
                // NOTE: should not blindly copy all of recorded data - need to make sure that the size of the buffers provided 
                // by the callback aren't smaller than the amount of recorded data first
                ioData->mBuffers[i].mDataByteSize = m_recordedDataSizeInBytes;
            }
            else 
            {
                printf("AudioEngine::playbackCallbackHelper playback buffer not large enough: m_recordedDataSizeInBytes = %d, buffer size = %d\n", m_recordedDataSizeInBytes, ioData->mBuffers[i].mDataByteSize);
            }
        }
        
        //printf("AudioEngine::playbackCallbackHelper FINISHED\n");
        
        return noErr;
    }

    
private:

    void allocate_input_buffers(UInt32 inNumberFrames)
    {
        printf("AudioEngine::allocate_input_buffers: inNumberFrames = %d\n", inNumberFrames);
               
        // malloc buffer list(s)
        UInt32 propsize = offsetof(AudioBufferList, mBuffers[0]) + (sizeof(AudioBuffer) * m_audioFormat.mChannelsPerFrame);
        m_inputBufferList = (AudioBufferList*)malloc(propsize);
        m_inputBufferList->mNumberBuffers = m_audioFormat.mChannelsPerFrame;
        UInt32 bufferSizeInBytes = inNumberFrames * m_audioFormat.mBytesPerFrame;
        for (UInt32 i = 0; i < m_inputBufferList->mNumberBuffers; i++)
        {
            m_inputBufferList->mBuffers[i].mNumberChannels = NUM_CHANNELS;
            m_inputBufferList->mBuffers[i].mDataByteSize = bufferSizeInBytes;
            m_inputBufferList->mBuffers[i].mData = malloc(bufferSizeInBytes);
        }   
        
        if (m_recordedData == NULL)
        {
            m_recordedData = malloc(bufferSizeInBytes);
            m_recordedDataSizeInBytes = bufferSizeInBytes;
        }
    }
    
    void print_audio_unit_properties()
    {
        print_audio_unit_uint32_property(kAudioUnitProperty_SampleRate, kAudioUnitScope_Global, "kAudioUnitProperty_SampleRate");
        print_audio_unit_uint32_property(kAudioUnitProperty_ElementCount, kAudioUnitScope_Global, "kAudioUnitProperty_ElementCount");
        print_audio_unit_uint32_property(kAudioUnitProperty_Latency, kAudioUnitScope_Global, "kAudioUnitProperty_Latency");
        print_audio_unit_uint32_property(kAudioUnitProperty_SupportedNumChannels, kAudioUnitScope_Global, "kAudioUnitProperty_SupportedNumChannels");
        print_audio_unit_uint32_property(kAudioUnitProperty_MaximumFramesPerSlice, kAudioUnitScope_Global, "kAudioUnitProperty_MaximumFramesPerSlice");
    }
    
    void print_audio_unit_uint32_property(AudioUnitPropertyID inID, AudioUnitScope inScope, const char* propertyDisplayName)
    {
        UInt32 propValue = 0;
        UInt32 propValueSize = sizeof(propValue);
        OSStatus status = AudioUnitGetProperty(m_audioUnit,
                                               inID,
                                               inScope,
                                               0,
                                               &propValue,
                                               &propValueSize);
        if (status != noErr)
        {
            // -- error codes --
            // kAudioUnitErr_InvalidProperty	-10879
            // kAudioUnitErr_InvalidScope       -10866
            printf("AudioEngine::print_audio_unit_uint32_property could not get property %s: status = %d\n", propertyDisplayName, status);
        }
        else
        {
            printf("%s = %d\n", propertyDisplayName, propValue);
        }

    }
    
    AudioComponentInstance m_audioUnit;
    AudioStreamBasicDescription m_audioFormat;
    AudioBufferList* m_inputBufferList;
    void* m_recordedData;
    UInt32 m_recordedDataSizeInBytes;
    
};

#endif // AUDIO_ENGINE_H