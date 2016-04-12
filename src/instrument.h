//Synth Plane, includes all Meshes and Tonic Presets

#pragma once

#include "ofMain.h"
#include "cube.h"
#include "ofxTonic.h"
#include "interfacePlane.h"
#include "GlobalScales.h"
#include "SynthPresetManager.h"
#include "GlobalGUI.h"
#include "ofxMidi.h"

struct noteLog {
    vector<bool> notes;
    vector<float> volume;
    noteLog() : notes(12, false) , volume(12,0.0)
    {}
    
};

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
    Tonic::ControlParameter release1, release2, attack1, attack2;
    Tonic::ADSR adsr1, adsr2;
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
    Instrument(int, string,int,float,float,int);
    
    void setup(int*, Tonic::ofxTonicSynth *,ofNode,int*,int*, ofxMidiOut*);
    void updateTonicOut();
    void update();
    void draw();
    
    
    void noteTrigger();
    void noteTriggerWest();
    void noteTriggerNorth();
    void noteTriggerEast();
    void noteTriggerSouth();
    
    void blinkNoteInfo(int);
    
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
    void setupOneSynth(cubeGroup&);
    void changePreset(bool);
    void updateGroupInfo(unsigned long,int,int);
    void setTranslate(ofVec3f);
    void setRotate(ofQuaternion);
    void setScale(float);
    void planeMovement(float);
    void loadMuster(vector< vector<bool> >&);
    
    void setSaturationOff();
    void setSaturationOn();
    
    void setAllNotesOff();
    
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
    int *bpmPtr;
    
    string instrumentId;
    void changeSynthVolume(float &);
    
    int gridTiles;
    float gridSize;
    float borderSize;
    
    ofColor innerColorDefault, outerColorDefault, rasterColor;
    vector<GlobalGUI> *guiPtr;
    
    //time
    int *stepperPos;
    
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
    Tonic::RampedValue outputRamp, lowFreqVolFac;
    
    bool synthHasChanged;
    bool userScale;
    
    int timeCounter;
    int pulseDivision;
    int nextPulseDivision;
    deque<float>tickTimes;
    float bpmTick;
    float lastTick;

    int ownSlot;
    int *uiState;
    
    //save played notes
    vector<noteLog> noteHistory;
    noteLog tempLog;
    int historyRows;
    int *globalStatePtr;
    
    bool globalHarmony;
    
    float getLfvf(int&);
    float getSynthRelease(int&);
    float getSynthAttack(int&);
    void setAllADSR(int&);
    
    ofxMidiOut *midiOutPtr;
    int channel;
};