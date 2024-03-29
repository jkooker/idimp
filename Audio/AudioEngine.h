// Copyright (c) 2009 Michelle Daniels and John Kooker
// 
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
// 
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.

/**
 *  @file AudioEngine.h
 *  iDiMP
 *
 *  Created by Michelle Daniels on 11/15/08.
 *
 *  This file defines the interface for the AudioEngine class.
 */

#ifndef AUDIO_ENGINE_H
#define AUDIO_ENGINE_H

#import <vector>

#import "AudioBasics.h"
#import "AudioEffect.h"
#import "Wavefile.h"
#import "TouchSynth.h"
#import "NetworkController.h"

/** AudioEngine class.
 * The AudioEngine class controls all audio recording, playback, and processing.
 * It follows the singleton pattern.
 */
class AudioEngine
{
public:

   /** 
    * AudioEngine destructor
    */
    ~AudioEngine();    
    
   /**
    * @return A pointer to the singleton instance of AudioEngine.
    */
    static AudioEngine* getInstance();  
    
   /**
    * Add a recording effect to this AudioEngine.
    * Recording effects are only applied to audio recorded from the microphone.
    * @param e A pointer to the AudioEffect to be added.
    * @see removeRecordingEffect
    */
    void addRecordingEffect(AudioEffect* e);    
   
   /** 
    * Remove a recording effect from this AudioEngine.
    * Recording effects are only applied to audio recorded from the microphone.
    * @param e A pointer to the AudioEffect to be removed.
    * @return true if the requested AudioEffect was found and removed, false otherwise.
    * @see addRecordingEffect
    */
    bool removeRecordingEffect(AudioEffect* e);    
    
   /**
    * Add a synthesis effect to this AudioEngine.
    * Synthesis effects are only applied to the synthesized audio.
    * @param e A pointer to the AudioEffect to be added.
    * @see removeSynthesisEffect
    */
    void addSynthesisEffect(AudioEffect* e);    
    
   /** 
    * Remove a synthesis effect from this AudioEngine.
    * Synthesis effects are only applied to the synthesized audio.
    * @param e A pointer to the AudioEffect to be removed.
    * @return true if the requested AudioEffect was found and removed, false otherwise.
    * @see addSynthesisEffect
    */
    bool removeSynthesisEffect(AudioEffect* e);
    
   /**
    * Add a network effect to this AudioEngine.
    * Network effects are only applied to the audio input from the network.
    * @param e A pointer to the AudioEffect to be added.
    * @see removeNetworkEffect
    */
    void addNetworkEffect(AudioEffect* e);    
   
   /** 
    * Remove a network effect from this AudioEngine.
    * Network effects are only applied to the audio input from the network.
    * @param e A pointer to the AudioEffect to be removed.
    * @return true if the requested AudioEffect was found and removed, false otherwise.
    * @see addNetworkEffect
    */
    bool removeNetworkEffect(AudioEffect* e);  
    
   /**
    * Add a master effect to this AudioEngine.
    * Master effects are applied to the mixed audio immediately before playback.
    * @param e A pointer to the AudioEffect to be added.
    * @see getMasterEffect
    * @see removeMasterEffect
    */
    void addMasterEffect(AudioEffect* e);    
    
   /**
    * Get the master AudioEffect at the given index.
    * Master effects are applied to the mixed audio immediately before playback.
    * @param index the index of the effect (effects are applied in the order they are added
    * and have corresponding indices).
    * @return a pointer to the requested AudioEffect object or NULL if no AudioEffect exists
    * at the given index.  The AudioEngine still owns the
    * object - the caller is not responsible for freeing any memory.
    * @see addMasterEffect
    * @see removeMasterEffect
    */
    AudioEffect* getMasterEffect(int index);
    
   /** 
    * Remove a master effect from this AudioEngine.
    * Master effects are applied to the mixed audio immediately before playback.
    * @param e A pointer to the AudioEffect to be removed.
    * @return true if the requested AudioEffect was found and removed, false otherwise.
    * @see addMasterEffect
    * @see getMasterEffect
    */
    bool removeMasterEffect(AudioEffect* e);  
    
   /** 
    * Get a pointer to the TouchSynth object associated with this AudioEngine.
    * @return A pointer to the TouchSynth object associated with this AudioEngine.
    */
    TouchSynth* getSynth() { return &m_synth; }
    
   /**
    * Find out whether or not recording input is muted.
    * @return true if recording input is muted, false otherwise.
    * @see setMuteRecording
    */
    bool getMuteRecording() { return m_recordingIsMuted; }
    
