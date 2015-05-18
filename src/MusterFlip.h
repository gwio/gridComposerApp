//One Grid Preset

#pragma once
#include "ofMain.h"

class MusterFlip{
public:
    MusterFlip();
  
    
    void setup(int,int);
    void update();
    void draw(ofVec3f);
    
    void makeTex(ofFbo&);
    
    ofTexture texture;
    ofVec3f pos;
    int size,gridTiles;
    
    vector<vector<bool> > layerInfo;


};