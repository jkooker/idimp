/*
 *  Oscillator.cpp
 *  iDiMP
 *
 *  Created by Michelle Daniels on 12/8/08.
 *  Copyright 2008 UCSD. All rights reserved.
 *
 */

#include "Oscillator.h"

// ---- Oscillator public methods ----

Oscillator::Oscillator() :
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

Oscillator::~Oscillator()
{
    printf("Oscillator::~Oscillator\n");
    if (m_wavetable != NULL)
    {
        delete m_wavetable;
        m_wavetable = NULL;
    }
}

float Oscillator::getAmp() const { return m_amp; }

void Oscillator::setAmp(float amp) 
{ 
    //printf("Oscillator::setAmp amp = %f\n", amp);
    // TODO: modify Oscillator so that amplitude changes don't cause
    // sudden discontinuities in the waveform (smooth changes over some number of samples)
    m_amp = amp; 
}

float Oscillator::getFreq() const { return m_freq; }

void Oscillator::setFreq(float freq)
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

void Oscillator::incrementWaveform()
{
    setWaveform((Waveform)((m_waveform + 1) % NumWaveforms));
}

Oscillator::Waveform Oscillator::getWaveform() const { return m_waveform; }

void Oscillator::setWaveform(Waveform wave)
{
    m_waveform = wave;
    switch (m_waveform)
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

void Oscillator::nextSampleBufferMono(float* buffer, int numSamples)
{
    nextSampleBuffer(buffer, numSamples, 1);
}

void Oscillator::nextSampleBuffer(float* buffer, int numSamplesPerChannel, int numChannels)
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

void Oscillator::addNextSamplesToBuffer(float* buffer, int numSamplesPerChannel, int numChannels)
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

