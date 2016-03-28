
#include "SynthPresetManager.h"

//SINE A & B global Generator from ofApp

SynthPresetManager::SynthPresetManager() {
    //update manual
    count = 4;
    
    //sampleTable
    
    int tableSize =1025;
    
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
        TonicFloat temp = 0;
        int sums = 50;
        
        for (int j = 0; j < sums; j++) {
            temp += sinf(phase*(float(j/2)))* powf((sums-float(j))/sums,4);
        }
        *sineSynthData++ = (temp*0.19);
        
        
        temp = 0;
      
        temp = sinf(phase) + (sinf(phase*2.133)*0.3) + (sinf(phase*3.977)*0.3) + (sinf(phase*6.1)*0.2) + (sinf(phase*8)*0.2) +  ((ofNoise(i*norm)*2-1)*0.1) ;
        
        
        *sineSynthData2++ = temp;
        
 
        //2. saw________________________________________
        
        
        temp = ( (((i *norm)*2-1) ) + (sinf(phase)*0.5) + (sinf(phase*12)*0.2) ) * (sinf(phase))+  ((ofNoise(i*norm)*2-1)*0.1) ;
        *tableSawData++ = temp;
        
     
      
        temp = ( (*(tableSineSimple.dataPointer()+i)*0.9) + (sinf(phase*2)*0.07)+ (sinf(phase*16)*0.025) + (sinf(phase*4)*0.1) + (sinf(phase*8)*0.05)   );
        
        *tableSawData2++ = temp*0.9;
        
        
       
        //6. snare
        
        temp = (*(tableNoiseSimple.dataPointer()+i))*sinf(phase*4)*cosf(phase*2)*4;
        

        *snareData++ = temp*3;
        
       

        
        temp = (*(tableNoiseSimple.dataPointer()+i)*sinf(phase*4))*sinf(phase*2);
        
        *snareData2++ = temp*3;

        if (temp > tempVolPlus) {
            tempVolPlus = temp;
        }
        
        if (temp < tempVolMinus) {
            tempVolMinus= temp;
        }
        
        //8. bell
        
        temp =         ((sinf(phase)*0.1)+ (sinf(phase*0.5)*0.002) +(sinf(phase)*0.02) + (sinf(phase*0.7)*0.001))*3;
        *bell1Data ++ = temp;
        
        
        temp =  ((sinf(phase*2.49)*0.02) +(sinf(phase*11)*0.04) + (sinf(phase*2.571)*0.02)) + ((sinf(phase*2.002)*0.008) +(sinf(phase*3)*0.02) + (sinf(phase*9.6)*0.004))*2;
        *bell2Data ++ = temp*4;
        
      
    }
    
    cout << tempVolPlus << " " << tempVolMinus << endl;
    
    
}


void SynthPresetManager::createSynth(int preset_,ofxTonicSynth& groupSynth_, Generator& output_, RampedValue& freq_, RampedValue& vol_, ControlGenerator& trigger_, RampedValue& freqVolFac_) {
    
    //1. sine synth__50 sine adder phase_* 0.5____________________________________________
    if (preset_ == 0) {
        
        attack = 0.32;
        ADSR adsr = ADSR(attack, 0.15, 0.55, 0.12).doesSustain(true).legato(true).trigger(trigger_).exponential(0);
        ADSR adsr2 = ADSR(0.05, 0.1, 0.42, 0.20).doesSustain(false).legato(true).trigger(trigger_).exponential(1);

        TableLookupOsc sine = TableLookupOsc().setLookupTable(sineSynth).freq(freq_  );
        TableLookupOsc sine2 = TableLookupOsc().setLookupTable(tableSineSimple).freq(freq_  );

        
        output_ = ((adsr*sine) + (adsr2*sine2))    *vol_;
    }
    //2. simple squarewave_______________________________________________
    else if(preset_ ==1 ){
        
        attack = 0.05;
        ADSR adsr = ADSR(attack, 0.08, 0.6, 0.05).doesSustain(false).legato(true).trigger(trigger_);
        ADSR adsr2 = ADSR(0.15, 0.05, 0.3, 0.25).doesSustain(false).legato(true).trigger(trigger_);
        
        
        TableLookupOsc myTable = TableLookupOsc().setLookupTable(tableSaw).freq(freq_);
        
        TableLookupOsc myTable2 = TableLookupOsc().setLookupTable(tableSaw2).freq(freq_);
        
        
        //  output_  =  (myTable  * adsr * (0.9+ ((*sineA_+1)/20) )) + ((myTable  * adsr2 * (0.8+ ((*sineA_+1)/10) ))*0.67>>BPF12().Q(10).cutoff(freq_*1.231));
        
        output_  = ((myTable2*adsr2)+(myTable*adsr)  )   *vol_;
        
        
    }
 
  
    //6. snare_______________________________________________
    
    else if (preset_ == 2) {
        attack = 0.0001;
        
        ADSR adsr = ADSR(attack, 0.05, 0.61, 0.003).doesSustain(false).legato(true).trigger(trigger_).exponential(0);
        ADSR adsr2 = ADSR(0.0002, 0.10, 0.7, 0.009).doesSustain(false).legato(true).trigger(trigger_);

        
        TableLookupOsc snareOsc = TableLookupOsc().setLookupTable(snare).freq(freq_);
        TableLookupOsc snareOsc2 = TableLookupOsc().setLookupTable(snare2).freq(freq_);


        output_ =  (snareOsc2*adsr )>>BPF12().cutoff(freq_).Q(9*vol_+1);
    }
    
    
    //bell test
    
    else if (preset_ == 3) {
        
        attack = 0.005;
        ADSR adsr1 = ADSR(attack, 0.08, 0.7, 1.0).doesSustain(false).legato(true).trigger(trigger_);
        ADSR adsr2 = ADSR(0.04, 0.08, 0.4, 0.75).doesSustain(false).legato(true).trigger(trigger_);
        
        
        TableLookupOsc bellAosc = TableLookupOsc().setLookupTable(bell1).freq(freq_);
        TableLookupOsc bellBosc = TableLookupOsc().setLookupTable(bell2).freq(freq_);
        
        
        //output_ = ((bellAosc*adsr1)+(bellBosc*adsr2)) * vol_;
        
        output_ =  (bellAosc*adsr1)+((bellBosc*adsr2))   *vol_;
        
    }
    
    
}
float SynthPresetManager::getPresetLfvf(int preset_) {
    float temp;
    if (preset_%count == 0) {
        temp = 2;
    } else if (preset_%count == 1) {
        temp = 3;
    } else if (preset_%count == 2) {
        temp = 125;
    } else if (preset_%count == 3) {
        temp = 5;
    }
    return temp;
}

float SynthPresetManager::getPresetColor(float hue_, int preset_){
    
    if (preset_ == 0) {
        hue_ = 8;
    } else if (preset_ == 1) {
        hue_ = 50;
    } else if (preset_ == 2) {
        hue_ = 138;
    } else if (preset_ == 3) {
        hue_ = 230;
    }
    return hue_;
    
}