#include "tapHelper.h"

TapHelper::TapHelper() {
    old = false;
}


TapHelper::TapHelper(unsigned long id_, ofVec2f layerIndex_) {
    
    tapId = id_;
    tapOrigin = layerIndex_;
    old = false;
}

TapHelper::TapHelper(unsigned long id_, ofVec2f layerIndex_, float zH_, ofColor c_) {
    
    tapId = id_;
    tapOrigin = layerIndex_;
    old = false;
    
    zH = zH_;
    cColor = c_;
}


