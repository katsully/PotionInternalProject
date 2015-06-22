//
//  Particle.cpp
//  FrameSubtraction
//
//  Created by Kathleen Sullivan on 6/18/15.
//
//

#include "Particle.h"


Particle::Particle( Vec2f pos ){
    mPos = pos;
    //  mVel = vel;
    mRadius = 3;
}

void Particle::draw(){
    // gl::setViewport( ci::app::getWindowBounds() );
    gl::color(1, 1, 1);
    gl::drawSolidCircle( mPos, mRadius, 16);
}