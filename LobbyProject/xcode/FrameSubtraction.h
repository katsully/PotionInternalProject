//
//  FrameSubtraction.h
//  LobbyProject
//
//  Created by Kat Sullivan on 6/22/15.
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
    void update();
    void draw();
    
    cv::Mat mInput;
    
    vector<Shape> mTrackedShapes;   // store tracked shapes
    
private:
    OpenNI::DeviceRef mDevice;
    OpenNI::DeviceManagerRef mDeviceManager;
    gl::TextureRef mTexture;
    gl::TextureRef mTextureDepth;

    ci::Surface8u mSurface;
    ci::Surface8u mSurfaceDepth;
    
    short mNearLimit;
    short mFarLimit;
    
    typedef vector< vector<cv::Point > > ContourVector;
    ContourVector mContours;
    int shapeUID;

    cv::Mat mInput;
    cv::vector<cv::Vec4i> mHierarchy;
    vector<Shape> mShapes;
    
    void onDepth( openni::VideoFrameRef frame, const OpenNI::DeviceOptions& deviceOptions );
    void onColor( openni::VideoFrameRef frame, const OpenNI::DeviceOptions& deviceOptions );
    vector< Shape > getEvaluationSet( ContourVector rawContours, int minimalArea, int maxArea );
    Shape* findNearestMatch( Shape trackedShape, vector< Shape > &shapes, float maximumDistance );
    cv::Mat removeBlack( cv::Mat input, short nearLimit, short farLimit );
    void screenShot();
};
