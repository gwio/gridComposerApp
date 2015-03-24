#include "GlobalGUI.h"


GlobalGUI::GlobalGUI(){
    
}

//colors for this start with 50,0,0
GlobalGUI::GlobalGUI(int counter_, string name_, ofColor pickc_) {
    elementName = name_;
    drawString = name_;
    elementColor = ofColor::fromHex( ofToInt( ofToHex( ofRandom(255*255*255) ) ) );
    
    counter=counter_;
    index[0]=0;
    index[0]=1;
    index[0]=2;
    index[0]=2;
    index[0]=3;
    index[0]=0;
    
   pickColor = pickc_;
}


void updateMainMesh(ofVboMesh& mesh_){
    
}

ofVec3f GlobalGUI::getWorldToScreen(ofVec3f& vec_, ofCamera& cam_){
    ofVec3f temp;
    temp =cam_.worldToScreen(vec_);
    return temp;
}