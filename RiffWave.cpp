/////////////////////////////////////////////////////////
//
// CRiffWave : RIFF-WAVE audio format parser
//
// (c) Ilkka Prusi, 2011
//

#include "RiffWave.h"


//////////////////// protected methods

void CRiffWave::Decode(CIffChunk *pChunk, CMemoryMappedFile &pFile)
{
	uint8_t *pChunkData = CRiffContainer::GetViewByOffset(pChunk->m_iOffset, pFile);
	
	// decode when actual playback
}


void CRiffWave::OnChunk(CIffChunk *pChunk, CMemoryMappedFile &pFile)
{
	uint8_t *pChunkData = CRiffContainer::GetViewByOffset(pChunk->m_iOffset, pFile);
	
	if (pChunk->m_iChunkID == MakeTag("fmt "))
	{
		// format description chunk
		WAVEFormat *pHeader = (WAVEFormat*)pChunkData;
		m_WaveHeader.wFormatTag = Swap2(pHeader->wFormatTag);
		m_WaveHeader.wChannels = Swap2(pHeader->wChannels);
		m_WaveHeader.dwSamplesPerSec = Swap4(pHeader->dwSamplesPerSec);
		m_WaveHeader.dwAvgBytesPerSec = Swap4(pHeader->dwAvgBytesPerSec);
		m_WaveHeader.wBlockAlign = Swap2(pHeader->wBlockAlign);
		
		pChunkData = CRiffContainer::GetViewByOffset(pChunk->m_iOffset + sizeof(WAVEFormat), pFile);
		if (m_WaveHeader.wFormatTag == fmt_WAVE_FORMAT_PCM)
		{
			// Sample size
			m_wBitsPerSample = Swap2((*((WORD*)pChunkData)));
		}
	}
	else if (pChunk->m_iChunkID == MakeTag("INFO"))
	{
	}
	else if (pChunk->m_iChunkID == MakeTag("LIST"))
	{
	}
	else if (pChunk->m_iChunkID == MakeTag("wavl"))
	{
	}
	else if (pChunk->m_iChunkID == MakeTag("slnt"))
	{
		// "silence" data?
		DWORD dwSampleCount = Swap4((*((DWORD*)pChunkData)));
	}
	else if (pChunk->m_iChunkID == MakeTag("fact"))
	{
	}
	else if (pChunk->m_iChunkID == MakeTag("data"))
	{
		// decode when actual playback
		//Decode(pChunk, pFile);
	}
	
	// TODO: determine other types of chunks..
	// crappy specs..
	// can have following:
	// - <fact-ck>
	// - <cue-ck>
	// - <playlist-ck>
	// - <assoc-data-list>
	
}


//////////////////// public methods

CRiffWave::CRiffWave(void)
	: AudioFile()
    , CRiffContainer()
	, m_File()
{
}

CRiffWave::~CRiffWave(void)
{
	// cleanup, if not done already
	m_File.Destroy();
}

bool CRiffWave::ParseFile(const std::wstring &szFileName)
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

