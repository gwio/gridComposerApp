
#include "GlobalScales.h"




GlobalScales::GlobalScales() {
    
}

void GlobalScales::loadScales() {
    
    Scale temp;
    vector<int> tempNotes;
    
    tempNotes.clear();
    temp.name = "Ionian";
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
    
    
    tempNotes.clear();
    temp.name = "Dorian";
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
    
    tempNotes.clear();
    temp.name = "Dorian Pentatonic";
    {
        bool notes[] = {1,0,1,1,0,0,0,1,0,1,0,0};
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