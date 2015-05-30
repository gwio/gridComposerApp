
#include "SynthPresetManager.h"


SynthPresetManager::SynthPresetManager() {
    //update manual
    count = 6;
}


void SynthPresetManager::createSynth(int preset_,ofxTonicSynth& groupSynth_, Generator& output_, RampedValue& freq_, RampedValue& vol_, ControlGenerator& trigger_) {
    
    //default sinewave adder
    if (preset_ == 0) {
        
        attack = 0.007;
        ADSR adsr = ADSR(attack, 0.2, 0.1, 0.05).doesSustain(true).legato(true).trigger(trigger_);

        output_ = SineWave().freq(freq_)*0.5;
      
        Generator harmonic = SineWave().freq(
                                             freq_ * 1.8
                                             ) * (0.35);
        
        Generator harmonic2 = SineWave().freq(
                                              freq_*4 + (vol_*40)
                                              ) * 0.1 ;
        
        
        
        output_ =  (output_+harmonic+harmonic2*adsr )*adsr*vol_;
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
        ADSR adsr = ADSR(attack, 0.12, 0.3, 0.05).doesSustain(false).legato(true).trigger(trigger_);

        //donald duck
        Generator outputGen = SineWave()
        .freq( freq_
              + (
                 SineWave().freq( freq_*1.2 *vol_) *
                 freq_/2 *
                 ( (1.0f + SineWave().freq((LFNoise().setFreq(0.5f) + 1.f) * 2.f + 0.2f)  )
                  )
                 )
              * ((SineWave().freq(0.15f) + 1.f) * 0.75f / vol_ + 0.25));
        
        //compressor
        Tonic::Compressor compressor = Compressor()
        .release(0.015)
        .attack(0.001)
        .threshold( dBToLin(-22) )
        .ratio(8)
        .lookahead(0.001)
        .bypass(false);

        
        output_ = (outputGen*adsr*vol_)>>compressor ;
    } else if (preset_ == 3) {
        attack = 0.015;
        ADSR adsr = ADSR(attack, 0.1, 0.3, 0.05).doesSustain(true).legato(true).trigger(trigger_);

        Generator outputGen = SineWave().freq( freq_+ ((LFNoise().setFreq(10.5)+10*vol_)*60) );
        output_ =( outputGen )* 0.7 * vol_*adsr;
        
    } else if (preset_ == 4) {
        
        Generator hpNoise = (Noise() * 0.75) >> HPF24().cutoff(freq_-3000.0) >> LPF12().cutoff(freq_+7500);

        attack = 0.00025;
        ADSR adsr = ADSR(attack, 0.035, 0.08, 0.03).doesSustain(false).legato(true).trigger(trigger_);
        ADSR adsrTone = ADSR(0.0015, 0.06, 0.03, 0.02).doesSustain(false).legato(true).trigger(trigger_);

        
        Generator tones = SineWave().freq(freq_) * 0.4 + SineWave().freq(freq_*0.8) * 0.23;

     
        //compressor
        Tonic::Compressor compressor = Compressor()
        .release(0.015)
        .attack(0.001)
        .threshold( dBToLin(-15) )
        .ratio(8)
        .lookahead(0.001)
        .bypass(false);
        
        output_ = (((hpNoise*adsr)+(tones*adsrTone))*vol_ )>>compressor ;
    } else if (preset_ == 5) {
        
        attack = 0.005;
        ADSR adsr = ADSR(attack, 0.1, 0.3, 0.15).doesSustain(false).legato(true).trigger(trigger_);

        
        Generator randomBass = (RectWave().freq( freq_ * SineWave().freq(3)) * 0.8) >> LPF24().cutoff( 2000 * (1 + ((SineWave().freq(0.1) + 1) * 0.5))).Q(1.5)  ;
        output_ = (randomBass)*adsr *vol_;
    }
    
}