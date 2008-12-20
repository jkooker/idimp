/**
 *  @file Wavefile.h
 *  iDiMP
 *
 *  Created by Michelle Daniels on 11/18/08.
 *  Copyright 2008 UCSD. All rights reserved.
 *
 *  This file defines the interface for the Wavefile class.
 */

#ifndef WAVEFILE_H
#define WAVEFILE_H

#import "AudioBasics.h"

static const CFStringEncoding DEFAULT_STRING_ENCODING = kCFStringEncodingMacRoman; ///< Default string encoding type - needed for generating file URLs

/**
 * Wavefile class.
 * This class encapsulates writing audio to a PCM wave file.
 * iDiMP uses it in debug mode to store audio output for analysis.
 */
class Wavefile
{
public:
   /**
    * Wavefile constructor.
    * The wavefile to be written is created and opened in this method.  
    * If the file already exists, it will be overwritten.
    * @param filename output audio file name - currently, it is assumed that this file lives 
    * in the default directory and providing a full file path/URL is not implemented as an option.
    */
    Wavefile(const char* filename) :
        m_debugFileID(0),
        m_debugFileByteOffset(0)
    {
        // Describe format
        AudioStreamBasicDescription audioFormat;
        PopulateAudioDescription(audioFormat);
        
        // create file URL from file name
        CFStringRef filenameRef = CFStringCreateWithCString(NULL, filename, DEFAULT_STRING_ENCODING);
        printf("Wavefile::Wavefile filenameRef = %s\n", CFStringGetCStringPtr(filenameRef, DEFAULT_STRING_ENCODING));
        
        CFURLRef urlRef = CFURLCreateWithString(NULL, filenameRef, NULL);
        printf("Wavefile::Wavefile urlRef = %s\n", CFStringGetCStringPtr(CFURLGetString(urlRef), DEFAULT_STRING_ENCODING));
        
        // create the audio file (NOTE: this also opens the file)
        OSStatus result = noErr;
        result = AudioFileCreateWithURL(urlRef, 
                                        kAudioFileWAVEType,
                                        &audioFormat,
                                        kAudioFileFlags_EraseFile,
                                        &m_debugFileID);	
        if (result != noErr)
        {
            printf("Wavefile::Wavefile error creating debug file: %d\n", result);
        }
        
        // release memory allocated above
        CFRelease(urlRef);
        CFRelease(filenameRef);
        
    }
    
   /**
    * Wavefile destructor.
    * This method closes the wavefile.
    */
    ~Wavefile()
    {
        if (m_debugFileID > 0)
        {
            AudioFileClose(m_debugFileID);
        }
    }
    
   /**
    * Write the contents of the given audio buffers to the file.
    * @param bufferList a pointer to the AudioBufferList object which contains the audio data to be written.
    */
    void WriteAudioBuffers(AudioBufferList* bufferList)
    {
        //printf("Wavefile::WriteAudioBuffers m_debugFileID = %d, m_debugFileByteOffset = %d, bufferList = %d\n", m_debugFileID, m_debugFileByteOffset, bufferList);
        if (m_debugFileID == 0)
        {
            // file was never properly initialized so we can't write to it
            return;
        }
        for (int i = 0; i < bufferList->mNumberBuffers; i++)
        {
            // write to debug file
            UInt32 numBytes = bufferList->mBuffers[i].mDataByteSize;
            //printf("Wavefile::WriteAudioBuffers i = %d, numBytes = %d\n", i, numBytes);
            OSStatus result = noErr;
            result = AudioFileWriteBytes(m_debugFileID,
                                         FALSE,
                                         m_debugFileByteOffset,
                                         &numBytes, // this should hold actual num bytes written upon return
                                         bufferList->mBuffers[i].mData);
            //printf("Wavefile::WriteAudioBuffers wrote bytes: numBytes = %d\n", numBytes);
            if (result != noErr)
            {
                printf("Wavefile::WriteAudioBuffers error writing to debug file: %d\n", result);
            }
            if (numBytes < bufferList->mBuffers[i].mDataByteSize)
            {
                printf("Wavefile::WriteAudioBuffers warning: some bytes were not written to the debug file\n");
            }
            m_debugFileByteOffset += numBytes;
        }
    }
    
private:
    AudioFileID m_debugFileID;
    SInt64 m_debugFileByteOffset;
};

#endif