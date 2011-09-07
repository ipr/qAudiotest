/////////////////////////////////////////////////////////
//
// CRiffWave : RIFF-WAVE audio format parser
//
// (c) Ilkka Prusi, 2011
//
// See also: https://ccrma.stanford.edu/courses/422/projects/WaveFormat/
//

#ifndef RIFFWAVE_H
#define RIFFWAVE_H

#include <stdint.h>

// I can't be arsed to else,
// simple is best
#include "MemoryMappedFile.h"
#include "RiffContainer.h"

// interface to define in audio-file
#include "AudioFile.h"


// std::string, for keeping sample&copyright descriptions
#include <string>

// support for old-style decl
//
typedef uint8_t UBYTE;
//typedef int16_t WORD;
typedef uint16_t UWORD;
//typedef int32_t LONG;
//typedef uint32_t ULONG;

/* 
  Format tag (wFormatTag) supported values. Also affects additions fields in fmt-chunk.
  Some found in RIFF-WAVE spec, others by googling..
*/
#define fmt_WAVE_FORMAT_UNKNOWN         (0x0000) /* unused? */
#define fmt_WAVE_FORMAT_PCM             (0x0001)  /* Microsoft Pulse Code Modulation (PCM) format */
#define fmt_WAVE_FORMAT_ADPCM           (0x0002)
#define fmt_WAVE_FORMAT_ALAW            (0x0006)
#define fmt_WAVE_FORMAT_MULAW           (0x0007)
#define fmt_WAVE_FORMAT_OKI_ADPCM       (0x0010)
#define fmt_WAVE_FORMAT_DIGISTD         (0x0015)
#define fmt_WAVE_FORMAT_DIGIFIX         (0x0016)
#define fmt_IBM_FORMAT_MULAW            (0x0101) /* IBM mu-law format */
#define fmt_IBM_FORMAT_ALAW             (0x0102)  /* IBM a-law format */
#define fmt_IBM_FORMAT_ADPCM            (0x0103) /* IBM AVC Adaptive Differential Pulse Code Modulation format */


// from format specifications,
// note one-byte alignment in struct

#pragma pack(push, 1)

/* Format description struct, see also wFormatTag */
typedef struct 
{
    WORD     wFormatTag;    // Format category
    WORD     wChannels;     // Number of channels
    DWORD    dwSamplesPerSec;  // Sampling rate
    DWORD    dwAvgBytesPerSec; // For buffer estimation
    WORD     wBlockAlign;   // Data block size
} WAVEFormat;

/* cue point */
/*
typedef struct {
DWORD  dwName;
DWORD  dwPosition;
// has sub-chunk?
DWORD  dwChunkStart;
DWORD  dwBlockStart;
DWORD  dwSampleOffset;
} CuePoint;
*/

#pragma pack(pop)


class CRiffWave : public AudioFile, public CRiffContainer
{
private:
	CMemoryMappedFile m_File;
	//CIffHeader *m_pHead; // inherited now

protected:
	WAVEFormat m_WaveHeader; // fmt 
	WORD m_wBitsPerSample; // extension to 'fmt ' chunk when format is: fmt_WAVE_FORMAT_PCM
	
protected:
	//void Decode(CIffChunk *pChunk, CMemoryMappedFile &pFile);
	
	virtual void OnChunk(CIffChunk *pChunk, CMemoryMappedFile &pFile);
	
	virtual bool IsSupportedType(CIffHeader *pHeader)
	{
		if (pHeader->m_iTypeID == MakeTag("WAVE"))
		{
			return true;
		}
		return false;
	}
	
	CIffChunk *GetDataChunk() const
	{
		return GetChunkById(MakeTag("data"));
	}
	
public:
    CRiffWave(void);
    virtual ~CRiffWave(void);
	
	virtual bool ParseFile(const std::wstring &szFileName);

	virtual bool isBigEndian()
	{
		if (GetHeader()->m_iFileID == MakeTag("RIFX"))
		{
			return true;
		}
		return false;
	}
	virtual long channelCount()
	{
		return m_WaveHeader.wChannels;
	}
	virtual unsigned long sampleRate()
	{
		return m_WaveHeader.dwSamplesPerSec;
	}
	virtual long sampleSize()
	{
		// only when format is: fmt_WAVE_FORMAT_PCM
		// (other cases, see formats..)
		return m_wBitsPerSample;
	}

	virtual bool isSigned()
	{
		if (m_wBitsPerSample <= 8)
		{
			// unsigned ?
			return false;
		}
		// 16 or more
		return true;
	}
	
	// actual sample data
	virtual unsigned char *sampleData()
	{
		// locate datachunk and information
		CIffChunk *pDataChunk = GetDataChunk();
		if (pDataChunk == nullptr)
		{
			return nullptr;
		}

		// file was closed? -> error
		if (m_File.IsCreated() == false)
		{
			return nullptr;
		}
		
		// locate actual data
		return CRiffContainer::GetViewByOffset(pDataChunk->m_iOffset, m_File);
	}
	
	// total size of sample data
	virtual uint64_t sampleDataSize()
	{
		// locate datachunk and information
		CIffChunk *pDataChunk = GetDataChunk();
		if (pDataChunk == nullptr)
		{
			return 0;
		}
		return pDataChunk->m_iChunkSize;
	}
	
};

#endif // RIFFWAVE_H
