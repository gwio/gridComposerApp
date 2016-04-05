

#include "MusterFlip.h"


MusterFlip::MusterFlip() {
    
}

void MusterFlip::setup(float size_,int tiles_){
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

void MusterFlip::makeBackTex(){
    ofPushStyle();
    ofFbo fbo_;
    
    fbo_.allocate( (size/gridTiles)*gridTiles, (size/gridTiles)*gridTiles, GL_RGBA);
    
    fbo_.begin();
    
    // ofEnableDepthTest();
    ofClear(ofColor::fromHsb(255,0,195,255));
    
    fbo_.end();
    texture.clear();
    texture = fbo_.getTextureReference();
    
    ofPopStyle();
    


}

void MusterFlip::makeTex(){

    
    ofPushStyle();
    ofFbo fbo_;
    
    fbo_.allocate( size, size, GL_RGBA);
    
    fbo_.begin();
   
    // ofEnableDepthTest();
    ofClear(ofColor::fromHsb(0,0,0,0));
    float rSize = size/gridTiles;
    //ofSetColor(ofColor::darkCyan);

   // ofRect(0,0, rSize*gridTiles, rSize*gridTiles);
    //ofNoFill();
      // ofTranslate(size/10, size/10);
    //ofScale(0.8, 0.8);
    ofSetColor(255, 255, 255,255);
    for (int i = 0; i < gridTiles; i++) {
        for (int j =0 ; j < gridTiles ; j++) {
            if( layerInfo.at(i).at(j) ){
                ofRect(i*rSize, ( (gridTiles-1)-j)*rSize, rSize, rSize);
            }
        }
    }
    fbo_.end();
    
    texture.clear();
    texture = fbo_.getTextureReference();
  
    ofPopStyle();
}
