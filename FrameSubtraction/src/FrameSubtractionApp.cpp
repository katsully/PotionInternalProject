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


using namespace ci;
using namespace ci::app;
using namespace std;

class FrameSubtractionApp : public AppNative {
public:
    void setup();
    void keyDown( KeyEvent event );
    void prepareSettings( Settings* settings );
    void draw();
private:
    Channel16u mChannel;
    OpenNI::DeviceRef mDevice;
    OpenNI::DeviceManagerRef mDeviceManager;
    gl::TextureRef mTexture;
    
    void onDepth( openni::VideoFrameRef frame, const OpenNI::DeviceOptions& deviceOptions );
    void screenShot();
};

void FrameSubtractionApp::setup(){
    mDeviceManager = OpenNI::DeviceManager::create();
    
    if( mDeviceManager->isInitialized() ){
        try{
            mDevice = mDeviceManager->createDevice();
        } catch ( OpenNI::ExcDeviceNotAvailable ex ){
            console() << ex.what() << endl;
            quit();
            return;
        }
        
        if( mDevice ){
            mDevice->connectDepthEventHandler( &FrameSubtractionApp::onDepth, this);
            mDevice->start();
        }
    }
}

void FrameSubtractionApp::onDepth( openni::VideoFrameRef frame, const OpenNI::DeviceOptions& deviceOptions ){
    mChannel = OpenNI::toChannel16u( frame );
}

void FrameSubtractionApp::keyDown( KeyEvent event ){
}

void FrameSubtractionApp::prepareSettings( Settings* settings ){
    settings->setFrameRate( 60.0f );
    settings->setWindowSize( 800, 600 );
}

void FrameSubtractionApp::draw()
{
    gl::setViewport( getWindowBounds() );
    // clear out the window with black
    gl::clear( Color( 0, 0, 0 ) );
    
    if( mChannel ){
        if( mTexture ){
            mTexture->update( Channel32f( mChannel ) );
        } else {
            mTexture = gl::Texture::create( Channel32f( mChannel ) );
        }
        gl::draw( mTexture, mTexture->getBounds(), getWindowBounds() );
    }
}

CINDER_APP_NATIVE( FrameSubtractionApp, RendererGl )
