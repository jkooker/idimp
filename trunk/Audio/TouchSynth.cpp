// Copyright (c) 2009 Michelle Daniels and John Kooker
// 
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
// 
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.

/*
 *  TouchSynth.cpp
 *  iDiMP
 *
 *  Created by Michelle Daniels on 12/8/08.
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
    m_isOn(false),
    m_turnOffRequested(false)
{
    setPosition(0.0, 0.0);
}

void Voice::draw(CGContextRef contextRef, CGRect& bounds) const
{
    if (!m_isOn || m_turnOffRequested) return;
    
    float xratio = (m_x / bounds.size.width);
    float yratio = 1 - (m_y / bounds.size.height);
    
    CGContextSetRGBFillColor(contextRef, 0, yratio, 1 - yratio, 0.8 * xratio);
    CGContextSetRGBStrokeColor(contextRef, 0, yratio, 1 - yratio, 0.2 + 0.8 * xratio);
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
            // Generate (sawtooth) right triangle path
            CGContextBeginPath(contextRef);
            CGContextMoveToPoint(contextRef, rect.origin.x - rect.size.width * 0.2, rect.origin.y + rect.size.height - rect.size.height * 0.2);
            CGContextAddLineToPoint(contextRef, rect.origin.x + rect.size.width - rect.size.width * 0.2, rect.origin.y + rect.size.height - rect.size.height * 0.2);
            CGContextAddLineToPoint(contextRef, rect.origin.x + rect.size.width - rect.size.width * 0.2, rect.origin.y - rect.size.height * 0.2);
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

void Voice::turnOn(float x, float y) 
{ 
    printf("Voice::turnOn %02X\n", this);
    
    // store new coordinates
    m_x = x;
    m_y = y;
    
    // set oscillator parameters based on position
    // map y to frequency and x to amplitude
    m_osc.setFreq(m_minFreq + m_freqRange * (1.0 - m_y / m_yMax));
    
    // set the amplitude to zero first so we will ramp up to the starting amplitude over the period of one buffer
    // this will avoid the clicks due to sudden turning on of voices
    m_osc.setAmp(0.0);
    m_osc.setAmpSmooth(m_minAmp + m_ampRange * (m_x / m_xMax));
    
    m_isOn = true; 
}

void Voice::turnOff() 
{ 
    printf("Voice::turnOff %02X\n", this);
    m_osc.setAmpSmooth(0.0);
    m_turnOffRequested = true; // don't turn off until after next callback - this allows smooth ramping down to zero
}

void Voice::renderAddToBuffer(float* output, int numSamplesPerChannel, int numChannels)
{
    if (m_isOn)
    {
        m_osc.addNextSamplesToBuffer(output, numSamplesPerChannel, numChannels);
    
        if (m_turnOffRequested)
        {
            m_isOn = false;
            m_turnOffRequested = false;
        }
    }
}

void Voice::setPosition(float x, float y)
{
    // store new coordinates
    m_x = x;
    m_y = y;
    
    // update oscillator parameters based on new position
    // map y to frequency and x to amplitude
    m_osc.setFreq(m_minFreq + m_freqRange * (1.0 - m_y / m_yMax));
    m_osc.setAmpSmooth(m_minAmp + m_ampRange * (m_x / m_xMax));
}

// ---- TouchSynth public methods ----

TouchSynth::TouchSynth()
{ }

bool TouchSynth::addTouchVoice(float xPos, float yPos)
{
    // find unused voice
    for (int i = 0; i < NUM_VOICES; i++)
    {
        if (!m_voices[i].isOn())
        {
            // found an unused voice
            m_voices[i].turnOn(xPos, yPos);
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

Oscillator::Waveform TouchSynth::getWaveform() const
{
    // TODO: check to see if NUM_VOICES > 0 ??
    return m_voices[0].getWaveform();
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