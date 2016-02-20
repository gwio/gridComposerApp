//Orbiting Line Pointer and Direction Toggle

#pragma once
#include "ofMain.h"


class InterfacePlane {
    
    

    public:
    InterfacePlane();
    
    InterfacePlane(int, float,bool[],bool[]);
    
    void update(int&, float&, int&, bool[],bool[], bool&, int&);
    void draw(bool&);
    
    void pulseDir(int);
    
    void setupMeshes(bool[],bool[]);
    
    void animation(float, int*);
    
    void transformButton(bool[],bool[], int&);

    bool animate;
    
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
    float gridSize;
    
    float pctBar;
    float linePct;
    ofMatrix4x4 pulseRot;
    
    float alpha;
    float lineAlpha;
    ofVec3f alphaPos;
    
    ofVboMesh lineMesh;
    ofVboMesh pulseLine;
    ofVboMesh directionMeshCon;
    ofVboMesh directionMeshAct;
    ofVboMesh directionMeshOff;
    
    ofVboMesh directionMeshConBig;
    ofVboMesh directionMeshActBig;
    ofVboMesh directionMeshOffBig;
    
    ofVboMesh directionMesh;

    vector<ofVec3f> lineMeshVertices;
    
    bool nextDirs[4];
    
    int buttonState[4];
    
    float aniPct[4];
    bool buttonMoving[4];
    ofVec3f moveTarget[4];
    
    ofColor connectedDir;
    ofColor notActiveDir;
    ofColor pulseColor;
    
    ofColor filterColor(ofColor);
    
};

