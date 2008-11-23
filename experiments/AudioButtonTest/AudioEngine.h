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

#import "AudioBasics.h"
#import "AudioEffect.h"
#import "Wavefile.h"
#import "Synth.h"

#define kOutputBus 0
#define kInputBus 1

static const int NUM_SYNTH_VOICES = 1;

//#define WRITE_DEBUG_FILE
#ifdef WRITE_DEBUG_FILE
    static const char* DEBUG_FILE_NAME = "debug.wav";
#endif

class AudioEngine
{
public:

    AudioEffect** m_effects;
    int m_numEffects;
    Synth* m_synth;
    
    AudioEngine() :
        m_inputBufferList(NULL),
        m_recordedData(NULL),
        m_recordedDataSizeInBytes(0),
        m_debugFile(NULL),
        m_effects(NULL),
        m_numEffects(0),
        m_recordingIsMuted(false),
        m_synthIsMuted(false),
        m_silenceBuffer(NULL),
        m_playbackSamplesAllChannels(0),
        m_synth(NULL)
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
        
        // init effects - they will be called in ascending array order at processing time
        m_numEffects = 2;
        m_effects = new AudioEffect*[m_numEffects];
        m_effects[0] = new RingMod();
        m_effects[1] = new AmplitudeScale();
        
        // init synth
        m_synth = new Synth(NUM_SYNTH_VOICES);
        
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
        if (m_effects != NULL)
        {
            for (int i = 0; i < m_numEffects; i++)
            {
                if (m_effects[i] != NULL)
                {
                    delete m_effects[i];
                    m_effects[i] = NULL;
                }
            }
            delete m_effects;
            m_effects = NULL;
        }
        
        // free silence buffer
        if (m_silenceBuffer != NULL)
        {
            delete m_silenceBuffer;
            m_silenceBuffer = NULL;
        }
        
        // free synth
        if (m_synth != NULL)
        {
            delete m_synth;
            m_synth = NULL;
        }
    }
    
    bool GetMuteRecording() { return m_recordingIsMuted; }
    void SetMuteRecording(bool on) { m_recordingIsMuted = on; }
    
    bool GetMuteSynth() { return m_synthIsMuted; }
    void SetMuteSynth(bool on) { m_synthIsMuted = on; }
    
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
    
    void allocate_silence_buffer(int numSamplesAllChannels)
    {
        if (m_silenceBuffer == NULL)
        {
            // allocate silence buffer for future use
            m_playbackSamplesAllChannels = numSamplesAllChannels;
            m_silenceBuffer = new float[m_playbackSamplesAllChannels];
            for (int n = 0; n < m_playbackSamplesAllChannels; n++)
            {
                m_silenceBuffer[n] = 0.0f; // can this be done with memset instead?
            }
        }
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
    
    void mix_and_convert(float* in1, float* in2, short* out1, int numSamples)
    {
        float* pIn1 = in1;
        float* pIn2 = in2;
        short* pOut1 = out1;
        for (int n = 0; n < numSamples; n++)
        {
            *(pOut1++) = (short)(AUDIO_MAX_AMPLITUDE *  ((*(pIn1++) + *(pIn2++)) / 2.0));
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
            // TODO: compare numSamplesAllChannels with stored m_playbackSamplesAllChannels to make sure the number hasn't changed (would be an error)
            float* tempRecorded = new float[numSamplesAllChannels];
            if (m_recordingIsMuted || m_recordedData == NULL || m_recordedDataSizeInBytes <= 0)
            {
                if (m_recordedData == NULL || m_recordedDataSizeInBytes <= 0)
                {
                    printf("AudioEngine::playback_callback: no recorded data to play - substituting silence!\n");
                }
                allocate_silence_buffer(numSamplesAllChannels);
                // insert silence
                memcpy(tempRecorded, m_silenceBuffer, m_playbackSamplesAllChannels * sizeof(float));    
            }
            else
            {
                convert_short_to_float((short*)m_recordedData, tempRecorded, numSamplesAllChannels);
            }
            
            // do processing on recorded data
            for (int effect = 0; effect < m_numEffects; effect++)
            {
                m_effects[effect]->Process(tempRecorded, numSamplesAllChannels / AUDIO_NUM_CHANNELS, AUDIO_NUM_CHANNELS);
            }
            
            // get synthesized audio
            float* tempSynthesized = new float[numSamplesAllChannels];
            if (m_synthIsMuted)
            {
                // insert silence
                allocate_silence_buffer(numSamplesAllChannels);
                memcpy(tempSynthesized, m_silenceBuffer, m_playbackSamplesAllChannels * sizeof(float));    
            }
            else
            {
                m_synth->RenderAudioBuffer(tempSynthesized, numSamplesAllChannels / AUDIO_NUM_CHANNELS, AUDIO_NUM_CHANNELS);
            }
            
            // TODO: do processing on synthesized data
            
            // TODO: make sure that the playback buffer is large enough for the recorded data
            // copy recorded and synthesized data into playback buffer
            mix_and_convert(tempRecorded, tempSynthesized, (short*)ioData->mBuffers[i].mData, numSamplesAllChannels);
            ioData->mBuffers[i].mDataByteSize = numSamplesAllChannels * AUDIO_BIT_DEPTH_IN_BYTES;
            
            // TODO: make tempRecorded and tempSynthesized member variables that are only allocated the first time around and then reused
            delete tempRecorded;
            delete tempSynthesized;
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
    bool m_recordingIsMuted;
    bool m_synthIsMuted;
    float* m_silenceBuffer;
    int m_playbackSamplesAllChannels;
};

#endif // AUDIO_ENGINE_H