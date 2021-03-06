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
    void makeBackTex();
    
    ofTexture texture;
    ofVec3f pos;
    float size,gridTiles;
    
    vector<vector<bool> > layerInfo;


};