#include "GlobalGUI.h"


GlobalGUI::GlobalGUI(){
    
}

//colors for this start with 50,0,0
GlobalGUI::GlobalGUI(int counter_, string name_,ofVec3f elementSize_ ,ofColor pickc_, ofVec3f placement_, ofVec3f offPlacement_, int fontS_, bool trans_, ofxFontStash* fsPtr_) {
    elementName = name_;
    
    isTrans = trans_;
    
    if (!trans_) {
        //elementColorOn =  filterColor(ofColor::darkCyan);
        elementColorOn = ofColor::fromHsb(255,0,204,255);
        //setColor(0);
        targetColor = elementColorOn;
        //targetColor = elementColorOn;
        elementColorDarker = ofColor::fromHsb(255, 0, 51, 255);
        elementColorOff = ofColor::fromHsb(0,0,0,0);

    } else {
        elementColorOn = ofColor::fromHsb(255,0,204,255);
        targetColor = elementColorOn;
        //ofColor whiteColor = filterColor( ofColor(255,255,255,255));
        displayColor = elementColorOn;
        elementColorTouch = ofColor(255,255,255);
        elementColorDarker = ofColor::fromHsb(255, 0, 51, 255);
        elementColorOff = ofColor::fromHsb(0, 0, 0, 0);
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
    
    sliderPct = 1.0;
    sliderPos = 0.0;
    showString = false;
    
    animation = false;
    animationB = false;
    touchDown = false;
    
    fontSize = fontS_;
    
    myTween = 0.0;
    blink = true;
    
    fsPtr = fsPtr_;
    
    drawStringPos = ofVec3f(-2000,-2000,0);
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
    
    drawStringPos = vec_+placement+(offPlacement);
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
        
        drawStringPos = vec_+placement+(offPlacement*(abs(moveDir-tween_)));
        curPos = vec_;
    }
    
    if(animation && tween_ >= 1.0){
        animation = false;
    }

    
    if (blink) {
        myTween = (myTween*1.12)+0.01;

    if (displayColor != targetColor) {
            displayColor = displayColor.lerp(targetColor, myTween);
        if (!isTrans){
            mesh_.setColor(0+counter,displayColor);
            mesh_.setColor(1+counter,displayColor);
            mesh_.setColor(2+counter,displayColor);
            mesh_.setColor(3+counter,displayColor);
        }
    }
        if (myTween >= 1.0) {
            displayColor = targetColor;
            if(!isTrans) {
            mesh_.setColor(0+counter,displayColor);
            mesh_.setColor(1+counter,displayColor);
            mesh_.setColor(2+counter,displayColor);
            mesh_.setColor(3+counter,displayColor);
            }
            blink = false;
            myTween = 1.0;
        }
    }
    
}


void GlobalGUI::updateMainMeshB(ofVboMesh& mesh_, ofVec3f vec_,float& tween_){
    if (animationB){
        mesh_.setVertex(0+counter, vec_+ofVec3f(-elementSize.x/2, -elementSize.y/2,0)+placement);
        mesh_.setVertex(1+counter, vec_+ofVec3f(-elementSize.x/2, elementSize.y/2,0)+placement);
        mesh_.setVertex(2+counter, vec_+ofVec3f(elementSize.x/2, elementSize.y/2,0)+placement);
        mesh_.setVertex(3+counter, vec_+ofVec3f(elementSize.x/2, -elementSize.y/2,0)+placement);
        
        minX = -elementSize.x/2+placement.x+vec_.x;
        maxX = elementSize.x/2+placement.x+vec_.x;
        minY = -elementSize.y/2+placement.y+vec_.y;
        maxY = elementSize.y/2+placement.y+vec_.y;
        
        drawStringPos = vec_+placement;
        curPos = vec_;
    }
    
    if(animationB && tween_ >= 1.0){
        animationB = false;
    }
    
    
    if (blink) {
        myTween = (myTween*1.12)+0.01;
        
        if (displayColor != targetColor) {
            displayColor = displayColor.lerp(targetColor, myTween);
            if (!isTrans){
            mesh_.setColor(0+counter,displayColor);
            mesh_.setColor(1+counter,displayColor);
            mesh_.setColor(2+counter,displayColor);
            mesh_.setColor(3+counter,displayColor);
            }
        }
        if (myTween >= 1.0) {
            displayColor = targetColor;
            if (!isTrans){
            mesh_.setColor(0+counter,displayColor);
            mesh_.setColor(1+counter,displayColor);
            mesh_.setColor(2+counter,displayColor);
            mesh_.setColor(3+counter,displayColor);
            }
            blink = false;
            myTween = 1.0;
        }
    }
    
}

