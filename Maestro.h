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


/*
Sample type is:
  0x0000 = Stereo, 16 bit
  0x0001 = Stereo, 8 bit
  0x0002 = Mono, 16 bit
  0x0003 = Mono, 8 bit

New:-

  0x0004 = Stereo, 24 bit
  0x0005 = Mono, 24 bit
  0x0006 = Stereo, 32 bit
  0x0007 = Mono, 32 bit
  0x0008 = Stereo, 32 bit, IEEE float
  0x0009 = Mone, 32 bit, IEEE float
  0x000A = Stereo, 64 bit, IEEE float
  0x000B = Mono, 64 bit, IEEE float
  0x000C = Stereo, 32 bit, FFP float
  0x000D = Mono, 32 bit, FFP float

  only sampling rates of 32000 Hz, 44100 Hz or 48000 Hz should be used
*/

/*
 FFP: "fast floating-point":
 24-bits mantissa, 8 bits exponent
 
*/
enum MaestroSampleType
{
	MSMT_STEREO_16	= 0x0000,
	MSMT_STEREO_8	= 0x0001,
	MSMT_MONO_16	= 0x0002,
	MSMT_MONO_8		= 0x0003,
	MSMT_STEREO_24	= 0x0004,
	MSMT_MONO_24	= 0x0005,
	MSMT_STEREO_32	= 0x0006,
	MSMT_MONO_32	= 0x0007,
	MSMT_STEREO_32F	= 0x0008,	// IEEE floating point
	MSMT_MONO_32F	= 0x0009,		// IEEE floating point
	MSMT_STEREO_64F	= 0x000A,	// IEEE floating point
	MSMT_MONO_64F	= 0x000B,		// IEEE floating point
	MSMT_STEREO_32FFP = 0x000C,	// FFP float
	MSMT_MONO_32FFP	= 0x000D,	// FFP float
	MSMT_UNKNOWN	= 0xFFFF	// placeholder for error detection..
};

enum MaestroValueType
{
	//MSVT_SIGNED_INT,
	//MSVT_UNSIGNED_INT,
	MSVT_INTEGER,
	MSVT_IEEE_FLOAT,
	MSVT_FFP_FLOAT
};

#pragma pack(push, 1)

// header same way as stored in file for simplicity
typedef struct 
{
	/*
	//char fileid[8]; // "MAESTRO"
	*/
	
	uint8_t version;
	uint8_t revision;
	uint8_t file_version; // always 0x02
	uint8_t padding1; // always 0x00
	
	uint16_t sampletype; // see list of modes
	uint16_t padding2;
	uint32_t samplecount; // number of samples
	uint32_t samplerate; // in Hz
	
} MaestroHeader_t;

#pragma pack(pop)

// helper for sample-type related information
struct MaestroSampleInfo_t
{
	size_t m_channelCount; // mono/stereo
	size_t m_sampleWidth; // bit-width

	// TODO: always signed?
	// always unsigned?
	bool m_isSigned;
	
	MaestroValueType m_enValueType;
	
	// constructor
	MaestroSampleInfo_t()
	{
		m_channelCount = 0;
		m_sampleWidth = 0;
		m_isSigned = false;
		m_enValueType = MSMT_UNKNOWN;
	}
	
	void setTypeInfo(uint16_t sampleType)
	{
		switch (sampleType)
		{
		case MSMT_STEREO_16:
			m_channelCount = 2;
			m_sampleWidth = 16;
			m_enValueType = MSVT_INTEGER;
			break;
		case MSMT_STEREO_8:
			m_channelCount = 2;
			m_sampleWidth = 8;
			m_enValueType = MSVT_INTEGER;
			break;
		case MSMT_MONO_16:
			m_channelCount = 1;
			m_sampleWidth = 16;
			m_enValueType = MSVT_INTEGER;
			break;
		case MSMT_MONO_8:
			m_channelCount = 1;
			m_sampleWidth = 8;
			m_enValueType = MSVT_INTEGER;
			break;
		case MSMT_STEREO_24:
			m_channelCount = 2;
			m_sampleWidth = 24;
			m_enValueType = MSVT_INTEGER;
			break;
		case MSMT_MONO_24:
			m_channelCount = 1;
			m_sampleWidth = 24;
			m_enValueType = MSVT_INTEGER;
			break;
		case MSMT_STEREO_32:
			m_channelCount = 2;
			m_sampleWidth = 32;
			m_enValueType = MSVT_INTEGER;
			break;
		case MSMT_MONO_32:
			m_channelCount = 1;
			m_sampleWidth = 32;
			m_enValueType = MSVT_INTEGER;
			break;
		case MSMT_STEREO_32F:
			m_channelCount = 2;
			m_sampleWidth = 32;
			m_enValueType = MSVT_IEEE_FLOAT;
			break;
		case MSMT_MONO_32F:
			m_channelCount = 1;
			m_sampleWidth = 32;
			m_enValueType = MSVT_IEEE_FLOAT;
			break;
		case MSMT_STEREO_64F:
			m_channelCount = 2;
			m_sampleWidth = 64;
			m_enValueType = MSVT_IEEE_FLOAT;
			break;
		case MSMT_MONO_64F:
			m_channelCount = 1;
			m_sampleWidth = 64;
			m_enValueType = MSVT_IEEE_FLOAT;
			break;
		case MSMT_STEREO_32FFP:
			m_channelCount = 2;
			m_sampleWidth = 32;
			m_enValueType = MSVT_FFP_FLOAT;
			break;
		case MSMT_MONO_32FFP:
			m_channelCount = 1;
			m_sampleWidth = 32;
			m_enValueType = MSVT_FFP_FLOAT;
			break;
		}
	}
	
};

class CMaestro : public AudioFile
{
private:
	CMemoryMappedFile m_File;
	
protected:
	// actual file header included information
	MaestroHeader_t m_MaestroHeader;
	
	// additional info for simplicity
	MaestroSampleInfo_t m_SampleInfo;

	uint16_t Swap2(const uint16_t val) const
    {
        return (((val >> 8)) | (val << 8));
    }
	uint32_t Swap4(const uint32_t val) const
    {
        return (
                ((val & 0x000000FF) << 24) + ((val & 0x0000FF00) <<8) |
                ((val & 0x00FF0000) >> 8) + ((val & 0xFF000000) >>24)
                );
    }
	float SwapF(const float fval) const
    {
        float fTemp = fval;
        uint32_t tmp = Swap4((*((uint32_t*)(&fTemp))));
        fTemp = (*((float*)(&tmp)));
        return fTemp;
    }

	
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
		return m_SampleInfo.m_channelCount;
	}
	
	virtual unsigned long sampleRate()
	{
		return m_MaestroHeader.samplerate;
	}
	
	virtual long sampleSize()
	{
		return m_SampleInfo.m_sampleWidth;
	}
	
	virtual bool isSigned()
	{
		return m_SampleInfo.m_isSigned;
	}
	
	// actual sample data
	virtual unsigned char *sampleData()
	{
		uint8_t *pView = (uint8_t*)m_File.GetView();
		return (pView + 0x18); // start of raw sample data
	}
	
	// total size of sample data
	virtual uint64_t sampleDataSize()
	{
		return (m_MaestroHeader.samplecount * (m_SampleInfo.m_sampleWidth/8));
	}
	
	virtual uint64_t decode(unsigned char *pBuffer, const uint64_t nBufSize /*, QAudioFormat *pOutput*/);
};

#endif // MAESTRO_H
