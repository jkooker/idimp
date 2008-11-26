/*
 *  Oscillator.h
 *  AudioButtonTest
 *
 *  Created by Michelle Daniels on 11/6/08.
 *  Copyright 2008 UCSD. All rights reserved.
 *
 */

#ifndef OSCILLATOR_H
#define OSCILLATOR_H

#import "AudioBasics.h"

static const float DEFAULT_FREQUENCY_IN_HZ  = 440.0;
static const float DEFAULT_AMPLITUDE = 1.0;
static const int WAVETABLE_POINTS = 2048;
static const int MAX_AMPLITUDE_16_BITS = 32767;

class Oscillator
{   
public:
    enum Waveform {
        Sinusoid = 0,
        SquareWave,
        SawtoothWave,
        TriangleWave,
        NumWaveforms
    };

    Oscillator() :
        m_waveform(Sinusoid),
        m_freq(DEFAULT_FREQUENCY_IN_HZ),
        m_wavetable(NULL),
        m_hop(m_freq * WAVETABLE_POINTS / AUDIO_SAMPLE_RATE),
        m_nextSampleIndex(0.0),
        m_amp(DEFAULT_AMPLITUDE)
    {
        printf("Oscillator::Oscillator\n");
        m_wavetable = new float[WAVETABLE_POINTS];
        setWaveform(m_waveform);
    }
    
    virtual ~Oscillator()
    {
        printf("Oscillator::~Oscillator\n");
        if (m_wavetable != NULL)
        {
            delete m_wavetable;
            m_wavetable = NULL;
        }
    }
    
    float getAmp() { return m_amp; }
    
    void setAmp(float amp) 
    { 
        //printf("Oscillator::setAmp amp = %f\n", amp);
        // TODO: modify Oscillator so that amplitude changes don't cause
        // sudden discontinuities in the waveform (smooth changes over some number of samples)
        m_amp = amp; 
    }
    
    float getFreq() { return m_freq; }
    
    void setFreq(float freq)
    {
        // check for valid range
        if (freq < -20000 || freq > 20000)
        {
            printf("Oscillator::setFreq frequency out of range: %f\n", freq);
            return;
        }
        m_freq = freq;
        m_nextSampleIndex -= m_hop;
        m_hop = freq * WAVETABLE_POINTS / AUDIO_SAMPLE_RATE;
        m_nextSampleIndex += m_hop;
        while (m_nextSampleIndex < 0)
        {
            m_nextSampleIndex += WAVETABLE_POINTS;
        }
        while (m_nextSampleIndex >= WAVETABLE_POINTS)
        {
            m_nextSampleIndex -= WAVETABLE_POINTS;
        }
    }
    
    void setWaveform(Waveform wave)
    {
        switch (wave)
        {
            case TriangleWave:
            {
                int halftable = WAVETABLE_POINTS / 2;
                // ramp up
                for (int i = 0; i < halftable; i++)
                {
                    m_wavetable[i] = (4.0 * i / (float) WAVETABLE_POINTS) - 1.0;
                }   
                // ramp down
                for (int i = halftable; i < WAVETABLE_POINTS; i++)
                {
                    m_wavetable[i] = 1.0 - (4.0 * (i - halftable) / (float) WAVETABLE_POINTS);
                }            
                break;
            }
            case SawtoothWave:
            {
                for (int i = 0; i < WAVETABLE_POINTS; i++)
                {
                    m_wavetable[i] = (2.0 * i / (float) WAVETABLE_POINTS) - 1.0;
                }            
                break;
            }
            case SquareWave:
            {
                int halftable = WAVETABLE_POINTS / 2;
                for (int i = 0; i < halftable; i++)
                {
                    m_wavetable[i] = 1.0;
                }   
                for (int i = halftable; i < WAVETABLE_POINTS; i++)
                {
                    m_wavetable[i] = -1.0;
                }            
                break;
            }
            default:    
            case Sinusoid:
            {
                for (int i = 0; i < WAVETABLE_POINTS; i++)
                {
                    m_wavetable[i] = cos(TWO_PI * i / WAVETABLE_POINTS);
                }            
                break;
            }
        }
    }
    
    void nextSampleBufferMono(float* buffer, int numSamples)
    {
        nextSampleBuffer(buffer, numSamples, 1);
    }
    
    void nextSampleBuffer(float* buffer, int numSamplesPerChannel, int numChannels)
    {
        float ampScalar = m_amp;
        for (int n = 0; n < numSamplesPerChannel; n++)
        {
            // same thing in all channels
            float nextSample = ampScalar * m_wavetable[(int)(m_nextSampleIndex + 0.5) % WAVETABLE_POINTS];
            for (int ch = 0; ch < numChannels; ch++)
            {
                // overwrite existing data
                buffer[(numChannels * n) + ch] = nextSample;
            }
            m_nextSampleIndex += m_hop;
            while (m_nextSampleIndex < 0)
            {
                m_nextSampleIndex += WAVETABLE_POINTS;
            }
            while (m_nextSampleIndex >= WAVETABLE_POINTS)
            {
                m_nextSampleIndex -= WAVETABLE_POINTS;
            }
        }
    }
    
    void addNextSamplesToBuffer(float* buffer, int numSamplesPerChannel, int numChannels)
    {
        float ampScalar = m_amp;
        for (int n = 0; n < numSamplesPerChannel; n++)
        {
            // same thing in all channels
            float nextSample = ampScalar * m_wavetable[(int)(m_nextSampleIndex + 0.5) % WAVETABLE_POINTS];
            for (int ch = 0; ch < numChannels; ch++)
            {
                // add to existing data - don't overwrite
                buffer[(numChannels * n) + ch] += nextSample;
            }
            m_nextSampleIndex += m_hop;
            while (m_nextSampleIndex < 0)
            {
                m_nextSampleIndex += WAVETABLE_POINTS;
            }
            while (m_nextSampleIndex >= WAVETABLE_POINTS)
            {
                m_nextSampleIndex -= WAVETABLE_POINTS;
            }
        }
    }
    
protected:
    Waveform m_waveform;
    float m_freq;
    float* m_wavetable;
    float m_hop;
    float m_nextSampleIndex;
    float m_amp;
};

#endif // OSCILLATOR_H