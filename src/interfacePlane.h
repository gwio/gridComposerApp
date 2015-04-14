#pragma once
#include "ofMain.h"


class InterfacePlane {
    
    
public:
    
    InterfacePlane();
    
    InterfacePlane(int);
    
    void update(int&, float&);
    void draw();

    ofVec3f pos;
    ofVec3f tempDir;
    ofNode posNode;
    
    int tiles;
    int stepCounter;
    int dirCounter;
    bool test;
    
    ofPolyline circlePath;
    
    deque<float> times;
    
    float lastTick;
    float thisTime;
    float len;
    
    float sideRad[4];
    int resolution;
    
    float pctRotate;
    float linePct;
    ofMatrix4x4 pulseRot;
    
    ofVboMesh lineMesh;
    vector<ofVec3f> lineMeshVerticesTarget;
    vector<ofVec3f> lineMeshVerticesDefault;
};

