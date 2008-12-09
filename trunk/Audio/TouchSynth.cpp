/*
 *  TouchSynth.cpp
 *  iDiMP
 *
 *  Created by Michelle Daniels on 12/8/08.
 *  Copyright 2008 UCSD. All rights reserved.
 *
 */

#include "TouchSynth.h"

// ---- Voice public methods ----

Voice::Voice() :
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

void Voice::draw(CGContextRef contextRef, CGRect& bounds) const
{
    if (!m_isOn) return;
    
    float xratio = (m_x / bounds.size.width);
    float yratio = 1 - (m_y / bounds.size.height);
    
    CGContextSetRGBFillColor(contextRef, 0, xratio, 1 - xratio, 0.8 * yratio);
    CGContextSetRGBStrokeColor(contextRef, 0, xratio, 1 - xratio, 0.2 + 0.8 * yratio);
    CGContextSetLineWidth(contextRef, 3);
    
    CGRect rect = CGRectMake(m_x - CIRCLE_RADIUS, m_y - CIRCLE_RADIUS, 2 * CIRCLE_RADIUS, 2 * CIRCLE_RADIUS);
    
    switch (m_osc.getWaveform()) 
    {
        case Oscillator::SquareWave:
            // Draw a square (filled)
            CGContextFillRect(contextRef, rect);
            // Draw a square (border only)
            CGContextStrokeRect(contextRef, rect);
            break;
            
        case Oscillator::SawtoothWave:
            // Generate downward pointing triangle path
            CGContextBeginPath(contextRef);
            CGContextMoveToPoint(contextRef, rect.origin.x, rect.origin.y);
            CGContextAddLineToPoint(contextRef, rect.origin.x + rect.size.width / 2, rect.origin.y + rect.size.height);
            CGContextAddLineToPoint(contextRef, rect.origin.x + rect.size.width, rect.origin.y);
            CGContextClosePath(contextRef);
            CGContextDrawPath(contextRef, kCGPathFillStroke);
            break;
            
        case Oscillator::TriangleWave:
        
            // Generate upward pointing triangle path
            CGContextBeginPath(contextRef);
            CGContextMoveToPoint(contextRef, rect.origin.x, rect.origin.y + rect.size.height);
            CGContextAddLineToPoint(contextRef, rect.origin.x + rect.size.width / 2, rect.origin.y);
            CGContextAddLineToPoint(contextRef, rect.origin.x + rect.size.width, rect.origin.y + rect.size.height);
            CGContextClosePath(contextRef);
            CGContextDrawPath(contextRef, kCGPathFillStroke);
            break;
            
        default:
        case Oscillator::Sinusoid:
            // Draw a circle (filled)
            CGContextFillEllipseInRect(contextRef, rect);
            // Draw a circle (border only)
            CGContextStrokeEllipseInRect(contextRef, rect);
            break;
    }
}


// ---- TouchSynth public methods ----

TouchSynth::TouchSynth() :
    m_silenceBuffer(NULL),
    m_silenceBufferSize(0)
{ }

TouchSynth::~TouchSynth()
{
    if (m_silenceBuffer != NULL)
    {
        delete m_silenceBuffer;
        m_silenceBuffer = NULL;
    }
}

bool TouchSynth::addTouchVoice(float xPos, float yPos)
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

bool TouchSynth::updateTouchVoice(float xCurrent, float yCurrent, float xPrev, float yPrev)
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

bool TouchSynth::removeTouchVoice(float xPos, float yPos)
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

void TouchSynth::removeAllVoices()
{
    for (int i = 0; i < NUM_VOICES; i++)
    {
        m_voices[i].turnOff();
    }
}

void TouchSynth::drawVoices(CGContextRef contextRef, CGRect& bounds) const
{
    for (int i = 0; i < NUM_VOICES; i++)
    {
        m_voices[i].draw(contextRef, bounds);
    }
}

void TouchSynth::printVoices() const
{
    printf("PRINTING VOICES\n");
    for (int i = 0; i < NUM_VOICES; i++)
    {
        m_voices[i].print();
    }
}

void TouchSynth::renderAudioBuffer(float* output, int numSamplesPerChannel, int numChannels)
{
    // first zero out output buffer (can I use memset to do this??)
    int nTotalSamples = numSamplesPerChannel * numChannels; 
    
    // init buffer with silence
    memset(output, 0, nTotalSamples * sizeof(float));
    
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

bool TouchSynth::allVoicesAreOff() const
{
    for (int i = 0; i < NUM_VOICES; i++)
    {
        if (m_voices[i].isOn()) return false;
    }
    return true;
}

void TouchSynth::setDisplayBounds(CGRect bounds)
{
    for (int i = 0; i < NUM_VOICES; i++)
    {
        m_voices[i].setMaxX(bounds.size.width);
        m_voices[i].setMaxY(bounds.size.height);
    }
}

void TouchSynth::incrementWaveform()
{
    // set the waveform to the next one in the list
    for (int i = 0; i < NUM_VOICES; i++)
    {
        m_voices[i].incrementWaveform();
    }
}

void TouchSynth::setWaveform(Oscillator::Waveform wave)
{
    // TODO: check to make sure that wave is a valid value??
    // set the desired waveform for all voices
    for (int i = 0; i < NUM_VOICES; i++)
    {
        m_voices[i].setWaveform(wave);
    }
}