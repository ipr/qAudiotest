/////////////////////////////////////////////////////////
//
// CRiffContainer : generic RIFF-format parser,
// slightly modified from IFF-format parsing CIffContainer
// where differences.
// since IFF is more generic.
//
// (c) Ilkka Prusi, 2011
//


#ifndef _RIFFCONTAINER_H_
#define _RIFFCONTAINER_H_

#include <stdint.h>

// chunk-node types
#include "IffChunk.h"

// I can't be arsed to else,
// simple is best
#include "MemoryMappedFile.h"


// handling of the general RIFF chunks in file,
// some differences to normal IFF-chunks (stupid MS just has to do differently..).
//
class CRiffContainer
{
private:
	// keep header and related data
	CIffHeader *m_pHeader;
	
	// set true when byteswapping needed:
	// RIFF file with big-endian CPU
	// or RIFX file with little-endian CPU (x86)
	bool m_bUseByteswap;

protected:

	// (note, should make inline but also needed in inherited classes..)

	// byteswap methods
	uint16_t Swap2(const uint16_t val) const;
	uint32_t Swap4(const uint32_t val) const;
	//uint64_t Swap8(const uint64_t val) const;
	float SwapF(const float fval) const;
	//double SwapD(const double dval) const;

	// make tag from string
	uint32_t MakeTag(const char *buf) const;

	uint32_t GetValueAtOffset(const int64_t iOffset, CMemoryMappedFile &pFile) const;
	uint8_t *GetViewByOffset(const int64_t iOffset, CMemoryMappedFile &pFile) const;

	CIffHeader *ReadHeader(int64_t &iOffset, CMemoryMappedFile &pFile) const;
	CIffChunk *ReadNextChunk(int64_t &iOffset, CMemoryMappedFile &pFile) const;
	
	void ReadChunks(int64_t &iOffset, CIffHeader *pHeader, CMemoryMappedFile &pFile);

protected:

	// methods which user might want to overload on inheritance

	// called on each found chunk when found:
	// user can process chunk-data immediately for single-pass handling.
	//
	// default implementation does nothing (empty)
	//
	virtual void OnChunk(CIffChunk *pChunk, CMemoryMappedFile &pFile) 
	{}
	
	// called on found file-header:
	// user can re-implement to allow certain types only
	//
	virtual bool IsSupportedType(CIffHeader *pHeader)
	{
		// generic: all supported
		return true;
	}

	// for inherited classes, get access to private member
	CIffHeader *GetHeader() const
	{
		return m_pHeader;
	}
	
	// use MakeTag("...") and call this in inherited class
	CIffChunk *GetChunkById(const uint32_t uiFourccID) const
	{
		CIffHeader *pHead = GetHeader();
		if (pHead != nullptr)
		{
			return GetNextChunkById(pHead->m_pFirst, uiFourccID);
		}
		// not opened/processed file?
		return nullptr;
	}
	
	// in case file has multiple chunks with same identifier, locate next
	CIffChunk *GetNextChunkById(CIffChunk *pPrevChunk, const uint32_t uiFourccID) const
	{
		CIffChunk *pChunk = pPrevChunk;
		while (pChunk != nullptr)
		{
			if (pChunk->m_iChunkID == uiFourccID)
			{
				// found -> return to caller
				return pChunk;
			}
			pChunk = pChunk->m_pNext;
		}
		// not found, incomplete file?
		return nullptr;
	}
	
	
public:
	CRiffContainer(void);
	virtual ~CRiffContainer(void);

	CIffHeader *ParseIffFile(CMemoryMappedFile &pFile);

	// TODO:?
	//CIffHeader *ParseIffBuffer(CMemoryMappedFile &pFile);
};

#endif // ifndef _RIFFCONTAINER_H_

