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
        //printf("AmplitudeScale::Process m_oldAmp = %f, m_amp = %f\n", m_oldAmp, m_amp);
        float amplitudeDelta = m_amp - m_oldAmp;
        for (int n = 0; n < numSamples; n++)
        {
            float percentage = (numSamples - n) / numSamples;
            float amp = m_amp - (amplitudeDelta * percentage);
            //printf("amp = %f\n", amp);
            // same content in both channels
            for (int ch = 0; ch < numChannels; ch++)
            {
                int index = (2 * n) + ch;
                buffer[index] = (short)(amp * buffer[index]);            
            }
        }
        m_oldAmp = m_amp;
    }
    
    void setAmp(float amp)
    {
        //printf("AmplitudeScale::Process setAmp amp = %f, m_amp = %f, m_oldAmp = %f\n", amp, m_amp, m_oldAmp);
        m_oldAmp = m_amp;
        m_amp = amp;
    }
    
private:
    float m_oldAmp;
    float m_amp;
};

#endif // AUDIO_EFFECT_H