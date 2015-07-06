//
//  ParticleController.cpp
//  FrameSubtraction
//
//  Created by Kathleen Sullivan on 6/18/15.
//
//  This class represents a particle system
//

#include "ParticleController.h"
#include "cinder/Rand.h"

using namespace ci;
using std::list;

ParticleController::ParticleController(){
    
}

void ParticleController::generateSystem( Shape shape ){
    mParticles.clear();

    for( int i=0; i<shape.hull.size(); i++ ){
        Vec2f newPos = Vec2f( shape.hull[i].x, shape.hull[i].y );
        mParticles.push_back( Particle( newPos, 5 ) );
    }
    Vec2f newPos = Vec2f( shape.centroid.x, shape.centroid.y );
    mParticles.push_back( Particle( newPos, 5 ) );
}

void ParticleController::update( cv::Point distPoint, cv::vector<cv::Point> hullPoints ){
    for( int i=0; i<mParticles.size(); i++ ){
        Vec2f newPos = Vec2f( distPoint.x+mParticles[i].mPos.x, distPoint.y + mParticles[i].mPos.y );
//        std::cout << "old pos: " << p->mPos << std::endl;
//        std::cout << "point " << distPoint << std::endl;
//        std::cout << "new pos " << newPos << std::endl;
        mParticles[i].mPos = newPos;
    }
    if( mParticles.size() < hullPoints.size() ){
        for( int j = hullPoints.size() - mParticles.size(); j < hullPoints.size(); j++ ){
            Vec2f newPos = Vec2f( hullPoints[j].x, hullPoints[j].y );
            mParticles.push_back( Particle( newPos, 5 ) );

        }
    }
}

void ParticleController::draw(){
    gl::setViewport(ci::app::getWindowBounds() );
    for( int i=0; i<mParticles.size(); i++ ){
        mParticles[i].draw();
    }
}