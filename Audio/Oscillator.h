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

/**
 * Oscillator class
 * Implements wavetable synthesis for sinusoids, square waves, sawtooth waves, and triangle waves.
 */
class Oscillator
{   
public:
   /**
    * Enum of different types of waveforms supported.
    */
    enum Waveform { 
        Sinusoid = 0, /*!< Sinusoid */ 
        SquareWave,   /*!< Square Wave */ 
        SawtoothWave, /*!< Sawtooth Wave */ 
        TriangleWave, /*!< Triangle Wave */ 
        NumWaveforms  /*!< Number of possible waveforms */ 
    };

   /** 
    * Oscillator constructor
    */
    Oscillator();
        
   /** 
    * Oscillator destructor
    */
    virtual ~Oscillator();
        
   /**
    * Get the current amplitude of this Oscillator
    * @return the amplitude in the range of [-1.0, 1.0]
    * @see setAmp
    */
    float getAmp() const;
    
   /**
    * Set the current ampitude of this Oscillator.
    * The amplitude will immediately jump to this new amplitude at the start of 
    * the next processing block.  If you want to avoid discontinuities due to these
    * sudden jumps, use setAmpSmooth instead.
    * @param the new amplitude
    * @see getAmp
    * @see setAmpSmooth
    */
    void setAmp(float amp);
    
   /**
    * Set the current ampitude of this Oscillator.
    * The amplitude will gradually change from the current amplitude to the new 
    * amplitude over the period of one processing block.
    * @param the new amplitude
    * @see getAmp
    * @see setAmp
    */
    void setAmpSmooth(float amp);
    
   /**
    * Get the current frequency of this Oscillator
    * @return the frequency in Hertz
    * @see setFreq
    */
    float getFreq() const;
   
   /**
    * Set the current frequency of this Oscillator
    * @param the new frequency
    * @see getFreq
    */ 
    void setFreq(float freq);
    
    /**
    * Switch to the next available waveform, in order of the Waveform enum
    */
    void incrementWaveform();
        
   /**
    * Get the current Waveform used to generate this Oscillator's wavetable
    * @return the Waveform used by this Oscillator
    * @see setWaveform
    */
    Waveform getWaveform() const;
    
   /**
    * Set the current Waveform used to generate this Oscillator's wavetable
    * @param wave the requested Waveform
    * @see getWaveform
    */
    void setWaveform(Waveform wave);
    
   /** 
    * Generate the next buffer of samples from this Oscillator's wavetable and store in a mono buffer
    * @param buffer the buffer in which to store the computed samples (previous buffer contents will be erased)
    * @param numSamples the number of samples to generate
    * @see nextSampleBuffer
    */
    void nextSampleBufferMono(float* buffer, int numSamples);
        
   /** 
    * Generate the next buffer of samples from this Oscillator's wavetable and store in a buffer 
    * with the given number of interleaved channels.
    * @param buffer the buffer in which to store the computed samples (previous buffer contents will be erased)
    * @param numSamples the number of samples to generate for each channel
    * @param numChannels the number of channels of samples to generate
    * @see nextSampleBufferMono
    * @see addNextSamplesToBuffer
    */
    void nextSampleBuffer(float* buffer, int numSamplesPerChannel, int numChannels);
    
   /** 
    * Generate the next buffer of samples from this Oscillator's wavetable and add them to a buffer 
    * with the given number of interleaved channels.
    * @param buffer the buffer in which to store the computed samples (added to the buffer's previous contents)
    * @param numSamples the number of samples to generate for each channel
    * @param numChannels the number of channels of samples to generate
    * @see nextSampleBuffer
    */
    void addNextSamplesToBuffer(float* buffer, int numSamplesPerChannel, int numChannels);
        
protected:
    Waveform m_waveform;
    float m_freq;
    float* m_wavetable;
    float m_hop;
    float m_nextSampleIndex;
    float m_amp;
    float m_oldAmp;
};

#endif // OSCILLATOR_H