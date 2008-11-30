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

#import <vector>

#import "AudioBasics.h"
#import "AudioEffect.h"
#import "Wavefile.h"
#import "Synth.h"

//#define WRITE_DEBUG_FILE
#ifdef WRITE_DEBUG_FILE
    static const char* DEBUG_FILE_NAME = "debug.wav";
#endif

class AudioEngine
{
public:

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
        
        // free silence buffer
        if (m_silenceBuffer != NULL)
        {
            delete m_silenceBuffer;
            m_silenceBuffer = NULL;
        }
    
        // free temp buffers
        if (m_tempRecordedBuffer != NULL)
        {
            delete m_tempRecordedBuffer;
            m_tempRecordedBuffer = NULL;
        }
        if (m_tempSynthesizedBuffer != NULL)
        {
            delete m_tempSynthesizedBuffer;
            m_tempSynthesizedBuffer = NULL;
        }
    }
    
    static AudioEngine* getInstance()
    {
        // this way, the static instance gets properly destructed when the app ends?
        static AudioEngine instance;
        return &instance;
    }
    
    void addRecordingEffect(AudioEffect* e)
    {
        m_recordingEffects.push_back(e);
    }
    
    bool removeRecordingEffect(AudioEffect* e)
    {
        for (std::vector<AudioEffect*>::iterator it = m_recordingEffects.begin(); it != m_recordingEffects.end(); it++) 
        {
            if ((*it) == e)
            {
                // effect found
                m_recordingEffects.erase(it);
                printf("AudioEngine::removeRecordingEffect effect found!\n");
                return true;
            }
        }
        // effect not found
        printf("AudioEngine::removeRecordingEffect effect NOT found!\n");
        return false;
    }
    
    void addSynthesisEffect(AudioEffect* e)
    {
        m_synthEffects.push_back(e);
    }
    
    bool removeSynthesisEffect(AudioEffect* e)
    {
        for (std::vector<AudioEffect*>::iterator it = m_synthEffects.begin(); it != m_synthEffects.end(); it++) 
        {
            if ((*it) == e)
            {
                // effect found
                m_synthEffects.erase(it);
                return true;
            }
        }
        // effect not found
        return false;
    }
    
    TouchSynth* getSynth() { return &m_synth; }
    
    bool getMuteRecording() { return m_recordingIsMuted; }
    void setMuteRecording(bool on) { m_recordingIsMuted = on; }
    
    bool getMuteSynth() { return m_synthIsMuted; }
    void setMuteSynth(bool on) { m_synthIsMuted = on; }
    
    bool isStarted() { return m_isStarted; }
    
    void start()
    {
        printf("AudioEngine::start\n");
        OSStatus status = AudioOutputUnitStart(m_audioUnit);
        if (status != noErr)
        {
            printf("AudioEngine::AudioEngine could not start audio unit: status = %d\n", status);
        }
        else
        {
            m_isStarted = true;
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
        else
        {
            m_isStarted = false;
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
    
protected:
     AudioEngine() :
        m_inputBufferList(NULL),
        m_recordedData(NULL),
        m_recordedDataSizeInBytes(0),
        m_debugFile(NULL),
        m_recordingIsMuted(false),
        m_synthIsMuted(false),
        m_silenceBuffer(NULL),
        m_playbackSamplesAllChannels(0),
        m_tempRecordedBuffer(NULL),
        m_tempSynthesizedBuffer(NULL),
        m_isStarted(false)
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
        
#ifdef WRITE_DEBUG_FILE
        m_debugFile = new Wavefile(DEBUG_FILE_NAME);
#endif
    }
    
    // TODO: implement these if desired.  For now, the compiler will complain if someone tries to use them
    AudioEngine(const AudioEngine&);
    
    AudioEngine& operator= (const AudioEngine&);

private:

    void allocate_input_buffers(UInt32 inNumberFrames);
        
    void allocate_silence_buffer(int n);
        
    void allocate_temp_buffers(int numSamplesAllChannels);
        
    void enable_playback();
        
    void enable_recording();
        
    void fill_buffer_with_silence(float* buffer, 
                                  int n);
        
    void get_recorded_data_for_playback(float* buffer, 
                                        int numSamplesAllChannels);
        
    void get_synthesized_data_for_playback(float* buffer, 
                                           int numSamplesAllChannels);
                                               
    void init_audio_format();
    
    void init_callbacks();
        
    static void mix(const float* in1, const float* in2, float* out1, int numSamples)
    {
        const float* pIn1 = in1;
        const float* pIn2 = in2;
        float* pOut1 = out1;
        for (int n = 0; n < numSamples; n++)
        {
            *(pOut1++) = ( *(pIn1++) + *(pIn2++) ) / 2.0;
        }
    }
    
    static void mix_and_convert(const float* in1, const float* in2, short* out1, int numSamples)
    {
        const float* pIn1 = in1;
        const float* pIn2 = in2;
        short* pOut1 = out1;
        for (int n = 0; n < numSamples; n++)
        {
            *(pOut1++) = (short)(AUDIO_MAX_AMPLITUDE *  ((*(pIn1++) + *(pIn2++)) / 2.0));
        }
    }
    
    static void print_audio_unit_properties(const AudioUnit unit, const char* name)
    {
        printf("AudioEngine Audio Unit Properties for %s\n", name);
        print_audio_unit_uint32_property(unit, kAudioUnitProperty_SampleRate, kAudioUnitScope_Global, "kAudioUnitProperty_SampleRate");
        print_audio_unit_uint32_property(unit, kAudioUnitProperty_ElementCount, kAudioUnitScope_Global, "kAudioUnitProperty_ElementCount");
        print_audio_unit_uint32_property(unit, kAudioUnitProperty_Latency, kAudioUnitScope_Global, "kAudioUnitProperty_Latency");
        print_audio_unit_uint32_property(unit, kAudioUnitProperty_SupportedNumChannels, kAudioUnitScope_Global, "kAudioUnitProperty_SupportedNumChannels");
        print_audio_unit_uint32_property(unit, kAudioUnitProperty_MaximumFramesPerSlice, kAudioUnitScope_Global, "kAudioUnitProperty_MaximumFramesPerSlice");
    }
    
    static void print_audio_unit_uint32_property(const AudioUnit unit, AudioUnitPropertyID inID, AudioUnitScope inScope, const char* propertyDisplayName)
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
                               AudioBufferList *ioData);
                               
    OSStatus recording_callback(AudioUnitRenderActionFlags *ioActionFlags, 
                                const AudioTimeStamp *inTimeStamp, 
                                UInt32 inBusNumber, 
                                UInt32 inNumberFrames, 
                                AudioBufferList *ioData);
    
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
    float* m_tempRecordedBuffer;
    float* m_tempSynthesizedBuffer;
    std::vector<AudioEffect*> m_recordingEffects;
    std::vector<AudioEffect*> m_synthEffects;
    TouchSynth m_synth;
    bool m_isStarted;
};

#endif // AUDIO_ENGINE_H