
#include "SynthPresetManager.h"


SynthPresetManager::SynthPresetManager() {
    count = 6;
}


void SynthPresetManager::createSynth(int preset_,ofxTonicSynth& groupSynth_, Generator& output_, RampedValue& freq_, RampedValue& vol_, ControlGenerator& trigger_) {
    
    //default sinewave adder
    if (preset_ == 0) {
        
        attack = 0.007;
        ADSR adsr = ADSR(attack, 0.2, 0.3, 0.05).doesSustain(true).legato(true).trigger(trigger_);

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
        
        
        output_ =  (output_ + harmonic + harmonic2 + harmonic3) *vol_ *adsr;
    } else if(preset_ ==1 ){
        
        //simple squarewave
        attack = 0.009;
        ADSR adsr = ADSR(attack, 0.22, 0.3, 0.05).doesSustain(true).legato(true).trigger(trigger_);

        
        output_ = SquareWave().freq(freq_)*0.35;
        Generator har1 =   SquareWave().freq(freq_* 4 * vol_)*0.15;
        
        Generator har2 =   SquareWave().freq(freq_* 8)*0.15;

        
        output_  = (output_ +har1+har2) * 0.5 *vol_*adsr;
        
    } else if (preset_ == 2) {
        attack = 0.010;
        ADSR adsr = ADSR(attack, 0.3, 0.3, 0.05).doesSustain(false).legato(true).trigger(trigger_);

        //donald duck
        Generator outputGen = SineWave()
        .freq( freq_
              + (
                 SineWave().freq( freq_*2 *vol_) *
                 freq_/2 *
                 ( (1.0f + SineWave().freq((LFNoise().setFreq(0.5f) + 1.f) * 2.f + 0.2f)  )
                  )
                 )
              * ((SineWave().freq(0.15f) + 1.f) * 0.75f / vol_ + 0.25));
        
        output_ = (outputGen *vol_*adsr) *0.9;
    } else if (preset_ == 3) {
        attack = 0.015;
        ADSR adsr = ADSR(attack, 0.1, 0.3, 0.05).doesSustain(true).legato(true).trigger(trigger_);

        Generator outputGen = SineWave().freq( freq_+ ((LFNoise().setFreq(10.5)+10*vol_)*60) );
        Generator harmonic1 = SineWave().freq(freq_*vol_*2)*0.2;
        output_ =( outputGen )* 0.7 * vol_*adsr;
        
    } else if (preset_ == 4) {
        Generator hpNoise = (Noise() * dBToLin(-18.0)) >> HPF24().cutoff(3000.0) >> LPF12().cutoff(7500);
        Generator tones = SineWave().freq(freq_) * dBToLin(-6.0) + SineWave().freq(222) * dBToLin(-18.0);

        attack = 0.002;
        ADSR adsr = ADSR(attack, 0.1, 0.3, 0.05).doesSustain(false).legato(true).trigger(trigger_);
        
        
        output_ = (hpNoise+tones +(tones*0.5)  )*vol_ *adsr ;
    } else if (preset_ == 5) {
        
        attack = 0.005;
        ADSR adsr = ADSR(attack, 0.2, 0.3, 0.15).doesSustain(true).legato(true).trigger(trigger_);

        Compressor duckingComp = Compressor()
        .attack(0.01)
        .threshold( dBToLin(-12) )
        .ratio(16)
        .lookahead(0.001);
        Generator randomBass = (RectWave().freq( freq_ * SineWave().freq(3)) * 0.8) >> LPF24().cutoff( 2000 * (1 + ((SineWave().freq(0.1) + 1) * 0.5))).Q(1.5)  ;
        output_ = (randomBass >> duckingComp)*adsr *vol_;
    }
    
}