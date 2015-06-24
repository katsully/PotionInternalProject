#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "FrameSubtraction.h"
#include "Mesh.h"
#include "cinder/Camera.h"
#include "cinder/params/Params.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class LobbyProjectApp : public AppNative {
  public:
	void setup();
	void keyDown( KeyEvent event );
    void mouseMove( MouseEvent event );
	void update();
	void draw();
    
    gl::TextureRef      mTexture;
    CameraPersp         mCamera;
    Vec3f               mEye, mCenter, mUp;
    Vec2f               mousePos;
    Quatf               mSceneRot;
    params::InterfaceGl mParams;
    
    float volumeMin;
    bool drawMesh;
    
    FrameSubtraction    mFrameSubtraction;
    Mesh                *myMesh;
};

void LobbyProjectApp::setup(){
    
    //init
    mEye = Vec3f(0, 0, 1.f);
    mCenter = Vec3f::zero();
    mUp = Vec3f::yAxis();
    volumeMin = 0.98f;
    mSceneRot = ci::Quatf(M_PI, 0, 0);
    drawMesh = true;
    
    mParams = params::InterfaceGl("mesh", Vec2i(225, 100));
    mParams.addParam("camera rotation", &mSceneRot);
    mParams.addParam("camera viewing volume min", &volumeMin);
    mParams.addParam("draw mesh", &drawMesh);
    
    mTexture = gl::Texture::create(loadImage(loadResource("demo.jpg")));
    mFrameSubtraction.setup();
    myMesh = new Mesh(40, 40, mTexture, 0);
}

void LobbyProjectApp::keyDown( KeyEvent event ){
    mFrameSubtraction.keyDown( event );
}

void LobbyProjectApp::mouseMove(MouseEvent event){
    mousePos = event.getPos();
}

void LobbyProjectApp::update()
{
    mCamera.setPerspective( 60.0f, 1.f, volumeMin, 3000.0f );
    mCamera.lookAt(mEye, mCenter, mUp);
    gl::setMatrices( mCamera );
    gl::rotate( mSceneRot);
    myMesh->update(mousePos);
    myMesh->update(mousePos);
}

void LobbyProjectApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
    
    mFrameSubtraction.draw();
    if (drawMesh) {
        myMesh->draw();
    }
    
    mParams.draw();
}

CINDER_APP_NATIVE( LobbyProjectApp, RendererGl )
