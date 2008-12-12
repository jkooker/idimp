/**
 *  @file AudioBasics.h
 *  iDiMP
 *
 *  Created by Michelle Daniels on 11/18/08.
 *  Copyright 2008 UCSD. All rights reserved.
 * 
 *  This is a central location for shared audio-related constants and helper functions.
 */

#ifndef AUDIO_BASICS_H
#define AUDIO_BASICS_H

#import <AudioToolbox/AudioToolbox.h>

// audio format constants
static const float AUDIO_SAMPLE_RATE              = 44100.0;
static const int   AUDIO_NUM_CHANNELS             = 2;
static const int   AUDIO_BIT_DEPTH_IN_BYTES       = 2;
static const int   AUDIO_BIT_DEPTH                = 8 * AUDIO_BIT_DEPTH_IN_BYTES;
static const int   AUDIO_FORMAT_FRAMES_PER_PACKET = 1;
static const int   AUDIO_FORMAT_ID                = kAudioFormatLinearPCM;
static const int   AUDIO_FORMAT_FLAGS             = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
static const int   AUDIO_FORMAT_IS_NONINTERLEAVED = FALSE;

// for audio units
static const int AUDIO_OUTPUT_BUS = 0;
static const int AUDIO_INPUT_BUS = 1;

// math constants
static const float PI = 3.14159265359;
static const float TWO_PI = (2 * PI);

// amplitude
static const short AUDIO_MAX_AMPLITUDE = (1 << (AUDIO_BIT_DEPTH - 1)) - 1;

/** 
 * \fn void AudioSamplesFloatToShort(const float* in, short* out, int numSamples)
 * This function converts an array of audio samples from floats in the 
 * range [-1.0, 1.0] to 16-bit signed shorts.
 * @param in the input array of floats
 * @param out the output array of shorts
 * @param numSamples the number of samples to convert.
 */
void AudioSamplesFloatToShort(const float* in, 
                              short* out, 
                              int numSamples);
                              
/** 
 * \fn void AudioSamplesShortToFloat(const short* in, float* out, int numSamples)
 * This function converts an array of audio samples from 16-bit signed shorts to 
 * floats in the range [-1.0, 1.0].
 * @param in the input array of shorts
 * @param out the output array of floats
 * @param numSamples the number of samples to convert.
 */
void AudioSamplesShortToFloat(const short* in, 
                              float* out, 
                              int numSamples);
                              
/** 
 * \fn void AudioSamplesMixFloat2ToFloat(const float* in1, const float* in2, float* out1, int numSamples)
 * This function mixes two arrays of float audio samples into one array of floats.  Each input is scaled 
 * by 1/2 before summing to avoid clipping.
 * @param in1 the first input array of floats to be mixed
 * @param in2 the second input array of floats to be mixed
 * @param out1 the output array of mixed floats
 * @param numSamples the number of samples to mix.
 */
void AudioSamplesMixFloat2ToFloat(const float* in1, 
                                  const float* in2, 
                                  float* out1, 
                                  int numSamples);
                                  
/** 
 * \fn void AudioSamplesMixFloat3ToFloat(const float* in1, const float* in2, const float* in3, float* out1, int numSamples)
 * This function mixes three arrays of float audio samples into one array of floats.  Each input is scaled 
 * by 1/3 before summing to avoid clipping.
 * @param in1 the first input array of floats to be mixed
 * @param in2 the second input array of floats to be mixed
 * @param in3 the third input array of floats to be mixed
 * @param out1 the output array of mixed floats
 * @param numSamples the number of samples to mix.
 */
void AudioSamplesMixFloat3ToFloat(const float* in1, 
                                  const float* in2, 
                                  const float* in3,
                                  float* out1, 
                                  int numSamples);
                                 
/** 
 * \fn void AudioSamplesMixFloat2ToShort(const float* in1, const float* in2, short* out1, int numSamples)
 * This function mixes two arrays of float audio samples, each in the range [-1.0, 1.0] into one array of 
 * 16-bit signed shorts.  Each input is scaled by 1/2 before summing to avoid clipping.
 * @param in1 the first input array of floats to be mixed
 * @param in2 the second input array of floats to be mixed
 * @param out1 the output array of mixed shorts
 * @param numSamples the number of samples to mix.
 */
void AudioSamplesMixFloat2ToShort(const float* in1, 
                                  const float* in2, 
                                  short* out1, 
                                  int numSamples);
                                 
/** 
 * \fn void AudioSamplesMixFloat3ToShort(const float* in1, const float* in2, const float* in3, short* out1, int numSamples)
 * This function mixes three arrays of float audio samples, each in the range [-1.0, 1.0] into one array of 
 * 16-bit signed shorts.  Each input is scaled by 1/3 before summing to avoid clipping.
 * @param in1 the first input array of floats to be mixed
 * @param in2 the second input array of floats to be mixed
 * @param in3 the third input array of floats to be mixed
 * @param out1 the output array of mixed shorts
 * @param numSamples the number of samples to mix.
 */
void AudioSamplesMixFloat3ToShort(const float* in1, 
                                  const float* in2, 
                                  const float* in3,
                                  short* out1, 
                                  int numSamples);
                              
/** 
 * \fn void PopulateAudioDescription(AudioStreamBasicDescription& desc)
 * This is a helper function that populates the given AudioStreamBasicDescription struct with 
 * the correct parameters based on the format constants defined in AudioBasics.h
 * @param desc the AudioStreamBasicDescription struct to be populated
 */
void PopulateAudioDescription(AudioStreamBasicDescription& desc);

#endif // AUDIO_BASICS_H