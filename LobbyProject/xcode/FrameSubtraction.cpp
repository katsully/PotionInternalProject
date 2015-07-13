//
//  FrameSubtraction.cpp
//  LobbyProject
//
//  Created by Kat Sullivan on 6/22/15.
//
//

#include "FrameSubtraction.h"

FrameSubtraction::FrameSubtraction()
{
}

void FrameSubtraction::setup( Json::Value data )
{
    // setup camera
    mDeviceManager = OpenNI::DeviceManager::create();
    
    shapeUID = 0;
    mTrackedShapes.clear();
    
    if ( mDeviceManager->isInitialized() ) {
        try {
            mDevice = mDeviceManager->createDevice( OpenNI::DeviceOptions().enableColor() );
        } catch ( OpenNI::ExcDeviceNotAvailable ex ){
            ci::app::console() << ex.what() << endl;
            return;
        }
        
        if (mDevice) {
            mDevice->connectDepthEventHandler( &FrameSubtraction::onDepth, this);
            mDevice->connectColorEventHandler( &FrameSubtraction::onColor, this );
            mDevice->start();
        }
    }
    
    // sets threshold to ignore all black pixels and pixels that are far away from the camera
    // read data from json file
    mNearLimit = data.get("mNearLimit", 0).asInt();
    mFarLimit = data.get("mFarLimit", 0).asInt();
    mThresh = data.get("mThresh", 0.0).asFloat();
    mMaxVal = data.get("mMaxVal", 0.0).asFloat();
}

void FrameSubtraction::onDepth( openni::VideoFrameRef frame, const OpenNI::DeviceOptions& deviceOptions )
{
    // convert frame from the camera to an OpenCV matrix
    mInput = toOcv( OpenNI::toChannel16u(frame) );

    cv::Mat thresh;
    cv::Mat eightBit;
    cv::Mat withoutBlack;
    
    // remove black pixels from frame which get detected as noise
    withoutBlack = removeBlack( mInput, mNearLimit, mFarLimit );
    
    // convert matrix from 16 bit to 8 bit with some color compensation
    withoutBlack.convertTo( eightBit, CV_8UC3, 0.1/1.0 );
    
    // invert the image
    cv::bitwise_not( eightBit, eightBit );
    
    mContours.clear();
    mApproxContours.clear();
    
    // using a threshold to reduce noise
    cv::threshold( eightBit, thresh, mThresh, mMaxVal, CV_8U );
    
    // draw lines around shapes
    cv::findContours( thresh, mContours, mHierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE );
    
    vector<cv::Point> approx;
    // approx number of points per contour
    for ( int i = 0; i < mContours.size(); i++ ) {
        cv::approxPolyDP(mContours[i], approx, 1, true );
        mApproxContours.push_back( approx );
    }
    
    mShapes.clear();
    // get data that we can later compare
    mShapes = getEvaluationSet( mApproxContours, 75, 100000 );
    
    // find the nearest match for each shape
    for ( int i = 0; i < mTrackedShapes.size(); i++ ) {
        Shape* nearestShape = findNearestMatch( mTrackedShapes[i], mShapes, 5000 );
        
        // a tracked shape was found, update that tracked shape with the new shape
        if ( nearestShape != NULL ) {
            nearestShape->matchFound = true;
            mTrackedShapes[i].centroid = nearestShape->centroid;
            mTrackedShapes[i].lastFrameSeen = ci::app::getElapsedFrames();
            mTrackedShapes[i].hull.clear();
            mTrackedShapes[i].hull = nearestShape->hull;
        }
    }
    
    // if shape->matchFound is false, add it as a new shape
    for ( int i = 0; i < mShapes.size(); i++ ) {
        if( mShapes[i].matchFound == false ){
            // assign an unique ID
            mShapes[i].ID = shapeUID;
            mShapes[i].lastFrameSeen = ci::app::getElapsedFrames();
            // add this new shape to tracked shapes
            mTrackedShapes.push_back( mShapes[i] );
            shapeUID++;
        }
    }
    
    // if we didn't find a match for x frames, delete the tracked shape
    for ( vector<Shape>::iterator it = mTrackedShapes.begin(); it != mTrackedShapes.end(); ) {
        if ( ci::app::getElapsedFrames() - it->lastFrameSeen > 20 ) {
            // remove the tracked shape
            it = mTrackedShapes.erase(it);
        } else {
            ++it;
        }
    }
}

