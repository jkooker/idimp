/*
 *  AudioEffect.h
 *  AudioButtonTest
 *
 *  Created by Michelle Daniels on 11/11/08.
 *  Copyright 2008 UCSD. All rights reserved.
 *
 */

#ifndef AUDIO_EFFECT_H
#define AUDIO_EFFECT_H

#import "Oscillator.h"

class AudioEffect
{
public:
    AudioEffect() {}
    virtual ~AudioEffect() {}
    virtual void Process(short* buffer, int numSamples, int numChannels) = 0;
};

class AmplitudeScale : public AudioEffect
{
public:
    AmplitudeScale() :
        AudioEffect(),
        m_amp(1.0),
        m_oldAmp(1.0)
    {
    
    }
    
    virtual void Process(short* buffer, int numSamples, int numChannels)
    {
        float goalAmp = m_amp;
        float amplitudeDelta = goalAmp - m_oldAmp;
        //printf("AmplitudeScale::Process m_oldAmp = %f, goalAmp = %f, delta = %f\n", m_oldAmp, goalAmp, amplitudeDelta);
        for (int n = 0; n < numSamples; n++)
        {
            float percentage = n / (float)numSamples;
            float amp = m_oldAmp + (amplitudeDelta * percentage);
            //printf("amp = %f, percentage = %f\n", amp, percentage);
            for (int ch = 0; ch < numChannels; ch++)
            {
                int index = (2 * n) + ch;
                buffer[index] = (short)(amp * buffer[index]);            
            }
        }
        m_oldAmp = goalAmp;
    }
    
    virtual void Process(float* buffer, int numSamples, int numChannels)
    {
        float goalAmp = m_amp;
        float amplitudeDelta = goalAmp - m_oldAmp;
        //printf("AmplitudeScale::Process m_oldAmp = %f, goalAmp = %f, delta = %f\n", m_oldAmp, goalAmp, amplitudeDelta);
        for (int n = 0; n < numSamples; n++)
        {
            float percentage = n / (float)numSamples;
            float amp = m_oldAmp + (amplitudeDelta * percentage);
            //printf("amp = %f, percentage = %f\n", amp, percentage);
            for (int ch = 0; ch < numChannels; ch++)
            {
                int index = (2 * n) + ch;
                buffer[index] = amp * buffer[index];            
            }
        }
        m_oldAmp = goalAmp;
    }

    
    float getAmp() { return m_amp; }
    
    void setAmp(float amp)
    {
        //printf("AmplitudeScale::setAmp amp = %f, m_amp = %f, m_oldAmp = %f\n", amp, m_amp, m_oldAmp);
        //m_oldAmp = m_amp;
        m_amp = amp;
    }
    
private:
    float m_oldAmp;
    float m_amp;
};

class RingMod : public AudioEffect
{
public:
    RingMod(int bufferSamples, int bufferChannels) :
        AudioEffect(),
        m_bufferSamples(bufferSamples),
        m_bufferChannels(bufferChannels),
        m_buffer(NULL)
    {   
        m_osc.setAmp(1.0); // TODO: somewhere we need to make sure we don't clip...
        m_osc.setFreq(0.0);
        
        m_buffer = new short[m_bufferSamples * m_bufferChannels];
        m_bufferFloat = new float[m_bufferSamples];
    }
    
    virtual ~RingMod()
    {
        if (m_buffer != NULL)
        {
            delete m_buffer;
        }
        if (m_bufferFloat != NULL)
        {
            delete m_bufferFloat;
        }
    }
    
    virtual void Process(short* buffer, int numSamples, int numChannels)
    {
        //printf("RingMod::Process mod amp = %f, mod freq = %f\n", m_osc.getAmp(), m_osc.getFreq());
        // protect against buffer size mismatch
        if (m_bufferSamples != numSamples || m_bufferChannels != numChannels)
        {
            printf("RingMod::Process expected buffer of %d samples and %d channels, was given %d samples and %d channels\n", m_bufferSamples, m_bufferChannels, numSamples, numChannels);
            return;
        }
        
        // fill buffer for modulating waveform
        m_osc.nextSampleBuffer(m_buffer, m_bufferSamples, m_bufferChannels, MAX_AMPLITUDE_16_BITS);
        
        for (int n = 0; n < numSamples; n++)
        {
            for (int ch = 0; ch < numChannels; ch++)
            {
                int index = (2 * n) + ch;
                buffer[index] = m_buffer[index] * buffer[index] / MAX_AMPLITUDE_16_BITS;       
                //buffer[index] = m_buffer[index];     
            }
        }
    }
    
    virtual void Process(float* buffer, int numSamples, int numChannels)
    {
        //printf("RingMod::Process mod amp = %f, mod freq = %f\n", m_osc.getAmp(), m_osc.getFreq());
        // protect against buffer size mismatch
        if (m_bufferSamples != numSamples || m_bufferChannels != numChannels)
        {
            printf("RingMod::Process expected buffer of %d samples and %d channels, was given %d samples and %d channels\n", m_bufferSamples, m_bufferChannels, numSamples, numChannels);
            return;
        }
        
        // fill buffer for modulating waveform
        m_osc.nextSampleBufferMono(m_bufferFloat, m_bufferSamples);
        
        for (int n = 0; n < numSamples; n++)
        {
            for (int ch = 0; ch < numChannels; ch++)
            {
                int index = (2 * n) + ch;
                buffer[index] = m_bufferFloat[n] * buffer[index];
            }
        }
    }
    
    void setModAmp(float amp) { m_osc.setAmp(amp); }
    float getModFreq() { return m_osc.getFreq(); }
    void setModFreq(float freq) { m_osc.setFreq(freq); }

private:
    Oscillator m_osc;
    int m_bufferSamples;
    int m_bufferChannels;
    short* m_buffer;
    float* m_bufferFloat;
};

#endif // AUDIO_EFFECT_H