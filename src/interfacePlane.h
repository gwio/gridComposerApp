#pragma once
#include "ofMain.h"


class InterfacePlane {
    
    
public:
    
    InterfacePlane();
    
    InterfacePlane(int, float);
    
    void update(int&, float&, int&, bool[],bool[]);
    void draw(int&);
    
    void pulseDir(int);

    ofVec3f pos;
    ofVec3f tempDir;
    ofNode posNode;
    
    int tiles;
    int stepCounter;
    int barCounter;
    bool test;
    
    ofPolyline circlePath;
    
    deque<float> times;
    
    float lastTick;
    float thisTime;
    float len;
    
    float tileSize;
    
    float pctBar;
    float linePct;
    ofMatrix4x4 pulseRot;
    
    float lineAlpha;
    
    ofVboMesh lineMesh;
    ofVboMesh pulseLine;
    ofVboMesh directionMesh;
    vector<ofVec3f> lineMeshVertices;
    
    bool nextDirs[4];
    
};

