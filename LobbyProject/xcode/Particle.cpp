//
//  Particle.cpp
//  LobbyProject
//
//  Created by Kathleen Sullivan on 6/18/15.
//
//

#include "Particle.h"


Particle::Particle( Vec2f pos , float _mRadius){
    mPos = pos;
    //  mVel = vel;
    //mRadius = 0.005;
    mRadius = _mRadius;
}

void Particle::draw(){
    // gl::setViewport( ci::app::getWindowBounds() );
    gl::color(1, 1, 1);
    std::cout<<mRadius<<std::endl;
    gl::drawSolidCircle( mPos, mRadius, 16);
}