#pragma once

#include "ofMain.h"
#include "cube.h"

struct synthInfo {
    bool hasCube;
    int cubeVecNum;
    synthInfo(){
        hasCube = false;
    }
};


class Instrument {
    
    
public:
    
    
    Instrument();
    Instrument(int,float,float);
    
    void setup();
    void update();
    void draw();
    void drawFbo();
    void play();
    void addCube(int,int);
    void removeCube(int,int);
    void drawDebug();
    void clickEvent(int,int);
    void updateCubeMesh();
    void updateFboMesh();
    
    int gridTiles;
    float gridSize;
    float borderSize;
    
    
    //synthinfo
    int rCounter,gCounter,bCounter;
    vector<vector<synthInfo> > layerInfo;
    
    //visual
    vector<ofVec3f> verticesOuter;
    vector<ofVec3f> verticesInner;
    ofVboMesh raster;
    ofVboMesh fboMesh;
    ofVboMesh cubes;
    
    map<int,ofVec2f> cubeMap;
    vector<Cube> cubeVector;
    
    
};