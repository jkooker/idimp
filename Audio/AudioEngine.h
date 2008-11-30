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

class AudioEngine
{
public:

    ~AudioEngine();    
    static AudioEngine* getInstance();    
    void addRecordingEffect(AudioEffect* e);    
    bool removeRecordingEffect(AudioEffect* e);    
    void addSynthesisEffect(AudioEffect* e);    
    bool removeSynthesisEffect(AudioEffect* e);
    
    TouchSynth* getSynth() { return &m_synth; }
    bool getMuteRecording() { return m_recordingIsMuted; }
    void setMuteRecording(bool on) { m_recordingIsMuted = on; }
    bool getMuteSynth() { return m_synthIsMuted; }
    void setMuteSynth(bool on) { m_synthIsMuted = on; }
    bool isStarted() { return m_isStarted; }
    
    void start();
    void stop();
    static OSStatus recordingCallback(void *inRefCon, 
                                      AudioUnitRenderActionFlags *ioActionFlags, 
                                      const AudioTimeStamp *inTimeStamp, 
                                      UInt32 inBusNumber, 
                                      UInt32 inNumberFrames, 
                                      AudioBufferList *ioData);
                                          
    static OSStatus playbackCallback(void *inRefCon, 
                                     AudioUnitRenderActionFlags *ioActionFlags, 
                                     const AudioTimeStamp *inTimeStamp, 
                                     UInt32 inBusNumber, 
                                     UInt32 inNumberFrames, 
                                     AudioBufferList *ioData);
    
protected:
    // constructor is protected because this class is a singleton
    AudioEngine();
    
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
        
    static void print_audio_unit_properties(const AudioUnit unit, 
                                            const char* name);    
                                            
    static void print_audio_unit_uint32_property(const AudioUnit unit, 
                                                 AudioUnitPropertyID inID, 
                                                 AudioUnitScope inScope, 
                                                 const char* propertyDisplayName);
                                                 
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