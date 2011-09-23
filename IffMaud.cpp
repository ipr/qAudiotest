/////////////////////////////////////////////////////////
//
// CIffMaud : IFF-MAUD audio format parser,
// multiple sample-sizes, compression options etc.
// (upto 16-bit or more?)
//
// Based on documentation by: Richard Koerber
//
//
// Author: Ilkka Prusi, 2011
// Contact: ilkka.prusi@gmail.com
//

#include "IffMaud.h"

//////////////////// protected methods

void CIffMaud::OnChunk(CIffChunk *pChunk, CMemoryMappedFile &pFile)
{
	uint8_t *pChunkData = CIffContainer::GetViewByOffset(pChunk->m_iOffset, pFile);
	
	// TODO: check chunk ID
	if (pChunk->m_iChunkID == MakeTag("MHDR"))
	{
		MaudHeader *pMhdr = (MaudHeader*)pChunkData;
		m_MaudHeader.mhdr_Samples = Swap4(pMhdr->mhdr_Samples);
		m_MaudHeader.mhdr_SampleSizeC = Swap2(pMhdr->mhdr_SampleSizeC);
		m_MaudHeader.mhdr_SampleSizeU = Swap2(pMhdr->mhdr_SampleSizeU);
		m_MaudHeader.mhdr_RateSource = Swap4(pMhdr->mhdr_RateSource);
		m_MaudHeader.mhdr_RateDevide = Swap2(pMhdr->mhdr_RateDevide);
		m_MaudHeader.mhdr_ChannelInfo = Swap2(pMhdr->mhdr_ChannelInfo);
		m_MaudHeader.mhdr_Channels = Swap2(pMhdr->mhdr_Channels);
		m_MaudHeader.mhdr_Compression = Swap2(pMhdr->mhdr_Compression);
	}
	else if (pChunk->m_iChunkID == MakeTag("MDAT"))
	{
		// pure audio data (body)
		// -> process on playback (decode())
	}
	else if (pChunk->m_iChunkID == MakeTag("MINF"))
	{
		// (optional) channel info chunk (for future),
		// no details -> not implemented
	}
	else
	{
		// handle common IFF-standard chunks in base
		CIffContainer::OnChunk(pChunk, pFile);
	}
}


//////////////////// public methods

CIffMaud::CIffMaud(void)
	: AudioFile()
    , CIffContainer()
	, m_File()
{
}

CIffMaud::~CIffMaud(void)
{
	m_File.Destroy();
}

bool CIffMaud::ParseFile(const std::wstring &szFileName)
{
	if (m_File.Create(szFileName.c_str()) == false)
	{
		return false;
	}

	if (ParseIffFile(m_File) == nullptr)
	{
		return false;
	}

	// use default implementation here only
    m_pDecodeCtx = new DecodeCtx();
	m_pDecodeCtx->initialize(channelCount(), sampleSize(), sampleRate());
	
	return true;
}

// notes on channel format:
// channel order: left, right, surround..
// (see header), left always comes first
//
// same way as with AIFF?
//
// note: compression is truly bit-compression
// so two 3-bit samples are really packed to a single byte..
//
uint64_t CIffMaud::decode(unsigned char *pBuffer, const uint64_t nBufSize /*, QAudioFormat *pOutput*/)
{
	if (m_MaudHeader.mhdr_Compression != 0)
	{
		// TODO: ALAW/ULAW decompression..
		//return Decompress(pBuffer, nBufSize);
	}

	// get uncompressed data
	CIffChunk *pChunk = GetDataChunk();
	uint8_t *pChunkData = CIffContainer::GetViewByOffset(pChunk->m_iOffset, m_File);
	
	
	// previous frame position
	uint64_t frame = m_pDecodeCtx->position();
	double duration = m_pDecodeCtx->frameduration();
	size_t frameSize = m_pDecodeCtx->frameSize();
	double frameTime = frame*duration; // current time-index
	
	// count amount of whole frames fitting to given buffer
	size_t outFrames = (nBufSize/frameSize);

	// now we may need some bitshifting and byteswapping to make audio suitable for output..
	// 
	// TODO: need output format knowledge:
    // for now, assume 16-bit output
    // and align to that if different
    //
    int nOutSampleSize = 16;
    if (m_pDecodeCtx->sampleSize() <= 8)
    {
        // quick hack for something close..
        nOutSampleSize = 8;
    }
	
	int iMaskSize = 0;
	
	// samples are "bit-packed" across bytes
	// so for odd-sized samples (not multiples of 8)
	// we need some masking of bits to output..
	//
	if ((m_pDecodeCtx->sampleSize()) % 8 != 0)
	{
		iMaskSize = (m_pDecodeCtx->sampleSize() / 8);
	}

	bool bShiftUp = false;
    int shift = 0;
    if (m_pDecodeCtx->sampleSize() < 8)
    {
        shift = (8 - m_pDecodeCtx->sampleSize());
        bShiftUp = true;
    }
    else if (m_pDecodeCtx->sampleSize() <= 16)
    {
        shift = (16 - m_pDecodeCtx->sampleSize());
        bShiftUp = true;
    }
    else
    {
        if (m_pDecodeCtx->sampleSize() > nOutSampleSize)
        {
            // 32 "raw", 16 out?
            shift = (m_pDecodeCtx->sampleSize() - nOutSampleSize);
            bShiftUp = false;
        }
        else
        {
            // 16 "raw", 24 out?
            shift = (nOutSampleSize - m_pDecodeCtx->sampleSize());
            bShiftUp = true;
        }
    }
    
    // bits read from raw data..
    //uint64_t iRawBits = 0;

	
	// write to buffer as much as there fits
	for (size_t n = 0; n < outFrames; n++)
	{
		// "bit-packed" data:
		// single byte may hold two samples
		// such as two 3-bit samples..
		// take note especially in larger sample sizes
		/*
	    if (m_pDecodeCtx->sampleSize() <= 8)
	    {
			// just mask and copy to output, similar to this
			(*(pBuffer + n)) = ((*(pChunkData+m)) & mask);
			n++;
			(*(pBuffer + n)) = ((*(pChunkData+m)) & (mask << maskshift));
			n++;
	    }
	    else if (m_pDecodeCtx->sampleSize() > 8 && m_pDecodeCtx->sampleSize() <= 16)
	    {
			// byteswap and mask to output..
	    }
	    else if (m_pDecodeCtx->sampleSize() > 16 && m_pDecodeCtx->sampleSize() <= 24)
	    {
			// three bytes in output also..?
			// byteswap and mask to output..
			// drop fourth byte so next sample continues correctly..
	    }
	    else if (m_pDecodeCtx->sampleSize() > 24 && m_pDecodeCtx->sampleSize() <= 32)
	    {
			// byteswap and mask to output..
	    }
	    */
	}
	
	
	// keep which frame we finished on
	m_pDecodeCtx->updatePosition(frame + outFrames);
	
	// return bytes written to buffer:
	// same amount will be written to audiodevice
    return (outFrames*frameSize);
}

