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
    for( int i=0; i<shape.hull.size(); i++ ){
        Vec2f newPos = Vec2f( shape.hull[i].x * 3.33, shape.hull[i].y * 1.875 );
        mParticles.push_back( Particle( newPos, 5 ) );
    }
    Vec2f newPos = Vec2f( shape.centroid.x * 3.33, shape.centroid.y * 1.875 );
    mParticles.push_back( Particle( newPos, 5 ) );

}

void ParticleController::update( cv::Point point ){
    for( list <Particle>::iterator p = mParticles.begin(); p != mParticles.end(); ++p ){
        Vec2f newPos = Vec2f( point.x+p->mPos.x, point.y+p->mPos.y );
        p->mPos = newPos;
    }
}

void ParticleController::draw(){
    gl::setViewport(ci::app::getWindowBounds() );
    for( list <Particle>::iterator p = mParticles.begin(); p != mParticles.end(); ++p){
        p->draw();
    }
}