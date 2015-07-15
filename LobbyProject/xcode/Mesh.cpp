//
//  Mesh.cpp
//  MeshController
//
//  Created by Luobin Wang on 6/23/15.
//
//

#include "Mesh.h"
#include "FrameSubtraction.h"



Mesh::Mesh(int &_vertices_x, int &_vertices_y, int &_meshType, bool &_isFirstMesh){
    this->VERTICES_X    = _vertices_x ;
    this->VERTICES_Y    = _vertices_y;
    this->isFirstMesh   = _isFirstMesh;
    this->meshType      = _meshType;
    
    zPct            = 0.f;
    zPctStart       = 0.f;
    currIter        = 0.f;
    totalIter       = 100.f;
    currIterStart   = 0.f;
    totalIterStart  = 100.f;
    xOffset         = 0.48f;
    yOffset         = 0.47f;
    transitionSpeed = 0.5f;
    timerMax        = 3.f;
    drawTexture     = true;
   
    
    if (isFirstMesh == true) {
        stateStable = false;
        stateStart  = true;
        stateFly    = false;
    }else{
        stateFly    = true;
        stateStable = false;
        stateStart  = false;
    }
    

    
    for (int x = 0; x < VERTICES_X; x++) {
        for (int y = 0; y < VERTICES_Y; y++) {
            
            timeDiff.push_back(0);
            isTarget.push_back(false);
            currIterBounce.push_back(0);
            totalIterBounce.push_back(100.f);
            zPctBounce.push_back(0);
            oscilateZ.push_back(0.f);
        }
    }
    
    
    
    
}


float Mesh::easeIn(float t, float b , float c, float d){
    if ((t/=d/2) < 1) return c/2*t*t*t + b;
    return c/2*((t-=2)*t*t + 2) + b;
}

void Mesh::getTrackedShapes(vector<Shape> &_mTrackedShapes){
    
    
    shapePos.clear();
    if (drawTexture) {
        this->mTrackedShapes = _mTrackedShapes;
        if (mTrackedShapes.size() > 0) {
            for (int i = 0; i < mTrackedShapes.size() - 1; i ++) {
                for (cv::vector<cv::Point>::iterator j = mTrackedShapes[i].hull.begin(); j != mTrackedShapes[i].hull.end() ; ++j ) {
                    shapePos.push_back(Vec3f(lmap<float>(j->x, 0, 320, 0, getWindowWidth()), lmap<float>(j->y, 0, 240, 0, getWindowHeight()),  lmap<float>(mTrackedShapes[i].depth, 0.f, 1.f, 1.f, -1.f)));
                }
                
            }
        }
    }
    
}

void Mesh::getTexture(gl::Texture &texture){
    
    if (drawTexture) {
        this->mTexture = texture;
    }
    
    
    vector<uint32_t> indices;
    vector<Vec2f> texCoords;
    
    int totalVertices = VERTICES_X * VERTICES_Y;
    int totalQuads = ( VERTICES_X - 1 ) * ( VERTICES_Y - 1 );
    gl::VboMesh::Layout layout;
    layout.setDynamicIndices();
    layout.setDynamicPositions();
    layout.setStaticTexCoords2d();
    layout.setDynamicColorsRGBA();
    
    
    //    determine mesh type
    if (meshType == 0) {
        mVboMesh = gl::VboMesh::create(totalVertices, totalQuads*4, layout, GL_QUADS);
    }else if (meshType == 1){
        mVboMesh = gl::VboMesh::create(totalVertices, totalQuads*4, layout, GL_POINTS);
    }else{
        mVboMesh = gl::VboMesh::create(totalVertices, totalQuads*4, layout, GL_QUADS);
    }
    
   
    
    
    //     buffer texCoords and indices
    for (int x = 0; x < VERTICES_X; x++) {
        for (int y = 0; y < VERTICES_Y; y++) {
   
            //    creating quads
            if (( x + 1 < VERTICES_X ) && ( y + 1 < VERTICES_Y )) {
                indices.push_back( (x+0) * VERTICES_Y + (y+0) );
                indices.push_back( (x+1) * VERTICES_Y + (y+0) );
                indices.push_back( (x+1) * VERTICES_Y + (y+1) );
                indices.push_back( (x+0) * VERTICES_Y + (y+1) );
            }
        }
    }
    
    
    //     mapping texture (0 to texture size) video target is 34037, pic tartget is 3553;
    if (mTexture && mTexture.getTarget() == 34037) {
        for (int x = 0; x < VERTICES_X; x++) {
            for (int y = VERTICES_Y; y > 0; y--) {
                //texCoords.push_back(Vec2f( mTexture.getWidth() * x / (float)VERTICES_X, mTexture.getHeight() * y / (float)VERTICES_Y ));
                texCoords.push_back(Vec2f( mTexture.getWidth() * x / (float)VERTICES_X, mTexture.getHeight() * y / (float)VERTICES_Y ));
                //std::cout<<mTexture.getWidth()<<std::endl;
            }
        }
    }else{
        for (int x = 0; x < VERTICES_X; x++) {
            for (int y = 0; y < VERTICES_Y; y++) {
                texCoords.push_back(Vec2f( x / (float)VERTICES_X, y / (float)VERTICES_Y ));
            }
        }
    }
    
    
    
    
    mVboMesh->bufferIndices(indices);
    mVboMesh->bufferTexCoords2d(0, texCoords);
    
}




