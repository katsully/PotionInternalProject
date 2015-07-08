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
    void mouseDown( MouseEvent event );
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
    bool nextMeshState, mouseClick;
    bool meshReset, meshStart, nextMeshReset, nextMeshStart;
    bool firstMesh;
    bool secondMesh;
    
    FrameSubtraction    mFrameSubtraction;
    Mesh                *myMesh;

    Mesh                *myNextMesh;
    
    vector<boost::filesystem::path> assetNames;
};

void LobbyProjectApp::setup()
{
    std::cout<<getAppPath()<<std::endl;

    addAssetDirectory("../../../../../assets");
    
    // get absolute path to assets' directory
    fs::path p( getAssetPath( "" ) );
    // iterate through the asset directory and add all filenames to the vector assetNames
    for ( fs::directory_iterator it( p ); it != fs::directory_iterator(); ++it ) {
        if ( ! is_directory( *it ) ) {
            assetNames.push_back( it->path().filename() );
        }
    }

    try{
        // pick a movie at random from the asset directory
        int randInt = Rand::randInt( 0, assetNames.size() );
        mMovie = qtime::MovieGl::create(loadAsset(assetNames[randInt]));
        
    } catch( ... ){
        console() << "file is not a valid movie" << std::endl;
    }
    
    
    //load movie and play
    if (mMovie) {
        mMovie->setLoop();
        mMovie->play();
        mMovie->setVolume(0.01f);
    }
    
    //init
    mEye            = Vec3f(0, 0, 0.79f);
    mCenter         = Vec3f::zero();
    mUp             = Vec3f::yAxis();
    volumeMin       = 0.20f;
    mSceneRot       = ci::Quatf(M_PI, 0, 0);
    drawMesh        = true;
    nextMeshState   = false;
    mouseClick      = false;
    firstMesh       = true;
    secondMesh      = false;
    
    mParams = params::InterfaceGl("mesh", Vec2i(225, 100));
    mParams.addParam("camera rotation", &mSceneRot);
    mParams.addParam("camera viewing volume min", &volumeMin);
    mParams.addParam("draw mesh", &drawMesh);
    
    mTexture = gl::Texture(loadImage(loadResource("demo.jpg")));
    mFrameSubtraction.setup();
    myMesh = new Mesh(32, 18, 0, firstMesh);
    myNextMesh = new Mesh(32 , 18 , 0, secondMesh);
    
}

void LobbyProjectApp::prepareSettings( Settings* settings )
{
    settings->setFrameRate( 60.0f );
    settings->setWindowSize( 960, 540 );
}

void LobbyProjectApp::keyDown( KeyEvent event )
{
}

void LobbyProjectApp::mouseMove( MouseEvent event )
{
    mousePos = event.getPos();
}

void LobbyProjectApp::mouseDown( MouseEvent event )
{
    nextMeshState = !nextMeshState;
    mouseClick = true;
}

void LobbyProjectApp::update()
{
    mCamera.setPerspective( 60.0f, 1.f, volumeMin, 3000.0f );
    mCamera.lookAt(mEye, mCenter, mUp);
    gl::setMatrices( mCamera );
    gl::rotate( mSceneRot);
    // will need to call mFrameSubtraction.mTrackedShapes, then iterate through the points of each tracked shape
   // myMesh->getParticle(mFrameSubtraction.mParticleController.mParticles);
    myMesh->getTrackedShapes(mFrameSubtraction.mTrackedShapes);
    myNextMesh->getTrackedShapes(mFrameSubtraction.mTrackedShapes);
    
    if ( mMovie ){
        mMovieTexture = gl::Texture(mMovie->getTexture());
    }
    
    myMesh->update(mousePos, mTexture, mouseClick);
    myNextMesh->update(mousePos, mMovieTexture, mouseClick);
    mouseClick = false;
}

void LobbyProjectApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 1, 1, 1 ) );
    gl::enableDepthRead();
    if (drawMesh) {
        myMesh->draw();
        myNextMesh->draw();
    }
    
    mParams.draw();
}

CINDER_APP_NATIVE( LobbyProjectApp, RendererGl )
