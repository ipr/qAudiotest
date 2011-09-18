/////////////////////////////////////////////////////////
//
// CMaestro : MAESTRO audio format parser,
// format used by MaestroPro sound card?
//
// Based on documentation by: Richard Koerber
//
//
// Author: Ilkka Prusi, 2011
// Contact: ilkka.prusi@gmail.com
//


#ifndef MAESTRO_H
#define MAESTRO_H

#include <stdint.h>

#include "MemoryMappedFile.h"

// interface to define in audio-file
#include "AudioFile.h"

// playback status&control
#include "DecodeCtx.h"



class CMaestro : public AudioFile
{
private:
	CMemoryMappedFile m_File;
	
public:
    CMaestro(void);
	virtual ~CMaestro(void);
	
	virtual bool ParseFile(const std::wstring &szFileName);

	virtual bool isBigEndian()
	{
		return true;
	}
	
	virtual long channelCount()
	{
		return 0;
	}
	
	virtual unsigned long sampleRate()
	{
		return 0;
	}
	
	virtual long sampleSize()
	{
		return 0;
	}
	
	virtual bool isSigned()
	{
		return false;
	}
	
	// actual sample data
	virtual unsigned char *sampleData()
	{
		return NULL;
	}
	
	// total size of sample data
	virtual uint64_t sampleDataSize()
	{
		return 0;
	}
	
	virtual uint64_t decode(unsigned char *pBuffer, const uint64_t nBufSize /*, QAudioFormat *pOutput*/);
};

#endif // MAESTRO_H
