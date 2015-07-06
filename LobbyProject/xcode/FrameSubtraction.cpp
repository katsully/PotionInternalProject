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
    // setup camera
    mDeviceManager = OpenNI::DeviceManager::create();
    
    shapeUID = 0;
    mBlurAmount = 10;
    mTrackedShapes.clear();
    
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

    cv::Mat mSubtracted;
    cv::Mat blur;
    cv::Mat thresh;
    cv::Mat eightBit;
    
    // blur the image to reduce noise
    cv::blur( mInput, blur, cv::Size( mBlurAmount, mBlurAmount ) );
    // background subtraction
    cv::absdiff( mBackground, blur, mSubtracted );
    
    mSubtracted.convertTo( eightBit, CV_8UC1, 0.1/1.0 );
    
    mContours.clear();
    // using a threshold to reduce noise
    cv::threshold( eightBit, thresh, 75.0, 255.0, CV_8U );
    cv::findContours( thresh, mContours, mHierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE );
    
    // get data that we can later compare
    mShapes.clear();
    mShapes = getEvaluationSet( mContours, 75, 100000 );
    
    // find the nearest match for each shape
    for( int i=0; i<mTrackedShapes.size(); i++ ){
        Shape* nearestShape = findNearestMatch( mTrackedShapes[i], mShapes, 5000 );
        
        // a tracked shape was found, update that tracked shape with the new shape
        if( nearestShape != NULL ){
            // update our tracked contours
            // set last frame seen
            nearestShape->matchFound = true;
            mTrackedShapes[i].centroid = nearestShape->centroid;
            mTrackedShapes[i].lastFrameSeen = ci::app::getElapsedFrames();
            mTrackedShapes[i].hull.clear();
            mTrackedShapes[i].hull = nearestShape->hull;
        }
    }
    
    // if shape->matchFound is false, add it as a new shape
    for( int i = 0; i<mShapes.size(); i++ ){
        if( mShapes[i].matchFound == false ){
            // assign an unique ID
            mShapes[i].ID = shapeUID;
            mShapes[i].lastFrameSeen = ci::app::getElapsedFrames();
            mShapes[i].particleSystem = false;
            mTrackedShapes.push_back( mShapes[i] );
            shapeUID++;
            //std::cout << "adding a new tracked shape with ID: " << mShapes[i].ID << std::endl;
        }
    }
    
    // if we didn't find a match for x frames, delete the tracked shape
    for( vector<Shape>::iterator it=mTrackedShapes.begin(); it!=mTrackedShapes.end(); ){
        if( ci::app::getElapsedFrames() - it->lastFrameSeen > 20 ){
            it = mTrackedShapes.erase(it);
        } else {
            ++it;
        }
    }
    
    mInput.copyTo( mPreviousFrame );
}

void FrameSubtraction::onColor(openni::VideoFrameRef frame, const OpenNI::DeviceOptions& deviceOptions){
    mSurface = OpenNI::toSurface8u( frame );
    cv::Mat mInput( toOcv( OpenNI::toSurface8u( frame), 0 ) );
}

vector< Shape > FrameSubtraction::getEvaluationSet( vector< vector<cv::Point> > rawContours, int minimalArea, int maxArea ){
    vector< Shape > vec;
    for( vector< cv::Point > &c : rawContours ){
        // create a matrix for the contour
        cv::Mat matrix = cv::Mat( c );
        
        // extract data from contour
        cv::Scalar center = mean( matrix );
        double area = cv::contourArea( matrix );
        
        // reject it if too small
        if( area < minimalArea ){
            continue;
        }
        
        // reject it if too big
        if( area > maxArea ){
            continue;
        }
        
        // store data
        Shape shape;
        shape.area = area;
        shape.centroid = cv::Point( center.val[0], center.val[1] );
        
        // convex hull is the polygon enclosing the contour
        shape.hull = c;
        shape.matchFound = false;
        vec.push_back( shape );
    }
    return vec;
}

Shape* FrameSubtraction::findNearestMatch( Shape trackedShape, vector< Shape > &shapes, float maximumDistance ){
    Shape* closestShape = NULL;
    float nearestDist = 1e5;
    if( shapes.empty() ){
        return NULL;
    }
    
    for( Shape &candidate : shapes ){
        // find dist between the center of the contour and the shape
        cv::Point distPoint = trackedShape.centroid - candidate.centroid;
        float dist = cv::sqrt( distPoint.x*distPoint.x + distPoint.y*distPoint.y);
        if( dist > maximumDistance ){
            continue;
        }
        if( candidate.matchFound ){
            continue;
        }
        if( dist < nearestDist ){
            nearestDist = dist;
            closestShape = &candidate;
        }
    }
    return closestShape;
}

void FrameSubtraction::update(){
    
}

void FrameSubtraction::keyDown( KeyEvent event ){
    mPreviousFrame.copyTo( mBackground );
}

void FrameSubtraction::draw()
{
    
}