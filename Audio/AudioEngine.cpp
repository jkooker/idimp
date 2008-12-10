/*
 *  AudioEngine.cpp
 *  iDiMP
 *
 *  Created by Michelle Daniels on 11/26/08.
 *  Copyright 2008 UCSD. All rights reserved.
 *
 */

#import "AudioEngine.h"

//#define WRITE_DEBUG_FILE
#ifdef WRITE_DEBUG_FILE
    static const char* DEBUG_FILE_NAME = "debug.wav";
#endif

/* ---- AudioEngine public methods ---- */

AudioEngine::~AudioEngine()
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

AudioEngine* AudioEngine::getInstance()
{
    // this way, the static instance gets properly destructed when the app ends?
    static AudioEngine instance;
    return &instance;
}

void AudioEngine::addRecordingEffect(AudioEffect* e)
{
    m_recordingEffects.push_back(e);
}

bool AudioEngine::removeRecordingEffect(AudioEffect* e)
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

void AudioEngine::addSynthesisEffect(AudioEffect* e)
{
    m_synthEffects.push_back(e);
}

bool AudioEngine::removeSynthesisEffect(AudioEffect* e)
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

void AudioEngine::addNetworkEffect(AudioEffect* e)
{
    m_networkEffects.push_back(e);
}

bool AudioEngine::removeNetworkEffect(AudioEffect* e)
{
    for (std::vector<AudioEffect*>::iterator it = m_networkEffects.begin(); it != m_networkEffects.end(); it++) 
    {
        if ((*it) == e)
        {
            // effect found
            m_networkEffects.erase(it);
            printf("AudioEngine::removeNetworkEffect effect found!\n");
            return true;
        }
    }
    // effect not found
    printf("AudioEngine::removeNetworkEffect effect NOT found!\n");
    return false;
}

void AudioEngine::addMasterEffect(AudioEffect* e)
{
    m_masterEffects.push_back(e);
}

bool AudioEngine::removeMasterEffect(AudioEffect* e)
{
    for (std::vector<AudioEffect*>::iterator it = m_masterEffects.begin(); it != m_masterEffects.end(); it++) 
    {
        if ((*it) == e)
        {
            // effect found
            m_masterEffects.erase(it);
            printf("AudioEngine::removeMasterEffect effect found!\n");
            return true;
        }
    }
    // effect not found
    printf("AudioEngine::removeMasterEffect effect NOT found!\n");
    return false;
}

OSStatus AudioEngine::recordingCallback(void *inRefCon, 
                                        AudioUnitRenderActionFlags *ioActionFlags, 
                                        const AudioTimeStamp *inTimeStamp, 
                                        UInt32 inBusNumber, 
                                        UInt32 inNumberFrames, 
                                        AudioBufferList *ioData) 
{
    AudioEngine* engine = (AudioEngine*)inRefCon;
    return engine->recording_callback(ioActionFlags,
                                      inTimeStamp,
                                      inBusNumber,
                                      inNumberFrames,
                                      ioData);
}

OSStatus AudioEngine::playbackCallback(void *inRefCon, 
                                       AudioUnitRenderActionFlags *ioActionFlags, 
                                       const AudioTimeStamp *inTimeStamp, 
                                       UInt32 inBusNumber, 
                                       UInt32 inNumberFrames, 
                                       AudioBufferList *ioData) 
{    
    AudioEngine* engine = (AudioEngine*)inRefCon;
    return engine->playback_callback(ioActionFlags, 
                                     inTimeStamp, 
                                     inBusNumber, 
                                     inNumberFrames, 
                                     ioData);
}

void AudioEngine::start()
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

void AudioEngine::stop()
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

/* ---- AudioEngine protected methods ---- */

AudioEngine::AudioEngine() :
    m_inputBufferList(NULL),
    m_recordedData(NULL),
    m_recordedDataSizeInBytes(0),
    m_debugFile(NULL),
    m_recordingIsMuted(false),
    m_synthIsMuted(false),
    m_networkIsMuted(true),
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


/* ---- AudioEngine private methods ---- */

void AudioEngine::allocate_input_buffers(UInt32 inNumberFrames)
{
    printf("AudioEngine::allocate_input_buffers: inNumberFrames = %d\n", inNumberFrames);
    
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
}

