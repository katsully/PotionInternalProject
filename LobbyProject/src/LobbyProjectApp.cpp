#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class LobbyProjectApp : public AppNative {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
};

void LobbyProjectApp::setup()
{
}

void LobbyProjectApp::mouseDown( MouseEvent event )
{
}

void LobbyProjectApp::update()
{
}

void LobbyProjectApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP_NATIVE( LobbyProjectApp, RendererGl )
