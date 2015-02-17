#pragma once

#include "ofMain.h"
#include "tapHelper.h"


class Cube{
    
    
public:
    
    Cube();
    Cube(ofVec3f*,ofVec3f*,ofVec3f*,ofVec3f*, int,int,int,int);
    
    ofVec3f *vec0Ptr,*vec1Ptr,*vec2Ptr,*vec3Ptr;
    int vIndex0,vIndex1,vIndex2,vIndex3;
    
    float zHeight;
    
    void setup();
    void update();
    
    
    ofColor cubeColor;
    ofColor altColor;
    ofColor pickColor;
    
    
    bool active;
    bool tapActive;
};