#pragma once
#include "ofMain.h"

struct Scale{
    
    string name;
    
    bool steps[12];
    
    Scale() {
        for (int i = 0; i < 12; i++){
            steps[i] = false;
        }
    }
    
};

class GlobalScales {
    
public:
    
    vector<Scale> scaleVec;
    vector< vector<int> > scaleNotes;
    
    GlobalScales();
    
    void loadScales();
    
};




