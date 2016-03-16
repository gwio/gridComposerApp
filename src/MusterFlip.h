//One Grid Preset, texture and drawing

#pragma once
#include "ofMain.h"

class MusterFlip{
public:
    MusterFlip();
  
    
    void setup(float,int);
    void loadData(bool**,int,int);
    void update();
    void draw(ofVec3f);
    
    void makeTex();
    
    ofTexture texture;
    ofVec3f pos;
    int size,gridTiles;
    
    vector<vector<bool> > layerInfo;


};