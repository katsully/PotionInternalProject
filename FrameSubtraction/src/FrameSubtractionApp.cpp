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
    cv::Mat mBackground;
    
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
            mBackground = cv::Mat( 240,320, CV_16UC1 );
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
    cv::Mat mOutput;
    
    
    // subtrackted depth
    cv::absdiff( mBackground, mInput, mSubtracted );
    
    mSubtracted.convertTo( mThreshold, CV_8UC1 );
    
    // move zeros to the back of a temp array
//    cv::Mat copyImg = mBackground;
//    uint8* ptr = copyImg.datastart;
//    uint8* ptr_end = copyImg.dataend;
//    while (ptr < ptr_end) {
//        if (*ptr == 0) { // swap if zero
//            uint8 tmp = *ptr_end;
//            *ptr_end = *ptr;
//            *ptr = tmp;
//            ptr_end--; // make array smaller
//        } else {
//            ptr++;
//        }
//    }
    
    // make a new matrix with only valid data
 //   cv::Mat nz = cv::Mat(std::vector<uint8>(copyImg.datastart,ptr_end),true);
    
    // compute optimal Otsu threshold
  //  double thresh = cv::threshold(nz,nz,0,255,CV_THRESH_BINARY | CV_THRESH_OTSU);
    
    // apply threshold
  //  cv::threshold(mThreshold,mThreshold,thresh,255,CV_THRESH_BINARY_INV);
    
    cv::threshold( mThreshold, mThreshold, 230, 255, cv::THRESH_BINARY );
    
    cv::Mat contourOuput = mThreshold.clone();
    vector<vector<cv::Point> > contours;
    
    cv::findContours( contourOuput, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE );
    
    
    cout << "contours size: " << contours.size() << endl;
    
    if ( ci::app::getElapsedFrames() % 200 )
    {
        cout << "my type " << mInput.type() << endl;
        cout << "my channels " << mInput.channels() << endl;
        cout << "my size " << mInput.size() << endl;
    }
    
    // convert to RGB
    cv::cvtColor( mThreshold,mThreshold, CV_GRAY2RGB );
    
    mThreshold.copyTo( mOutput );
    
    // draw all the contours
    cv::drawContours( mOutput, contours, -1, cv::Scalar(0,255,0) );
    
    // save the current frame
    mInput.copyTo( mPreviousFrame );
    
    // display the threshold image
    mSurfaceDepth = Surface8u( fromOcv( mOutput ) );
}

void FrameSubtractionApp::onColor(openni::VideoFrameRef frame, const OpenNI::DeviceOptions& deviceOptions){
    mSurface = OpenNI::toSurface8u( frame );
    cv::Mat mInput( toOcv( OpenNI::toSurface8u( frame), 0 ) );
}

void FrameSubtractionApp::update(){

}

void FrameSubtractionApp::keyDown( KeyEvent event ){
    mColor = Color( randFloat(), randFloat(), randFloat() );
    mPreviousFrame.copyTo( mBackground );
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
