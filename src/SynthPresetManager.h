#pragma  once
#include "ofMain.h"
#include "ofxTonic.h"

using namespace Tonic;

class SynthPresetManager {
    
    
public:
    SynthPresetManager();
    int count;
    
    void createSynth(int,ofxTonicSynth&, Generator&, RampedValue&, RampedValue&, ControlGenerator&);
};