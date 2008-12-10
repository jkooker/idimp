/*
 *  AudioBasics.cpp
 *  AudioButtonTest
 *
 *  Created by Michelle Daniels on 11/18/08.
 *  Copyright 2008 UCSD. All rights reserved.
 *
 */

#include "AudioBasics.h"

void AudioSamplesFloatToShort(const float* in, short* out, int numSamples)
{
    const float* pIn = in;
    short* pOut = out;
    for (int n = 0; n < numSamples; n++)
    {
        *(pOut++) = (short)(*(pIn++) * AUDIO_MAX_AMPLITUDE);
    }
}

void AudioSamplesShortToFloat(const short* in, float* out, int numSamples)
{
    const short* pIn = in;
    float* pOut = out;
    for (int n = 0; n < numSamples; n++)
    {
        *(pOut++) = *(pIn++) / (float)AUDIO_MAX_AMPLITUDE;
    }
}

void AudioSamplesMixFloat2ToFloat(const float* in1, const float* in2, float* out1, int numSamples)
{
    const float* pIn1 = in1;
    const float* pIn2 = in2;
    float* pOut1 = out1;
    for (int n = 0; n < numSamples; n++)
    {
        *(pOut1++) = ( *(pIn1++) + *(pIn2++) ) / 2.0;
    }
}

void AudioSamplesMixFloat2ToFloat(const float* in1, const float* in2, const float* in3, float* out1, int numSamples)
{
    const float* pIn1 = in1;
    const float* pIn2 = in2;
    const float* pIn3 = in3;
    float* pOut1 = out1;
    for (int n = 0; n < numSamples; n++)
    {
        *(pOut1++) = ( *(pIn1++) + *(pIn2++) + *(pIn3++) ) / 3.0;
    }
}

void AudioSamplesMixFloat2ToShort(const float* in1, const float* in2, short* out1, int numSamples)
{
    const float* pIn1 = in1;
    const float* pIn2 = in2;
    short* pOut1 = out1;
    for (int n = 0; n < numSamples; n++)
    {
        *(pOut1++) = (short)(AUDIO_MAX_AMPLITUDE *  ((*(pIn1++) + *(pIn2++)) / 2.0));
    }
}

void AudioSamplesMixFloat3ToShort(const float* in1, const float* in2, const float* in3, short* out1, int numSamples)
{
    const float* pIn1 = in1;
    const float* pIn2 = in2;
    const float* pIn3 = in3;
    short* pOut1 = out1;
    for (int n = 0; n < numSamples; n++)
    {
        *(pOut1++) = (short)(AUDIO_MAX_AMPLITUDE *  (( *(pIn1++) + *(pIn2++) + *(pIn3++) ) / 3.0));
    }
}

void PopulateAudioDescription(AudioStreamBasicDescription& desc)
{
    // describe format
    /*desc.mSampleRate       = AUDIO_SAMPLE_RATE;
    desc.mFormatID		    = AUDIO_FORMAT_ID;
    desc.mFormatFlags      = AUDIO_FORMAT_FLAGS;
    desc.mFramesPerPacket  = AUDIO_FORMAT_FRAMES_PER_PACKET;
    desc.mChannelsPerFrame = AUDIO_NUM_CHANNELS;
    desc.mBitsPerChannel	= AUDIO_BIT_DEPTH;
    desc.mBytesPerPacket	= AUDIO_BIT_DEPTH_IN_BYTES * AUDIO_NUM_CHANNELS;
    desc.mBytesPerFrame	= AUDIO_BIT_DEPTH_IN_BYTES * AUDIO_NUM_CHANNELS;  */
    FillOutASBDForLPCM(desc, AUDIO_SAMPLE_RATE, AUDIO_NUM_CHANNELS, AUDIO_BIT_DEPTH, AUDIO_BIT_DEPTH, false, false, AUDIO_FORMAT_IS_NONINTERLEAVED);
}
