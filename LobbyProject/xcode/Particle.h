//
//  Particle.h
//  LobbyProject
//
//  Created by Kathleen Sullivan on 6/18/15.
//
//

#pragma once

using namespace ci;

class Particle {
public:
    Particle();
    Particle( Vec2f pos , float _mRadius);
    void draw();
    
    Vec2f mPos;
    Vec2f mVel;
    float mRadius;
};