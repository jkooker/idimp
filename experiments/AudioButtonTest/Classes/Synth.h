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
        m_numVoices(numVoices)
    {
        m_voices = new Oscillator[m_numVoices];
    }
    
    ~Synth()
    {
        if (m_voices != NULL)
        {
            delete m_voices;
            m_voices = NULL;
        }
    }
    
    void RenderAudioBuffer(float* output, int numSamplesPerChannel, int numChannels)
    {
        // first zero out output buffer (can I use memset to do this??)
        int nTotalSamples = numSamplesPerChannel * numChannels; 
        for (int n = 0; n < nTotalSamples; n++)
        {
            output[n] = 0.0f;
        }
        
        for (int v = 0; v < m_numVoices; v++)
        {
            m_voices[v].addNextSamplesToBuffer(output, numSamplesPerChannel, numChannels);
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

private:
    Oscillator* m_voices;
    int m_numVoices;
};

#endif // SYNTH_H