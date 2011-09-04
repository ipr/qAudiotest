/////////////////////////////////////////////////////////
//
// CIffAiff : IFF-AIFF audio format parser
// (Audio Interchange File Format, Audio IFF)
// from Apple, used by SGI.
//
// Should be (mostly) usable with AIFF-C also,
// see specification: DAVIC 1.4.1 Part 9 Annex B
// (http://www.davic.org)
//
// Audio IFF conforms to the EA IFF 85 standard
// and supports sample point sizes from 1 to 32 bits.
//
// (c) Ilkka Prusi, 2011
//


#ifndef IFFAIFF_H
#define IFFAIFF_H

#include <stdint.h>

#include "MemoryMappedFile.h"
#include "IffContainer.h"

// interface to define in audio-file
#include "AudioFile.h"


// std::string, for keeping sample&copyright descriptions
#include <string>
#include <vector>

/* playMode specifies which type of looping is to be performed (INST) */
#define NoLooping               0
#define ForwardLooping          1
#define ForwardBackwardLooping  2


// from format specifications,
// note one-byte alignment in struct

/*
  note: empty array on struct is error on some compiler (Visual C++ for one)
  so need to change from specification-definition to something else..
*/

#pragma pack(push, 1)

//// COMM
typedef struct 
{
    short           numChannels;
    unsigned long   numSampleFrames;
    short           sampleSize;
	
	/* note: "extended" or "long double" is not supported on Visual C++
	  and it truncates definition to common "double" (80-bit -> 64-bit). */
	/* extended: 80 bit IEEE Standard 754 floating point number 
     (Standard Apple Numeric Environment [SANE] data type Extended). */
	/* see _controlfp_s for Visual C++ handling.. */
    //extended        sampleRate; // note: 80-bit -> not available on x86.. (no compiler support)
	unsigned char    sampleRate[10]; // check handling! need some way to convert correctly..
} CommonChunk; 

//// Extended Common Chunk (AIFF-C)
// additional compression values
typedef struct 
{
    unsigned long    compressionType;
    unsigned char    compNameLength;
    //unsigned char       compressionName[]; // -> undefined length
} ExtendedCommonChunk; 


//// SSND
typedef struct 
{
    unsigned long       offset;
    unsigned long       blockSize;
    //unsigned char       soundData[]; // -> undefined length
} SoundDataChunk;  

//// MARK

typedef short   MarkerId;

// -> see class PascalString and Marker

//// INST

// see playmode definitions for 'playMode'
typedef struct {
    short           playMode;
    MarkerId        beginLoop;
    MarkerId        endLoop;
} Loop;

// units are MIDI note numbers (0..127, Middle C is 60)
typedef struct {
    char            baseNote;
    char            detune;
    char            lowNote;
    char            highNote;
    char            lowVelocity;
    char            highVelocity;
    short           gain;
    Loop            sustainLoop;
    Loop            releaseLoop;
} InstrumentChunk;

//// MIDI
/*
typedef struct {
    unsigned char       MIDIdata[];
} MIDIDataChunk;
*/

//// AESD

// just fixed-length array of data,
// AES recording data for user convenience
typedef struct 
{
    unsigned char       AESChannelStatusData[24];
} AudioRecordingChunk;

//// APPL

typedef unsigned long OSType;
/*
typedef struct {
    OSType      applicationSignature;
    char        data[]; // optional -> don't include in struct
} ApplicationSpecificChunk;
*/

//// COMT

typedef struct {
    unsigned long       timeStamp;
    MarkerId            marker;
    unsigned short      count;
} CommentFields;
// -> followed by actual string

#pragma pack(pop)

/*
  note about pstring:
  pstring: 	Pascal-style string, a one byte count followed by text bytes. 
  The total number of bytes in this data type should be even. 
  A pad byte can be added at the end of the text to accomplish this. 
  This pad byte is not reflected in the count.
*/

// pstring handling,
// single byte as length followed by character-data
class PascalString
{
public:
	PascalString(void)
	    : m_stringlen(0)
	    , m_szString()
	{}
	~PascalString(void)
	{}
	
	// when used in marker 'pstring'
	void ReadBuffer(const unsigned char *pData)
	{
		m_stringlen = pData[0]; // stored as single byte
		m_szString.assign((const char *)pData +1, m_stringlen);
	}

	// when in comment as longer string
	void ReadBuffer(const unsigned short count, const unsigned char *pData)
	{
		m_stringlen = count;
		m_szString.assign((const char *)pData, m_stringlen);
	}
	
	// stored as single byte
	unsigned short m_stringlen;
	std::string m_szString;
};

class Marker
{
public:
	Marker(void)
	    : id(0)
	    , position(0)
	    , string()
	{}
	~Marker(void)
	{}
	
    MarkerId            id;
    unsigned long       position;
	PascalString        string;
};

class Comment
{
public:
	Comment(void)
	    : timeStamp(0)
	    , marker(0)
	    , string()
	{}
	~Comment(void)
	{}
	
