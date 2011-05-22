/////////////////////////////////////////////////////////
//
// CIffAiff : IFF-AIFF audio format parser
// (Audio Interchange File Format, Audio IFF)
//
// (c) Ilkka Prusi, 2011
//

#include "IffAiff.h"


//////////////////// protected methods

void CIffAiff::Decode(CIffChunk *pChunk, CMemoryMappedFile &pFile)
{
	uint8_t *pChunkData = CIffContainer::GetViewByOffset(pChunk->m_iOffset, pFile);
	
	// Sound data chunk: one at most, zero if CommonChunk::numSampleFrames is zero
	//
	// has sample frame data
	SoundDataChunk *pSound = (SoundDataChunk*)pChunkData;
	
	int64_t i64SamplePointCount = m_Common.numSampleFrames * m_Common.numChannels;
	
	for (int64_t i = 0; i < i64SamplePointCount; i++)
	{
		SoundDataChunk Sound;
		Sound.offset = Swap4(pSound->offset);
		Sound.blockSize = Swap4(pSound->blockSize);
		uint8_t *pData = (uint8_t*)(pSound +1);
		
		// TODO: size of actual data?
		//uint8_t *pSampleData = new uint8_t[m_Common.sampleSize];
		//::memcpy(pSampleData, pData, m_Common.sampleSize);
		//pSound = pChunkData + sizeof(SoundDataChunk) + m_Common.sampleSize;
	}
	
}

void CIffAiff::OnChunk(CIffChunk *pChunk, CMemoryMappedFile &pFile)
{
	uint8_t *pChunkData = CIffContainer::GetViewByOffset(pChunk->m_iOffset, pFile);
	
	if (pChunk->m_iChunkID == MakeTag("COMM"))
	{
		// Common chunk
		CommonChunk *pComm = (CommonChunk*)pChunkData;
		m_Common.numChannels = Swap2(pComm->numChannels);
		m_Common.numSampleFrames = Swap4(pComm->numSampleFrames);
		m_Common.sampleSize = Swap2(pComm->sampleSize);
		
		// TODO: convert 'sampleRate' from 'extended' (80-bit long double) to 64-bit double
		// since Visual C++ does not support it..
	}
	else if (pChunk->m_iChunkID == MakeTag("SSND"))
	{
		// decode when actual playback
		//Decode(pChunk, pFile);
	}
	else if (pChunk->m_iChunkID == MakeTag("INST"))
	{
		if (pChunk->m_iChunkSize != 20)
		{
			// ASIF, Apple IIGS Sampled Instrument Format
			// also has "INST" chunk but different size
			// -> not supported (yet)
			return;
		}
			
		InstrumentChunk *pInst = (InstrumentChunk*)pChunkData;
		m_Instrument.baseNote = pInst->baseNote;
		m_Instrument.detune = pInst->detune;
		m_Instrument.lowNote = pInst->lowNote;
		m_Instrument.highNote = pInst->highNote;
		m_Instrument.lowVelocity = pInst->lowVelocity;
		m_Instrument.highVelocity = pInst->highVelocity;
		m_Instrument.gain = pInst->gain;
		
		m_Instrument.sustainLoop.playMode = Swap2(pInst->sustainLoop.playMode);
		m_Instrument.sustainLoop.beginLoop = Swap2(pInst->sustainLoop.beginLoop);
		m_Instrument.sustainLoop.endLoop = Swap2(pInst->sustainLoop.endLoop);
		m_Instrument.releaseLoop.playMode = Swap2(pInst->releaseLoop.playMode);
		m_Instrument.releaseLoop.beginLoop = Swap2(pInst->releaseLoop.beginLoop);
		m_Instrument.releaseLoop.endLoop = Swap2(pInst->releaseLoop.endLoop);
	}
	else if (pChunk->m_iChunkID == MakeTag("MIDI"))
	{
		// just array of data
		//mididata = pChunkData;
	}
	else if (pChunk->m_iChunkID == MakeTag("AESD"))
	{
		// fixed-size array of data for user convenience (24 bytes)
		//AudioRecordingChunk *pAesd = (AudioRecordingChunk*)pChunkData;
		::memcpy((void*)&m_AesdChunk, (void*)pChunkData, sizeof(AudioRecordingChunk));
	}
	else if (pChunk->m_iChunkID == MakeTag("APPL"))
	{
		// Application specific info
		OSType *pType = (OSType*)pChunkData;
		m_OSType = Swap4((*pType));
		
		if ((pChunk->m_iChunkSize - sizeof(OSType)) > 0)
		{
			// additional application specific data..
		}
	}
	else if (pChunk->m_iChunkID == MakeTag("MARK"))
	{
		unsigned short numMarkers = Swap2((*((UWORD*)pChunkData)));
		pChunkData = CIffContainer::GetViewByOffset(pChunk->m_iOffset + sizeof(UWORD), pFile);

		// each must be aligned to start at even byte boundary
		// -> may have padding bytes between 
		for (int i = 0; i < numMarkers; i++)
		{
			Marker m;
			m.id = Swap2((*((UWORD*)pChunkData)));
			m.position = Swap4((*((long*)pChunkData +2)));
			m.string.ReadBuffer(pChunkData +6);
			
			int iTotalSize = m.string.m_stringlen +1;
			if (iTotalSize % 2 != 0)
			{
				iTotalSize += 1; // padding byte of string
			}
			iTotalSize += 6; // preceding fields

			// next marker
			pChunkData = (pChunkData + iTotalSize);
			m_Markers.push_back(m);
		}
	}
	else if (pChunk->m_iChunkID == MakeTag("COMT"))
	{
		// Comments
		unsigned short numComments = Swap2((*((UWORD*)pChunkData)));
		pChunkData = CIffContainer::GetViewByOffset(pChunk->m_iOffset + sizeof(UWORD), pFile);
		
		// array of structs, no padding between (always even-length)
		for (int i = 0; i < numComments; i++)
		{
			CommentFields *pComm = (CommentFields*)pChunkData;
			Comment c;
			c.timeStamp = Swap4(pComm->timeStamp);
			c.marker = Swap2(pComm->marker);
			c.string.ReadBuffer(Swap2(pComm->count), pChunkData+sizeof(CommentFields));
			
			pChunkData = (pChunkData + (sizeof(CommentFields) + c.string.m_stringlen));
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
}


//////////////////// public methods

CIffAiff::CIffAiff(void)
	: AudioFile()
    , CIffContainer()
	, m_File()
{
}

CIffAiff::~CIffAiff(void)
{
	m_File.Destroy();
}

bool CIffAiff::ParseFile(const std::wstring &szFileName)
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