   /**
    * Enable or disable muting of recorded input.
    * @param on true to mute recording, false to unmute.
    * @see getMuteRecording
    */
    void setMuteRecording(bool on) { m_recordingIsMuted = on; }
    
   /**
    * Find out whether or not synthesized audio is muted.
    * @return true if synthesized audio is muted, false otherwise.
    * @see setMuteSynth
    */
    bool getMuteSynth() { return m_synthIsMuted; }
    
   /**
    * Enable or disable muting of synthesized audio.
    * @param on true to mute synthesized audio, false to unmute.
    * @see getMuteSynth
    */
    void setMuteSynth(bool on) { m_synthIsMuted = on; }
   
   /**
    * Find out whether or not networked audio is muted.
    * @return true if networked audio is muted, false otherwise.
    * @see setMuteNetwork
    */
    bool getMuteNetwork() { return m_networkIsMuted; }
    
   /**
    * Enable or disable muting of networked audio.
    * @param on true to mute networked audio, false to unmute.
    * @see getMuteNetwork
    */
    void setMuteNetwork(bool on) { m_networkIsMuted = on; }
    
   /**
    * Establish connection to network controller.
    * If this connection happens too early on startup, we get crashes when publishing Bonjour services.
    */
    void connectToNetworkController() { m_networkController = [NetworkController sharedInstance]; }
      
   /**
    * Find out whether audio playback and recording have been started.
    * @return true if started, false otherwise.
    * @see start
    * @see stop
    */
    bool isStarted() { return m_isStarted; }
    
   /**
    * Start audio playback and recording.
    * @see stop
    * @see isStarted
    */
    void start();
    
   /**
    * Stop audio playback and recording.
    * @see start
    * @see isStarted
    */
    void stop();
    
   /**
    * recordingCallback is the method called when there is recorded input available from the microphone.
    * Do not call this method directly.  Instead, to start/stop callbacks, use start() and stop().
    * @param inRefCon a reference to the AudioEngine instance that will process this callback.
    * @param ioActionFlags flags describing how an AudioUnit will render the recorded data.
    * @param inTimeStamp the time stamp of the recorded audio data.
    * @param inBusNumber the AudioUnit bus number for recording audio.
    * @param inNumberFrames the number of frames of recorded audio data available (a frame is one sample of all channels)
    * @param ioData not used at this time but a necessary part of the callback function declaration
    * @see start
    * @see stop
    */
    static OSStatus recordingCallback(void *inRefCon, 
                                      AudioUnitRenderActionFlags *ioActionFlags, 
                                      const AudioTimeStamp *inTimeStamp, 
                                      UInt32 inBusNumber, 
                                      UInt32 inNumberFrames, 
                                      AudioBufferList *ioData);
                                          
   /**
    * playbackCallback is the method called when the audio output device wants to request audio samples for playback.
    * Do not call this method directly.  Instead, to start/stop callbacks, use start() and stop().
    * @param inRefCon a reference to the AudioEngine instance that will process this callback.
    * @param ioActionFlags flags describing how an AudioUnit will render the audio data.
    * @param inTimeStamp the time stamp of the audio data.
    * @param inBusNumber the AudioUnit bus number for playing audio.
    * @param inNumberFrames the number of frames of audio data requested (a frame is one sample of all channels)
    * @param ioData a pointer to the AudioBufferList whose buffers will be filled with audio data for playback
    * @see start
    * @see stop
    */
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
        
    void allocate_temp_buffers(int numSamplesAllChannels);
        
    void enable_playback();
        
    void enable_recording();
        
    void fill_buffer_with_silence(float* buffer, 
                                  int n);
        
    void get_recorded_data_for_playback(float* buffer, 
                                        int numSamplesAllChannels);
        
    void get_synthesized_data_for_playback(float* buffer, 
                                           int numSamplesAllChannels);
                                           
    void get_network_data_for_playback(float* buffer, 
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
    bool m_networkIsMuted;
    int m_playbackSamplesAllChannels;
    float* m_tempRecordedBuffer;
    float* m_tempSynthesizedBuffer;
    float* m_tempNetworkBuffer;
    short* m_tempNetworkBufferShort;
    float* m_tempMixedPlaybackBuffer;
    float* m_tempMixedNetworkOutputBuffer;
    short* m_tempMixedNetworkOutputBufferShort;
    std::vector<AudioEffect*> m_recordingEffects;
    std::vector<AudioEffect*> m_synthEffects;
    std::vector<AudioEffect*> m_networkEffects;
    std::vector<AudioEffect*> m_masterEffects;
    TouchSynth m_synth;
    NetworkController *m_networkController;
    bool m_isStarted;
};

#endif // AUDIO_ENGINE_H