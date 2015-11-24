#pragma once

#include "ofMain.h"

#include "BaseVideoDecoder.h"
#include "OMXDisplay.h"


class VideoDecoderDirect : public BaseVideoDecoder
{
public:
  
    VideoDecoderDirect();
    ~VideoDecoderDirect();
    bool open(StreamInfo& streamInfo,
              OMXClock *clock,
              bool deinterlace = false, 
              bool hdmi_clock_sync = false);
    
    bool decode(uint8_t *pData, int iSize, double pts);
  
    void updateDisplay(ofRectangle& cropRectangle, ofRectangle& rectangle);
    void updateFrameCount();
    void onUpdate(ofEventArgs& args);
    
    int getCurrentFrame();
    void resetFrameCounter();
    
    Component imageFXComponent;
    Tunnel imageFXTunnel;
    bool doDeinterlace;
    bool doHDMISync;    
    OMXDisplay display;
    OMXDisplay* getOMXDisplay()
    {
        return &display;
    }
    
private:
    int frameCounter;
    int frameOffset;
    
    
};