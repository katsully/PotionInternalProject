#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/qtime/QuickTime.h"
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
    
    qtime::MovieGlRef   mMovie;
    gl::Texture         mMovieTexture;
    gl::Texture         mTexture;
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
    
    std::cout<<getAppPath()<<std::endl;
    //absolute path now...
   // addAssetDirectory("/Users/luobin/PotionInternalProject/LobbyProject/assets");
    addAssetDirectory("../../../../../assets");
    try{
        // fs::path path = getOpenFilePath();
        mMovie = qtime::MovieGl::create(loadAsset("po.mp4"));
        
    } catch( ... ){
        console() << "file is not a valid movie" << std::endl;
    }
    
    
    //load movie and play
    if (mMovie) {
        mMovie->setLoop();
        mMovie->play();
        mMovie->setVolume(0.1f);
    }

    
    //init
    mEye = Vec3f(0, 0, 1.f);
    mCenter = Vec3f::zero();
    mUp = Vec3f::yAxis();
    volumeMin = 0.9f;
    mSceneRot = ci::Quatf(M_PI, 0, 0);
    drawMesh = true;
    
    mParams = params::InterfaceGl("mesh", Vec2i(225, 100));
    mParams.addParam("camera rotation", &mSceneRot);
    mParams.addParam("camera viewing volume min", &volumeMin);
    mParams.addParam("draw mesh", &drawMesh);
    
    mTexture = gl::Texture(loadImage(loadResource("demo.jpg")));
    mFrameSubtraction.setup();
    myMesh = new Mesh(40, 40, 0);
    
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
    
    //-----> load texture from movie --- could expand and determine which texture to read
    //-----> also because there's issue with texture2d and GL_TEXTURE_RECTANGLE_ARB tex coordinates
    //-----> changing texture needs the coordinates in mesh.cpp to be updated
    if( mMovie ){
        mMovieTexture = gl::Texture(mMovie->getTexture());
    }
    
    myMesh->getParticle(mFrameSubtraction.mParticleController.mParticles);
    myMesh->update(mousePos, mMovieTexture);
    
}

void LobbyProjectApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
    gl::enableDepthRead();
    mFrameSubtraction.draw();
    if (drawMesh) {
        myMesh->draw();
    }
    
    mParams.draw();
}

CINDER_APP_NATIVE( LobbyProjectApp, RendererGl )
