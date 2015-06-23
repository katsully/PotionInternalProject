//
//  FrameSubtraction.cpp
//  LobbyProject
//
//  Created by Kathleen Sullivan on 6/22/15.
//
//

#include "FrameSubtraction.h"

FrameSubtraction::FrameSubtraction(){
    
}

void FrameSubtraction::setup(){
    
    // Setup the parameters
    //    mParams = params::INterfaceGl("Parameters", Vec2i(200, 150) );
    //    mParams.addParam( "Picked Color", &mPickedColor, "readonly=1" );
    mColor = Color(1, 0, 0);
    
    mDeviceManager = OpenNI::DeviceManager::create();
    
    if( mDeviceManager->isInitialized() ){
        try{
            mDevice = mDeviceManager->createDevice( OpenNI::DeviceOptions().enableColor() );
        } catch ( OpenNI::ExcDeviceNotAvailable ex ){
            ci::app::console() << ex.what() << endl;
            return;
        }
        
        if( mDevice ){
            mDevice->connectDepthEventHandler( &FrameSubtraction::onDepth, this);
            mDevice->connectColorEventHandler( &FrameSubtraction::onColor, this );
            mPreviousFrame = cv::Mat( 240,320, CV_16UC1 );
            mBackground = cv::Mat( 240,320, CV_16UC1 );
            mDevice->start();
        }
    }
}

void FrameSubtraction::onDepth(openni::VideoFrameRef frame, const OpenNI::DeviceOptions& deviceOptions){
    
    cv::Mat mInput = toOcv( OpenNI::toChannel16u( frame ) );
    cv::Mat mInput8bit;
    cv::Mat mSubtracted;
    cv::Mat mThreshold;
    cv::Mat mOutput;
    
    
    // subtrackted depth
    cv::absdiff( mBackground, mInput, mSubtracted );
    
    mSubtracted.convertTo( mThreshold, CV_8UC1 );
    
    cv::adaptiveThreshold(mThreshold, mThreshold, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY_INV, 105, 1);
    //cv::threshold( mThreshold, mThreshold, 128, 255, cv::THRESH_BINARY );
    cv::dilate( mThreshold, mThreshold, cv::Mat(), cv::Point( -1, -1), 2, 1, 1 );
    //cv::Mat kernel( 30, 30, CV_8UC1 );
    // cv::erode( mThreshold, mThreshold, cv::Mat() );
    
    cv::Mat contourOuput = mThreshold.clone();
    vector<vector<cv::Point> > contours;
    
    cv::findContours( contourOuput, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE );
    
    
    //    cout << "contours size: " << contours.size() << endl;
    //
    //    if ( ci::app::getElapsedFrames() % 200 )
    //    {
    //        cout << "my type " << mInput.type() << endl;
    //        cout << "my channels " << mInput.channels() << endl;
    //        cout << "my size " << mInput.size() << endl;
    //    }
    
    // convert to RGB
    cv::cvtColor( mThreshold,mThreshold, CV_GRAY2RGB );
    
    mThreshold.copyTo( mOutput );
    
    // draw all the contours
    cv::drawContours( mOutput, contours, -1, cv::Scalar(0,255,0) );
    
    //    cv::Mat imWithKeypoints;
    //    //blob detection
    //    cv::drawKeypoints(mThreshold, mKeyPoints, imWithKeypoints, cv::Scalar(0, 255, 0) );
    
    // save the current frame
    mInput.copyTo( mPreviousFrame );
    
    // display the threshold image
    mSurfaceDepth = Surface8u( fromOcv( mOutput ) );
    mParticleController.addParticles( mSurfaceDepth );
}

void FrameSubtraction::onColor(openni::VideoFrameRef frame, const OpenNI::DeviceOptions& deviceOptions){
    mSurface = OpenNI::toSurface8u( frame );
    cv::Mat mInput( toOcv( OpenNI::toSurface8u( frame), 0 ) );
}

void FrameSubtraction::update(){
    
}

void FrameSubtraction::keyDown( KeyEvent event ){
    mPreviousFrame.copyTo( mBackground );
}

void FrameSubtraction::draw()
{
    gl::setViewport( ci::app::getWindowBounds() );
    // clear out the window with black
    // gl::clear( Color( 0, 0, 0 ) );
    if( mTexture ){
        //    gl::color( mColor );
        gl::draw( mTexture, ci::app::getWindowBounds() );
    }
    //
    //    if( mSurface ){
    //        if( mTexture ){
    //            mTexture->update( mSurface );
    //        } else {
    //            mTexture = gl::Texture::create( mSurface );
    //        }
    //        gl::draw( mTexture, mTexture->getBounds(), getWindowBounds() );
    //    }
    //
    //    if( mSurfaceDepth ){
    //        if( mTextureDepth ) {
    //            mTextureDepth->update( mSurfaceDepth );
    //        } else {
    //            mTextureDepth = gl::Texture::create( mSurfaceDepth );
    //        }
    //        gl::draw( mTextureDepth, mTextureDepth->getBounds(), getWindowBounds() );
    //    }
    mParticleController.draw();
}