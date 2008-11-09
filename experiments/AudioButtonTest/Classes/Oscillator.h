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
static const double DEFAULT_FREQUENCY_IN_HZ  = 440.0;
static const double DURATION_IN_SECONDS  = 4.0;
static const int NUM_CHANNELS = 2;
static const int WAVETABLE_POINTS = 2048;

class Oscillator
{   
public:
    Oscillator() :
        m_freq(DEFAULT_FREQUENCY_IN_HZ),
        m_wavetable(NULL),
        m_hop(m_freq * WAVETABLE_POINTS / SAMPLE_RATE),
        m_nextSampleIndex(0.0)
    {
        printf("Oscillator::Oscillator\n");
        m_wavetable = new double[WAVETABLE_POINTS];
        for (int i = 0; i < WAVETABLE_POINTS; i++)
        {
            m_wavetable[i] = cos(TWO_PI * i / WAVETABLE_POINTS);
        }
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
    
    float getFreq() { return m_freq; }
    
    void setFreq(float freq)
    {
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
    
    void nextSampleBuffer(short* buffer, int numSamples, int numChannels) // buffer size should be numSamples * numChannels
    {
        //printf("Oscillator::nextSampleBuffer\n");
        int scale = 1 << 14; // 1/2 of max value of 2^15 so we don't clip - could set amplitude as a parameter
        for (int n = 0; n < numSamples; n++)
        {
            // rounding
            double sample = scale * m_wavetable[(int)(m_nextSampleIndex + 0.5) % WAVETABLE_POINTS];
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
    float m_freq;
    double* m_wavetable;
    double m_hop;
    double m_nextSampleIndex;
};

#endif // OSCILLATOR_H