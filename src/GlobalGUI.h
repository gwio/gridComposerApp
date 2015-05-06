#pragma once
#include "ofMain.h"


class GlobalGUI{
    
public:
    GlobalGUI();
        
    
    
    GlobalGUI(int,string,ofVec3f,ofColor,ofVec3f,ofVec3f, int, bool);
    
    int counter;
    
    bool showString;
    bool animation;
    
    ofVec3f curPos;
    ofVec3f elementSize;
    ofVec3f placement;
    ofVec3f offPlacement;
    string elementName;
    
    string drawString;
    
    float sliderWidth;
    
    ofVec3f drawStringPos;
    
    void setStringWidth(float);
    float stringWidth, stringHeight;
    
    int moveDir;
    
    float minVal, maxVal, curVal;
    float minX,minY,maxX,maxY;
    
    ofColor elementColorOn, elementColorOff, elementColorDarker;
    
    ofVec3f vertices[4];
    int index[6];
    void update();
    
    void setColor(float);
    void activateOnColor(ofVboMesh&);
    void activateDarkerColor(ofVboMesh&);

    
    void updateMainMesh(ofVboMesh&, ofVec3f);
    void updateMainMesh(ofVboMesh&, ofVec3f, float&);
    void updateMainMeshSlider(ofVboMesh&, ofVec3f, float, float&);
    
    void setSlider(ofVboMesh&, float);
    
    bool isInside(ofVec2f);
    bool onOff;
    
    void switchColor(ofVboMesh&);
    void setOn(ofVboMesh&);
    void setOff(ofVboMesh&);
    
    bool touchDown;
    ofVec2f touchStart;
    int tempInt;
    
    int fontSize;
};