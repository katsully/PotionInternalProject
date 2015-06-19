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



class vboMeshTest : public AppBasic {
public:
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
    
    vector<float> timeDiff;
    vector<bool>  isTarget;
    vector<float> timeDiff2;
    vector<bool>  isTarget2;
    
};

void vboMeshTest::setup()
{
    
    setWindowSize(1280, 720);
    //camera setup
    mCamera.setPerspective( 60.0f, getWindowAspectRatio(), 149.0f, 3000.0f );
    //std::cout<<getWindowAspectRatio()<<std::endl;
    mParams = params::InterfaceGl( "MeshTex", Vec2i( 225, 200 ) );
    mParams.addParam( "Rotation", &mSceneRotation );
    mParams.addParam( "Center", &mCenter );
    mParams.addParam( "Eye pos", &mEye );
    mParams.addParam( "map min", &mappingMin );
    mParams.addParam( "map max", &mappingMax );
    mParams.addParam( "vision min", &cameraVisionMin );
    mParams.addParam( "vision max", &cameraVisionMax );

    
    mEye        = Vec3f( 0, 0, 150.0f );
    mCenter     = Vec3f(0, 0, 0);
    mUp         = Vec3f::yAxis();
    mSceneRotation = ci::Quatf(M_PI/2, 0, 0);
    //mappingMin  = 0.05;
    mappingMin  = 0.1;
    mappingMax  = -1.5;
    //cameraVisionMin = 14.0f;
    cameraVisionMin = 147.0f;
    cameraVisionMax = 3000.0f;
    
    // setup the parameters of the Vbo
    int totalVertices = VERTICES_X * VERTICES_Z;
    int totalQuads = ( VERTICES_X - 1 ) * ( VERTICES_Z - 1 );
    gl::VboMesh::Layout layout;
    layout.setStaticIndices();
    layout.setDynamicPositions();
    layout.setStaticTexCoords2d();
    //layout.setDynamicTexCoords2d();
    layout.setDynamicColorsRGBA();
    mVboMesh = gl::VboMesh::create( totalVertices, totalQuads * 4, layout, GL_QUADS );
    
    // buffer static data - the texcoords and the indices
    vector<uint32_t> indices;
    vector<Vec2f> texCoords;
    
    for( int x = 0; x < VERTICES_X; ++x ) {
        for( int z = 0; z < VERTICES_Z; ++z ) {
            
            timeDiff.push_back(0);
            isTarget.push_back(false);
            timeDiff2.push_back(0);
            isTarget2.push_back(false);
            
            // create a quad for each vertex, except for along the bottom and right edges
            if( ( x + 1 < VERTICES_X ) && ( z + 1 < VERTICES_Z ) ) {
                indices.push_back( (x+0) * VERTICES_Z + (z+0) );
                indices.push_back( (x+1) * VERTICES_Z + (z+0) );
                indices.push_back( (x+1) * VERTICES_Z + (z+1) );
                indices.push_back( (x+0) * VERTICES_Z + (z+1) );
                
                
                
//                indices.push_back( (x+0) * VERTICES_Z + (z+0) );
//                indices.push_back( (x+1) * VERTICES_Z + (z+0) );
//                indices.push_back( (x+0) * VERTICES_Z + (z+1) );
//                
                
//                indices.push_back( (x+1) * VERTICES_Z + (z+0) );
//                indices.push_back( (x+1) * VERTICES_Z + (z+1) );
//                indices.push_back( (x+0) * VERTICES_Z + (z+1) );
//                
                
                
            }
            // the texture coordinates are mapped to [0,1.0)
            texCoords.push_back( Vec2f( x / (float)VERTICES_X, z / (float)VERTICES_Z ) );
        }
    }
    
    mVboMesh->bufferIndices( indices );
    mVboMesh->bufferTexCoords2d( 0, texCoords );
    
    // make a second Vbo that uses the statics from the first
    mVboMesh2 = gl::VboMesh::create( totalVertices, totalQuads * 4, mVboMesh->getLayout(), GL_POINTS, &mVboMesh->getIndexVbo(), &mVboMesh->getStaticVbo(), NULL );
    mVboMesh3 = gl::VboMesh::create( totalVertices, totalQuads * 4, mVboMesh->getLayout(), GL_QUADS, &mVboMesh->getIndexVbo(), &mVboMesh->getStaticVbo(), NULL );
    
    //mVboMesh2->setTexCoordOffset( 0, mVboMesh->getTexCoordOffset( 0 ) );
    
    
    
    mTexture = gl::Texture::create( loadImage( loadAsset("unnamed.jpg") ) );
    revealImage = gl::Texture::create(loadImage(loadAsset("future_energy.jpg")));
}

