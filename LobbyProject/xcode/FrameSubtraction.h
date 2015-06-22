//
//  FrameSubtraction.h
//  LobbyProject
//
//  Created by Kathleen Sullivan on 6/22/15.
//
//

#include "cinder/app/AppNative.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/gl.h"
#include "Cinder-OpenNI.h"
#include "CinderOpenCV.h"
#include "cinder/Rand.h"
#include "ParticleController.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class FrameSubtraction {
public:
    FrameSubtraction();
    void setup();
    void keyDown( KeyEvent event );
    void update();
    void draw();
    
    cv::Mat mPreviousFrame;
    cv::Mat mBackground;
    
    Color mColor;
    
    cv::SimpleBlobDetector mDetector;
    vector<cv::KeyPoint> mKeyPoints;
    
    ParticleController mParticleController;
    
    
private:
    OpenNI::DeviceRef mDevice;
    OpenNI::DeviceManagerRef mDeviceManager;
    gl::TextureRef mTexture;
    gl::TextureRef mTextureDepth;
    
    ci::Surface8u mSurface;
    ci::Surface8u mSurfaceDepth;
    
    void onDepth(openni::VideoFrameRef frame, const OpenNI::DeviceOptions& deviceOptions);
    void onColor(openni::VideoFrameRef frame, const OpenNI::DeviceOptions& deviceOptions);
    void screenShot();
};
