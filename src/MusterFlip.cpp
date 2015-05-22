

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
    
    //makeTex(tempFbo);
    
}

void MusterFlip::loadData(bool** data_, int tilesX_, int tilesY_) {
    for (int i = 0; i < tilesX_; i++) {
        for (int j = 0; j < tilesY_; j++) {
            layerInfo.at(i).at(j) = data_[j][i];
        }
    }
    
    makeTex();
}

void MusterFlip::draw(ofVec3f pos_){
    texture.draw(pos_);
    
}

void MusterFlip::makeTex(){

    ofFbo fbo_;
    
    fbo_.allocate( (size/gridTiles)*gridTiles, (size/gridTiles)*gridTiles, GL_RGBA);
    
    fbo_.begin();
    ofClear(ofColor::darkCyan);
    
    ofSetColor(255,255,255,230);
    float rSize = size/gridTiles;
    for (int i = 0; i < gridTiles; i++) {
        for (int j =0 ; j < gridTiles ; j++) {
            if( layerInfo.at(i).at(j) ){
                ofRect(i*rSize, j*rSize, rSize, rSize);
            }
        }
    }
    fbo_.end();
    
    texture = fbo_.getTextureReference();
  
    
}
