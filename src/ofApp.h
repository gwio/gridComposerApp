#pragma once

#include "ofMain.h"
#include "ofxRay.h"
#include "instrument.h"
#include "tapHelper.h"
#include "ofxTonic.h"
#include "ofxGui.h"

using namespace Tonic;


class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    void drawDebug();
    void intersectPlane();
    void updateFboMesh();
    
    void updateTapMap();
    
    void setupOfxGui();
    
    
    bool pointInsideGrid(ofVec3f);
    
    bool drawInfo;
    
    //tonic
    void setupAudio();
    void pulseEvent(float &);
    void audioRequested (float * output, int bufferSize, int nChannels);
    //mainout
    ofxTonicSynth tonicSynth;
    RampedValue volumeRamp;
    Generator mainOut;
    
    //3d scene
    ofCamera cam;
    ofVec3f globalTranslate;
    
    //Synthebenen
    int activeSynth;
    vector<Instrument> synths;
    
    
    //ofx ray testing
    ofRay mouseRay;
    ofPlane intersecPlane;
    ofVec3f worldMouse, intersectPos;
    int vectorPosX, vectorPosY;
    ofPlanePrimitive planeTemp;
    
    //colorpicking grid
    ofVboMesh pickingRaster;
    
    //objects for colorpicking
    ofFbo fbo;
    ofPixels selection;
    ofImage fboImage;
    bool drawFboImage;
    unsigned char RGB[3];
    ofColor lastPickColor;
    
    
    ofTexture mousePick;
    
    //light
    ofLight light;
    
    //misc tap
    unsigned long doubleClickTime, curTap, lastTap;
    map<unsigned long,TapHelper> tapMap;
    bool mouseDragging;
    unsigned long tapCounter;
    
    unsigned long curMouseId;
    
    
    //sound time
     int timeCounter;
    
   //ofx gui elements
    bool showGui;
    ofxPanel gui;
    ofParameter<float> volumeRampValue;
    void volumeRampValueChanged(float&volumeRampValue);
    
    
    //hack for ofxgui panel
    ofFbo guiFbo;
    void updateGuiFbo();
};
