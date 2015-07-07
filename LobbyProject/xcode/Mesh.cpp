//
//  Mesh.cpp
//  MeshController
//
//  Created by Luobin Wang on 6/23/15.
//
//

#include "Mesh.h"
#include "FrameSubtraction.h"



Mesh::Mesh(int vertices_x, int vertices_y, int meshType, bool &_isFirstMesh){
    this->VERTICES_X = vertices_x;
    this->VERTICES_Y = vertices_y;
    
    zPct = 0.f;
    zPctStart = 0.f;
    currIter = 0.f;
    totalIter = 100.f;
    currIterStart = 0.f;
    totalIterStart = 100.f;
    stateStable = true;
    
    int totalVertices = VERTICES_X * VERTICES_Y;
    int totalQuads = ( VERTICES_X - 1 ) * ( VERTICES_Y - 1 );
    gl::VboMesh::Layout layout;
    layout.setStaticIndices();
    layout.setDynamicPositions();
    layout.setStaticTexCoords2d();
    layout.setDynamicColorsRGBA();
    
    
    //-----> determine mesh type
    if (meshType == 0) {
        mVboMesh = gl::VboMesh::create(totalVertices, totalQuads*4, layout, GL_QUADS);
    }else if (meshType == 1){
        mVboMesh = gl::VboMesh::create(totalVertices, totalQuads*4, layout, GL_POINTS);
    }else{
        mVboMesh = gl::VboMesh::create(totalVertices, totalQuads*4, layout, GL_QUADS);
    }
    
}






float Mesh::easeIn(float t, float b , float c, float d){
    return c*((t=t/d-1)*t*t + 1) + b;
}

void Mesh::getTrackedShapes(vector<Shape> &_mTrackedShapes){
    this->mTrackedShapes = _mTrackedShapes;
    shapePos.clear();
    if (mTrackedShapes.size() > 0) {
        for (int i = 0; i < mTrackedShapes.size() - 1; i ++) {
            shapePos.push_back(Vec2f(lmap<float>(mTrackedShapes[i].centroid.x, 0, 320, 0, getWindowWidth()), lmap<float>(mTrackedShapes[i].centroid.y, 0, 240, 0, getWindowHeight())));
        }
    }
    


}


//void Mesh::getParticle(std::list<Particle> &_mParticles){
//    this->mParticles = _mParticles;
//    particlePos.clear();
//    particleRad.clear();
//    //-----> copy particle list from main cpp
//    for (list <Particle>::iterator p = mParticles.begin() ; p!= mParticles.end(); p++) {
//        particlePos.push_back(p->mPos);
//        particleRad.push_back(p->mRadius);
//        
//    }
//
//    
//    
//}


