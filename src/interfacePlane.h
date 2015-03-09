#pragma once
#include "ofMain.h"

class InterfacePlane {
    
    
public:
    InterfacePlane();
    
    
    void setup(ofVec3f, float, int, float, ofVboMesh&, ofVboMesh&, ofVboMesh& );
    void update();
    void draw();
    void drawFbo();
    void pulse();
    
    
    ofVboMesh* activeMesh;
    ofVboMesh* fbo_activeMesh;
    ofVboMesh* connectedMesh;
    int direction;
    
    ofVec3f activeMeshON;
    ofVec3f default1, default4;
    
    ofVec3f connectedOn1, connectedOn2, connectedOff1, connectedOff2;
    ofVec3f connectedAni;
    float onPositionPct;
    float connectedAniPct;
    float pctSpeed, connectedPctSpeed;
    float pctTemp, connectedPctTemp;
    
    ofVboMesh fbo_connectedMesh;
    
    ofColor activeFboColor;
    ofColor connectedFboColor;
    
    ofColor activeDrawColor;
    ofColor activeOnColor;
    ofColor activeOffColor;
    
    ofColor connectedDrawColor;
    ofColor connectedOnColor;
    ofColor connectedOffColor;
    
    
    bool active, connected;
    
};

