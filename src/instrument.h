#pragma once

#include "ofMain.h"
#include "cube.h"
#include "tapHelper.h"
#include "ofxTonic.h"



struct synthInfo {
    bool hasCube;
    bool blocked;
    int cubeVecNum;
    unsigned long cubeGroupId;
    synthInfo(){
        hasCube = false;
        blocked = false;
        //cubeGroupId = 0;
    }
};

struct cubeGroup {
    
    int size;
    int ownId;
    ofColor groupColor;
    int lowX, highX;
    int lowY, highY;
    
    Tonic::Generator synth;
    Tonic::RampedValue ramp;
    
    cubeGroup(){
       // size = 0;
    };
    cubeGroup(int tiles_) {
        lowX = tiles_-1;
        lowY = tiles_-1;
        highX = -1;
        highY = -1;
        size = 0;
    }
};


class Instrument {
    
    
public:
    
    
    Instrument();
    Instrument(int,float,float);
    
    void setup(int*);
    void updateTonicOut();
    void update();
    void draw();
    void drawFbo();
    void noteTrigger();
    void generateSynths();
    void addCube(int,int);
    void removeCube(int,int);
    void replaceCube(int,int,float,ofColor);
    void drawDebug();
    void tapEvent(int,int);
    void moveEvent(int,int,float,ofColor);
    void updateCubeMesh();
    void updateFboMesh();
    void updateSoundsMap(int,int, bool);
    void resetCubeGroup(unsigned long, int, int);
    void setupOneSynth(cubeGroup*);
    void updateGroupInfo(unsigned long,int,int);
    
    int gridTiles;
    float gridSize;
    float borderSize;
    
    int *stepperPos;
    
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
    
    //tonic
    Tonic::Generator instrumentOut;
    
};