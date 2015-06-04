
#include "SynthPresetManager.h"


SynthPresetManager::SynthPresetManager() {
    //update manual
    count = 8;
    
    //sampleTable
    
    int tableSize =4097;
    
    tableSineSimple = SampleTable(tableSize,1);
    TonicFloat* tableSineData = tableSineSimple.dataPointer();

    tableNoiseSimple = SampleTable(tableSize,1);
    TonicFloat* tableNoiseData = tableNoiseSimple.dataPointer();

    sineSynth = SampleTable(tableSize,1);
    TonicFloat* sineSynthData = sineSynth.dataPointer();
    
    sineSynth2 = SampleTable(tableSize,1);
    TonicFloat* sineSynthData2 = sineSynth2.dataPointer();

    simpleSquare = SampleTable(tableSize,1);
    TonicFloat* simpleSquareData = simpleSquare.dataPointer();
    
    whistler = SampleTable(tableSize,1);
    TonicFloat* whistlerData = whistler.dataPointer();
    
    snare = SampleTable(tableSize,1);
    TonicFloat* snareData = snare.dataPointer();
    
    TonicFloat norm = 1.0f / tableSize;
    
    for (unsigned long i= 0; i < tableSize; i++) {
        TonicFloat phase = TWO_PI*i*norm;

        
        *tableSineData++ = sinf(phase);
        *tableNoiseData++ = ((TonicFloat)rand()/RAND_MAX) * 2.0f - 1.0f;

        //1.sine
        TonicFloat temp = 0;
        int sums = 50;
        for (int j = 1; j < sums; j++) {
            temp += sinf(phase*(float(j/2)))* powf((sums-float(j))/sums,4);
        }
        *sineSynthData++ = temp*0.25;
        
        //2. squares
        temp=0;
        sums = 12;
        for (int j = 0; j < sums; j++) {
            if (i*(norm*j) > 0.5) {
                temp += 1.0 * powf((sums-float(j))/sums,2);
            } else {
                temp += -1.0 * powf((sums-float(j))/sums,2);
            }
            
        }
        *simpleSquareData++ = temp* 0.25;
        cout << temp << endl;
        //3. whistler
        
        //4. box
        *snareData++ = ((*tableNoiseSimple.dataPointer()+i*0.8)+(*tableSineSimple.dataPointer()+i*0.2))*ofSignedNoise(i*0.0001);
        
        *whistlerData++ =  (0.5*sinf(phase)* ofSignedNoise(i*0.0001))+(0.5*sinf(phase*2)* ofSignedNoise(i*0.001));
        *sineSynthData2++ = 0.4*sinf(phase) + 0.2*sinf(phase*2.1);
                                                                        


    }
    

}


void SynthPresetManager::createSynth(int preset_,ofxTonicSynth& groupSynth_, Generator& output_, RampedValue& freq_, RampedValue& vol_, ControlGenerator& trigger_) {
    
    //1. sine synth__50 sine adder phase_* 0.5____________________________________________
    if (preset_ == 0) {
        
        attack = 0.007;
        ADSR adsr = ADSR(attack, 0.3, 0.5, 0.05).doesSustain(true).legato(true).trigger(trigger_);
        
        TableLookupOsc sine = TableLookupOsc().setLookupTable(sineSynth).freq(freq_);

        
        output_ =  sine*adsr*vol_;
    }
    //2. simple squarewave_______________________________________________
    else if(preset_ ==1 ){
        
        attack = 0.019;
        ADSR adsr = ADSR(attack, 0.02, 0.82, 0.05).doesSustain(true).legato(true).trigger(trigger_);
        
        
        TableLookupOsc myTable = TableLookupOsc().setLookupTable(simpleSquare).freq(freq_);
        
        output_  =  (((myTable*2)>>BPF12().Q(22*(1.15-vol_)).cutoff(freq_))+(myTable*0.3))*adsr*vol_;
        
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
        
        TableLookupOsc sine1 = TableLookupOsc().setLookupTable(tableSineSimple).freq(40);
        TableLookupOsc whistler1 = TableLookupOsc().setLookupTable(whistler).freq(freq_+(sine1*100));
        
        output_ = (whistler1*2)*adsr*vol_;
        
    }
    //box_______________________________________________
    
    else if (preset_ == 4) {
        
        
        attack = 0.00055;
        ADSR adsr = ADSR(attack, 0.105, 0.01, 0.03).doesSustain(false).legato(false).trigger(trigger_);
        
        
        TableLookupOsc noiseTable = TableLookupOsc().setLookupTable(tableNoiseSimple).freq(12*(freq_*0.3));
        
        
        output_ = (noiseTable>>HPF6().Q(22).cutoff(freq_))*adsr*vol_;
        
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
        attack = 0.00015;
        
        
//        TableLookupOsc sine1 = TableLookupOsc().setLookupTable(tableSine).freq(freq_*1.65);

  //      TableLookupOsc noise = TableLookupOsc().setLookupTable(tableNoise).freq(2) ;

        //Generator tone = sine1;
       // Generator noise4 = Noise() >> BPF12().Q(1).cutoff( (1.2-vol_)*freq_*0.75);
        ADSR adsr = ADSR(attack, 0.075, 0.3, 0.15).doesSustain(false).legato(true).trigger(trigger_);
        
        
        SampleTable myTable = SampleTable(513,1);
        
        TonicFloat norm = 1.0f / 513;
        TonicFloat* data = myTable.dataPointer();
        
        for (unsigned long i= 0; i < 513; i++) {
            TonicFloat phase = TWO_PI*i*norm;
            *data ++ = (0.2*sinf(phase))+((TonicFloat)rand()/RAND_MAX) * 2.0f - 1.0f;

        }
        
        TableLookupOsc snare = TableLookupOsc().setLookupTable(myTable).freq(freq_);
        TableLookupOsc snare2 = TableLookupOsc().setLookupTable(myTable).freq(4+(freq_*0.1));

       // Generator tone = SineWave().freq(freq_*1.65)*0.1*(1.2-vol_)*sine1;
        
        //compressor
        Tonic::Compressor compressor = Compressor()
        .release(0.015)
        .attack(0.0005)
        .threshold( dBToLin(-5) )
        .ratio(6)
        .lookahead(0.001)
        .bypass(false);

        
        output_ = (  (snare2+snare*adsr)>>BPF12().Q(1).cutoff(freq_*1.2) ) *vol_ ;
    }
    
    //tableLookupTest_______________________________________________
    
    else if (preset_ == 7) {
        
        attack = 0.009;
        ADSR adsr = ADSR(attack, 0.09, 0.08, 0.05).doesSustain(false).legato(true).trigger(trigger_);
        

        TableLookupOsc sineS = TableLookupOsc().setLookupTable(sineSynth2).freq(freq_+(vol_*100));
        output_ = sineS*adsr*vol_;
    }

    
}