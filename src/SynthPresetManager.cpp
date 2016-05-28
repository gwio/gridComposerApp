
#include "SynthPresetManager.h"

//SINE A & B global Generator from ofApp

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
        int sums = 80;
        
        for (int j = 0; j < sums; j++) {
            if(j%3 == 0){
            sineA += sinf(phase*(float(j/2)))* powf((sums-float(j+1))/sums,4)*0.30;
            }else {
        sineA += sinf(phase*(float(j/2)))* (powf((sums-float(j+1))/sums,4)/4)*0.080;
            }
        }
        *sineSynthData++ = sineA;
        
        
        TonicFloat sineB = 0;
        
        sineB = ((sinf(phase)*0.7) + (sinf(phase*2.133)*0.245) + (sinf(phase*3.977)*0.27) + (sinf(phase*6.1)*0.15) + (sinf(phase*8)*0.176)) *0.885 ;
        
        
        *sineSynthData2++ = sineB;
        
        
      
        
        //2. saw________________________________________
        TonicFloat sawA = 0;
        TonicFloat sawB = 0;
        
        sawA =  (((((i*norm)*2-1))*0.55) + (sinf(phase)*0.35) + (sinf(phase*6)*0.15)  + (*(tableNoiseSimple.dataPointer()+i)*0.05)) * 1.55 ;
        *tableSawData++ = sawA;
        
        
        
        sawB = ( (*(tableSineSimple.dataPointer()+i)*0.75) + (sinf(phase*2)*0.14)+ (sinf(phase*10)*0.125) + (*(tableNoiseSimple.dataPointer()+i)*0.045) + (sinf(phase*4)*0.05)   );
        
        *tableSawData2++ = sawB;
        
      
        
        //6. snare
        
        TonicFloat snareA  = 0;
        TonicFloat snareB = 0;
        snareA = (*(tableNoiseSimple.dataPointer()+i))*sinf(phase*4)*cosf(phase*2)*1.25;
        
        
        *snareData++ = snareA;
        
        
        
        
        snareB = (*(tableNoiseSimple.dataPointer()+i)*sinf(phase*4))*sinf(phase*2);
        
        *snareData2++ = snareB*3;
        
        
        
        //8. bell
        
        TonicFloat bellA = 0;
        TonicFloat bellB = 0;
        bellA =         ((sinf(phase)*0.1)+ (sinf(phase*0.5)*0.004) +(sinf(phase)*0.04) + (sinf(phase*0.7)*0.002))*6;
        *bell1Data ++ = bellA;
        
        bellB =  ((sinf(phase*2.49)*0.02) +(sinf(phase*11)*0.04) + (sinf(phase*2.571)*0.02) + (sinf(phase*2.002)*0.008) +(sinf(phase*3)*0.02) + (sinf(phase*9.6)*0.004))*8.8;
        *bell2Data ++ = bellB;
        
        
        if (bellB > tempVolPlus) {
            tempVolPlus = bellB;
        }
        
        if (bellB < tempVolMinus) {
            tempVolMinus= bellB;
        }
    }
    
    cout << tempVolPlus << " " << tempVolMinus << endl;
    
    
    
}


