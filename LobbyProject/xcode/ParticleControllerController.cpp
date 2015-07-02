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

void ParticleControllerController::update( std::vector< Shape > trackedShapes ){
    for( int i = 0; i < trackedShapes.size(); i++ ){
        // if there is a new shape, give it a particle system
        if( trackedShapes[i].particleSystem == false ){
            std::cout << "new shape " << trackedShapes[i].ID << std::endl;
            ParticleController particleController;
            particleController.centroid = trackedShapes[i].centroid;
            particleController.particleSystemUID = trackedShapes[i].ID;
            particleController.generateSystem( trackedShapes[i] );
            mParticleControllers.push_back( particleController );
            trackedShapes[i].particleSystem = true;
       // if the particle system already exists, update its location
        } else {
            std::cout << "exisiting shape beginning " << trackedShapes[i].ID << std::endl;
            for( std::list <ParticleController>::iterator pc = mParticleControllers.begin(); pc != mParticleControllers.end(); ++pc ){
                if( pc->particleSystemUID == trackedShapes[i].ID ){
                    std::cout << "exisiting shape " << trackedShapes[i].ID << std::endl;
                    cv::Point distPoint = pc->centroid - trackedShapes[i].centroid;
                    pc->update( distPoint );
                }
            }
        }
    }
    
    // remove any particle systems whose shapes no longer exist
    for( std::list <ParticleController>::iterator pc = mParticleControllers.begin(); pc != mParticleControllers.end(); ++pc ){
        Boolean keep = false;
        for( int i=0; i<trackedShapes.size(); i++ ){
            if( pc->particleSystemUID == trackedShapes[i].ID ){
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