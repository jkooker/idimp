/*
 *  AudioEngine.h
 *  AudioButtonTest
 *
 *  Created by Michelle Daniels on 11/15/08.
 *  Copyright 2008 UCSD. All rights reserved.
 *
 */

#ifndef AUDIO_ENGINE_H
#define AUDIO_ENGINE_H

#include "AudioBasics.h"
#include "Wavefile.h" // NOTE: could use preprocessor to not link this in at all if desired when not in debug mode

#define kOutputBus 0
#define kInputBus 1

//#define WRITE_DEBUG_FILE
#ifdef WRITE_DEBUG_FILE
    static const char* DEBUG_FILE_NAME = "debug.wav";
#endif

class AudioEngine
{
public:
    AmplitudeScale* m_ampEffect;
    RingMod* m_ringModEffect;

    AudioEngine() :
        m_inputBufferList(NULL),
        m_recordedData(NULL),
        m_recordedDataSizeInBytes(0),
        m_debugFile(NULL),
        m_ampEffect(NULL),
        m_ringModEffect(NULL)
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
        
        // Get audio unit
        OSStatus status = AudioComponentInstanceNew(inputComponent, &m_audioUnit);
        if (status != noErr)
        {
            printf("AudioEngine::AudioEngine could not create new audio component: status = %d\n", status);
        }
        
        // enable IO for recording
        enable_recording();
                
        // enable IO for playback
        enable_playback();
        
        // init audio format for input and output
        init_audio_format();
                        
        // set input and output callbacks
        init_callbacks();
                
        // initialize
        status = AudioUnitInitialize(m_audioUnit);
        if (status != noErr)
        {
            printf("AudioEngine::AudioEngine could not initialize audio unit: status = %d\n", status);
        }    
        
        print_audio_unit_properties(m_audioUnit, "REMOTE IO");
        
