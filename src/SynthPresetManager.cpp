
#include "SynthPresetManager.h"


SynthPresetManager::SynthPresetManager() {
    //update manual
    count = 8;
}


void SynthPresetManager::createSynth(int preset_,ofxTonicSynth& groupSynth_, Generator& output_, RampedValue& freq_, RampedValue& vol_, ControlGenerator& trigger_) {
    
    //default sinewave adder_______________________________________________
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
    }
    //simple squarewave_______________________________________________
    else if(preset_ ==1 ){
        
        attack = 0.009;
        ADSR adsr = ADSR(attack, 0.22, 0.22, 0.05).doesSustain(true).legato(true).trigger(trigger_);
        
        
        output_ = SquareWave().freq(freq_)*0.35;
        Generator har1 =   SquareWave().freq(freq_* 4 * vol_)*0.15;
        
        Generator har2 =   SquareWave().freq(freq_* 8)*0.15;
        
        
        output_  = (output_ +har1+har2) * 0.5 *vol_*adsr;
        
    }
    //dukken______________________________________________
    else if (preset_ == 2) {
        attack = 0.010;
        ADSR adsr = ADSR(attack, 0.12, 0.35, 0.05).doesSustain(false).legato(true).trigger(trigger_);
        
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
    }
    //whistler_______________________________________________
    
    else if (preset_ == 3) {
        attack = 0.015;
        ADSR adsr = ADSR(attack, 0.1, 0.3, 0.05).doesSustain(true).legato(true).trigger(trigger_);
        
        Generator outputGen = SineWave().freq( freq_+ ((LFNoise().setFreq(10.5)+10*vol_)*60) );
        output_ =( outputGen )* 0.7 * vol_*adsr;
        
    }
    //box_______________________________________________
    
    else if (preset_ == 4) {
        
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
    }
    //dbender_______________________________________________
    
    else if (preset_ == 5) {
        
        attack = 0.005;
        ADSR adsr = ADSR(attack, 0.1, 0.3, 0.15).doesSustain(false).legato(true).trigger(trigger_);
        
        
        Generator randomBass = (RectWave().freq( freq_ * SineWave().freq(3)) * 0.8) >> LPF24().cutoff( 2000 * (1 + ((SineWave().freq(0.1) + 1) * 0.5))).Q(1.5)  ;
        output_ = (randomBass)*adsr *vol_;
    }
    //snare_______________________________________________
    
    else if (preset_ == 6) {
        attack = 0.0015;
        
        Generator sine1 = SineWave().freq(20);
        Generator noise = Noise() >> BPF12().Q(1).cutoff( (1.2-vol_)*freq_*0.75);
        ADSR adsr = ADSR(attack, 0.075, 0.3, 0.15).doesSustain(false).legato(true).trigger(trigger_);
        
        Generator tone = SineWave().freq(freq_*1.65)*0.1*(1.2-vol_)*sine1;
        
        //compressor
        Tonic::Compressor compressor = Compressor()
        .release(0.015)
        .attack(0.0005)
        .threshold( dBToLin(-25) )
        .ratio(6)
        .lookahead(0.001)
        .bypass(false);

        
        output_ = ((noise*adsr*1.4)+(tone*adsr) *vol_)>>compressor;
    }
    
    //tableLookupTest_______________________________________________
    
    else if (preset_ == 7) {
        
        attack = 0.007;
        ADSR adsr = ADSR(attack, 0.2, 0.1, 0.05).doesSustain(true).legato(true).trigger(trigger_);
        
        int tableSize = 2500;
        
        SampleTable table = SampleTable(tableSize,1);
        
        TonicFloat norm = 1.0f / tableSize;
        TonicFloat* data = table.dataPointer();
        
        for (unsigned long i= 0; i < tableSize; i++) {
            *data++ = sinf(TWO_PI*i*norm);
        }
        
        TableLookupOsc osc = TableLookupOsc().setLookupTable(table).freq(freq_);
        
        output_ = osc*adsr*vol_;
    }

    
}