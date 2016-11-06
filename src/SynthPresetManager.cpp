
#include "SynthPresetManager.h"

SynthPresetManager::SynthPresetManager() {
    //update manual
    count = 4;
    
    //sampleTable
    
    int tableSize =4097;
    
    //basic-noise
    tableNoiseSimple = SampleTable(tableSize,1);
    TonicFloat* tableNoiseData = tableNoiseSimple.dataPointer();
    
    //basic-sine
    tableSineSimple = SampleTable(tableSize,1);
    TonicFloat* tableSineData = tableSineSimple.dataPointer();
    
    
    
    //basic-saw
    tableSaw = SampleTable(tableSize,1);
    TonicFloat* tableSawData = tableSaw.dataPointer();
    
    tableSaw2 = SampleTable(tableSize,1);
    TonicFloat* tableSawData2 = tableSaw2.dataPointer();
    
    //sine
    sineSynth = SampleTable(tableSize,1);
    TonicFloat* sineSynthData = sineSynth.dataPointer();
    
    sineSynth2 = SampleTable(tableSize,1);
    TonicFloat* sineSynthData2 = sineSynth2.dataPointer();
    
    //snare
    snare = SampleTable(tableSize,1);
    TonicFloat* snareData = snare.dataPointer();
    
    snare2 = SampleTable(tableSize,1);
    TonicFloat* snareData2 = snare2.dataPointer();
    
    //bell
    bell1 = SampleTable(tableSize,1);
    TonicFloat* bell1Data = bell1.dataPointer();
    
    bell2 = SampleTable(tableSize,1);
    TonicFloat* bell2Data = bell2.dataPointer();
    
    
    TonicFloat norm = 1.0f / tableSize;
    
    float tempVolPlus = 0;
    float tempVolMinus = 0;
    for (unsigned long i= 0; i < tableSize; i++) {
        TonicFloat phase = TWO_PI*i*norm;
        
        *tableSineData++ = sinf(phase);
        *tableNoiseData++ = ((TonicFloat)rand()/RAND_MAX) * 2.0f - 1.0f;
        
        //1.sine________________________________________
        TonicFloat sineA = 0;
        int sums = 150;
        float amp = 1.0;
        for (int j = 0; j < sums; j++) {
            amp*=0.77;
            if(j%4==0) {
                sineA += sinf(phase*(j))*amp*2 ;
            } else {
                sineA += sinf(phase*(j*2))*amp*0.333 ;
            }
        }
        *sineSynthData++ = sineA;
        
        
        TonicFloat sineB = 0;
        
        sineB = ((sinf(phase*1.0)*0.55) + (sinf(phase*2.0312)*0.25) + (sinf(phase*4.0233)*0.25) + (sinf(phase*8.02334)*0.15) + (sinf(phase*16.023)*0.1))  ;
        
        
        *sineSynthData2++ = sineB;
        
        
        //2. saw________________________________________
        TonicFloat sawA = 0;
        TonicFloat sawB = 0;
        
        sawA =  (  ((((i*norm)*2-1))* abs(sinf(phase*2)))  + (sinf(phase)*0.35) + (sinf(phase*6)*0.15)  + (*(tableNoiseSimple.dataPointer()+i)*0.045)) * 1.85 ;
        *tableSawData++ = sawA;
        
        
        
        sawB = ( (*(tableSineSimple.dataPointer()+i)*0.75) + (sinf(phase*2)*0.14)+ (sinf(phase*10)*0.125) + (*(tableNoiseSimple.dataPointer()+i)*0.035) + (sinf(phase*4)*0.05)   );
        
        *tableSawData2++ = sawB;
    
        //6. snare
        
        TonicFloat snareA  = 0;
        TonicFloat snareB = 0;
        snareA = ((*(tableNoiseSimple.dataPointer()+i))*0.85) + (*(tableSaw.dataPointer()+i)*(cosf(phase*6))*0.15 ) ;
        
        
        *snareData++ = snareA*2.0;
    
        snareB = (*(tableNoiseSimple.dataPointer()+i)*sinf(phase*4))*sinf(phase*2);
        
        *snareData2++ = snareB*3;
     
        //8. bell
        
        TonicFloat bellA = 0;
        TonicFloat bellB = 0;
        bellA =         ((sinf(phase)*0.1)+ (sinf(phase*0.5)*0.002) +(sinf(phase)*0.02) + (sinf(phase*0.7)*0.001))*8;
        *bell1Data ++ = bellA;
        
        bellB =  ((sinf(phase*2.49)*0.02) +(sinf(phase*11)*0.04) + (sinf(phase*2.571)*0.02) + (sinf(phase*2.002)*0.008) +(sinf(phase*3)*0.02) + (sinf(phase*9.6)*0.004))*8.8;
        *bell2Data ++ = bellB;
        
        
        if (bellA > tempVolPlus) {
            tempVolPlus = bellA;
        }
        
        if (bellA < tempVolMinus) {
            tempVolMinus= bellA;
        }
        
    }
    
    cout << tempVolPlus << " " << tempVolMinus << endl;
    
    
    
}

