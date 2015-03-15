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
    
    void setupPathAndAnmation();
    
    void setupOfxGui();
    
    void updateCamera();
    
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
    ofEasyCam cam;
    ofCamera testCam;
    
   // ofCamera testCam;
    ofNode globalTranslate;
    ofNode camActiveSynth;
    ofNode camNotActiveSynth;
    ofNode synthActivePos;
    float camFov;
    float camActiveFov;
    float camTargetFov;
    float camDefaultFov;
    
    ofQuaternion camQuatDefault, camQuatTarget;
    
    //animation paths
    ofPolyline oneToActive, twoToActive, threeToActive;
    ofPolyline oneToBack, twoToBack, threeToBack;
    ofPolyline camPath;
    ofPolyline camPathBack;
    ofPolyline camUsePath;
    float aniPct;
    float aniCam;
    bool animCam;
    
    //Synthebenen
    int activeSynth;
    vector<Instrument> synths;
    vector<ofNode> synthPos;
    
    //ofx ray testing
    ofRay mouseRay;
    vector<ofPlane> intersecPlane;
    ofVec3f worldMouse, intersectPos;
    int vectorPosX, vectorPosY;
    
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
    
    //interface stuff
    bool focusCam;
    int synthButton[3];
    
    
    bool debugCam;
    
};
