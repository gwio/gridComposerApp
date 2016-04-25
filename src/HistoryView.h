/* flowing note history in harmony menu */ 

#pragma once
#include "ofMain.h"
#include "GlobalGUI.h"
#include "instrument.h"
//#include "instrument.h"

class HistoryView{
    
public:
    HistoryView();
    
    void setupMesh(GlobalGUI*,int,int);
    void setupSpacer(ofVec3f,float, ofVec3f);
    void update(vector<noteLog>&,vector<GlobalGUI>&, ofVboMesh&);
    void updateStateEditDetail(vector<noteLog>&,vector<GlobalGUI>&, ofVboMesh&, int);

    void draw();
    
    int rows;
    int interfaceIndex;
    ofVec2f pos;
    ofVec3f elementSize;
    
    float elementDist;
    float elementWhiteSpace;
    
    ofVboMesh mesh, spacer;
    GlobalGUI *guiPtr;

    bool changeColor;
    ofColor displayC, targetC;
    float myTween;
    
    void updateColor();
    void changeC(ofColor);
    
    void blink();
};