

#include "HistoryView.h"


HistoryView::HistoryView(){
    
}

void HistoryView::setup(GlobalGUI *guiPtr_, int rows_, int index_){
    guiPtr = guiPtr_;
    rows = rows_;
    interfaceIndex = index_;
    mesh.clear();
   // pos = guiPtr->drawStringPos;
    offset = guiPtr->elementSize;
    mesh.setMode(OF_PRIMITIVE_LINES);
}

void HistoryView::update(vector<noteLog>& noteLog_, vector<GlobalGUI>& guiIndex_){
    pos = guiIndex_.at(interfaceIndex).drawStringPos+ofVec3f(-offset.x/2,+offset.y/2,0);
    ofVec3f temp;
    ofColor tempC;
   
    mesh.clearVertices();
    mesh.clearColors();
    for (int i = noteLog_.size()-1; i > 0 ; i--) {
        for (int j = 0; j < 12; j++) {
            if(noteLog_.at(i).notes.at(j)){
                temp = ofVec3f(pos.x+(offset.x*j)+(offset.x/2),pos.y-( (noteLog_.size()-i-1) *3),0);
                tempC = ofColor(255,255,255);
                tempC.a =255*( 1- abs(ofMap( (255/noteLog_.size())*i,0,255,-1,1 )) )-10;
                 float volumeB = noteLog_.at(i).volume.at(j);
                
                mesh.addVertex(temp - ofVec3f((offset.x*volumeB/2),0,0) );


                mesh.addVertex(temp + ofVec3f((offset.x*volumeB/2),0,0));

               
                mesh.addColor(tempC);

                mesh.addColor(tempC);
            }
        }
    }
}


void HistoryView::draw(){
    mesh.draw();
}
