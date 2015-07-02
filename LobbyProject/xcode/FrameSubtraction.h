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
#include "ParticleControllerController.h"
#include "Shape.h"

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
    
    ParticleControllerController mParticleControllerController;
    
    
private:
    OpenNI::DeviceRef mDevice;
    OpenNI::DeviceManagerRef mDeviceManager;
    gl::TextureRef mTexture;
    gl::TextureRef mTextureDepth;
    
    ci::Surface8u mSurface;
    ci::Surface8u mSurfaceDepth;
    
    typedef vector< vector<cv::Point > > ContourVector;
    ContourVector mContours;
    int shapeUID;
    int mBlurAmount;
    
    cv::Mat mInput;
    cv::vector<cv::Vec4i> mHierarchy;
    vector<Shape> mShapes;
    vector<Shape> mTrackedShapes;
    
    void onDepth(openni::VideoFrameRef frame, const OpenNI::DeviceOptions& deviceOptions);
    void onColor(openni::VideoFrameRef frame, const OpenNI::DeviceOptions& deviceOptions);
    vector< Shape > getEvaluationSet( vector< vector<cv::Point> > rawContours, int minimalArea, int maxArea );
    Shape* findNearestMatch( Shape trackedShape, vector< Shape > &shapes, float maximumDistance );
    void screenShot();
};
