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
    
    SampleTable tableSineSimple, tableNoiseSimple, sineSynth, simpleSquare, whistler,sineSynth2, snare;

    
    void createSynth(int,ofxTonicSynth&, Generator&, RampedValue&, RampedValue&, ControlGenerator&);
};