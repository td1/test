#include "ofMain.h"
#include "picoApp.h"
//#include "ofGLProgrammableRenderer.h"

int main()
{
	ofSetLogLevel(OF_LOG_VERBOSE);
	//ofSetCurrentRenderer(ofGLProgrammableRenderer::TYPE);
	// ofSetupOpenGL(1280, 720, OF_WINDOW);
        ofSetupOpenGL(640, 480, OF_WINDOW);
	ofRunApp( new picoApp());
        
}