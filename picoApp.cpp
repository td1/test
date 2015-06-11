#include "picoApp.h"

void picoApp::setup()
{
	ofSetLogLevel(OF_LOG_WARNING);
	ofSetLogLevel("ofThread", OF_LOG_ERROR);
	doSaveImage = false;
	doUpdatePixels = true;
    startPlayVideo = false;
        
#ifdef TD1        
    string videoPath = ofToDataPath("./bunny1.mp4", true);
	boardID = ID_TD1;
    fadeRight = true;
    fadeDown = false;
#elif TD2 
    string videoPath = ofToDataPath("./bunny2.mp4", true);
    boardID = ID_TD2;
    fadeRight = false;
    fadeDown = true;
#elif TD3 
    string videoPath = ofToDataPath("./bunny3.mp4", true);
    boardID = ID_TD3;
    fadeRight = false;
    fadeDown = true;
#elif TD4 
    string videoPath = ofToDataPath("./bunny4.mp4", true);
    boardID = ID_TD4;
    fadeRight = false;
    fadeDown = true;
#else 
    string videoPath = ofToDataPath("./bunny1.mp4", true);
	boardID = ID_TD1W;
    fadeRight = true;
    fadeDown = false;
#endif
    
#ifdef DEBUG_HOMOGRAPHY
videoPath = ofToDataPath("./testpattern.mp4", true);
#endif    
    
    getHomography(boardID);
      
	switch (boardID) {
        case ID_TD1:
            sprintf(matrixFN, "unity.txt"); 
            break;
        case ID_TD2:
            sprintf(matrixFN, "o2h1invh2.txt"); 
            break;
        case ID_TD3:
        case ID_TD4:
        case ID_TD1W:
        default:
            sprintf(matrixFN, "unity.txt"); 
    }
    readMatrix2(matrixFN);

    syncVideo(boardID);
	consoleListener.setup(this);
    ofSetFrameRate(30);

    startPlayVideo = true;
	omxPlayer.loadMovie(videoPath); 
	width = omxPlayer.getWidth();
    height = omxPlayer.getHeight();
    if (width != WIDTH || height != HEIGHT) {
        printf("MOVIE RESOLUTION = %d x %d, DEFAULT = %d x %d\n", width, height, WIDTH, HEIGHT);
    }
    
}

void picoApp::update()
{
    if (doSaveImage ) {
        doSaveImage = false;
        // ofLogVerbose() << " saving image... ";
        omxPlayer.saveImage();
    }
    
    if (doUpdatePixels) {
    	omxPlayer.updatePixels();
        if (!pixelOutput.isAllocated()) {
            pixelOutput.allocate(width, height, GL_RGBA);
        }
    }
}

void picoApp::draw(){
    int i,j,k;
    int var1, var2, nChannels;

    //if(!omxPlayer.isTextureEnabled) {
    //    ofLogVerbose() << " texture is not enabled ";
    //    return;
    // }
    
    // optional to display the original video
    // printf("DRAW RESOLUTION = %d x %d\n", ofGetWidth(), ofGetHeight());
    // omxPlayer.draw(0, 0, ofGetWidth(), ofGetHeight());

    unsigned char *pixels = omxPlayer.getPixels();
    nChannels = 4; // omxPlayer.getPixelsRef().getNumChannels();
   
#ifdef ENABLE_BLENDING    
    // calculate fading factors
    if ((boardID % 2) == 0) { // boardID = 2,4
        for (i=0; i<height; i++) {             
            for (j=0; j<xOverlapRight[i]; j++) {    
                var1 = (i*WIDTH + j)*nChannels;
                for (k=0; k<3; k++) {
                    var2 = pixels[var1+k]*xfadeMat[i][j];
                    pixels[var1+k] = var2/256; 
                }
            }        
        }
    }
    else {
        for (i=0; i<height; i++) {             
            for (j=xOverlapLeft[i]; j<width; j++) {    
                var1 = (i*WIDTH + j)*nChannels;
                for (k=0; k<3; k++) {
                    var2 = pixels[var1+k]*xfadeMat[i][j];
                    pixels[var1+k] = var2/256; 
                }
            }        
        }
    }
#endif

#ifdef HOMOGRAPHY_TRANSFORM_ENABLE
    pixelOutput.loadData(pixels, width, height, GL_RGBA);
    glPushMatrix();
    glMultMatrixf(myMatrix);
    glTranslatef(0,0,0);    
    pixelOutput.draw(0, 0, omxPlayer.getWidth(), omxPlayer.getHeight());
    glPopMatrix();
#else
    pixelOutput.loadData(pixels, width, height, GL_RGBA);
    pixelOutput.draw(0, 0, omxPlayer.getWidth(), omxPlayer.getHeight());
#endif

#if false
    stringstream info;
    info <<"\n" << "p=Pause,f=Play,s=Save" << doUpdatePixels << startPlayVideo << doSaveImage;
    ofDrawBitmapStringHighlight(omxPlayer.getInfo() + info.str(), 60, 60, ofColor(ofColor::black, 90), ofColor::yellow);
#endif
    
}

void picoApp::keyPressed  (int key)
{
    if(key == 's')
    {
        doSaveImage = true;	
    }

    if(key == 'p')
    {
        doUpdatePixels = !doUpdatePixels;	
    }

    if(key == 'f')
    {
        startPlayVideo = true;	
    }
}

void picoApp::onCharacterReceived(SSHKeyListenerEventData& e)
{
    keyPressed((int)e.character);
}

void picoApp::readMatrix2(char* filename)
{
    FILE *matp;
    int i,j;

    matp = fopen(filename, "r");
    if (matp == NULL)
    {
        fputs("Matrix file read error.\n", stderr);
        return;
    }
	
    printf("Matrix 3x3: ");
    for (i=0; i<3; i++) {
	for (j=0; j<3; j++) {
            if (!fscanf(matp, "%f", &matrix[i][j])) {
                fputs("Matrix file read error: 3x3 matrix parameters\n", stderr);
                return;
            }
            else {
                printf("%lf ", matrix[i][j]);
            }
        }
    }            
    printf("\n");
    
    printf("Mapping Matrix 3x3 to Matrix 4x4...\n");
    myMatrix[0] = matrix[0][0]; myMatrix[1] = matrix[1][0]; myMatrix[2] = 0; myMatrix[3] = matrix[2][0];
    myMatrix[4] = matrix[0][1]; myMatrix[5] = matrix[1][1]; myMatrix[6] = 0; myMatrix[7] = matrix[2][1];
    myMatrix[8] = 0;            myMatrix[9] = 0;            myMatrix[10]= 0; myMatrix[11]= 0;
    myMatrix[12]= matrix[0][2]; myMatrix[13]= matrix[1][2]; myMatrix[14]= 0; myMatrix[15]= matrix[2][2];
    
    for (i=0; i<16; i++)
        printf("%lf ", myMatrix[i]);

#ifdef ENABLE_BLENDING
    printf("htlx,htly,hblx,hbly,hbrx,hbry,htrx,htry : \n");
    if (!fscanf(matp, "%lf %lf %lf %lf %lf %lf %lf %lf", &htlx, &htly, &hblx, &hbly, &htrx, &htry, &hbrx, &hbry)) {
        fputs("Matrix file read error: 8 horizontal parameters\n", stderr);
        return;
    }
    printf("%lf %lf %lf %lf %lf %lf %lf %lf \n", htlx, htly, hblx, hbly, htrx, htry, hbrx, hbry);
        
    if (!fscanf(matp, "%lf %lf %lf %lf %lf %lf %lf %lf", &vtlx, &vtly, &vtrx, &vtry, &vblx, &vbly, &vbrx, &vbry)) {
        fputs("Matrix file read error: 8 vertical parameters\n", stderr);
        return;
    }

    topSlope = (vtly-vtry)/(vtlx-vtrx);
    // topSlope = 0.0;
    bottomSlope = (vbly-vbry)/(vblx-vbrx);
    // bottomSlope = 0.0; 
    
    // prevent singularity
    if (htlx == hblx) {
        leftSlope = 100000; 
        leftSlopeInv = 0;
    }
    else {
        leftSlope = (htly-hbly)/ (htlx-hblx);
        leftSlopeInv = (htlx-hblx)/(htly-hbly);
    }
    
    if (htrx == hbrx) {
        rightSlope = 100000;
        rightSlopeInv = 0;
    }
    else {
        rightSlope = (htry-hbry)/(htrx-hbrx);
        rightSlopeInv = (htrx-hbrx)/(htry-hbry);
    }
    
    printf("Top Slope: %lf\n", topSlope);
    printf("Bottom Slope: %lf\n", bottomSlope);
    printf("Left Slope: %lf\n", leftSlope);
    printf("Right Slope: %lf\n", rightSlope);
    printf("Left Slope Inversed: %lf\n", leftSlopeInv);
    printf("Right Slope Inversed: %lf\n", rightSlopeInv);
#endif
    
    fclose(matp);
}

