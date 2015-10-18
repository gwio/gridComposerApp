

#include "HistoryView.h"


HistoryView::HistoryView(){
    
}

void HistoryView::setup(GlobalGUI *guiPtr_, int rows_, int index_){
    guiPtr = guiPtr_;
    rows = rows_;
    interfaceIndex = index_;
    mesh.clear();
   // pos = guiPtr->drawStringPos;
    offset = guiPtr->elementSize.x;
    mesh.setMode(OF_PRIMITIVE_TRIANGLES);
}

void HistoryView::update(vector<noteLog>& noteLog_, vector<GlobalGUI>& guiIndex_){
    pos = guiIndex_.at(interfaceIndex).drawStringPos-(offset/2);
    ofVec3f temp;
    ofColor tempC;
    mesh.clearVertices();
    mesh.clearColors();
    for (int i = noteLog_.size()-1; i > 0 ; i--) {
        for (int j = 0; j < 12; j++) {
            if(noteLog_.at(i).notes.at(j)){
            temp = ofVec3f(pos.x+(offset*j),pos.y-( (noteLog_.size()-i-1) *4),0);
                tempC = guiIndex_.at(interfaceIndex+j).elementColorOn;
                mesh.addVertex(temp);
                mesh.addVertex(temp+ofVec3f(offset/2,-4,0));
                mesh.addVertex(temp+ofVec3f(offset,0,0));
                
                mesh.addColor(tempC);
                mesh.addColor(tempC);
                mesh.addColor(tempC);
            }
        }
    }
}


void HistoryView::draw(){
    mesh.draw();
}
