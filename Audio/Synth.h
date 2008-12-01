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

#import <UIKit/UIKit.h>
#import "Oscillator.h"

static const float DEFAULT_MIN_FREQUENCY_HZ = 20.0;
static const float DEFAULT_MAX_FREQUENCY_HZ = 10000.0;
static const float DEFAULT_MIN_AMPLITUDE = 0.0;
static const float DEFAULT_MAX_AMPLITUDE = 1.0;

static const int NUM_VOICES = 5;

// for drawing voices
static const float CIRCLE_RADIUS = 80;

class Voice
{
public:
    Voice() :
        m_xMax(1.0),
        m_yMax(1.0),
        m_minFreq(DEFAULT_MIN_FREQUENCY_HZ),
        m_freqRange(DEFAULT_MAX_FREQUENCY_HZ - DEFAULT_MIN_FREQUENCY_HZ),
        m_minAmp(DEFAULT_MIN_AMPLITUDE),
        m_ampRange(DEFAULT_MAX_AMPLITUDE - DEFAULT_MIN_AMPLITUDE),
        m_isOn(false)
    {
        setPosition(0.0, 0.0);
    }
    
    Voice(float x, float xMax, float y, float yMax) :
        m_xMax(xMax),
        m_yMax(yMax),
        m_minFreq(DEFAULT_MIN_FREQUENCY_HZ),
        m_freqRange(DEFAULT_MAX_FREQUENCY_HZ - DEFAULT_MIN_FREQUENCY_HZ),
        m_minAmp(DEFAULT_MIN_AMPLITUDE),
        m_ampRange(DEFAULT_MAX_AMPLITUDE - DEFAULT_MIN_AMPLITUDE),
        m_isOn(false)
    {
        setPosition(x, y);
    }
    
    void draw(CGContextRef contextRef, CGRect& bounds)
    {
        if (!m_isOn) return;
        
        float xratio = (m_x / bounds.size.width);
        float yratio = 1 - (m_y / bounds.size.height);
        
        CGContextSetRGBFillColor(contextRef, 0, xratio, 1 - xratio, 0.8 * yratio);
        CGContextSetRGBStrokeColor(contextRef, 0, xratio, 1 - xratio, 0.2 + 0.8 * yratio);
        CGContextSetLineWidth(contextRef, 3);
        
        // Draw a circle (filled)
        CGContextFillEllipseInRect(contextRef, CGRectMake(m_x - CIRCLE_RADIUS, m_y - CIRCLE_RADIUS, 2 * CIRCLE_RADIUS, 2 * CIRCLE_RADIUS));

        // Draw a circle (border only)
        CGContextStrokeEllipseInRect(contextRef, CGRectMake(m_x - CIRCLE_RADIUS, m_y - CIRCLE_RADIUS, 2 * CIRCLE_RADIUS, 2 * CIRCLE_RADIUS));
    }
    
    float getX() { return m_x; }
    float getY() { return m_y; }
    
    bool isOn() { return m_isOn; }
    
    void turnOn() 
    { 
        printf("Voice::turnOn %02X\n", this);
        m_isOn = true; 
    }
    
    void turnOff() 
    { 
        printf("Voice::turnOff %02X\n", this);
        m_isOn = false; 
    }
    
    void renderAddToBuffer(float* output, int numSamplesPerChannel, int numChannels)
    {
        if (m_isOn)
        {
            m_osc.addNextSamplesToBuffer(output, numSamplesPerChannel, numChannels);
        }
    }
    
    void setMaxX(float x) { m_xMax = x; }
    void setMaxY(float y) { m_yMax = y; }
    
    void setPosition(float x, float y)
    {
        // store new coordinates
        m_x = x;
        m_y = y;
        
        // update oscillator parameters based on new position
        // map y to frequency and x to amplitude
        m_osc.setFreq(m_minFreq + m_freqRange * (1.0 - m_y / m_yMax));
        m_osc.setAmp(m_minAmp + m_ampRange * (m_x / m_xMax));
    }
    
    void setWaveform(Oscillator::Waveform wave)
    {
        m_osc.setWaveform(wave);
    }
    
    void print()
    {
        printf("Voice %02X pos x = %f, y = %f, on = %d\n", this, m_x, m_y, m_isOn);
    }
    
protected:
    float m_x;
    float m_y;
    Oscillator m_osc;
    float m_xMax;
    float m_yMax;
    float m_minFreq;
    float m_freqRange;
    float m_minAmp;
    float m_ampRange;
    bool m_isOn;
};

