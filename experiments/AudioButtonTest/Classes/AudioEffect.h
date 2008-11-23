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

class AudioEffectParameter
{
public:

    AudioEffectParameter(const char* displayName, const char* description) :
        m_displayName(displayName),
        m_description(description),
        m_value(0.0f),
        m_minValue(FLT_MIN),
        m_maxValue(FLT_MAX)
    {
        // TODO: should we make our own copies of the name and description strings?
    }
    
    const char* getDisplayName() const { return m_displayName; }
    const char* getDescription() const { return m_description; }
    
    float getValue() const { return m_value; }
    void setValue(float value) { m_value = value; }

    float getMinValue() const { return m_minValue; }
    void setMinValue(float minValue) { m_minValue = minValue; }

    float getMaxValue() const { return m_maxValue; }
    void setMaxValue(float maxValue) { m_maxValue = maxValue; }

private:
    const char* m_displayName;
    const char* m_description;
    float m_value;
    float m_minValue;
    float m_maxValue;
};

class AudioEffect
{
public:
    AudioEffect() :
        m_params(NULL),
        m_numParams(0)
    {}
    
    virtual ~AudioEffect() {}
    virtual void Process(float* buffer, int numSamplesPerChannel, int numChannels) = 0;
    
    AudioEffectParameter* getParameter(int index) const
    {
        if (m_params == NULL || index >= m_numParams)
        {
            return NULL;
        }
        
        return &m_params[index];
    }
    
    int getNumParameters() const { return m_numParams; }
    
protected:
    AudioEffectParameter* m_params;
    int m_numParams;
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
    
    virtual void Process(float* buffer, int numSamplesPerChannel, int numChannels)
    {
        float goalAmp = m_amp;
        float amplitudeDelta = goalAmp - m_oldAmp;
        for (int n = 0; n < numSamplesPerChannel; n++)
        {
            float percentage = n / (float)numSamplesPerChannel;
            float amp = m_oldAmp + (amplitudeDelta * percentage);
            for (int ch = 0; ch < numChannels; ch++)
            {
                int index = (2 * n) + ch;
                buffer[index] = amp * buffer[index];            
            }
        }
        m_oldAmp = goalAmp;
    }

    float getAmp() { return m_amp; }
    
    void setAmp(float amp) { m_amp = amp; }
    
private:
    float m_oldAmp;
    float m_amp;
};

class RingMod : public AudioEffect
{
public:
    RingMod() :
        AudioEffect(),
        m_bufferSamples(0),
        m_bufferFloat(NULL)
    {   
        m_osc.setAmp(1.0); // TODO: somewhere we need to make sure we don't clip...
        m_osc.setFreq(0.0);
    }
    
    virtual ~RingMod()
    {
        if (m_bufferFloat != NULL)
        {
            delete m_bufferFloat;
        }
    }
    
    virtual void Process(float* buffer, int numSamplesPerChannel, int numChannels)
    {
        // allocate buffer if necessary
        if (m_bufferFloat == NULL)
        {
            m_bufferSamples = numSamplesPerChannel;
            m_bufferFloat = new float[m_bufferSamples];
        }
        // protect against buffer size mismatch
        else if (m_bufferSamples != numSamplesPerChannel)
        {
            printf("RingMod::Process expected buffer of %d samples but was given %d samples\n", m_bufferSamples, numSamplesPerChannel);
            return;
        }
        
        if (m_osc.getFreq() <= 0.0f)
        {
            // don't do anything if the modulator freq is zero
            return;
        }
        
        // fill buffer for modulating waveform
        m_osc.nextSampleBufferMono(m_bufferFloat, m_bufferSamples);
        
        // multiply input samples with modulating waveform
        for (int n = 0; n < numSamplesPerChannel; n++)
        {
            float modSample = m_bufferFloat[n];
            for (int ch = 0; ch < numChannels; ch++)
            {
                buffer[(2 * n) + ch] *= modSample;
            }
        }
    }
    
    void setModAmp(float amp) { m_osc.setAmp(amp); }
    float getModFreq() { return m_osc.getFreq(); }
    void setModFreq(float freq) { m_osc.setFreq(freq); }

private:
    Oscillator m_osc;
    int m_bufferSamples;
    short* m_buffer;
    float* m_bufferFloat;
};

#endif // AUDIO_EFFECT_H