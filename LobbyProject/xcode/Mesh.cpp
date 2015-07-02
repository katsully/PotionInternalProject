//
//  Mesh.cpp
//  MeshController
//
//  Created by Luobin Wang on 6/23/15.
//
//

#include "Mesh.h"
#include "FrameSubtraction.h"



Mesh::Mesh(int vertices_x, int vertices_y, int meshType){
    this->VERTICES_X = vertices_x;
    this->VERTICES_Y = vertices_y;
    
    zPct = 0.f;
    currIter = 0.f;
    totalIter = 100.f;
    
    int totalVertices = VERTICES_X * VERTICES_Y;
    int totalQuads = ( VERTICES_X - 1 ) * ( VERTICES_Y - 1 );
    gl::VboMesh::Layout layout;
    layout.setStaticIndices();
    layout.setDynamicPositions();
    layout.setStaticTexCoords2d();
    
    
    //-----> determine mesh type
    if (meshType == 0) {
        mVboMesh = gl::VboMesh::create(totalVertices, totalQuads*4, layout, GL_QUADS);
    }else if (meshType == 1){
        mVboMesh = gl::VboMesh::create(totalVertices, totalQuads*4, layout, GL_POINTS);
    }else{
        mVboMesh = gl::VboMesh::create(totalVertices, totalQuads*4, layout, GL_QUADS);
    }
    
}






float Mesh::easeIn(float t,float b , float c, float d){
    return c*((t=t/d-1)*t*t + 1) + b;
}

void Mesh::getParticle(std::list<Particle> &_mParticles){
    this->mParticles = _mParticles;
    particlePos.clear();
    particleRad.clear();
    //-----> copy particle list from main cpp
    for (list <Particle>::iterator p = mParticles.begin() ; p!= mParticles.end(); p++) {
        particlePos.push_back(p->mPos);
        particleRad.push_back(p->mRadius);
        
    }

    
    
}


void Mesh::update(Vec2f &_mousePos, gl::Texture &texture, bool &flyAway){
    
    
    vector<uint32_t> indices;
    vector<Vec2f> texCoords;
    zPct = easeIn(currIter, 0.0, 1.0, totalIter);
   
    
    for (int i = 0 ; i < particlePos.size() - 1; i ++) {
        if (timeDiffP.size() != particlePos.size()) {
            timeDiffP.push_back(0);
        }
        if (isTargetP.size() != particlePos.size()) {
            isTargetP.push_back(false);
        }
        
    }
    
    
    
   
//    isTargetP[0] = true;
    
    //-----> buffer texCoords and indices
    for (int x = 0; x < VERTICES_X; x++) {
        for (int y = 0; y < VERTICES_Y; y++) {
            
            timeDiff.push_back(0);
            isTarget.push_back(false);
            
            
            //-----> creating quads
            if (( x + 1 < VERTICES_X ) && ( y + 1 < VERTICES_Y )) {
                indices.push_back( (x+0) * VERTICES_Y + (y+0) );
                indices.push_back( (x+1) * VERTICES_Y + (y+0) );
                indices.push_back( (x+1) * VERTICES_Y + (y+1) );
                indices.push_back( (x+0) * VERTICES_Y + (y+1) );
            }
            //-----> mapping texture (0 to texture size)
            if (mTexture) {
                texCoords.push_back(Vec2f( mTexture.getWidth() * x / (float)VERTICES_X, mTexture.getHeight() * y / (float)VERTICES_Y ));
                //std::cout<<mTexture.getWidth()<<std::endl;
            }else{
                texCoords.push_back(Vec2f( x / (float)VERTICES_X, y / (float)VERTICES_Y ));
            }
            
        }
    }
    
    mVboMesh->bufferIndices(indices);
    mVboMesh->bufferTexCoords2d(0, texCoords);
    

    float time = getElapsedSeconds();
    this->mousePos = _mousePos;
    this->mTexture = texture;
    
    
    //---generate movements
    gl::VboMesh::VertexIter iter = mVboMesh->mapVertexBuffer();
    for( int x = 0; x < VERTICES_X; ++x ) {
        for( int y = 0; y < VERTICES_Y; ++y ) {
            Vec3f position = Vec3f(Vec3f( x / (float)VERTICES_X, y / (float)VERTICES_Y, 0.f));
            Vec2f pPos;
            
            // mesh influence from particles
            for (int i = 0 ; i < particlePos.size() - 1; i ++) {
                Vec2f diff = Vec2f((particlePos[i].x * 1.8 - position.x), (particlePos[i].y * 2.f - position.y));
                //std::cout<<diff.length()<<std::endl;
                float particleInfluence = particleRad[i] * diff.lengthSquared();
                
                if (diff.lengthSquared() < 0.5f &&particleRad[i] != 0) {
                    timeDiffP[i] = time;
                }
                
                if (time - timeDiffP[i] < 3.f && diff.length() < 0.5f) {
                    position.z +=  timeDiffP[i] / time * 0.001f;
                }
               
                if (diff.length() < 0.1f && particleInfluence != 0.f) {
                    position.z += particleInfluence * 10.f;
                }

                
            }
  
            //---->calculate relative location to mouse
            Vec2f relPos = Vec2f(lmap<float>(position.x, 0, 1, 0, getWindowWidth()), lmap<float>(position.y, 0, 1, 0, getWindowHeight()));
            Vec2f dist = Vec2f(std::abs(relPos.x - mousePos.x)/1000, std::abs(relPos.y - mousePos.y)/1000);
            if (dist.lengthSquared() < 0.001f) {
                position.z -= lmap<float>(dist.lengthSquared(), 0, 1, 0, 0);
            }
            
            
            if (dist.lengthSquared() < 0.001f) {
                position.z -= 0.0001f;
                isTarget[x * VERTICES_Y + y] = true;
                timeDiff[x * VERTICES_Y + y] = time;
            }
            
            //--->timer affecting mesh
            if ( (time - timeDiff[x * VERTICES_Y + y] ) < 2.f && isTarget[x * VERTICES_Y + y] == true) {
                position.z -= 0.0001f +  timeDiff[x * VERTICES_Y + y ] /time * 0.1f;
                //position.z -= 0.0001f + 1/  (time - timeDiff[x * VERTICES_Y + y ] ) * 0.1;
            }else if ((time - timeDiff[x * VERTICES_Y + y]) >= 10.f){
                isTarget[x * VERTICES_Y + y] = false;
            }
            
            // ----generating Perlin Noise
            float nX = relPos.x * 0.0001f;
            float nY = relPos.y * 0.0001f;
            float nZ = app::getElapsedSeconds() * 0.1f;
            float nZ2 = app::getElapsedSeconds() * 0.1f + 500.f;
            Vec3f v( nX, nY, nZ );
            Vec3f v2( nX, nY, nZ2 );
            float noise = mPerlin.fBm(v);
            float noise2 = mPerlin.fBm(v2);
            
            
                        
            // ----> position adjustment
            
            //std::cout << noise3<< std::endl;
            float a = sin(getElapsedSeconds());
            position -= Vec3f(0.48 + noise * 0.05, 0.48 + noise2 * 0.05,  -1.f * zPct);
            
            iter.setPosition(position);
            ++iter;
       }
    }
    if (flyAway) {
        if (currIter< totalIter) {
            currIter ++;
        }
    }
   // std::cout<<zPct<<std::endl;

}

void Mesh::draw(){
    if(mTexture){
    mTexture.enableAndBind();
    }
    gl::draw(mVboMesh);
    if(mTexture){
        mTexture.unbind();
    }
    ;
}
