#pragma once

#include "ofMain.h"
#include "ofxRay.h"
#include "instrument.h"

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
    
    bool pointInsideGrid(ofVec3f);
    
    //3d scene
    ofEasyCam cam;
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
    
    
    ofTexture mousePick;
    
    //light
    ofLight light;
    
    
};