//--------------------------------------------------------------
void picoApp::readMatrix(char* filename)
{
    FILE *matp;
    int i,j;

    matp = fopen(filename, "r");
    if (matp == NULL)
    {
        fputs("Matrix file read error.\n", stderr);
        return;
    }
	
    printf("Matrix 3x3: ");
    for (i=0; i<3; i++) {
	for (j=0; j<3; j++) {
            if (!fscanf(matp, "%f", &matrix[i][j])) {
                fputs("Matrix file read error: 3x3 matrix parameters\n", stderr);
                return;
            }
            else {
                printf("%lf ", matrix[i][j]);
            }
        }
    }            
    printf("\n");
    
    printf("Mapping Matrix 3x3 to Matrix 4x4...\n");
    myMatrix[0] = matrix[0][0]; myMatrix[1] = matrix[1][0]; myMatrix[2] = 0; myMatrix[3] = matrix[2][0];
    myMatrix[4] = matrix[0][1]; myMatrix[5] = matrix[1][1]; myMatrix[6] = 0; myMatrix[7] = matrix[2][1];
    myMatrix[8] = 0;            myMatrix[9] = 0;            myMatrix[10]= 0; myMatrix[11]= 0;
    myMatrix[12]= matrix[0][2]; myMatrix[13]= matrix[1][2]; myMatrix[14]= 0; myMatrix[15]= matrix[2][2];
    
    for (i=0; i<16; i++)
        printf("%lf ", myMatrix[i]);

    printf("htlx,htly,hblx,hbly,hbrx,hbry,htrx,htry : \n");
    if (!fscanf(matp, "%lf %lf %lf %lf %lf %lf %lf %lf", &htlx, &htly, &hblx, &hbly, &htrx, &htry, &hbrx, &hbry)) {
        fputs("Matrix file read error: 8 horizontal parameters\n", stderr);
        return;
    }
    printf("%lf %lf %lf %lf %lf %lf %lf %lf \n", htlx, htly, hblx, hbly, htrx, htry, hbrx, hbry);
        
    if (!fscanf(matp, "%lf %lf %lf %lf %lf %lf %lf %lf", &vtlx, &vtly, &vtrx, &vtry, &vblx, &vbly, &vbrx, &vbry)) {
        fputs("Matrix file read error: 8 vertical parameters\n", stderr);
        return;
    }

    // topSlope = (vtly-vtry)/(vtlx-vtrx);
    topSlope = 0.0;
    // bottomSlope = (vbly-vbry)/(vblx-vbrx);
    bottomSlope = 0.0; 
    
    // prevent singularity
    if (htlx == hblx) {
        leftSlope = 100000; 
        leftSlopeInv = 0;
    }
    else {
        leftSlope = (htly-hbly)/ (htlx-hblx);
        leftSlopeInv = (htlx-hblx)/(htly-hbly);
    }
    
    if (htrx == hbrx) {
        rightSlope = 100000;
        rightSlopeInv = 0;
    }
    else {
        rightSlope = (htry-hbry)/(htrx-hbrx);
        rightSlopeInv = (htrx-hbrx)/(htry-hbry);
    }
    
    printf("Top Slope: %lf\n", topSlope);
    printf("Bottom Slope: %lf\n", bottomSlope);
    printf("Left Slope: %lf\n", leftSlope);
    printf("Right Slope: %lf\n", rightSlope);
    printf("Left Slope Inversed: %lf\n", leftSlopeInv);
    printf("Right Slope Inversed: %lf\n", rightSlopeInv);

    fclose(matp);
}

int picoApp::getRightX(int y)
{
    // xOverlapRight[y] = (y-htry)/(rightSlope)+htrx;
    xOverlapRight[y] = (y-htry)*(rightSlopeInv)+htrx;
    // printf("getRightX: %d\n", xOverlapRight[y]);
    if(xOverlapRight[y] > WIDTH-1) 
	xOverlapRight[y] = WIDTH-1; 
    return xOverlapRight[y];
} 

int picoApp::getLeftX(int y)
{
    // xOverlapLeft[y] = (y-htly)/(leftSlope)+htlx;
    xOverlapLeft[y] = (y-htly)*(leftSlopeInv)+htlx;
    
    // printf("getLeftX: %d\n", xOverlapLeft[y]);
    if(xOverlapLeft[y] < 0)
	xOverlapLeft[y] = 0;
    return xOverlapLeft[y];
}

int picoApp::getTopY(int x)
{
    yOverlapTop[x] = topSlope*(x-vtrx)+vtry;
    if(yOverlapTop[x] < 0)
	yOverlapTop[x] = 0;
    return yOverlapTop[x];
}

int picoApp::getBottomY(int x)
{
    yOverlapBottom[x] = bottomSlope*(x-vbrx)+vbry;
    if(yOverlapBottom[x] > HEIGHT-1)
	yOverlapBottom[x] = HEIGHT-1;
    return yOverlapBottom[x];
}

double picoApp::getXFade(int x, int y)
{
    double x1,x2,result;
    double result2,gamma;

    if (fadeRight)	// Projector 1, 3
    {
        x2 = getRightX(y)-x;
        x1 = x - getLeftX(y);
    }
    else	// Projector 2, 4
    {
        x2 = x-getLeftX(y);
        x1 = getRightX(y) - x;
    }
    // remove adjustment result = ((x2+0.5*x1)/(x2+x1));
    result = x2/(x2+x1);

    #if 1 // turn on option to do gamma correction
    /* add gamma correction */
    gamma = 2.2;
    result2 = pow(result,1/gamma);
    // printf("getXFade: %lf\n", result2);
    return (result2 >= 0 && result2 <= 1)? result2:1;
    #else
    return result;
    #endif
}

double picoApp::getYFade(int x, int y)
{
    double y1,y2,result;
    double gamma, result2;

    if(!fadeDown)	// Projector 1, 2
    {
            y2 = getBottomY(x)-y;
            y1 = y - getTopY(x);
    }
    else	// Projector 3, 4
    {
            y2 = y - getTopY(x);
            y1 = getBottomY(x) - y;
    }
    result = ((y2)/(y2+y1));

    /* add gamma correction */
    gamma = 2.2;
    result2 = pow(result,1/gamma);

    // printf("*** getYFade[%d,%d]=%lf %lf\n", x, y, result, result2);	
    if(result > 0 && result <= 1)
    {
    /*cout << "Pixel: " << x << "," << y << endl;
    cout << "Top bound: " << getTopY(x) << endl;
    cout << "Bottom bound: " << getBottomY(x) << endl;
    cout << "Y1: " << y1 << endl;
    cout << "Y2: " << y2 << endl;
    cout << "Factor: " << result << endl;*/
    }
    return (result >= 0 && result <= 1) ? result2 : 1;
}

/* allocate a double matrix with subscript range m[nrl..nrh][ncl..nch] */
double **dmatrix(int nrl, int nrh, int ncl, int nch)
{
	int i,nrow=nrh-nrl+1,ncol=nch-ncl+1;
	double **m;
	/* allocate pointers to rows */
	m=(double **) malloc((size_t)((nrow+NR_END)*sizeof(double*)));
	m += NR_END;
	m -= nrl;
	/* allocate rows and set pointers to them */
	m[nrl]=(double *) malloc((size_t)((nrow*ncol+NR_END)*sizeof(double)));
	m[nrl] += NR_END;
	m[nrl] -= ncl;
	for(i=nrl+1;i<=nrh;i++) m[i]=m[i-1]+ncol;
	/* return pointer to array of pointers to rows */
	return m;
}

double *dvector(int nl, int nh)
/* allocate a double vector with subscript range v[nl..nh] */
{
	double *v;
	v=(double *)malloc((size_t) ((nh-nl+1+NR_END)*sizeof(double)));
	return v-nl+NR_END;
}

void free_dvector(double *v, int nl, int nh)
/* free a double vector allocated with dvector() */
{
	free((FREE_ARG) (v+nl-NR_END));
}

double pythag(double a, double b)
/* compute (a2 + b2)^1/2 without destructive underflow or overflow */
{
	double absa,absb;
	absa=fabs(a);
	absb=fabs(b);
	if (absa > absb) return absa*sqrt(1.0+(absb/absa)*(absb/absa));
	else return (absb == 0.0 ? 0.0 : absb*sqrt(1.0+(absa/absb)*(absa/absb)));
}