class TouchSynth
{
public:
    TouchSynth() :
        m_silenceBuffer(NULL),
        m_silenceBufferSize(0)
    { }
    
    ~TouchSynth()
    {
        if (m_silenceBuffer != NULL)
        {
            delete m_silenceBuffer;
            m_silenceBuffer = NULL;
        }
    }
    
    bool addTouchVoice(float xPos, float yPos)
    {
        // find unused voice
        for (int i = 0; i < NUM_VOICES; i++)
        {
            if (!m_voices[i].isOn())
            {
                // found an unused voice
                m_voices[i].setPosition(xPos, yPos);
                //printf("TouchSynth::addTouchVoice added x = %f, y = %f\n", xPos, yPos);
                m_voices[i].turnOn();
                return (true);
            }
        }
        
        // no unused voice found
        return false;
    }
    
    bool updateTouchVoice(float xCurrent, float yCurrent, float xPrev, float yPrev)
    {
        for (int i = 0; i < NUM_VOICES; i++)
        {
            if (!m_voices[i].isOn()) continue;
            
            if (m_voices[i].getX() == xPrev && m_voices[i].getY() == yPrev)
            {
                // found a match - update its position
                m_voices[i].setPosition(xCurrent, yCurrent);
                return true;
            }
        }
        
        // no match found
        return false;
    }
    
    bool removeTouchVoice(float xPos, float yPos)
    {
        for (int i = 0; i < NUM_VOICES; i++)
        {
            if (!m_voices[i].isOn()) continue;
            
            if (m_voices[i].getX() == xPos && m_voices[i].getY() == yPos)
            {
                // found a match - turn it off
                m_voices[i].turnOff();
                return true;
            }
        }
        
        // no match found
        return false;
    }
    
    void removeAllVoices()
    {
        for (int i = 0; i < NUM_VOICES; i++)
        {
            m_voices[i].turnOff();
        }
    }
    
    void drawVoices(CGContextRef contextRef, CGRect& bounds)
    {
        for (int i = 0; i < NUM_VOICES; i++)
        {
            m_voices[i].draw(contextRef, bounds);
        }
    }
    
    void printVoices()
    {
        printf("PRINTING VOICES\n");
        for (int i = 0; i < NUM_VOICES; i++)
        {
            m_voices[i].print();
        }
    }
    
    void renderAudioBuffer(float* output, int numSamplesPerChannel, int numChannels)
    {
        // first zero out output buffer (can I use memset to do this??)
        int nTotalSamples = numSamplesPerChannel * numChannels; 
        allocate_silence_buffer(nTotalSamples);
        fill_buffer_with_silence(output, nTotalSamples);
        
        for (int v = 0; v < NUM_VOICES; v++)
        {
            m_voices[v].renderAddToBuffer(output, numSamplesPerChannel, numChannels);
        }
        
        // scale output by number of voices to avoid clipping
        if (NUM_VOICES > 1)
        {
            for (int n = 0; n < nTotalSamples; n++)
            {
                output[n] /= NUM_VOICES;
            }
        }
    }
    
    Voice* getVoices() { return m_voices; }
    
    int getNumVoices() const { return NUM_VOICES; }
    
    void setDisplayBounds(CGRect bounds)
    {
        for (int i = 0; i < NUM_VOICES; i++)
        {
            m_voices[i].setMaxX(bounds.size.width);
            m_voices[i].setMaxY(bounds.size.height);
        }
    }
    
    void setWaveform(Oscillator::Waveform wave)
    {
        // set the desired waveform for all voices
        for (int i = 0; i < NUM_VOICES; i++)
        {
            m_voices[i].setWaveform(wave);
        }
    }
    
private:

    void allocate_silence_buffer(int n)
    {
        // TODO: check to make sure n hasn't changed from what was originally allocated?
        m_silenceBufferSize = n;
        if (m_silenceBuffer == NULL)
        {
            // allocate silence buffer for future use
            m_silenceBuffer = new float[m_silenceBufferSize];
            for (int i = 0; i < m_silenceBufferSize; i++)
            {
                m_silenceBuffer[i] = 0.0f; // can this be done with memset instead?
            }
        }
    }

    void fill_buffer_with_silence(float* buffer, int n)
    {
        // make sure slience buffer has been initialized
        allocate_silence_buffer(n);
                
        // insert silence
        memcpy(buffer, m_silenceBuffer, n * sizeof(float));    

    }

    Voice m_voices[NUM_VOICES];
    float* m_silenceBuffer;
    int m_silenceBufferSize;
};

#endif // SYNTH_H