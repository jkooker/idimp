/*
 *  AudioQueueWrapper.h
 *  AudioButtonTest
 *
 *  Created by Michelle Daniels on 11/2/08.
 *  Copyright 2008 UCSD. All rights reserved.
 *
 */

#ifndef AUDIO_QUEUE_WRAPPER_H
#define AUDIO_QUEUE_WRAPPER_H

#import <math.h>
#import <stdlib.h>
#import <AudioToolbox/AudioToolbox.h>

static const int NUM_AUDIO_DATA_BUFFERS = 3;
static const int BYTES_PER_SAMPLE = 2;
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
        m_wavetable(NULL),
        m_hop(DEFAULT_FREQUENCY_IN_HZ * WAVETABLE_POINTS / SAMPLE_RATE),
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
    
    void setFreq(double freq)
    {
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
    
    double nextSample()
    {
        // rounding
        double sample = m_wavetable[(int)(m_nextSampleIndex + 0.5) % WAVETABLE_POINTS];
        m_nextSampleIndex += m_hop;
        if (m_nextSampleIndex >= WAVETABLE_POINTS)
        {
            m_nextSampleIndex -= WAVETABLE_POINTS;
        }
        return sample;
    }
    
    void nextSampleBuffer(short* buffer, int numSamples, int numChannels) // buffer size should be numSamples * numChannels
    {
        //printf("Oscillator::nextSampleBuffer\n");
        int scale = 1 << 14; // 1/2 of max value of 2^15 so we don't clip
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

    double* m_wavetable;
    double m_hop;
    double m_nextSampleIndex;
};

class AudioQueueWrapper 
{
public:

    AudioQueueWrapper();
    virtual ~AudioQueueWrapper();
        
    int InitQueue();    
    void AQBufferCallback(AudioQueueRef inQ, AudioQueueBufferRef outQB);
    
    bool isRunning() const;    
    void play();    
    void stop();    
    void pause();    
    void resume();
        
protected:
    UInt32 m_playPtr;
    UInt32 m_numSamplesToPlay;
    AudioQueueRef m_audioQueue;
    AudioStreamBasicDescription m_dataFormat;
    UInt32 m_samplesPerFramePerChannel;
    bool m_audioPlayerShouldStopImmediately;
    AudioQueueBufferRef m_buffers[NUM_AUDIO_DATA_BUFFERS];
    short* m_pcmBuffer;
    Oscillator m_osc;
};

#endif // AUDIO_QUEUE_WRAPPER_H