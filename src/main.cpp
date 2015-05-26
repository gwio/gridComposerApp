#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
    ofSetupOpenGL(2048*0.7, 1536*0.5,OF_WINDOW);			// <-------- setup the GL context
    
    // this kicks off the running of my app
    // can be OF_WINDOW or OF_FULLSCREEN
    // pass in width and height too:
    ofRunApp(new ofApp());
    
}
