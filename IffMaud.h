/////////////////////////////////////////////////////////
//
// CIffMaud : IFF-MAUD audio format parser,
// multiple sample-sizes, compression options etc.
// (upto 16-bit or more?)
//
// Based on documentation by: Richard Koerber
//
//
// Author: Ilkka Prusi, 2011
// Contact: ilkka.prusi@gmail.com
//

#ifndef IFFMAUD_H
#define IFFMAUD_H

#include <stdint.h>

#include "MemoryMappedFile.h"
#include "IffContainer.h"

// interface to define in audio-file
#include "AudioFile.h"

// playback status&control
#include "DecodeCtx.h"

// std::string, for keeping sample&copyright descriptions
#include <string>


//// values from: maud.i

/* mhdr_ChannelInfo
MCI_MONO         equ  0  ;mono
MCI_STEREO       equ  1  ;stereo
MCI_MULTIMONO    equ  2  ;mono multichannel (channels can be 2, 3, 4, ...)
MCI_MULTISTEREO  equ  3  ;stereo multichannel (channels must be 4, 6, 8, ...)
MCI_MULTICHANNEL equ  4  ;multichannel (requires additional MINF-chunks) (future)
*/

// see details on asm-definitions above
enum MaudChannelInfo
{
	MCI_MONO = 0,			// mono
	MCI_STEREO = 1,			// stereo
	MCI_MULTIMONO = 2,		// mono multichannel
	MCI_MULTISTEREO = 3,	// stereo multichannel
	MCI_MULTICHANNEL = 4	// multichannel (for future)
};

/* mhdr_Compression
MCOMP_NONE       equ  0  ;no compression
MCOMP_FIBDELTA   equ  1  ;'Fibonacci Delta Compression' as used in 8SVX
MCOMP_ALAW       equ  2  ;16->8 bit, European PCM standard A-Law
MCOMP_ULAW       equ  3  ;16->8 bit, American PCM standard ƒÊ-Law
MCOMP_ADPCM2     equ  4  ;16->2 bit, ADPCM compression
MCOMP_ADPCM3     equ  5  ;16->3 bit, ADPCM compression
MCOMP_ADPCM4     equ  6  ;16->4 bit, ADPCM compression
MCOMP_ADPCM5     equ  7  ;16->5 bit, ADPCM compression
MCOMP_LONGDAT    equ  8  ;16->12 bit, used for DAT-longplay
*/

// see details on asm-definitions above
enum MaudCompression
{
	MCOMP_NONE       =  0,
	MCOMP_FIBDELTA   =  1,
	MCOMP_ALAW       =  2,
	MCOMP_ULAW       =  3,
	MCOMP_ADPCM2     =  4,
	MCOMP_ADPCM3     =  5,
	MCOMP_ADPCM4     =  6,
	MCOMP_ADPCM5     =  7,
	MCOMP_LONGDAT    =  8
};


#pragma pack(push, 1)

// names from documentation as-is
typedef struct 
{
	uint32_t mhdr_Samples;		// number of samples stored in MDAT
	uint16_t mhdr_SampleSizeC;	// number of bits per sample as stored in MDAT
	uint16_t mhdr_SampleSizeU;	// number of bits per sample after decompression
	uint32_t mhdr_RateSource;	// clock source frequency
	uint16_t mhdr_RateDevide;	// clock devide
	uint16_t mhdr_ChannelInfo;	// channel information (see below)
	uint16_t mhdr_Channels;		// number of channels (mono: 1, stereo: 2, ...)
	uint16_t mhdr_Compression;	// compression type (see below)
	uint32_t mhdr_Reserved1;
	uint32_t mhdr_Reserved2;
	uint32_t mhdr_Reserved3;
	
} MaudHeader;

#pragma pack(pop)


class CIffMaud : public AudioFile, public CIffContainer
{
private:
	CMemoryMappedFile m_File;
	//CIffHeader *m_pHead; // inherited now

protected:
	
	MaudHeader m_MaudHeader; // MHDR
	
	// process detected chunk
	virtual void OnChunk(CIffChunk *pChunk, CMemoryMappedFile &pFile);
	
	virtual bool IsSupportedType(CIffHeader *pHeader)
	{
        // standard MAUD chunk ID
		if (pHeader->m_iTypeID == MakeTag("MAUD"))
		{
			return true;
		}
		return false;
	}

	// sample body chunk
	CIffChunk *GetDataChunk() const
	{
		return GetChunkById(MakeTag("MDAT"));
	}
	
public:
	CIffMaud(void);
	virtual ~CIffMaud(void);

	virtual bool ParseFile(const std::wstring &szFileName);

	virtual bool isBigEndian()
	{
		return true;
	}
	virtual bool isInteger()
	{
		return true;
	}
	
	virtual long channelCount()
	{
		// note enumeration in mhdr_ChannelInfo
		//
		return m_MaudHeader.mhdr_Channels;
	}
	
	virtual unsigned long sampleRate()
	{
		// note: need conversion (see header comments):
		// value may be stored as clock oscillations of a hardware-device
		//
		return (m_MaudHeader.mhdr_RateSource / m_MaudHeader.mhdr_RateDevide);
	}
	
	virtual long sampleSize()
	{
		// this?
		return m_MaudHeader.mhdr_SampleSizeU;
	}
	
	virtual bool isSigned()
	{
		// only 8-bit samples are stored unsigned,
		// all other sizes are signed..
		if (m_MaudHeader.mhdr_SampleSizeU != 8)
		{
			// signed
			return true;
		}
		// unsigned
		return false;
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
		return CIffContainer::GetViewByOffset(pDataChunk->m_iOffset, m_File);
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
	
	virtual uint64_t decode(unsigned char *pBuffer, const uint64_t nBufSize /*, QAudioFormat *pOutput*/);
};

#endif // IFFMAUD_H