void GlobalGUI::setSlider(ofVboMesh& mesh_, float width_) {
    sliderPct =ofMap(width_, 0.0, 1.0, -1.0, 1.0);
    
    mesh_.setVertex(0+counter, curPos+ofVec3f(-elementSize.x/2, -elementSize.y/2,0)+placement+(offPlacement*(abs(moveDir-1))));
    mesh_.setVertex(1+counter, curPos+ofVec3f(-elementSize.x/2, elementSize.y/2,0)+placement+(offPlacement*(abs(moveDir-1))));
    mesh_.setVertex(2+counter, curPos+ofVec3f( (elementSize.x/2)*sliderPct , elementSize.y/2,0)+placement+(offPlacement*(abs(moveDir-1))));
    mesh_.setVertex(3+counter, curPos+ofVec3f( (elementSize.x/2)*sliderPct , -elementSize.y/2,0)+placement+(offPlacement*(abs(moveDir-1))));
    
    minX = -elementSize.x/2+placement.x+curPos.x;
    maxX = elementSize.x/2+placement.x+curPos.x;
    minY = -elementSize.y/2+placement.y+curPos.y;
    maxY = elementSize.y/2+placement.y+curPos.y;
    
    drawStringPos = curPos+placement+(offPlacement*(abs(moveDir-1)));
    
}

void GlobalGUI::updateMainMeshSlider(ofVboMesh& mesh_, ofVec3f vec_, float& tween_){
    
    if (animation) {
        mesh_.setVertex(0+counter, vec_+ofVec3f(-elementSize.x/2, -elementSize.y/2,0)+placement+(offPlacement*(abs(moveDir-tween_))));
        mesh_.setVertex(1+counter, vec_+ofVec3f(-elementSize.x/2, elementSize.y/2,0)+placement+(offPlacement*(abs(moveDir-tween_))));
        mesh_.setVertex(2+counter, vec_+ofVec3f( (elementSize.x/2)*sliderPct , elementSize.y/2,0)+placement+(offPlacement*(abs(moveDir-tween_))));
        mesh_.setVertex(3+counter, vec_+ofVec3f( (elementSize.x/2)*sliderPct, -elementSize.y/2,0)+placement+(offPlacement*(abs(moveDir-tween_))));
        
        minX = -elementSize.x/2+placement.x+vec_.x;
        maxX = elementSize.x/2+placement.x+vec_.x;
        minY = -elementSize.y/2+placement.y+vec_.y;
        maxY = elementSize.y/2+placement.y+vec_.y;
        
        drawStringPos = vec_+placement+(offPlacement*(abs(moveDir-tween_)));
        curPos = vec_;
    }
    if(animation && tween_ >= 1.0){
        animation = false;
    }
   
    
    
    if (blink) {
        myTween = (myTween*1.12)+0.01;
        
        if (displayColor != targetColor) {
            displayColor = displayColor.lerp(targetColor, myTween);
            if (!isTrans){
                mesh_.setColor(0+counter,displayColor);
                mesh_.setColor(1+counter,displayColor);
                mesh_.setColor(2+counter,displayColor);
                mesh_.setColor(3+counter,displayColor);
            }
        }
        if (myTween >= 1.0) {
            displayColor = targetColor;
            if(!isTrans) {
                mesh_.setColor(0+counter,displayColor);
                mesh_.setColor(1+counter,displayColor);
                mesh_.setColor(2+counter,displayColor);
                mesh_.setColor(3+counter,displayColor);
            }
            blink = false;
            myTween = 1.0;
        }
    }
    
}

void GlobalGUI::setColor(float hue_) {
    
    elementColorOn = ofColor::fromHsb(hue_, 120, 140,255);
    
   // elementColorOff = ofColor::fromHsb(elementColorOn.getHue(), elementColorOn.getSaturation(), elementColorOn.getBrightness(), 0 );
    //elementColorDarker = ofColor::fromHsb(elementColorOn.getHue(),elementColorOn.getSaturation(),51, 255 );
    elementColorTouch = ofColor::fromHsb(elementColorOn.getHue(), elementColorOn.getSaturation()-50, 255, 255);

}


