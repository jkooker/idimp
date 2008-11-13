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

static const double SAMPLE_RATE = 44100.0;
static const double PI = 3.14159265359;
static const double TWO_PI = (2 * PI);
static const float DEFAULT_FREQUENCY_IN_HZ  = 440.0;
static const float DEFAULT_AMPLITUDE = 1.0;
static const int WAVETABLE_POINTS = 2048;
static const int MAX_AMPLITUDE_16_BITS = 32767;

class Oscillator
{   
public:
    enum Waveform {
        Sinusoid = 0,
        SquareWave = 1
    };

    Oscillator() :
        m_waveform(Sinusoid),
        m_freq(DEFAULT_FREQUENCY_IN_HZ),
        m_wavetable(NULL),
        m_hop(m_freq * WAVETABLE_POINTS / SAMPLE_RATE),
        m_nextSampleIndex(0.0),
        m_amp(DEFAULT_AMPLITUDE)
    {
        printf("Oscillator::Oscillator\n");
        m_wavetable = new double[WAVETABLE_POINTS];
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
        m_amp = amp; 
    }
    
    float getFreq() { return m_freq; }
    
    void setFreq(float freq)
    {
        // check for valid range
        if (freq < 0 || freq > 20000) // TODO: it would be nice to handle negative frequencies - need to handle table wrap-around
        {
            printf("Oscillator::setFreq frequency out of range: %f\n", freq);
            return;
        }
        m_freq = freq;
        m_nextSampleIndex -= m_hop;
        m_hop = freq * WAVETABLE_POINTS / SAMPLE_RATE;
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
            case SquareWave:
            {
                int halftable = WAVETABLE_POINTS / 2;
                for (int i = 0; i < halftable; i++)
                {
                    m_wavetable[i] = 0.7;//1.0;
                }   
                for (int i = halftable; i < WAVETABLE_POINTS; i++)
                {
                    m_wavetable[i] = -0.7;//-1.0;
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
    
    void nextSampleBuffer(short* buffer, int numSamples, int numChannels, int scale) // buffer size should be numSamples * numChannels
    {
        float ampScalar = scale * m_amp;
        //printf("Oscillator::nextSampleBuffer ampScalar = %f\n", ampScalar);
        for (int n = 0; n < numSamples; n++)
        {
            // rounding
            double sample = ampScalar * m_wavetable[(int)(m_nextSampleIndex + 0.5) % WAVETABLE_POINTS];
            m_nextSampleIndex += m_hop;
            if (m_nextSampleIndex >= WAVETABLE_POINTS)
            {
                m_nextSampleIndex -= WAVETABLE_POINTS;
            }
            
            // same content in both channels
            for (int ch = 0; ch < numChannels; ch++)
            {
                buffer[(2 * n) + ch] = (short)sample;
            }
        }
    }
    
    
protected:
    Waveform m_waveform;
    float m_freq;
    double* m_wavetable;
    double m_hop;
    double m_nextSampleIndex;
    float m_amp;
};

#endif // OSCILLATOR_H