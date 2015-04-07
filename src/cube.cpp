
#include "cube.h"

#define SPEED 0.065


Cube::Cube(){
    displayColor =ofColor( 1, 55, 65,0);
    cubeColor = ofColor( 1, 55, 65,0);
    groupColor = ofColor( 1, 55, 65,0);
    scanColor=ofColor( 11, 55, 65,0);
    
    aniPct = 0.0;
    diff = 0.0;
    aniFac = 0.0;
    
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
    
    displayColor =ofColor( 11, 55, 65,0);
    cubeColor = ofColor( 11, 55, 65,0);
    scanColor=ofColor( 1, 55, 65,0);
    groupColor = ofColor( 11, 55, 65,0);
    
    
}

void Cube::update(){
    if (defaultZ != vec0Ptr->z) {
        
        if (diff > 0) {
            aniFac = ofClamp(1-pow(1-aniPct,2),0.0,1.0);
            
            
        }
        
        if (diff < 0) {
            aniFac = ofClamp(pow(aniPct,4),0.0,1.0);
            
        }
        
        // cout << aniFac << endl;
        vec0Ptr->z = actualZ + (diff*aniFac);
        vec1Ptr->z = actualZ + (diff*aniFac);
        vec2Ptr->z = actualZ + (diff*aniFac);
        vec3Ptr->z = actualZ + (diff*aniFac);
        
        aniPct+=SPEED;
    }
    
    
    
    
    if (displayColor != cubeColor) {
        displayColor = displayColor.lerp(cubeColor, 0.11);
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
    diff = height_-vec0Ptr->z;
    actualZ = vec0Ptr->z;
    defaultZ = height_;
    aniPct = 0.0;
}