//--------------------------------------------------------------
void SynthPresetManager::createSynth(int preset_,ofxTonicSynth& groupSynth_, Generator& output_, RampedValue& freq_, RampedValue& vol_, ControlGenerator& trigger_, ADSR& adsr1_, ADSR& adsr2_,
                                     ControlParameter& attack1_, ControlParameter& attack2_, ControlParameter& release1_, ControlParameter& release2_) {
    
    
    
    //decay adsr
    
    
    Generator tempGen;
    output_ = tempGen;
    
    //1. sine synth__50 sine adder phase_* 0.5____________________________________________
    if (preset_ == 0) {
        
        ADSR  adsr1 = ADSR().attack(attack1_).decay(attack1_).sustain(0.60).release(release1_).doesSustain(true).legato(true).trigger(trigger_).exponential(true);
        ADSR  adsr2 = ADSR().attack(attack2_).decay(attack2_*0.5).sustain(0.20).release(release2_*0.5).doesSustain(false).legato(true).trigger(trigger_).exponential(true);
        
        TableLookupOsc sine = TableLookupOsc().setLookupTable(sineSynth).freq(freq_);
        TableLookupOsc sine2 = TableLookupOsc().setLookupTable(sineSynth2).freq(freq_);
        
        
        tempGen = ((adsr1*sine) + (adsr2*sine2))*vol_*0.225;
        
    }
    
    
    //bell test
    
    else if (preset_ == 1) {
        
        ADSR   adsr1 = ADSR().attack(attack1_*0.55).decay(0.0).sustain(1.0).release(release1_*1.25).doesSustain(false).legato(true).trigger(trigger_).exponential(true);
        ADSR   adsr2 = ADSR().attack(attack2_).decay(0.0).sustain(1.0).release(release2_).doesSustain(false).legato(true).trigger(trigger_).exponential(true);
        
        TableLookupOsc bellAosc = TableLookupOsc().setLookupTable(bell1).freq(freq_);
        TableLookupOsc bellBosc = TableLookupOsc().setLookupTable(bell2).freq(freq_);
        
        
        //output_ = ((bellAosc*adsr1)+(bellBosc*adsr2)) * vol_;
        
        tempGen =  ((bellAosc*adsr1)+(bellBosc*adsr2))*vol_*0.25;
        
    }
    
    //2. simple squarewave_______________________________________________
    else if(preset_ == 2){
        
        
        ADSR  adsr1 = ADSR().attack(attack1_*0.75).decay(attack1_*0.5).sustain(0.5).release(release1_*2).doesSustain(false).legato(true).trigger(trigger_).exponential(true);
        ADSR  adsr2 = ADSR().attack(attack2_).decay(attack2_).sustain(0.54).release(release2_/2).doesSustain(false).legato(true).trigger(trigger_).exponential(true);
        
        
        TableLookupOsc myTable = TableLookupOsc().setLookupTable(tableSaw).freq(freq_);
        
        TableLookupOsc myTable2 = TableLookupOsc().setLookupTable(tableSaw2).freq(freq_);
        
        
        //  output_  =  (myTable  * adsr * (0.9+ ((*sineA_+1)/20) )) + ((myTable  * adsr2 * (0.8+ ((*sineA_+1)/10) ))*0.67>>BPF12().Q(10).cutoff(freq_*1.231));
        
        tempGen  = ((myTable*adsr1)+(myTable2*adsr2))*vol_*0.25 ;
        
        
    }
    
    
    //6. snare_______________________________________________
    
    else if (preset_ == 3) {
        
        ADSR  adsr1 = ADSR().attack(attack1_*1.0).decay(attack1_*1.5).sustain(0.035).release(release1_).doesSustain(false).legato(false).trigger(trigger_).exponential(true);
        ADSR  adsr2 = ADSR().attack(attack2_*0.88).decay(attack1_*0.88).sustain(0.05).release(release2_).doesSustain(false).legato(false).trigger(trigger_).exponential(true);
        
        
        TableLookupOsc sawOsc = TableLookupOsc().setLookupTable(tableSaw).freq(freq_);
        TableLookupOsc snareOsc2 = TableLookupOsc().setLookupTable(tableNoiseSimple).freq(freq_*0.01);
        
        
        //tempGen  = ((snareOsc*adsr2*0.34*vol_))>>BPF12().cutoff(1500).Q(0.7-(0.3*vol_));
        tempGen  = ((sawOsc*adsr2*0.5)+ ((snareOsc2*adsr1)>>BPF12().cutoff(1500).Q(0.85)) )*vol_*0.625;
        //tempGen  = ((snareOsc2*adsr1)>>BPF12().cutoff(1500).Q(0.85))*vol_*0.4;
        //tempGen  = ((sawOsc*adsr2))*vol_*0.4;
        
    }
    
    output_ = tempGen;
}

//--------------------------------------------------------------
float SynthPresetManager::getPresetLfvf(int preset_) {
    float temp;
    if (preset_%count == 0) {
        temp = 2;
    } else if (preset_%count == 1) {
        temp = 2;
    } else if (preset_%count == 2) {
        temp = 2;
    } else if (preset_%count == 3) {
        temp = 3.15;
    }
    return temp;
}

//--------------------------------------------------------------
float SynthPresetManager::getPresetColor(float hue_, int preset_){
    
    float hue;
    if (preset_ == 0) {
        hue = 9;
    } else if (preset_ == 1) {
        // hue = 111;
        hue = 232;
    } else if (preset_ == 2) {
        //hue = 137;
        hue = 35;
    } else if (preset_ == 3) {
        hue = 137;
    }
    return hue;
}

//--------------------------------------------------------------
float SynthPresetManager::getPresetRelease(int preset_){
    float rel;
    if (preset_%count == 0) {
        rel = 0.95;
    } else if (preset_%count == 1) {
        rel = 1.5;
    } else if (preset_%count == 2) {
        rel = 0.185;
    } else if (preset_%count == 3) {
        rel = 0.04;
    }
    return rel;
}

//--------------------------------------------------------------
float SynthPresetManager::getPresetAttack(int preset_){
    float att;
    if (preset_%count == 0) {
        att =  0.5;
    } else if (preset_%count == 1) {
        att = 0.125;
    } else if (preset_%count == 2) {
        att = 0.15;
    } else if (preset_%count == 3) {
        att = 0.085;
    }
    return att;
}
