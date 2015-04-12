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
    bool test;
    
    ofPolyline circlePath;
    
    deque<float> times;
    
    float lastTick;
    float thisTime;
    float len;
    
    float sideRad[4];
    int resolution;
    
    float pctRotate;
    ofMatrix4x4 pulseRot;
};

