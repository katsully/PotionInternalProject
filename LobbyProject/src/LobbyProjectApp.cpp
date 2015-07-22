#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/Light.h"
#include "cinder/qtime/QuickTime.h"
#include "ShapeDetection.h"
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
   
    int                 meshMinX, meshMinY, meshType, meshResolution;
    float volumeMin;
    float time, timer, timerInterval;
    bool drawMesh;
    bool nextMeshState, switchMesh;
    bool meshReset, meshStart, nextMeshReset, nextMeshStart;
    bool firstMesh;
    bool secondMesh;
    bool textureType, textureType2;
    
    ShapeDetection    mShapeDetection;
    Mesh                *myMesh;
    Mesh                *myNextMesh;
    
    std::ofstream oStream;

//    vector<boost::filesystem::path> mAssetNames;    // list of all asset names
    vector<gl::Texture> mAssets;
    int mCurrentAsset;  // keep track of which asset is being shown
    
    Json::Value mData;  // to store GUI params
    Json::Reader mReader;   // this will read the json file where the gui params are stored and parse it to mData
    Json::StyledStreamWriter writer;    // write json values back to json file
    string mGuiParamsFilePath;   // string representing the path to the json gui params file
    
    bool mFullScreen;   // bool for whether app is fullscreen or not
    bool mShowParams;    // bool for whether gui params are shown
    bool mOnTop;    // bool for whether the window always remains above all other windows
    bool mPoints;   // bool for whether motion tracking points are shown
    bool mCursorHidden;  // bool for whether mouse is showing
    bool mShowDepthCamera;  // bool for whether depth camera is showing, displays surface after removing 'noisy' pixels, and converted to eightBit
    
    int mFrameRate;
};