void AudioEngine::allocate_temp_buffers(int numSamplesAllChannels)
{
    // TODO: check to make sure m_playbackSamplesAllChannels hasn't changed once buffers have been allocated
    m_playbackSamplesAllChannels = numSamplesAllChannels;
    if (m_tempRecordedBuffer == NULL)
    {
        m_tempRecordedBuffer = new float[numSamplesAllChannels];
    }
    if (m_tempSynthesizedBuffer == NULL)
    {
        m_tempSynthesizedBuffer = new float[numSamplesAllChannels];
    }
}

void AudioEngine::enable_playback()
{
    // enable IO for playback
    UInt32 flag = 1;
    OSStatus status = AudioUnitSetProperty(m_audioUnit, 
                                           kAudioOutputUnitProperty_EnableIO, 
                                           kAudioUnitScope_Output, 
                                           AUDIO_OUTPUT_BUS,
                                           &flag, 
                                           sizeof(flag));
    if (status != noErr)
    {
        printf("AudioEngine::enable_playback failed: status = %d\n", status);
    }
}

void AudioEngine::enable_recording()
{
    // Enable IO for recording
    UInt32 flag = 1;
    OSStatus status = AudioUnitSetProperty(m_audioUnit, 
                                           kAudioOutputUnitProperty_EnableIO, 
                                           kAudioUnitScope_Input, 
                                           AUDIO_INPUT_BUS,
                                           &flag, 
                                           sizeof(flag));
    if (status != noErr)
    {
        printf("AudioEngine::enable_recording failed: status = %d\n", status);
    }
}

void AudioEngine::fill_buffer_with_silence(float* buffer, int n)
{
    // insert silence
    memset(buffer, 0, n * sizeof(float));
}

void AudioEngine::get_recorded_data_for_playback(float* buffer, int numSamplesAllChannels)
{
    // copy recorded data to temp buffer in float form if there is any - otherwise insert silence
    if (m_recordingIsMuted || m_recordedData == NULL || m_recordedDataSizeInBytes <= 0)
    {
        if (m_recordedData == NULL || m_recordedDataSizeInBytes <= 0)
        {
            printf("AudioEngine::get_recorded_data_for_playback: no recorded data to play - substituting silence!\n");
        }
        fill_buffer_with_silence(buffer, numSamplesAllChannels);
    }
    else
    {
        AudioSamplesShortToFloat((short*)m_recordedData, buffer, numSamplesAllChannels);
        
        // do processing on recorded data
        for (int effect = 0; effect < m_recordingEffects.size(); effect++)
        {
            m_recordingEffects[effect]->Process(buffer, numSamplesAllChannels / AUDIO_NUM_CHANNELS, AUDIO_NUM_CHANNELS);
        }
    }
}

void AudioEngine::get_synthesized_data_for_playback(float* buffer, int numSamplesAllChannels)
{
    // get synthesized audio
    if (m_synthIsMuted || m_synth.allVoicesAreOff())
    {
        fill_buffer_with_silence(buffer, numSamplesAllChannels);
    }
    else
    {
        m_synth.renderAudioBuffer(buffer, numSamplesAllChannels / AUDIO_NUM_CHANNELS, AUDIO_NUM_CHANNELS);
        
        // do processing on synthesized data
        for (int effect = 0; effect < m_synthEffects.size(); effect++)
        {
            m_synthEffects[effect]->Process(buffer, numSamplesAllChannels / AUDIO_NUM_CHANNELS, AUDIO_NUM_CHANNELS);
        }
    }
}

void AudioEngine::get_network_data_for_playback(float* buffer, int numSamplesAllChannels)
{
    // copy network data to temp buffer in float form if there is any - otherwise insert silence
    if (m_networkIsMuted)
    {
        fill_buffer_with_silence(buffer, numSamplesAllChannels);
    }
    else
    {
        // fill buffer of shorts from network - data is expected to be interleaved (sample1_left, sample1_right, sample2_left, sample2_right, etc.)
        
        // convert shorts to floats for processing
        //AudioSamplesShortToFloat(<networkData here>, buffer, numSamplesAllChannels);
        
        // do processing on network data
        for (int effect = 0; effect < m_networkEffects.size(); effect++)
        {
            m_networkEffects[effect]->Process(buffer, numSamplesAllChannels / AUDIO_NUM_CHANNELS, AUDIO_NUM_CHANNELS);
        }
    }
}

