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

    
    void createSynth(int,ofxTonicSynth&, Generator&, RampedValue&, RampedValue&, ControlGenerator&, RampedValue&);
    
    float getPresetColor(float, int);
    
    float getPresetLfvf(int);
};