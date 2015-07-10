#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/qtime/QuickTime.h"
#include "FrameSubtraction.h"
#include "Mesh.h"
#include "cinder/Camera.h"
#include "cinder/params/Params.h"
#include "json/json.h"
#include <fstream>

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
    void getRandomFile(int _meshTag);
	void update();
	void draw();
    void shutdown();
    
    qtime::MovieGlRef   mMovie ,mMovie2;
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
    bool textureType, textureType2;
    
    FrameSubtraction    mFrameSubtraction;
    Mesh                *myMesh;

    Mesh                *myNextMesh;
    
    vector<boost::filesystem::path> mAssetNames;
    vector<boost::filesystem::path> mRemainingAssetNames;
    
    Json::Value mData;  // to store GUI params
    Json::Reader mReader;   // this will read the json file where the gui params are stored and parse it to mData
    
    bool mFullScreen;   // bool for whether app is fullscreen or not
    bool mShowParams;    // boo for whether gui params are shown
    
    int mFrameRate;
};

void LobbyProjectApp::setup()
{
    addAssetDirectory("../../../../../assets");
    
    // get absolute path to assets' directory
    fs::path p( getAssetPath( "" ) );
    // iterate through the asset directory and add all filenames to the vector assetNames
    for ( fs::directory_iterator it( p ); it != fs::directory_iterator(); ++it ) {
        if ( ! is_directory( *it ) ) {
            if ( it->path().filename() != ".DS_Store" && it->path().filename() != "gui_params.json" ) {
                mAssetNames.push_back( it->path().filename() );
            }
        }
    }
    mRemainingAssetNames = mAssetNames;
    
    getRandomFile(0);
    getRandomFile(1);
    
    //init
    mEye            = Vec3f(0, 0, 0.79f);
    mCenter         = Vec3f::zero();
    mUp             = Vec3f::yAxis();
    nextMeshState   = false;
    mouseClick      = false;
    firstMesh       = true;
    secondMesh      = false;
    textureType     = false;
    textureType2    = false;
    meshX           = 64;
    meshY           = 36;
    meshType        = 0;
    mFrameRate = getAverageFps();
    mFullScreen = true;
    mShowParams = false;
    
    // set app to fullscreen
    setFullScreen(mFullScreen);
    
    // get filepath to json file
    string guiParamsFilePath = p.string() + "/gui_params.json";
    // read the json file
    ifstream ifile(guiParamsFilePath, std::ifstream::binary);
    // parse the data to mData
    bool itworked = mReader.parse( ifile, mData, false );
    // if succesful, assign variables values based on the json file
    if (itworked) {
        Json::Value sceneRotParams = mData.get( "mSceneRot", {} );
        mSceneRot = ci::Quatf( sceneRotParams.get( "xRotation", 0.0f ).asFloat(), sceneRotParams.get( "yRotation", 0.0f ).asFloat(), sceneRotParams.get( "zRotation", 0.0f ).asFloat() );
        volumeMin = mData.get( "volumeMin", 0.0f ).asFloat();
        drawMesh = mData.get( "drawMesh", false ).asBool();
        timerInterval = mData.get( "timerInterval", 0.0f ).asFloat();
    }
    
    mParams = params::InterfaceGl( "mesh", Vec2i( 225, 125 ) );
    mParams.addParam( "camera rotation", &mSceneRot );
    mParams.addParam( "camera viewing volume min", &volumeMin );
    mParams.addParam( "draw mesh", &drawMesh );
    mParams.addParam( "timer interval", &timerInterval );
    mParams.addParam( "fps", &mFrameRate );
    
    mFrameSubtraction.setup( mData );
    myMesh = new Mesh( meshX, meshY, meshType, firstMesh );
    myNextMesh = new Mesh( meshX , meshY , meshType, secondMesh );
    
}

void LobbyProjectApp::prepareSettings( Settings* settings )
{
    settings->setFrameRate( 60.0f );
    settings->setWindowSize( 960, 540 );
}

void LobbyProjectApp::keyDown( KeyEvent event )
{
    if ( event.getChar() == 'f' ) {
        setFullScreen(!mFullScreen);
        mFullScreen = !mFullScreen;
    } else if ( event.getChar() == 'p' ) {
        mShowParams = !mShowParams;
    }
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

void LobbyProjectApp::getRandomFile(int _meshTag)
{
    // if no remaining assets, start over
    if ( mRemainingAssetNames.empty() ) {
        mRemainingAssetNames = mAssetNames;
    }
    
    // select a random asset from the list of remaining assets
    Rand::randomize();
    int randInt = Rand::randInt( 0, mRemainingAssetNames.size() );
    boost::filesystem::path assetName = mRemainingAssetNames[randInt];
   
    // remove this asset from list of remaining assets
    mRemainingAssetNames.erase(find(mRemainingAssetNames.begin(), mRemainingAssetNames.end(), assetName ) );
    // get the extension of the asset
    boost::filesystem::path ext = assetName.extension();

    // if it is a movie, load and play the movie
    if( ext == ".mp4" ) {
        if (_meshTag == 0) {
            textureType = false;
            try{
                mMovie = qtime::MovieGl::create(loadAsset(assetName));
                
            } catch( ... ){
                console() << "file is not a valid movie" << std::endl;
            }
        }else{
            textureType2 = false;
            try{
                mMovie2 = qtime::MovieGl::create(loadAsset(assetName));
                
            } catch( ... ){
                console() << "file is not a valid movie" << std::endl;
            }
        }
        
        //load movie and play
        if (_meshTag == 0) {
            textureType = false;
            if (mMovie) {
                mMovie->setLoop();
                mMovie->play();
                mMovie->setVolume(0.01f);
            }
        }else{
            textureType2 = false;
            if (mMovie2) {
                mMovie2->setLoop();
                mMovie2->play();
                mMovie2->setVolume(0.01f);
            }
        }
        // else load it as a texture
    } else {
        
        if (_meshTag == 0) {
            textureType = true;
            console() << "asset name" << assetName << endl;
            mTexture = gl::Texture(loadImage(loadAsset(assetName)));
        }
        if (_meshTag == 1) {
            textureType2 = true;
            mMovieTexture = gl::Texture(loadImage(loadAsset(assetName)));
        }
    }
        
}

void LobbyProjectApp::update()
{
    mFrameRate = getAverageFps();
    
    mCamera.setPerspective( 60.0f, 1.f, volumeMin, 3000.0f );
    mCamera.lookAt(mEye, mCenter, mUp);
    gl::setMatrices( mCamera );
    gl::rotate( mSceneRot);
    time = getElapsedSeconds();
    float timeDiff = time - timer;
    if ( timeDiff >= timerInterval ) {
        mouseClick = true;
        timer = time;
    }

    myMesh->getTrackedShapes(mFrameSubtraction.mTrackedShapes);
    myNextMesh->getTrackedShapes(mFrameSubtraction.mTrackedShapes);
    
    //reset
    if ( myMesh->resetMovie == true) {
        getRandomFile(0);
    }
    if ( mMovie && textureType == false ) {
        mTexture = gl::Texture(mMovie->getTexture());
    }
    
    if ( myNextMesh->resetMovie == true) {
        getRandomFile(1);
    }
    if ( mMovie2 && textureType2 == false ) {
        mMovieTexture = gl::Texture(mMovie2->getTexture());
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
    
    if (mShowParams) {
        mParams.draw();
    }
}

void LobbyProjectApp::shutdown(){
    mFrameSubtraction.shutdown();
}

CINDER_APP_NATIVE( LobbyProjectApp, RendererGl )
