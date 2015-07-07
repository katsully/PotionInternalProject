//
//  Mesh.h
//  MeshController
//
//  Created by Luobin Wang on 6/23/15.
//
//

#pragma once



#include "cinder/gl/gl.h"
#include "cinder/gl/Vbo.h"
#include "cinder/gl/Texture.h"
#include "cinder/Perlin.h"
#include "Particle.h"
#include "Shape.h"




using namespace ci;
using namespace ci::app;
using std::vector;


class Mesh{
public:
    Mesh(int vertices_x, int vertices_y, int meshType, bool &isFirstMesh);
    
    void getParticle(std::list<Particle> &_mParticles);
    void getTrackedShapes(vector<Shape> &_mTrackedShapes);
    void update(Vec2f &_mousePos, gl::Texture &texture, bool &_mouseClick);
    void draw();
    
    int VERTICES_X, VERTICES_Y;
    
    
    gl::VboMeshRef  mVboMesh;
    gl::Texture  mTexture;
    
    vector<float>   timeDiff;
    vector<bool>    isTarget;
    vector<float>   timeDiffP;
    vector<bool>    isTargetP;
    vector<float>   currIterBounce, totalIterBounce, zPctBounce;
    float           zOffset, xOffset, yOffset;
    float           zPct, zPctStart, currIter, totalIter, currIterStart, totalIterStart;
    float           easeIn(float t,float b , float c, float d);
    bool            stateStable, stateFly, stateStart;
    bool            isFirstMesh;
    
    
    vector<Vec2f>       particlePos;
    vector<float>       particleRad;
    vector<Vec2f>       shapePos;
    std::list<Particle> mParticles;
    vector<Shape>       mTrackedShapes;
    
    Vec2f           mousePos;
    Perlin          mPerlin;
    Perlin          mOsci;
    
    
};









