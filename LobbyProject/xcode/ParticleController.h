//
//  ParticleController.h
//  FrameSubtraction
//
//  Created by Kathleen Sullivan on 6/18/15.
//
//

#pragma once
#include "Particle.h"
#include "Shape.h"

class ParticleController {
public:
    ParticleController();
    void generateSystem ( Shape shape );
    void update( cv::Point point );
    void draw();
    
    float radius;
    cv::Point centroid;
    int particleSystemUID;
    std::list< Particle > mParticles;
};