void FrameSubtraction::onColor( openni::VideoFrameRef frame, const OpenNI::DeviceOptions& deviceOptions )
{
    mSurface = OpenNI::toSurface8u(frame);
    cv::Mat mInput( toOcv( OpenNI::toSurface8u(frame), 0 ) );
}

vector< Shape > FrameSubtraction::getEvaluationSet( ContourVector rawContours, int minimalArea, int maxArea )
{
    vector< Shape > vec;
    for ( vector< cv::Point > &c : rawContours ) {
        // create a matrix for the contour
        cv::Mat matrix = cv::Mat(c);
        
        // extract data from contour
        cv::Scalar center = mean(matrix);
        double area = cv::contourArea(matrix);
        
        // reject it if too small
        if ( area < minimalArea ) {
            continue;
        }
        
        // reject it if too big
        if ( area > maxArea ) {
            continue;
        }
        
        // store data
        Shape shape;
        shape.area = area;
        shape.centroid = cv::Point( center.val[0], center.val[1] );
        
        // get depth value from center point
        // map 10 4000 to 0 1
        float centerDepth = (float)mInput.at<short>( center.val[1], center.val[0] );
        shape.depth = lmap( centerDepth, (float)mNearLimit, (float)mFarLimit, 0.0f, 1.0f );
        
        // store points around shape
        shape.hull = c;
//        console() << "shape size " << shape.hull.size() << endl;
        shape.matchFound = false;
        vec.push_back(shape);
    }
    return vec;
}

Shape* FrameSubtraction::findNearestMatch( Shape trackedShape, vector< Shape > &shapes, float maximumDistance )
{
    Shape* closestShape = NULL;
    float nearestDist = 1e5;
    if ( shapes.empty() ) {
        return NULL;
    }
    
    for ( Shape &candidate : shapes ) {
        // find dist between the center of the contour and the shape
        cv::Point distPoint = trackedShape.centroid - candidate.centroid;
        float dist = cv::sqrt( distPoint.x * distPoint.x + distPoint.y * distPoint.y );
        if ( dist > maximumDistance ) {
            continue;
        }
        if ( candidate.matchFound ) {
            continue;
        }
        if ( dist < nearestDist ) {
            nearestDist = dist;
            closestShape = &candidate;
        }
    }
    return closestShape;
}

void FrameSubtraction::update()
{
}

cv::Mat FrameSubtraction::removeBlack( cv::Mat input, short nearLimit, short farLimit )
{
    for( int y = 0; y < input.rows; y++ ) {
        for( int x = 0; x < input.cols; x++ ) {
            if( input.at<short>(y,x) < nearLimit || input.at<short>(y,x) > farLimit ) {
                input.at<short>(y,x) = 4000;
            }
        }
    }
    return input;
}

void FrameSubtraction::shutdown(){
    // stop the camera after the app quits
    mDevice->stop();
}

void FrameSubtraction::draw()
{
    for( int i=0; i<mTrackedShapes.size(); i++){
        glBegin( GL_POINTS );
        for( int j=0; j<mTrackedShapes[i].hull.size(); j++ ){
            gl::color( Color( 0.0f, 1.0f, 0.0f ) );
            Vec2i v = fromOcv( mTrackedShapes[i].hull[j] );
            v.x *= ( getWindowHeight() / 240);
            v.y *= ( getWindowWidth() / 320 );
            gl::vertex( v );
        }
        glEnd();
    }
}