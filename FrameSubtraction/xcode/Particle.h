//
//  Particle.h
//  FrameSubtraction
//
//  Created by Kathleen Sullivan on 6/18/15.
//
//

#pragma once

using namespace ci;

class Particle {
public:
    Particle();
    Particle( Vec2f pos );
    void draw();
    
    Vec2f mPos;
    Vec2f mVel;
    float mRadius;
};
