#pragma once
#include "ofMain.h"


class GlobalGUI{
    
public:
    GlobalGUI();
        
    
    
    GlobalGUI(int,string,ofVec3f,ofColor,ofVec3f);
    
    int counter;
    
    ofVec3f elementSize;
    ofVec3f placement;
    string elementName;
    
    string drawString;
    
    ofVec3f drawStringPos;
    
    float minVal, maxVal, curVal;
    float minX,minY,maxX,maxY;
    
    ofColor elementColor, elementColorOff;
    
    ofVec3f vertices[4];
    int index[6];
    void update();
    
    void updateMainMesh(ofVboMesh&, ofVec3f);
    
    bool isInside(ofVec2f);
    bool onOff;
    
    void switchColor(ofVboMesh&);
    void setOn(ofVboMesh&);
    void setOff(ofVboMesh&);
};