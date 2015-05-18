

#include "MusterFlip.h"


MusterFlip::MusterFlip() {
    
}

void MusterFlip::setup(int size_,int tiles_){
    size = size_;
    gridTiles = tiles_;
    ofFbo tempFbo;
    
    texture.allocate(size, size, GL_RGBA);
    
    layerInfo.resize(tiles_);
    for (int i = 0;i  < layerInfo.size(); i++) {
        layerInfo.at(i).resize(tiles_);
    }
    
    makeTex(tempFbo);
    
}

void MusterFlip::draw(ofVec3f pos_){
    texture.draw(pos_);
    
}

void MusterFlip::makeTex(ofFbo& fbo_){

    
    
    fbo_.allocate( (size/gridTiles)*gridTiles, (size/gridTiles)*gridTiles, GL_RGBA);
    
    fbo_.begin();
    ofClear(ofColor::darkCyan);
    
    ofSetColor(255,255,255,230);
    float rSize = size/gridTiles;
    for (int i = 0; i < gridTiles; i++) {
        for (int j =gridTiles-1 ; j >= 0 ; j--) {
            if(ofRandom(100) > 65){
                ofRect(i*rSize, j*rSize, rSize, rSize);
                layerInfo.at(i).at((gridTiles-1)-j) = true;
            } else {
                layerInfo.at(i).at((gridTiles-1)-j) = false;
            }
        }
    }
    fbo_.end();
    
    texture = fbo_.getTextureReference();
  
    
}
