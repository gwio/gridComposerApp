//
//  cube.cpp
//  sound_table_app
//
//  Created by Christian Gwiozda on 10.02.15.
//
//

#include "cube.h"

#define SPEED 2


Cube::Cube(){
    displayColor = ofColor::white;
    cubeColor = ofColor::white;
    groupColor = ofColor::white;
    scanColor=ofColor::white;
    


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
    
    displayColor = ofColor::white;
    cubeColor = ofColor::white;
    scanColor=ofColor::white;
    groupColor = ofColor::white;
    

}

void Cube::update(){
    if (vec0Ptr->z < defaultZ) {
        vec0Ptr->z += SPEED;
        vec1Ptr->z += SPEED;
        vec2Ptr->z += SPEED;
        vec3Ptr->z += SPEED;

    }
    
    if (vec0Ptr->z > defaultZ) {
        vec0Ptr->z -= SPEED;
        vec1Ptr->z -= SPEED;
        vec2Ptr->z -= SPEED;
        vec3Ptr->z -= SPEED;
    }
    
  
    if (displayColor != cubeColor) {
        displayColor = displayColor.lerp(cubeColor, 0.15);
    }
   
}

  
void Cube::changeGroupColor(ofColor c_) {
    cubeColor = c_;
   // displayColor = c_;
    groupColor = c_;
    scanColor = ofColor::fromHsb(c_.getHue(), c_.getSaturation()-50, c_.getBrightness()+80);
}

void Cube::setColor(ofColor c_){
    cubeColor = c_;
   // displayColor = c_;
}
void Cube::setDefaultHeight(float height_) {
    defaultZ = height_;
}