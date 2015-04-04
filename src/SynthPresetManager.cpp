
#include "SynthPresetManager.h"


SynthPresetManager::SynthPresetManager() {
    count = 6;
}


void SynthPresetManager::createSynth(int preset_,ofxTonicSynth& groupSynth_, Generator& output_, RampedValue& freq_, RampedValue& vol_) {
    
    //default sinewave adder
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
        
        //simple squarewave
        
        output_ = SquareWave().freq(freq_)*0.25;
        Generator har1 =   SquareWave().freq(freq_* 4)*0.15;
        
        output_  = (output_ +har1) * 0.5 *vol_;
        
    } else if (preset_ == 2) {
        
        //donald duck
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
    } else if (preset_ == 3) {
        
        
        Generator outputGen = SineWave().freq( freq_+ ((LFNoise().setFreq(10.5)+10*vol_)*60) );
        Generator harmonic1 = SineWave().freq(freq_*vol_*2)*0.2;
        output_ =( outputGen )* 0.7 * vol_;
        
    } else if (preset_ == 4) {
        Generator hpNoise = (Noise() * dBToLin(-18.0)) >> HPF24().cutoff(3000.0) >> LPF12().cutoff(10000);
        Generator tones = SineWave().freq(freq_) * dBToLin(-6.0) + SineWave().freq(222) * dBToLin(-18.0);

      
        
        output_ = (hpNoise+tones +(tones*0.5)  )  *vol_;
    } else if (preset_ == 5) {
        
        Compressor duckingComp = Compressor()
        .attack(0.001)
        .threshold( dBToLin(-52) )
        .ratio(16)
        .lookahead(0.001);
        Generator randomBass = (RectWave().freq( freq_ ) * 0.8) >> LPF24().cutoff( 300 * (1 + ((SineWave().freq(0.1) + 1) * 0.5))).Q(1.5);
        output_ = (randomBass >> duckingComp) *vol_;
    }
    
}