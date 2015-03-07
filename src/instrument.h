#pragma once

#include "ofMain.h"
#include "cube.h"
#include "tapHelper.h"
#include "ofxTonic.h"
#include "interfacePlane.h"


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

    vector< int> y_in_x_elements;
    vector< int> x_in_y_elements;
    Tonic::ofxTonicSynth groupSynth;
    Tonic::Generator output;

    cubeGroup(){

    };
    cubeGroup(int tiles_) {
        lowX = tiles_-1;
        lowY = tiles_-1;
        highX = -1;
        highY = -1;
        size = 0;
        
        y_in_x_elements.resize(tiles_);
        x_in_y_elements.resize(tiles_);
        
    }
};


class Instrument {
    
    
public:
    
    
    Instrument();
    Instrument(string,int,float,float);
    
    void setup(int*, Tonic::ofxTonicSynth *);
    void updateTonicOut();
    void update();
    void draw();
    void drawFbo();
    
    
    void noteTrigger();
    void noteTriggerWest();
    void noteTriggerNorth();
    void noteTriggerEast();
    void noteTriggerSouth();


    void nextDirection();
    
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
    
    string instrumentId;
    
    int gridTiles;
    float gridSize;
    float borderSize;
    
    int *stepperPos;
    int scanDirection;
    
    //active = direction will be triggered by pulse
    // connected = if direction is active, then if connected -> sound, else-> pause
    bool activeDirection[4] = {1,1,1,1};
    bool connectedDirection[4] = {1,1,1,1};

    
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
    float colorHue;
    
    float layerZ;
    float emptyInnerZ;
    float scanZ;
    
    map<int,ofVec2f> cubeMap;
    vector<Cube> cubeVector;
    
    
    //interface planes
    
    vector<InterfacePlane> planes;
    
    //tonic
    Tonic::ofxTonicSynth *mainTonicPtr;
    Tonic::Generator instrumentOut;
    Tonic::RampedValue outputRamp;
       
    bool synthHasChanged;
    
    
    
};