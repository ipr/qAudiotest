/////////////////////////////////////////////////////////
//
// CRiffWave : RIFF-WAVE audio format parser
//
// (c) Ilkka Prusi, 2011
//

#ifndef RIFFWAVE_H
#define RIFFWAVE_H

#include <stdint.h>

// I can't be arsed to else,
// simple is best
#include "MemoryMappedFile.h"
#include "RiffContainer.h"

// support for old-style decl
//
typedef uint8_t UBYTE;
//typedef int16_t WORD;
typedef uint16_t UWORD;
//typedef int32_t LONG;
//typedef uint32_t ULONG;

/* Format tag (wFormatTag) supported values. Also affects additions fields in fmt-chunk */
#define fmt_WAVE_FORMAT_PCM (0x0001)  /* Microsoft Pulse Code Modulation (PCM) format */
#define fmt_IBM_FORMAT_MULAW (0x0101) /* IBM mu-law format */
#define fmt_IBM_FORMAT_ALAW (0x0102)  /* IBM a-law format */
#define fmt_IBM_FORMAT_ADPCM (0x0103) /* IBM AVC Adaptive Differential Pulse Code Modulation format */


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


class CRiffWave : public CRiffContainer
{
private:
	CMemoryMappedFile m_File;
	//CIffHeader *m_pHead; // inherited now

protected:
	WAVEFormat m_WaveHeader; // fmt
	
protected:
	virtual void OnChunk(CIffChunk *pChunk, CMemoryMappedFile &pFile);
	
	virtual bool IsSupportedType(CIffHeader *pHeader)
	{
		if (pHeader->m_iTypeID == MakeTag("WAVE"))
		{
			return true;
		}
		return false;
	}
	
public:
    CRiffWave(void);
    virtual ~CRiffWave(void);
	
	bool ParseFile(LPCTSTR szPathName);
	
};

#endif // RIFFWAVE_H