/******************************************************************************/
void svdcmp(double **a, int m, int n, double w[], double **v)
/*******************************************************************************
Given a matrix a[1..m][1..n], this routine computes its singular value
decomposition, A = U.W.VT.  The matrix U replaces a on output.  The diagonal
matrix of singular values W is output as a vector w[1..n].  The matrix V (not
the transpose VT) is output as v[1..n][1..n].
*******************************************************************************/
{
	int flag,i,its,j,jj,k,l,nm;
	double anorm,c,f,g,h,s,scale,x,y,z,*rv1;

	rv1=dvector(1,n);
	g=scale=anorm=0.0; /* Householder reduction to bidiagonal form */
	for (i=1;i<=n;i++) {
		l=i+1;
		rv1[i]=scale*g;
		g=s=scale=0.0;
		if (i <= m) {
			for (k=i;k<=m;k++) scale += fabs(a[k][i]);
			if (scale) {
				for (k=i;k<=m;k++) {
					a[k][i] /= scale;
					s += a[k][i]*a[k][i];
				}
				f=a[i][i];
				g = -SIGN(sqrt(s),f);
				h=f*g-s;
				a[i][i]=f-g;
				for (j=l;j<=n;j++) {
					for (s=0.0,k=i;k<=m;k++) s += a[k][i]*a[k][j];
					f=s/h;
					for (k=i;k<=m;k++) a[k][j] += f*a[k][i];
				}
				for (k=i;k<=m;k++) a[k][i] *= scale;
			}
		}
		w[i]=scale *g;
		g=s=scale=0.0;
		if (i <= m && i != n) {
			for (k=l;k<=n;k++) scale += fabs(a[i][k]);
			if (scale) {
				for (k=l;k<=n;k++) {
					a[i][k] /= scale;
					s += a[i][k]*a[i][k];
				}
				f=a[i][l];
				g = -SIGN(sqrt(s),f);
				h=f*g-s;
				a[i][l]=f-g;
				for (k=l;k<=n;k++) rv1[k]=a[i][k]/h;
				for (j=l;j<=m;j++) {
					for (s=0.0,k=l;k<=n;k++) s += a[j][k]*a[i][k];
					for (k=l;k<=n;k++) a[j][k] += s*rv1[k];
				}
				for (k=l;k<=n;k++) a[i][k] *= scale;
			}
		}
		anorm = DMAX(anorm,(fabs(w[i])+fabs(rv1[i])));
	}
	for (i=n;i>=1;i--) { /* Accumulation of right-hand transformations. */
		if (i < n) {
			if (g) {
				for (j=l;j<=n;j++) /* Double division to avoid possible underflow. */
					v[j][i]=(a[i][j]/a[i][l])/g;
				for (j=l;j<=n;j++) {
					for (s=0.0,k=l;k<=n;k++) s += a[i][k]*v[k][j];
					for (k=l;k<=n;k++) v[k][j] += s*v[k][i];
				}
			}
			for (j=l;j<=n;j++) v[i][j]=v[j][i]=0.0;
		}
		v[i][i]=1.0;
		g=rv1[i];
		l=i;
	}
	for (i=IMIN(m,n);i>=1;i--) { /* Accumulation of left-hand transformations. */
		l=i+1;
		g=w[i];
		for (j=l;j<=n;j++) a[i][j]=0.0;
		if (g) {
			g=1.0/g;
			for (j=l;j<=n;j++) {
				for (s=0.0,k=l;k<=m;k++) s += a[k][i]*a[k][j];
				f=(s/a[i][i])*g;
				for (k=i;k<=m;k++) a[k][j] += f*a[k][i];
			}
			for (j=i;j<=m;j++) a[j][i] *= g;
		} else for (j=i;j<=m;j++) a[j][i]=0.0;
		++a[i][i];
	}
	for (k=n;k>=1;k--) { /* Diagonalization of the bidiagonal form. */
		for (its=1;its<=30;its++) {
			flag=1;
			for (l=k;l>=1;l--) { /* Test for splitting. */
				nm=l-1; /* Note that rv1[1] is always zero. */
				if ((double)(fabs(rv1[l])+anorm) == anorm) {
					flag=0;
					break;
				}
				if ((double)(fabs(w[nm])+anorm) == anorm) break;
			}
			if (flag) {
				c=0.0; /* Cancellation of rv1[l], if l > 1. */
				s=1.0;
				for (i=l;i<=k;i++) {
					f=s*rv1[i];
					rv1[i]=c*rv1[i];
					if ((double)(fabs(f)+anorm) == anorm) break;
					g=w[i];
					h=pythag(f,g);
					w[i]=h;
					h=1.0/h;
					c=g*h;
					s = -f*h;
					for (j=1;j<=m;j++) {
						y=a[j][nm];
						z=a[j][i];
						a[j][nm]=y*c+z*s;
						a[j][i]=z*c-y*s;
					}
				}
			}
			z=w[k];
			if (l == k) { /* Convergence. */
				if (z < 0.0) { /* Singular value is made nonnegative. */
					w[k] = -z;
					for (j=1;j<=n;j++) v[j][k] = -v[j][k];
				}
				break;
			}
			if (its == 30) printf("no convergence in 30 svdcmp iterations");
			x=w[l]; /* Shift from bottom 2-by-2 minor. */
			nm=k-1;
			y=w[nm];
			g=rv1[nm];
			h=rv1[k];
			f=((y-z)*(y+z)+(g-h)*(g+h))/(2.0*h*y);
			g=pythag(f,1.0);
			f=((x-z)*(x+z)+h*((y/(f+SIGN(g,f)))-h))/x;
			c=s=1.0; /* Next QR transformation: */
			for (j=l;j<=nm;j++) {
				i=j+1;
				g=rv1[i];
				y=w[i];
				h=s*g;
				g=c*g;
				z=pythag(f,h);
				rv1[j]=z;
				c=f/z;
				s=h/z;
				f=x*c+g*s;
				g = g*c-x*s;
				h=y*s;
				y *= c;
				for (jj=1;jj<=n;jj++) {
					x=v[jj][j];
					z=v[jj][i];
					v[jj][j]=x*c+z*s;
					v[jj][i]=z*c-x*s;
				}
				z=pythag(f,h);
				w[j]=z; /* Rotation can be arbitrary if z = 0. */
				if (z) {
					z=1.0/z;
					c=f*z;
					s=h*z;
				}
				f=c*g+s*y;
				x=c*y-s*g;
				for (jj=1;jj<=m;jj++) {
					y=a[jj][j];
					z=a[jj][i];
					a[jj][j]=y*c+z*s;
					a[jj][i]=z*c-y*s;
				}
			}
			rv1[l]=0.0;
			rv1[k]=f;
			w[k]=x;
		}
	}
	free_dvector(rv1,1,n);
}

typedef struct s_thdata {
    char myID;
    char shotAnalyzed;
    char time2wait;
} thread_data;

#define MAX_QR  4 // Four QR sets