void vboMeshTest::update()
{
    //updateing camera parameters
    mCamera.lookAt(mEye,mCenter,mUp);
              //mCamera.setPerspective( 60.0f, mTexture->getAspectRatio(), 5.0f, 3000.0f );
    mCamera.setPerspective( 60.0f, mTexture->getAspectRatio(), cameraVisionMin, cameraVisionMax );

    gl::setMatrices( mCamera );
    gl::rotate( mSceneRotation );
    
    
    //updating time
    float time = getElapsedSeconds();
    const float timeFreq = 5.0f;
    const float zFreq = 3.0f;
    const float xFreq = 7.0f;
    float offset = getElapsedSeconds() * timeFreq;
    
    
    // dynmaically generate mesh
    gl::VboMesh::VertexIter iter = mVboMesh->mapVertexBuffer();
    for( int x = 0; x < VERTICES_X; ++x ) {
        for( int z = 0; z < VERTICES_Z; ++z ) {
            float height = sin( z / (float)VERTICES_Z * zFreq + x / (float)VERTICES_X * xFreq + offset ) / 5.0f;
            Vec3f position = Vec3f(Vec3f( x / (float)VERTICES_X, height/10000.0f  , z / (float)VERTICES_Z)) ;
     
            
            //calculate relative location to mouse
            Vec2f relPos = Vec2f(lmap<float>(position.x, 0, 1, 0, getWindowWidth()), lmap<float>(position.z, 0, 1, 0, getWindowHeight()));
            Vec2f dist = Vec2f(std::abs(relPos.x - mousePos.x)/1000, std::abs(relPos.y - mousePos.y)/1000);
            if (dist.lengthSquared() < 0.005f) {
                position.y += lmap<float>(dist.lengthSquared(), 0, 1, mappingMin, mappingMax);
            }
            
            
            
            /*---------------//repulsion
             
            Vec2f diff = Vec2f((relPos.x - mousePos.x)*0.1, (relPos.y - mousePos.y)*0.1);
            position.x += diff.normalized().x * 0.1;
            position.z += diff.normalized().y * 0.1;
             */
            
            if (dist.lengthSquared() < 0.01f) {
                position.y += 0.01f;
                isTarget[x * VERTICES_Z + z] = true;
                timeDiff[x * VERTICES_Z + z] = time;
            }
            
            // timer affecting mesh
            if ( (time - timeDiff[x * VERTICES_Z + z] ) < 15.f && isTarget[x * VERTICES_Z + z] == true) {
                position.y += 0.01f + 1/  (time - timeDiff[x * VERTICES_Z + z ] ) * 0.1;
            }else if ((time - timeDiff[x * VERTICES_Z + z]) >= 5.f){
                isTarget[x * VERTICES_Z + z] = false;
            }

            
            //perlin noise
            float nX = relPos.x * 0.002f;
            float nY = relPos.y * 0.002f;
            float nZ = app::getElapsedSeconds() * 0.1f;
            float nZ2 = app::getElapsedSeconds() * 0.1f + 500.f;
            Vec3f v( nX, nY, nZ );
            Vec3f v2( nX, nY, nZ2 );
            float noise = mPerlin.fBm(v);
            float noise2 = mPerlin.fBm(v2);
            
            //position and oscilation
            position -= Vec3f(0.5f + noise * 0.1, 0, 0.5f + noise2 * 0.1);
            iter.setPosition( position);
            
            //set RGBA
            if (dist.lengthSquared() < 0.001f){
                iter.setColorRGBA(ColorA(1.f, 1.f, 1.f, dist.lengthSquared()));
            }else{
                iter.setColorRGBA(ColorA(1.f, 1.f, 1.f, 1.f));
            }
            
            ++iter;
        }
    }
    
    
    gl::VboMesh::VertexIter iter2 = mVboMesh2->mapVertexBuffer();
   
    
    for( int x = 0; x < VERTICES_X; ++x ) {
        for( int z = 0; z < VERTICES_Z; ++z ) {
            float height = sin( z / (float)VERTICES_Z * zFreq + x / (float)VERTICES_X * xFreq + offset ) / 5.0f;
            Vec3f position = Vec3f(Vec3f( x / (float)VERTICES_X, height/100.0f  , z / (float)VERTICES_Z));
            
            
            //get relative position to mouse
            Vec2f relPos = Vec2f(lmap<float>(position.x, 0, 1, 0, getWindowWidth()), lmap<float>(position.z, 0, 1, 0, getWindowHeight()));
            Vec2f dist = Vec2f(std::abs(relPos.x - mousePos.x)/1000, std::abs(relPos.y - mousePos.y)/1000);
            //float offset = lmap<float>(dist.lengthSquared(), 0, 1, mappingMin, mappingMax);
            
        
            
            //activating points
            if (dist.lengthSquared() < 0.01f) {
                position.y += 0.2f;
                isTarget2[x * VERTICES_Z + z] = true;
                timeDiff2[x * VERTICES_Z + z] = time;
            }
            

           
            if ( (time - timeDiff2[x * VERTICES_Z + z] ) < 5.f && isTarget2[x * VERTICES_Z + z] == true) {
                
                position.y += 0.01f + 1/  (time - timeDiff2[x * VERTICES_Z + z] ) * 0.00001;
            }else if ((time - timeDiff2[x * VERTICES_Z + z]) >= 5.f){
                isTarget2[x * VERTICES_Z + z] = false;
            }
     
            
            
            /*------------------- somewhat working mouse solution
            Vec3f relPosition = position.normalized();
            Vec2f diff = Vec2f(std::abs(relPosition.x - mouseRelPos.x) * 10, std::abs(relPosition.z - mouseRelPos.y) * 10);
            float dist = diff.lengthSquared();
            if (dist < 1) {
                position -= Vec3f(0, dist/10, 0);
            }
            std::cout<<diff.y<<std::endl;
             */
            
            //Perlin
            float nX = relPos.x * 0.002f;
            float nY = relPos.y * 0.002f;
            float nZ = app::getElapsedSeconds() * 0.1f;
            float nZ2 = app::getElapsedSeconds() * 0.1f + 500.f;
            Vec3f v( nX, nY, nZ );
            Vec3f v2( nX, nY, nZ2 );
            float noise = mPerlin.fBm(v);
            float noise2 = mPerlin.fBm(v2);
            
            //position and oscilation
             position -= Vec3f(0.5f + noise * 0.1, 0, 0.5f + noise2 * 0.1);
//            position -= Vec3f(0.f + noise * 0.1, 0, 0.5f + noise2 * 0.1);
            iter2.setPosition( position );
            
           // set color of mesh
            if (dist.lengthSquared() < 0.1f){
                //iter2.setColorRGBA(ColorA(1.f, 1.f, 1.f, 1.f));
                iter2.setColorRGBA(ColorA(1.f, 1.f, 1.f, dist.lengthSquared()/2));
            }else{
                iter2.setColorRGBA(ColorA(1.f, 1.f, 1.f, 0.05f));
            }
            
            
            ++iter2;
        }
    }
    
    // dynmaically generate mesh
    gl::VboMesh::VertexIter iter3 = mVboMesh3->mapVertexBuffer();
    for( int x = 0; x < VERTICES_X; ++x ) {
        for( int z = 0; z < VERTICES_Z; ++z ) {
            float height = sin( z / (float)VERTICES_Z * zFreq + x / (float)VERTICES_X * xFreq + offset ) / 5.0f;
            Vec3f position = Vec3f(Vec3f( x / (float)VERTICES_X, height/10000.0f  , z / (float)VERTICES_Z)) ;
            Vec2f relPos = Vec2f(lmap<float>(position.x, 0, 1, 0, getWindowWidth()), lmap<float>(position.z, 0, 1, 0, getWindowHeight()));
         
            
       
            
            
            //perlin noise
            float nX = relPos.x * 0.001f;
            float nY = relPos.y * 0.001f;
            float nZ = app::getElapsedSeconds() * 0.1f + 999.f;
            float nZ2 = app::getElapsedSeconds() * 0.1f + 1500.f;
            Vec3f v( nX, nY, nZ );
            Vec3f v2( nX, nY, nZ2 );
            float noise = mPerlin.fBm(v);
            float noise2 = mPerlin.fBm(v2);
            
            //position and oscilation
            position -= Vec3f(0.5f + noise * 0.01, 0, 0.5f + noise2 * 0.01);
            iter3.setPosition( position);
            
            //set RGBA
            iter3.setColorRGBA(ColorA(1.f, 1.f, 1.f, .5f));
            
            ++iter3;
        }
    }

    
    prevTime = time;

}

void vboMeshTest::mouseMove(MouseEvent event){
    
    mousePos = event.getPos();
    mouseRelPos = mousePos.normalized();
    
   // std::cout<< mouseRelPos<<std::endl;
}

void vboMeshTest::draw()
{
    // this pair of lines is the standard way to clear the screen in OpenGL
    gl::clear( Color( 0.0f, 0.0f, 0.0f ) );
    
    glEnable(GL_BLEND);
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    gl::scale( Vec3f( 180, 180, 180) );
    revealImage->enableAndBind();
    gl::draw(mVboMesh3);
    revealImage->unbind();
    mTexture->enableAndBind();
    gl::draw( mVboMesh );
    mTexture->unbind();
    gl::draw( mVboMesh2 );
    mParams.draw();
    
   
    
}


CINDER_APP_BASIC( vboMeshTest, RendererGl )
