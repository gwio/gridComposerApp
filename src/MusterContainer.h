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
    
    vector<MusterFlip> flips;
    ofVec3f centerPos;
    ofVec2f designGrid;
    
    int flipSize;
    int gridTiles;
    
    vector<ofVec3f>  displayGrid;
    
    int isInside(ofVec2f);

};