#pragma once

#include "ofMain.h"
#include "cube.h"
#include "tapHelper.h"

struct synthInfo {
    bool hasCube;
    bool blocked;
    int cubeVecNum;
    unsigned long cubeGroupId;
    synthInfo(){
        hasCube = false;
        blocked = false;
    }
};

struct cubeGroup {
    
    int size;
    int ownId;
    bool empty;
    ofColor groupColor;
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
    void replaceCube(int,int,float,ofColor);
    void drawDebug();
    void tapEvent(int,int);
    void moveEvent(int,int,float,ofColor);
    void updateCubeMesh();
    void updateFboMesh();
    void updateSoundsMap(int,int);
    //test
    void updateSoundsMap();
    
    int gridTiles;
    float gridSize;
    float borderSize;
    
    
    //synthinfo
    int rCounter,gCounter,bCounter;
    vector<vector<synthInfo> > layerInfo;
    map<unsigned long,cubeGroup> soundsMap;
    unsigned long soundsCounter;
    
    //visual
    vector<ofVec3f> verticesOuter;
    vector<ofVec3f> verticesInner;
    ofVboMesh raster;
    ofVboMesh fboMesh;
    ofVboMesh cubes;
    
    map<int,ofVec2f> cubeMap;
    vector<Cube> cubeVector;
    
    
};