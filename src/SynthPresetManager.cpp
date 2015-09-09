
#include "SynthPresetManager.h"

//SINE A & B global Generator from ofApp

SynthPresetManager::SynthPresetManager() {
    //update manual
    count = 9;
    
    //sampleTable
    
    int tableSize =2049;
    
    tableSineSimple = SampleTable(tableSize,1);
    TonicFloat* tableSineData = tableSineSimple.dataPointer();

    tableNoiseSimple = SampleTable(tableSize,1);
    TonicFloat* tableNoiseData = tableNoiseSimple.dataPointer();
    
    tableSaw = SampleTable(tableSize,1);
    TonicFloat* tableSawData = tableSaw.dataPointer();

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
    
    bell1 = SampleTable(tableSize,1);
    TonicFloat* bell1Data = bell1.dataPointer();
    
    bell2 = SampleTable(tableSize,1);
    TonicFloat* bell2Data = bell2.dataPointer();
    
    dukken = SampleTable(tableSize,1);
    TonicFloat* dukkenData = dukken.dataPointer();
    
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
        *sineSynthData++ = temp*0.19;
       
        if (temp > tempVolPlus) {
            tempVolPlus = temp;
        }
        
        if (temp < tempVolMinus) {
            tempVolMinus= temp;
        }
        
        //2. saw________________________________________

        
        temp = (i *norm)*2-1;
        
        *tableSawData++ = temp;
        
    
       
        
        
        //3. dukken________________________________________
      temp =  (sinf(phase)* (phase*4)*0.5)+ofSignedNoise(i*0.001)+((*(sineSynth.dataPointer()+i))* (phase*4)*0.5*-1);
        temp -= 1.2;
        
        

        *dukkenData++ = temp*0.11;
        
       
        //4. whistler________________________________________

         sums = 66;
        temp = 0;
        
        for (int j = 1; j < sums; j++) {
            float amp = 1/(j);
            temp += (sinf(phase*j)*amp)+(sinf(phase*j*1.2)*amp*0.8)+ (sinf(phase*j*2.5)*amp*0.5)+ (sinf(phase*j*5.5)*amp*0.2)+ (sinf(phase*j*5.8)*amp*0.2);
        }

        temp +=0.12;
        
        *whistlerData++ = temp*0.49;
        
       
        //6. snare
        *snareData++ = (*(tableNoiseSimple.dataPointer()+i)*0.52)+((sinf(phase*1.86))+(sinf(phase*2.72))+(sinf(phase*3.64))+(sinf(phase*4.5))+(sinf(phase*5.46))*0.5)*0.22;
        

       
        *sineSynthData2++ = 0.4*sinf(phase) + 0.2*sinf(phase*2.1);
        
        //8. bell
    
        temp =         ((sinf(phase)*0.1)+ (sinf(phase*0.5)*0.002) +(sinf(phase)*0.02) + (sinf(phase*0.7)*0.001))*2;
        *bell1Data ++ = temp*5;
        
        
        
        
        temp =  ((sinf(phase*2.49)*0.02) +(sinf(phase*11)*0.04) + (sinf(phase*2.571)*0.02)) + ((sinf(phase*2.002)*0.008) +(sinf(phase*3)*0.02) + (sinf(phase*9.6)*0.004))*2;
        *bell2Data ++ = temp*7;

        
        
    }
    
    cout << tempVolPlus << " " << tempVolMinus << endl;


}


