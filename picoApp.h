#pragma once

#include "ofMain.h"
#include "ofxOMXPlayer.h"

#include "ConsoleListener.h"

extern "C" {
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
// #include <linux/fb.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#include <sys/ioctl.h>
    
    
#include "fb.h"

}

#define ID_TD1              1
#define ID_TD2              2
#define ID_TD3              3
#define ID_TD4              4
#define ID_TD1W             5

#define TD2                 TRUE
#define NUMBER_OF_QRCODE    8

#define MAX_FRAMES          99
#define BAR_WIDTH           5	// pixels
#define BAR_HEIGHT          200	// pixels
#define BAR_DISTANCE        20	// pixels
#define UPPERLEFT_X         120 // pixels
#define UPPERLEFT_Y         140 // pixels
#define CAPTURE_RATE        1	// seconds

#define NUMBER_OF_POINTS    8
#define NROW                16
#define NCOL                9

#define NR_END              1
#define FREE_ARG char*
#define SIGN(a,b) ((b) >= 0.0 ? fabs(a) : -fabs(a))
static double dmaxarg1,dmaxarg2;
#define DMAX(a,b) (dmaxarg1=(a),dmaxarg2=(b),(dmaxarg1) > (dmaxarg2) ?\
(dmaxarg1) : (dmaxarg2))
static int iminarg1,iminarg2;
#define IMIN(a,b) (iminarg1=(a),iminarg2=(b),(iminarg1) < (iminarg2) ?\
(iminarg1) : (iminarg2))

class picoApp : public ofBaseApp, public SSHKeyListener{
	
public:
	
	void setup();
	void update();
	void draw();
	
	void keyPressed(int key);

	ofxOMXPlayer omxPlayer;
	bool doSaveImage;
	
	ConsoleListener consoleListener;
	void onCharacterReceived(SSHKeyListenerEventData& e);

        void readMatrix(char* filename);
        void readMatrix2(char* filename);

        int  getRightX(int y);
        int  getLeftX(int y);
        int getTopY(int x);
        int getBottomY(int x);
        double getXFade(int x, int y);
        double getYFade(int x, int y);
        void calFading(void);
        
        int syncVideo(int boardID);
        int getHomography(int boardID);
               
	ofTexture pixelOutput;
	bool doUpdatePixels;
        bool startPlayVideo;
        
        int width, height;
        char matrixFN[30];
        int boardID;
        unsigned char fadeRight,fadeDown;

        int xoffset,yoffset;
        double htlx,htly,htrx,htry,hblx,hbly,hbrx,hbry;
        double vtlx,vtly,vtrx,vtry,vblx,vbly,vbrx,vbry;
        double topSlope,bottomSlope,leftSlope,rightSlope,leftSlopeInv,rightSlopeInv;
        static const int WIDTH = 640;
        static const int HEIGHT = 480;
        int yOverlapTop[WIDTH];
        int yOverlapBottom[WIDTH];
        int xOverlapLeft[HEIGHT];
        int xOverlapRight[HEIGHT];
        unsigned char xfadeMat[HEIGHT][WIDTH];
        float matrix[3][3];      
        
        double h1[9];      
        double h1inv[9];
        double h2[9];
        double h2inv[9];
        
        // double **dmatrix(int, int, int, int);
        // double *dvector(int, int);
        // void svdcmp(double **, int, int, double *, double **);
        // END WORKING HERE
        
        pthread_mutex_t mutex;
        double barRate;
        double framePeriod;
        
        float myMatrix[16];
};

