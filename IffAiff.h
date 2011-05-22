/////////////////////////////////////////////////////////
//
// CIffAiff : IFF-AIFF audio format parser
// (Audio Interchange File Format, Audio IFF)
//
// (c) Ilkka Prusi, 2011
//


#ifndef IFFAIFF_H
#define IFFAIFF_H

#include <stdint.h>

#include "MemoryMappedFile.h"
#include "IffContainer.h"

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

//// SSND
typedef struct 
{
    unsigned long       offset;
    unsigned long       blockSize;
    unsigned char       soundData[];
} SoundDataChunk;  

//// MARK

typedef short   MarkerId;

/*
  note about pstring:
  pstring: 	Pascal-style string, a one byte count followed by text bytes. 
  The total number of bytes in this data type should be even. 
  A pad byte can be added at the end of the text to accomplish this. 
  This pad byte is not reflected in the count.
*/
// -> see class PascalString
/*
typedef struct
{
	BYTE                nameLength;
	char                markerName[];
} pstring;

typedef struct {
    MarkerId            id;
    unsigned long       position;
    //pstring             markerName;
} Marker;
// -> followed by PascalString

typedef struct {
    unsigned short      numMarkers;
    Marker              Markers[];
} MarkerChunk;
// -> 
*/

//// INST

typedef struct {
    short           playMode;
    MarkerId        beginLoop;
    MarkerId        endLoop;
} Loop;

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

// just fixed-length array of data
typedef struct 
{
    unsigned char       AESChannelStatusData[24];
} AudioRecordingChunk;

//// APPL

typedef unsigned long OSType;
typedef struct {
    OSType      applicationSignature;
    char        data[];
} ApplicationSpecificChunk;

//// COMT

typedef struct {
    unsigned long       timeStamp;
    MarkerId            marker;
    unsigned short      count;
    //char                text;
} CommentFields;

/*
typedef struct {
    unsigned short      numComments;
    Comment             comments[];
} CommentsChunk;
*/

#pragma pack(pop)

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
		m_szString.assign(pData +1, m_stringlen);
	}

	// when in comment as longer string
	void ReadBuffer(const unsigned short count, const unsigned char *pData)
	{
		m_stringlen = count;
		m_szString.assign(pData, m_stringlen);
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

class CIffAiff : public CIffContainer
{
private:
	CMemoryMappedFile m_File;
	//CIffHeader *m_pHead; // inherited now

protected:
	std::string m_szName; // NAME
	std::string m_szAuthor; // AUTH
	std::string m_szAnnotations; // ANNO
	std::string m_szCopyright; // (c)
	
	typedef std::vector<Marker> tMarkerList;
	tMarkerList m_Markers;
	
	typedef std::vector<Comment> tCommentList;
	tCommentList m_Comments;
	
	
protected:

	virtual void OnChunk(CIffChunk *pChunk, CMemoryMappedFile &pFile);
	
	virtual bool IsSupportedType(CIffHeader *pHeader)
	{
		if (pHeader->m_iTypeID == MakeTag("AIFF"))
		{
			return true;
		}
		return false;
	}
	
	
public:
	CIffAiff(void);
	virtual ~CIffAiff(void);

	bool ParseFile(LPCTSTR szPathName);

};

#endif // IFFAIFF_H
