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

#include "IffMaud.h"

//////////////////// protected methods

void CIffMaud::OnChunk(CIffChunk *pChunk, CMemoryMappedFile &pFile)
{
	uint8_t *pChunkData = CIffContainer::GetViewByOffset(pChunk->m_iOffset, pFile);
	
	// TODO: check chunk ID
	if (pChunk->m_iChunkID == MakeTag("MHDR"))
	{
	}
	else if (pChunk->m_iChunkID == MakeTag("MDAT"))
	{
		// pure audio data (body)
		
	}
	else if (pChunk->m_iChunkID == MakeTag("MINF"))
	{
		// (optional) channel info chunk (for future)
	}
	else
	{
		// handle common IFF-standard chunks in base
		CIffContainer::OnChunk(pChunk, pFile);
	}
}


//////////////////// public methods

CIffMaud::CIffMaud(void)
	: AudioFile()
    , CIffContainer()
	, m_File()
{
}

CIffMaud::~CIffMaud(void)
{
	m_File.Destroy();
}

bool CIffMaud::ParseFile(const std::wstring &szFileName)
{
	if (m_File.Create(szFileName.c_str()) == false)
	{
		return false;
	}

	if (ParseIffFile(m_File) == nullptr)
	{
		return false;
	}

	return true;
}

// notes on channel format:
// channel order: left, right, surround..
// (see header)
//
// same way as with AIFF?
//
// note: compression is truly bit-compression
// so two 3-bit samples are really packed to a single byte..
//
uint64_t CIffMaud::decode(unsigned char *pBuffer, const uint64_t nBufSize /*, QAudioFormat *pOutput*/)
{
	// TODO:
	return 0;
}

