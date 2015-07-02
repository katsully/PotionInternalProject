//
//  ParticleControllerController.h
//  LobbyProject
//
//  Created by Kathleen Sullivan on 6/18/15.
//
//

#pragma once
#include "ParticleController.h"
#include "Shape.h"

class ParticleControllerController {
public:
    ParticleControllerController();
    void update( std::vector< Shape > shapes );
    void draw();
    
    std::list< ParticleController > mParticleControllers;
};