void Mesh::update(Vec2f &_shapePos, gl::Texture &texture,  bool &_mouseClick){
    getTexture(texture);
    
    float time = getElapsedSeconds();
    
    bool mouseClick = _mouseClick;
    float zPctReverse = lmap<float>(zPct, 0.0, 1.0, 1.0, 0);
    float zPctStarting = lmap<float>(zPct, 0.0, 1.0, -1.0, 0);
    zPct = easeIn(currIter, 0.0, 1.0, totalIter);
    zPctStart = lmap<float>(easeIn(currIterStart, 0.0, 1.0f, totalIterStart), 0.f, 1.f, 1.f, 0);
    
    
    
    if (mouseClick && stateStable) {
        stateFly = true;
        stateStable = false;
    } else if (mouseClick && stateFly && zPct == 1.f){
        stateStart = true;
        stateFly = false;
    } else if (zPctStart == 0.f && stateStart){
        stateStable = true;
        stateStart  = false;
    }
    



    
    
    //    generate movements
    gl::VboMesh::VertexIter iter = mVboMesh->mapVertexBuffer();
    for( int x = 0; x < VERTICES_X; ++x ) {
        for( int y = 0; y < VERTICES_Y; ++y ) {
            Vec3f position = Vec3f(Vec3f( x / (float)VERTICES_X, y / (float)VERTICES_Y, 0.f));
            //    mapping mesh pos to relative pos to the window
            Vec2f relPos = Vec2f(lmap<float>(position.x, 0, 1, 0, getWindowWidth()), lmap<float>(position.y, 0, 1, 0, getWindowHeight()));
            
            
            //     generating Perlin Noise
            float noise, noise2;
            if (drawTexture) {
                float nX = relPos.x * 0.0001f;
                float nY = relPos.y * 0.0001f;
                float nZ = app::getElapsedSeconds() * 0.1f;
                float nZ2 = app::getElapsedSeconds() * 0.1f + 500.f;
                Vec3f v( nX, nY, nZ );
                Vec3f v2( nX, nY, nZ2 );
                float noiseMulti = 0.05f;
                noise = mPerlin.fBm(v) * noiseMulti;
                noise2 = mPerlin.fBm(v2) * noiseMulti;
            }else{
                noise = 0.f;
                noise2 = 0.f;
            }
            
            //      transition animation
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
            

            
            if (position.z <= 0.8f && position.z >= -0.4f) {
                
                //       calculate shape influence
                if (shapePos.size() > 0) {
                    for (int i = 0; i < shapePos.size(); i ++) {
                        Vec2f diff = Vec2f((shapePos[i].x - relPos.x) * 0.001f, (shapePos[i].y - relPos.y)  * 0.001f);
                        float shapeInfluence = diff.lengthSquared();
                        if (shapeInfluence < 0.005f ) {
                            //position.z -= shapeInfluence * 1.f + shapePos[i].z * 0.001f ;
                            //position.z -= shapePos[i].z * 0.01f ;
                            isTarget[x * VERTICES_Y + y] = true;
                            timeDiff[x * VERTICES_Y + y] = time;
                        }
                    }
                }
                
                //       oscilate every single vertices in z axis.
                float timer = time - timeDiff[x * VERTICES_Y + y];
                oscilateZ[x * VERTICES_Y + y] = sin(timer * 8.f) * (timerMax - timer) * 0.1f;
                
                
                //       influnce timer
                // zPctBounce[x * VERTICES_Y + y] = lmap<float>(easeIn(currIterBounce[x * VERTICES_Y + y], 0.0, 1.0f, totalIterBounce[x * VERTICES_Y + y]), 0.f, 1.f, 1.f, 0);
                if ( timer < timerMax && isTarget[x * VERTICES_Y + y] == true ) {
                    //position.z -= 0.1f * zPctBounce[x * VERTICES_Y + y] * oscilateZ[x * VERTICES_Y + y];
                    position.z -= 0.1f * oscilateZ[x * VERTICES_Y + y];
                }else if( timer >= timerMax){
                    isTarget[x * VERTICES_Y + y] = false;
                }
                /* useless if oscillation has (timerMax - timer)
                if (timer == 0) {
                    if (currIterBounce[x * VERTICES_Y + y] < totalIterBounce[x * VERTICES_Y + y]) {
                        currIterBounce[x * VERTICES_Y + y] += transitionSpeed;
                    }
                }else{
                    currIterBounce[x * VERTICES_Y + y] = 0.f;
                }
                */
            }
            
            //      check if texture is in visible range
            if (position.z <= 0.8f && position.z >= -0.6f){
                drawTexture = true;
            }else{
                drawTexture = false;
            }
            
            //       check movie reset time
            if (position.z == -1.f && stateStart) {
                resetMovie = true;
            }else{
                resetMovie = false;
            }
            
            
            iter.setPosition(position);
           
            //iter.setColorRGBA(ColorA(1.f, 1.f, 1.f, 1.f));
            
            
            ++iter;
        }
    }
    
    
    //transition iter
    if (stateFly) {
        if (currIter< totalIter) {
            currIter += transitionSpeed;
        }
    }
    
    if (stateStart) {
        if (currIterStart< totalIterStart) {
            currIterStart += transitionSpeed * 0.75f;
        }
    }
    
    
    
}

void Mesh::draw(){
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    //draw only when mesh in visible range
    if (drawTexture) {
        if(mTexture){
            mTexture.enableAndBind();
        }
        gl::draw(mVboMesh);
        if(mTexture){
            mTexture.unbind();
            mTexture.disable();
        }

    }
    
}
