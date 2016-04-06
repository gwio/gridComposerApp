//information for all patches

#pragma  once
#include "ofMain.h"
#include "ofxTonic.h"

using namespace Tonic;

class SynthPresetManager {
    
    
public:
    SynthPresetManager();
    int count;
    float attack;
    
    SampleTable tableSineSimple, tableSaw, tableSaw2, tableNoiseSimple, sineSynth, sineSynth2, snare, snare2,bell1, bell2;

    
    void createSynth(int,ofxTonicSynth&, Generator&, RampedValue&, RampedValue&, ControlGenerator&, ADSR&,ADSR&, ControlParameter&, ControlParameter&, ControlParameter&, ControlParameter&);
    
    float getPresetColor(float, int);
    
    float getPresetLfvf(int);
    
    float getPresetRelease(int);
    
    float getPresetAttack(int);
};