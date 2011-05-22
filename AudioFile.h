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
	//virtual bool isIntegerSample() = 0; // is sample data integers (floats if false)
	//virtual bool isSignedIntegerSample() = 0; // is signed/unsigned
	//virtual long sampleDigitSize() = 0; // sizeof(digit) (byte/short/float..)
	
	virtual unsigned char *sampleData() = 0; // actual sample data (may need decode here)
	virtual unsigned long sampleDataSize() = 0; // total size of sample data
};

#endif // AUDIOFILE_H