/* screenCapture for getHomography function */
void *screenCapture(void* ptrData)
{
    FILE *fp;
    char fileToOpen[50], qrstr[40], *numstr, systemCmd[100];
    int i, j, fileSize, sshotNum = 0, tempNum, projectorID, projectorFN, numQR;
    int qrcorner[MAX_QR][10];
    // HUNG1015 int min_counter = 999;
    
    bool doneCaptureQR = false;
    
    thread_data *tdata; // pointer to thread_data
    tdata = (thread_data *)ptrData; // get passing pointer  

    printf("screenCapture QRs start...\n");   

    // not detect all displayed QRs yet (one per each pico set)
    while (doneCaptureQR != true)
    {
        /* start to capture and analyze one frame */
        numQR = 0;
        for (i=0; i<MAX_QR; i++)
            for (j=0; j<10; j++)
                qrcorner[i][j] = 0;

        system("echo 'pause' >/tmp/test.fifo");
        system("echo 'screenshot 0' >/tmp/test.fifo");
        sshotNum++;

        // save screenshot to SDCard
        sprintf(fileToOpen, "shot%04d.png", sshotNum);
        while (1)
        {
            fp = fopen(fileToOpen, "r");
            if (fp)
            {
                fseek(fp, 0, SEEK_END);
                fileSize = ftell(fp);
                if (fileSize > 0) break;
                else fclose(fp);
            }
        }
        fclose(fp);
        // system("./scan_image shot%04d.png > QRcorners", sshotNum);
        // sprintf(systemCmd, "zbarimg shot%04d.png > QRresult", sshotNum);

        // sprintf(systemCmd, "./scan_image sample4qr.png > QRcorners");
        // system(systemCmd);
    
        sprintf(systemCmd, "./scan_image shot%04d.png > QRcorners", sshotNum);
        system(systemCmd);

        /*
        fp = fopen("QRcorners", "r");
        while(!feof(fp))
        {
            if (fgets(qrstr, 20, fp) != NULL)
            {
                numstr = strtok(qrstr, ":");
                numstr = strtok(NULL, ":");
                tempNum = atoi(numstr);
                projectorID = tempNum / 100;
                projectorFN = tempNum % 100;
                // store the bigger frame number for each projector ID
                if (qrcode[projectorID] < projectorFN) 
                    qrcode[projectorID] = projectorFN;
            }
        }
        */
        
        fp = fopen("QRcorners", "r");
        i = j = 0;
        while(!feof(fp))
        {
            if (fgets(qrstr, 50, fp) != NULL)
            {
                // printf("%s\n", qrstr);
                numstr = strtok(qrstr, ":");
                // printf("%s\n", numstr);
                numstr = strtok(NULL, ",");
                while (numstr != NULL) {
                    tempNum = atoi(numstr);
                    // printf("%s\n", numstr);
                    // printf("j = %d, tempNum= %d\n", j, tempNum);

                    if (j == 0) {
                        projectorID = tempNum / 100;
                        projectorFN = tempNum % 100;
                        qrcorner[i][0] = projectorID;
                        qrcorner[i][1] = projectorFN % 2;
                        j = 2;
                    }
                    else {
                        qrcorner[i][j] = tempNum;
                        j = j + 1;
                    }
                    numstr = strtok(NULL, ",");
                } 
                i = i + 1; j = 0; // next QR
            }
        }
        
   
        // count number of projector based on at least one good QR detected for each pico set
        for (i=0; i<MAX_QR; i++)
            if (qrcorner[i][0] > 0) numQR++;
                
        printf("#QRs: %d \n", numQR);
        
        if (numQR >= NUMBER_OF_QRCODE) {
            doneCaptureQR = true;
            
            #ifdef DEBUG_HOMOGRAPHY
                printf("qrcorner= ");
                for (i=0; i<MAX_QR; i++)
                    for (j=0; j<10; j++)
                        printf("%d ", qrcorner[i][j]);
            #endif

        }
    }
    
    /* START CALCULATE HOMOGRAPHY */
    printf("***** Get QR codes, start calculate the homography\n");

    double **a1, **a2, **a1inv, **a2inv;
    double *w;
    double **u,**v;
    double *h;

    double t;
    double t1[NROW+1],t2[NROW+1];
    int k;
    
    /* local, need to put into thread_data */
    /*
    double h1[NCOL+1];      
    double h1inv[NCOL+1];
    double h2[NCOL+1];
    double h2inv[NCOL+1];
    double h1h2inv[NCOL+1];      
    double a[3][3];    
    double b[3][3];    
    double c[3][3]; 
    double offset1[3][3]; 
     */
    
    double **h1,**h1inv,**h2,**h2inv,**tform2,**otform2,**offset2;
    double *htl, *hbl, *hbr, *htr, *vtl, *vtr, *vbl, *vbr;

    h1     = dmatrix(1,3,1,3); // start row,row size,start col,col size 
    h1inv  = dmatrix(1,3,1,3);
    h2     = dmatrix(1,3,1,3);
    h2inv  = dmatrix(1,3,1,3);
    tform2  = dmatrix(1,3,1,3);
    otform2 = dmatrix(1,3,1,3);
    offset2 = dmatrix(1,3,1,3);
    
    htl    = dvector(1,2); 
    hbl    = dvector(1,2);
    hbr    = dvector(1,2);
    htr    = dvector(1,2);

    vtl    = dvector(1,2); 
    vtr    = dvector(1,2);
    vbl    = dvector(1,2);
    vbr    = dvector(1,2);

    // inner corners LEFT MID & RIGHT MID
    const double x1[] = {0,87,87,253,253,387,387,553,553};
    const double y1[] = {0,157,323,323,157,157,323,323,157};
 
    const double x2[] = {0,87,87,253,253,387,387,553,553};
    const double y2[] = {0,157,323,323,157,157,323,323,157};
 
    // detected corners

// #define REFERENCE_CORNERS_TEST    
#ifdef REFERENCE_CORNERS_TEST
    const double X1[] = {0,128,128,211,211,428,428,512,512};
    const double Y1[] = {0,198,282,282,198,198,282,282,198};
    const double X2[] = {0,128,128,211,211,428,428,512,512};
    const double Y2[] = {0,198,282,282,198,198,282,282,198};
#else 
    double X1[] = {0,0,0,0,0,0,0,0,0};
    double Y1[] = {0,0,0,0,0,0,0,0,0};
    double X2[] = {0,0,0,0,0,0,0,0,0};
    double Y2[] = {0,0,0,0,0,0,0,0,0};
    
    /* 1. Get detected corners of set 1 */
    for (i=0; i<MAX_QR; i++) {
        if (qrcorner[i][0] == 1) {
            X1[0] = 1; /* detected valid QR code */
            if (qrcorner[i][1] == 0) {
                printf("\n*** get detected corners of set 1 LEFT \n");
                for (j=1; j<=4; j++) {
                    X1[j] = qrcorner[i][2*j];
                    printf("%lf ", X1[j]);
                    Y1[j] = qrcorner[i][2*j+1];
                    printf("%lf ", Y1[j]);
                }
            }
            else {
                printf("\n*** get detected corners of set 1 RIGHT \n");
                for (j=1; j<=4; j++) {
                    X1[j+4] = qrcorner[i][2*j];
                    printf("%lf ", X1[j+4]);
                    Y1[j+4] = qrcorner[i][2*j+1];
                    printf("%lf ", Y1[j+4]);
                }
            }
        }
        else if (qrcorner[i][0] == 2) {
            X2[0] = 1; /* detected valid QR code */
            if (qrcorner[i][1] == 0) {
                printf("\n*** get detected corners of set 2 LEFT \n");
                for (j=1; j<=4; j++) {
                    X2[j] = qrcorner[i][2*j];
                    printf("%lf ", X2[j]);
                    Y2[j] = qrcorner[i][2*j+1];
                    printf("%lf ", Y2[j]);
                }
            }
            else {
                printf("\n*** get detected corners of set 2 RIGHT \n");
                for (j=1; j<=4; j++) {
                    X2[j+4] = qrcorner[i][2*j];
                    printf("%lf ", X2[j+4]);
                    Y2[j+4] = qrcorner[i][2*j+1];
                    printf("%lf ", Y2[j+4]);
                }
            }
        }
    }
#endif
    
    /* a matrix */
    a1 = dmatrix(1,NROW,1,NCOL); // mxn 
    a2 = dmatrix(1,NROW,1,NCOL); // mxn 
    a1inv = dmatrix(1,NROW,1,NCOL); // mxn 
    a2inv = dmatrix(1,NROW,1,NCOL); // mxn 

    u = dmatrix(1,NROW,1,NCOL); // mxn
    w = dvector(1,NCOL);     // 1xn
    v = dmatrix(1,NCOL,1,NCOL); // nxn
    h = dvector(1,NCOL);     // 1xn

        // calculate a1 for h1
        for (i=1; i<= NUMBER_OF_POINTS; i++) {
            a1[2*i-1][1] = -x1[i];
            a1[2*i-1][2] = -y1[i];
            a1[2*i-1][3] = -1.0;
            a1[2*i-1][4] = 0.0;
            a1[2*i-1][5] = 0.0;
            a1[2*i-1][6] = 0.0;
            a1[2*i-1][7] = x1[i]*X1[i];
            a1[2*i-1][8] = y1[i]*X1[i];
            a1[2*i-1][9] = X1[i];
            
            a1[2*i][1] = 0;
            a1[2*i][2] = 0;
            a1[2*i][3] = 0;
            a1[2*i][4] = -x1[i];
            a1[2*i][5] = -y1[i];
            a1[2*i][6] = -1.0;
            a1[2*i][7] = x1[i]*Y1[i];
            a1[2*i][8] = y1[i]*Y1[i];
            a1[2*i][9] = Y1[i];
        }
    
        // calculate a1inv for h1inv
        for (i=1; i<= NUMBER_OF_POINTS; i++) {
            a1inv[2*i-1][1] = -X1[i];
            a1inv[2*i-1][2] = -Y1[i];
            a1inv[2*i-1][3] = -1.0;
            a1inv[2*i-1][4] = 0.0;
            a1inv[2*i-1][5] = 0.0;
            a1inv[2*i-1][6] = 0.0;
            a1inv[2*i-1][7] = X1[i]*x1[i];
            a1inv[2*i-1][8] = Y1[i]*x1[i];
            a1inv[2*i-1][9] = x1[i];
            
            a1inv[2*i][1] = 0;
            a1inv[2*i][2] = 0;
            a1inv[2*i][3] = 0;
            a1inv[2*i][4] = -X1[i];
            a1inv[2*i][5] = -Y1[i];
            a1inv[2*i][6] = -1.0;
            a1inv[2*i][7] = X1[i]*y1[i];
            a1inv[2*i][8] = Y1[i]*y1[i];
            a1inv[2*i][9] = y1[i];
        }
        
        // calculate a2 for h2
        for (i=1; i<= NUMBER_OF_POINTS; i++) {
            a2[2*i-1][1] = -x2[i];
            a2[2*i-1][2] = -y2[i];
            a2[2*i-1][3] = -1.0;
            a2[2*i-1][4] = 0.0;
            a2[2*i-1][5] = 0.0;
            a2[2*i-1][6] = 0.0;
            a2[2*i-1][7] = x2[i]*X2[i];
            a2[2*i-1][8] = y2[i]*X2[i];
            a2[2*i-1][9] = X2[i];
            
            a2[2*i][1] = 0;
            a2[2*i][2] = 0;
            a2[2*i][3] = 0;
            a2[2*i][4] = -x2[i];
            a2[2*i][5] = -y2[i];
            a2[2*i][6] = -1.0;
            a2[2*i][7] = x2[i]*Y2[i];
            a2[2*i][8] = y2[i]*Y2[i];
            a2[2*i][9] = Y2[i];
        }

        // calculate a2inv for h2inv
        for (i=1; i<= NUMBER_OF_POINTS; i++) {
            a2inv[2*i-1][1] = -X2[i];
            a2inv[2*i-1][2] = -Y2[i];
            a2inv[2*i-1][3] = -1.0;
            a2inv[2*i-1][4] = 0.0;
            a2inv[2*i-1][5] = 0.0;
            a2inv[2*i-1][6] = 0.0;
            a2inv[2*i-1][7] = X2[i]*x2[i];
            a2inv[2*i-1][8] = Y2[i]*x2[i];
            a2inv[2*i-1][9] = x2[i];
            
            a2inv[2*i][1] = 0;
            a2inv[2*i][2] = 0;
            a2inv[2*i][3] = 0;
            a2inv[2*i][4] = -X2[i];
            a2inv[2*i][5] = -Y2[i];
            a2inv[2*i][6] = -1.0;
            a2inv[2*i][7] = X2[i]*y2[i];
            a2inv[2*i][8] = Y2[i]*y2[i];
            a2inv[2*i][9] = y2[i];
        }
    
        printf("\nA1 = \n");
        for (i=1; i<=NROW; i++) {
            for (j=1; j<=NCOL; j++) {
                u[i][j] = a1[i][j];
                printf("%5.0lf ", u[i][j]);    
            }
            printf("\n");
            // printf("%5.0lf %5.0lf %5.0lf %5.0lf %5.0lf %5.0lf %5.0lf %5.0lf %5.0lf\n",u[i][1],u[i][2],u[i][3],u[i][4],u[i][5],u[i][6],u[i][7],u[i][8],u[i][9]);
	}
	svdcmp(u,NROW,NCOL,w,v);
        /* Sort the singular values in descending order */
	for (i=1; i<NCOL; i++) {
            for (j=i+1; j<=NCOL; j++) {
		if (w[i]<w[j]) {
                    t = w[i];
                    w[i] = w[j];
                    w[j] = t;
                    for (k=1; k<=NROW; k++) t1[k] = u[k][i];
                    for (k=1; k<=NCOL; k++) t2[k] = v[k][i];
                    for (k=1; k<=NROW; k++) u[k][i] = u[k][j];
                    for (k=1; k<=NCOL; k++) v[k][i] = v[k][j];
                    for (k=1; k<=NROW; k++) u[k][j] = t1[k];
                    for (k=1; k<=NCOL; k++) v[k][j] = t2[k];
		}
            }
	}
        
        /*
        for (i=1; i<=NCOL; i++) {
            printf("        S[%d]    = %lf\n", i, w[i]);
	}
        for (i=1; i<=NROW; i++) {
            printf("        U[%d]    = %lf %lf %lf %lf %lf %lf %lf %lf %lf\n", i, u[i][1], u[i][2], u[i][3], u[i][4], u[i][5], u[i][6], u[i][7], u[i][8], u[i][9]);
	}
        for (i=1; i<=NCOL; i++) {
            printf("        V[%d]    = %lf %lf %lf %lf %lf %lf %lf %lf %lf\n", i, v[i][1], v[i][2], v[i][3], v[i][4], v[i][5], v[i][6], v[i][7], v[i][8], v[i][9]);
	}
         */
        for (i=1; i<=NCOL; i++) {
            h[i] = v[i][9];
            // printf("        h[%d]    = %lf\n", i, h[i]);
	}
        for (i=1; i<=NCOL; i++) {
            h[i] = h[i]/h[9];
            // printf("        H[%d]    = %lf\n", i, h[i]);
	}

        printf("h1 = ");
        for (i=1; i<=3; i++) {
            for (j=1; j<=3; j++) {
                h1[i][j] = h[j+3*i-3];
                printf("%lf ", h1[i][j]);
            }
	}
        printf("\n");
        
/////////////////////////////////////////////        
        printf("\n A1inv = \n");
        for (i=1; i<=NROW; i++) {
            for (j=1; j<=NCOL; j++) {
                u[i][j] = a1inv[i][j];
                printf("%5.0lf ", u[i][j]);    
            }
            printf("\n");
            // printf("%5.0lf %5.0lf %5.0lf %5.0lf %5.0lf %5.0lf %5.0lf %5.0lf %5.0lf\n",u[i][1],u[i][2],u[i][3],u[i][4],u[i][5],u[i][6],u[i][7],u[i][8],u[i][9]);
	}
	svdcmp(u,NROW,NCOL,w,v);
        
	/* Sort the singular values in descending order */
	for (i=1; i<NCOL; i++) {
            for (j=i+1; j<=NCOL; j++) {
		if (w[i]<w[j]) {
                    t = w[i];
                    w[i] = w[j];
                    w[j] = t;
                    for (k=1; k<=NROW; k++) t1[k] = u[k][i];
                    for (k=1; k<=NCOL; k++) t2[k] = v[k][i];
                    for (k=1; k<=NROW; k++) u[k][i] = u[k][j];
                    for (k=1; k<=NCOL; k++) v[k][i] = v[k][j];
                    for (k=1; k<=NROW; k++) u[k][j] = t1[k];
                    for (k=1; k<=NCOL; k++) v[k][j] = t2[k];
		}
            }
	}
        /*
        for (i=1; i<=NCOL; i++) {
            printf("        S1[%d]    = %lf\n", i, w[i]);
	}
        for (i=1; i<=NROW; i++) {
            printf("        U1[%d]    = %lf %lf %lf %lf %lf %lf %lf %lf %lf\n", i, u[i][1], u[i][2], u[i][3], u[i][4], u[i][5], u[i][6], u[i][7], u[i][8], u[i][9]);
	}
        for (i=1; i<=NCOL; i++) {
            printf("        V1[%d]    = %lf %lf %lf %lf %lf %lf %lf %lf %lf\n", i, v[i][1], v[i][2], v[i][3], v[i][4], v[i][5], v[i][6], v[i][7], v[i][8], v[i][9]);
	}
        */
        for (i=1; i<=NCOL; i++) {
            h[i] = v[i][9];
            // printf("        h[%d]    = %lf\n", i, h[i]);
	}
        for (i=1; i<=NCOL; i++) {
            h[i] = h[i]/h[9];
            // printf("        H[%d]    = %lf\n", i, h[i]);
	}
        printf("h1inv = ");
        for (i=1; i<=3; i++) {
            for (j=1; j<=3; j++) {
                h1inv[i][j] = h[j+3*i-3];
                printf("%lf ", h1inv[i][j]);
            }
	}
        printf("\n");
        
         /* Write to h1inv.txt */
    FILE *matp;
    matp = fopen("h1inv.txt", "w");
    if (matp == NULL) {
        exit -1;
    }
    for (i=1; i<=3; i++) {
        for (j=1; j<=3; j++) {
            fprintf(matp, "%f ", h1inv[i][j]);
        }
    }
    fclose(matp);
    
        
////////////////////////////////////////////////////////
        printf("\n A2 = \n");
        for (i=1; i<=NROW; i++) {
            for (j=1; j<=NCOL; j++) {
                u[i][j] = a2[i][j];
                printf("%5.0lf ", u[i][j]);    
            }
            printf("\n");
            // printf("%5.0lf %5.0lf %5.0lf %5.0lf %5.0lf %5.0lf %5.0lf %5.0lf %5.0lf\n",u[i][1],u[i][2],u[i][3],u[i][4],u[i][5],u[i][6],u[i][7],u[i][8],u[i][9]);
	}
	svdcmp(u,NROW,NCOL,w,v);
	/* Sort the singular values in descending order */
	for (i=1; i<NCOL; i++) {
            for (j=i+1; j<=NCOL; j++) {
		if (w[i]<w[j]) {
                    t = w[i];
                    w[i] = w[j];
                    w[j] = t;
                    for (k=1; k<=NROW; k++) t1[k] = u[k][i];
                    for (k=1; k<=NCOL; k++) t2[k] = v[k][i];
                    for (k=1; k<=NROW; k++) u[k][i] = u[k][j];
                    for (k=1; k<=NCOL; k++) v[k][i] = v[k][j];
                    for (k=1; k<=NROW; k++) u[k][j] = t1[k];
                    for (k=1; k<=NCOL; k++) v[k][j] = t2[k];
		}
            }
	}
        for (i=1; i<=NCOL; i++) {
            h[i] = v[i][9];
            // printf("        h[%d]    = %lf\n", i, h[i]);
	}
        for (i=1; i<=NCOL; i++) {
            h[i] = h[i]/h[9];
            // printf("        H[%d]    = %lf\n", i, h[i]);
	}

        printf("h2 = ");
        for (i=1; i<=3; i++) {
            for (j=1; j<=3; j++) {
                h2[i][j] = h[j+3*i-3];
                printf("%lf ", h2[i][j]);
            }
	}
        printf("\n");
        
/////////////////////////////////////////////        
        printf("\n A2inv = \n");
        for (i=1; i<=NROW; i++) {
            for (j=1; j<=NCOL; j++) {
                u[i][j] = a2inv[i][j];
                printf("%5.0lf ", u[i][j]);    
            }
            printf("\n");
            // printf("%5.0lf %5.0lf %5.0lf %5.0lf %5.0lf %5.0lf %5.0lf %5.0lf %5.0lf\n",u[i][1],u[i][2],u[i][3],u[i][4],u[i][5],u[i][6],u[i][7],u[i][8],u[i][9]);
	}
	svdcmp(u,NROW,NCOL,w,v);
	/* Sort the singular values in descending order */
	for (i=1; i<NCOL; i++) {
            for (j=i+1; j<=NCOL; j++) {
		if (w[i]<w[j]) {
                    t = w[i];
                    w[i] = w[j];
                    w[j] = t;
                    for (k=1; k<=NROW; k++) t1[k] = u[k][i];
                    for (k=1; k<=NCOL; k++) t2[k] = v[k][i];
                    for (k=1; k<=NROW; k++) u[k][i] = u[k][j];
                    for (k=1; k<=NCOL; k++) v[k][i] = v[k][j];
                    for (k=1; k<=NROW; k++) u[k][j] = t1[k];
                    for (k=1; k<=NCOL; k++) v[k][j] = t2[k];
		}
            }
	}
        for (i=1; i<=NCOL; i++) {
            h[i] = v[i][9];
            // printf("        h[%d]    = %lf\n", i, h[i]);
	}
         for (i=1; i<=NCOL; i++) {
            h[i] = h[i]/h[9];
            // printf("        H[%d]    = %lf\n", i, h[i]);
	}
        printf("h2inv = ");
        for (i=1; i<=3; i++) {
            for (j=1; j<=3; j++) {
                h2inv[i][j] = h[j+3*i-3];
                printf("%lf ", h2inv[i][j]);
            }
	}
        printf("\n");

    /* Write to h2inv.txt */
    matp = fopen("h2inv.txt", "w");
    if (matp == NULL) {
        exit -1;
    }
    for (i=1; i<=3; i++) {
        for (j=1; j<=3; j++) {
            fprintf(matp, "%f ", h2inv[i][j]);
        }
    }
    fclose(matp);
    
    /* Write to tform2, h1invh2.txt */
    for (i=1; i<=3; i++) {
        for (j=1; j<=3; j++) {
            tform2[i][j] = 0;
            for (k=1;k<=3;k++) {
                tform2[i][j] = tform2[i][j] + h1inv[i][k]*h2[k][j];
            }
        }
    }
    matp = fopen("h1invh2.txt", "w");
    if (matp == NULL) {
        exit -1;
    }
    for (i=1; i<=3; i++) {
        for (j=1; j<=3; j++) {
            fprintf(matp, "%5.5g ", tform2[i][j]);
        }
    }
    fclose(matp);
    
    /* inversed offset2 */
    for (i=1; i<=3; i++) {
        for (j=1; j<=3; j++) {
            offset2[i][j] = 0;
        }
    }
    offset2[1][1] = offset2[2][2] = offset2[3][3] = 1;
    offset2[1][3] = -560; // HUNG TEST -640 current setting = 560
    
    /* Write to o2h1invh2.txt */
    for (i=1; i<=3; i++) {
        for (j=1; j<=3; j++) {
            otform2[i][j] = 0;
            for (k=1;k<=3;k++) {
                otform2[i][j] = otform2[i][j] + offset2[i][k]*tform2[k][j];
            }
        }
    }
    matp = fopen("o2h1invh2.txt", "w");
    if (matp == NULL) {
        exit -1;
    }
    for (i=1; i<=3; i++) {
        for (j=1; j<=3; j++) {
            fprintf(matp, "%5.5g ", otform2[i][j]);
        }
    }
    fclose(matp);
    
    // HUNG WORKING NEW test with o2h2invh1.txt
    /* Write to tform2, h2invh1.txt */
    for (i=1; i<=3; i++) {
        for (j=1; j<=3; j++) {
            tform2[i][j] = 0;
            for (k=1;k<=3;k++) {
                tform2[i][j] = tform2[i][j] + h2inv[i][k]*h1[k][j];
            }
        }
    }
    matp = fopen("h2invh1.txt", "w");
    if (matp == NULL) {
        exit -1;
    }
    for (i=1; i<=3; i++) {
        for (j=1; j<=3; j++) {
            fprintf(matp, "%5.5g ", tform2[i][j]);
        }
    }
    fclose(matp);
    
    /* inversed offset2 */
    for (i=1; i<=3; i++) {
        for (j=1; j<=3; j++) {
            offset2[i][j] = 0;
        }
    }
    offset2[1][1] = offset2[2][2] = offset2[3][3] = 1;
    offset2[1][3] = 580; // HUNG TEST 640,  current setting = 560
    
    /* Write to o2h2invh1.txt */
    for (i=1; i<=3; i++) {
        for (j=1; j<=3; j++) {
            otform2[i][j] = 0;
            for (k=1;k<=3;k++) {
                otform2[i][j] = otform2[i][j] + offset2[i][k]*tform2[k][j];
            }
        }
    }
    matp = fopen("o2h2invh1.txt", "w");
    if (matp == NULL) {
        exit -1;
    }
    for (i=1; i<=3; i++) {
        for (j=1; j<=3; j++) {
            fprintf(matp, "%5.5g ", otform2[i][j]);
        }
    }
    fclose(matp);
    
    // HUNG TODO 
    // Calculate offset values for blending
    // for (i=1; i<=3; i++) {
    //      offset3[j] = 0;
    // }
    
    // for set 1, need tform2 = h1inv*h2
    // for set 2, need tform1 = h2inv*h1
    // calculate tform1, h2inv*h1 
    /*
    for (i=1; i<=3; i++) {
        for (j=1; j<=3; j++) {
            tform1[i][j] = 0;
            for (k=1;k<=3;k++) {
                tform1[i][j] = tform1[i][j] + h2inv[i][k]*h1[k][j];
            }
        }
    }
    */
    
    /* htl */
    /*
    for (i=1; i<3; i++) {
        htl[i] = 0; 
        for (k=1; k<=3; k++) {
            htl[i] = htl[i] + h2inv[i][k]*h1[k][k];
        }
    }
    */
    
    
    /* END OF HOMOGRAPHY CALCULATION HERE */

    tdata->time2wait = WAIT_AFTER_DONE_GET_HOMOGRAPHY; // WAIT FOR ANOTHER CATCH UP
    
    tdata->shotAnalyzed = 1;
    return tdata;
}

