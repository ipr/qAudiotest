//////////////////////////////////////
//
// DecodeCtx.h
//
// base class for audio decode/playback
// status and control,
// to use with decoder control and status.
//
// gives unified interface across formats
// and decoders.
//
// Author: Ilkka Prusi, 2011
// Contact: ilkka.prusi@gmail.com
//

#ifndef DECODECTX_H
#define DECODECTX_H

#include <stdint.h>

// TODO: in progress..
// allow deriving for implementations specifics..
//
// also use internally in decoder/playback handler
// to keep track of status and positions.
//
class DecodeCtx
{
protected:
    
    // TODO: check for necessary helpers during playback..
    
    // current frame position in decode/playback
    uint64_t m_nCurrentFrame;
    
    // decode/playback frame size:
    // channels * (samplesize/8) 
    // -> single frame in bytes
    size_t m_nFrameSize;
    
    // count of frames per second:
    uint64_t m_nFramesPerSecond;
    
    // frames per second?
    
    // bytes per second:
    // filesize / (framesize * sample rate)
    double m_dBytesPerSecond;
    
    // TODO:
    // duration of single frame in fractions of second?
    // (for timing-conversions?)
    //double m_dFrameDuration;

    // channel count: amount of channels per frame
    // sample size: sample width in bits    
    void setFrameSize(const size_t nChannels, const size_t nSampleSize)
    {
        m_nFrameSize = nChannels * (nSampleSize/8);
    }
    /*
    void setFrameCount(const uint64_t fileSize)
    {
        m_nFrameCount = (fileSize / m_nFrameSize);
    }
    */
    // for simplistic buffer estimate:
    // filesize in bytes
    // sample rate in Hz
    void setByteRate(const uint64_t fileSize, const double dSampleRate)
    {
        m_dBytesPerSecond = (fileSize / (m_nFrameSize * dSampleRate));
    }
    
    //
    // TODO: helper conversions?
    // some mods have stuff like VBlank/VSync value for timing
    // which need to be converted for playback on entirely different hardware..
    // some use stuff like CPU jiffies/ticks for timing values..
    // need some cross-platform timing support such that
    // duration = (base-frequency / dividend) .. where duration is some suitable unit..
    // e.g. dur = 50.0MHz / 2 -> 25.0MHz
    // 
    
public:
    DecodeCtx() 
        : m_nCurrentFrame(0)
        , m_nFrameSize(0)
        , m_nFramesPerSecond(0)
        , m_dBytesPerSecond(0)
    {}
    
    // this is currently just for linear PCM file (uncompressed),
    // need other ways later..
    void initialize(const size_t nChannels, const size_t nSampleSize, const double dSampleRate)
    {
        m_nFrameSize = nChannels * (nSampleSize/8);
        //m_nFramesPerSecond = (m_nFrameSize * dSampleRate);
        
        m_dBytesPerSecond = (m_nFrameSize * dSampleRate);
        
        //setFrameSize(nChannels, nSampleSize);
        //setFrameCount(fileSize);
        //setByteRate(fileSize, dSampleRate);
    }

    // set values to start (same as update(0))
    void setBegin()
    {
        m_nCurrentFrame = 0;
    }
    
    // step forwards n frames
    void stepFwd(int64_t i64Count = 0)
    {
        if ((m_nCurrentFrame + i64Count) < m_nFrameCount)
        {
            m_nCurrentFrame += i64Count;
        }
        else
        {
            // just set to end
            m_nCurrentFrame = m_nFrameCount;
        }
    }
    
    // step backwards n frames
    void stepBck(int64_t i64Count = 0)
    {
        if (m_nCurrentFrame > i64Count)
        {
            m_nCurrentFrame -= i64Count;
        }
        else
        {
            // just set to start
            m_nCurrentFrame = 0;
        }
    }
    
    // set values to start (same as update(length()))
    void setEnd()
    {
        m_nCurrentFrame = m_nFrameCount;
    }
    
    // force absolute position
    void updatePos(const int64_t i64Pos)
    {
        m_nCurrentFrame = i64Pos;
    }
    
    // get current frame position
    uint64_t position()
    {
        return m_nCurrentFrame;
    }
    
    // total length in frames
    uint64_t length()
    {
        return m_nFrameCount;
    }
    
    // meh.. does not include inherited (stupid mistake)
    //friend class CIffContainer;
};


#endif // DECODECTX_H
