

#include "HistoryView.h"


HistoryView::HistoryView(){
    
}

//--------------------------------------------------------------
void HistoryView::setupMesh(GlobalGUI *guiPtr_, int rows_, int index_){
    guiPtr = guiPtr_;
    rows = rows_;
    interfaceIndex = index_;
    mesh.clear();
    elementSize = guiPtr->elementSize;
    mesh.setMode(OF_PRIMITIVE_TRIANGLES);
    
}

//--------------------------------------------------------------
void HistoryView::setupSpacer(ofVec3f pos_, float scale_, ofVec3f size_) {
    
    spacer.setMode(OF_PRIMITIVE_LINES);
    ofVec3f position = pos_ - ofVec3f(size_.x*scale_,0,0);
    elementDist =(size_.x*2*scale_)/12 ;
    elementWhiteSpace = (elementDist-elementSize.x)/2;
    for(int i = 0; i < 13; i++){
        spacer.addVertex(position+ofVec3f(0,6,0)+ofVec3f(elementDist*i,0,0));
        spacer.addVertex(position+ofVec3f(0,-6,0)+ofVec3f(elementDist*i,0,0));
        
        spacer.addColor(ofColor::fromHsb(255,0,195,255));
        spacer.addColor(ofColor::fromHsb(255,0,195,255));
    }
    
    spacer.addColor(ofColor::fromHsb(255,0,195,255));
    spacer.addColor(ofColor::fromHsb(255,0,195,255));
    
}

//--------------------------------------------------------------
void HistoryView::update(vector<noteLog>& noteLog_, vector<GlobalGUI>& guiIndex_, ofVboMesh &mainMesh_){
    
    ofVec3f temp;
    ofColor tempC;
    float alpha;
    mesh.clearVertices();
    mesh.clearColors();
    for (int i = noteLog_.size()-1; i > 0 ; i--) {
        temp = ofVec3f(0 ,((noteLog_.size()-i-1) *4)+elementSize.y+6,0);
        alpha =255*( 1- abs(ofMap( (255/noteLog_.size())*i,0,255,1,0 )) ) ;
        for (int j = 0; j < 12; j++) {
            if(noteLog_.at(i).notes.at(j)){
                tempC = guiIndex_.at(interfaceIndex+j).elementColorOn;
                tempC.a = alpha;
                
                float volumeB = (1.0-noteLog_.at(i).volume.at(j))*elementSize.x/2;
                
                //read the position form the main interface vertices
                mesh.addVertex(mainMesh_.getVertex(guiIndex_.at(interfaceIndex+j).counter+1)-temp+ofVec3f(+volumeB,0,0));
                mesh.addVertex(mainMesh_.getVertex(guiIndex_.at(interfaceIndex+j).counter+1)-temp+ofVec3f(+volumeB,2,0));
                mesh.addVertex(mainMesh_.getVertex(guiIndex_.at(interfaceIndex+j).counter+2)-temp+ofVec3f(-volumeB,2,0));
                
                mesh.addVertex(mainMesh_.getVertex(guiIndex_.at(interfaceIndex+j).counter+2)-temp+ofVec3f(-volumeB,2,0));
                mesh.addVertex(mainMesh_.getVertex(guiIndex_.at(interfaceIndex+j).counter+2)-temp+ofVec3f(-volumeB,0,0));
                mesh.addVertex(mainMesh_.getVertex(guiIndex_.at(interfaceIndex+j).counter+1)-temp+ofVec3f(+volumeB,0,0));
                
                mesh.addColor(tempC);
                mesh.addColor(tempC);
                mesh.addColor(tempC);
                mesh.addColor(tempC);
                mesh.addColor(tempC);
                mesh.addColor(tempC);
            }
        }
    }
    
    
    
    //move spacer with interface animation
    if ( (spacer.getVertex(0).y != guiPtr->drawStringPos.y+6)  || (spacer.getVertex(0).x != guiPtr->drawStringPos.x-(elementWhiteSpace*2)) ) {
        for (int i = 0; i < spacer.getNumVertices(); i+=2) {
            spacer.setVertex(i, ofVec3f(guiPtr->drawStringPos.x+(i*elementWhiteSpace*2)-(elementWhiteSpace*2),guiPtr->drawStringPos.y+(elementSize.y/4),0) );
            spacer.setVertex(i+1, ofVec3f(guiPtr->drawStringPos.x+(i*elementWhiteSpace*2)-(elementWhiteSpace*2),guiPtr->drawStringPos.y-(elementSize.y/4),0) );
        }
    }
    
    
}

//--------------------------------------------------------------
void HistoryView::updateStateEditDetail(vector<noteLog>& noteLog_, vector<GlobalGUI>& guiIndex_, ofVboMesh &mainMesh_, int newIndex_){
    
    //move spacer with interface animation
    if ( (spacer.getVertex(0).y != guiIndex_.at(newIndex_).drawStringPos.y+6)  || (spacer.getVertex(0).x != guiIndex_.at(newIndex_).drawStringPos.x-(elementWhiteSpace*2)) ) {
        for (int i = 0; i < spacer.getNumVertices(); i+=2) {
            spacer.setVertex(i, ofVec3f(guiIndex_.at(newIndex_).drawStringPos.x+(i*elementWhiteSpace*2)-(elementWhiteSpace*2),guiIndex_.at(newIndex_).drawStringPos.y+(elementSize.y/4),0) );
            spacer.setVertex(i+1, ofVec3f(guiIndex_.at(newIndex_).drawStringPos.x+(i*elementWhiteSpace*2)-(elementWhiteSpace*2),guiIndex_.at(newIndex_).drawStringPos.y-(elementSize.y/4),0) );
        }
    }
    
    
}

//--------------------------------------------------------------
void HistoryView::updateColor(){
    
    if (changeColor) {
        myTween = (myTween*1.12)+0.01;
        
        if (displayC != targetC) {
            displayC = displayC.lerp(targetC, myTween);
        }
        
        if (myTween >= 1.0) {
            displayC = targetC;
            myTween = 0.0;
            changeColor = false;
        }
        
        for(int i = 0; i < spacer.getNumVertices();i++){
            spacer.setColor(i, displayC);
        }
    }
    
}

//--------------------------------------------------------------
void HistoryView::draw(){
    mesh.draw();
    spacer.draw();
}

//--------------------------------------------------------------
void HistoryView::blink(){
    displayC = ofColor::fromHsb(displayC.getHue(), displayC.getSaturation()-50, 255, 255);
    myTween = 0.0;
    changeColor = true;
}

//--------------------------------------------------------------
void HistoryView::changeC(ofColor target_){
    if(displayC != target_){
        displayC = ofColor::fromHsb(target_.getHue(), target_.getSaturation()-50, 255, 255);
        targetC = target_;
        myTween = 0.0;
        changeColor = true;
    }
}
