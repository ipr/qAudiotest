///////////////////////////////
//
// 16-bit IFF-MAUD format
// reading for playback
//
// (c) Ilkka Prusi, 2011
//

#include "IffMaud.h"


//////////////////// protected methods

void CIffMaud::OnChunk(CIffChunk *pChunk, CMemoryMappedFile &pFile)
{
	uint8_t *pChunkData = CIffContainer::GetViewByOffset(pChunk->m_iOffset, pFile);
	
	// TODO: check chunk ID
	if (pChunk->m_iChunkID == MakeTag("mhdr"))
	{
	}
	else if (pChunk->m_iChunkID == MakeTag("MDAT"))
	{
		// pure audio data (body)
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

