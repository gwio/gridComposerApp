//Manager for saving Grid Presets and own Ui

#pragma once
#include "ofMain.h"
#include "MusterFlip.h"




class MusterContainer{
    
    
public:
    MusterContainer();
    MusterContainer(ofVec3f, ofVec2f,int);
    
    void setup();
    void update(ofVec3f);
    void draw();
    
    void setColor(float);
    
    void saveToFlip(int);
    
    vector<MusterFlip> flips;
    ofVec3f centerPos;
    ofVec2f designGrid;
    
    float flipSize;
    int gridTiles;
    
    vector<ofVec3f>  displayGrid;
    
    int isInside(ofVec2f);
    
    bool saveReady;
    
    ofColor displayColor, targetColor;
    ofColor elementColorOn, elementColorOff,elementColorDarker,elementColorTouch, elementColorDarkerTrans;

};