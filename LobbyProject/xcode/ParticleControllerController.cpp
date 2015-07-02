//
//  ParticleControllerController.cpp
//  LobbyProject
//
//  Created by Kathleen Sullivan on 7/2/15.
//
//  This class maintains a list of particle systems
//

#include "ParticleControllerController.h"

ParticleControllerController::ParticleControllerController(){
    
}

void ParticleControllerController::update( std::vector< Shape > shapes ){
    for( int i = 0; i < shapes.size(); i++ ){
        // if there is a new shape, give it a particle system
        std::cout << "has particle system " << shapes[i].particleSystem << std::endl;
        if( shapes[i].particleSystem == false ){
            std::cout << "new shape " << shapes[i].ID << std::endl;
            ParticleController particleController;
            particleController.centroid = shapes[i].centroid;
            particleController.particleSystemUID = shapes[i].ID;
            particleController.generateSystem( shapes[i] );
            mParticleControllers.push_back( particleController );
            shapes[i].particleSystem = true;
       // if the particle system already exists, update its location
        } else {
            for( std::list <ParticleController>::iterator pc = mParticleControllers.begin(); pc != mParticleControllers.end(); ++pc ){
                if( pc->particleSystemUID == shapes[i].ID ){
                    cv::Point distPoint = pc->centroid - shapes[i].centroid;
                    pc->update( distPoint );
                }
            }
        }
    }
    
    // remove any particle systems whose shapes no longer exist
    for( std::list <ParticleController>::iterator pc = mParticleControllers.begin(); pc != mParticleControllers.end(); ++pc ){
        Boolean keep = false;
        for( int i=0; i<shapes.size(); i++ ){
            if( pc->particleSystemUID == shapes[i].ID ){
                keep = true;
            }
        }
        if( !keep ){
            pc = mParticleControllers.erase( pc );
        }
    }
}

void ParticleControllerController::draw(){
    // draw each Particle Controller
    for( std::list <ParticleController>::iterator pc = mParticleControllers.begin(); pc != mParticleControllers.end(); ++pc ){
        pc->draw();
    }
}