void Mesh::update(Vec2f &_mousePos, gl::Texture &texture,  bool &_mouseClick){
    
    float time = getElapsedSeconds();
    if (shapePos.size() > 0){
        this->mousePos = shapePos[0];
    }
    bool mouseClick = _mouseClick;
    
    
    this->mTexture = texture;
    vector<uint32_t> indices;
    vector<Vec2f> texCoords;
    zPct = easeIn(currIter, 0.0, 1.0, totalIter);
    zPctStart = lmap<float>(easeIn(currIterStart, 0.0, 1.0f, totalIterStart), 0.f, 1.f, 1.f, 0);
    
 
    
    if (mouseClick && stateStable) {
        stateFly = true;
        stateStable = false;
        std::cout<<"fly and reset"<<std::endl;
    } else if (mouseClick && stateFly){
        stateStart = true;
        stateFly = false;
        std::cout<<"start"<<std::endl;
    } else if (mouseClick && stateStart){
        stateStable = true;
        stateStart  = false;
        std::cout<<"stable"<<std::endl;
    }
    
    float zPctReverse = lmap<float>(zPct, 0.0, 1.0, 1.0, 0);
    float zPctStarting = lmap<float>(zPct, 0.0, 1.0, -1.0, 0);
   
    if (shapePos.size() > 0) {
        for (int i = 0 ; i < shapePos.size(); i ++) {
            if (timeDiffP.size() != shapePos.size()) {
                timeDiffP.push_back(0);
            }
            if (isTargetP.size() != shapePos.size()) {
                isTargetP.push_back(false);
            }
        }
    }
    
    
    if (mTexture){
     
        //std::cout<<mTexture.getTarget()<<std::endl;
        
    }
    
   
   
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
            //-----> mapping texture (0 to texture size) video target is 34037, pic tartget is 3553;
            
            if (mTexture && mTexture.getTarget() == 34037) {
                
                texCoords.push_back(Vec2f( mTexture.getWidth() * x / (float)VERTICES_X, mTexture.getHeight() * y / (float)VERTICES_Y ));
                //std::cout<<mTexture.getWidth()<<std::endl;
            }else{
                
                texCoords.push_back(Vec2f( x / (float)VERTICES_X, y / (float)VERTICES_Y ));
            }
            
        }
    }
   
    
    mVboMesh->bufferIndices(indices);
    mVboMesh->bufferTexCoords2d(0, texCoords);
    

   
    
    
    
    
    //---generate movements
    gl::VboMesh::VertexIter iter = mVboMesh->mapVertexBuffer();
    for( int x = 0; x < VERTICES_X; ++x ) {
        for( int y = 0; y < VERTICES_Y; ++y ) {
            Vec3f position = Vec3f(Vec3f( x / (float)VERTICES_X, y / (float)VERTICES_Y, 0.f));
           
            
//            // mesh influence from particles
//            for (int i = 0 ; i < particlePos.size() - 1; i ++) {
//                Vec2f diff = Vec2f((particlePos[i].x * 1.8 - position.x), (particlePos[i].y * 2.f - position.y));
//                //std::cout<<diff.length()<<std::endl;
//                float particleInfluence = particleRad[i] * diff.lengthSquared();
//                
//                if (diff.lengthSquared() < 0.5f &&particleRad[i] != 0) {
//                    timeDiffP[i] = time;
//                }
//                
//                if (time - timeDiffP[i] < 3.f && diff.length() < 0.5f) {
//                    position.z +=  timeDiffP[i] / time * 0.001f;
//                }
//               
//                if (diff.length() < 0.1f && particleInfluence != 0.f) {
//                    position.z += particleInfluence * 10.f;
//                }
//
//                
//            }
            Vec2f relPos = Vec2f(lmap<float>(position.x, 0, 1, 0, getWindowWidth()), lmap<float>(position.y, 0, 1, 0, getWindowHeight()));

            if (shapePos.size() > 0) {
                
                for (int i = 0; i < shapePos.size(); i ++) {
                    Vec2f diff = Vec2f((shapePos[i].x - relPos.x) * 0.001f, (shapePos[i].y - relPos.y)  * 0.001f);
                    float shapeInfluence = diff.lengthSquared();
                    //std::cout<<shapeInfluence<<std::endl;
                    if (shapeInfluence < 0.005f) {
                        position.z -= shapeInfluence * 10.f;
                        timeDiff[i] = time;
                    }
                
                    if (time - timeDiffP[i] < 5.f  ) {
                        position.z -= timeDiffP[i] / time * 0.1f;
                    }
                
                }
            }
            
 
            //---->calculate relative location to mouse
            
            
            
            
            
                        Vec2f dist = Vec2f(std::abs(relPos.x - mousePos.x)/1000, std::abs(relPos.y - mousePos.y )/1000);
            
          
            
            
            if (dist.lengthSquared() < 0.001f) {
               // position.z -= lmap<float>(dist.lengthSquared(), 0, 1, 0, 0);
            }
            
            
            if (dist.lengthSquared() < 0.001f) {
                //position.z -= 0.0001f;
                isTarget[x * VERTICES_Y + y] = true;
                timeDiff[x * VERTICES_Y + y] = time;
            }
            
            //--->timer affecting mesh
            if ( (time - timeDiff[x * VERTICES_Y + y] ) < 2.f && isTarget[x * VERTICES_Y + y] == true) {
                //position.z -= 0.0001f +  timeDiff[x * VERTICES_Y + y ] /time * 0.1f;
                ////position.z -= 0.0001f + 1/  (time - timeDiff[x * VERTICES_Y + y ] ) * 0.1;
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
            //position -= Vec3f(0.48 + noise * 0.05, 0.48 + noise2 * 0.05, 0.f);
            
            if (stateStable) {
                currIterStart = 0.f;
                position -= Vec3f(0.45f + noise * 0.05f, 0.45f + noise2 * 0.05f,  0.f);
                iter.setColorRGBA(ColorA(1.f, 1.f, 1.f, 1.f));
            }else if (stateFly) {
                
                if(zPct == 1.f){
                    position -= Vec3f(0.45f + noise * 0.05f, 0.45f + noise2 * 0.05f,  1.f);
                }else{
                    position -= Vec3f(0.45f + noise * 0.05f, 0.45f + noise2 * 0.05f,  -1.f * zPct);
                    iter.setColorRGBA(ColorA(1.f, 1.f, 1.f, zPctReverse));
                }
            }else if (stateStart) {
                currIter = 0.f;
                position -= Vec3f(0.45f + noise * 0.05f, 0.45f + noise2 * 0.05f,  zPctStart);
                iter.setColorRGBA(ColorA(lmap<float>(zPctStart, 1.0f, 0, 0, 1.0f), lmap<float>(zPctStart, 1.0f, 0, 0, 1.0f), lmap<float>(zPctStart, 1.0f, 0, 0, 1.0f), 1.f));
            }else{
                
                position -= Vec3f(0.45f + noise * 0.05f, 0.45f + noise2 * 0.05f,  0.f);
            }
            
            
            iter.setPosition(position);
            //iter.setColorRGBA(ColorA(1.f, 1.f, 1.f, 1.f));

           
            ++iter;
       }
    }
    
  
    if (stateFly) {
        if (currIter< totalIter) {
            currIter ++;
        }
    }
    
    if (stateStart) {
        if (currIterStart< totalIterStart) {
            currIterStart ++;
        }
    }
    
    
    


}

void Mesh::draw(){
    glEnable(GL_BLEND);
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    if(mTexture){
    mTexture.enableAndBind();
    }
    gl::draw(mVboMesh);
    if(mTexture){
    mTexture.unbind();
    mTexture.disable();
    }
    ;
}
