#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/Light.h"
#include "cinder/qtime/QuickTime.h"
#include "FrameSubtraction.h"
#include "Mesh.h"
#include "cinder/Camera.h"
#include "cinder/params/Params.h"
#include "json/json.h"
#include "cinder/gl/Fbo.h"
#include <fstream>

using namespace ci;
using namespace ci::app;
using namespace std;

class LobbyProjectApp : public AppNative {
public:
    void prepareSettings( Settings* settings );
    void setup();
	void keyDown( KeyEvent event );
    void mouseMove( MouseEvent event );
    void mouseDown( MouseEvent event );
    void getRandomFile(int _meshTag);
	void update();
	void draw();
    void shutdown();

    
    qtime::MovieGlRef   mMovie ,mMovie2;
    gl::Texture         mNextTexture;
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
    bool cursorHidden;
    
    FrameSubtraction    mFrameSubtraction;
    Mesh                *myMesh;
    Mesh                *myNextMesh;
    
    vector<boost::filesystem::path> mAssetNames;    // list of all asset names
    int mCurrentAsset;  // keep track of which asset is being shown
    
    Json::Value mData;  // to store GUI params
    Json::Reader mReader;   // this will read the json file where the gui params are stored and parse it to mData
    
    bool mFullScreen;   // bool for whether app is fullscreen or not
    bool mShowParams;    // bool for whether gui params are shown
    bool mOnTop;    // bool for whether the window always remains above all other windows
    bool mPoints;   // bool for whether motion tracking points are shown
    
    int mFrameRate;
};

void LobbyProjectApp::setup()
{
    addAssetDirectory( "../../../../../assets" );
    
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
    mCurrentAsset = 0;
    
    getRandomFile(0);
    getRandomFile(1);
    
    //init
    mEye            = Vec3f( 0, 0, 0.79f );
    mCenter         = Vec3f::zero();
    mUp             = Vec3f::yAxis();
    nextMeshState   = false;
    mouseClick      = false;
    firstMesh       = true;
    secondMesh      = false;
    textureType     = false;
    textureType2    = false;
    meshX           = 48;
    meshY           = 27;
    meshType        = 0;
    mFrameRate      = getAverageFps();
    mFullScreen     = true;
    mShowParams     = false;
    mOnTop          = true;
    mPoints         = false;
    cursorHidden    = true;
 
    
    // set app to fullscreen
    setFullScreen(mFullScreen);
    
    hideCursor();
    gl::enableDepthRead();
    gl::enableDepthWrite();
    
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
    // sets whether the window always remains above all other windows
    settings->setAlwaysOnTop();
    settings->setFrameRate( 60.0f );
    settings->setWindowSize( 960, 540 );
}

void LobbyProjectApp::keyDown( KeyEvent event )
{
    if ( event.getChar() == 'f' ) {
        mFullScreen = !mFullScreen;
        setFullScreen(mFullScreen);
    } else if ( event.getChar() == 'p' ) {
        mShowParams = !mShowParams;
    } else if( event.getChar() == 't' ) {
        mOnTop = !mOnTop;
        getWindow()->setAlwaysOnTop(mOnTop);
    } else if( event.getChar() == 'o' ) {
        mPoints = !mPoints;
    }
}

void LobbyProjectApp::mouseMove( MouseEvent event )
{
    mousePos = event.getPos();
}

void LobbyProjectApp::mouseDown( MouseEvent event )
{
    nextMeshState   = !nextMeshState;
    mouseClick      = true;
    cursorHidden    = !cursorHidden;
}

void LobbyProjectApp::getRandomFile(int _meshTag)
{
    // select the next available asset
    boost::filesystem::path assetName = mAssetNames[mCurrentAsset];
    mCurrentAsset++;
    
    // if you went through the entire asset directory, start over
    if ( mCurrentAsset == mAssetNames.size() ) {
        mCurrentAsset = 0;
    }
   
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
            // if cannot load texture, it will print the error and asset name to the console, then it will increment the asset index counter, and just show the following image
            try {
                mTexture = gl::Texture(loadImage(loadAsset(assetName)));
 
            } catch (Exception ex ){
                std::cout << "something went wrong loading the image " << ex.what() << std::endl;
                std::cout << "image name " << assetName << std::endl;
                if ( mCurrentAsset == mAssetNames.size() - 1 ) {
                    mCurrentAsset = 0;
                } else {
                    mCurrentAsset++;
                }
                assetName = mAssetNames[mCurrentAsset];
            }
        }
        if (_meshTag == 1) {
            textureType2 = true;
            // if cannot load texture, it will print the error and asset name to the console, then it will increment the asset index counter, and just show the following image
            try {
                mNextTexture = gl::Texture(loadImage(loadAsset(assetName)));
                
            } catch (Exception ex ){
                std::cout << "something went wrong loading the image " << ex.what() << std::endl;
                std::cout << "image name " << assetName << std::endl;
                if ( mCurrentAsset == mAssetNames.size() - 1 ) {
                    mCurrentAsset = 0;
                } else {
                    mCurrentAsset++;
                }
                assetName = mAssetNames[mCurrentAsset];
            }
        }
    }
}

void LobbyProjectApp::update()
{
    mFrameRate = getAverageFps();
    
    mCamera.setPerspective( 60.0f, 1.0, volumeMin, 3000.0f );
    mCamera.lookAt(mEye, mCenter, mUp);
    gl::setMatrices( mCamera );
    gl::rotate( mSceneRot);
    time = getElapsedSeconds();
    float timeDiff = time - timer;
    if ( timeDiff >= timerInterval ) {
        mouseClick = true;
        timer = time;
    }
    
    if (!cursorHidden){
        showCursor();
    }else{
        hideCursor();
    }
    
    
    myMesh->getTrackedShapes(mFrameSubtraction.mTrackedShapes);
    myNextMesh->getTrackedShapes(mFrameSubtraction.mTrackedShapes);
    
    //reset
    if ( myMesh->resetTexture == true) {
        getRandomFile(0);
    }
    if ( mMovie && textureType == false ) {
        mTexture = gl::Texture(mMovie->getTexture());
    }
    
    if ( myNextMesh->resetTexture == true) {
        getRandomFile(1);
    }
    if ( mMovie2 && textureType2 == false ) {
        mNextTexture = gl::Texture(mMovie2->getTexture());
    }

    myMesh->update(mousePos, mTexture, mouseClick);
    myNextMesh->update(mousePos, mNextTexture, mouseClick);
    mouseClick = false;
}

void LobbyProjectApp::draw()
{

    // clear the window to black
    gl::clear( Color::black() );
    
    gl::enableDepthRead();
    gl::enableDepthWrite();
 
    if (drawMesh) {
        if ( myMesh->zPct != 1.f ) {
            myMesh->draw();
        }
        if ( myNextMesh->zPct != 1.f ) {
            myNextMesh->draw();
        }
    }

    // draw points over mesh
    if (mPoints) {
        mFrameSubtraction.draw();
    }
    
    if (mShowParams) {
        mParams.draw();
    }
}




void LobbyProjectApp::shutdown(){
    mFrameSubtraction.shutdown();
}

CINDER_APP_NATIVE( LobbyProjectApp, RendererGl )
