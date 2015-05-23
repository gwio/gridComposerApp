//Synth Plane, includes all Meshes and Tonic Presets

#pragma once

#include "ofMain.h"
#include "cube.h"
#include "ofxTonic.h"
#include "interfacePlane.h"
#include "GlobalScales.h"
#include "SynthPresetManager.h"


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
    int groupNote;
    
    vector< int> y_in_x_elements;
    vector< int> x_in_y_elements;
    Tonic::ofxTonicSynth groupSynth;
    Tonic::Generator output;
    Tonic::RampedValue rampVol, freqRamp;
    Tonic::ControlGenerator trigger;
    
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
    
    void setup(int*, Tonic::ofxTonicSynth *,ofNode, float *);
    void updateTonicOut();
    void update();
    void draw();
    
    
    void noteTrigger();
    void noteTriggerWest();
    void noteTriggerNorth();
    void noteTriggerEast();
    void noteTriggerSouth();
    
    
    void nextDirection();
    
    void addCube(int,int);
    void removeCube(int,int);
    void replaceCube(int,int,float,ofColor);
    void drawDebug();
    void tapEvent(int,int);
    void moveEvent(int,int,float,ofColor);
    void updateCubeMesh();
    void updateSoundsMap(int,int, bool);
    void resetCubeGroup(unsigned long, int, int);
    void setupOneSynth(cubeGroup*);
    void changePreset(bool);
    void updateGroupInfo(unsigned long,int,int);
    void setTranslate(ofVec3f);
    void setRotate(ofQuaternion);
    void setScale(float);
    void planeMovement(float);
    void setNormals(ofVboMesh&);
    void loadMuster(vector< vector<bool> >&);
    
    void setSaturationOff();
    void setSaturationOn();
    
    
    void getLayerInfo(vector< vector <bool> >&);
    
    SynthPresetManager presetManager;
    
    int getRandomNote();
    
    int keyNote;
    int pitchMod;
    int octaveRange;
    void setKeyNote(int);
    float sVolume;
    Scale activeScale;
    vector<int> scaleNoteSteps;
    void setMusicScale(GlobalScales&, int);
    void changeMusicScale(int);
    void applyNewScale();
    void applyPitchMod(float);
    int currentScaleVecPos;
    float synthAttack;
    
    string instrumentId;
    void changeSynthVolume(float &);
    
    int gridTiles;
    float gridSize;
    float borderSize;
    
    ofColor innerColorDefault, outerColorDefault, rasterColor;
    ofColor filterColor(ofColor);
    
    //time
    int *stepperPos;
    float *bpmTick;
    
    int scanDirection;
    int preset;
    
    //active = direction will be triggered by pulse
    // connected = if direction is active, then if connected -> sound, else-> pause
    bool activeDirection[4] ;
    bool connectedDirection[4] ;
    bool animate;
    bool scaling;
    bool inFocus;
    
    bool pause;
    bool trackSwitchOn;
    //synthinfo
    vector<vector<synthInfo> > layerInfo;
    map<unsigned long,cubeGroup> soundsMap;
    unsigned long soundsCounter;
    
    
    //transformation
    ofNode myNode;
    ofQuaternion myDefault;
    ofQuaternion myTarget;
    ofPolyline aniPath;
    ofPolyline getEmptyPath(ofVec3f);
    float myScaleTarget;
    float myScaleDefault;
    
    
    //visual
    vector<ofVec3f> verticesOuter;
    vector<ofVec3f> verticesInner;
    ofVboMesh raster;
    ofVboMesh cubes;
    float colorHue;
    
    float layerZ;
    float emptyInnerZ;
    float scanZ;
    
    map<int,ofVec2f> cubeMap;
    vector<Cube> cubeVector;
    
    //interface planes
    InterfacePlane pulsePlane;
    //tonic
    Tonic::ofxTonicSynth *mainTonicPtr;
    Tonic::Generator instrumentOut;
    Tonic::RampedValue outputRamp;
    
    bool synthHasChanged;
    bool userScale;
    
    
};