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
	
	MaestroHeader_t *pHdr = (MaestroHeader_t*)(pView+0x08);
	m_MaestroHeader.version = pHdr->version;
	m_MaestroHeader.revision = pHdr->revision;
	m_MaestroHeader.file_version = pHdr->file_version;
	m_MaestroHeader.padding1 = pHdr->padding1;
	m_MaestroHeader.sampletype = Swap2(pHdr->sampletype);
	m_MaestroHeader.padding2 = Swap2(pHdr->padding2);
	m_MaestroHeader.samplecount = Swap4(pHdr->samplecount);
	m_MaestroHeader.samplerate = Swap4(pHdr->samplerate);
	
	// if not supported -> end
	if (m_MaestroHeader.file_version != 0x02)
	{
		return false;
	}
	
	// simplify type-info for later..
	m_SampleInfo.setTypeInfo(m_MaestroHeader.sampletype);
	
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
