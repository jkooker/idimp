/*
 *  AudioBasics.cpp
 *  AudioButtonTest
 *
 *  Created by Michelle Daniels on 11/18/08.
 *  Copyright 2008 UCSD. All rights reserved.
 *
 */

#include "AudioBasics.h"

void PopulateAudioDescription(AudioStreamBasicDescription& desc)
{
    // describe format
    desc.mSampleRate       = AUDIO_SAMPLE_RATE;
    desc.mFormatID		    = AUDIO_FORMAT_ID;
    desc.mFormatFlags      = AUDIO_FORMAT_FLAGS;
    desc.mFramesPerPacket  = AUDIO_FORMAT_FRAMES_PER_PACKET;
    desc.mChannelsPerFrame = AUDIO_NUM_CHANNELS;
    desc.mBitsPerChannel	= AUDIO_BIT_DEPTH;
    desc.mBytesPerPacket	= AUDIO_BIT_DEPTH_IN_BYTES * AUDIO_NUM_CHANNELS;
    desc.mBytesPerFrame	= AUDIO_BIT_DEPTH_IN_BYTES * AUDIO_NUM_CHANNELS;  
}
