//
// By: Kat Sullivan
// June 16, 2015
// Internal Potion Project
//

#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "Cinder-OpenNI.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class MotionTrackingApp : public AppNative {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
};

void MotionTrackingApp::setup()
{
}

void MotionTrackingApp::mouseDown( MouseEvent event )
{
}

void MotionTrackingApp::update()
{
}

void MotionTrackingApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP_NATIVE( MotionTrackingApp, RendererGl )
