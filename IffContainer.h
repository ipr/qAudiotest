/////////////////////////////////////////////////////////
//
// CIffContainer : generic IFF-format parser,
// detailed parsing must be per file-type (format)
// since IFF is more generic.
//
// (c) Ilkka Prusi, 2011
//

#ifndef _IFFCONTAINER_H_
#define _IFFCONTAINER_H_

#include <stdint.h>

// chunk-node types
#include "IffChunk.h"

// I can't be arsed to else,
// simple is best
#include "MemoryMappedFile.h"


// handling of the general IFF-FORM chunks in file
//
class CIffContainer
{
private:
	// keep header and related data
	CIffHeader *m_pHeader;

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

	// simple example: IFF-ILBM picture with chunk CMAP
	// -> create handler for that type of chunk.
	// (chunk handling can depend of what format data is stored in general IFF-file)
	//
	/*
	virtual CIffChunk *CreateChunkNode(CIffHeader *pHead, uint32_t iChunkID)
	{
		if (pHead->m_iFileID == ID_ILBM
			&& iChunkID == ID_CMAP)
		{
			return new CIlbmCmap();
		}

		// should return default-type if non-supported by inherited implementation?
		return new CIffChunk();
		// ..although it can be skipped if implementation doesn't support such chunk..
		//return nullptr;
	}
    */

	// similar to above but for (optional) sub-chunks
	/*
	virtual CIffSubChunk *CreateSubChunkNode(CIffHeader *pHead, CIffChunk *pChunk, uint32_t iSubChunkID)
	{
		if (pHead->m_iFileID == ID_XXXX
			&& pChunk->m_iChunkID == ID_YYYY
			&& iSubChunkID == ID_ZZZZ)
		{
			return new CXYZ();
		}

		// should return null when there is no sub-chunk
		return nullptr;
	}
    */

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
	CIffContainer(void);
	virtual ~CIffContainer(void);

	CIffHeader *ParseIffFile(CMemoryMappedFile &pFile);

	// TODO:?
	//CIffHeader *ParseIffBuffer(CMemoryMappedFile &pFile);
};

#endif // ifndef _IFFCONTAINER_H_