void SynthPresetManager::createSynth(int preset_,ofxTonicSynth& groupSynth_, Generator& output_, RampedValue& freq_, RampedValue& vol_, ControlGenerator& trigger_, Generator *sineA_,  Generator *sineB_) {
    
    //1. sine synth__50 sine adder phase_* 0.5____________________________________________
    if (preset_ == 0) {
        
        attack = 0.007;
        ADSR adsr = ADSR(attack, 0.21, 0.5, 0.09).doesSustain(true).legato(true).trigger(trigger_);
        
        TableLookupOsc sine = TableLookupOsc().setLookupTable(sineSynth).freq(freq_  );

        
        output_ =  sine * adsr * (0.9+ ((*sineA_+1)/20) ) ;
    }
    //2. simple squarewave_______________________________________________
    else if(preset_ ==1 ){
        
        attack = 0.019;
        ADSR adsr = ADSR(attack, 0.04, 0.62, 0.15).doesSustain(false).legato(true).trigger(trigger_);
        ADSR adsr2 = ADSR(0.33, 0.04, 0.32, 0.05).doesSustain(false).legato(true).trigger(trigger_);

        
        TableLookupOsc myTable = TableLookupOsc().setLookupTable(tableSaw).freq(freq_);
        
        TableLookupOsc myTable2 = TableLookupOsc().setLookupTable(tableSaw).freq(freq_);

        
      //  output_  =  (myTable  * adsr * (0.9+ ((*sineA_+1)/20) )) + ((myTable  * adsr2 * (0.8+ ((*sineA_+1)/10) ))*0.67>>BPF12().Q(10).cutoff(freq_*1.231));
        
        output_  =  ((myTable+myTable2*adsr**sineA_) + (myTable * adsr2**sineB_));

        
    }
    //3. dukken______________________________________________
    else if (preset_ == 2) {
        attack = 0.020;
        ADSR adsr = ADSR(attack, 0.12, 0.65, 0.05).doesSustain(true).legato(true).trigger(trigger_);
        
        TableLookupOsc dukkenTable = TableLookupOsc().setLookupTable(dukken).freq(freq_+((freq_*0.04)*(*sineA_)*vol_) );
        TableLookupOsc noiseT = TableLookupOsc().setLookupTable(tableNoiseSimple).freq(freq_);
        
        
        output_ = (dukkenTable*adsr* (0.9+ ((*sineA_+1)/20) )) + ((noiseT*adsr*0.8)>>BPF12().Q(8).cutoff(freq_));
      
    }
    //whistler_______________________________________________
    
    else if (preset_ == 3) {
        attack = 0.001;
        ADSR adsr = ADSR(attack, 0.15, 0.6, 0.025).doesSustain(false).legato(true).trigger(trigger_);
        
        ADSR adsr2 = ADSR(attack*24, 0.1, 0.1, 0.7).doesSustain(false).legato(true).trigger(trigger_);

        TableLookupOsc sineOSC = TableLookupOsc().freq(freq_).setLookupTable(sineSynth);
        TableLookupOsc whistler1 = TableLookupOsc().setLookupTable(tableNoiseSimple).freq(freq_);
        
        output_ = ((whistler1*adsr *0.1)+(sineOSC*adsr2 * (0.9+ ((*sineA_+1)/20) ) ))>>LPF6().Q(8).cutoff(freq_*10.5);
        
    }
    //box_______________________________________________
    
    else if (preset_ == 4) {
        
        
        attack = 0.00055;
        ADSR adsr = ADSR(attack, 0.105, 0.01, 0.03).doesSustain(false).legato(false).trigger(trigger_);
        
        TableLookupOsc noiseTable = TableLookupOsc().setLookupTable(tableNoiseSimple).freq(12*(freq_*0.3));
        
        output_ = (noiseTable>>HPF6().Q(22).cutoff(freq_))*adsr;
        
    }
    //dbender_______________________________________________
    
    else if (preset_ == 5) {
        
        attack = 0.05;
        ADSR adsr = ADSR(attack, 0.1, 0.8, 0.35).doesSustain(false).legato(true).trigger(trigger_);
        
        TableLookupOsc sine1 = TableLookupOsc().setLookupTable(tableSineSimple).freq(freq_+(vol_*freq_*0.2));
        
        output_ = (sine1)*(0.9+ ((*sineA_+1)/20));
    }
    //6. snare_______________________________________________
    
    else if (preset_ == 6) {
        attack = 0.0025;
        
        ADSR adsr = ADSR(attack, 0.105, 0.01, 0.03).doesSustain(false).legato(true).trigger(trigger_);

        TableLookupOsc snareOsc = TableLookupOsc().setLookupTable(snare).freq((freq_*0.8)*vol_);
        
        output_ = snareOsc*adsr;
    }
    
    //tableLookupTest_______________________________________________
    
    else if (preset_ == 7) {
        
        attack = 0.009;
        ADSR adsr = ADSR(attack, 0.09, 0.08, 0.05).doesSustain(false).legato(true).trigger(trigger_);
        

        TableLookupOsc sineS = TableLookupOsc().setLookupTable(sineSynth2).freq(freq_+(vol_*100));
        output_ = sineS*adsr **sineA_ **sineA_ **sineA_;
    }
    
    //bell test

    else if (preset_ == 8) {
        
        attack = 0.009;
        ADSR adsr1 = ADSR(0.0005, 0.0, 1.0, 1.0).doesSustain(false).legato(true).trigger(trigger_);
        ADSR adsr2 = ADSR(0.04, 0.0, 1.0, 0.5).doesSustain(false).legato(true).trigger(trigger_);

        
        TableLookupOsc bellAosc = TableLookupOsc().setLookupTable(bell1).freq(freq_);
        TableLookupOsc bellBosc = TableLookupOsc().setLookupTable(bell2).freq(freq_);

        
        //output_ = ((bellAosc*adsr1)+(bellBosc*adsr2));

        output_ = ((bellAosc*adsr1)*(0.9+ ((*sineA_+1)/20)))+((bellBosc*adsr1)*(0.9+ ((*sineA_+1)/20)))>>LPF6().Q(8).cutoff(freq_*7.5);

    }

    
}