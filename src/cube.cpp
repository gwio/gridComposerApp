
#include "cube.h"

#define SPEED 0.065


Cube::Cube(){
    displayColor = ( ofColor( 0,0,0,255) );
    cubeColor =  (ofColor( 0, 0, 0,255));
    groupColor = (ofColor( 222, 222, 222,255));
    scanColor= (ofColor( 11, 55, 65,255));
    
    aniPct = 0.0;
    diff = 0.0;
    aniFac = 0.0;
    myTween = 1.0;
    
    noSaturation = false;
    
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
    noSaturation = false;
    
}


void Cube::setup(){
    
    displayColor = ( ofColor( 0,0,0,255) );
    cubeColor =  (ofColor( 0, 0, 0,255));
    groupColor = (ofColor( 1, 55, 65,255));
    scanColor= (ofColor( 11, 55, 65,255));
}

void Cube::update(){
    if (defaultZ != vec0Ptr->z) {
        
        if (diff > 0) {
            aniFac = ofClamp(1-pow(1-aniPct, 2),0.0,1.0);
            
            aniPct+= (0.1* (5-*pulseDivPtr) );
            
        }
        
        if (diff < 0) {
            aniFac = ofClamp(pow(aniPct,4),0.0,1.0);
            aniPct += (0.06* (5-*pulseDivPtr)) ;
        }
        
        // cout << aniFac << endl;
        vec0Ptr->z = actualZ + (diff*aniFac);
        vec1Ptr->z = actualZ + (diff*aniFac);
        vec2Ptr->z = actualZ + (diff*aniFac);
        vec3Ptr->z = actualZ + (diff*aniFac);
        
        // aniPct+=SPEED;
        // aniPct+= ofClamp( ofMap(*attack, 0.001, 0.01, 0.95, 0.1), 1.0, 0.1);
    }
    
    
    if (slowChange) {
        if (myTween < 1.0) {
            myTween = (myTween*1.0008)+ (0.002 * (5-*pulseDivPtr));
            if (displayColor != cubeColor) {
                displayColor =displayColor.lerp(cubeColor, myTween) ;
            }
            
        }
        
        if (myTween >= 1.0) {
            
            displayColor = cubeColor;
            slowChange = false;
            fastChange = false;
        }
    }
    
    if (fastChange) {
        if (myTween < 1.0) {
            myTween = (myTween*1.55)+(0.035 * (5-*pulseDivPtr));
            if (displayColor != cubeColor) {
                displayColor =displayColor.lerp(cubeColor, myTween) ;
            }
            
        }
        
        if (myTween >= 1.0) {
            
            displayColor = cubeColor;
            slowChange = false;
            fastChange = false;
        }
    }
    
}


void Cube::changeGroupColor(ofColor c_) {
    
    if (!noSaturation) {
        groupColor = c_;
        scanColor = ofColor::fromHsb(c_.getHue(), c_.getSaturation()-50, 255) ;
    }
    
    
    
}

void Cube::setColor(ofColor c_,bool fast){
    
    if (!noSaturation) {
        cubeColor = c_;
        //displayColor = c_;
        
        myTween = 0.0;
        fastChange = fast;
        slowChange = !fast;
    }
    
    
}
void Cube::setDefaultHeight(float height_) {
    diff = height_-vec0Ptr->z;
    actualZ = vec0Ptr->z;
    defaultZ = height_;
    aniPct = 0.0;
}

void Cube::satOff(){
    tempColor = groupColor;
    changeGroupColor(ofColor::fromHsb(groupColor.getHue(), ofClamp( groupColor.getSaturation()-255,0,255) , groupColor.getBrightness(), 255));
    setColor(groupColor,false );
    noSaturation = true;
    
    
}

void Cube::satOn() {
    noSaturation = false;
    changeGroupColor(tempColor);
    setColor(groupColor, true);
}
