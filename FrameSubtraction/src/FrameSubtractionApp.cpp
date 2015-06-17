//
// By: Kat Sullivan
// June 16, 2015
// Internal Potion Project
//

#include "cinder/app/AppNative.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/gl.h"
#include "Cinder-OpenNI.h"
#include "CinderOpenCV.h"
#include "cinder/Rand.h"


using namespace ci;
using namespace ci::app;
using namespace std;

class FrameSubtractionApp : public AppNative {
 public:
    void setup();
    void prepareSettings( Settings* settings );
    void keyDown( KeyEvent event );
    void update();
    void draw();
    
    cv::Mat mPreviousFrame;
    vector< cv::Point2f > mPreviousFeautres, mFeatures;
    vector< uint8_t > mFeatureStatuses;
    
    Color mColor;
    
    
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

void FrameSubtractionApp::setup(){
    
    // Setup the parameters
//    mParams = params::INterfaceGl("Parameters", Vec2i(200, 150) );
//    mParams.addParam( "Picked Color", &mPickedColor, "readonly=1" );
    mColor = Color::white();
    
    mDeviceManager = OpenNI::DeviceManager::create();
    
    if( mDeviceManager->isInitialized() ){
        try{
            mDevice = mDeviceManager->createDevice( OpenNI::DeviceOptions().enableColor() );
        } catch ( OpenNI::ExcDeviceNotAvailable ex ){
            console() << ex.what() << endl;
            quit();
            return;
        }
        
        if( mDevice ){
            mDevice->connectDepthEventHandler( &FrameSubtractionApp::onDepth, this);
            mDevice->connectColorEventHandler( &FrameSubtractionApp::onColor, this );
            mPreviousFrame = cv::Mat( 240,320, CV_16UC1 );
            mDevice->start();
        }
    }
}

void FrameSubtractionApp::prepareSettings( Settings* settings ){
    settings->setFrameRate( 60.0f );
    settings->setWindowSize( 800, 600 );
}

void FrameSubtractionApp::onDepth(openni::VideoFrameRef frame, const OpenNI::DeviceOptions& deviceOptions){
    
    cv::Mat mInput = toOcv( OpenNI::toChannel16u( frame ) );
    cv::Mat mInput8bit;
    cv::Mat mSubtracted;
    cv::Mat mThreshold;
    cv::Mat mThresholdCopy;
    
    
    
    // subtrackted depth
    cv::absdiff( mPreviousFrame, mInput, mSubtracted );
    
    mSubtracted.convertTo( mThreshold, CV_8UC1 );
    cv::threshold( mThreshold, mThreshold, 100 ,0, cv::THRESH_BINARY );
    
    
    
    if ( ci::app::getElapsedFrames() % 200 )
    {
        cout << "my type " << mInput.type() << endl;
        cout << "my channels " << mInput.channels() << endl;
        cout << "my size " << mInput.size() << endl;
        //cv::imwrite( "test_threshold.png" ,  mThreshold );
    }
    
    // convert to RGB color space, with some compensation
    //mInput.convertTo( mInput8bit, CV_8UC3, 0.1/1.0 );
    mSubtracted.convertTo( mInput8bit, CV_8UC3, 0.1/1.0 );
    
    mInput.copyTo( mPreviousFrame );
    
    mSurfaceDepth = Surface8u( fromOcv( mInput8bit ) );
}

void FrameSubtractionApp::onColor(openni::VideoFrameRef frame, const OpenNI::DeviceOptions& deviceOptions){
    mSurface = OpenNI::toSurface8u( frame );
    cv::Mat mInput( toOcv( OpenNI::toSurface8u( frame), 0 ) );
}

void FrameSubtractionApp::update(){

}

void FrameSubtractionApp::keyDown( KeyEvent event ){
    mColor = Color( randFloat(), randFloat(), randFloat() );
}

void FrameSubtractionApp::draw()
{
    gl::setViewport( getWindowBounds() );
    // clear out the window with black
    gl::clear( Color( 0, 0, 0 ) );
    if( mTexture ){
        gl::color( mColor );
        gl::draw( mTexture, getWindowBounds() );
    }
    
    if( mSurface ){
        if( mTexture ){
            mTexture->update( mSurface );
        } else {
            mTexture = gl::Texture::create( mSurface );
        }
        gl::draw( mTexture, mTexture->getBounds(), getWindowBounds() );
    }
    
    if( mSurfaceDepth ){
        if( mTextureDepth ) {
            mTextureDepth->update( mSurfaceDepth );
        } else {
            mTextureDepth = gl::Texture::create( mSurfaceDepth );
        }
        gl::draw( mTextureDepth, mTextureDepth->getBounds(), getWindowBounds() );
    }
}

CINDER_APP_NATIVE( FrameSubtractionApp, RendererGl )
