//Orbiting Line Pointer and Direction Toggle

#pragma once
#include "ofMain.h"


class InterfacePlane {
    
    

    public:
    InterfacePlane();
    
    InterfacePlane(int, float,bool[],bool[]);
    
    void update(int&, float&, int&, bool[],bool[], bool&, int&, float&, int&);
    void draw(bool&);
    
    void pulseDir(int);
    
    void setupMeshes(bool[],bool[]);
    
    void animationTransition(float);
    
    void transformButton(bool[],bool[], int&);
    
    void blinkP();
    
    void setColor(float);

    bool animate;
    
    ofVec3f pos;
    ofVec3f tempDir;
    ofNode getRotNode(int&,float&, int&, bool[],bool[]);
    float len(float, int, int&);
    int stepperArg, curStepper, lastStepper;
    
    float tiles;
    int stepCounter;
    int barCounter;
    int scanDir;
    
    
    float alphaPart;
    float lastTick;
    float thisTime;
    float lastTime;
    
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
    
    int dirMeshVerts;

    deque<ofVec3f> lineMeshQA,lineMeshQB;
    
    bool nextDirs[4];
    
    int buttonState[4];
    
    float aniPct[4];
    bool buttonMoving[4];
    ofVec3f moveTarget[4];
    bool blink[4];
    float blinkPct[4];
    int meshState[4];
    bool meshBig;
    ofMesh targetMesh;
    
    ofColor connected;
    ofColor active;
    ofColor offColor;
    ofColor pulseColorA;
    ofColor pulseColorC;
    
    ofColor tempC;
    ofVec3f tempVec;
    
    ofColor trailColor;
    
    float positionMod, scaleMod;
    float positionModDef, positionModTar, scaleModDef, scaleModTar;
    int linePowMod;
    float thisScale;
    int flipCounter;
    
    int activeDirs;
    bool lineWaitForBeat;
};

