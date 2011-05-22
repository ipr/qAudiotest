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


/* playMode specifies which type of looping is to be performed (INST) */
#define NoLooping               0
#define ForwardLooping          1
#define ForwardBackwardLooping  2


// from format specifications,
// note one-byte alignment in struct

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

typedef struct
{
	BYTE                nameLength;
	char                markerName[];
} pstring;

typedef struct {
    MarkerId            id;
    unsigned long       position;
    pstring             markerName;
} Marker;

typedef struct {
    unsigned short      numMarkers;
    Marker              Markers[];
} MarkerChunk;

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

typedef struct {
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
    char                text;
} Comment;

typedef struct {
    unsigned short      numComments;
    Comment             comments[];
} CommentsChunk;

#pragma pack(pop)


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
