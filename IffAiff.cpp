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
		unsigned short numMarkers = Swap2((*((UWORD*)pChunkData)));
		pChunkData = CIffContainer::GetViewByOffset(pChunk->m_iOffset + sizeof(UWORD), pFile);
		
		for (int i = 0; i < numMarkers; i++)
		{
			Marker m;
			m.id = Swap2((*((UWORD*)pChunkData)));
			m.position = Swap4((*((long*)pChunkData +2));
			m.string.ReadBuffer(pChunkData +6);
			
			pChunkData = (pChunkData + (6 + m.string.m_stringlen +1));
			m_Markers.push_back(m);
		}
	}
	else if (pChunk->m_iChunkID == MakeTag("MIDI"))
	{
		// just array of data
		//mididata = pChunkData;
	}
	else if (pChunk->m_iChunkID == MakeTag("AESD"))
	{
		// fixed-size array of data
		AudioRecordingChunk *pAesd = (AudioRecordingChunk*)pChunkData;
	}
	else if (pChunk->m_iChunkID == MakeTag("APPL"))
	{
		// Application specific info
	}
	else if (pChunk->m_iChunkID == MakeTag("COMT"))
	{
		// Comments
		unsigned short numComments = Swap2((*((UWORD*)pChunkData)));
		pChunkData = CIffContainer::GetViewByOffset(pChunk->m_iOffset + sizeof(UWORD), pFile);
		
		// array of structs
		for (int i = 0; i < numComments; i++)
		{
			CommentFields *pComm = (CommentFields*)pChunkData;
			Comment c;
			c.timeStamp = Swap4(pComm->timeStamp);
			c.marker = Swap2(pComm->marker);
			c.string.ReadBuffer(Swap2(pComm->count), pChunkData+sizeof(CommentFields));
			
			pChunkData = (pChunkData + (sizeof(CommentFields) + m.string.m_stringlen));
			m_Comments.push_back(c);
		}
		
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

