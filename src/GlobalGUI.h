#pragma once
#include "ofMain.h"


class GlobalGUI{
    
public:
    GlobalGUI();
        
    
    
    GlobalGUI(int,string,ofColor);
    
    int counter;
    
    ofVec3f elementPos, elementSize;
    
    string elementName;
    
    string drawString;
    
    ofVec3f drawStringPos;
    
    float minVal, maxVal, curVal;
    
    ofColor elementColor, pickColor;
    
    ofVec3f vertices[4];
    int index[6];
    void update();
    
    ofVec3f getWorldToScreen(ofVec3f&, ofCamera&);
    void updateMainMesh(ofVboMesh& mesh_);
};