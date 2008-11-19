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

#define kOutputBus 0
#define kInputBus 1

// audio format constants
//static const float SAMPLE_RATE = 44100;
static const int NUM_CHANNELS = 2;
static const int BIT_DEPTH_IN_BYTES = 2;
static const int BIT_DEPTH = 8 * BIT_DEPTH_IN_BYTES;
static const int FORMAT_ID = kAudioFormatLinearPCM;
static const int FORMAT_FRAMES_PER_PACKET = 1;
static const int FORMAT_FLAGS = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;

// debug file constants
//#define WRITE_DEBUG_FILE
#ifdef WRITE_DEBUG_FILE
    static const char* DEBUG_FILE_NAME = "debug.wav";
    static const CFStringEncoding DEFAULT_STRING_ENCODING = kCFStringEncodingMacRoman;
#endif

class AudioEngine
{
public:
    AudioEngine() :
        m_inputBufferList(NULL),
        m_recordedData(NULL),
        m_recordedDataSizeInBytes(0),
        m_debugFileID(0),
        m_debugFileByteOffset(0)
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
        init_debug_file();
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
            printf("AudioEngine::recordingCallbackHelper could not render audio unit: status = %d\n", status);
        }
        
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

        for (int i = 0; i < ioData->mNumberBuffers; i++)
        {
            //printf("buffer %d: mNumberChannels = %d, mDataByteSize = %d, mData = %d\n", i, ioData->mBuffers[i].mNumberChannels, ioData->mBuffers[i].mDataByteSize, ioData->mBuffers[i].mData);

            ioData->mBuffers[i].mNumberChannels = m_audioFormat.mChannelsPerFrame;
            if (m_recordedData == NULL || m_recordedDataSizeInBytes <= 0)
            {
                printf("AudioEngine::playbackCallbackHelper: nothing to play - inserting silence!\n");
                
                // fill the buffer with silence
                memset(ioData->mBuffers[i].mData, 0, ioData->mBuffers[i].mDataByteSize);
            }
            else if (m_recordedDataSizeInBytes <= ioData->mBuffers[i].mDataByteSize)
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
        
#ifdef WRITE_DEBUG_FILE
        write_debug_file(ioData);
#endif
        //printf("AudioEngine::playbackCallbackHelper FINISHED\n");
        
        return noErr;
    }

    
private:

    void allocate_input_buffers(UInt32 inNumberFrames)
    {
        printf("AudioEngine::allocate_input_buffers: inNumberFrames = %d\n", inNumberFrames);
               
        UInt32 bufferSizeInBytes = inNumberFrames * m_audioFormat.mBytesPerFrame;
        
        // allocate buffer list
        m_inputBufferList = new AudioBufferList; 
        //m_inputBufferList->mNumberBuffers = m_audioFormat.mChannelsPerFrame;
        m_inputBufferList->mNumberBuffers = 1; // 1 because we're using interleaved data - all channels will go in one buffer
        for (UInt32 i = 0; i < m_inputBufferList->mNumberBuffers; i++)
        {
            printf("AudioEngine::allocate_input_buffers: i = %d, m_inputBufferList->mBuffers[i] = %d\n", i, m_inputBufferList->mBuffers[i]);
            m_inputBufferList->mBuffers[i].mNumberChannels = NUM_CHANNELS;
            m_inputBufferList->mBuffers[i].mDataByteSize = bufferSizeInBytes;
            m_inputBufferList->mBuffers[i].mData = malloc(bufferSizeInBytes); // could write this with new/delete...
        }
                
        if (m_recordedData == NULL)
        {
            m_recordedData = malloc(bufferSizeInBytes);
            m_recordedDataSizeInBytes = bufferSizeInBytes;
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
        // Describe format
        m_audioFormat.mSampleRate       = SAMPLE_RATE;
        m_audioFormat.mFormatID		    = FORMAT_ID;
        m_audioFormat.mFormatFlags      = FORMAT_FLAGS;
        m_audioFormat.mFramesPerPacket  = FORMAT_FRAMES_PER_PACKET;
        m_audioFormat.mChannelsPerFrame = NUM_CHANNELS;
        m_audioFormat.mBitsPerChannel	= BIT_DEPTH;
        m_audioFormat.mBytesPerPacket	= BIT_DEPTH_IN_BYTES * NUM_CHANNELS;
        m_audioFormat.mBytesPerFrame	= BIT_DEPTH_IN_BYTES * NUM_CHANNELS;  
              
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
    
#ifdef WRITE_DEBUG_FILE
    void init_debug_file()
    {
        if (m_debugFileID == 0) 
        {
            // create file URL from file name
            CFStringRef filename = CFStringCreateWithCString(NULL, DEBUG_FILE_NAME, DEFAULT_STRING_ENCODING);
            printf("AudioEngine::init_debug_file: filename = %s\n", CFStringGetCStringPtr(filename, DEFAULT_STRING_ENCODING));
            
            CFURLRef urlRef = CFURLCreateWithString( NULL, filename, NULL);
            printf("AudioEngine::init_debug_file: urlRef = %s\n", CFStringGetCStringPtr(CFURLGetString(urlRef), DEFAULT_STRING_ENCODING));
            
            // create the audio file (NOTE: this also opens the file)
            OSStatus result = noErr;
            result = AudioFileCreateWithURL(urlRef, 
                                            kAudioFileWAVEType,
                                            &m_audioFormat,
                                            kAudioFileFlags_EraseFile,
                                            &m_debugFileID);	
            if (result != noErr)
            {
                printf("AudioEngine::init_debug_file: error creating debug file: %d\n", result);
            }
            
            // release memory allocated above
            CFRelease(urlRef);
            CFRelease(filename);
        }
    }
    
    void write_debug_file(AudioBufferList* bufferList)
    {
        //printf("AudioEngine::write_debug_file m_debugFileID = %d, m_debugFileByteOffset = %d, bufferList = %d\n", m_debugFileID, m_debugFileByteOffset, bufferList);
        if (m_debugFileID == 0)
        {
            // file was never properly initialized so we can't write to it
            return;
        }
        for (int i = 0; i < bufferList->mNumberBuffers; i++)
        {
            // write to debug file
            UInt32 numBytes = bufferList->mBuffers[i].mDataByteSize;
            //printf("AudioEngine::write_debug_file i = %d, numBytes = %d\n", i, numBytes);
            OSStatus result = noErr;
            result = AudioFileWriteBytes(m_debugFileID,
                                         FALSE,
                                         m_debugFileByteOffset,
                                         &numBytes, // this should hold actual num bytes written upon return
                                         bufferList->mBuffers[i].mData);
            //printf("AudioEngine::write_debug_file wrote bytes: numBytes = %d\n", numBytes);
            if (result != noErr)
            {
                printf("AudioEngine::write_debug_file: error writing to debug file: %d\n", result);
            }
            if (numBytes < bufferList->mBuffers[i].mDataByteSize)
            {
                printf("AudioEngine::write_debug_file: warning: some bytes were not written to the debug file\n");
            }
            m_debugFileByteOffset += numBytes;
        }
    }
#endif    
    
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
    
    AudioUnit m_audioUnit;
    AudioStreamBasicDescription m_audioFormat;
    AudioBufferList* m_inputBufferList;
    void* m_recordedData;
    UInt32 m_recordedDataSizeInBytes;
    AudioFileID m_debugFileID;
    SInt64 m_debugFileByteOffset;
};

#endif // AUDIO_ENGINE_H