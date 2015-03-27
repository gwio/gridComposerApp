#include "GlobalGUI.h"


GlobalGUI::GlobalGUI(){
    
}

//colors for this start with 50,0,0
GlobalGUI::GlobalGUI(int counter_, string name_,ofVec3f elementSize_ ,ofColor pickc_, ofVec3f placement_) {
    elementName = name_;
    drawString = name_;
    elementColor = ofColor(ofRandom(255),ofRandom(255),ofRandom(255));
    
    counter=counter_*4;
    index[0]=0;
    index[1]=1;
    index[2]=2;
    index[3]=2;
    index[4]=3;
    index[5]=0;
    
   pickColor = pickc_;
   placement =  placement_;
    
    elementSize = elementSize_;
}


void GlobalGUI::updateMainMesh(ofVboMesh& mesh_, ofVec3f vec_){
    mesh_.setVertex(0+counter, vec_+ofVec3f(-elementSize.x/2, -elementSize.y/2,0)+placement);
    mesh_.setVertex(1+counter, vec_+ofVec3f(-elementSize.x/2, elementSize.y/2,0)+placement);
    mesh_.setVertex(2+counter, vec_+ofVec3f(elementSize.x/2, elementSize.y/2,0)+placement);
    mesh_.setVertex(3+counter, vec_+ofVec3f(elementSize.x/2, -elementSize.y/2,0)+placement);
   
    minX = -elementSize.x/2+placement.x+vec_.x;
    maxX = elementSize.x/2+placement.x+vec_.x;
    minY = -elementSize.y/2+placement.y+vec_.y;
    maxY = elementSize.y/2+placement.y+vec_.y;
   }


bool GlobalGUI::isInside(ofVec2f click_) {
    bool result;
    if (click_.x >= minX && click_.x <= maxX && click_.y >= minY && click_.y <= maxY) {
        result = true;
        return result;
    } else {
        result = false;
        return result;
    }
}
