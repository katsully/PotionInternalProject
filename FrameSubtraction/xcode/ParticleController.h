//
//  ParticleController.h
//  FrameSubtraction
//
//  Created by Kathleen Sullivan on 6/18/15.
//
//

#pragma once
#include "Particle.h"

class ParticleController {
public:
    ParticleController();
    void addParticles (ci::Surface8u surface );
    void draw();
    
    std::list<Particle> mParticles;
};
