/*
 *  TouchSynth.h
 *  iDiMP
 *
 *  Created by Michelle Daniels on 12/8/08.
 *  Copyright 2008 UCSD. All rights reserved.
 *
 */

#ifndef TOUCH_SYNTH_H
#define TOUCH_SYNTH_H

#import <UIKit/UIKit.h>
#import "Oscillator.h"

static const float DEFAULT_MIN_FREQUENCY_HZ = 20.0;
static const float DEFAULT_MAX_FREQUENCY_HZ = 3000.0;
static const float DEFAULT_MIN_AMPLITUDE = 0.0;
static const float DEFAULT_MAX_AMPLITUDE = 1.0;

static const int NUM_VOICES = 5;

// for drawing voices
static const float CIRCLE_RADIUS = 80;

/** Voice class.
 * The Voice class encapsulates both audio synthesis and graphical UI rendering for one Voice
 */
class Voice
{
public:
   /**
    * Voice constructor
    */
    Voice();
        
   /**
    * Draw a representation of this Voice in the given graphical context within the given bounds
    * @param contextRef the graphical context into which this Voice will be rendered
    * @param bounds the bounding rectangle into which this Voice will be rendered
    */
    void draw(CGContextRef contextRef, 
              CGRect& bounds) const;
    
   /**
    * Get the current X position of this voice on the screen.
    * @return the current X position of this voice on the screen.
    */
    float getX() const { return m_x; }
    
   /**
    * Get the current Y position of this voice on the screen.
    * @return the current Y position of this voice on the screen.
    */
    float getY() const { return m_y; }
    
   /**
    * Query whether or not this voice is currently on (active).
    * @return true if the voice is on, false otherwise
    * @see turnOn
    * @see turnOff
    */
    bool isOn() const { return m_isOn; }
    
   /**
    * Turn on this voice with the given X,Y coordinates as its starting position.
    * @param x the starting X position of this voice.
    * @param y the starting Y position of this voice.
    * @see isOn
    * @see turnOff
    */
    void turnOn(float x, float y);
        
   /** 
    * Turn off this voice.
    * @see isOn
    * @see turnOn
    */
    void turnOff();
        
   /** 
    * Render this voice's audio and add it to the audio in the given buffer.
    * Assumes that the buffer has already been initialized with silence or the desired added audio.
    * @param output the buffer into which this voice's audio data will be rendered/added
    * @numSamplesPerChannel the number of samples per channel to be rendered
    * @numChannels the number of channels to be rendered (Note: samples are interleaved) 
    */
    void renderAddToBuffer(float* output, int numSamplesPerChannel, int numChannels);
        
   /**
    * Set the maximum X position on the screen (generally screen width)
    * @param x the new maximum X position
    * @see setMaxY
    */ 
    void setMaxX(float x) { m_xMax = x; }
    
   /** 
    * Set the maximum Y position on the screen (generally screen height)
    * @param y the new maximum Y position
    * @see setMaxX
    */
    void setMaxY(float y) { m_yMax = y; }
    
   /**
    * Set the current position of this voice on the screen
    * @param x the new X position of this voice
    * @param y the new Y position of this voice
    */
    void setPosition(float x, float y);
        
   /**
    * Increment the Waveform used by this voice to the next one in the list of Oscillator::Waveforms.
    * @see setWaveform
    */    
    void incrementWaveform()
    {
        m_osc.incrementWaveform();
    }

   /**
    * Set the Waveform used by this voice to the given Waveform.
    * @param wave the Waveform for this voice to use
    * @see incrementWaveform
    */
    void setWaveform(Oscillator::Waveform wave)
    {
        m_osc.setWaveform(wave);
    }
    
   /**
    * Print debugging information about this voice (X and Y positions and whether or not it is on).
    */
    void print() const
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

/** TouchSynth class.
 * TouchSynth manages a collection of synthesized voices, encapsulating both 
 * audio synthesis and graphical rendering for the UI.
 */
class TouchSynth
{
public:
   /**
    * TouchSynth constructor
    */
    TouchSynth();
    
   /**
    * Add a Voice to this TouchSynth
    * @param xPos the starting horizontal position of the Voice on the touch screen
    * @param yPos the starting vertical position of the Voice on the touch screen
    * @return true if the Voice was successfully added, false if there was no room for an additional voice
    * @see updateTouchVoice
    * @see removeTouchVoice
    */
    bool addTouchVoice(float xPos, 
                       float yPos);  
    
   /**
    * Update a member voice of this TouchSynth
    * @param xCurrent the current horizontal position of the Voice on the touch screen
    * @param yCurrent the current vertical position of the Voice on the touch screen
    * @param xPrev the previous horizontal position of the Voice on the touch screen
    * @param yPrev the previous vertical position of the Voice on the touch screen
    * @return true if the requested voice was found and updated in this TouchSynth, false otherwise
    * @see addTouchVoice
    * @see removeTouchVoice
    */  
    bool updateTouchVoice(float xCurrent, 
                          float yCurrent, 
                          float xPrev, 
                          float yPrev);
                              
   /**
    * Remove a Voice from this TouchSynth
    * @param xPos the horizontal position on the touch screen of the Voice to be removed
    * @param yPos the vertical position on the touch screen of the Voice to be removed
    * @return true if the requested voice was found and removed from this TouchSynth, false otherwise
    * @see addTouchVoice
    * @see updateTouchVoice
    * @see removeAllVoices
    */
    bool removeTouchVoice(float xPos, 
                          float yPos);
                              
   /**
    * Remove all Voices from this TouchSynth
    * @see removeTouchVoice
    */
    void removeAllVoices(); 
    
   /**
    * draw the Voices of this TouchSynth into the given graphical context
    * @param contextRef the graphical context into which the Voices will be rendered
    * @param bounds the bounding rectangle into which the Voices will be rendered
    */      
    void drawVoices(CGContextRef contextRef, 
                    CGRect& bounds) const;    
    
   /**
    * print debugging information for each Voice in this TouchSynth
    */  
    void printVoices() const;    
    
   /**
    * synthesize the audio for each Voice and render into the given output buffer
    * @param buffer the output buffer into which the audio for the Voices will be rendered
    * @param numSamplesPerChannels the number of samples per channel in the output buffer
    * @param numChannels the number of audio channels in the audio buffer (interleaved)
    */  
    void renderAudioBuffer(float* output, 
                           int numSamplesPerChannel, 
                           int numChannels);
                           
    
   /**
    * determines whether or not all Voices in this TouchSynth are currently turned off (disabled)
    * @return true if all voices are off, false otherwise
    */ 
    bool allVoicesAreOff() const;
        
   /**
    * sets new display bounds for this TouchSynth - bounds are used to determine correct ranges of parameters
    * @param bounds the bounding rectangle into which this TouchSynth is rendered
    */
    void setDisplayBounds(CGRect bounds);
    
   /**
    * Switch each Voice in this TouchSynth to the next Waveform in the list
    */
    void incrementWaveform();
      
   /**
    * Set the Waveform for each voice in this TouchSynth to the given Waveform.
    * @param wave the requested Waveform
    */
    void setWaveform(Oscillator::Waveform wave);
        
private:

    Voice m_voices[NUM_VOICES];
};

#endif // TOUCH_SYNTH_H