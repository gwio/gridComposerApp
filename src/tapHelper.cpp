//
//  tapHelper.cpp
//  sound_table_app
//
//  Created by Christian Gwiozda on 17.02.15.
//
//

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


