/*
 *  AudioBasics.h
 *  AudioButtonTest
 *
 *  Created by Michelle Daniels on 11/18/08.
 *  Copyright 2008 UCSD. All rights reserved.
 * 
 *  This is a central location for shared audio-related constants and helper functions
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

// math constants
static const float PI = 3.14159265359;
static const float TWO_PI = (2 * PI);

// amplitude
static const short AUDIO_MAX_AMPLITUDE = (1 << (AUDIO_BIT_DEPTH - 1)) - 1;

// helper functions
void PopulateAudioDescription(AudioStreamBasicDescription& desc);

#endif // AUDIO_BASICS_H