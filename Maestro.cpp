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

#include "Maestro.h"


//////////////////// public methods

CMaestro::CMaestro(void)
	: AudioFile()
	, m_File()
{
}

CMaestro::~CMaestro(void)
{
	m_File.Destroy();
}

bool CMaestro::ParseFile(const std::wstring &szFileName)
{
	if (m_File.Create(szFileName.c_str()) == false)
	{
		return false;
	}
	
	int64_t iSize = m_File.GetSize();
	uint8_t *pView = (uint8_t*)m_File.GetView();
	if (::memcmp(pView, "MAESTRO", 7) != 0)
	{
		return false;
	}
	
	uint8_t version = pView[8];
	uint8_t revision = pView[9];
	uint8_t file_version = pView[10]; // always 0x02
	uint8_t padding = pView[11]; // always 0x00

	// TODO: byteswap rest..	
	
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
	
	uint16_t sampletype = (*((uint16_t*)(pView + 0x0C)));
	uint16_t padding = (*((uint16_t*)(pView + 0x0E)));
	uint32_t samplecount = (*((uint32_t*)(pView + 0x10))); // amount of samples
	uint32_t samplerate = (*((uint32_t*)(pView + 0x14))); // in Hz

	// rest is sample data..
	
	return true;
}

// fileformat info (channel order):
// stereo channels: left, right..
//
//
uint64_t CMaestro::decode(unsigned char *pBuffer, const uint64_t nBufSize /*, QAudioFormat *pOutput*/)
{
	int64_t iSize = m_File.GetSize();
	uint8_t *pView = (uint8_t*)m_File.GetView();
	pView = (pView + 0x18); // start of sample data
	
	
	return 0;
}
