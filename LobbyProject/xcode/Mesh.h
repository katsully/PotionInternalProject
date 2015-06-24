//
//  VBOMesh.h
//  LobbyProject
//
//  
//
//

#include "Resources.h"

#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Vbo.h"
#include "cinder/gl/Texture.h"
#include "cinder/Camera.h"
#include "cinder/ImageIo.h"
#include "cinder/params/Params.h"
#include "cinder/Rand.h"
#include "cinder/Perlin.h"

using namespace ci;
using namespace ci::app;

using std::vector;
params::InterfaceGl mParams;

class VBOMesh {
public:
    VBOMesh();
    void setup();
    void update();
    void draw();
    void mouseMove(MouseEvent event);
    
    static const int VERTICES_X = 50, VERTICES_Z = 50;
    
    gl::VboMeshRef	mVboMesh, mVboMesh2 , mVboMesh3;
    gl::TextureRef	mTexture;
    
    gl::TextureRef  revealImage;
    
    CameraPersp		mCamera;
    Vec3f           mEye, mCenter, mUp;
    Vec2f           mousePos, mouseRelPos;
    Quatf mSceneRotation;
    Perlin          mPerlin;
    
    float mappingMin, mappingMax;
    float cameraVisionMin, cameraVisionMax;
    float time , timeCount, prevTime;
    bool showLastLayer, showFrontLayer;
    
    vector<float> timeDiff;
    vector<bool>  isTarget;
    vector<float> timeDiff2;
    vector<bool>  isTarget2;
    
};
