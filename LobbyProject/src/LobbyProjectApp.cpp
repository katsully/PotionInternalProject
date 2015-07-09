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
    void getRandomFile();
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
   
    int                 meshX, meshY, meshType;
    float volumeMin;
    float time, timer, timerInterval;
    bool drawMesh;
    bool nextMeshState, mouseClick;
    bool meshReset, meshStart, nextMeshReset, nextMeshStart;
    bool firstMesh;
    bool secondMesh;
    
    FrameSubtraction    mFrameSubtraction;
    Mesh                *myMesh;

    Mesh                *myNextMesh;
    
    vector<boost::filesystem::path> mAssetNames;
    vector<boost::filesystem::path> mRemainingAssetNames;

};

void LobbyProjectApp::setup()
{
    addAssetDirectory("../../../../../assets");
    
    // get absolute path to assets' directory
    fs::path p( getAssetPath( "" ) );
    // iterate through the asset directory and add all filenames to the vector assetNames
    for ( fs::directory_iterator it( p ); it != fs::directory_iterator(); ++it ) {
        if ( ! is_directory( *it ) ) {
            mAssetNames.push_back( it->path().filename() );
        }
    }
    mRemainingAssetNames = mAssetNames;
    
    getRandomFile();
    
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
    meshX           = 48;
    meshY           = 27;
    meshType        = 0;
    timerInterval   = 20.f;
    
    mParams = params::InterfaceGl("mesh", Vec2i(225, 100));
    mParams.addParam("camera rotation", &mSceneRot);
    mParams.addParam("camera viewing volume min", &volumeMin);
    mParams.addParam("draw mesh", &drawMesh);
    mParams.addParam("timer interval", &timerInterval);
    
    
    
    mFrameSubtraction.setup();
    myMesh = new Mesh(meshX, meshY, meshType, firstMesh);
    myNextMesh = new Mesh(meshX , meshY , meshType, secondMesh);
    
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

void LobbyProjectApp::getRandomFile()
{
    // if no remaining assets, start over
    if ( mRemainingAssetNames.empty() ) {
        mRemainingAssetNames = mAssetNames;
    }
    
    // select a random asset
    Rand::randomize();
    int randInt = Rand::randInt( 0, mRemainingAssetNames.size() );
    boost::filesystem::path assetName = mRemainingAssetNames[randInt];
    // remove this asset from list of remaining assets
    mRemainingAssetNames.erase(find(mRemainingAssetNames.begin(), mRemainingAssetNames.end(), assetName ) );
    // get the extension of the asset
    boost::filesystem::path ext = assetName.extension();
    
    // if it is a movie, load and play the movie
    if( ext == ".mp4" ) {
        try{
            mMovie = qtime::MovieGl::create(loadAsset(assetName));
        
        } catch( ... ){
            console() << "file is not a valid movie" << std::endl;
        }
    
    
        //load movie and play
        if (mMovie) {
            mMovie->setLoop();
            mMovie->play();
            mMovie->setVolume(0.01f);
        }
    // else load it as a texture
    } else {
        mTexture = gl::Texture(loadImage(loadAsset(assetName)));
    }

}

void LobbyProjectApp::update()
{
    mCamera.setPerspective( 60.0f, 1.f, volumeMin, 3000.0f );
    mCamera.lookAt(mEye, mCenter, mUp);
    gl::setMatrices( mCamera );
    gl::rotate( mSceneRot);
    time = getElapsedSeconds();
    float timeDiff = time - timer;
    if (timeDiff >= timerInterval) {
        mouseClick = true;
        timer = time;
    }
    
    // will need to call mFrameSubtraction.mTrackedShapes, then iterate through the points of each tracked shape
   // myMesh->getParticle(mFrameSubtraction.mParticleController.mParticles);
    myMesh->getTrackedShapes(mFrameSubtraction.mTrackedShapes);
    myNextMesh->getTrackedShapes(mFrameSubtraction.mTrackedShapes);
    
    //reset
    if ( mMovie ){
        mMovieTexture = gl::Texture(mMovie->getTexture());
        if (myMesh->mTexture && myMesh->mTexture.getTarget() == 34037 && myMesh->resetMovie == true ) {
            mMovie->seekToStart();
        }
        if (myNextMesh->mTexture && myNextMesh->mTexture.getTarget() == 34037 && myNextMesh->resetMovie == true) {
            mMovie->seekToStart();
        }
        
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
        if (myMesh->zPct != 1.f) {
            myMesh->draw();
        }
        if (myNextMesh->zPct != 1.f) {
            myNextMesh->draw();
        }
    
    }
    
    mParams.draw();
}

CINDER_APP_NATIVE( LobbyProjectApp, RendererGl )
