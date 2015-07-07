#include "ofMain.h"
#include "picoApp.h"
//#include "ofGLProgrammableRenderer.h"

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Usage: %s myID\n", argv[0]);
        return 1;
    }
    myboardID = atoi(argv[1]); /* convert strings to integer */
    printf("myID = %d\n", myboardID);
    
	// ofSetLogLevel(OF_LOG_VERBOSE);
    ofSetLogLevel(OF_LOG_WARNING);
	//ofSetCurrentRenderer(ofGLProgrammableRenderer::TYPE);
	
    /* for res1280x960 measuring ofSetupOpenGL(1280, 960, OF_WINDOW); */
    /* change when measure the HDMI 720p output mode 
     * ofSetupOpenGL(1280, 720, OF_WINDOW); */
    // ofSetupOpenGL(640, 480, OF_WINDOW);
    ofSetupOpenGL(640, 360, OF_WINDOW);
	ofRunApp( new picoApp());
        
}