        // init effects
        m_ampEffect = new AmplitudeScale();
        m_ringModEffect = new RingMod();

#ifdef WRITE_DEBUG_FILE
        m_debugFile = new Wavefile(DEBUG_FILE_NAME);
#endif
    }
    
    ~AudioEngine()
    {
        printf("AudioEngine::~AudioEngine\n");
        
        // uninitialize audio unit
        AudioUnitUninitialize(m_audioUnit);
        
        // free input buffer list used to retrieve recorded data
        if (m_inputBufferList != NULL)
        {
            for (UInt32 i = 0; i < m_inputBufferList->mNumberBuffers; i++)
            {
                free (m_inputBufferList->mBuffers[i].mData);
            }
            delete (m_inputBufferList);
            m_inputBufferList = NULL;
        }
        
        // free buffer for recorded data
        if (m_recordedData != NULL)
        {
            free (m_recordedData);
            m_recordedData = NULL;
        }
        
        if (m_debugFile != NULL)
        {
            delete m_debugFile;
            m_debugFile = NULL;
        }
        
        // free memory from effect objects
        if (m_ampEffect != NULL)
        {
            delete m_ampEffect;
            m_ampEffect = NULL;
        }
        if (m_ringModEffect != NULL)
        {
            delete m_ringModEffect;
            m_ringModEffect = NULL;
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
        return engine->recording_callback(ioActionFlags,
                                          inTimeStamp,
                                          inBusNumber,
                                          inNumberFrames,
                                          ioData);
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
        return engine->playback_callback(ioActionFlags, 
                                         inTimeStamp, 
                                         inBusNumber, 
                                         inNumberFrames, 
                                         ioData);
    }
    
private:

    void allocate_input_buffers(UInt32 inNumberFrames)
    {
        printf("AudioEngine::allocate_input_buffers: inNumberFrames = %d\n", inNumberFrames);
               
        //UInt32 bufferSizeInBytes = inNumberFrames * m_audioFormat.mBytesPerFrame;
        UInt32 bufferSizeInBytes = inNumberFrames * (AUDIO_FORMAT_IS_NONINTERLEAVED ? AUDIO_BIT_DEPTH_IN_BYTES :  (AUDIO_BIT_DEPTH_IN_BYTES * AUDIO_NUM_CHANNELS));
        
        // allocate buffer list
        m_inputBufferList = new AudioBufferList; 
        m_inputBufferList->mNumberBuffers = AUDIO_FORMAT_IS_NONINTERLEAVED ? AUDIO_NUM_CHANNELS : 1;
        for (UInt32 i = 0; i < m_inputBufferList->mNumberBuffers; i++)
        {
            printf("AudioEngine::allocate_input_buffers: i = %d, bufferSizeInBytes = %d\n", i, bufferSizeInBytes);
            m_inputBufferList->mBuffers[i].mNumberChannels = AUDIO_FORMAT_IS_NONINTERLEAVED ? 1 : AUDIO_NUM_CHANNELS;
            m_inputBufferList->mBuffers[i].mDataByteSize = bufferSizeInBytes;
            m_inputBufferList->mBuffers[i].mData = malloc(bufferSizeInBytes); // could write this with new/delete...
        }
                
        if (m_recordedData == NULL)
        {
            m_recordedData = malloc(bufferSizeInBytes);
            m_recordedDataSizeInBytes = bufferSizeInBytes;
        }
        //printf("AudioEngine::allocate_input_buffers finished: m_inputBufferList = %d\n", m_inputBufferList);
    }
    
    void convert_float_to_short(float* in, short* out, int numSamples)
    {
        float* pIn = in;
        short* pOut = out;
        for (int n = 0; n < numSamples; n++)
        {
            *(pOut++) = (short)(*(pIn++) * AUDIO_MAX_AMPLITUDE);
        }
    }
    
    void convert_short_to_float(short* in, float* out, int numSamples)
    {
        short* pIn = in;
        float* pOut = out;
        for (int n = 0; n < numSamples; n++)
        {
            *(pOut++) = *(pIn++) / (float)AUDIO_MAX_AMPLITUDE;
        }
    }
    
    void enable_playback()
    {
        // enable IO for playback
        UInt32 flag = 1;
        OSStatus status = AudioUnitSetProperty(m_audioUnit, 
                                               kAudioOutputUnitProperty_EnableIO, 
                                               kAudioUnitScope_Output, 
                                               kOutputBus,
                                               &flag, 
                                               sizeof(flag));
        if (status != noErr)
        {
            printf("AudioEngine::enable_playback failed: status = %d\n", status);
        }
    }
    
    void enable_recording()
    {
        // Enable IO for recording
        UInt32 flag = 1;
        OSStatus status = AudioUnitSetProperty(m_audioUnit, 
                                               kAudioOutputUnitProperty_EnableIO, 
                                               kAudioUnitScope_Input, 
                                               kInputBus,
                                               &flag, 
                                               sizeof(flag));
        if (status != noErr)
        {
            printf("AudioEngine::enable_recording failed: status = %d\n", status);
        }
    }
    
    void init_audio_format()
    {
        // describe format
        PopulateAudioDescription(m_audioFormat);
              
        // Apply output format
        OSStatus status = AudioUnitSetProperty(m_audioUnit, 
                                               kAudioUnitProperty_StreamFormat, 
                                               kAudioUnitScope_Output, 
                                               kInputBus, 
                                               &m_audioFormat, 
                                               sizeof(m_audioFormat));
        if (status != noErr)
        {
            printf("AudioEngine::init_audio_format could not set output format: status = %d\n", status);
        }
        
        // Apply input format
        status = AudioUnitSetProperty(m_audioUnit, 
                                      kAudioUnitProperty_StreamFormat, 
                                      kAudioUnitScope_Input, 
                                      kOutputBus, 
                                      &m_audioFormat, 
                                      sizeof(m_audioFormat));
        if (status != noErr)
        {
            printf("AudioEngine::init_audio_format could not set input format: status = %d\n", status);
        }
    }
    
    void init_callbacks()
    {
        // Set input callback
        AURenderCallbackStruct callbackStruct;
        callbackStruct.inputProc = recordingCallback;
        callbackStruct.inputProcRefCon = this;
        OSStatus status = AudioUnitSetProperty(m_audioUnit, 
                                               kAudioOutputUnitProperty_SetInputCallback, 
                                               kAudioUnitScope_Global, 
                                               kInputBus, 
                                               &callbackStruct, 
                                               sizeof(callbackStruct));
        if (status != noErr)
        {
            printf("AudioEngine::init_callbacks could not set input callback: status = %d\n", status);
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
            printf("AudioEngine::init_callbacks could not set output callback: status = %d\n", status);
        }
    }    
    
    void mix(float* in1, float* in2, float* out1, int numSamples)
    {
        float* pIn1 = in1;
        float* pIn2 = in2;
        float* pOut1 = out1;
        for (int n = 0; n < numSamples; n++)
        {
            *(pOut1++) = ( *(pIn1++) + *(pIn2++) ) / 2.0;
        }
    }
    
    void mix_and_convert(float* in1, short* in2, short* out1, int numSamples)
    {
        float* pIn1 = in1;
        short* pIn2 = in2;
        short* pOut1 = out1;
        for (int n = 0; n < numSamples; n++)
        {
            *(pOut1++) = (short)(( (*(pIn1++) * AUDIO_MAX_AMPLITUDE) + *(pIn2++) ) / 2.0);
        }
    }
    
    static void print_audio_unit_properties(AudioUnit unit, const char* name)
    {
        printf("AudioEngine Audio Unit Properties for %s\n", name);
        print_audio_unit_uint32_property(unit, kAudioUnitProperty_SampleRate, kAudioUnitScope_Global, "kAudioUnitProperty_SampleRate");
        print_audio_unit_uint32_property(unit, kAudioUnitProperty_ElementCount, kAudioUnitScope_Global, "kAudioUnitProperty_ElementCount");
        print_audio_unit_uint32_property(unit, kAudioUnitProperty_Latency, kAudioUnitScope_Global, "kAudioUnitProperty_Latency");
        print_audio_unit_uint32_property(unit, kAudioUnitProperty_SupportedNumChannels, kAudioUnitScope_Global, "kAudioUnitProperty_SupportedNumChannels");
        print_audio_unit_uint32_property(unit, kAudioUnitProperty_MaximumFramesPerSlice, kAudioUnitScope_Global, "kAudioUnitProperty_MaximumFramesPerSlice");
    }
    
    static void print_audio_unit_uint32_property(AudioUnit unit, AudioUnitPropertyID inID, AudioUnitScope inScope, const char* propertyDisplayName)
    {
        UInt32 propValue = 0;
        UInt32 propValueSize = sizeof(propValue);
        OSStatus status = AudioUnitGetProperty(unit,
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
    
    OSStatus playback_callback(AudioUnitRenderActionFlags *ioActionFlags, 
                               const AudioTimeStamp *inTimeStamp, 
                               UInt32 inBusNumber, 
                               UInt32 inNumberFrames, 
                               AudioBufferList *ioData) 
    {    
        //printf("AudioEngine::playback_callback: inBusNumber = %d, inNumberFrames = %d, ioData = %d\n", inBusNumber, inNumberFrames, ioData);
        //printf("AudioEngine::playback_callback: ioData->mNumberBuffers = %d\n", ioData->mNumberBuffers);

        for (int i = 0; i < ioData->mNumberBuffers; i++)
        {
            ioData->mBuffers[i].mNumberChannels = AUDIO_FORMAT_IS_NONINTERLEAVED ? 1: AUDIO_NUM_CHANNELS;
            //printf("AudioEngine::playback_callback: i = %d, mBuffers[i].mNumberChannels = %d, mBuffers[i].mDataByteSize = %d\n", i, ioData->mBuffers[i].mNumberChannels, ioData->mBuffers[i].mDataByteSize);
            
            // copy recorded data to temp buffer in float form if there is any - otherwise insert silence
            int numSamplesAllChannels = m_recordedDataSizeInBytes / AUDIO_BIT_DEPTH_IN_BYTES;
            float* tempRecorded = new float[numSamplesAllChannels];
            if (m_recordedData == NULL || m_recordedDataSizeInBytes <= 0)
            {
                printf("AudioEngine::playback_callback: no recorded data to play - substituting silence!\n");
                // TODO: store a static array of float data that can be copied in here - or can memset be used for a float array?
                for (int n = 0; n < numSamplesAllChannels; n++)
                {
                    tempRecorded[n] = 0.0f;
                }
            }
            else
            {
                convert_short_to_float((short*)m_recordedData, tempRecorded, numSamplesAllChannels);
            }
            
            // do processing and add synthesized audio
            m_ringModEffect->Process(tempRecorded, numSamplesAllChannels / AUDIO_NUM_CHANNELS, AUDIO_NUM_CHANNELS);
            m_ampEffect->Process(tempRecorded, numSamplesAllChannels / AUDIO_NUM_CHANNELS, AUDIO_NUM_CHANNELS);
            
            if (m_recordedDataSizeInBytes <= ioData->mBuffers[i].mDataByteSize)
            {
                // copy recorded data into playback buffer
                convert_float_to_short(tempRecorded, (short*)ioData->mBuffers[i].mData, numSamplesAllChannels);
                ioData->mBuffers[i].mDataByteSize = numSamplesAllChannels * AUDIO_BIT_DEPTH_IN_BYTES;
            }
            else 
            {
                printf("AudioEngine::playback_callback playback buffer not large enough: m_recordedDataSizeInBytes = %d, buffer size = %d\n", m_recordedDataSizeInBytes, ioData->mBuffers[i].mDataByteSize);
            }
            
            delete (tempRecorded);
        }
        
#ifdef WRITE_DEBUG_FILE
        m_debugFile->WriteAudioBuffers(ioData);
#endif
        //printf("AudioEngine::playback_callback FINISHED\n");
        
        return noErr;
    }
    
      
    OSStatus recording_callback(AudioUnitRenderActionFlags *ioActionFlags, 
                                const AudioTimeStamp *inTimeStamp, 
                                UInt32 inBusNumber, 
                                UInt32 inNumberFrames, 
                                AudioBufferList *ioData) 
    {
        //printf("AudioEngine::recording_callback: inBusNumber = %d, inNumberFrames = %d, ioData = %d\n", inBusNumber, inNumberFrames, ioData);
        //printf("inTimeStamp->mSampleTime = %d, mHostTime = %d, mRateScalar = %d, mFlags = %d\n", inTimeStamp->mSampleTime, inTimeStamp->mHostTime, inTimeStamp->mRateScalar, inTimeStamp->mFlags);

        if (m_inputBufferList == NULL)
        {
            allocate_input_buffers(inNumberFrames);
        }
        
        // fill buffer list with recorded samples
        OSStatus status = AudioUnitRender(m_audioUnit, 
                                          ioActionFlags, 
                                          inTimeStamp, 
                                          inBusNumber, 
                                          inNumberFrames, 
                                          m_inputBufferList);
        if (status != noErr)
        {
            // -- error codes --
            // paramErr = -50,  /*error in user parameter list*/
            printf("AudioEngine::recording_callback could not render audio unit: status = %d\n", status);
        }
        
        memcpy(m_recordedData, m_inputBufferList->mBuffers[0].mData, m_inputBufferList->mBuffers[0].mDataByteSize);
        
        //printf("AudioEngine::recording_callback FINISHED\n");
        
        return noErr;
    }
    
    AudioUnit m_audioUnit;
    AudioStreamBasicDescription m_audioFormat;
    AudioBufferList* m_inputBufferList;
    void* m_recordedData;
    UInt32 m_recordedDataSizeInBytes;
    Wavefile* m_debugFile;
};

#endif // AUDIO_ENGINE_H