void GlobalGUI::setColor(ofColor color_) {
    
    elementColorOn = color_;
    
   // elementColorOff = ofColor::fromHsb(elementColorOn.getHue(), elementColorOn.getSaturation(), elementColorOn.getBrightness(), 0 );
    //elementColorDarker = ofColor::fromHsb(elementColorOn.getHue(),elementColorOn.getSaturation(),51, 255 );
    elementColorTouch = ofColor::fromHsb(elementColorOn.getHue(), elementColorOn.getSaturation()-50, 255, 255);

    
}

void GlobalGUI::activateOnColor(){
    targetColor = elementColorOn;
    myTween = 0.0;
    blink = true;
    /*
    mesh_.setColor(0+counter, displayColor);
    mesh_.setColor(1+counter, displayColor);
    mesh_.setColor(2+counter, displayColor);
    mesh_.setColor(3+counter, displayColor);
     */
}

void GlobalGUI::activateDarkerColor(){
    targetColor = elementColorDarker;
    myTween = 0.0;
    blink = true;
    /*
    mesh_.setColor(0+counter, displayColor);
    mesh_.setColor(1+counter, displayColor);
    mesh_.setColor(2+counter, displayColor);
    mesh_.setColor(3+counter, displayColor);
    */
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

void GlobalGUI::switchColor() {
    
    if (onOff) {
       
        targetColor = elementColorOff;
        myTween = 0.0;
        blink = true;
    } else {
       
        targetColor = elementColorOn;
        myTween = 0.0;
        blink = true;
    }
 onOff = !onOff;
    
}

void GlobalGUI::setOn() {
    targetColor = elementColorOn;
    myTween = 0.0;
    blink = true;
/*
    mesh_.setColor(0+counter, elementColorOn);
    mesh_.setColor(1+counter,elementColorOn);
    mesh_.setColor(2+counter, elementColorOn);
    mesh_.setColor(3+counter, elementColorOn);
 */
    onOff = true;
}


void GlobalGUI::setOff() {
    
    
    targetColor = elementColorOff;
    myTween = 0.0;
    blink = true;
/*
    mesh_.setColor(0+counter, elementColorOff);
    mesh_.setColor(1+counter,elementColorOff);
    mesh_.setColor(2+counter, elementColorOff);
    mesh_.setColor(3+counter, elementColorOff);
 */
    onOff = false;
}


void GlobalGUI::setStringWidth(float sW_) {
    stringWidth = sW_/2;
}

void GlobalGUI::blinkOn(){
    displayColor = elementColorTouch;
    myTween = 0.0;
    blink = true;
}

void GlobalGUI::drawFontString() {
    
    if (showString && isTrans){
        ofPushStyle();
        ofSetColor(displayColor);
        fsPtr->draw(elementName,
                    fontSize,
                    drawStringPos.x-stringWidth,
                    drawStringPos.y+stringHeight
                    );
    } else if (showString) {
        ofPushStyle();
        if(onOff) {
        ofSetColor(ofColor::fromHsb(255,0,204,255));
        } else {
            ofSetColor(elementColorDarker);
        }
        fsPtr->draw(elementName,
                    fontSize,
                    drawStringPos.x-stringWidth,
                    drawStringPos.y+stringHeight
                    );
    }
    ofPopStyle();

}

void GlobalGUI::drawFontString(float offsetX_, float offsetY_) {
    
    if (showString && isTrans){
        ofPushStyle();
        ofSetColor(displayColor);
        fsPtr->draw(elementName,
                    fontSize,
                    drawStringPos.x-stringWidth+offsetX_,
                    drawStringPos.y+stringHeight+offsetY_
                    );
    } else if (showString) {
        ofPushStyle();
        if(onOff) {
        ofSetColor(ofColor::fromHsb(255,0,204,255));
    } else {
        ofSetColor(elementColorDarker);
    }
    fsPtr->draw(elementName,
                    fontSize,
                    drawStringPos.x-stringWidth+offsetX_,
                    drawStringPos.y+stringHeight+offsetY_
                    );
    }
    ofPopStyle();
    
}



