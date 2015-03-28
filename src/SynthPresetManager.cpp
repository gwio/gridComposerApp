
#include "SynthPresetManager.h"


SynthPresetManager::SynthPresetManager() {
    count = 3;
}


void SynthPresetManager::createSynth(int preset_,ofxTonicSynth& groupSynth_, Generator& output_, RampedValue& freq_, RampedValue& vol_) {
    
    
    if (preset_ == 0) {
    output_ = SineWave().freq(freq_)*0.5;
    
    
    Generator harmonic = SineWave().freq(
                                                       freq_ * 2 + (vol_*45)
                                                       ) * (0.25);
    
    Generator harmonic2 = SineWave().freq(
                                                        freq_*4
                                                        ) * 0.1 ;
    
    Generator harmonic3 = SineWave().freq(
                                                        freq_*6
                                                        )  *(0.1 * vol_);
    
    
    output_ =  (output_ + harmonic + harmonic2 + harmonic3) * vol_;
    } else if(preset_ ==1 ){
        
        output_ = SquareWave().freq(freq_)*0.25;
      Generator har1 =   SquareWave().freq(freq_* 4)*0.15;
        
        output_  = (output_ +har1) * 0.5 *vol_;
        
    } else if (preset_ == 2) {
        
        
        Generator outputGen = SineWave()
        .freq( freq_
              + (
                 SineWave().freq( freq_*2 *vol_) *
                 freq_/2 *
                 ( (1.0f + SineWave().freq((LFNoise().setFreq(0.5f) + 1.f) * 2.f + 0.2f)  )
                 )
              )
              * ((SineWave().freq(0.15f) + 1.f) * 0.75f + 0.25));
        
              output_ = outputGen *vol_;
    }

}