	// timestamp:
	// - on Amiga, seconds since January 1, 1978
	// - on Apple, seconds since January 1, 1904
    unsigned long       timeStamp;
    MarkerId            marker; // link to Marker
	PascalString        string;
};

class Common
{
public:
	Common(void)
	    : numChannels(0)
	    , numSampleFrames(0)
	    , sampleSize(0)
	    , sampleRate(0)
	{}
	~Common(void)
	{}
	
	//void HandleCommonChunk(CommonChunk *pComm);
	
    short           numChannels;
    unsigned long   numSampleFrames;
    short           sampleSize;
	double          sampleRate; // note: convert from 'extended' (long double) when not supported (Visual C++)
	
};

// compression handling, pure virtual base
class CAifcCompression
{
public:
    const unsigned long m_ulCompresionType;
    std::string m_szCompressionName;
    
public:
    CAifcCompression(const unsigned long ulCompressionType)
        : m_ulCompresionType(ulCompressionType)
    {}
    ~CAifcCompression()
    {}
    
    //virtual void Decode() = 0;
    //virtual void Encode() = 0;
};

// 'NONE' compression
class CAifcNone : public CAifcCompression
{
public:
    CAifcNone()
        : CAifcCompression(0x4E4F4E45)
    {}
    ~CAifcNone()
    {}
    
    //virtual void Decode() {}
    //virtual void Encode() {}
};

/*
// 'sowt'/'twos' for little-endian type (just byteswap, no compression)
class CAifcSowt : public CAifcCompression
{};
// 
class CAifcTwos : public CAifcCompression
{};
*/

/*
class CAifcCompressionFl32 : public CAifcCompression
{};
class CAifcCompressionFl64 : public CAifcCompression
{};
*/

class CIffAiff : public AudioFile, public CIffContainer
{
private:
	CMemoryMappedFile m_File;
	//CIffHeader *m_pHead; // inherited now

protected:
	
	Common m_Common; // COMM
    
    // compression type and name from extended common chunk,
    // AIFF-C only, compression handling support
    CAifcCompression *m_pCompression;
    unsigned long m_ulAifcVersionDate; // version of standard (creation date)
    
    // actual sample frame data
    SoundDataChunk m_SoundData; // SSND
    
    // count when needed..
    //uint8_t *m_pSoundData; // pointer to file data (must change if file is reopened..)
    
	OSType m_OSType; // APPL
	AudioRecordingChunk m_AesdChunk; // AESD, AES recording data
	InstrumentChunk m_Instrument; // INST
	
	std::string m_szName; // NAME
	std::string m_szAuthor; // AUTH
	std::string m_szAnnotations; // ANNO
	std::string m_szCopyright; // (c)
	
	typedef std::vector<Marker> tMarkerList;
	tMarkerList m_Markers;
	
	typedef std::vector<Comment> tCommentList;
	tCommentList m_Comments;
	
	
protected:

	double ExtendedToDouble(unsigned char *pvalue);

    void Decode(CIffChunk *pChunk, CMemoryMappedFile &pFile);

    // get handler for compression used in file
    virtual CAifcCompression *GetCompression(CIffChunk *pChunk, uint8_t *pChunkData);
    
    // process detected chunk
	virtual void OnChunk(CIffChunk *pChunk, CMemoryMappedFile &pFile);
	
	virtual bool IsSupportedType(CIffHeader *pHeader)
	{
        // standard AIFF chunk ID
		if (pHeader->m_iTypeID == MakeTag("AIFF"))
		{
			return true;
		}
        // AIFF-C chunk ID,
        // TODO: check rest of code for support
        if (pHeader->m_iTypeID == MakeTag("AIFC"))
        {
            return true;
        }
		return false;
	}

	CIffChunk *GetDataChunk() const
	{
		return GetChunkById(MakeTag("SSND"));
	}
	
public:
	CIffAiff(void);
	virtual ~CIffAiff(void);

	virtual bool ParseFile(const std::wstring &szFileName);

	// values to use for QAudioFormat or similar
	//codec (PCM-coded)
	
	virtual bool isBigEndian()
	{
		return true;
	}
	virtual long channelCount()
	{
		return m_Common.numChannels;
	}
	virtual unsigned long sampleRate()
	{
		// note: check value unit
        // we may have this in kHz
		return m_Common.sampleRate;
	}
	virtual long sampleSize()
	{
		return m_Common.sampleSize;
	}
	
	virtual bool isSigned()
	{
		// always signed?
		// (2's complement)
		return true;
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
        
        // count position of data (we may have offset to certain frame..)
        uint8_t *pChunkData = CIffContainer::GetViewByOffset(pDataChunk->m_iOffset, pFile);
        return (pChunkData + m_SoundData.offset);
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
		return (pDataChunk->m_iChunkSize - sizeof(SoundDataChunk));
	}
};

#endif // IFFAIFF_H
