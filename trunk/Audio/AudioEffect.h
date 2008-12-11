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

/** AudioEffectParameter class.
 * The AudioEffectParameter class is a generic interface for parameters of an AudioEffect.
 */
class AudioEffectParameter
{
public:

   /**
    * AudioEffectParameter constructor.
    * @param displayName the name of this AudioEffectParameter for use in a GUI
    * @param description the description of this AudioEffectParameter for use in a GUI
    */
    AudioEffectParameter(const char* displayName, const char* description) :
        m_displayName(displayName),
        m_description(description),
        m_value(0.0f),
        m_minValue(FLT_MIN),
        m_maxValue(FLT_MAX)
    {
        // TODO: should we make our own copies of the name and description strings?
    }
    
   /**
    * get the display name of this AudioEffectParameter
    * @return the display name of this AudioEffectParameter
    */
    const char* getDisplayName() const { return m_displayName; }
    
   /**
    * get the description name of this AudioEffectParameter
    * @return the description of this AudioEffectParameter
    */
    const char* getDescription() const { return m_description; }
    
   /** 
    * get the value of this AudioEffectParameter
    * @return the value of this AudioEffectParameter
    * @see setValue
    */
    float getValue() const { return m_value; }
    
   /** 
    * set the value of this AudioEffectParameter
    * @param the new value of this AudioEffectParameter
    * @see getValue
    */
    void setValue(float value) { m_value = value; }

   /** 
    * get the minimum value of this AudioEffectParameter
    * @return the minimum value of this AudioEffectParameter
    * @see getMinValue
    */
    float getMinValue() const { return m_minValue; }
    
   /** 
    * set the minimum value of this AudioEffectParameter
    * @param the new minimum value of this AudioEffectParameter
    * @see getMinValue
    */
    void setMinValue(float minValue) { m_minValue = minValue; }

   /** 
    * get the maximum value of this AudioEffectParameter
    * @return the maximum value of this AudioEffectParameter
    * @see getMaxValue
    */
    float getMaxValue() const { return m_maxValue; }
    
   /** 
    * set the maximum value of this AudioEffectParameter
    * @param the new maximum value of this AudioEffectParameter
    * @see getMaxValue
    */
    void setMaxValue(float maxValue) { m_maxValue = maxValue; }

private:
    const char* m_displayName;
    const char* m_description;
    float m_value;
    float m_minValue;
    float m_maxValue;
};

/** AudioEffect class.
 * The AudioEffect class is an abstract base class for specific audio processing effects.
 */
class AudioEffect
{
public:

   /**
    * AudioEffect constructor.
    * @param numParams the number of AudioEffectParameters this AudioEffect has
    */
    AudioEffect(int numParams) :
        m_params(NULL),
        m_numParams(numParams)
    {
        // allocate array of AudioEffectParameter pointers and initialize them all to NULL
        m_params = new AudioEffectParameter*[m_numParams];
        for (int i = 0; i < m_numParams; i++)
        {
            m_params[i] = NULL;
        }
    }
    
   /**
    * AudioEffect desctructor
    */
    virtual ~AudioEffect() 
    {
        printf("AudioEffect::~AudioEffect\n");
        if (m_params != NULL)
        {
            // delete all parameters
            for (int i = 0; i < m_numParams; i++)
            {
                if (m_params[i] != NULL)
                {
                    delete m_params[i];
                    m_params[i] = NULL;
                }
            }
            
            // delete the array
            delete m_params;
            m_params = NULL;
        }
    }
    
   /**
    * Process the samples contained in the given buffer, applying an effect
    * @param buffer the buffer containing the samples to be processed. 
    * Processing occurs in-place and the results are placed in the same buffer.
    * @param numSamplesPerChannel the number of samples per channel in the buffer to be processed
    * @param numChannels the number of channels in the buffer to be processed.  Channel samples are interleaved.
    */
    virtual void Process(float* buffer, int numSamplesPerChannel, int numChannels) = 0;
    
