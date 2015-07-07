//
//  ParticleControllerController.cpp
//  LobbyProject
//
//  Created by Kat Sullivan on 7/2/15.
//
//  This class maintains a list of particle systems
//

#include "ParticleControllerController.h"

// CURRENTLY NOT USING THIS CLASS

ParticleControllerController::ParticleControllerController(){
    
}

void ParticleControllerController::update( std::vector< Shape > trackedShapes ){
    for( int i = 0; i < trackedShapes.size(); i++ ){
        // if there is a new shape, give it a particle system
//        if( trackedShapes[i].particleSystem == false ){
//            //std::cout << "new shape " << trackedShapes[i].ID << std::endl;
//            ParticleController particleController;
//            particleController.centroid = trackedShapes[i].centroid;
//            particleController.particleSystemUID = trackedShapes[i].ID;
//            particleController.generateSystem( trackedShapes[i] );
//            mParticleControllers.push_back( particleController );
//       // if the particle system already exists, update its location
//        } else {
//           // std::cout << "exisiting shape beginning " << trackedShapes[i].ID << std::endl;
//            for( std::list <ParticleController>::iterator pc = mParticleControllers.begin(); pc != mParticleControllers.end(); ++pc ){
//                if( pc->particleSystemUID == trackedShapes[i].ID ){
//                    cv::Point distPoint = pc->centroid - trackedShapes[i].centroid;
//                    // update the centroid
//                    pc->centroid = trackedShapes[i].centroid;
//                    // update the exterior points, and add or delete particles as the hull gets updated
//                    std::cout << "hull points " << trackedShapes[i].hull.size()   << std::endl;
//                    std::cout << "particle count " << pc->mParticles.size()   << std::endl;
//                    // invert the distance point aka [-1, 5] become [1, -5]
//                    distPoint *= -1;
//                    pc->update( distPoint, trackedShapes[i].hull );
//                    break;
//                }
//            }
//        }
    }
    
    // remove any particle systems whose shapes no longer exist
    for( std::list <ParticleController>::iterator pc = mParticleControllers.begin(); pc != mParticleControllers.end(); ){
        Boolean keep = false;
        for( int i=0; i<trackedShapes.size(); i++ ){
            if( pc->particleSystemUID == trackedShapes[i].ID ){
                keep = true;
            }
        }
        if( !keep ){
            pc = mParticleControllers.erase( pc );
        } else {
            ++pc;
        }
    }
    
    
}

void ParticleControllerController::draw(){
    // draw each Particle Controller
    for( std::list <ParticleController>::iterator pc = mParticleControllers.begin(); pc != mParticleControllers.end(); ++pc ){
        pc->draw();
    }
}