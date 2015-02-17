#pragma once

#include "ofMain.h"

class TapHelper {
    
    
public:
    
    TapHelper();
    TapHelper(unsigned long,ofVec2f);
    TapHelper(unsigned long,ofVec2f, float, ofColor);

    
    unsigned long tapId;
    ofVec2f tapOrigin;
    float zH;
    ofColor cColor;
    
    bool old;
};