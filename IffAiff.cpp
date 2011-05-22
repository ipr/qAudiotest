/////////////////////////////////////////////////////////
//
// CIffAiff : IFF-AIFF audio format parser
// (Audio Interchange File Format, Audio IFF)
//
// (c) Ilkka Prusi, 2011
//

#include "IffAiff.h"


//////////////////// protected methods

void CIffAiff::OnChunk(CIffChunk *pChunk, CMemoryMappedFile &pFile)
{
	uint8_t *pChunkData = CIffContainer::GetViewByOffset(pChunk->m_iOffset, pFile);
	
	if (pChunk->m_iChunkID == MakeTag("COMM"))
	{
		// Common chunk
		CommonChunk *pComm = (CommonChunk*)pChunkData;
	}
	else if (pChunk->m_iChunkID == MakeTag("INST"))
	{
	}
	else if (pChunk->m_iChunkID == MakeTag("MARK"))
	{
	}
	else if (pChunk->m_iChunkID == MakeTag("MIDI"))
	{
		//pChunkData = mididata;
	}
	else if (pChunk->m_iChunkID == MakeTag("AESD"))
	{
		AudioRecordingChunk *pAesd = (AudioRecordingChunk*)pChunkData;
	}
	else if (pChunk->m_iChunkID == MakeTag("APPL"))
	{
		// Application specific info
	}
	else if (pChunk->m_iChunkID == MakeTag("COMT"))
	{
		// Comments
	}
	else if (pChunk->m_iChunkID == MakeTag("NAME"))
	{
		// string-data (CHAR[])
		m_szName.assign((char*)pChunkData, pChunk->m_iChunkSize);
	}
	else if (pChunk->m_iChunkID == MakeTag("AUTH"))
	{
		// string-data (CHAR[])
		m_szAuthor.assign((char*)pChunkData, pChunk->m_iChunkSize);
	}
	else if (pChunk->m_iChunkID == MakeTag("ANNO"))
	{
		// string-data (CHAR[])
		m_szAnnotations.assign((char*)pChunkData, pChunk->m_iChunkSize);
	}
	else if (pChunk->m_iChunkID == MakeTag("(c) "))
	{
		// string-data (CHAR[])
		m_szCopyright.assign((char*)pChunkData, pChunk->m_iChunkSize);
	}
	else if (pChunk->m_iChunkID == MakeTag("SSND"))
	{
		// Sound data chunk
	}
}


//////////////////// public methods

CIffAiff::CIffAiff(void)
	: CIffContainer()
	, m_File()
{
}

CIffAiff::~CIffAiff(void)
{
	m_File.Destroy();
}

bool CIffAiff::ParseFile(LPCTSTR szPathName)
{
	if (m_File.Create(szPathName) == false)
	{
		return false;
	}

	if (ParseIffFile(m_File) == nullptr)
	{
		return false;
	}

	return true;
}