void SynthPresetManager::createSynth(int preset_,ofxTonicSynth& groupSynth_, Generator& output_, RampedValue& freq_, RampedValue& vol_, ControlGenerator& trigger_, ADSR& adsr1_, ADSR& adsr2_,
                                     ControlParameter& attack1_, ControlParameter& attack2_, ControlParameter& release1_, ControlParameter& release2_) {
    
    
    
    //decay adsr
    
    
    Generator tempGen;
    output_ = tempGen;
    
    //1. sine synth__50 sine adder phase_* 0.5____________________________________________
    if (preset_ == 0) {
        
        ADSR  adsr1 = ADSR().attack(attack1_).decay(attack1_).sustain(0.60).release(release1_).doesSustain(true).legato(true).trigger(trigger_).exponential(true);
        ADSR  adsr2 = ADSR().attack(attack2_).decay(attack2_).sustain(0.20).release(release2_).doesSustain(true).legato(true).trigger(trigger_).exponential(true);
        
        TableLookupOsc sine = TableLookupOsc().setLookupTable(sineSynth).freq(freq_);
        TableLookupOsc sine2 = TableLookupOsc().setLookupTable(sineSynth2).freq(freq_);
        
        
        tempGen = ((adsr1*sine) + (adsr2*sine2))*vol_*0.0385;
        
    }
    //2. simple squarewave_______________________________________________
    else if(preset_ == 1){
        
        
        ADSR  adsr1 = ADSR().attack(attack1_*0.75).decay(attack1_).sustain(0.35).release(release1_/2).doesSustain(false).legato(true).trigger(trigger_).exponential(true);
        ADSR  adsr2 = ADSR().attack(attack2_).decay(attack2_*2).sustain(0.54).release(release2_*2).doesSustain(false).legato(true).trigger(trigger_).exponential(true);
        
        
        TableLookupOsc myTable = TableLookupOsc().setLookupTable(tableSaw).freq(freq_);
        
        TableLookupOsc myTable2 = TableLookupOsc().setLookupTable(tableSaw2).freq(freq_);
        
        
        //  output_  =  (myTable  * adsr * (0.9+ ((*sineA_+1)/20) )) + ((myTable  * adsr2 * (0.8+ ((*sineA_+1)/10) ))*0.67>>BPF12().Q(10).cutoff(freq_*1.231));
        
        tempGen  = ((myTable*adsr1)+(myTable2*adsr2))*vol_ *0.0575;
        
        
    }
    
    
    //6. snare_______________________________________________
    
    else if (preset_ == 2) {
        
        ADSR  adsr1 = ADSR().attack(attack1_).decay(0.02).sustain(0.1).release(release1_).doesSustain(false).legato(true).trigger(trigger_).exponential(true);
        ADSR  adsr2 = ADSR().attack(attack2_).decay(attack2_*4).sustain(0.05).release(release2_).doesSustain(false).legato(true).trigger(trigger_).exponential(true);
        
        
        TableLookupOsc snareOsc = TableLookupOsc().setLookupTable(snare).freq(freq_);
        TableLookupOsc snareOsc2 = TableLookupOsc().setLookupTable(snare2).freq(freq_);
        
        
        tempGen  =  (snareOsc*adsr2*0.45)>>BPF12().cutoff(freq_).Q(0.5-(0.2*vol_));
    }
    
    
    //bell test
    
    else if (preset_ == 3) {
        
        ADSR   adsr1 = ADSR().attack(attack1_*0.85).decay(attack1_).sustain(0.65).release(release1_).doesSustain(false).legato(true).trigger(trigger_).exponential(true);
        ADSR   adsr2 = ADSR().attack(attack2_).decay(attack2_*0.5).sustain(0.4).release(release2_).doesSustain(false).legato(true).trigger(trigger_).exponential(true);
        
        TableLookupOsc bellAosc = TableLookupOsc().setLookupTable(bell1).freq(freq_);
        TableLookupOsc bellBosc = TableLookupOsc().setLookupTable(bell2).freq(freq_);
        
        
        //output_ = ((bellAosc*adsr1)+(bellBosc*adsr2)) * vol_;
        
        tempGen =  ((bellAosc*adsr1)+(bellBosc*adsr2))*vol_*0.08;
        
    }
    
    output_ = tempGen;
}
float SynthPresetManager::getPresetLfvf(int preset_) {
    float temp;
    if (preset_%count == 0) {
        temp = 0;
    } else if (preset_%count == 1) {
        temp = 0;
    } else if (preset_%count == 2) {
        temp = 4;
    } else if (preset_%count == 3) {
        temp = 0;
    }
    return temp;
}

float SynthPresetManager::getPresetColor(float hue_, int preset_){
    
    float hue;
    if (preset_ == 0) {
        hue = 9;
    } else if (preset_ == 1) {
        hue = 111;
       //? hue = 35;
    } else if (preset_ == 2) {
        hue = 137;
    } else if (preset_ == 3) {
        hue = 232;
    }
    return hue;
}

float SynthPresetManager::getPresetRelease(int preset_){
    float rel;
    if (preset_%count == 0) {
        rel = 0.95;
    } else if (preset_%count == 1) {
        rel = 0.085;
    } else if (preset_%count == 2) {
        rel = 0.05;
    } else if (preset_%count == 3) {
        rel = 1.5;
    }
    return rel;
}

float SynthPresetManager::getPresetAttack(int preset_){
    float att;
    if (preset_%count == 0) {
        att =  0.4;
    } else if (preset_%count == 1) {
        att = 0.125;
    } else if (preset_%count == 2) {
        att = 0.05;
    } else if (preset_%count == 3) {
        att = 0.1;
    }
    return att;
}