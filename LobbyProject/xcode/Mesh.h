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



using namespace ci;
using namespace ci::app;
using std::vector;


class Mesh{
public:
    Mesh(int vertices_x, int vertices_y, gl::TextureRef &texture,  int meshType);
    
    void update(Vec2f &_mousePos);
    void draw();
    
    int VERTICES_X, VERTICES_Y;
    
    gl::VboMeshRef mVboMesh;
    gl::TextureRef mTexture;
    
    vector<float> timeDiff;
    vector<bool>  isTarget;
    Vec2f mousePos;
    Perlin          mPerlin;
    
    
};









