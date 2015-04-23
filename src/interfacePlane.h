#pragma once
#include "ofMain.h"


class InterfacePlane {
    
    
public:
    
    InterfacePlane();
    
    InterfacePlane(int, float);
    
    void update(int&, float&, int&, bool[],bool[]);
    void draw();
    
    void pulseDir(int);

    ofVec3f pos;
    ofVec3f tempDir;
    ofNode posNode;
    
    float tiles;
    int stepCounter;
    int barCounter;
    int scanDir;
    
    float alphaPart;
    float lastTick;
    float thisTime;
    float len;
    
    float tileSize;
    
    float pctBar;
    float linePct;
    ofMatrix4x4 pulseRot;
    
    float alpha;
    float lineAlpha;
    ofVec3f alphaPos;
    
    ofVboMesh lineMesh;
    ofVboMesh pulseLine;
    ofVboMesh directionMesh;
    vector<ofVec3f> lineMeshVertices;
    
    bool nextDirs[4];
    
    ofColor connectedDir;
    ofColor notActiveDir;
    ofColor pulseColor;
};

