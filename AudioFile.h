/////////////////////////////////////////////////////////
//
// AudioFile.h : pure virtual interface
// to implement in audio-parsing file.
//
// (c) Ilkka Prusi, 2011
//


#ifndef AUDIOFILE_H
#define AUDIOFILE_H

#include <stdint.h>
#include <string>

class AudioFile
{
protected:
	// must be inherited to create instance
	AudioFile(void) 
	{}

public:
	// "anyone" can destroy inherited..
	virtual ~AudioFile(void) 
	{}
	
	virtual bool ParseFile(const std::wstring &szFileName) = 0;

	// values to use for QAudioFormat or similar
	//codec (PCM-coded)
	
	virtual bool isBigEndian() = 0; // if big-endian data
	virtual long channelCount() = 0; // count of channels
	virtual unsigned long sampleRate() = 0; // sample rate
	virtual long sampleSize() = 0; // size of single sample

	// TODO: ?
	//virtual bool isInterleaved() = 0; // is interleaved channel data?
	//virtual bool isStereo() = 0; // is stereo data? (from channel count only..?)
	virtual bool isSigned() = 0; // is signed data?
	//virtual bool isIntegerSample() = 0; // is sample data integers (floats if false)
	//virtual bool isSignedIntegerSample() = 0; // is signed/unsigned
	//virtual long sampleDigitSize() = 0; // sizeof(digit) (byte/short/float..)
	
	virtual unsigned char *sampleData() = 0; // actual raw sample data 
	virtual uint64_t sampleDataSize() = 0; // total size of sample data
    
    // optional decoding for playback
    // TODO: additional options for conversion..?
    virtual uint64_t decode(unsigned char *pBuffer, const uint64_t nBufSize) 
    {
        // nothing written to output
        return 0;
    }
    
    /*
    // optional encoding&storage for recoding?
    virtual uint64_t encode(const unsigned char *pData, const uint64_t nDataSize)
    {
        // nothing encoded from input
        return 0;
    }
    */
};

#endif // AUDIOFILE_H
