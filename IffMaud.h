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


class CIffMaud : public AudioFile, public CIffContainer
{
private:
	CMemoryMappedFile m_File;
	//CIffHeader *m_pHead; // inherited now

protected:
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
	
	virtual long channelCount()
	{
		// TODO: determine
		//return m_VoiceHeader. ?numChannels;
		//return 8;
		return 0;
	}
	
	virtual unsigned long sampleRate()
	{
		//return m_VoiceHeader.samplesPerSec;
		return 0;
	}
	
	virtual long sampleSize()
	{
		// 8-bit only
		//return 8;
		return 0;
	}
	
	virtual bool isSigned()
	{
		// always signed
		//return true;
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
		
		// TODO: may need decode..
		// pull/push mode?
		//Decode(pDataChunk, m_File);
		
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
};

#endif // IFFMAUD_H
