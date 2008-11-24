/*
 *  Synth.h
 *  AudioButtonTest
 *
 *  Created by Michelle Daniels on 11/22/08.
 *  Copyright 2008 UCSD. All rights reserved.
 *
 */

#ifndef SYNTH_H
#define SYNTH_H

#import "Oscillator.h"

class Synth
{
public:
    Synth(int numVoices) :
        m_voices(NULL),
        m_numVoices(numVoices),
        m_voiceIsOn(NULL),
        m_silenceBuffer(NULL),
        m_silenceBufferSize(0)
    {
        m_voices = new Oscillator[m_numVoices];
        m_voiceIsOn = new bool[m_numVoices];
        // init all voices to "on"
        for (int v = 0; v < m_numVoices; v++)
        {
            m_voiceIsOn[v] = true;
        }
    }
    
    ~Synth()
    {
        if (m_voices != NULL)
        {
            delete m_voices;
            m_voices = NULL;
        }
        
        if (m_voiceIsOn != NULL)
        {
            delete m_voiceIsOn;
            m_voiceIsOn = NULL;
        }
        
        if (m_silenceBuffer != NULL)
        {
            delete m_silenceBuffer;
            m_silenceBuffer = NULL;
        }
    }
    
    void RenderAudioBuffer(float* output, int numSamplesPerChannel, int numChannels)
    {
        // first zero out output buffer (can I use memset to do this??)
        int nTotalSamples = numSamplesPerChannel * numChannels; 
        allocate_silence_buffer(nTotalSamples);
        fill_buffer_with_silence(output, nTotalSamples);
        
        for (int v = 0; v < m_numVoices; v++)
        {
            if (m_voiceIsOn[v])
            {
                m_voices[v].addNextSamplesToBuffer(output, numSamplesPerChannel, numChannels);
            }
        }
        
        // scale output by number of voices to avoid clipping
        if (m_numVoices > 1)
        {
            for (int n = 0; n < nTotalSamples; n++)
            {
                output[n] /= m_numVoices;
            }
        }
    }
    
    float getVoiceAmp(int voice)
    {
        if (voice < 0 || voice >= m_numVoices)
        {
            printf("Synth::getVoiceAmp voice index out of range: %d\n", voice);
            return 0.0;
        }
        
        return m_voices[voice].getAmp();
    }

    void setVoiceAmp(int voice, float amp)
    {
        if (voice < 0 || voice >= m_numVoices)
        {
            printf("Synth::setVoiceAmp voice index out of range: %d\n", voice);
            return;
        }
        
        m_voices[voice].setAmp(amp);
    }
    
    float getVoiceFreq(int voice)
    {
        if (voice < 0 || voice >= m_numVoices)
        {
            printf("Synth::getVoiceFreq voice index out of range: %d\n", voice);
            return 0.0;
        }
        
        return m_voices[voice].getFreq();
    }

    void setVoiceFreq(int voice, float freq)
    {
        if (voice < 0 || voice >= m_numVoices)
        {
            printf("Synth::setVoiceFreq voice index out of range: %d\n", voice);
            return;
        }
        
        m_voices[voice].setFreq(freq);
    }
    
    void setVoiceWaveform(int voice, Oscillator::Waveform wave)
    {
        if (voice < 0 || voice >= m_numVoices)
        {
            printf("Synth::setVoiceWaveform voice index out of range: %d\n", voice);
            return;
        }
        
        m_voices[voice].setWaveform(wave);
    }
    
    void voiceOff(int voice) 
    { 
        if (voice < 0 || voice >= m_numVoices)
        {
            printf("Synth::voiceOff voice index out of range: %d\n", voice);
            return;
        }
        m_voiceIsOn[voice] = false; 
    }

    void voiceOn(int voice) 
    { 
        if (voice < 0 || voice >= m_numVoices)
        {
            printf("Synth::voiceOn voice index out of range: %d\n", voice);
            return;
        }
        m_voiceIsOn[voice] = true; 
    }
    
    bool voiceIsOn(int voice) 
    { 
        if (voice < 0 || voice >= m_numVoices)
        {
            printf("Synth::voiceIsOn voice index out of range: %d\n", voice);
            return false;
        }
        return m_voiceIsOn[voice]; 
    }

private:

    void allocate_silence_buffer(int n)
    {
        // TODO: check to make sure n hasn't changed from what was originally allocated?
        m_silenceBufferSize = n;
        if (m_silenceBuffer == NULL)
        {
            // allocate silence buffer for future use
            m_silenceBuffer = new float[m_silenceBufferSize];
            for (int i = 0; i < m_silenceBufferSize; i++)
            {
                m_silenceBuffer[i] = 0.0f; // can this be done with memset instead?
            }
        }
    }

    void fill_buffer_with_silence(float* buffer, int n)
    {
        // make sure slience buffer has been initialized
        allocate_silence_buffer(n);
                
        // insert silence
        memcpy(buffer, m_silenceBuffer, n * sizeof(float));    

    }

    Oscillator* m_voices;
    bool* m_voiceIsOn;
    int m_numVoices;
    float* m_silenceBuffer;
    int m_silenceBufferSize;
};

#endif // SYNTH_H