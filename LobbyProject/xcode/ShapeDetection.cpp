//
//  ShapeDetection.cpp
//  LobbyProject
//
//  Created by Kat Sullivan on 6/22/15.
//
//

#include "ShapeDetection.h"

ShapeDetection::ShapeDetection()
{
}

void ShapeDetection::setup( Json::Value data )
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
            mCameraPresent = false;
            return;
        }
        
        if (mDevice) {
            mDevice->connectDepthEventHandler( &ShapeDetection::onDepth, this);
            mDevice->connectColorEventHandler( &ShapeDetection::onColor, this );
            mDevice->start();
            mCameraPresent = true;
        }
    }
    
    // start off drawing points, not shapes
    mDrawShapes = false;
    
    // sets threshold to ignore all black pixels and pixels that are far away from the camera
    // read data from json file
    mNearLimit = data.get("mNearLimit", 0).asInt();
    mFarLimit = data.get("mFarLimit", 0).asInt();
    mThresh = data.get("mThresh", 0.0).asFloat();
    mMaxVal = data.get("mMaxVal", 0.0).asFloat();
}

void ShapeDetection::onDepth( openni::VideoFrameRef frame, const OpenNI::DeviceOptions& deviceOptions )
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
            // get depth value from center point
            float centerDepth = (float)mInput.at<short>( mTrackedShapes[i].centroid.y, mTrackedShapes[i].centroid.x );
            // map 10 4000 to 0 1
            mTrackedShapes[i].depth = lmap( centerDepth, (float)mNearLimit, (float)mFarLimit, 0.0f, 1.0f );
            mTrackedShapes[i].lastFrameSeen = ci::app::getElapsedFrames();
            mTrackedShapes[i].hull.clear();
            mTrackedShapes[i].hull = nearestShape->hull;
            mTrackedShapes[i].moving = nearestShape->moving;
            mTrackedShapes[i].motion = nearestShape->motion;
        }
    }
    
    // if shape->matchFound is false, add it as a new shape
    for ( int i = 0; i < mShapes.size(); i++ ) {
        if( mShapes[i].matchFound == false ){
            // assign an unique ID
            mShapes[i].ID = shapeUID;
            mShapes[i].lastFrameSeen = ci::app::getElapsedFrames();
            mShapes[i].moving = true;
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
    mSurfaceDepth = Surface8u( fromOcv( mInput  ) );
    mSurfaceSubtract = Surface8u( fromOcv(eightBit) );
}

void ShapeDetection::onColor( openni::VideoFrameRef frame, const OpenNI::DeviceOptions& deviceOptions )
{
    mSurface = OpenNI::toSurface8u(frame);
    cv::Mat mInput( toOcv( OpenNI::toSurface8u(frame), 0 ) );
}

vector< Shape > ShapeDetection::getEvaluationSet( ContourVector rawContours, int minimalArea, int maxArea )
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
        float centerDepth = (float)mInput.at<short>( shape.centroid.y, shape.centroid.x );
        // map 10 4000 to 0 1
        shape.depth = lmap( centerDepth, (float)mNearLimit, (float)mFarLimit, 0.0f, 1.0f );
        
        // store points around shape
        shape.hull = c;
        shape.matchFound = false;
        vec.push_back(shape);
    }
    return vec;
}

Shape* ShapeDetection::findNearestMatch( Shape trackedShape, vector< Shape > &shapes, float maximumDistance )
{
    Shape* closestShape = NULL;
    float nearestDist = 1e5;
    if ( shapes.empty() ) {
        return NULL;
    }
    // finalDist keeps track of the distance between the trackedShape and the chosen candidate
    float finalDist;
    
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
            finalDist = dist;
        }
    }

    // if a candidate was matched to the tracked shape
    if(closestShape){
        // if the shape isn't moving
        if ( finalDist < 1.5 ) {
            // 'dilute' motion
            closestShape->motion = trackedShape.motion * .995;
            // if diluted motion is under the threshold or it was already not moving, the object is not moving
            if ( closestShape->motion < 1.5 || trackedShape.moving == false ) {
                closestShape->moving = false;
                closestShape->motion = 0;
            }
        } else if ( finalDist > 19 || trackedShape.moving == true ) {
                closestShape->moving = true;
                closestShape->motion = finalDist;
        }
    }
    return closestShape;
}

void ShapeDetection::update()
{
}

cv::Mat ShapeDetection::removeBlack( cv::Mat input, short nearLimit, short farLimit )
{
    for( int y = 0; y < input.rows; y++ ) {
        for( int x = 0; x < input.cols; x++ ) {
            // if a shape is too close or too far away, set the depth to a fixed number
            if( input.at<short>(y,x) < nearLimit || input.at<short>(y,x) > farLimit ) {
                input.at<short>(y,x) = farLimit;
            }
        }
    }
    return input;
}

void ShapeDetection::shutdown(){
    // stop the camera after the app quits, if the camera is connected
    if(mCameraPresent){
        mDevice->stop();
    }
}

void ShapeDetection::drawSurface(){
    glMatrixMode(GL_TEXTURE);
    gl::pushMatrices();
    gl::scale(.02, .02);
    gl::translate(-1,-17,-.1);
    if( mSurfaceSubtract ){
        if( mTextureDepth ){
            mTextureDepth->update( Channel32f( mSurfaceSubtract ) );
        } else {
            mTextureDepth = gl::Texture::create( Channel32f( mSurfaceSubtract ) );
        }
        gl::draw( mTextureDepth, Area(-1.55, -1.5, 20, 20)) ;
    }
    gl::popMatrices();
}

void ShapeDetection::draw()
{
    // draw points
    for( int i=0; i<mTrackedShapes.size(); i++){
        if(mDrawShapes){
            glBegin( GL_POLYGON );
        } else{
            glBegin(GL_POINTS);
        }
        for( int j=0; j<mTrackedShapes[i].hull.size(); j++ ){
            if(mTrackedShapes[i].moving){
                gl::color( Color( 0.0f, 1.0f, 0.0f ) );
                Vec2i v = fromOcv( mTrackedShapes[i].hull[j] );
                // offset the points to align with the camera used for the mesh
                Vec3f pos = Vec3f( v.x / 300.f - 0.55f, v.y / 250.f - 0.5f, -.1 );
                gl::vertex( pos );
            }
        }
        glEnd();
    }
}