/* screenShot for syncVideo function */
void *screenShot(void* ptrData)
{
    FILE *fp;
    char fileToOpen[50], qrstr[20], *numstr, systemCmd[100];
    int i, numProjector = 0, fileSize, tempWait, sshotNum = 0, tempNum, projectorID, projectorFN;
    int qrcode[MAX_QR];
    int min_counter = 999;

    thread_data *tdata; // pointer to thread_data
    tdata = (thread_data *)ptrData; // get passing pointer  

    printf("screenShot start\n");   

    // not detect all displayed QRs yet (one per each pico set)
    while (numProjector != NUMBER_OF_QRCODE)
    {
        numProjector = 0;
        for (i=0; i<MAX_QR; i++) qrcode[i] = 0;

        system("echo 'pause' >/tmp/test.fifo");
        system("echo 'screenshot 0' >/tmp/test.fifo");
        sshotNum++;

        // save screenshot to SDCard
        sprintf(fileToOpen, "shot%04d.png", sshotNum);
        while (1)
        {
            fp = fopen(fileToOpen, "r");
            if (fp)
            {
                fseek(fp, 0, SEEK_END);
                fileSize = ftell(fp);
                if (fileSize > 0) break;
                else fclose(fp);
            }
        }
        fclose(fp);

        // sprintf(systemCmd, "/home/root/zbar-0.10/zbarimg/zbarimg shot%04d.png > QRresult", sshotNum);
        sprintf(systemCmd, "zbarimg shot%04d.png > QRresult", sshotNum);
        system(systemCmd);
    
        fp = fopen("QRresult", "r");
        while(!feof(fp))
        {
            if (fgets(qrstr, 20, fp) != NULL)
            {
                numstr = strtok(qrstr, ":");
                numstr = strtok(NULL, ":");
                tempNum = atoi(numstr);
                projectorID = tempNum / 100;
                projectorFN = tempNum % 100;
                // store the bigger frame number for each projector ID
                printf("detected pID=%d frame=%d\n", projectorID, projectorFN);
                if (qrcode[projectorID-1] < projectorFN) 
                    qrcode[projectorID-1] = projectorFN;
                
                 // reject homography QR
                if (qrcode[projectorID-1] == 98 || qrcode[projectorID-1] == 99)
                    qrcode[projectorID-1] = 0;
            }
        }

        // count number of projector based on at least one good QR detected for each pico set
        for (i=0; i<MAX_QR; i++)
            if (qrcode[i] > 0) numProjector++;
        
        printf("syncVideo #QR = %d ", numProjector);
        for (i=0; i<MAX_QR; i++)
            // if (qrcode[i] > 0) 
            printf("%d ", qrcode[i]);
        printf("\n");
    }

    // calculate lagging time for my projector
    // find min frame number among projectors
    for (i=0; i<MAX_QR; i++) {
        if (qrcode[i] > 0 && qrcode[i] < min_counter) min_counter = qrcode[i];
    }

    if (min_counter == 999)   
        printf("ERROR: cannot get good qrcode, min_counter=%d\n", min_counter);   
    else
        printf("min frame number: %d\n", min_counter);   

    // calculate time2wait, if min_counter is my frame, my time2wait = 0
    if (tdata->myID == 1) tempWait = qrcode[0] - min_counter;
    else if (tdata->myID == 2) tempWait = qrcode[1] - min_counter;
    else if (tdata->myID == 3) tempWait = qrcode[2] - min_counter;
    else if (tdata->myID == 4) tempWait = qrcode[3] - min_counter;
  
    if (tempWait < 0) {
        tdata->time2wait = 0;
    }
    else {
      tdata->time2wait = tempWait;
    }
    printf(">>>>>>>> my ID:%d, time2wait: %d\n", tdata->myID, tempWait);   
    tdata->shotAnalyzed = 1;
    return tdata;
}

