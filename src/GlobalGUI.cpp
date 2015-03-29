#include "GlobalGUI.h"


GlobalGUI::GlobalGUI(){
    
}

//colors for this start with 50,0,0
GlobalGUI::GlobalGUI(int counter_, string name_,ofVec3f elementSize_ ,ofColor pickc_, ofVec3f placement_, ofVec3f offPlacement_) {
    elementName = name_;
    drawString = name_;
    elementColor = ofColor(ofRandom(255),ofRandom(255),ofRandom(100,155), 255);
    
    counter=counter_*4;
    index[0]=0;
    index[1]=1;
    index[2]=2;
    index[3]=2;
    index[4]=3;
    index[5]=0;
    
    elementColorOff = ofColor::fromHsb(elementColor.getHue(), elementColor.getBrightness(), elementColor.getSaturation(), 25 );
   placement =  placement_;
    offPlacement = offPlacement_;
    
    elementSize = elementSize_;
    onOff = true;
    
    sliderWidth = 0.0;
    showString = false;
    
    animation = false;
}

void GlobalGUI::updateMainMesh(ofVboMesh& mesh_, ofVec3f vec_){
    mesh_.setVertex(0+counter, vec_+ofVec3f(-elementSize.x/2, -elementSize.y/2,0)+placement+(offPlacement));
    mesh_.setVertex(1+counter, vec_+ofVec3f(-elementSize.x/2, elementSize.y/2,0)+placement+(offPlacement));
    mesh_.setVertex(2+counter, vec_+ofVec3f(elementSize.x/2, elementSize.y/2,0)+placement+(offPlacement));
    mesh_.setVertex(3+counter, vec_+ofVec3f(elementSize.x/2, -elementSize.y/2,0)+placement+(offPlacement));
    
    minX = -elementSize.x/2+placement.x+vec_.x;
    maxX = elementSize.x/2+placement.x+vec_.x;
    minY = -elementSize.y/2+placement.y+vec_.y;
    maxY = elementSize.y/2+placement.y+vec_.y;
    
    drawStringPos = vec_+placement+ofVec3f(-(elementSize.x/2),0,0)+(offPlacement);
}

void GlobalGUI::updateMainMesh(ofVboMesh& mesh_, ofVec3f vec_,float& tween_){
    if (animation){
    mesh_.setVertex(0+counter, vec_+ofVec3f(-elementSize.x/2, -elementSize.y/2,0)+placement+(offPlacement*( abs(moveDir-tween_))));
    mesh_.setVertex(1+counter, vec_+ofVec3f(-elementSize.x/2, elementSize.y/2,0)+placement+(offPlacement*( abs(moveDir-tween_))));
    mesh_.setVertex(2+counter, vec_+ofVec3f(elementSize.x/2, elementSize.y/2,0)+placement+(offPlacement*( abs(moveDir-tween_))));
    mesh_.setVertex(3+counter, vec_+ofVec3f(elementSize.x/2, -elementSize.y/2,0)+placement+(offPlacement*( abs(moveDir-tween_))));
   
    minX = -elementSize.x/2+placement.x+vec_.x;
    maxX = elementSize.x/2+placement.x+vec_.x;
    minY = -elementSize.y/2+placement.y+vec_.y;
    maxY = elementSize.y/2+placement.y+vec_.y;
    
    drawStringPos = vec_+placement+ofVec3f(-(elementSize.x/2),0,0)+(offPlacement*(abs(moveDir-tween_)));
   }
    
    if(animation && tween_ >= 1.0){
        animation = false;
    }
}

void GlobalGUI::updateMainMeshSlider(ofVboMesh& mesh_, ofVec3f vec_, float width_, float& tween_){
    
    if (animation) {
    mesh_.setVertex(0+counter, vec_+ofVec3f(-elementSize.x/2, -elementSize.y/2,0)+placement+(offPlacement*(abs(moveDir-tween_))));
    mesh_.setVertex(1+counter, vec_+ofVec3f(-elementSize.x/2, elementSize.y/2,0)+placement+(offPlacement*(abs(moveDir-tween_))));
    mesh_.setVertex(2+counter, vec_+ofVec3f(elementSize.x/2-width_, elementSize.y/2,0)+placement+(offPlacement*(abs(moveDir-tween_))));
    mesh_.setVertex(3+counter, vec_+ofVec3f(elementSize.x/2-width_, -elementSize.y/2,0)+placement+(offPlacement*(abs(moveDir-tween_))));
    
    minX = -elementSize.x/2+placement.x+vec_.x;
    maxX = elementSize.x/2+placement.x+vec_.x;
    minY = -elementSize.y/2+placement.y+vec_.y;
    maxY = elementSize.y/2+placement.y+vec_.y;
    
    drawStringPos = vec_+placement+ofVec3f(-(elementSize.x/2),0,0)+(offPlacement*(abs(moveDir-tween_)));
    }
    if(animation && tween_ >= 1.0){
        animation = false;
    }
}

void GlobalGUI::setColor(ofColor c_) {
    
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

void GlobalGUI::switchColor(ofVboMesh& mesh_) {
    onOff = !onOff;
    if (onOff) {
        mesh_.setColor(0+counter, elementColor);
        mesh_.setColor(1+counter,elementColor);
        mesh_.setColor(2+counter, elementColor);
        mesh_.setColor(3+counter, elementColor);
    } else {
            mesh_.setColor(0+counter, elementColorOff);
            mesh_.setColor(1+counter,elementColorOff);
            mesh_.setColor(2+counter, elementColorOff);
            mesh_.setColor(3+counter, elementColorOff);
    }
    
}

void GlobalGUI::setOn(ofVboMesh& mesh_) {
    mesh_.setColor(0+counter, elementColor);
    mesh_.setColor(1+counter,elementColor);
    mesh_.setColor(2+counter, elementColor);
    mesh_.setColor(3+counter, elementColor);
    onOff = true;
}


void GlobalGUI::setOff(ofVboMesh& mesh_) {
    mesh_.setColor(0+counter, elementColorOff);
    mesh_.setColor(1+counter,elementColorOff);
    mesh_.setColor(2+counter, elementColorOff);
    mesh_.setColor(3+counter, elementColorOff);
    onOff = false;
}


