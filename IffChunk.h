/////////////////////////////////////////////////////////
//
// IffChunk.h : IFF-chunk node type
// for generic file decoding/processing.
//
// Used in CIffContainer and inherited classes.
//
// (c) Ilkka Prusi, 2011
//

#ifndef IFFCHUNK_H
#define IFFCHUNK_H

#include <stdint.h>

// fwd. decl.
//
class CIffHeader;
class CIffChunk; 
class CIffSubChunk; 


// not part of general IFF-standard,
// parser must detect:
// used by some fileformats
//
class CIffSubChunk
{
public:
	CIffSubChunk()
		: m_pParent(nullptr)
		, m_pNextSub(nullptr)
		, m_iChunkID(0)
		, m_iSize(0)
		, m_iOffset(0)
	{
	}
	~CIffSubChunk()
	{
		// do nothing,
		// parent must destroy all subchunks
	}

	CIffChunk *m_pParent;
	//CIffSubChunk *m_pChild;
	CIffSubChunk *m_pNextSub;
	//CIffSubChunk *m_pPrevSub;

	// actually CHAR[4]
	uint32_t m_iChunkID;

	// size of sub-chunk (check, 16-bit or 32-bit)
	uint16_t m_iSize;
	//uint32_t m_iSize;

	// offset to start of data in actual file
	// (from start of file): we maintain this
	int64_t m_iOffset;
};

class CIffChunk
{
public:
	CIffChunk() 
		: m_pPrevious(nullptr)
		, m_pNext(nullptr)
		, m_pSubChunk(nullptr)
		, m_iChunkID(0)
		, m_iChunkSize(0)
		, m_iOffset(0)
	{
	}
	~CIffChunk()
	{
		// if any chunks within this chunk..
		DestroySubChunks();
	}

	void DestroySubChunks()
	{
		// use simple loop instead of recursion,
		// otherwise easily we run out of stack-limits
		CIffSubChunk *pCurrent = m_pSubChunk;
		while (pCurrent != nullptr)
		{
			CIffSubChunk *pNext = pCurrent->m_pNextSub;
			delete pCurrent;
			pCurrent = pNext;
		}
		m_pSubChunk = nullptr;
	}

	CIffChunk *m_pPrevious;
	CIffChunk *m_pNext;
	CIffSubChunk *m_pSubChunk;

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
	    , m_pComposite(nullptr)
	    //, m_pParent(nullptr)
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
		DestroyComposites();
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
	
	// only top-most should do this 
	// so that recursion won't reach stack-limit?
	void DestroyComposites()
	{
		// temp, use recursion..
		CIffHeader *pComposite = m_pComposite;
		m_pComposite = nullptr; // avoid secondary-destruction by mistake
		if (pComposite != nullptr)
		{
			pComposite->DestroyComposites();
		}
	}

	void AddComposite(CIffHeader *pSubForm)
	{
		if (m_pComposite == nullptr)
		{
			m_pComposite = pSubForm;
		}
		else
		{
			// recusive call to locate last
			m_pComposite->AddComposite(pSubForm);
		}
	}

	// start from first chunk in file
	CIffChunk *m_pFirst;
	
	// in case of composite-files
	// (e.g. ANIM with audio&images)
	// TODO: should have list of these?
	CIffHeader *m_pComposite;
	// parent of composite-FORM
	//CIffHeader *m_pParent;
	
	// offset to start of data in actual file
	// (from start of file): we maintain this
	int64_t m_iOffset;
	
	// type of payload in this FORM:
	// ILBM/8SVX or other
	uint32_t m_iTypeID;

	// actually CHAR[4],
	// tag-ID is type of file (usually "FORM")
	uint32_t m_iFileID;

	// data size given in file header
	uint32_t m_iDataSize;

	// size of actual file
	int64_t m_iFileSize;
};



#endif // IFFCHUNK_H
