#pragma once

#include "ofMain.h"


class Cube{
    
    
public:
    
    Cube();
    Cube(ofVec3f*,ofVec3f*,ofVec3f*,ofVec3f*, int,int,int,int);
    
    ofVec3f *vec0Ptr,*vec1Ptr,*vec2Ptr,*vec3Ptr;
    int vIndex0,vIndex1,vIndex2,vIndex3;
    
    float defaultZ;
    float actualZ;
    
    float aniPct;
    float diff;
    float aniFac;
    
    float myTween;
    
    void setup();
    void update();
    
    
    ofColor displayColor;
    
    ofColor cubeColor;
    
    ofColor groupColor;
    
    ofColor scanColor;
    
    //for fbo picking
    ofColor fboColor;
    
    bool active;
    bool tapActive;
    
    bool noSaturation;
    
    bool fastChange, slowChange;
    
    void setColor(ofColor,bool);
    
    void changeGroupColor(ofColor);
    
    void satOn();
    void satOff();
    ofColor tempColor;
    
    float *attack;
    
    void setDefaultHeight(float);
    
};