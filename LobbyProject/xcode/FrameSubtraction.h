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
    void keyDown( KeyEvent event );
    void update();
    void draw();
    
    cv::Mat mPreviousFrame; // track the previous frame which is then used to set the background frame
    cv::Mat mBackground;    // stores the background image which is used for background subtraction to capture motion
    
        vector<Shape> mTrackedShapes;   // store tracked shapes
    
    
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
    
    void onDepth(openni::VideoFrameRef frame, const OpenNI::DeviceOptions& deviceOptions);
    void onColor(openni::VideoFrameRef frame, const OpenNI::DeviceOptions& deviceOptions);
    vector< Shape > getEvaluationSet( ContourVector rawContours, int minimalArea, int maxArea );
    Shape* findNearestMatch( Shape trackedShape, vector< Shape > &shapes, float maximumDistance );
    void screenShot();
};
