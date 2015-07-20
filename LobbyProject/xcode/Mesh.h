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
#include "Shape.h"


using namespace ci;
using namespace ci::app;
using std::vector;


class Mesh{
public:
    Mesh(int &_meshType, bool &_isFirstMesh);
    
    void getTrackedShapes(vector<Shape> &_mTrackedShapes);
    void getTexture(gl::Texture &texture);
    void update(Vec2f &_shapePos, gl::Texture &texture, bool &_mouseClick, int &_vertices_x, int &_vertices_y);
    void draw();
    
    int VERTICES_X, VERTICES_Y;
    
    
    gl::VboMeshRef  mVboMesh;
    gl::Texture  mTexture;
    
    vector<float>   depthOffset;
    vector<float>   timeDiff;
    vector<bool>    isTarget;
    vector<float>   currIterBounce, totalIterBounce, zPctBounce;
    float           zOffset, xOffset, yOffset;
    float           zPct, zPctStart, currIter, totalIter, currIterStart, totalIterStart;
    float           easeInOut(float t,float b , float c, float d);
    float           transitionSpeed;
    float           meshType;
    float           timerMax;
    bool            stateStable, stateFly, stateStart;
    bool            isFirstMesh;
    bool            drawTexture;
    bool            resetTexture;
    
    vector<Vec2f>       particlePos;
    vector<float>       particleRad;
    vector<float>       oscilateZ;
    vector<Vec3f>       shapePos;
    vector<Shape>       mTrackedShapes;
    
    Perlin          mPerlin;
    Perlin          mOsci;
    
};