#define WAIT_FOR_ALL_PICO_SENDING_QR    5 // 30

int picoApp::getHomography(int BoardID)
{

    int fbfd = 0;
    int loopNum = 0;
    int synch = 0;
    int numBars = -1;

    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;

    long int screensize = 0;
    char *fbp = 0;
    int x, y;
    int shift;
    long int location = 0;
    char fileToOpen[30];
    struct timeval tv;
    double prevBarTime;
    FILE *fp;
    const char *fifoname = "/tmp/test.fifo";

    unsigned char red, green, blue, tookShot = 0;
    unsigned char *video_frame; 
    unsigned char *pixel_ptr;
    video_frame = (unsigned char*)malloc(640*480*3);

    // HUNG mutex = PTHREAD_MUTEX_INITIALIZER;
    barRate = 0.2;
    framePeriod = 0.2;

    pthread_t thread2;
    thread_data thdata2; // instantiate passing thread data

    thdata2.myID = boardID;
    thdata2.shotAnalyzed = 0;
    thdata2.time2wait = 0;

    system("rm -f shot*");
    system("echo -e '\\033[?17;0;0c' > /dev/tty1");	// Disable cursor
    system("echo -e \\\\x1b[9\\;0] > /dev/tty1");		// Disable screensaver
    
    if (access (fifoname, F_OK))
        system("mkfifo /tmp/test.fifo");

    system("mplayer -nolirc -slave -quiet -input file=/tmp/test.fifo -vo null -vf screenshot -tv driver=v4l2 tv:// &");

    // Open the file for reading and writing
    fbfd = open("/dev/fb0", O_RDWR);
    if (!fbfd) {
        printf("Error: cannot open framebuffer device.\n");
        return 0;
    }
    printf("The framebuffer device was opened successfully.\n");

    // Get fixed screen information
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo)) {
        printf("Error reading fixed information.\n");
        return 0;
    }

    // Get variable screen information */
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
        printf("Error reading variable information.\n");
        return 0;
    }

    // Figure out the size of the screen in bytes
    screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;
    printf("screensize = %ld\n", screensize);
    printf("vinfo.xres = %d\n", vinfo.xres);
    printf("vinfo.yres = %d\n", vinfo.yres);
    printf("vinfo.bits_per_pixel = %d\n", vinfo.bits_per_pixel);

    /* Map the device to memory */
    fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0); 

    if ((int)fbp == -1) {
        printf("Error: failed to map framebuffer device to memory.\n");
        return 0;
    }
    // printf("The framebuffer device was mapped to memory successfully.\n");

    gettimeofday(&tv, NULL);
    prevBarTime = (double)tv.tv_sec + (0.000001 * tv.tv_usec);

    while (1)
    {
        // STOP HERE UNTIL barRate
        gettimeofday(&tv, NULL);
        if ((double)tv.tv_sec + (0.000001 * tv.tv_usec) - prevBarTime < barRate)	continue; 

        numBars++;
        loopNum++;
        // printf("BAR%dLOOP%d ", numBars, loopNum);

        // clear screen
        if ((loopNum == 1) || ((numBars == MAX_FRAMES) && (!synch)))
        {
            for (y=0; y<479; y++)
            for (x=0; x<640; x++)
            {
                location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +
                           (y+vinfo.yoffset) * finfo.line_length;
                *((unsigned short int*)(fbp + location)) = 0;
            }
            numBars = 1;
        }

        // keep sending out QRs
        // upperleft_x = UPPERLEFT_X + (BAR_WIDTH + BAR_DISTANCE) * (numBars - 1);
        
        // keep sending out QRs sequentially, not sync yet  
        // HUNG TODO need to add a fixed time to wait for other sets
        // fixed delay time when wait=0
        // if (synch == 0 || (loopNum < MAX_FRAMES)) {
        // if (synch == 0) {
        // HUNG WORKING NEW to wait after done getHomography
        if (synch == 0 || thdata2.time2wait > 0)
        {
            if (thdata2.time2wait) 
            {
                printf("getHomography done...wait %f\n", thdata2.time2wait);
                thdata2.time2wait--;
            }

            // sprintf(fileToOpen, "../../video/qrblob/QR%03d.rgb", numBars + thdata2.myID * 100);
            sprintf(fileToOpen, "../../video/qrblob/QR%03d.rgb", (numBars%2) + 98 + thdata2.myID * 100);
            // printf("sending QR#%d\n", (numBars%2) + 98 + thdata2.myID * 100);
            
            fp = fopen(fileToOpen, "r");
            fread(video_frame, 1, 640*480*3, fp);
            pixel_ptr = video_frame;

            // shifted positions for left and right QRs              
            if (numBars % 2 == 0) shift = -150;
            else shift = 150;

            // send a QR by updating pixels in the frame buffer
            for (y=125; y<355; y++) {
                for (x=205+shift; x<435+shift; x++) {
                    red = *pixel_ptr++;
                    green = *pixel_ptr++;
                    blue = *pixel_ptr++;
                   
                    // remove brightness reduction 
                    // red /= 3;
                    // green /= 3;
                    // blue /= 3;
                    
                    location =  (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +
                                (y+vinfo.yoffset) * finfo.line_length;
                    *((unsigned short int*)(fbp + location)) = ((red>>3)<<11)|((green>>2)<<5)|(blue>>3);
                }   
            }
        }
        else { // synch=1, finish get matrix                
            printf("*** Done screenCapture...\n");
            break;
        }
        
        // create thread1 to take screenshot of QRs, adjust WAIT_FOR_ALL_PICO_SENDING_QR time to have all pico sets sending their QRs 
        if (loopNum > WAIT_FOR_ALL_PICO_SENDING_QR && synch == 0)
        {
            // Take screenshots to analyze and sync
            if (tookShot == 0)
            {
                printf("*** screenCapture thread...\n");
                pthread_create(&thread2, NULL, &screenCapture, &thdata2);
                tookShot = 1;
            }

            if (thdata2.shotAnalyzed) {
                printf("*** get return from screenCapture...\n");
                synch = 1;
                // break; HUNG TEST // should we break here after finished
            }
        }
        
        prevBarTime = (double)tv.tv_sec + (0.000001 * tv.tv_usec);
    } // end while loop

    system("echo 'stop' > /tmp/test.fifo");
    pthread_join(thread2, NULL);
    munmap(fbp, screensize);
    close(fbfd);
    printf("DONE GET HOMOGRAPHY...\n");
    // HUNG TODO check out pthread_exit(NULL);
    return 1;
}

