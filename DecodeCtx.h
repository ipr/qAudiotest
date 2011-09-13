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
    // channels*(samplesize/8) -> single frame in bytes
    size_t m_nFrameSize;
    
    // count of frames:
    // filesize / framesize
    uint64_t m_nFrameCount;
    
    // bytes per second:
    // filesize / (framesize*frequency)
    double m_dBytesPerSecond;
    
public:
    DecodeCtx() {}
    

    // set values to start (same as update(0))
    void setBegin();
    
    // step forwards n frames
    void stepFwd(int64_t i64Count = 0);
    
    // step backwards n frames
    void stepBck(int64_t i64Count = 0);
    
    // set values to start (same as update(length()))
    void setEnd();
    
    // force absolute position
    void updatePos(int64_t i64Pos);
    
    // get current position
    int64_t position();
    
    // total length
    int64_t length();
};


#endif // DECODECTX_H