void LobbyProjectApp::setup()
{
    addAssetDirectory( "../../../../../assets" );
    
    // get absolute path to assets' directory
    fs::path p( getAssetPath( "" ) );
    // iterate through the asset directory and add load all assets as Textures
    for ( fs::directory_iterator it( p ); it != fs::directory_iterator(); ++it ) {
        if ( ! is_directory( *it ) ) {
            // ignore non image files
            if ( it->path().filename() != ".DS_Store" && it->path().filename() != "gui_params.json" ) {
                // if cannot load texture, it will print the error and asset name to the console, then it will increment the asset index counter, and just show the following image
                try {
                    mAssets.push_back(loadImage(loadAsset( it->path().filename() ) ));
                } catch (Exception ex ){
                    std::cout << "exception caught while loading image " << ex.what() << std::endl;
                    std::cout << "asset name " << it->path().filename() << std::endl;
                }
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
    firstMesh       = true;
    secondMesh      = false;
    textureType     = false;
    textureType2    = false;
    meshMinX        = 16;
    meshMinY        = 9;
    meshType        = 0;
    mFrameRate      = getAverageFps();
    mFullScreen     = true;
    mShowParams     = false;
    mOnTop          = true;
    mPoints         = false;
    mCursorHidden   = true;
    switchMesh      = true;
    timer           = 1.0f;
    mCursorHidden    = true;
    mShowDepthCamera = false;
 
    // set app to fullscreen
    setFullScreen(mFullScreen);
    
    // hide mouse cursor
    hideCursor();
    
    gl::enableDepthRead();
    gl::enableDepthWrite();
    
    
//    std::string myPath = "errorlog.txt";
//    oStream.open(myPath);
//    oStream << "error log in here" << std::endl;
    
    
    // get filepath to json file
    mGuiParamsFilePath = p.string() + "/gui_params.json";
    // read the json file
    ifstream ifile(mGuiParamsFilePath, std::ifstream::binary);
    // parse the data to mData
    bool itworked = mReader.parse( ifile, mData, false );
    // if succesful, assign variables values based on the json file
    if (itworked) {
        Json::Value sceneRotParams = mData.get( "mSceneRot", {} );
        mSceneRot       = ci::Quatf( sceneRotParams.get( "xRotation", 0.0f ).asFloat(), sceneRotParams.get( "yRotation", 0.0f ).asFloat(), sceneRotParams.get( "zRotation", 0.0f ).asFloat() );
        volumeMin       = mData.get( "volumeMin", 0.0f ).asFloat();
        drawMesh        = mData.get( "drawMesh", false ).asBool();
        timerInterval   = mData.get( "timerInterval", 0.0f ).asFloat();
        meshResolution  = mData.get( "meshResolution", 0).asInt();
    }
    // close the stream
    ifile.close();
    
    mParams = params::InterfaceGl( "mesh", Vec2i( 400, 220 ) );
    mParams.addParam( "camera rotation", &mSceneRot );
    mParams.addParam( "camera viewing volume min", &volumeMin );
    mParams.addParam( "draw mesh", &drawMesh );
    mParams.addParam( "timer interval", &timerInterval );
    mParams.addParam( "fps", &mFrameRate );
    mParams.addParam( "mesh density", &meshResolution);
    mParams.addParam( "near limit", &mShapeDetection.mNearLimit );
    mParams.addParam( "far limit", &mShapeDetection.mFarLimit );
    mParams.addParam( "minimun threshold", &mShapeDetection.mThresh );
    mParams.addParam( "maximum threshold", &mShapeDetection.mMaxVal );
    
    mShapeDetection.setup( mData );
    myMesh = new Mesh(meshType, firstMesh );
    myNextMesh = new Mesh(meshType, secondMesh );
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
        mOnTop = !mOnTop;
        getWindow()->setAlwaysOnTop(mOnTop);
    } else if ( event.getChar() == 'p' ) {
        mShowParams = !mShowParams;
    } else if( event.getChar() == 'o' ) {
        mPoints = !mPoints;
    } else if( event.getChar() == 'c' ) {
        mShowDepthCamera = !mShowDepthCamera;
    } else if (event.getChar() == 's' ) {
        mShapeDetection.mDrawShapes = !mShapeDetection.mDrawShapes;
    }
}

void LobbyProjectApp::mouseMove( MouseEvent event )
{
    mousePos = event.getPos();
}

void LobbyProjectApp::mouseDown( MouseEvent event )
{
    nextMeshState   = !nextMeshState;
    if (event.isLeft()) {
        switchMesh = true;
    }

    mCursorHidden    = !mCursorHidden;
    if (mCursorHidden) {
        hideCursor();
    } else {
        showCursor();
    }
}

void LobbyProjectApp::getRandomFile(int _meshTag)
{
    // select the next available asset
    gl::Texture asset = mAssets[mCurrentAsset];
    mCurrentAsset++;
    
    // if you went through the entire asset directory, start over
    if ( mCurrentAsset == mAssets.size() ) {
        mCurrentAsset = 0;
    }

    // if it is a movie, load and play the movie
    // CURRENTLY NOT USING MOVIES
//    if( ext == ".mp4" ) {
//        if (_meshTag == 0) {
//            textureType = false;
//            try{
//                mMovie = qtime::MovieGl::create(loadAsset(assetName));
//                
//            } catch( ... ){
//                console() << "file is not a valid movie: " << assetName << std::endl;
//            }
//        }else{
//            textureType2 = false;
//            try{
//                mMovie2 = qtime::MovieGl::create(loadAsset(assetName));
//                
//            } catch( ... ){
//                console() << "file is not a valid movie: " << assetName << std::endl;
//            }
//        }
//        
//        //load movie and play
//        if (_meshTag == 0) {
//            textureType = false;
//            if (mMovie) {
//                mMovie->setLoop();
//                mMovie->play();
//                mMovie->setVolume(0.01f);
//            }
//        }else{
//            textureType2 = false;
//            if (mMovie2) {
//                mMovie2->setLoop();
//                mMovie2->play();
//                mMovie2->setVolume(0.01f);
//            }
//        }
//        // else load it as a texture
//    } else {
    
    if (_meshTag == 0) {
        textureType = true;
        mTexture = asset;
    }
    if (_meshTag == 1) {
        textureType2 = true;
        mNextTexture = asset;
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
        switchMesh = true;
        timer = time;
        
    }
    
    myMesh->getTrackedShapes(mShapeDetection.mTrackedShapes);
    myNextMesh->getTrackedShapes(mShapeDetection.mTrackedShapes);
    
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
    
    //  changing mesh resolution here. if meshResolution is 0, then use base resolution instead
    int meshVetX = meshMinX * meshResolution;
    int meshVetY = meshMinY * meshResolution;
    if (meshResolution > 0){
        myMesh->update(mousePos, mTexture, switchMesh, meshVetX, meshVetY);
        myNextMesh->update(mousePos, mNextTexture, switchMesh, meshVetX, meshVetY);
    }else{
        myMesh->update(mousePos, mTexture, switchMesh, meshMinX, meshMinY);
        myNextMesh->update(mousePos, mNextTexture, switchMesh, meshMinX, meshMinY);
    }

    switchMesh = false;
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
    
    // show depth camera's view
    if(mShowDepthCamera) {
        mShapeDetection.drawSurface();
    }

    // draw points over mesh
    if (mPoints) {
        mShapeDetection.draw();
    }
    
    if (mShowParams) {
        mParams.draw();
    }
}

void LobbyProjectApp::shutdown(){
    // write param values back to json file
    ofstream ofile(mGuiParamsFilePath);
    mData["drawMesh"] = drawMesh;
    // if camera is not successfully connected, don't update these parameters
    if (mShapeDetection.mCameraPresent) {
        // if statements are to ensure no crazy values are written to the json file which will make the camera useless
        if ( mShapeDetection.mFarLimit <= 10000 && mShapeDetection.mFarLimit >= 0 ) {
            mData["mFarLimit"] = mShapeDetection.mFarLimit;
        }
        if ( mShapeDetection.mMaxVal <= 255 && mShapeDetection.mThresh >= 0 ){
            mData["mMaxVal"] = mShapeDetection.mMaxVal;
        }
        if ( mShapeDetection.mNearLimit <= 10000 && mShapeDetection.mNearLimit >= 0 ) {
            mData["mNearLimit"] = mShapeDetection.mNearLimit;
        }
        if ( mShapeDetection.mThresh <= 255 && mShapeDetection.mThresh >= 0) {
            mData["mThresh"] = mShapeDetection.mThresh;
        }
    }
    mData["meshResolution"] = meshResolution;
    mData["timerInterval"] = timerInterval;
    mData["volumeMin"] = volumeMin;
    writer.write(ofile, mData);
    ofile.close();
    
    mShapeDetection.shutdown();
}

CINDER_APP_NATIVE( LobbyProjectApp, RendererGl )
