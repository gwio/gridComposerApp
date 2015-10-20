/* flowing note history in harmony menu */ 

#pragma once
#include "ofMain.h"
#include "GlobalGUI.h"
#include "instrument.h"
//#include "instrument.h"

class HistoryView{
    
public:
    HistoryView();
    
    void setup(GlobalGUI*,int,int);
    void update(vector<noteLog>&,vector<GlobalGUI>&);
    void draw();
    
    int rows;
    int interfaceIndex;
    ofVec2f pos;
    ofVec3f offset;
    
    ofVboMesh mesh;
    GlobalGUI *guiPtr;

};