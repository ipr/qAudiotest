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

// I can't be arsed to else,
// simple is best
#include "MemoryMappedFile.h"


// fwd. decl.
//
//class CIffHeader;
class CIffChunk; 


class CIffChunk
{
public:
	CIffChunk() 
		: m_pPrevious(nullptr)
		, m_pNext(nullptr)
		, m_iChunkID(0)
		, m_iChunkSize(0)
		, m_iOffset(0)
	{
	}
	~CIffChunk()
	{
	}

	CIffChunk *m_pPrevious;
	CIffChunk *m_pNext;

	// actually CHAR[4]
	uint32_t m_iChunkID;

	// size of data in chunk
	uint32_t m_iChunkSize;

	// offset to start of data in actual file
	// (from start of file): we maintain this
	int64_t m_iOffset;
};

// file header node:
// special only to locate origin
//
class CIffHeader
{
public:
	CIffHeader() 
		: m_pFirst(nullptr)
	    , m_iOffset(0)
	    , m_iTypeID(0)
		, m_iFileID(0)
		, m_iDataSize(0)
		, m_iFileSize(0)
	{
	}
	~CIffHeader()
	{
		DestroyChunks();
	}

	void DestroyChunks()
	{
		// use simple loop instead of recursion,
		// otherwise easily we run out of stack-limits
		CIffChunk *pCurrent = m_pFirst;
		while (pCurrent != nullptr)
		{
			// keep next before destroying current
			CIffChunk *pNext = pCurrent->m_pNext;
			delete pCurrent;
			pCurrent = pNext;
		}
		m_pFirst = nullptr;
	}
	

	// start from first chunk in file
	CIffChunk *m_pFirst;
	
	// offset to start of data in actual file
	// (from start of file): we maintain this
	int64_t m_iOffset;
	
	// type of payload in this container:
	// (WAVE or other)
	uint32_t m_iTypeID;

	// actually CHAR[4],
	// tag-ID is type of file (usually "RIFF")
	uint32_t m_iFileID;

	// data size given in file header
	uint32_t m_iDataSize;

	// size of actual file
	int64_t m_iFileSize;
};


// handling of the general RIFF chunks in file
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

public:
	CRiffContainer(void);
	virtual ~CRiffContainer(void);

	CIffHeader *ParseIffFile(CMemoryMappedFile &pFile);

	// TODO:?
	//CIffHeader *ParseIffBuffer(CMemoryMappedFile &pFile);
};

#endif // ifndef _RIFFCONTAINER_H_

