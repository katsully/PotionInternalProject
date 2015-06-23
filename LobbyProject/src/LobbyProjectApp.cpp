#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "FrameSubtraction.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class LobbyProjectApp : public AppNative {
  public:
	void setup();
	void keyDown( KeyEvent event );
	void update();
	void draw();
    
    FrameSubtraction mFrameSubtraction;
};

void LobbyProjectApp::setup(){
    mFrameSubtraction.setup();
}

void LobbyProjectApp::keyDown( KeyEvent event ){
    mFrameSubtraction.keyDown( event );
}

void LobbyProjectApp::update()
{
}

void LobbyProjectApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
    
    mFrameSubtraction.draw();
}

CINDER_APP_NATIVE( LobbyProjectApp, RendererGl )
