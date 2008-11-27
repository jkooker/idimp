/*
 *  AudioEngine.cpp
 *  iDiMP
 *
 *  Created by Michelle Daniels on 11/26/08.
 *  Copyright 2008 UCSD. All rights reserved.
 *
 */

#import "AudioEngine.h"

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

void AudioEngine::allocate_silence_buffer(int n)
{
    // NOTE: we don't need to check to make sure that n hasn't changed from previous
    // calls since allocate_temp_buffers already does this check first
    if (m_silenceBuffer == NULL)
    {
        // allocate silence buffer for future use
        m_silenceBuffer = new float[n];
        for (int i = 0; i < n; i++)
        {
            m_silenceBuffer[i] = 0.0f; // can this be done with memset instead?
        }
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
        mix_and_convert(m_tempRecordedBuffer, m_tempSynthesizedBuffer, (short*)ioData->mBuffers[i].mData, numSamplesAllChannels);
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
