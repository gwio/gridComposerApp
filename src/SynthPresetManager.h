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
    
    SampleTable tableSineSimple, tableSaw, tableNoiseSimple, sineSynth, simpleSquare, whistler,sineSynth2, snare, bell1, bell2, dukken;

    
    void createSynth(int,ofxTonicSynth&, Generator&, RampedValue&, RampedValue&, ControlGenerator&, Generator*,  Generator*);
};