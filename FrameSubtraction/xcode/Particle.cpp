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
    mRadius = 5;
}

void Particle::draw(){
    gl::color(0, 0, 1);
    gl::drawSolidCircle( mPos, mRadius, 16);
}