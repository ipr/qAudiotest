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

#include "IffAiff.h"

// use standard 80-bit extended (long double) to 64-bit double 
// conversion routines: for compatibility and less bugs,
// same code is used just about everywhere i think..
extern "C" 
{
 #include "ieee.h"
}


//////////////////// protected methods


// convert 80-bit extended-precision value ('long double')
// to 64-bit double since Visual C++ does not support it..
// (many CPU support it though..)
//
// Note about sampleRate regarding AIFF-C (DAVIC 1.4.1 standard):
//
// sampleRate: An 80-bit floating point value indicating the rate at which the sound was sampled. The format of the
// floating point value is double-extended precision floating point, which includes one sign bit, 15-bit exponent, and 64-
// bit mantissa according to the IEEE 96-bit floating point representation (using only 15 bits instead of 31 for the
// exponent). For DAVIC 1.4.1, the only valid sample rates are show in the table below.
//
// +-------------+------------------------+
// | Sample Rate | Hex Representation     |
// +-------------+------------------------+
// | 16.000 kHz  | 0x400CFA00000000000000 |
// | 22.050 kHz  | 0x400DAC44000000000000 |
// | 24.000 kHz  | 0x400DBB80000000000000 |
// | 32.000 kHz  | 0x400DFA00000000000000 |
// | 44.100 kHz  | 0x400EAC44000000000000 |
// | 48.000 kHz  | 0x400EBB80000000000000 |
// +-------------+------------------------+
//
double CIffAiff::ExtendedToDouble(unsigned char *pvalue)
{
    // use standard 80-bit extended (long double) to 64-bit double 
    // conversion routines: for compatibility and less bugs.
    // there aren't really alternatives either for correct algorithm..
    //
	return ConvertFromIeeeExtended(pvalue);
}

void CIffAiff::Decode(CIffChunk *pChunk, CMemoryMappedFile &pFile)
{
	uint8_t *pChunkData = CIffContainer::GetViewByOffset(pChunk->m_iOffset, pFile);
	
	int64_t i64SamplePointCount = m_Common.numSampleFrames * m_Common.numChannels;
	
    // handled before in SSND chunk
    uint8_t *pData = m_pSoundData;
    
	for (int64_t i = 0; i < i64SamplePointCount; i++)
	{
		// TODO: size of actual data?
        // byteswap, decode, format change?
        
        /*
        if (m_pCompression != nullptr)
        {
            m_pCompression->Decode();
        }
        */
        
		//uint8_t *pSampleData = new uint8_t[m_Common.sampleSize];
		//::memcpy(pSampleData, pData, m_Common.sampleSize);
		//pSound = pChunkData + sizeof(SoundDataChunk) + m_Common.sampleSize;
	}
}

// get handler for compression used in file,
// handle extended-part in common chunk
CAifcCompression *CIffAiff::GetCompression(CIffChunk *pChunk, uint8_t *pChunkData)
{
    ExtendedCommonChunk *pExtComm = (ExtendedCommonChunk*)(pChunkData+sizeof(CommonChunk));
    
    // should be 0x4E4F4E45 for 'NONE' when no compression (see DAVIC 1.4.1),
    // can be 'sowt'/'twos' for little-endian AIFF-C (Mac OS X) ?
    unsigned long ulCompressionType = Swap4(pExtComm->compressionType);

    CAifcCompression *pCompression = nullptr;
    switch (ulCompressionType)
    {
    case 0x4E4F4E45:
        pCompression = new CAifcNone();
        break;
        
    default:
        // base as placeholder for now..
        pCompression = new CAifcCompression(ulCompressionType);
        break;
    }
    
    // should be constant value 0x6E6F7420636F70726573736564
    // for 'no compression' when not compressed (see DAVIC 1.4.1)
    pCompression->m_CompressionName.ReadBuffer(pExtComm->compNameLength,
                                 pChunkData+sizeof(CommonChunk)+sizeof(ExtendedCommonChunk)
                                 );
    
    return pCompression;
}


// process detected chunk, file may have chunks in any order
// -> handle when found
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
		
		// note: convert 'sampleRate' from 'extended' (80-bit long double) to 64-bit double
		// since Visual C++ does not support it..
		m_Common.sampleRate = ExtendedToDouble(pComm->sampleRate);
        
        // extended common chunk (compression information)
        //
        // note: compression is not used in DAVIC 1.4.1 standard 
        // so compression type should be constant value 0x4E4F4E45
        //
        if (GetHeader()->m_iTypeID == MakeTag("AIFC")
            && pChunk->m_iChunkSize >= (sizeof(CommonChunk)+sizeof(ExtendedCommonChunk)))
        {
            // read extended-part of common chunk 
            // for compression handling object
            m_pCompression = GetCompression(pChunk, pChunkData);
        }
	}
	else if (pChunk->m_iChunkID == MakeTag("SSND"))
	{
        // Sound data chunk: one at most, zero if CommonChunk::numSampleFrames is zero
        //
        // has sample frame data
        SoundDataChunk *pSound = (SoundDataChunk*)pChunkData;
		m_SoundData.offset = Swap4(pSound->offset);
		m_SoundData.blockSize = Swap4(pSound->blockSize);
		m_pSoundData = (pChunkData +sizeof(SoundDataChunk));
		
		// when fixed-size data with offset defined
		if (m_SoundData.offset > 0)
		{
			m_pSoundData = (m_pSoundData + m_SoundData.offset);
		}
        
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
    else if (pChunk->m_iChunkID == MakeTag("FVER"))
	{
        // AIFF-C Format Version Chunk:
        // should be constant value of specification creation date
        // (0xA2805140 for 1990-05-23, 14:40)
        m_ulAifcVersionDate = Swap4((*((unsigned long*)pChunkData)));
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
    , m_pCompression(nullptr)
    , m_pSoundData(nullptr)
{
}

CIffAiff::~CIffAiff(void)
{
    m_pSoundData = nullptr; // don't delete, unmap view
    if (m_pCompression != nullptr)
    {
        delete m_pCompression;
    }
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

