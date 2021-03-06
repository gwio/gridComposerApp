
#include "GlobalScales.h"




GlobalScales::GlobalScales() {
    
}

//--------------------------------------------------------------
void GlobalScales::loadScales() {
    
    Scale temp;
    vector<int> tempNotes;
    
    
    //___________________
    
    tempNotes.clear();
    temp.name = "IONIAN";
    {
        bool notes[] = {1,0,1,0,1,1,0,1,0,1,0,1};
        for (int i = 0; i < 12; i++) {
            temp.steps[i] = notes[i];
            if (notes[i]) {
                tempNotes.push_back(i);
            }
        }
    }
    scaleVec.push_back(temp);
    scaleNotes.push_back(tempNotes);
    
    //___________________
    
    tempNotes.clear();
    temp.name = "DORIAN";
    {
        bool notes[] = {1,0,1,1,0,1,0,1,0,1,1,0};
        for (int i = 0; i < 12; i++) {
            temp.steps[i] = notes[i];
            if (notes[i]) {
                tempNotes.push_back(i);
            }
        }
    }
    scaleVec.push_back(temp);
    scaleNotes.push_back(tempNotes);
    
    //___________________
    
    tempNotes.clear();
    temp.name = "PHRYGIAN";
    {
        bool notes[] = {1,1,0,1,0,1,0,1,1,0,1,0};
        for (int i = 0; i < 12; i++) {
            temp.steps[i] = notes[i];
            if (notes[i]) {
                tempNotes.push_back(i);
            }
        }
    }
    scaleVec.push_back(temp);
    scaleNotes.push_back(tempNotes);
    
    //___________________
    
    tempNotes.clear();
    temp.name = "LYDIAN";
    {
        bool notes[] = {1,0,1,0,1,0,1,1,0,1,0,1};
        for (int i = 0; i < 12; i++) {
            temp.steps[i] = notes[i];
            if (notes[i]) {
                tempNotes.push_back(i);
            }
        }
    }
    scaleVec.push_back(temp);
    scaleNotes.push_back(tempNotes);
    
    //___________________
    
    tempNotes.clear();
    temp.name = "MIXOLYDIAN";
    {
        bool notes[] = {1,0,1,0,1,1,0,1,0,1,1,0};
        for (int i = 0; i < 12; i++) {
            temp.steps[i] = notes[i];
            if (notes[i]) {
                tempNotes.push_back(i);
            }
        }
    }
    scaleVec.push_back(temp);
    scaleNotes.push_back(tempNotes);
    
    
    //___________________
    
    tempNotes.clear();
    temp.name = "AEOLIAN";
    {
        bool notes[] = {1,0,1,1,0,1,0,1,1,0,1,0};
        for (int i = 0; i < 12; i++) {
            temp.steps[i] = notes[i];
            if (notes[i]) {
                tempNotes.push_back(i);
            }
        }
    }
    scaleVec.push_back(temp);
    scaleNotes.push_back(tempNotes);
    
    //___________________
    
    
    tempNotes.clear();
    temp.name = "LOCRIAN";
    {
        bool notes[] = {1,1,0,1,0,1,1,0,1,0,1,0};
        for (int i = 0; i < 12; i++) {
            temp.steps[i] = notes[i];
            if (notes[i]) {
                tempNotes.push_back(i);
            }
        }
    }
    scaleVec.push_back(temp);
    scaleNotes.push_back(tempNotes);
    
    
    
    //_____________________
    
    tempNotes.clear();
    temp.name = "M PENTATONIC";
    {
        bool notes[] = {1,0,1,0,1,0,0,1,0,1,0,0};
        for (int i = 0; i < 12; i++) {
            temp.steps[i] = notes[i];
            if (notes[i]) {
                tempNotes.push_back(i);
            }
        }
    }
    scaleVec.push_back(temp);
    scaleNotes.push_back(tempNotes);
    
    
    //___________________
    
    
    tempNotes.clear();
    temp.name = "m PENTATONIC";
    {
        bool notes[] = {1,0,0,1,0,1,0,1,0,0,1,0};
        for (int i = 0; i < 12; i++) {
            temp.steps[i] = notes[i];
            if (notes[i]) {
                tempNotes.push_back(i);
            }
        }
    }
    scaleVec.push_back(temp);
    scaleNotes.push_back(tempNotes);
    
    //___________________
    
    
    tempNotes.clear();
    temp.name = "m HARMONIC";
    {
        bool notes[] = {1,0,1,1,0,1,0,1,1,0,0,1};
        for (int i = 0; i < 12; i++) {
            temp.steps[i] = notes[i];
            if (notes[i]) {
                tempNotes.push_back(i);
            }
        }
    }
    scaleVec.push_back(temp);
    scaleNotes.push_back(tempNotes);
    
    //___________________
    
    
    tempNotes.clear();
    temp.name = "m MELODIC";
    {
        bool notes[] = {1,0,1,1,0,1,0,1,0,1,0,1};
        for (int i = 0; i < 12; i++) {
            temp.steps[i] = notes[i];
            if (notes[i]) {
                tempNotes.push_back(i);
            }
        }
    }
    scaleVec.push_back(temp);
    scaleNotes.push_back(tempNotes);
    
    //___________________
    
    
    tempNotes.clear();
    temp.name = "HUNGARIAN";
    {
        bool notes[] = {1,0,1,1,0,0,1,1,1,0,1,0};
        for (int i = 0; i < 12; i++) {
            temp.steps[i] = notes[i];
            if (notes[i]) {
                tempNotes.push_back(i);
            }
        }
    }
    scaleVec.push_back(temp);
    scaleNotes.push_back(tempNotes);
    
}
