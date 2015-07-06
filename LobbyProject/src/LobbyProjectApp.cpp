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
    void prepareSettings( Settings* settings );
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
    
   // addAssetDirectory("/Users/luobin/PotionInternalProject/LobbyProject/assets");
    addAssetDirectory("../../../../../assets");
    try{
        // fs::path path = getOpenFilePath();
        mMovie = qtime::MovieGl::create(loadAsset("po.mp4"));
        
    } catch( ... ){
        console() << "file is not a valid movie" << std::endl;
    }
    
    setWindowSize(960, 540);
    //load movie and play
    if (mMovie) {
        mMovie->setLoop();
        mMovie->play();
        mMovie->setVolume(0.01f);
    }

    
    //init
    mEye = Vec3f(0, 0, 0.75f);
    mCenter = Vec3f::zero();
    mUp = Vec3f::yAxis();
    volumeMin = 0.50f;
    mSceneRot = ci::Quatf(M_PI, 0, 0);
    drawMesh = true;
    
    mParams = params::InterfaceGl("mesh", Vec2i(225, 100));
    mParams.addParam("camera rotation", &mSceneRot);
    mParams.addParam("camera viewing volume min", &volumeMin);
    mParams.addParam("draw mesh", &drawMesh);
    
    mTexture = gl::Texture(loadImage(loadResource("demo.jpg")));
    mFrameSubtraction.setup();
    myMesh = new Mesh(16, 9, 0);
    
}

void LobbyProjectApp::prepareSettings( Settings* settings ){
    settings->setFrameRate( 60.0f );
    settings->setWindowSize( 800, 600 );
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
    // will need to call mFrameSubtraction.mTrackedShapes, then iterate through the points of each tracked shape
   // myMesh->getParticle(mFrameSubtraction.mParticleController.mParticles);
    
    //-----> load texture from movie --- could expand and determine which texture to read
    //-----> also because there's an issue with texture2d and GL_TEXTURE_RECTANGLE_ARB tex coordinates
    //-----> changing texture needs the coordinates in mesh.cpp to be updated (fix pending)
    if( mMovie ){
        mMovieTexture = gl::Texture(mMovie->getTexture());
    }
    bool fly = true;
    
    
    myMesh->update(mousePos, mMovieTexture, fly);
}

void LobbyProjectApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 1, 1, 1 ) );
    gl::enableDepthRead();
    // no longer need to draw frame subtraction
    // mFrameSubtraction.draw();
    if (drawMesh) {
        myMesh->draw();
    }
    
    mParams.draw();
}

CINDER_APP_NATIVE( LobbyProjectApp, RendererGl )
