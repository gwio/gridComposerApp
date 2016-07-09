#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
    ofSetupOpenGL(1024*1.25, 768,OF_WINDOW);			// <-------- setup the GL context
    
    // this kicks off the running of my app
    // can be OF_WINDOW or OF_FULLSCREEN
    // pass in width and height too:
    ofRunApp(new ofApp());
    
}

/*

io settings

 ofiOSWindowSettings settings;
 settings.enableRetina = true; // enables retina resolution if the device supports it.
 settings.enableDepth = true; // enables depth buffer for 3d drawing.
 settings.enableAntiAliasing = true; // enables anti-aliasing which smooths out graphics on the screen.
 settings.numOfAntiAliasingSamples = 2; // number of samples used for anti-aliasing.
 settings.enableHardwareOrientation = true; // enables native view orientation.
 settings.enableHardwareOrientationAnimation = false; // enables native orientation changes to be animated.
 settings.glesVersion = OFXIOS_RENDERER_ES1; // type of renderer to use, ES1, ES2, ES3
 //settings.windowMode = OF_FULLSCREEN;
 settings.setupOrientation = OF_ORIENTATION_90_RIGHT;
 ofCreateWindow(settings);

*/