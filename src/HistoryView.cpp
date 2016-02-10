

#include "HistoryView.h"


HistoryView::HistoryView(){
    
}

void HistoryView::setupMesh(GlobalGUI *guiPtr_, int rows_, int index_){
    guiPtr = guiPtr_;
    rows = rows_;
    interfaceIndex = index_;
    mesh.clear();
    // pos = guiPtr->drawStringPos;
    elementSize = guiPtr->elementSize;
    mesh.setMode(OF_PRIMITIVE_TRIANGLES);
    
}

void HistoryView::setupSpacer(ofVec3f pos_, float scale_, ofVec3f size_) {
    
    spacer.setMode(OF_PRIMITIVE_LINES);
    ofVec3f position = pos_ - ofVec3f(size_.x*scale_,0,0);
    elementDist =(size_.x*2*scale_)/12 ;
    elementWhiteSpace = (elementDist-elementSize.x)/2;
    for(int i = 0; i < 13; i++){
        spacer.addVertex(position+ofVec3f(0,6,0)+ofVec3f(elementDist*i,0,0));
        spacer.addVertex(position+ofVec3f(0,-6,0)+ofVec3f(elementDist*i,0,0));
        
        spacer.addColor(ofColor(255,255,255));
        spacer.addColor(ofColor(255,255,255));
    }
}

void HistoryView::update(vector<noteLog>& noteLog_, vector<GlobalGUI>& guiIndex_, ofVboMesh &mainMesh_){
    
    ofVec3f temp;
    ofColor tempC;
    float alpha;
    mesh.clearVertices();
    mesh.clearColors();
    for (int i = noteLog_.size()-1; i > 0 ; i--) {
        temp = ofVec3f(0 ,(noteLog_.size()-i-1) *5+elementSize.y,0);
        alpha =255*( 1- abs(ofMap( (255/noteLog_.size())*i,0,255,1,0 )) ) ;
        for (int j = 0; j < 12; j++) {
            if(noteLog_.at(i).notes.at(j)){
                tempC = guiIndex_.at(interfaceIndex+j).elementColorOn;
                tempC.a = alpha;
                
                float volumeB = noteLog_.at(i).volume.at(j);

                //read the position form the main interface vertices
                mesh.addVertex(mainMesh_.getVertex(guiIndex_.at(interfaceIndex+j).counter+1)-temp);
                mesh.addVertex(mainMesh_.getVertex(guiIndex_.at(interfaceIndex+j).counter+1)-temp+ofVec3f(0,2,0));
                mesh.addVertex(mainMesh_.getVertex(guiIndex_.at(interfaceIndex+j).counter+2)-temp+ofVec3f(0,2,0));
                
                mesh.addVertex(mainMesh_.getVertex(guiIndex_.at(interfaceIndex+j).counter+2)-temp+ofVec3f(0,2,0));
                mesh.addVertex(mainMesh_.getVertex(guiIndex_.at(interfaceIndex+j).counter+2)-temp);
                mesh.addVertex(mainMesh_.getVertex(guiIndex_.at(interfaceIndex+j).counter+1)-temp);
                
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
    if (spacer.getVertex(0).y != guiPtr->drawStringPos.y) {
        for (int i = 0; i < spacer.getNumVertices(); i+=2) {
            spacer.setVertex(i, ofVec3f(spacer.getVertex(i).x,guiPtr->drawStringPos.y+6,0) );
            spacer.setVertex(i+1, ofVec3f(spacer.getVertex(i).x,guiPtr->drawStringPos.y-6,0) );
        }
    }
}


void HistoryView::draw(){
    mesh.draw();
    spacer.draw();
}
