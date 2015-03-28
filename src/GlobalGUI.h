#pragma once
#include "ofMain.h"


class GlobalGUI{
    
public:
    GlobalGUI();
        
    
    
    GlobalGUI(int,string,ofVec3f,ofColor,ofVec3f);
    
    int counter;
    
    bool showString;
    
    ofVec3f elementSize;
    ofVec3f placement;
    string elementName;
    
    string drawString;
    
    float sliderWidth;
    
    ofVec3f drawStringPos;
    
    float minVal, maxVal, curVal;
    float minX,minY,maxX,maxY;
    
    ofColor elementColor, elementColorOff;
    
    ofVec3f vertices[4];
    int index[6];
    void update();
    
    void setColor(ofColor);
    
    void updateMainMesh(ofVboMesh&, ofVec3f);
    void updateMainMeshSlider(ofVboMesh&, ofVec3f, float);
    
    bool isInside(ofVec2f);
    bool onOff;
    
    void switchColor(ofVboMesh&);
    void setOn(ofVboMesh&);
    void setOff(ofVboMesh&);
    
};