//
//  cube.cpp
//  sound_table_app
//
//  Created by Christian Gwiozda on 10.02.15.
//
//

#include "cube.h"

Cube::Cube(){
    cubeColor = ofColor::white;

}

Cube::Cube(ofVec3f* ptr0_,ofVec3f* ptr1_,ofVec3f* ptr2_, ofVec3f* ptr3_, int v0_, int v1_, int v2_, int v3_) {
    
    vec0Ptr = ptr0_;
    vec1Ptr = ptr1_;
    vec2Ptr = ptr2_;
    vec3Ptr = ptr3_;
    
    vIndex0 = v0_;
    vIndex1 = v1_;
    vIndex2 = v2_;
    vIndex3 = v3_;
    
   }


void Cube::setup(){
    
}

void Cube::update(){
    
    
}

