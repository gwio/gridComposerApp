#pragma once
#include "ofMain.h"

class InterfacePlane {
    
    
public:
    InterfacePlane();
    
    
    void setup(ofVec3f, float, int, float );
    void update();
    void draw();
    void drawFbo();
    void pulse();
    
    
    ofVboMesh activeMesh;
    ofVboMesh fbo_activeMesh;
    ofVec3f activeMeshON;
    ofVec3f default1, default4;
    float onPositionPct;
    float pctSpeed;
    float pctTemp;
    
    ofVboMesh connectedMesh;
    ofVboMesh fbo_connectedMesh;
    
    ofColor activeFboColor;
    ofColor connectedFboColor;
    
    ofColor activeDrawColor;
    ofColor activeOnColor;
    ofColor activeOffColor;
    
    ofColor connectedColor;
    
    
    bool active, connected;
    
};

