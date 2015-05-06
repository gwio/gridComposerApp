#include "GlobalGUI.h"


GlobalGUI::GlobalGUI(){
    
}

//colors for this start with 50,0,0
GlobalGUI::GlobalGUI(int counter_, string name_,ofVec3f elementSize_ ,ofColor pickc_, ofVec3f placement_, ofVec3f offPlacement_, int fontS_, bool trans_) {
    elementName = name_;
    drawString = name_;
    
    if (!trans_) {
    elementColorOn = ofColor::gray;
    } else {
        elementColorOn = ofColor(0,0,0,0);
    }
    
    counter=counter_*4;
    index[0]=0;
    index[1]=1;
    index[2]=2;
    index[3]=2;
    index[4]=3;
    index[5]=0;
    
    placement =  placement_;
    offPlacement = offPlacement_;
    
    elementSize = elementSize_;
    onOff = true;
    
    sliderWidth = 0.0;
    showString = false;
    
    animation = false;
    
    touchDown = false;
    
    fontSize = fontS_;
    
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
    
    drawStringPos = vec_+placement+ofVec3f(-stringWidth,stringHeight,0)+(offPlacement);
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
        
        drawStringPos = vec_+placement+ofVec3f(-stringWidth,stringHeight,0)+(offPlacement*(abs(moveDir-tween_)));
        curPos = vec_;
    }
    
    if(animation && tween_ >= 1.0){
        animation = false;
    }
}

void GlobalGUI::setSlider(ofVboMesh& mesh_, float width_) {
    sliderWidth = width_;
    
    mesh_.setVertex(0+counter, curPos+ofVec3f(-elementSize.x/2, -elementSize.y/2,0)+placement+(offPlacement*(abs(moveDir-1))));
    mesh_.setVertex(1+counter, curPos+ofVec3f(-elementSize.x/2, elementSize.y/2,0)+placement+(offPlacement*(abs(moveDir-1))));
    mesh_.setVertex(2+counter, curPos+ofVec3f(elementSize.x/2-sliderWidth, elementSize.y/2,0)+placement+(offPlacement*(abs(moveDir-1))));
    mesh_.setVertex(3+counter, curPos+ofVec3f(elementSize.x/2-sliderWidth, -elementSize.y/2,0)+placement+(offPlacement*(abs(moveDir-1))));
    
    minX = -elementSize.x/2+placement.x+curPos.x;
    maxX = elementSize.x/2+placement.x+curPos.x;
    minY = -elementSize.y/2+placement.y+curPos.y;
    maxY = elementSize.y/2+placement.y+curPos.y;
    
    drawStringPos = curPos+placement+ofVec3f(-stringWidth,stringHeight,0)+(offPlacement*(abs(moveDir-1)));
    
}

void GlobalGUI::updateMainMeshSlider(ofVboMesh& mesh_, ofVec3f vec_, float width_, float& tween_){
    
    if (animation) {
        mesh_.setVertex(0+counter, vec_+ofVec3f(-elementSize.x/2, -elementSize.y/2,0)+placement+(offPlacement*(abs(moveDir-tween_))));
        mesh_.setVertex(1+counter, vec_+ofVec3f(-elementSize.x/2, elementSize.y/2,0)+placement+(offPlacement*(abs(moveDir-tween_))));
        mesh_.setVertex(2+counter, vec_+ofVec3f(elementSize.x/2-sliderWidth, elementSize.y/2,0)+placement+(offPlacement*(abs(moveDir-tween_))));
        mesh_.setVertex(3+counter, vec_+ofVec3f(elementSize.x/2-sliderWidth, -elementSize.y/2,0)+placement+(offPlacement*(abs(moveDir-tween_))));
        
        minX = -elementSize.x/2+placement.x+vec_.x;
        maxX = elementSize.x/2+placement.x+vec_.x;
        minY = -elementSize.y/2+placement.y+vec_.y;
        maxY = elementSize.y/2+placement.y+vec_.y;
        
        drawStringPos = vec_+placement+ofVec3f(-stringWidth,stringHeight,0)+(offPlacement*(abs(moveDir-tween_)));
        curPos = vec_;
    }
    if(animation && tween_ >= 1.0){
        animation = false;
    }
}

void GlobalGUI::setColor(float hue_) {
    
    elementColorOn = ofColor::fromHsb(hue_, 100, 100);
    elementColorOff = ofColor::fromHsb(elementColorOn.getHue(), elementColorOn.getBrightness(), elementColorOn.getSaturation(), 25 );
    elementColorDarker = ofColor::fromHsb(elementColorOn.getHue(), elementColorOn.getBrightness()-60, elementColorOn.getSaturation(), 255 );
    

}

void GlobalGUI::activateOnColor(ofVboMesh& mesh_){
    mesh_.setColor(0+counter, elementColorOn);
    mesh_.setColor(1+counter, elementColorOn);
    mesh_.setColor(2+counter, elementColorOn);
    mesh_.setColor(3+counter, elementColorOn);
}

void GlobalGUI::activateDarkerColor(ofVboMesh& mesh_){
    mesh_.setColor(0+counter, elementColorDarker);
    mesh_.setColor(1+counter, elementColorDarker);
    mesh_.setColor(2+counter, elementColorDarker);
    mesh_.setColor(3+counter, elementColorDarker);
    
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
        mesh_.setColor(0+counter, elementColorOn);
        mesh_.setColor(1+counter,elementColorOn);
        mesh_.setColor(2+counter, elementColorOn);
        mesh_.setColor(3+counter, elementColorOn);
    } else {
        mesh_.setColor(0+counter, elementColorOff);
        mesh_.setColor(1+counter,elementColorOff);
        mesh_.setColor(2+counter, elementColorOff);
        mesh_.setColor(3+counter, elementColorOff);
    }
    
}

void GlobalGUI::setOn(ofVboMesh& mesh_) {
    mesh_.setColor(0+counter, elementColorOn);
    mesh_.setColor(1+counter,elementColorOn);
    mesh_.setColor(2+counter, elementColorOn);
    mesh_.setColor(3+counter, elementColorOn);
    onOff = true;
}


void GlobalGUI::setOff(ofVboMesh& mesh_) {
    mesh_.setColor(0+counter, elementColorOff);
    mesh_.setColor(1+counter,elementColorOff);
    mesh_.setColor(2+counter, elementColorOff);
    mesh_.setColor(3+counter, elementColorOff);
    onOff = false;
}


void GlobalGUI::setStringWidth(float sW_) {
    stringWidth = sW_/2;
    drawStringPos.x = (((maxX-minX)/2)-stringWidth)+minX ;
}

