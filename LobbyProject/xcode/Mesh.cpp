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
    this->isFirstMesh = _isFirstMesh;
    
    zPct = 0.f;
    zPctStart = 0.f;
    currIter = 0.f;
    totalIter = 100.f;
    currIterStart = 0.f;
    totalIterStart = 100.f;
    xOffset = 0.48f;
    yOffset = 0.43f;
   
    
    if (isFirstMesh == true) {
        stateStable = true;
        stateStart = false;
        stateFly = false;
    }else{
        stateFly  = true;
        stateStable = false;
        stateStart = false;
    }
    
    
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
    } else if (mouseClick && stateFly && zPct == 1.f){
        stateStart = true;
        stateFly = false;
        std::cout<<"start"<<std::endl;
    } else if (zPctStart == 0.f && stateStart){
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

   
    //-----> buffer texCoords and indices
    for (int x = 0; x < VERTICES_X; x++) {
        for (int y = 0; y < VERTICES_Y; y++) {
            
            timeDiff.push_back(0);
            isTarget.push_back(false);
            currIterBounce.push_back(0);
            totalIterBounce.push_back(100.f);
            zPctBounce.push_back(0);
            
            
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
           
            
            // ----->  mapping mesh pos to relative pos to the window
            Vec2f relPos = Vec2f(lmap<float>(position.x, 0, 1, 0, getWindowWidth()), lmap<float>(position.y, 0, 1, 0, getWindowHeight()));

            //
            if (shapePos.size() > 0) {
                for (int i = 0; i < shapePos.size(); i ++) {
                    Vec2f diff = Vec2f((shapePos[i].x - relPos.x) * 0.001f, (shapePos[i].y - relPos.y)  * 0.001f);
                    float shapeInfluence = diff.lengthSquared();
                    if (shapeInfluence < 0.005f) {
                        position.z -= shapeInfluence * 2.f;
                        isTarget[x * VERTICES_Y + y] = true;
                        timeDiff[x * VERTICES_Y + y] = time;
                    }
                }
            }
            
            // -----> influnce timer
            zPctBounce[x * VERTICES_Y + y] = lmap<float>(easeIn(currIterBounce[x * VERTICES_Y + y], 0.0, 1.0f, totalIterBounce[x * VERTICES_Y + y]), 0.f, 1.f, 1.f, 0);
            if ((time - timeDiff[x * VERTICES_Y + y] ) < 5.f && isTarget[x * VERTICES_Y + y] == true ) {
                position.z -= 0.1f * zPctBounce[x * VERTICES_Y + y];
            }else if((time - timeDiff[x * VERTICES_Y + y]) >= 10.f){
                isTarget[x * VERTICES_Y + y] = false;
            }
            if (isTarget[x * VERTICES_Y + y]) {
                if (currIterBounce[x * VERTICES_Y + y]< totalIterBounce[x * VERTICES_Y + y]) {
                    currIterBounce[x * VERTICES_Y + y] ++;
                }
            }else{
                currIterBounce[x * VERTICES_Y + y] = 0.f;
            }
            

            
            // ----generating Perlin Noise
            float nX = relPos.x * 0.0001f;
            float nY = relPos.y * 0.0001f;
            float nZ = app::getElapsedSeconds() * 0.1f;
            float nZ2 = app::getElapsedSeconds() * 0.1f + 500.f;
            Vec3f v( nX, nY, nZ );
            Vec3f v2( nX, nY, nZ2 );
            float noiseMulti = 0.05f;
            float noise = mPerlin.fBm(v) * noiseMulti;
            float noise2 = mPerlin.fBm(v2) * noiseMulti;
            
            
            
                        
            // ----> position adjustment
            
            //std::cout << noise3<< std::endl;
            float a = sin(getElapsedSeconds());
            //position -= Vec3f(0.48 + noise * 0.05, 0.48 + noise2 * 0.05, 0.f);
            
            if (stateStable) {
                currIterStart = 0.f;
                position -= Vec3f(xOffset + noise, yOffset + noise2,  0.f);
                iter.setColorRGBA(ColorA(1.f, 1.f, 1.f, 1.f));
            }else if (stateFly) {
                
                if(zPct == 1.f){
                    position -= Vec3f(xOffset + noise, yOffset + noise2,  1.f);
                }else{
                    position -= Vec3f(xOffset + noise, yOffset + noise2,  -1.f * zPct);
                    iter.setColorRGBA(ColorA(1.f, 1.f, 1.f, zPctReverse));
                }
            }else if (stateStart) {
                currIter = 0.f;
                position -= Vec3f(xOffset + noise, yOffset + noise2,  zPctStart);
                iter.setColorRGBA(ColorA(lmap<float>(zPctStart, 1.0f, 0, 0, 1.0f), lmap<float>(zPctStart, 1.0f, 0, 0, 1.0f), lmap<float>(zPctStart, 1.0f, 0, 0, 1.0f), 1.f));
            }else{
                
                position -= Vec3f(xOffset + noise, yOffset + noise2,  0.f);
            }
            
            
            iter.setPosition(position);
            //iter.setColorRGBA(ColorA(1.f, 1.f, 1.f, 1.f));

           
            ++iter;
       }
    }
    
  
    //transi
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