int picoApp::syncVideo(int BoardID)
{

    int fbfd = 0;
    int loopNum = 0;
    int sync = 0;
    int upperleft_x;
    int numBars = -1;
    int newtime2wait = 0;

    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;

    long int screensize = 0;
    char *fbp = 0, convertCmd[200];
    int x, y, imageCounter, fileSize, shift;
    long int location = 0;
    char fileToOpen[30];
    struct timeval tv;
    double prevBarTime;
    FILE *fp;
    const char *fifoname = "/tmp/test.fifo";

    unsigned char red, green, blue, tookShot = 0;
    unsigned char *video_frame = (unsigned char*)malloc(640*480*3);
    unsigned char *pixel_ptr;
    
    // HUNG mutex = PTHREAD_MUTEX_INITIALIZER;
    barRate = 0.2;
    framePeriod = 0.2;

    pthread_t thread1;
    thread_data thdata1; // instantiate passing thread data

    thdata1.myID = boardID;
    thdata1.shotAnalyzed = 0;
    thdata1.time2wait = 0;

    system("rm -f shot*");
    system("echo -e '\\033[?17;0;0c' > /dev/tty1");	// Disable cursor
    system("echo -e \\\\x1b[9\\;0] > /dev/tty1");		// Disable screensaver
    
    if (access (fifoname, F_OK))
        system("mkfifo /tmp/test.fifo");

    system("mplayer -nolirc -slave -quiet -input file=/tmp/test.fifo -vo null -vf screenshot -tv driver=v4l2 tv:// &");
    
    // Open the file for reading and writing
    fbfd = open("/dev/fb0", O_RDWR);
    if (!fbfd) {
        printf("Error: cannot open framebuffer device.\n");
        return 0;
    }
    printf("The framebuffer device was opened successfully.\n");

    // Get fixed screen information
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo)) {
        printf("Error reading fixed information.\n");
        return 0;
    }

    // Get variable screen information */
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
        printf("Error reading variable information.\n");
        return 0;
    }

    // Figure out the size of the screen in bytes
    screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;
    printf("screensize = %ld\n", screensize);
    printf("vinfo.xres = %d\n", vinfo.xres);
    printf("vinfo.yres = %d\n", vinfo.yres);
    printf("vinfo.bits_per_pixel = %d\n", vinfo.bits_per_pixel);

    /* Map the device to memory */
    fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0); 

    if ((int)fbp == -1) {
        printf("Error: failed to map frame buffer device to memory.\n");
        return 0;
    }
    printf("The framebuffer device was mapped to memory successfully.\n");

    gettimeofday(&tv, NULL);
    prevBarTime = (double)tv.tv_sec + (0.000001 * tv.tv_usec);

    while (1)
    {
        /* STEP6: done syncVideo, playing video now */
        if (startPlayVideo == true) {
            printf(">>>>>>>>>>>> start play video = %d\n", startPlayVideo);
            break;
        } 

        gettimeofday(&tv, NULL);
        if ((double)tv.tv_sec + (0.000001 * tv.tv_usec) - prevBarTime < barRate)	continue; 

        /* STEP3: done detected QRs, start wait time to sync */
        if (sync == 0 && thdata1.shotAnalyzed)
        {
            sync = 1; 
            loopNum = MAX_FRAMES - MIN_FRAME_DELAY - thdata1.time2wait; 
            newtime2wait = MAX_FRAMES - loopNum;
            printf("time2wait = %d\n", newtime2wait);
        }
        
        /* STEP4: continue sending out event sync = 1 */
        if (sync == 1) {
            newtime2wait = MAX_FRAMES - loopNum;
            printf("time2wait = %d\n", newtime2wait);
        }
        
        numBars++;
        loopNum++;

        // clear screen
        if ((loopNum == 1) || ((numBars == MAX_FRAMES) && (!sync)))
        {
            printf("clear screen at beginning\n");
            for (y=0; y<479; y++)
            for (x=0; x<640; x++)
            {
                location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +
                           (y+vinfo.yoffset) * finfo.line_length;
                *((unsigned short int*)(fbp + location)) = 0;
            }
            numBars = 1;
        }

        upperleft_x = UPPERLEFT_X + (BAR_WIDTH + BAR_DISTANCE) * (numBars - 1);
        
        /* STEP1: keep sending out QRs,
                  even after sync, until reach MAX_FRAMES */
        if (sync == 0 || (loopNum < MAX_FRAMES)) {
            sprintf(fileToOpen, "../../video/qrblob/QR%03d.rgb", numBars + thdata1.myID * 100);
            fp = fopen(fileToOpen, "r");
            fread(video_frame, 1, 640*480*3, fp);
            pixel_ptr = video_frame;

            /* clear screen */
            for (y=0; y<479; y++) {
                for (x=0; x<640; x++) {
                    location =  (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +
                                (y+vinfo.yoffset) * finfo.line_length;
                    *((unsigned short int*)(fbp + location)) = 0;
                }
            }

            // shifted positions for left and right QRs              
            if (numBars % 2 == 0) shift = -150;
            //else if (numBars % 3 == 1) shift = 0;
            else shift = 150;
            //shift = 0;

            // send a QR by updating pixels in the frame buffer
            for (y=125; y<355; y++) {
                for (x=205+shift; x<435+shift; x++) {
                    red = *pixel_ptr++;
                    green = *pixel_ptr++;
                    blue = *pixel_ptr++;

                    // remove brightness reduction 
                    // red /= 3;
                    // green /= 3;
                    // blue /= 3;

                    location =  (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +
                                (y+vinfo.yoffset) * finfo.line_length;
                    *((unsigned short int*)(fbp + location)) = ((red>>3)<<11)|((green>>2)<<5)|(blue>>3);
                }   
            }
            
        }
        /* STEP5: REACH MAX FRAMES AND sync = 1 => OK to start playing video */ 
        else { // wait=0, sync=1, start playing now                
            printf("wait=%d, sync=%d, start playing video now...\n", newtime2wait, sync);
            startPlayVideo = true;
        }

        /* STEP2: start after a while and wait for shotAnalyzed then skip with sync = 1
           create thread1 to take screenshot of QRs, adjust WAIT_FOR_ALL_PICO_SENDING_QR time to have all pico sets sending their QRs 
         */ 
        if (loopNum > WAIT_FOR_ALL_PICO_SENDING_QR && thdata1.time2wait == 0 && sync == 0) {
            // Take screenshots to analyze and sync
            if (tookShot == 0) {
                pthread_create(&thread1, NULL, &screenShot, &thdata1);
                tookShot = 1;
            }
        }
        
        prevBarTime = (double)tv.tv_sec + (0.000001 * tv.tv_usec);
    } // end while loop

    system("echo 'stop' > /tmp/test.fifo");
    pthread_join(thread1, NULL);
    munmap(fbp, screensize);
    close(fbfd);
    return 0;
}

void picoApp::calFading(void)
{
    int i,j,k,x,y;
    double w,xfade,yfade;

    for (i=0; i<HEIGHT; i++) {             
        for (j=0; j<WIDTH; j++) {    
            w = matrix[2][0] * j + matrix[2][1] * i + matrix[2][2];
            x = (int)((matrix[0][0] * j + matrix[0][1] * i + matrix[0][2])/w);
            y = (int)((matrix[1][0] * j + matrix[1][1] * i + matrix[1][2])/w);
            // printf(" p[%d %d]=%d %d d% ",i,j,x,y,w);

            if (x >= 0 && x < WIDTH && y > 0 && y < HEIGHT) {
                if (j >= getLeftX(i) && j <= getRightX(i)) {
                    xfade = getXFade(j,i);
                    if (xfade < 1) 
                        xfadeMat[i][j] = xfade*256;
                    else
                        xfadeMat[i][j] = 255;
                    // printf("xfadeMat[%d %d]=%d", i,j,xfadeMat[i][j]);
                }
            }
        }
    }
}

