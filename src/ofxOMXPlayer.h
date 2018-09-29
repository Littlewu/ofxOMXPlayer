#pragma once

#include "ofMain.h"
#include "ofxOMXPlayerEngine.h"


class ofxOMXPlayer;

class ofxOMXPlayerListener
{
public:
    virtual void onVideoEnd(ofxOMXPlayer*) = 0;
};

class ofxOMXPlayer : public EngineListener
{
public:
     
    
    COMXCore omxCore;
    ofxOMXPlayerEngine engine;

    
    int speedMultiplier;

    ofxOMXPlayerSettings settings;
    ofxOMXPlayerListener* listener;
    bool engineNeedsRestart;
    ofxOMXPlayer()
    {
        listener = NULL;
        engineNeedsRestart = false;
        OMX_Init();
        av_register_all();
        avformat_network_init();
        omxCore.Initialize();
        ofAddListener(ofEvents().update, this, &ofxOMXPlayer::onUpdate);

        
    }
    ~ofxOMXPlayer()
    {
        
    }
    
    void loadMovie(string videoPath)
    {
        settings.videoPath = videoPath;
        engineNeedsRestart = true;
    }
    
    void reopen()
    {
        engineNeedsRestart = true;
    }
    
    void onVideoEnd()
    {
        if(listener)
        {
            listener->onVideoEnd(this);
        }
    }
    void onVideoLoop(bool needsRestart)
    {
        engineNeedsRestart = needsRestart;
    }

    
    bool setup(ofxOMXPlayerSettings settings_)
    {
      
        settings = settings_;
        if(settings.listener)
        {
            listener = settings.listener;  
        }
        bool result = openEngine();
        if(result)
        {
            engine.listener = this; 
        }
        return result;
    }
    
    void onUpdate(ofEventArgs& eventArgs)
    {
        if(engineNeedsRestart)
        {
            engineNeedsRestart = false;
            if(isOpen())
            {
                engine.close();
            }
            
            setup(settings);
        }
    }
    void close()
    {
        engine.close();
    }
    
    void enableLooping()
    {
        engine.m_loop = true;
    }
    
    void disableLooping()
    {
        engine.m_loop = false; 
    }
    
    bool isLoopingEnabled()
    {
        return engine.m_loop; 
    }
    
    
    bool isTextureEnabled()
    {
        return settings.enableTexture;
    }
    
   
    int getWidth()
    {
        return engine.videoWidth; 
    }
    
    int getHeight()
    {
        return engine.videoHeight; 
    }
    
    
    bool openEngine()
    {
        return engine.setup(settings);
    }

    void draw(float x, float y, float w, float h)
    {
        engine.draw(x, y, w, h);
    }
    
    ofTexture&  getTextureReference()
    {
        return engine.fbo.getTextureReference();
    }
    
    GLuint getTextureID()
    {
        return engine.texture.getTextureData().textureID;
    }
    
    bool isPaused()
    {
        return engine.m_Pause;
    }
    
    void setPaused(bool doPause)
    {
        engine.m_Pause = doPause;
    }
    
    void togglePause()
    {
        engine.m_Pause = !engine.m_Pause;
    }
    
    void increaseVolume()
    {
        engine.increaseVolume();
    }
    
    void decreaseVolume()
    {
        engine.decreaseVolume();

    }
    
    void stepFrameForward()
    {
        stepNumFrames(1);
    }
    
    void stepNumFrames(int step)
    {
        engine.stepNumFrames(step);
    }
    
    void updatePixels()
    {
        engine.updatePixels();
    }
    
    bool isFrameNew()
    {
        return engine.hasNewFrame;
    }
    
    unsigned char* getPixels()
    {
        return engine.pixels;
    }
    
    void saveImage(string imagePath="")
    {
        if(!isTextureEnabled()) return;
        if(imagePath == "")
        {
            imagePath = ofToDataPath(ofGetTimestampString()+".png", true);
        }
        updatePixels();
        //TODO make smarter, re-allocating every time
        ofImage image;
        image.setFromPixels(getPixels(), getWidth(), getHeight(), OF_IMAGE_COLOR_ALPHA);
        image.saveImage(imagePath);
        
        ofLog() << "SAVED IMAGE TO: " << imagePath;
    }
    
    
    
    void setNormalSpeed()
    {
        engine.setNormalSpeed();
    }
    
    void increaseSpeed()
    {
        engine.increaseSpeed();
    }
    
    void decreaseSpeed()
    {
        engine.decreaseSpeed();
    }
    
    float getFPS()
    {
        return engine.videoFrameRate;
    }
    
    int getTotalNumFrames()
    {
        return engine.totalNumFrames;
    }
    float getVolume()
    {
        return engine.m_Volume;
    }
    
    float getDurationInSeconds()
    {
        return engine.duration;
    }
    void seekToTimeInSeconds(int timeInSeconds)
    {
        engine.seekToTimeInSeconds(timeInSeconds);
    }
    float getMediaTime()
    {
        float t = (float)(engine.omxClock.OMXMediaTime()*1e-6);
        return t;
    }
    