void AudioEngine::init_audio_format()
{
    // describe format
    PopulateAudioDescription(m_audioFormat);
    
    // Apply output format
    OSStatus status = AudioUnitSetProperty(m_audioUnit, 
                                           kAudioUnitProperty_StreamFormat, 
                                           kAudioUnitScope_Output, 
                                           AUDIO_INPUT_BUS, 
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
                                  AUDIO_OUTPUT_BUS, 
                                  &m_audioFormat, 
                                  sizeof(m_audioFormat));
    if (status != noErr)
    {
        printf("AudioEngine::init_audio_format could not set input format: status = %d\n", status);
    }
}

void AudioEngine::init_callbacks()
{
    // Set input callback
    AURenderCallbackStruct callbackStruct;
    callbackStruct.inputProc = recordingCallback;
    callbackStruct.inputProcRefCon = this;
    OSStatus status = AudioUnitSetProperty(m_audioUnit, 
                                           kAudioOutputUnitProperty_SetInputCallback, 
                                           kAudioUnitScope_Global, 
                                           AUDIO_INPUT_BUS, 
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
                                  AUDIO_OUTPUT_BUS,
                                  &callbackStruct, 
                                  sizeof(callbackStruct));
    if (status != noErr)
    {
        printf("AudioEngine::init_callbacks could not set output callback: status = %d\n", status);
    }
}    

OSStatus AudioEngine::playback_callback(AudioUnitRenderActionFlags *ioActionFlags, 
                                        const AudioTimeStamp *inTimeStamp, 
                                        UInt32 inBusNumber, 
                                        UInt32 inNumberFrames, 
                                        AudioBufferList *ioData) 
{    
    for (int i = 0; i < ioData->mNumberBuffers; i++)
    {
        ioData->mBuffers[i].mNumberChannels = AUDIO_FORMAT_IS_NONINTERLEAVED ? 1: AUDIO_NUM_CHANNELS;
        
        int numSamplesAllChannels = m_recordedDataSizeInBytes / AUDIO_BIT_DEPTH_IN_BYTES;
        
        // if needed, allocate buffers for temporary storage of recorded and synthesized data
        allocate_temp_buffers(numSamplesAllChannels);
        
        get_recorded_data_for_playback(m_tempRecordedBuffer, numSamplesAllChannels);
        
        get_synthesized_data_for_playback(m_tempSynthesizedBuffer, numSamplesAllChannels);
        
        // TODO: make sure that the playback buffer is large enough for the recorded data ??
        
        // copy recorded and synthesized data into playback buffer
        AudioSamplesMixFloatToShort(m_tempRecordedBuffer, m_tempSynthesizedBuffer, (short*)ioData->mBuffers[i].mData, numSamplesAllChannels);
        ioData->mBuffers[i].mDataByteSize = numSamplesAllChannels * AUDIO_BIT_DEPTH_IN_BYTES;
    }
    
#ifdef WRITE_DEBUG_FILE
    m_debugFile->WriteAudioBuffers(ioData);
#endif
    
    return noErr;
}

OSStatus AudioEngine::recording_callback(AudioUnitRenderActionFlags *ioActionFlags, 
                                         const AudioTimeStamp *inTimeStamp, 
                                         UInt32 inBusNumber, 
                                         UInt32 inNumberFrames, 
                                         AudioBufferList *ioData) 
{
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
    
    return noErr;
}

void AudioEngine::print_audio_unit_properties(const AudioUnit unit, const char* name)
{
    printf("AudioEngine Audio Unit Properties for %s\n", name);
    print_audio_unit_uint32_property(unit, kAudioUnitProperty_SampleRate, kAudioUnitScope_Global, "kAudioUnitProperty_SampleRate");
    print_audio_unit_uint32_property(unit, kAudioUnitProperty_ElementCount, kAudioUnitScope_Global, "kAudioUnitProperty_ElementCount");
    print_audio_unit_uint32_property(unit, kAudioUnitProperty_Latency, kAudioUnitScope_Global, "kAudioUnitProperty_Latency");
    print_audio_unit_uint32_property(unit, kAudioUnitProperty_SupportedNumChannels, kAudioUnitScope_Global, "kAudioUnitProperty_SupportedNumChannels");
    print_audio_unit_uint32_property(unit, kAudioUnitProperty_MaximumFramesPerSlice, kAudioUnitScope_Global, "kAudioUnitProperty_MaximumFramesPerSlice");
}

void AudioEngine::print_audio_unit_uint32_property(const AudioUnit unit, AudioUnitPropertyID inID, AudioUnitScope inScope, const char* propertyDisplayName)
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