    AudioEffectParameter* getParameter(int index) const
    {
        if (m_params == NULL || index >= m_numParams)
        {
            return NULL;
        }
        
        return m_params[index];
    }
    
   /** get the number of AudioEffectParameters controlling this AudioEffect
    * @return the number of AudioEffectParameters controlling this AudioEffect
    */
    int getNumParameters() const { return m_numParams; }
    
protected:
    AudioEffectParameter** m_params;
    int m_numParams;
};

/** AmplitudeScale class.
 * The AmplitudeScale class is an AudioEffect which scales the amplitude of the contents of a 
 * given audio buffer.  Amplitude changes occur gradually over the period of one audio buffer to 
 * avoid discontinuities with sudden changes.
 */
class AmplitudeScale : public AudioEffect
{
public:

   /**
    * AmplitudeScale constructor
    */
    AmplitudeScale() :
        AudioEffect(1),
        m_oldAmp(1.0)
    {
        m_params[0] = new AudioEffectParameter("Amplitude", "Amplitude for AmplitudeScale");
        m_params[0]->setValue(1.0);
        m_params[0]->setMinValue(0.0);
        m_params[0]->setMaxValue(1.0);
    }
    
   /**
    * Process the samples contained in the given buffer, applying the amplitude scaling effect
    * @param buffer the buffer containing the samples to be processed. 
    * Processing occurs in-place and the results are placed in the same buffer.
    * @param numSamplesPerChannel the number of samples per channel in the buffer to be processed
    * @param numChannels the number of channels in the buffer to be processed.  Channel samples are interleaved.
    */
    virtual void Process(float* buffer, int numSamplesPerChannel, int numChannels)
    {
        float goalAmp = m_params[0]->getValue();
        
        if (goalAmp == 0.0 && m_oldAmp == 0.0)
        {
            // just fill with silence
            memset(buffer, 0, numChannels * numSamplesPerChannel * sizeof(float));
            return;
        } 
        else if (goalAmp == 1.0 && m_oldAmp == 1.0)
        {
            // do nothing - unity gain
            return;
        }
        
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

private:
    float m_oldAmp;
};

static const float RING_MOD_MAX_FREQ_HZ = 5000.0;

/** RingMod class.
 * The RingMod class is an AudioEffect which performs classic ring modulation on the
 * given audio buffer.
 */
class RingMod : public AudioEffect
{
public:

   /** 
    * RingMod constructor
    */
    RingMod() :
        AudioEffect(2),
        m_bufferSamples(0),
        m_bufferFloat(NULL)
    {           
        m_params[0] = new AudioEffectParameter("Ring Mod Freq", "Frequency for Ring Mod Modulator");
        m_params[0]->setValue(0.0);
        m_params[0]->setMinValue(0.0);
        m_params[0]->setMaxValue(RING_MOD_MAX_FREQ_HZ);
        
        m_params[1] = new AudioEffectParameter("Ring Mod Amp", "Amplitude for Ring Mod Modulator");
        m_params[1]->setValue(1.0);
        m_params[1]->setMinValue(0.0);
        m_params[1]->setMaxValue(1.0);
    }
    
   /** 
    * RingMod destructor
    */
    virtual ~RingMod()
    {
        printf("RingMod::~RingMod\n");
        if (m_bufferFloat != NULL)
        {
            delete m_bufferFloat;
        }
    }
    
   /**
    * Process the samples contained in the given buffer, applying the ring modulation effect
    * @param buffer the buffer containing the samples to be processed. 
    * Processing occurs in-place and the results are placed in the same buffer.
    * @param numSamplesPerChannel the number of samples per channel in the buffer to be processed
    * @param numChannels the number of channels in the buffer to be processed.  Channel samples are interleaved.
    */
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
        
        m_osc.setFreq(m_params[0]->getValue());
        m_osc.setAmp(m_params[1]->getValue());
        
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
    
private:
    Oscillator m_osc;
    int m_bufferSamples;
    short* m_buffer;
    float* m_bufferFloat;
};

#endif // AUDIO_EFFECT_H