    int getCurrentFrame()
    {
        int result =0;
        int fps = getFPS();
        if(fps)
        {            
            result = getMediaTime()*fps;
        }
        return result;
    }
    
    void seekToFrame(int frameTarget)
    {
        engine.seekToFrame(frameTarget);

    }
    
    void fastForward()
    {
        engine.fastForward();
    }
    
    void rewind()
    {
        engine.rewind();
    }
    
    void restartMovie()
    {
        seekToFrame(0);
    }
    
    void setVolumeNormalized(float volume)
    {
        float value = ofMap(volume, 0.0, 1.0, -6000.0, 6000.0, true);
        engine.m_Volume = value;
    }
    
    float getVolumeNormalized()
    {
        float value = ofMap(engine.m_Volume, -6000.0, 6000.0, 0.0, 1.0, true);
        return value;
    }
    
    int getClockSpeed()
    {
        return engine.omxClock.OMXPlaySpeed();
    }
    
    string getInfo()
    {
        stringstream info;
        info << "APP FPS: "+ ofToString(ofGetFrameRate()) << endl;
        if(isOpen())
        {
            int t = getMediaTime();
            info << "MEDIA TIME: " << (t/3600)<<"h:"<< (t/60)%60 <<"m:"<< t%60 <<":s"<<  " raw: " << getMediaTime() <<endl;
            
            info << "OMX CLOCK SPEED: " << getClockSpeed() << endl;
            info << "DIMENSIONS: " << getWidth()<<"x"<<getHeight();
            
            info << "FPS: " << getFPS() << endl;
            info << "DURATION IN SECS: " << getDurationInSeconds() << endl;
            info << "TOTAL FRAMES: " << getTotalNumFrames() << endl;
            info << "CURRENT FRAME: " << getCurrentFrame() << endl;
            if (getTotalNumFrames() > 0) 
            {
                info << "REMAINING FRAMES: " << getTotalNumFrames() - getCurrentFrame() << endl;
            }else 
            {
                info << "REMAINING FRAMES: N/A, NO TOTAL FRAMES" << endl;
            }        
            info << "CURRENT VOLUME: " << getVolume() << endl;
            info << "CURRENT VOLUME NORMALIZED: " << getVolumeNormalized() << endl; 
        }else
        {
            info << "CLOSED" << endl;

        }

        
        return info.str();
    }
    //void        draw(float x=0, float y=0);
    //void        draw(ofRectangle&);
    bool isOpen()
    {
        
        return engine.isOpen;
    }
    
    bool getIsOpen()
    {
        return isOpen();
    }
    
    bool isPlaying()
    {
        bool result = false;
        if(isOpen() && !isPaused() && engine.isThreadRunning())
        {
            result = true;
        }
        return result;
    }
    
    float getPlaybackSpeed()
    {
        return engine.currentPlaybackSpeed;
    }
    
    
    int getSpeedMultiplier()
    {
        ofLogError() << "NOT IMPLEMENTED";
        return 0;
    }
   
    void scrubForward(int step=1)
    {
        ofLogError() << "NOT IMPLEMENTED";

    }
    
    
    COMXStreamInfo&  getVideoStreamInfo()
    {
        return engine.m_config_video.hints;
    }
    COMXStreamInfo&  getAudioStreamInfo()
    {
         return engine.m_config_audio.hints;
    }
    
    static string getRandomVideo(string path)
    {
        string videoPath = "";
        
        ofDirectory currentVideoDirectory(ofToDataPath(path, true));
        if (currentVideoDirectory.exists())
        {
            currentVideoDirectory.listDir();
            currentVideoDirectory.sort();
            vector<ofFile> files = currentVideoDirectory.getFiles();
            if (files.size()>0)
            {
                if (files.size()==1)
                {
                    videoPath = files[0].path();
                }else
                {
                    int randomIndex = ofRandom(files.size());
                    videoPath = files[randomIndex].path(); 
                }
                
            }
            
        }else
        {
            ofLogError(__func__) << "NO FILES FOUND AT" << currentVideoDirectory.path();
        }
        
        if(videoPath.empty())
        {
            ofLogWarning(__func__) << "returning empty string";
        }
        return videoPath;
    }  
    
#if 0     
    void applyFilter(OMX_IMAGEFILTERTYPE filter);

    //direct only
    void        setDisplayRect(float x, float y, float width, float height);
    void        setDisplayRect(ofRectangle&);
    void        cropVideo(ofRectangle&);
    void        cropVideo(float x, float y, float width, float height);
    void        rotateVideo(int degrees);
    void        setMirror(bool);
    void        setAlpha(int alpha);
    void        setFullScreen(bool);
    void        setForceFill(bool);
    ofRectangle* cropRectangle;
    ofRectangle* drawRectangle;
    
#endif
    
};


