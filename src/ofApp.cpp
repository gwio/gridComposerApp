#include "ofApp.h"
#define TILES 5
#define TILESIZE (100/TILES)
#define TILEBORDER 0.055
#define BPM (120)
#define ANI_SPEED 0.035
#define BPM_MAX 260
#define BPM_DIVISION_MAX 12.0
#define HISTORY_ROWS 35
#define HARMONY_ROWS_SCALE 0.777

#define ppPosModFocus 1.0
#define ppPosModDefault 1.0

#define ppScaleModFocus 1.0
#define ppScaleModDefault 1.0

#define ppScaleModTempo 4.0
#define ppPosModTempo 1.65


#define attSldMin 0.25
#define attSldMax 1.8
#define VERSION "1.00.52"

#define sWIDTH 2208
#define sHEIGHT 1242

enum currentState {
    STATE_DEFAULT,
    STATE_EDIT,
    STATE_VOLUME,
    STATE_EDIT_DETAIL,
    STATE_BPM,
    STATE_HARMONY,
    STATE_SAVE,
    STATE_SETTINGS
};




//--------------------------------------------------------------
void ofApp::setup(){
#if TARGET_OS_IPHONE
    ofSetOrientation(OF_ORIENTATION_90_RIGHT);
    setSampleRate(44100);
    ofSoundStreamSetup(2, 0, this, 44100, 256*2, 4);
    [[AVAudioSession sharedInstance] setCategory:AVAudioSessionCategoryPlayback error:nil];
    [[AVAudioSession sharedInstance] setActive:YES error:nil];
    [UIApplication sharedApplication].idleTimerDisabled = YES;
    
    swipping.setup();
    ofAddListener(swipping.swipeRecognized, this, &ofApp::onSwipe);
#else
    setSampleRate(44100);
    ofSoundStreamSetup(2, 0, this, 44100, 256*2, 4);
#endif
    
#if TARGET_OS_MAC
    // ofSetDataPathRoot("../Resources/data/");
#endif
    
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    //ofEnableDepthTest();
    ofEnableAlphaBlending();
    
    
    
    
    
    
    scaleCollection.loadScales();
    globalScaleVecPos = 0;
    
    makeDesignGrid();
    setupFonts();
    
    currentState = STATE_DEFAULT;
    appVersion = VERSION;
    
    synthPos.resize(3);
    
    camActiveFov = 33;
    camFov = 24;
    camEditDetailFov = 20;
    
    nearClip = 10;
    farClip = 10000;
    
    bpm = BPM;
    staticDelayValue = 0.5;
    dynamicDelayValue = 0.5;
    
    lastTouch = 0.0;
    sleepTimer = 0.0;
    
    dynamicVelo = true;
    globalKey = 0;
    globalVelo = 0.5;
    revTime = 0.5;
    revSize = 0.5;
    soundDelay = true;
    autoDelay = true;
    soundReverb = false;
    
    //global volume
    ControlParameter rampTarget = tonicSynth.addParameter("mainVolumeRamp").max(1.0).min(0.0);
    tonicSynth.setParameter("mainVolumeRamp", 0.0);
    mainVol = 0.0;
    volumeRamp = RampedValue().value(0.0).length(0.1).target(rampTarget);
    startUp = true;
    volumeRestart = 0.0;
    volumeRestartTarget = 0.0;
    
    ControlParameter delayTarget = tonicSynth.addParameter("delay");
    tonicSynth.setParameter("delay", 0.25);
    delayRamp = RampedValue().value(0.0).length(0.35).target(delayTarget);
    
    
    reverbTime = tonicSynth.addParameter("revTime");
    tonicSynth.setParameter("revTime", revTime);
    
    reverbSize = tonicSynth.addParameter("revSize");
    tonicSynth.setParameter("revSize", revSize);
    
    bpmpara = tonicSynth.addParameter("BPM");
    tonicSynth.setParameter("BPM", bpm*BPM_DIVISION_MAX);
    ControlGenerator pulse = ControlMetro().bpm(bpmpara);
    //hmm
    //
    //ControlGenerator pulseTriad = ControlMetro().bpm(bpmpara/4*3);
    //ControlGenerator pulseTriad = ControlMetro().bpm(bpmpara*0.666);
    
    //  1/4 -> 4x quarternote
    ControlMetroDivider pulseDiv1 = ControlMetroDivider().divisions(3).input(pulse);
    // 1/3  -> 3x  triads
    ControlMetroDivider pulseDiv2 = ControlMetroDivider().divisions(4).input(pulse);
    // 1/2 -> 2x halfnote
    ControlMetroDivider pulseDiv3 = ControlMetroDivider().divisions(6).input(pulse);
    // 1/1 -> 1x wholenote
    ControlMetroDivider pulseDiv4 = ControlMetroDivider().divisions(12).input(pulse);
    
    
    
    //  1/4 -> 4x quarternote
    ofEvent<float>* pulseEventDiv1 = tonicSynth.createOFEvent(pulseDiv1);
    ofAddListener(*pulseEventDiv1, this, &ofApp::pulseEventDiv1, OF_EVENT_ORDER_AFTER_APP );
    // 1/3  -> 3x  triads
    ofEvent<float>* pulseEventDiv2 = tonicSynth.createOFEvent(pulseDiv2);
    ofAddListener(*pulseEventDiv2, this, &ofApp::pulseEventDiv2, OF_EVENT_ORDER_AFTER_APP );
    // 1/2 -> 2x halfnote
    ofEvent<float>* pulseEventDiv3 = tonicSynth.createOFEvent(pulseDiv3);
    ofAddListener(*pulseEventDiv3, this, &ofApp::pulseEventDiv3, OF_EVENT_ORDER_AFTER_APP );
    // 1/1 -> 1x wholenote
    ofEvent<float>* pulseEventDiv4= tonicSynth.createOFEvent(pulseDiv4);
    ofAddListener(*pulseEventDiv4, this, &ofApp::pulseEventDiv4, OF_EVENT_ORDER_AFTER_APP );
    
    
    camNotActivePos = ofVec3f(0, -TILES*TILESIZE*2, TILES*TILESIZE*10.5);
    
    for (int i = -1; i < 2; i++) {
        ofNode temp;
        //temp.setPosition( i*(TILES*TILESIZE*2), 0, 0);
        temp.setPosition( intersectPlane( designGrid[i+1][1] ) );
        synthPos[i+1]=temp;
    }
    
    
    
    
    
    synthButton[0] = 0;
    synthButton[1] = 1;
    synthButton[2] = 2;
    
    synths.resize(3);
    
    synths[0] = Instrument(1,"a",TILES,TILESIZE,TILEBORDER,HISTORY_ROWS);
    synths[0].setup(&timeCounter, &tonicSynth, synthPos[0],&currentState,&bpm,&midiOut);
    synths[0].setMusicScale(scaleCollection, 0);
    synths[0].setKeyNote(60+globalKey-12);
    synths[0].ownSlot = 0;
    
    synths[1] = Instrument(2,"b",TILES,TILESIZE,TILEBORDER,HISTORY_ROWS);
    synths[1].setup(&timeCounter, &tonicSynth, synthPos[1],&currentState,&bpm,&midiOut);
    synths[1].setMusicScale(scaleCollection, 0);
    synths[1].setKeyNote(60+globalKey);
    synths[1].ownSlot = 1;
    
    synths[2] = Instrument(3,"c",TILES,TILESIZE,TILEBORDER,HISTORY_ROWS);
    synths[2].setup(&timeCounter, &tonicSynth, synthPos[2],&currentState,&bpm,&midiOut);
    synths[2].setMusicScale(scaleCollection, 0);
    synths[2].setKeyNote(60+globalKey+12);
    synths[2].ownSlot = 2;
    
    activeSynth = 1;
    
    
    makePresetString();
    
    setupStatesAndAnimation();
    setupGlobalInterface();
    
    synths[0].guiPtr = &mainInterfaceData;
    synths[1].guiPtr = &mainInterfaceData;
    synths[2].guiPtr = &mainInterfaceData;
    
    synths[0].uiState = &currentState;
    synths[1].uiState = &currentState;
    synths[2].uiState = &currentState;
    
    hvSlotA.setupMesh(&mainInterfaceData[75],HISTORY_ROWS,75);
    hvSlotA.setupSpacer(designGrid[0][1],HARMONY_ROWS_SCALE, designGrid[0][0]);
    hvSlotB.setupMesh(&mainInterfaceData[87],HISTORY_ROWS,87);
    hvSlotB.setupSpacer(designGrid[1][1],HARMONY_ROWS_SCALE, designGrid[0][0]);
    hvSlotC.setupMesh(&mainInterfaceData[99],HISTORY_ROWS,99);
    hvSlotC.setupSpacer(designGrid[2][1],HARMONY_ROWS_SCALE, designGrid[0][0]);
    //state_edit_detail
    hvSlotD = hvSlotB;
    
    //intersectplane
    planeForIntersect.set(TILES*TILESIZE,TILES*TILESIZE);
    thisIntersect.setFrom(planeForIntersect);
    
    
    ofBackground( ofColor(19,19,19) );
    
    light.setPosition(synthActivePos.getPosition()+ofVec3f(0,-100,1000));
    light.setDiffuseColor( ofColor(100,100,100));
    material.setShininess(55);
    // material.setSpecularColor( filterColor(ofColor::white));
    //temp sketch
    // light.setAmbientColor( filterColor(ofColor::white) );
    
    doubleClickTime = 300;
    curTap = 0;
    lastTap = 0;
    mouseDragging = false;
    tapCounter = 0;
    drawInfo = false;
    
    timeCounter = -1;
    sleepMode = false;
    
    focusCam = false;
    
    
    
    //  aniPct = 1.0;
    aniCam = 1.0;
    
    animCam = false;
    interfaceMoving = false;
    debugCam = false;
    insideSynth = false;
    
    
    // scaleFac = 1-(1/((designGrid[0][0].y*6)/144));
    scaleFac = designGrid[0][0].y/2;
    
    // scaleFac *= 0.35;
    scaleFac *= 1.0;
    
    iconSize = fontSizeBigger*0.52;
    
    //load icons
    ofImage tempImg;
    
    tempImg.loadImage("icons/back.png");
    backIcon.loadData(tempImg.getPixelsRef(),GL_RGBA);
    
    tempImg.loadImage("icons/left.png");
    tempImg.resize(iconSize*3.33, iconSize);
    left.loadData(tempImg.getPixelsRef(), GL_RGBA);
    
    tempImg.loadImage("icons/right.png");
    tempImg.resize(iconSize*3.33, iconSize);
    right.loadData(tempImg.getPixelsRef(), GL_RGBA);
    
    tempImg.loadImage("icons/leftDouble.png");
    tempImg.resize(iconSize*3.33, iconSize);
    leftDouble.loadData(tempImg.getPixelsRef(), GL_RGBA);
    
    tempImg.loadImage("icons/rightDouble.png");
    tempImg.resize(iconSize*3.33, iconSize);
    rightDouble.loadData(tempImg.getPixelsRef(), GL_RGBA);
    
    tempImg.loadImage("icons/pause.png");
    tempImg.resize(iconSize*3.33, iconSize);
    pause.loadData(tempImg.getPixelsRef(), GL_RGBA);
    
    tempImg.loadImage("icons/play.png");
    tempImg.resize(iconSize*3.33, iconSize);
    play.loadData(tempImg.getPixelsRef(), GL_RGBA);
    
    
    
    muster = MusterContainer(mainInterfaceData[39].drawStringPos, ofVec2f( mainInterfaceData[39].elementSize), TILES);
    muster.setup();
    
    //setup click zones for directions
    
    for (int i = 0; i < 4; i++) {
        ofVboMesh &aMesh = synths[0].pulsePlane.directionMeshConBig;
        directionClickZonesA[i].addVertex(testCam.worldToScreen((aMesh.getVertex((i*9)+0)*0.45*timeMatrix.getOrientationQuat())+OneTimePathOn.getVertices().back()));
        directionClickZonesA[i].addVertex(testCam.worldToScreen((aMesh.getVertex((i*9)+1)*0.45*timeMatrix.getOrientationQuat())+OneTimePathOn.getVertices().back()));
        directionClickZonesA[i].addVertex(testCam.worldToScreen((aMesh.getVertex((i*9)+3)*0.45*timeMatrix.getOrientationQuat())+OneTimePathOn.getVertices().back()));
        directionClickZonesA[i].addVertex(testCam.worldToScreen((aMesh.getVertex((i*9)+4)*0.45*timeMatrix.getOrientationQuat())+OneTimePathOn.getVertices().back()));
        directionClickZonesA[i].close();
        
        ofVboMesh &bMesh = synths[1].pulsePlane.directionMeshConBig;
        directionClickZonesB[i].addVertex(testCam.worldToScreen((bMesh.getVertex((i*9)+0)*0.45*timeMatrix.getOrientationQuat())+TwoTimePathOn.getVertices().back()));
        directionClickZonesB[i].addVertex(testCam.worldToScreen((bMesh.getVertex((i*9)+1)*0.45*timeMatrix.getOrientationQuat())+TwoTimePathOn.getVertices().back()));
        directionClickZonesB[i].addVertex(testCam.worldToScreen((bMesh.getVertex((i*9)+3)*0.45*timeMatrix.getOrientationQuat())+TwoTimePathOn.getVertices().back()));
        directionClickZonesB[i].addVertex(testCam.worldToScreen((bMesh.getVertex((i*9)+4)*0.45*timeMatrix.getOrientationQuat())+TwoTimePathOn.getVertices().back()));
        directionClickZonesB[i].close();
        
        
        ofVboMesh &cMesh = synths[2].pulsePlane.directionMeshConBig;
        directionClickZonesC[i].addVertex(testCam.worldToScreen((cMesh.getVertex((i*9)+0)*0.45*timeMatrix.getOrientationQuat())+ThreeTimePathOn.getVertices().back()));
        directionClickZonesC[i].addVertex(testCam.worldToScreen((cMesh.getVertex((i*9)+1)*0.45*timeMatrix.getOrientationQuat())+ThreeTimePathOn.getVertices().back()));
        directionClickZonesC[i].addVertex(testCam.worldToScreen((cMesh.getVertex((i*9)+3)*0.45*timeMatrix.getOrientationQuat())+ThreeTimePathOn.getVertices().back()));
        directionClickZonesC[i].addVertex(testCam.worldToScreen((cMesh.getVertex((i*9)+4)*0.45*timeMatrix.getOrientationQuat())+ThreeTimePathOn.getVertices().back()));
        directionClickZonesC[i].close();
        
    }
    
    //setup save manager, font files , size
    saveManager.setup(fontSizeSmall, fontSizeDefault, fontSizeBigger,
                      designGrid[0][0],
                      &tekoLight,&tekoRegular,
                      &tekoMedium,
                      &tekoSemibold,
                      &tekoBold,
                      &mainInterfaceData[48].drawStringPos);
    
    
    //setup stateBpm Fx Mesh
    bpmFx.setMode(OF_PRIMITIVE_LINES);
    bpmFx.clear();
    
    for (int i = 0; i < 8; i++) {
        bpmFx.addVertex(ofVec3f(-500,-500,0));
        bpmFx.addColor(ofColor(0,0,0,0));
    }
    
    
    //load previous settings
#if TARGET_OS_IPHONE
    loadFromXml(ofxiOSGetDocumentsDirectory()+"settings.xml", true);
#else
    loadFromXml("settings.xml",true);
#endif
    
    //load saves
#if TARGET_OS_IPHONE
    saveManager.loadSaveFolder(ofxiOSGetDocumentsDirectory());
#else
    saveManager.loadSaveFolder(";-)");
#endif
    
    
    
    sendMidi = true;
    //setNewGUI();
    setupAudio();
    setupMidi();
}

//--------------------------------------------------------------

void ofApp::setupFonts(){
    fontSizeDefault = designGrid[0][0].y/3.25;
    fontSizeSmall = fontSizeDefault*0.7;
    fontSizeBigger = fontSizeDefault*1.618;
    
    tekoLight.setup("fonts/Teko/Teko-Light.ttf", //font file, ttf only
                    1.0,					//lineheight percent
                    512,					//texture atlas dimension
                    false,					//create mipmaps of the font, useful to scale down the font at smaller sizes
                    8,					//texture atlas element padding, shouldbe >0 if using mipmaps otherwise
                    1.0f					//dpi scaleup, render textures @2x the reso
                    );				//lower res mipmaps wil bleed into each other
    
    
    tekoLight.setKerning(tekoLight.getKerning());
    tekoLight.setCharacterSpacing(fontSizeDefault/10);
    
    tekoRegular.setup("fonts/Teko/Teko-Regular.ttf", //font file, ttf only
                      1.0,					//lineheight percent
                      512,					//texture atlas dimension
                      false,					//create mipmaps of the font, useful to scale down the font at smaller sizes
                      8,					//texture atlas element padding, shouldbe >0 if using mipmaps otherwise
                      1.0f					//dpi scaleup, render textures @2x the reso
                      );				//lower res mipmaps wil bleed into each other
    
    
    tekoRegular.setKerning(tekoRegular.getKerning());
    tekoRegular.setCharacterSpacing(fontSizeDefault/16);
    
    tekoMedium.setup("fonts/Teko/Teko-Medium.ttf", //font file, ttf only
                     1.0,					//lineheight percent
                     512,					//texture atlas dimension
                     false,					//create mipmaps of the font, useful to scale down the font at smaller sizes
                     8,					//texture atlas element padding, shouldbe >0 if using mipmaps otherwise
                     1.0f					//dpi scaleup, render textures @2x the reso
                     );				//lower res mipmaps wil bleed into each other
    
    
    tekoMedium.setKerning(tekoMedium.getKerning());
    tekoMedium.setCharacterSpacing(fontSizeDefault/10);
    
    tekoSemibold.setup("fonts/Teko/Teko-SemiBold.ttf", //font file, ttf only
                       1.0,					//lineheight percent
                       512,					//texture atlas dimension
                       false,					//create mipmaps of the font, useful to scale down the font at smaller sizes
                       8,					//texture atlas element padding, shouldbe >0 if using mipmaps otherwise
                       1.0f					//dpi scaleup, render textures @2x the reso
                       );				//lower res mipmaps wil bleed into each other
    
    
    tekoSemibold.setKerning(tekoSemibold.getKerning());
    tekoSemibold.setCharacterSpacing(fontSizeDefault/12);
    
    tekoBold.setup("fonts/Teko/Teko-Bold.ttf", //font file, ttf only
                   1.0,					//lineheight percent
                   512,					//texture atlas dimension
                   false,					//create mipmaps of the font, useful to scale down the font at smaller sizes
                   8,					//texture atlas element padding, shouldbe >0 if using mipmaps otherwise
                   1.0f					//dpi scaleup, render textures @2x the reso
                   );				//lower res mipmaps wil bleed into each other
    
    
    tekoBold.setKerning(tekoBold.getKerning());
    tekoBold.setCharacterSpacing(fontSizeDefault/10);
    
    
    micon.setup("fonts/miconTest.ttf", //font file, ttf only
                1.0,					//lineheight percent
                512,					//texture atlas dimension
                false,					//create mipmaps of the font, useful to scale down the font at smaller sizes
                8,					//texture atlas element padding, shouldbe >0 if using mipmaps otherwise
                1.0f					//dpi scaleup, render textures @2x the reso
                );				//lower res mipmaps wil bleed into each other
    
    
    micon.setKerning(micon.getKerning());
    micon.setCharacterSpacing(fontSizeDefault/10);
    
}
//--------------------------------------------------------------

void ofApp::setupMidi(){
    
    
    
#if TARGET_OS_IPHONE
    ofxMidi::enableNetworking();
    midiOut.listPorts();
    midiOut.openPort(0);
#else
    midiOut.listPorts();
    midiOut.openPort(0);
#endif
}
//--------------------------------------------------------------

void ofApp::setupAudio(){
    
    Generator temp;
    for (int i = 0; i < synths.size(); i++) {
        temp = temp + (synths[i].instrumentOut*0.2);
    }
    mainOut = temp ;
    
    StereoDelay delay = StereoDelay(0.60,0.65)
    .delayTimeRight(delayRamp*1.1)
    .delayTimeLeft(delayRamp)
   // .feedback(0.2+(delayRamp*0.15))
    .feedback(0.15)
    .dryLevel(0.95)
    .wetLevel(0.25);
    
    
    Reverb rev = Reverb()
    .decayTime(reverbTime)
    .preDelayTime(0.02)
    .roomShape(0.425)
    .roomSize(reverbSize)
    //.inputLPFCutoff(6000)
    .dryLevel(0.0)
    .wetLevel(0.25)
    .density(0.25)
    .stereoWidth(0.75);
    
    
    //compressor
    Compressor compressor = Compressor()
    .release(0.015)
    .attack(0.001)
    .threshold( dBToLin(-30) )
    .ratio(4)
    .lookahead(0.001)
    .makeupGain(4.0)
    .bypass(false);
    
    
    
    
    Generator revMix;
    revMix = mainOut >> compressor >> rev;
    
    if(soundDelay && soundReverb){
        tonicSynth.setOutputGen( ( (((mainOut >>compressor >>delay)*0.825)+(revMix)) *volumeRamp)  >> HPF24().cutoff(35).Q(0.15) >> LPF24().cutoff(7500).Q(0.15)  );
    } else if(soundDelay){
        tonicSynth.setOutputGen( ((mainOut  >>compressor >> delay )*volumeRamp)  >> HPF24().cutoff(35).Q(0.15) >> LPF24().cutoff(7500).Q(0.15)  );
    }else if(soundReverb){
        tonicSynth.setOutputGen( ( (((mainOut  >>compressor)*0.825) + (revMix)) *volumeRamp)  >> HPF24().cutoff(35).Q(0.15) >> LPF24().cutoff(7500).Q(0.15)  );
    } else {
        tonicSynth.setOutputGen( ((mainOut  >>compressor  )*volumeRamp)  >> HPF24().cutoff(35).Q(0.15) >> LPF24().cutoff(7500).Q(0.15)  );
    }
    
    tonicSynth.setParameter("revTime", getRevTime(revTime));
    cout << "revTime:" << revTime << endl;
    tonicSynth.setParameter("revSize", getRevSize(revSize));
    cout << "revSize:" << revSize << endl;
    if(autoDelay){
        tonicSynth.setParameter("delay",getBpmValue(dynamicDelayValue));
        cout << "dynamic delay" << dynamicDelayValue << endl;
    } else {
        tonicSynth.setParameter("delay", getBpmValue(staticDelayValue));
        cout << "static delay" << staticDelayValue << endl;
    }
}

//--------------------------------------------------------------

void ofApp::updateBpmMenuMesh(){
    
    ofVec3f linePct;
    ofVec3f offset = ofVec3f(0,20,0);
    bpmFx.setVertex(0,mainInterface.getVertices().at(mainInterfaceData[45].counter+1)+offset);
    bpmFx.setVertex(1,mainInterface.getVertices().at(mainInterfaceData[45].counter+1)+offset);
    
    bpmFx.setColor(0, ofColor(255,255,255,255));
    bpmFx.setColor(1, ofColor(255,255,255,255));
    
    linePct.x = mainInterfaceData[55].elementSize.x * synths[synthButton[0]].pulsePlane.linePct;
    
    bpmFx.setVertex(2,mainInterface.getVertices().at(mainInterfaceData[55].counter+1)+offset);
    bpmFx.setVertex(3,mainInterface.getVertices().at(mainInterfaceData[55].counter+1)+offset+linePct);
    
    
    if (*synths[synthButton[0]].stepperPos != 5){
        bpmFx.setColor(2,ofColor::fromHsb(synths[synthButton[0]].colorHue, 200, 200, 255 ));
        bpmFx.setColor(3, ofColor::fromHsb(synths[synthButton[0]].colorHue, 200, 200, 255 ));
    } else {
        bpmFx.setColor(2,bpmFx.getColor(2).lerp(ofColor(0,0,0,0), 0.1));
        bpmFx.setColor(3,bpmFx.getColor(3).lerp(ofColor(0,0,0,0), 0.1));
    }
    
    linePct.x = mainInterfaceData[56].elementSize.x * synths[synthButton[1]].pulsePlane.linePct;
    bpmFx.setVertex(4,mainInterface.getVertices().at(mainInterfaceData[56].counter+1)+offset);
    bpmFx.setVertex(5,mainInterface.getVertices().at(mainInterfaceData[56].counter+1)+offset+linePct);
    
    if (*synths[synthButton[1]].stepperPos != 5){
        bpmFx.setColor(4,ofColor::fromHsb(synths[synthButton[1]].colorHue, 200, 200, 255 ));
        bpmFx.setColor(5, ofColor::fromHsb(synths[synthButton[1]].colorHue, 200, 200, 255 ));
    } else {
        bpmFx.setColor(4,bpmFx.getColor(4).lerp(ofColor(0,0,0,0), 0.1));
        bpmFx.setColor(5,bpmFx.getColor(5).lerp(ofColor(0,0,0,0), 0.1));
    }
    
    linePct.x = mainInterfaceData[57].elementSize.x * synths[synthButton[2]].pulsePlane.linePct;
    bpmFx.setVertex(6,mainInterface.getVertices().at(mainInterfaceData[57].counter+1)+offset);
    bpmFx.setVertex(7,mainInterface.getVertices().at(mainInterfaceData[57].counter+1)+offset+linePct);
    
    if (*synths[synthButton[2]].stepperPos != 5){
        bpmFx.setColor(6,ofColor::fromHsb(synths[synthButton[2]].colorHue, 200, 200, 255 ));
        bpmFx.setColor(7, ofColor::fromHsb(synths[synthButton[2]].colorHue, 200, 200, 255 ));
    } else {
        bpmFx.setColor(6,bpmFx.getColor(6).lerp(ofColor(0,0,0,0), 0.1));
        bpmFx.setColor(7,bpmFx.getColor(7).lerp(ofColor(0,0,0,0), 0.1));
    }
    
    
}
//--------------------------------------------------------------
void ofApp::update(){
    //fade in Volume at start
    if (startUp && volumeRestart < volumeRestartTarget) {
        setMainVolume(volumeRestart);
        volumeRestart += .012;
       // mainVol = volumeRestart;
    } else if (startUp && volumeRestart >= volumeRestartTarget) {
        //mainVol = volumeRestartTarget;
        setMainVolume(mainVol);
        startUp = false;
    }
    
    for (int i = 0; i < synths.size(); i++) {
        synths[i].update();
        
        
        if (synths[i].synthHasChanged == true){
            setupAudio();
            synths[i].synthHasChanged = false;
        }
    }
    
    //for animation
    if (aniPct < 1.0) {
        interfaceMoving = true;
        tweenFloat = easeInOut( ofClamp(aniPct, 0.0, 1.0), 0.705);
        for (int i = 0; i < synths.size(); i++) {
            synths[i].planeMovement(tweenFloat);
        }
        if (animCam) {
            updateCamera(tweenFloat);
        }
        aniPct += ANI_SPEED;
    }
    
    
    
    if (aniPct >= 1.0) {
        for (int i = 0; i < synths.size(); i++) {
            synths[i].planeMovement(1.0);
        }
        if (animCam) {
            updateCamera(1.0);
        }
        aniPct = 1.0;
        tweenFloat = easeInOut( ofClamp(aniPct, 0.0, 1.0), 0.705);
        planeForIntersect.setPosition(synths[activeSynth].myNode.getPosition());
        planeForIntersect.setOrientation(synths[activeSynth].myNode.getOrientationQuat());
        thisIntersect.setFrom(planeForIntersect);
        interfaceMoving = false;
        updateInterfaceMesh();
        muster.update(mainInterfaceData[39].drawStringPos-(mainInterfaceData[39].elementSize/2));
    }
    
    
    if (interfaceMoving) {
        updateInterfaceMesh();
        muster.update(mainInterfaceData[39].drawStringPos-(mainInterfaceData[39].elementSize/2));
        if ( currentState == STATE_DEFAULT || currentState == STATE_HARMONY) {
            hvSlotA.update(synths[synthButton[0]].noteHistory, mainInterfaceData, mainInterface);
            hvSlotB.update(synths[synthButton[1]].noteHistory, mainInterfaceData, mainInterface);
            hvSlotC.update(synths[synthButton[2]].noteHistory, mainInterfaceData, mainInterface);
        }
        
        if (currentState == STATE_EDIT_DETAIL || currentState == STATE_EDIT || currentState == STATE_DEFAULT) {
            hvSlotD.updateStateEditDetail(synths[activeSynth].noteHistory, mainInterfaceData, mainInterface, 63);
        }
    }
    
    
    // intersectPlane(ofGetMouseX(),ofGetMouseY());
    if (currentState == STATE_BPM) {
        updateBpmMenuMesh();
    }
    
    if (currentState == STATE_SAVE){
        saveManager.update();
    }
    
    if (currentState == STATE_EDIT_DETAIL || currentState == STATE_EDIT ){
        hvSlotD.updateColor();
    }
    
    if (currentState == STATE_HARMONY){
        hvSlotA.updateColor();
        hvSlotB.updateColor();
        hvSlotC.updateColor();
    }
    
    updateSleepTimer();
    
    
    //light.setPosition(  synths[activeSynth].myNode.getPosition()+ofVec3f(0,200,150));
}

void ofApp::updateInterfaceMesh() {
    
    // for index number reference see ofApp::setupGlobalInterface()
    
    mainInterfaceData[0].updateMainMeshSlider(mainInterface, designGrid[1][0],tweenFloat);
    mainInterfaceData[51].updateMainMesh(mainInterface, designGrid[1][0],tweenFloat);
    
    
    mainInterfaceData[8].updateMainMesh(mainInterface,designGrid[0][2],tweenFloat);
    mainInterfaceData[9].updateMainMesh(mainInterface, designGrid[1][2],tweenFloat);
    mainInterfaceData[10].updateMainMesh(mainInterface, designGrid[2][2],tweenFloat);
    
    mainInterfaceData[11].updateMainMesh(mainInterface, ofVec3f(designGrid[1][0].x, designGrid[1][2].y,0),tweenFloat);
    
    
    mainInterfaceData[37].updateMainMesh(mainInterface, designGrid[0][0], tweenFloat);
    
    mainInterfaceData[38].updateMainMesh(mainInterface, designGrid[1][0], tweenFloat);
    mainInterfaceData[45].updateMainMeshSlider(mainInterface, designGrid[1][0], tweenFloat);
    
    
    mainInterfaceData[1].updateMainMeshSlider(mainInterface,designGrid[0][1], tweenFloat);
    mainInterfaceData[2].updateMainMeshSlider(mainInterface, designGrid[1][1],tweenFloat);
    mainInterfaceData[3].updateMainMeshSlider(mainInterface, designGrid[2][1],tweenFloat);
    
    
    mainInterfaceData[52].updateMainMesh(mainInterface, designGrid[0][1],tweenFloat);
    mainInterfaceData[53].updateMainMesh(mainInterface, designGrid[1][1],tweenFloat);
    mainInterfaceData[54].updateMainMesh(mainInterface, designGrid[2][1],tweenFloat);
    
    // mainInterfaceData[40].updateMainMesh(mainInterface, designGrid[0][1], tweenFloat);
    
    
    mainInterfaceData[39].updateMainMesh(mainInterface,designGrid[2][1], tweenFloat);
    mainInterfaceData[39].updateMainMeshB(mainInterface,ofVec3f(designGrid[2][0].x, designGrid[0][1].y+(abs((editDetailMoveDirection-tweenFloat))*(designGrid[0][0].y*1.5)),0)
                                          ,tweenFloat);
    
    mainInterfaceData[136].updateMainMeshSlider(mainInterface,designGrid[0][1], tweenFloat);
    mainInterfaceData[136].updateMainMeshSliderB(mainInterface,ofVec3f(designGrid[0][0].x, designGrid[0][1].y+(abs((editDetailMoveDirection-tweenFloat))*(designGrid[0][0].y*1.5)),0)
                                                 ,tweenFloat);
    
    mainInterfaceData[137].updateMainMesh(mainInterface,designGrid[0][1], tweenFloat);
    mainInterfaceData[137].updateMainMeshB(mainInterface,ofVec3f(designGrid[0][0].x, designGrid[0][1].y+(abs((editDetailMoveDirection-tweenFloat))*(designGrid[0][0].y*1.5)),0)
                                           ,tweenFloat);
    
    
    
    mainInterfaceData[12].updateMainMesh(mainInterface,designGrid[2][0], tweenFloat);
    
    
    for (int i = 0; i < 12; i++) {
        mainInterfaceData[13+i].updateMainMesh(mainInterface,designGrid[1][1],tweenFloat);
    }
    for (int i = 0; i < 12; i++) {
        mainInterfaceData[25+i].updateMainMesh(mainInterface,designGrid[1][1],tweenFloat);
    }
    
    
    
    mainInterfaceData[4].updateMainMesh(mainInterface,designGrid[0][1],tweenFloat);
    
    mainInterfaceData[6].updateMainMesh(mainInterface,designGrid[1][1],tweenFloat);
    
    mainInterfaceData[127].updateMainMesh(mainInterface,designGrid[2][1],tweenFloat);
    
    
    // mainInterfaceData[5].updateMainMesh(mainInterface, ofVec3f(0, designGrid[0][1].y,0) ,tweenFloat);
    
    mainInterfaceData[7].updateMainMesh(mainInterface,designGrid[0][1],tweenFloat);
    
    
    //move in edit to detail state
    /*
     mainInterfaceData[5].updateMainMeshB(mainInterface, ofVec3f(0,designGrid[0][1].y+(abs((editDetailMoveDirection-tweenFloat))*(designGrid[0][0].y*1.5)),0)
     ,tweenFloat);
     */
    
    mainInterfaceData[7].updateMainMeshB(mainInterface,  ofVec3f(designGrid[0][1].x,designGrid[0][1].y+(abs((editDetailMoveDirection-tweenFloat))*(designGrid[0][0].y*1.5)),0)
                                         ,tweenFloat);
    
    
    mainInterfaceData[43].updateMainMesh(mainInterface, designGrid[1][2],tweenFloat);
    mainInterfaceData[44].updateMainMesh(mainInterface, designGrid[1][0],tweenFloat);
    
    mainInterfaceData[41].updateMainMesh(mainInterface, designGrid[2][0],tweenFloat);
    
    mainInterfaceData[42].updateMainMesh(mainInterface, designGrid[1][0],tweenFloat);
    
    
    mainInterfaceData[49].updateMainMeshSlider(mainInterface, designGrid[1][0],tweenFloat);
    mainInterfaceData[5].updateMainMesh(mainInterface, designGrid[1][0],tweenFloat);
    
    
    mainInterfaceData[55].updateMainMeshSlider(mainInterface,designGrid[0][1], tweenFloat);
    mainInterfaceData[56].updateMainMeshSlider(mainInterface, designGrid[1][1],tweenFloat);
    mainInterfaceData[57].updateMainMeshSlider(mainInterface, designGrid[2][1],tweenFloat);
    
    
    mainInterfaceData[58].updateMainMeshSlider(mainInterface,designGrid[0][1], tweenFloat);
    mainInterfaceData[59].updateMainMeshSlider(mainInterface, designGrid[1][1],tweenFloat);
    mainInterfaceData[60].updateMainMeshSlider(mainInterface, designGrid[2][1],tweenFloat);
    
    mainInterfaceData[61].updateMainMeshSlider(mainInterface, designGrid[0][2],tweenFloat);
    
    mainInterfaceData[62].updateMainMeshSlider(mainInterface, designGrid[1][2],tweenFloat);
    
    for (int i = 0; i < 12; i++) {
        mainInterfaceData[63+i].updateMainMesh(mainInterface, ofVec3f(designGrid[0][1].x, designGrid[0][1].y,0) ,tweenFloat);
        mainInterfaceData[63+i].updateMainMeshB(mainInterface, ofVec3f(designGrid[0][1].x, designGrid[0][1].y+(abs((editDetailMoveDirection-tweenFloat))*(designGrid[0][0].y*1.5)),0) ,tweenFloat);
    }
    
    mainInterfaceData[135].updateMainMesh(mainInterface, ofVec3f(designGrid[0][1].x, designGrid[0][1].y,0) ,tweenFloat);
    mainInterfaceData[135].updateMainMeshB(mainInterface, ofVec3f(designGrid[0][1].x, designGrid[0][1].y+(abs((editDetailMoveDirection-tweenFloat))*(designGrid[0][0].y*1.5)),0) ,tweenFloat);
    
    
    for (int i = 0; i < 12; i++) {
        mainInterfaceData[75+i].updateMainMesh(mainInterface, designGrid[0][0] ,tweenFloat);
    }
    for (int i = 0; i < 12; i++) {
        mainInterfaceData[87+i].updateMainMesh(mainInterface,designGrid[1][0] ,tweenFloat);
    }
    for (int i = 0; i < 12; i++) {
        mainInterfaceData[99+i].updateMainMesh(mainInterface, designGrid[2][0] ,tweenFloat);
    }
    
    mainInterfaceData[111].updateMainMesh(mainInterface, designGrid[0][0],tweenFloat);
    
    for (int i = 0; i < 4; i++) {
        mainInterfaceData[112+i].updateMainMesh(mainInterface, designGrid[0][1],tweenFloat);
    }
    for (int i = 0; i < 4; i++) {
        mainInterfaceData[116+i].updateMainMesh(mainInterface, designGrid[1][1],tweenFloat);
    }
    for (int i = 0; i < 4; i++) {
        mainInterfaceData[120+i].updateMainMesh(mainInterface, designGrid[2][1],tweenFloat);
    }
    
    mainInterfaceData[46].updateMainMesh(mainInterface, designGrid[0][2],tweenFloat);
    mainInterfaceData[40].updateMainMesh(mainInterface, designGrid[1][2],tweenFloat);
    
    mainInterfaceData[47].updateMainMesh(mainInterface, designGrid[2][2],tweenFloat);
    mainInterfaceData[48].updateMainMesh(mainInterface, designGrid[0][0],tweenFloat);
    
    mainInterfaceData[50].updateMainMesh(mainInterface, designGrid[1][2],tweenFloat);
    
    mainInterfaceData[124].updateMainMesh(mainInterface, designGrid[0][2],tweenFloat);
    
    mainInterfaceData[125].updateMainMesh(mainInterface, designGrid[2][0],tweenFloat);
    
    mainInterfaceData[126].updateMainMesh(mainInterface, designGrid[2][2],tweenFloat);
    
    mainInterfaceData[128].updateMainMesh(mainInterface, designGrid[0][2],tweenFloat);
    mainInterfaceData[129].updateMainMesh(mainInterface, designGrid[1][2],tweenFloat);
    
    mainInterfaceData[130].updateMainMesh(mainInterface, designGrid[0][0],tweenFloat);
    
    
    mainInterfaceData[131].updateMainMesh(mainInterface, designGrid[1][0],tweenFloat);
    mainInterfaceData[138].updateMainMesh(mainInterface, designGrid[1][0],tweenFloat);
    
    mainInterfaceData[139].updateMainMeshSlider(mainInterface, designGrid[2][0],tweenFloat);
    mainInterfaceData[140].updateMainMesh(mainInterface, designGrid[2][0],tweenFloat);
    
    mainInterfaceData[141].updateMainMesh(mainInterface, designGrid[0][1],tweenFloat);
    mainInterfaceData[142].updateMainMesh(mainInterface, designGrid[1][1],tweenFloat);
    
    mainInterfaceData[143].updateMainMeshSlider(mainInterface, designGrid[2][1],tweenFloat);
    mainInterfaceData[144].updateMainMesh(mainInterface, designGrid[2][1],tweenFloat);
    
    mainInterfaceData[145].updateMainMesh(mainInterface, designGrid[0][1],tweenFloat);
    
    mainInterfaceData[146].updateMainMeshSlider(mainInterface, designGrid[1][1],tweenFloat);
    mainInterfaceData[147].updateMainMesh(mainInterface, designGrid[1][1],tweenFloat);
    
    mainInterfaceData[148].updateMainMeshSlider(mainInterface, designGrid[2][1],tweenFloat);
    mainInterfaceData[149].updateMainMesh(mainInterface, designGrid[2][1],tweenFloat);
    
    for (int i = 0; i < 3; i++) {
        mainInterfaceData[132+i].updateMainMesh(mainInterface, designGrid[i][0],tweenFloat);
    }
    
    saveManager.animateGrid(tweenFloat);
    
    interfaceDraw.clear();
    int indexCounter = 0;
    for (int i = 0; i < mainInterfaceData.size(); i++) {
        if ((mainInterfaceData[i].drawStringPos.x >= (-mainInterfaceData[i].elementSize.x/2) &&  mainInterfaceData[i].drawStringPos.x <= ((designGrid[0][0].x*6)+(mainInterfaceData[i].elementSize.x/2) )) &&
            mainInterfaceData[i].drawStringPos.y >= (0-mainInterfaceData[i].elementSize.y/2) &&  mainInterfaceData[i].drawStringPos.y <= ((designGrid[0][0].y*6)+(mainInterfaceData[i].elementSize.y/2) )) {
            
            for (int j = 0; j < 4; j++){
                interfaceDraw.addVertex(mainInterface.getVertex((i*4)+j));
                interfaceDraw.addColor(mainInterface.getColor((i*4)+j));
                
            }
            
            for (int j = 0; j < 6; j++) {
                interfaceDraw.addIndex(mainInterfaceData.at(i).index[j]+(4*indexCounter));
            }
            
            indexCounter++;
        }
        
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    glLineWidth(2);
    
    //glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    
    glEnable(GL_MULTISAMPLE);
    //ofEnableLighting();
    //light.enable();
    //material.begin();
    
    if (!debugCam) {
        testCam.begin();
    } else {
        cam.begin();
    }
    //  thisIntersect.draw();
    for (int i = 0; i < 3; i++) {
        synths[i].myNode.transformGL();
        
        synths[i].draw();
        
        synths[i].myNode.restoreTransformGL();
    }
    
    
    if (!debugCam) {
        testCam.end();
    } else {
        cam.end();
    }
    
    // material.end();
    //ofDisableLighting();
    
    if (drawInfo) {
        drawDebug();
    }
    
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_MULTISAMPLE);
    
    drawInterface();
    
    
    
    
}

void ofApp::drawInterface(){
    //mainInterface.draw();
    interfaceDraw.draw();
    
    if ( (currentState == STATE_HARMONY) || (interfaceMoving && currentState == STATE_DEFAULT) ){
        hvSlotA.draw();
        hvSlotB.draw();
        hvSlotC.draw();
    }
    
    if (currentState == STATE_EDIT || currentState == STATE_EDIT_DETAIL || (interfaceMoving && currentState == STATE_DEFAULT)) {
        hvSlotD.draw();
    }
    
    
    drawStringAndIcons();
    
    drawSliderPos();
    
    muster.draw();
    
    
    //bpm fx
    if (currentState == STATE_BPM) {
        glLineWidth(2);
        bpmFx.draw();
    }
    
    //save menu
    if ( (currentState == STATE_SAVE) || (interfaceMoving && currentState == STATE_DEFAULT) ){
        saveManager.draw();
    }
    
}

void ofApp::drawSliderPos(){
    ofPushStyle();
    float sliderY = designGrid[0][0].y*2*0.25;
    ofVec2f rectLine = ofVec2f(designGrid[0][0].x*0.12,sliderY*1.25);
    float lineOff = abs(rectLine.y-(designGrid[0][0].y*2*0.25))/2.0;
    
    //volume slider white  line
    if ( (currentState == STATE_VOLUME) || (interfaceMoving && currentState == STATE_DEFAULT) ){
        for (int i = 0; i < 4; i++){
            ofSetColor(ofColor(21,21,21,80));
            ofDrawRectangle( mainInterface.getVertex(mainInterfaceData.at(i).counter+3).x-(rectLine.x*2.0/2.0),mainInterface.getVertex(mainInterfaceData.at(51+i).counter+3).y,
                            rectLine.x*2.0, sliderY);
            
            ofSetColor(ofColor::fromHsb(255,0,195,255));
            ofDrawRectangle( mainInterface.getVertex(mainInterfaceData.at(i).counter+3).x-(rectLine.x/2),mainInterface.getVertex(mainInterfaceData.at(51+i).counter+3).y-lineOff,
                            rectLine.x, rectLine.y);
        }
    }
    
    //bpm slider white line
    if ( (currentState == STATE_BPM) || (interfaceMoving && currentState == STATE_DEFAULT) ){
        ofSetColor(ofColor(21,21,21,80));
        ofDrawRectangle( mainInterface.getVertex(mainInterfaceData.at(45).counter+3).x-(rectLine.x*2.0/2.0),mainInterface.getVertex(mainInterfaceData.at(38).counter+3).y-lineOff,
                        rectLine.x*2.0, rectLine.y);
        
        ofSetColor(ofColor::fromHsb(255,0,195,255));
        ofDrawRectangle( mainInterface.getVertex(mainInterfaceData.at(45).counter+3).x-(rectLine.x/2),mainInterface.getVertex(mainInterfaceData.at(38).counter+3).y-lineOff,
                        rectLine.x, rectLine.y);
    }
    
    //settings slider white line
    if ( (currentState == STATE_SETTINGS) || (interfaceMoving && currentState == STATE_DEFAULT) ){
        if(!dynamicVelo){
            ofSetColor(ofColor(21,21,21,80));
            ofDrawRectangle( mainInterface.getVertex(mainInterfaceData.at(139).counter+3).x-(rectLine.x*2.0/2.0),mainInterface.getVertex(mainInterfaceData.at(140).counter+3).y-lineOff,
                            rectLine.x*2.0, rectLine.y);
            
            ofSetColor(ofColor::fromHsb(255,0,195,255));
            ofDrawRectangle( mainInterface.getVertex(mainInterfaceData.at(139).counter+3).x-(rectLine.x/2),mainInterface.getVertex(mainInterfaceData.at(140).counter+3).y-lineOff,
                            rectLine.x, rectLine.y);
        }
        
        if (soundDelay && !autoDelay){
            ofSetColor(ofColor(21,21,21,80));
            ofDrawRectangle( mainInterface.getVertex(mainInterfaceData.at(143).counter+3).x-(rectLine.x*2.0/2.0),mainInterface.getVertex(mainInterfaceData.at(144).counter+3).y-lineOff,
                            rectLine.x*2.0, rectLine.y);
            
            ofSetColor(ofColor::fromHsb(255,0,195,255));
            ofDrawRectangle( mainInterface.getVertex(mainInterfaceData.at(143).counter+3).x-(rectLine.x/2),mainInterface.getVertex(mainInterfaceData.at(144).counter+3).y-lineOff,
                            rectLine.x, rectLine.y);
        }
        
        
        if (soundReverb){
            ofSetColor(ofColor(21,21,21,80));
            ofDrawRectangle( mainInterface.getVertex(mainInterfaceData.at(148).counter+3).x-(rectLine.x*2.0/2.0),mainInterface.getVertex(mainInterfaceData.at(149).counter+3).y-lineOff,
                            rectLine.x*2.0, rectLine.y);
            
            ofSetColor(ofColor::fromHsb(255,0,195,255));
            ofDrawRectangle( mainInterface.getVertex(mainInterfaceData.at(148).counter+3).x-(rectLine.x/2),mainInterface.getVertex(mainInterfaceData.at(149).counter+3).y-lineOff,
                            rectLine.x, rectLine.y);
        }
        
        if (soundReverb){
            ofSetColor(ofColor(21,21,21,80));
            ofDrawRectangle( mainInterface.getVertex(mainInterfaceData.at(146).counter+3).x-(rectLine.x*2.0/2.0),mainInterface.getVertex(mainInterfaceData.at(147).counter+3).y-lineOff,
                            rectLine.x*2.0, rectLine.y);
            
            ofSetColor(ofColor::fromHsb(255,0,195,255));
            ofDrawRectangle( mainInterface.getVertex(mainInterfaceData.at(146).counter+3).x-(rectLine.x/2),mainInterface.getVertex(mainInterfaceData.at(147).counter+3).y-lineOff,
                            rectLine.x, rectLine.y);
        }
    }
    
    //STATE_EDT_DETAIL keynote slider white line
    if ( (currentState == STATE_EDIT_DETAIL) || (currentState == STATE_EDIT) || (interfaceMoving && currentState == STATE_DEFAULT) ){
        ofSetColor(ofColor(21,21,21,80));
        ofDrawRectangle( mainInterface.getVertex(mainInterfaceData.at(49).counter+3).x-(rectLine.x*2.0/2),mainInterface.getVertex(mainInterfaceData.at(5).counter+3).y-lineOff,
                        rectLine.x*2.0, rectLine.y);
        
        ofSetColor(ofColor::fromHsb(255,0,195,255));
        ofDrawRectangle( mainInterface.getVertex(mainInterfaceData.at(49).counter+3).x-(rectLine.x/2),mainInterface.getVertex(mainInterfaceData.at(5).counter+3).y-lineOff,
                        rectLine.x, rectLine.y);
        
        ofSetColor(ofColor(21,21,21,80));
        ofDrawRectangle( mainInterface.getVertex(mainInterfaceData.at(136).counter+3).x-(rectLine.x*2.0/2),mainInterface.getVertex(mainInterfaceData.at(137).counter+3).y-lineOff,
                        rectLine.x*2.0, rectLine.y);
        
        ofSetColor(ofColor::fromHsb(255,0,195,255));
        ofDrawRectangle( mainInterface.getVertex(mainInterfaceData.at(136).counter+3).x-(rectLine.x/2),mainInterface.getVertex(mainInterfaceData.at(137).counter+3).y-lineOff,
                        rectLine.x, rectLine.y);
        
    }
    
    ofPopStyle();
}

void ofApp::drawStringAndIcons(){
    
    
    ofPushStyle();
    // ofSetColor(ofColor::fromHsb(255,0,195,255));
    
    //drawStrings
    for (int i = 0; i < mainInterfaceData.size();i ++){
        /*
         if(i >= 13 && i <= 36) {
         ofSetColor(255, 255, 255,190);
         } else {
         ofSetColor(255, 255, 255,255);
         }
         */
        
        if ( mainInterfaceData[i].showString &&
            
            ((mainInterfaceData[i].drawStringPos.x >= (0-200) &&  mainInterfaceData[i].drawStringPos.x <= ((designGrid[0][0].x*6)+200 )) &&
             mainInterfaceData[i].drawStringPos.y >= (0-200) &&  mainInterfaceData[i].drawStringPos.y <= ((designGrid[0][0].y*6)+200 ))
            ) {
            
            if(i < 25 || i > 36) {
                mainInterfaceData[i].drawFontString();
            } else {
                mainInterfaceData[i].drawFontString(0,-(fontSizeSmall/2)+(designGrid[0][0].y/4)  );
                
            }
            
        }
        
        
    }
    
    float tempTrans = 0.5*scaleFac;
    
    //draw icons
    
    
    
    
    
    
    //button left-right
    ofSetColor(mainInterfaceData[58].displayColor);
    
    ofVec3f tri = mainInterface.getVertices().at(mainInterfaceData.at(58).counter+1);
    
    ofDrawTriangle(mainInterface.getVertices().at(mainInterfaceData.at(58).counter+1),
                   mainInterface.getVertices().at(mainInterfaceData.at(58).counter+1)+ ofVec3f(0,-mainInterfaceData.at(58).elementSize.y,0),
                   mainInterface.getVertices().at(mainInterfaceData.at(58).counter+1)+ ofVec3f(-12,-mainInterfaceData.at(58).elementSize.y/2,0));
    
    ofDrawTriangle(mainInterface.getVertices().at(mainInterfaceData.at(58).counter+2),
                   mainInterface.getVertices().at(mainInterfaceData.at(58).counter+2)+ ofVec3f(0,-mainInterfaceData.at(58).elementSize.y,0),
                   mainInterface.getVertices().at(mainInterfaceData.at(58).counter+2)+ ofVec3f(+12,-mainInterfaceData.at(58).elementSize.y/2,0));
    
    ofSetColor(mainInterfaceData[59].displayColor);
    ofDrawTriangle(mainInterface.getVertices().at(mainInterfaceData.at(59).counter+1),
                   mainInterface.getVertices().at(mainInterfaceData.at(59).counter+1)+ ofVec3f(0,-mainInterfaceData.at(59).elementSize.y,0),
                   mainInterface.getVertices().at(mainInterfaceData.at(59).counter+1)+ ofVec3f(-12,-mainInterfaceData.at(59).elementSize.y/2,0));
    
    ofDrawTriangle(mainInterface.getVertices().at(mainInterfaceData.at(59).counter+2),
                   mainInterface.getVertices().at(mainInterfaceData.at(59).counter+2)+ ofVec3f(0,-mainInterfaceData.at(59).elementSize.y,0),
                   mainInterface.getVertices().at(mainInterfaceData.at(59).counter+2)+ ofVec3f(+12,-mainInterfaceData.at(59).elementSize.y/2,0));
    
    
    ofSetColor(mainInterfaceData[60].displayColor);
    ofDrawTriangle(mainInterface.getVertices().at(mainInterfaceData.at(60).counter+1),
                   mainInterface.getVertices().at(mainInterfaceData.at(60).counter+1)+ ofVec3f(0,-mainInterfaceData.at(60).elementSize.y,0),
                   mainInterface.getVertices().at(mainInterfaceData.at(60).counter+1)+ ofVec3f(-12,-mainInterfaceData.at(60).elementSize.y/2,0));
    
    ofDrawTriangle(mainInterface.getVertices().at(mainInterfaceData.at(60).counter+2),
                   mainInterface.getVertices().at(mainInterfaceData.at(60).counter+2)+ ofVec3f(0,-mainInterfaceData.at(60).elementSize.y,0),
                   mainInterface.getVertices().at(mainInterfaceData.at(60).counter+2)+ ofVec3f(+12,-mainInterfaceData.at(60).elementSize.y/2,0));
    
    
    
    ofSetColor(mainInterfaceData[61].displayColor);
    ofDrawTriangle(mainInterface.getVertices().at(mainInterfaceData.at(61).counter+1),
                   mainInterface.getVertices().at(mainInterfaceData.at(61).counter+1)+ ofVec3f(0,-mainInterfaceData.at(61).elementSize.y,0),
                   mainInterface.getVertices().at(mainInterfaceData.at(61).counter+1)+ ofVec3f(-12,-mainInterfaceData.at(61).elementSize.y/2,0));
    
    ofDrawTriangle(mainInterface.getVertices().at(mainInterfaceData.at(61).counter+2),
                   mainInterface.getVertices().at(mainInterfaceData.at(61).counter+2)+ ofVec3f(0,-mainInterfaceData.at(61).elementSize.y,0),
                   mainInterface.getVertices().at(mainInterfaceData.at(61).counter+2)+ ofVec3f(+12,-mainInterfaceData.at(61).elementSize.y/2,0));
    
    ofSetColor(mainInterfaceData[62].displayColor);
    ofDrawTriangle(mainInterface.getVertices().at(mainInterfaceData.at(62).counter+1),
                   mainInterface.getVertices().at(mainInterfaceData.at(62).counter+1)+ ofVec3f(0,-mainInterfaceData.at(58).elementSize.y,0),
                   mainInterface.getVertices().at(mainInterfaceData.at(62).counter+1)+ ofVec3f(-12,-mainInterfaceData.at(58).elementSize.y/2,0));
    
    ofDrawTriangle(mainInterface.getVertices().at(mainInterfaceData.at(62).counter+2),
                   mainInterface.getVertices().at(mainInterfaceData.at(62).counter+2)+ ofVec3f(0,-mainInterfaceData.at(62).elementSize.y,0),
                   mainInterface.getVertices().at(mainInterfaceData.at(62).counter+2)+ ofVec3f(+12,-mainInterfaceData.at(62).elementSize.y/2,0));
    
    
    ofSetColor(mainInterfaceData[7].displayColor);
    ofDrawTriangle(mainInterface.getVertices().at(mainInterfaceData.at(7).counter+1),
                   mainInterface.getVertices().at(mainInterfaceData.at(7).counter+1)+ ofVec3f(0,-mainInterfaceData.at(7).elementSize.y,0),
                   mainInterface.getVertices().at(mainInterfaceData.at(7).counter+1)+ ofVec3f(-12,-mainInterfaceData.at(7).elementSize.y/2,0));
    
    ofDrawTriangle(mainInterface.getVertices().at(mainInterfaceData.at(7).counter+2),
                   mainInterface.getVertices().at(mainInterfaceData.at(7).counter+2)+ ofVec3f(0,-mainInterfaceData.at(7).elementSize.y,0),
                   mainInterface.getVertices().at(mainInterfaceData.at(7).counter+2)+ ofVec3f(+12,-mainInterfaceData.at(7).elementSize.y/2,0));
    
    
    
    
    
    ofPopStyle();
    
}

#if !TARGET_OS_IPHONE

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    if (key == 's') {
        getFboScreen();
    }
    
    
    if (key == 'd') {
        drawInfo = !drawInfo;
    }
    
    if (key == 'r') {
        
        for (int i = 0; i < TILES; i++) {
            for (int j = 0; j < TILES; j++) {
                if (ofRandom(100)>70) {
                    synths[activeSynth].tapEvent(i, j);
                }
            }
        }
    }
    
    if (key == 'F') {
        ofToggleFullscreen();
    }
    
    
    
    if (key == 'c') {
        debugCam = !debugCam;
        
    }
    
    if(key == 'p') {
        savePreset();
    }
    
    if(key == '1'){
        buttonOnePress();
    }
    
    if(key == '2'){
        buttonTwoPress();
    }
    
    if(key == '3'){
        buttonThreePress();
    }
    
    if(key == '4'){
        buttonFourPress();
        mainInterfaceData[131].elementName = "VOLUME";
        mainInterfaceData[131].setStringWidth();
        mainInterfaceData[131].activateDarkerColor();
        mainInterfaceData[37].blinkOn();
    }
    
    if(key == '5'){
        harmonyButtonPress();
        mainInterfaceData[42].blinkOn();
        mainInterfaceData[131].elementName = "HARMONY";
        mainInterfaceData[131].setStringWidth();
        mainInterfaceData[131].activateDarkerColor();
    }
    
    if(key == '6'){
        mainInterfaceData[41].blinkOn();
        mainInterfaceData[131].elementName = "TEMPO";
        mainInterfaceData[131].setStringWidth();
        mainInterfaceData[131].activateDarkerColor();
    }
    
    if(key == '7'){
        loadSaveButtonPress();
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}
#endif

//--------------------------------------------------------------

#if TARGET_OS_IPHONE
void ofApp::touchMoved(ofTouchEventArgs & touch){
    
    int x = touch.x;
    int y = touch.y;
    
    replaceMouseDragged(x, y);
}
#else
void ofApp::mouseDragged(int x, int y, int button){
    replaceMouseDragged(x, y);
}
#endif
//--------------------------------------------------------------

void ofApp::replaceMouseDragged(int x, int y){
    
    
    //-------------------------
    
    if (!interfaceMoving && !sleepMode) {
        
        if (currentState == STATE_BPM) {
            
            if(mainInterfaceData[45].touchDown) {
                float value = ofClamp(ofMap(x, mainInterfaceData[45].minX, mainInterfaceData[45].maxX, 0.0, 1.0), 0.0, 1.0);
                mainInterfaceData[45].setSlider(mainInterface,value);
                bpm=ceil(value*BPM_MAX)+20;
                dynamicDelayValue = value;
                tonicSynth.setParameter("BPM",bpm*BPM_DIVISION_MAX);
                if (autoDelay){
                    tonicSynth.setParameter("delay", getBpmValue(dynamicDelayValue));
                }
                //mainInterfaceData[38].elementName = ofToString(ceil(value*BPM_MAX));
                //mainInterfaceData[38].setStringWidth(mainInterfaceData[38].fsPtr->getBBox(mainInterfaceData[38].elementName, mainInterfaceData[38].fontSize, 0, 0).getWidth());
                for (int i = 0; i < 3; i++){
                    synths[i].setAllADSR(synths[i].preset);
                }
            }
            
        } else if(currentState == STATE_EDIT) {
            
            if (mainInterfaceData[136].touchDown) {
                float value = ofClamp(ofMap(x, mainInterfaceData[136].minX, mainInterfaceData[136].maxX, 0.0, 1.0), 0.0, 1.0);
                mainInterfaceData[136].setSlider(mainInterface, value);
                synths[activeSynth].attackSlider = ofMap(value, 0.0, 1.0, attSldMax, attSldMin);
                synths[activeSynth].setAllADSR(synths[activeSynth].preset);
                
            }
        }
        
        else if (currentState == STATE_VOLUME) {
            
            if (mainInterfaceData[1].touchDown) {
                float value = ofClamp(ofMap(x, mainInterfaceData[1].minX, mainInterfaceData[1].maxX, 0.0, 1.0), 0.0, 1.0);
                mainInterfaceData[1].setSlider(mainInterface, value);
                synths[synthButton[0]].changeSynthVolume(value);
                //mainInterfaceData[52].elementName = ofToString(value,2);
                //mainInterfaceData[52].setStringWidth(mainInterfaceData[52].fsPtr->getBBox(mainInterfaceData[52].elementName, mainInterfaceData[52].fontSize, 0, 0).getWidth());
            }
            
            else if (mainInterfaceData[2].touchDown) {
                float value = ofClamp(ofMap(x, mainInterfaceData[2].minX, mainInterfaceData[2].maxX, 0.0, 1.0), 0.0, 1.0);
                mainInterfaceData[2].setSlider(mainInterface, value);
                synths[synthButton[1]].changeSynthVolume(value);
                // mainInterfaceData[53].elementName = ofToString(value,2);
                // mainInterfaceData[53].setStringWidth(mainInterfaceData[53].fsPtr->getBBox(mainInterfaceData[53].elementName, mainInterfaceData[53].fontSize, 0, 0).getWidth());
            }
            
            else if (mainInterfaceData[3].touchDown) {
                float value = ofClamp(ofMap(x, mainInterfaceData[3].minX, mainInterfaceData[3].maxX, 0.0, 1.0), 0.0, 1.0);
                mainInterfaceData[3].setSlider(mainInterface, value);
                synths[synthButton[2]].changeSynthVolume(value);
                //  mainInterfaceData[54].elementName = ofToString(value,2);
                //  mainInterfaceData[54].setStringWidth(mainInterfaceData[54].fsPtr->getBBox(mainInterfaceData[54].elementName, mainInterfaceData[54].fontSize, 0, 0).getWidth());
            }
            
            else if (mainInterfaceData[0].touchDown) {
                float value = ofClamp(ofMap(x, mainInterfaceData[0].minX, mainInterfaceData[0].maxX, 0.0, 1.0), 0.0, 1.0);
                mainInterfaceData[0].setSlider(mainInterface, value);
                setMainVolume(value);
                mainVol = value;
                //mainInterfaceData[51].elementName = ofToString(value,2);
                //mainInterfaceData[51].setStringWidth(mainInterfaceData[51].fsPtr->getBBox(mainInterfaceData[51].elementName, mainInterfaceData[51].fontSize, 0, 0).getWidth());
            }
            
        }
        
        else if (currentState == STATE_SAVE) {
            if(!saveManager.animate && !saveManager.slotDetail){
                
                if (!saveManager.touchDown){
                    saveManager.touchDown = true;
                    saveManager.touchPos = y;
                    saveManager.oldTouchPos = y;
                }
                
                else if(saveManager.touchDown){
                    //scrollOffset.x == temp for y
                    //saveManager.scrollOffset.y = ofClamp(y-saveManager.touchStart.y+saveManager.scrollOffset.x, -saveManager.offsetDown.y+designGrid[0][0].y*5,0);
                    saveManager.oldTouchPos = saveManager.touchPos;
                    saveManager.touchPos = y;
                    saveManager.acc = saveManager.touchPos-saveManager.oldTouchPos;
                }
            }
        }
        
        else if (currentState == STATE_EDIT_DETAIL) {
            
            if(mainInterfaceData[136].touchDown) {
                float value = ofClamp(ofMap(x, mainInterfaceData[136].minX, mainInterfaceData[136].maxX, 0.0, 1.0), 0.0, 1.0);
                mainInterfaceData[136].setSlider(mainInterface, value);
                synths[activeSynth].attackSlider = ofMap(value, 0.0, 1.0, attSldMax, attSldMin);
                synths[activeSynth].setAllADSR(synths[activeSynth].preset);
            }
            
            if(mainInterfaceData[49].touchDown) {
                float value = ofClamp(ofMap(x, mainInterfaceData[49].minX, mainInterfaceData[49].maxX, 0.0, 1.0), 0.0, 1.0);
                mainInterfaceData[49].setSlider(mainInterface, value);
                int keyMod = ofMap(value, 0.0, 1.0, 12, 96);
                synths[activeSynth].setKeyNote(keyMod-synths[activeSynth].keyNote);
                setNewGUI();
            }
            
        }
        
        else if (currentState == STATE_SETTINGS){
            
            if(mainInterfaceData[139].touchDown) {
                if (!dynamicVelo){
                    float value = ofClamp(ofMap(x, mainInterfaceData[139].minX, mainInterfaceData[139].maxX, 0.0, 1.0), 0.0, 1.0);
                    mainInterfaceData[139].setSlider(mainInterface, value);
                    globalVelo = value;
                    for (int i = 0; i < 3; i++) {
                        synths[i].velocity = value;
                    }
                }
            }
            
            else  if(mainInterfaceData[143].touchDown) {
                if(soundDelay && !autoDelay){
                    float value = ofClamp(ofMap(x, mainInterfaceData[143].minX, mainInterfaceData[143].maxX, 0.0, 1.0), 0.0, 1.0);
                    staticDelayValue = 1.0-value;
                    mainInterfaceData[143].setSlider(mainInterface, value);
                    if(!autoDelay){
                        tonicSynth.setParameter("delay",getBpmValue(staticDelayValue));
                    }
                }
            }
            
            else if (mainInterfaceData[146].touchDown) {
                if(soundReverb){
                    float value = ofClamp(ofMap(x, mainInterfaceData[146].minX, mainInterfaceData[146].maxX, 0.0, 1.0), 0.0, 1.0);
                    revTime = value;
                    mainInterfaceData[146].setSlider(mainInterface, value);
                    tonicSynth.setParameter("revTime",getRevTime(value));
                }
            }
            
            else if (mainInterfaceData[148].touchDown) {
                if(soundReverb){
                    float value = ofClamp(ofMap(x, mainInterfaceData[148].minX, mainInterfaceData[148].maxX, 0.0, 1.0), 0.0, 1.0);
                    revSize = value;
                    mainInterfaceData[148].setSlider(mainInterface, value);
                    tonicSynth.setParameter("revSize",getRevSize(value));
                }
            }
            
        }
        
    }
    //-----------------------------
    //for the sleeptimer
    if (!sleepMode){
        lastTouch = ofGetElapsedTimef();
    } else {
        sleepMode = false;
        lastTouch = ofGetElapsedTimef();
        for  (int i = 0; i < 8; i++){
            mainInterfaceData.at(defaultStateIndex[i]).switchColor();
        }
    }
    
    
}



#if TARGET_OS_IPHONE
//--------------------------------------------------------------
void ofApp::touchDown(ofTouchEventArgs & touch){
    
    int x = touch.x;
    int y = touch.y;
    
    replaceMousePressed(x, y);
}
#else
//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
    replaceMousePressed(x, y);
}
#endif
//--------------------------------------------------------------

void ofApp::replaceMousePressed(int x, int y) {
    
    
    //-------------------------
    
    if (!interfaceMoving && !sleepMode) {
        
        if (currentState == STATE_BPM) {
            
            if(mainInterfaceData[45].isInside(ofVec2f(x,y))) {
                if(!mainInterfaceData[45].touchDown){
                    mainInterfaceData[45].touchDown = true;
                }
                float value = ofClamp(ofMap(x, mainInterfaceData[45].minX, mainInterfaceData[45].maxX, 0.0, 1.0), 0.0, 1.0);
                mainInterfaceData[45].setSlider(mainInterface,value);
                bpm=ceil(value*BPM_MAX)+20;
                dynamicDelayValue = value;
                tonicSynth.setParameter("BPM",bpm*BPM_DIVISION_MAX);
                if (autoDelay){
                    tonicSynth.setParameter("delay", getBpmValue(dynamicDelayValue));
                }
                //mainInterfaceData[38].elementName = ofToString(ceil(value*BPM_MAX));
                //mainInterfaceData[38].setStringWidth(mainInterfaceData[38].fsPtr->getBBox(mainInterfaceData[38].elementName, mainInterfaceData[38].fontSize, 0, 0).getWidth());
                for (int i = 0; i < 3; i++){
                    synths[i].setAllADSR(synths[i].preset);
                }
            }
            
        } else if(currentState == STATE_EDIT) {
            
            if (mainInterfaceData[136].isInside(ofVec2f(x,y))) {
                if(!mainInterfaceData[136].touchDown){
                    mainInterfaceData[136].touchDown = true;
                }
                float value = ofClamp(ofMap(x, mainInterfaceData[136].minX, mainInterfaceData[136].maxX, 0.0, 1.0), 0.0, 1.0);
                mainInterfaceData[136].setSlider(mainInterface, value);
                synths[activeSynth].attackSlider = ofMap(value, 0.0, 1.0, attSldMax, attSldMin);
                synths[activeSynth].setAllADSR(synths[activeSynth].preset);
                
            }
        }
        
        else if (currentState == STATE_VOLUME) {
            
            if (mainInterfaceData[1].isInside(ofVec2f(x,y))) {
                if(!mainInterfaceData[1].touchDown){
                    mainInterfaceData[1].touchDown = true;
                }
                float value = ofClamp(ofMap(x, mainInterfaceData[1].minX, mainInterfaceData[1].maxX, 0.0, 1.0), 0.0, 1.0);
                mainInterfaceData[1].setSlider(mainInterface, value);
                synths[synthButton[0]].changeSynthVolume(value);
                //mainInterfaceData[52].elementName = ofToString(value,2);
                //mainInterfaceData[52].setStringWidth(mainInterfaceData[52].fsPtr->getBBox(mainInterfaceData[52].elementName, mainInterfaceData[52].fontSize, 0, 0).getWidth());
            }
            
            else if (mainInterfaceData[2].isInside(ofVec2f(x,y))) {
                if(!mainInterfaceData[2].touchDown){
                    mainInterfaceData[2].touchDown = true;
                }
                float value = ofClamp(ofMap(x, mainInterfaceData[2].minX, mainInterfaceData[2].maxX, 0.0, 1.0), 0.0, 1.0);
                mainInterfaceData[2].setSlider(mainInterface, value);
                synths[synthButton[1]].changeSynthVolume(value);
                // mainInterfaceData[53].elementName = ofToString(value,2);
                // mainInterfaceData[53].setStringWidth(mainInterfaceData[53].fsPtr->getBBox(mainInterfaceData[53].elementName, mainInterfaceData[53].fontSize, 0, 0).getWidth());
            }
            
            else if (mainInterfaceData[3].isInside(ofVec2f(x,y))) {
                if(!mainInterfaceData[3].touchDown){
                    mainInterfaceData[3].touchDown = true;
                }
                float value = ofClamp(ofMap(x, mainInterfaceData[3].minX, mainInterfaceData[3].maxX, 0.0, 1.0), 0.0, 1.0);
                mainInterfaceData[3].setSlider(mainInterface, value);
                synths[synthButton[2]].changeSynthVolume(value);
                //  mainInterfaceData[54].elementName = ofToString(value,2);
                //  mainInterfaceData[54].setStringWidth(mainInterfaceData[54].fsPtr->getBBox(mainInterfaceData[54].elementName, mainInterfaceData[54].fontSize, 0, 0).getWidth());
            }
            
            else if (mainInterfaceData[0].isInside(ofVec2f(x,y))) {
                if(!mainInterfaceData[0].touchDown){
                    mainInterfaceData[0].touchDown = true;
                }
                float value = ofClamp(ofMap(x, mainInterfaceData[0].minX, mainInterfaceData[0].maxX, 0.0, 1.0), 0.0, 1.0);
                mainInterfaceData[0].setSlider(mainInterface, value);
                setMainVolume(value);
                mainVol = value;
                //mainInterfaceData[51].elementName = ofToString(value,2);
                //mainInterfaceData[51].setStringWidth(mainInterfaceData[51].fsPtr->getBBox(mainInterfaceData[51].elementName, mainInterfaceData[51].fontSize, 0, 0).getWidth());
            }
            
        }
        
        
        else if (currentState == STATE_SAVE) {
            lastClick.x = x;
            lastClick.y = y;
        }
        
        
        else if (currentState == STATE_EDIT_DETAIL) {
            
            if(mainInterfaceData[136].isInside(ofVec2f(x,y))) {
                if(!mainInterfaceData[136].touchDown){
                    mainInterfaceData[136].touchDown = true;
                }
                float value = ofClamp(ofMap(x, mainInterfaceData[136].minX, mainInterfaceData[136].maxX, 0.0, 1.0), 0.0, 1.0);
                mainInterfaceData[136].setSlider(mainInterface, value);
                synths[activeSynth].attackSlider = ofMap(value, 0.0, 1.0, attSldMax, attSldMin);
                synths[activeSynth].setAllADSR(synths[activeSynth].preset);
            }
            
            if(mainInterfaceData[49].isInside(ofVec2f(x,y))) {
                if(!mainInterfaceData[49].touchDown){
                    mainInterfaceData[49].touchDown = true;
                }
                float value = ofClamp(ofMap(x, mainInterfaceData[49].minX, mainInterfaceData[49].maxX, 0.0, 1.0), 0.0, 1.0);
                mainInterfaceData[49].setSlider(mainInterface, value);
                int keyMod = ofMap(value, 0.0, 1.0, 12, 96);
                synths[activeSynth].setKeyNote(keyMod-synths[activeSynth].keyNote);
                setNewGUI();
            }
            
        }
        
        else if (currentState == STATE_SETTINGS){
            
            if(mainInterfaceData[139].isInside(ofVec2f(x,y))) {
                if (!dynamicVelo){
                    if(!mainInterfaceData[139].touchDown){
                        mainInterfaceData[139].touchDown = true;
                    }
                    float value = ofClamp(ofMap(x, mainInterfaceData[139].minX, mainInterfaceData[139].maxX, 0.0, 1.0), 0.0, 1.0);
                    mainInterfaceData[139].setSlider(mainInterface, value);
                    globalVelo = value;
                    for (int i = 0; i < 3; i++) {
                        synths[i].velocity = value;
                    }
                }
            }
            
            else  if(mainInterfaceData[143].isInside(ofVec2f(x,y))) {
                if(soundDelay && !autoDelay){
                    if(!mainInterfaceData[143].touchDown){
                        mainInterfaceData[143].touchDown = true;
                    }
                    float value = ofClamp(ofMap(x, mainInterfaceData[143].minX, mainInterfaceData[143].maxX, 0.0, 1.0), 0.0, 1.0);
                    staticDelayValue = 1.0-value;
                    mainInterfaceData[143].setSlider(mainInterface, value);
                    if(!autoDelay){
                        tonicSynth.setParameter("delay",getBpmValue(staticDelayValue));
                    }
                    cout << getBpmValue(staticDelayValue) << endl;
                }
            }
            
            else if (mainInterfaceData[146].isInside(ofVec2f(x,y))) {
                if(soundReverb){
                    if(!mainInterfaceData[146].touchDown){
                        mainInterfaceData[146].touchDown = true;
                    }
                    float value = ofClamp(ofMap(x, mainInterfaceData[146].minX, mainInterfaceData[146].maxX, 0.0, 1.0), 0.0, 1.0);
                    revTime = value;
                    mainInterfaceData[146].setSlider(mainInterface, value);
                    tonicSynth.setParameter("revTime",getRevTime(value));
                }
            }
            
            else if (mainInterfaceData[148].isInside(ofVec2f(x,y))) {
                if(soundReverb){
                    if(!mainInterfaceData[148].touchDown){
                        mainInterfaceData[148].touchDown = true;
                    }
                    float value = ofClamp(ofMap(x, mainInterfaceData[148].minX, mainInterfaceData[148].maxX, 0.0, 1.0), 0.0, 1.0);
                    revSize = value;
                    mainInterfaceData[148].setSlider(mainInterface, value);
                    tonicSynth.setParameter("revSize",getRevSize(value));
                }
            }
            
        }
        
    }
    
    
    //for the sleeptimer
    if (!sleepMode){
        lastTouch = ofGetElapsedTimef();
    }
    
}

#if TARGET_OS_IPHONE
//--------------------------------------------------------------
void ofApp::touchUp(ofTouchEventArgs & touch){
    int x = touch.x;
    int y = touch.y;
    
    replaceMouseReleased(x, y);
}
#else
//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    replaceMouseReleased(x, y);
}
#endif
//--------------------------------------------------------------


void ofApp::replaceMouseReleased(int x,int y) {
    
    if(!interfaceMoving){
        
        
       
        
      
        
        
        
    }
    //-------------------------
    if (!interfaceMoving && !sleepMode) {
        if (currentState == STATE_EDIT) {
            intersectPlane(x, y);
            if ( (intersectPos.x < 100 && intersectPos.x > 0) && (intersectPos.y < 100 && intersectPos.y > 0) ) {
                synths[activeSynth].tapEvent(vectorPosX,vectorPosY);
                insideSynth = true;
                markSynthNotes(63);
            } else {
                insideSynth = false;
            }
        }
    }
    
    if (!interfaceMoving && !sleepMode) {
        
        
        if (currentState == STATE_DEFAULT) {
            
            if( (x > designGrid[0][1].x-designGrid[0][0].x && x < designGrid[0][1].x+designGrid[0][0].x) &&
               (y > designGrid[0][1].y-designGrid[0][0].y && y < designGrid[0][1].y+designGrid[0][0].y) ){
                if(!synths[synthButton[0]].trackSwitchOn) {
                    synths[synthButton[0]].trackSwitchOn = true;
                } else {
                    buttonOnePress();
                }
            }
            
            else if( (x > designGrid[1][1].x-designGrid[0][0].x && x < designGrid[1][1].x+designGrid[0][0].x) &&
                    (y > designGrid[1][1].y-designGrid[0][0].y && y < designGrid[1][1].y+designGrid[0][0].y) ){
                buttonTwoPress();
            }
            
            else if( (x > designGrid[2][1].x-designGrid[0][0].x && x < designGrid[2][1].x+designGrid[0][0].x) &&
                    (y > designGrid[2][1].y-designGrid[0][0].y && y < designGrid[2][1].y+designGrid[0][0].y) ){
                if(!synths[synthButton[2]].trackSwitchOn) {
                    synths[synthButton[2]].trackSwitchOn = true;
                } else {
                    buttonThreePress();
                }
            }
        }
        
        if (currentState == STATE_EDIT) {
            
            if( (x > designGrid[0][2].x-designGrid[0][0].x && x < designGrid[0][2].x+designGrid[0][0].x) &&
               (y > designGrid[0][2].y-designGrid[0][0].y+100 && y < designGrid[0][2].y+designGrid[0][0].y) ){
                if(synths[synthButton[0]].trackSwitchOn) {
                    buttonOnePress();
                } else {
                    synths[synthButton[0]].trackSwitchOn = true;
                }
            }
            
            else if( (x > designGrid[2][2].x-designGrid[0][0].x && x < designGrid[2][2].x+designGrid[0][0].x) &&
                    (y > designGrid[2][2].y-designGrid[0][0].y +100 && y < designGrid[2][2].y+designGrid[0][0].y) ){
                if(synths[synthButton[2]].trackSwitchOn) {
                    buttonThreePress();
                } else {
                    synths[synthButton[2]].trackSwitchOn = true;
                }
            }
        }
    }
    
    if ( !interfaceMoving && !insideSynth && !sleepMode ){
        
        bool touchUp = false;
        if(mainInterfaceData[136].touchDown){
            mainInterfaceData[136].touchDown = false;
            touchUp = true;
        }
        if(mainInterfaceData[45].touchDown){
            mainInterfaceData[45].touchDown = false;
            touchUp = true;
        }
        if(mainInterfaceData[0].touchDown){
            mainInterfaceData[0].touchDown = false;
            touchUp = true;
        }
        if(mainInterfaceData[1].touchDown){
            mainInterfaceData[1].touchDown = false;
            touchUp = true;
        }
        if(mainInterfaceData[2].touchDown){
            mainInterfaceData[2].touchDown = false;
            touchUp = true;
        }
        if(mainInterfaceData[3].touchDown){
            mainInterfaceData[3].touchDown = false;
            touchUp = true;
        }
        if(mainInterfaceData[49].touchDown){
            mainInterfaceData[49].touchDown = false;
            touchUp = true;
        }
        if(mainInterfaceData[136].touchDown){
            mainInterfaceData[136].touchDown = false;
            touchUp = true;
        }
        if(mainInterfaceData[139].touchDown){
            mainInterfaceData[139].touchDown = false;
            touchUp = true;
        }
        if(mainInterfaceData[143].touchDown){
            mainInterfaceData[143].touchDown = false;
            touchUp = true;
        }
        if(mainInterfaceData[146].touchDown){
            mainInterfaceData[146].touchDown = false;
            touchUp = true;
        }
        if(mainInterfaceData[148].touchDown){
            mainInterfaceData[148].touchDown = false;
            touchUp = true;
        }
        
        if (!touchUp) {
        if (currentState == STATE_DEFAULT) {
            
            if (mainInterfaceData[8].isInside(ofVec2f(x,y))) {
                mainInterfaceData[8].blinkOn();
                
                if (!synths[synthButton[0]].pause) {
                    synths[synthButton[0]].setSaturationOff();
                    mainInterfaceData[8].elementName = "\uE039";
                } else {
                    synths[synthButton[0]].setSaturationOn();
                    mainInterfaceData[8].elementName = "\uE036";
                }
                
                mainInterfaceData[8].setStringWidth();
                synths[synthButton[0]].pause = !synths[synthButton[0]].pause;
            }
            
            else if (mainInterfaceData[9].isInside(ofVec2f(x,y))) {
                mainInterfaceData[9].blinkOn();
                
                if (!synths[synthButton[1]].pause) {
                    synths[synthButton[1]].setSaturationOff();
                    mainInterfaceData[9].elementName = "\uE039";
                } else {
                    synths[synthButton[1]].setSaturationOn();
                    mainInterfaceData[9].elementName = "\uE036";
                }
                
                mainInterfaceData[9].setStringWidth();
                synths[synthButton[1]].pause = !synths[synthButton[1]].pause;
                
            }
            
            else if (mainInterfaceData[10].isInside(ofVec2f(x,y))) {
                mainInterfaceData[10].blinkOn();
                
                if (!synths[synthButton[2]].pause) {
                    synths[synthButton[2]].setSaturationOff();
                    mainInterfaceData[10].elementName = "\uE039";
                } else {
                    synths[synthButton[2]].setSaturationOn();
                    mainInterfaceData[10].elementName = "\uE036";
                }
                
                mainInterfaceData[10].setStringWidth();
                synths[synthButton[2]].pause = !synths[synthButton[2]].pause;
            }
            
            else if (mainInterfaceData[37].isInside(ofVec2f(x,y))) {
                buttonFourPress();
                mainInterfaceData[131].elementName = "VOLUME";
                mainInterfaceData[131].setStringWidth();
                mainInterfaceData[131].activateDarkerColor();
                mainInterfaceData[37].blinkOn();
            }
            
            else if (mainInterfaceData[41].isInside(ofVec2f(x,y))) {
                mainInterfaceData[41].blinkOn();
                mainInterfaceData[131].elementName = "TEMPO";
                mainInterfaceData[131].setStringWidth();
                mainInterfaceData[131].activateDarkerColor();
                bpmButtonPress();
            }
            
            else if (mainInterfaceData[42].isInside(ofVec2f(x,y))) {
                mainInterfaceData[42].blinkOn();
                mainInterfaceData[131].elementName = "HARMONY";
                mainInterfaceData[131].setStringWidth();
                mainInterfaceData[131].activateDarkerColor();
                harmonyButtonPress();
            }
            
            else if (mainInterfaceData[46].isInside(ofVec2f(x,y))) {
                mainInterfaceData[46].blinkOn();
                loadSaveButtonPress();
            }
            else if (mainInterfaceData[40].isInside(ofVec2f(x,y))) {
                mainInterfaceData[40].blinkOn();
                settingsButtonPress();
            }
            
        }
        
        else  if (currentState == STATE_VOLUME) {
            
            
            if (mainInterfaceData[43].isInside(ofVec2f(x,y))) {
                buttonFourPress();
                mainInterfaceData[43].blinkOn();
            }
        }
        
        else  if (currentState == STATE_EDIT_DETAIL) {
            
            //muster container
            if (mainInterfaceData[39].isInside(ofVec2f(x,y))) {
                int musterIndex =  muster.isInside(ofVec2f(x,y));
                
                if (!muster.saveReady) {
                    if ( musterIndex >= 0) {
                        synths[activeSynth].loadMuster(muster.flips.at(musterIndex).layerInfo);
                        markSynthNotes(63);
                    }
                } else if (musterIndex >= 0){
                    synths[activeSynth].getLayerInfo(muster.flips.at(musterIndex).layerInfo);
                    muster.flips.at(musterIndex).makeTex();
                    muster.saveReady = false;
                }
            }
            
            //synth
            else if(  mainInterfaceData[7].isInside(ofVec2f(x,y))) {
                if (  (x-mainInterfaceData[7].minX) > ((mainInterfaceData[7].maxX-mainInterfaceData[7].minX)/2)) {
                    synths[activeSynth].changePreset(true);
                } else {
                    synths[activeSynth].changePreset(false);
                }
                setNewGUI();
                mainInterfaceData[7].blinkOn();
            }
            
            else if(  mainInterfaceData[11].isInside(ofVec2f(x,y))) {
                buttonEditDetail();
                mainInterfaceData[11].blinkOn();
            }
            
            else if(mainInterfaceData[135].isInside(ofVec2f(x,y))){
                if (  (x-mainInterfaceData[135].minX) > ((mainInterfaceData[135].maxX-mainInterfaceData[135].minX)/2)) {
                    synths[activeSynth].makeScaleStep(1);
                } else {
                    synths[activeSynth].makeScaleStep(-1);
                }
                hvSlotD.blink();
                
                markScaleSteps(63);
            }
            
            
            
            for (int i = 1; i < 12; i++) {
                
                if (mainInterfaceData[13+i].isInside(ofVec2f(x,y))) {
                    synths[activeSynth].changeMusicScale(i);
                    if(!mainInterfaceData[13+i].onOff) {
                        mainInterfaceData[13+i].blinkOn();
                        mainInterfaceData[13+i].switchColor();
                        // mainInterfaceData[25+i].blinkOn();
                        mainInterfaceData[25+i].onOff = true;
                        
                    } else {
                        mainInterfaceData[13+i].switchColor();
                        mainInterfaceData[25+i].onOff = false;
                    }
                    synths[activeSynth].userScale = true;
                    
                    markScaleSteps(63);
                    hvSlotD.blink();
                }
            }
            
            
            
            //toogle grid preset container
            muster.isInside(ofVec2f(x,y));
            
        }
        
        else if (currentState == STATE_BPM){
            
            if (mainInterfaceData[43].isInside(ofVec2f(x,y))) {
                bpmButtonPress();
                mainInterfaceData[43].blinkOn();
            }
            
            
            //bpm synth bpm metro factor
            else if(mainInterfaceData[55].isInside(ofVec2f(x,y))) {
                for (int i = 0; i < 4; i++) {
                    if(mainInterfaceData[112+i].isInside(ofVec2f(x,y))) {
                        synths[synthButton[0]].nextPulseDivision = 4-i;
                        mainInterfaceData[112+i].setOn();
                        mainInterfaceData[112+i].blinkOn();
                        synths[synthButton[0]].setAllADSR(synths[synthButton[0]].preset);
                    } else {
                        mainInterfaceData[112+i].setOff();
                    }
                }
            }
            
            else if(mainInterfaceData[56].isInside(ofVec2f(x,y))) {
                for (int i = 0; i < 4; i++) {
                    if(mainInterfaceData[116+i].isInside(ofVec2f(x,y))) {
                        synths[synthButton[1]].nextPulseDivision = 4-i;
                        mainInterfaceData[116+i].setOn();
                        mainInterfaceData[116+i].blinkOn();
                        synths[synthButton[1]].setAllADSR(synths[synthButton[1]].preset);
                    } else {
                        mainInterfaceData[116+i].setOff();
                    }
                }
            }
            
            else if(mainInterfaceData[57].isInside(ofVec2f(x,y))) {
                for (int i = 0; i < 4; i++) {
                    if(mainInterfaceData[120+i].isInside(ofVec2f(x,y))) {
                        synths[synthButton[2]].nextPulseDivision = 4-i;
                        mainInterfaceData[120+i].setOn();
                        mainInterfaceData[120+i].blinkOn();
                        synths[synthButton[2]].setAllADSR(synths[synthButton[2]].preset);
                    } else {
                        mainInterfaceData[120+i].setOff();
                    }
                }
            }
            
            //pulse direction
            else if (x > designGrid[0][1].x-designGrid[0][0].x && x < designGrid[0][1].x+designGrid[0][0].x){
                for (int i = 0; i < 4; i++) {
                    if(directionClickZonesA[i].inside(x,y)){
                        if(synths[synthButton[0]].connectedDirection[i] && synths[synthButton[0]].activeDirection[i] ) {
                            synths[synthButton[0]].connectedDirection[i] =  false;
                        } else if (!synths[synthButton[0]].connectedDirection[i] && synths[synthButton[0]].activeDirection[i]) {
                            synths[synthButton[0]].activeDirection[i] =  false;
                            synths[synthButton[0]].pulsePlane.activeDirs--;
                        } else {
                            synths[synthButton[0]].connectedDirection[i] =  true;
                            synths[synthButton[0]].activeDirection[i] =  true;
                            synths[synthButton[0]].pulsePlane.activeDirs++;
                        }
                    }
                }
                synths[synthButton[0]].pulsePlane.lineMeshQA.clear();
                synths[synthButton[0]].pulsePlane.lineMeshQB.clear();
                synths[synthButton[0]].pulsePlane.lineMesh.clear();
                synths[synthButton[0]].pulsePlane.lineWaitForBeat = true;
            }
            
            else if (x > designGrid[1][1].x-designGrid[0][0].x && x < designGrid[1][1].x+designGrid[0][0].x) {
                for (int i = 0; i < 4; i++) {
                    if (directionClickZonesB[i].inside(x, y)){
                        if(synths[synthButton[1]].connectedDirection[i] && synths[synthButton[1]].activeDirection[i] ) {
                            synths[synthButton[1]].connectedDirection[i] =  false;
                        } else if (!synths[synthButton[1]].connectedDirection[i] && synths[synthButton[1]].activeDirection[i]) {
                            synths[synthButton[1]].activeDirection[i] =  false;
                            synths[synthButton[1]].pulsePlane.activeDirs--;
                        } else {
                            synths[synthButton[1]].connectedDirection[i] =  true;
                            synths[synthButton[1]].activeDirection[i] =  true;
                            synths[synthButton[1]].pulsePlane.activeDirs++;
                        }
                    }
                }
                synths[synthButton[1]].pulsePlane.lineMeshQA.clear();
                synths[synthButton[1]].pulsePlane.lineMeshQB.clear();
                synths[synthButton[1]].pulsePlane.lineMesh.clear();
                synths[synthButton[1]].pulsePlane.lineWaitForBeat = true;
            }
            
            else if (x > designGrid[2][1].x-designGrid[0][0].x && x < designGrid[2][1].x+designGrid[0][0].x) {
                for (int i = 0; i < 4; i++) {
                    if (directionClickZonesC[i].inside(x, y)){
                        if(synths[synthButton[2]].connectedDirection[i] && synths[synthButton[2]].activeDirection[i] ) {
                            synths[synthButton[2]].connectedDirection[i] =  false;
                        } else if (!synths[synthButton[2]].connectedDirection[i] && synths[synthButton[2]].activeDirection[i]) {
                            synths[synthButton[2]].activeDirection[i] =  false;
                            synths[synthButton[2]].pulsePlane.activeDirs--;
                        } else {
                            synths[synthButton[2]].connectedDirection[i] =  true;
                            synths[synthButton[2]].activeDirection[i] =  true;
                            synths[synthButton[2]].pulsePlane.activeDirs++;
                        }
                    }
                }
                synths[synthButton[2]].pulsePlane.lineMeshQA.clear();
                synths[synthButton[2]].pulsePlane.lineMeshQB.clear();
                synths[synthButton[2]].pulsePlane.lineMesh.clear();
                synths[synthButton[2]].pulsePlane.lineWaitForBeat = true;
                
            }
        }
        
        else if (currentState == STATE_HARMONY) {
            
            for(int i = 0; i < 3; i++) {
                if(  mainInterfaceData[58+i].isInside(ofVec2f(x,y))) {
                    if (  (x-mainInterfaceData[58+i].minX) > ((mainInterfaceData[58+i].maxX-mainInterfaceData[58+i].minX)/2)) {
                        if(synths[synthButton[i]].globalHarmony){
                            synths[synthButton[i]].setKeyNote(12);
                        } else {
                            synths[synthButton[i]].setKeyNote(1);
                        }
                    } else {
                        if(synths[synthButton[i]].globalHarmony){
                            synths[synthButton[i]].setKeyNote(-12);
                        } else {
                            synths[synthButton[i]].setKeyNote(-1);
                        }
                    }
                    
                    mainInterfaceData[58+i].elementName = ofToString(notes[synths[synthButton[i]].keyNote%12]) +" / "+ofToString(synths[synthButton[i]].keyNote);
                    mainInterfaceData[58+i].setStringWidth();
                    
                    mainInterfaceData[58+i].blinkOn();
                    //setNewGUI();
                    
                    if(i==0) {
                        hvSlotA.blink();
                    }
                    
                    if(i==1) {
                        hvSlotB.blink();
                    }
                    
                    if(i==2) {
                        hvSlotC.blink();
                    }
                }
            }
            
            for (int i = 132; i < 132+3; i++){
                if(mainInterfaceData[i].isInside(ofVec2f(x,y))){
                    if (  (x-mainInterfaceData[i].minX) > ((mainInterfaceData[i].maxX-mainInterfaceData[i].minX)/2)) {
                        synths[synthButton[i-132]].makeScaleStep(1);
                    } else {
                        synths[synthButton[i-132]].makeScaleStep(-1);
                    }
                    
                    if(i-132 == 0){
                        hvSlotA.blink();
                    } else if(i-132 == 1) {
                        hvSlotB.blink();
                    }else if(i -132 == 2) {
                        hvSlotC.blink();
                    }
                    markScaleSteps();
                }
            }
            
            if (mainInterfaceData[43].isInside(ofVec2f(x,y))) {
                harmonyButtonPress();
                mainInterfaceData[43].blinkOn();
            }
            
            else if (mainInterfaceData[61].isInside(ofVec2f(x,y))) {
                if (  (x-mainInterfaceData[61].minX) > ((mainInterfaceData[61].maxX-mainInterfaceData[61].minX)/2)) {
                    globalKey = ofWrap(globalKey+1,0,12);
                } else {
                    globalKey = ofWrap(globalKey-1,0,12);
                }
                
                
                for (int i = 0; i < 3; i++) {
                    if(synths[synthButton[i]].globalHarmony){
                        //synths[synthButton[i]].setKeyNote( ((synths[synthButton[i]].keyNote)-( (synths[synthButton[i]].keyNote/12)*12 +(globalKey%12) ))*-1 );
                        if (synths[synthButton[i]].keyNote == 96) {
                            synths[synthButton[i]].setKeyNote(    (globalKey%12) - (synths[synthButton[i]].keyNote%12) -12 );
                        } else {
                            synths[synthButton[i]].setKeyNote(    (globalKey%12) - (synths[synthButton[i]].keyNote%12) );
                        }
                        mainInterfaceData[58+i].elementName = ofToString(notes[synths[synthButton[i]].keyNote%12]) +" / "+ofToString(synths[synthButton[i]].keyNote);
                        mainInterfaceData[58+i].setStringWidth();
                        mainInterfaceData[58+i].blinkOn();
                    }
                }
                
                mainInterfaceData[61].elementName = ofToString(notes[globalKey%12]);
                mainInterfaceData[61].setStringWidth();
                mainInterfaceData[61].blinkOn();
                
                
                if(synths[synthButton[0]].globalHarmony) {
                    hvSlotA.blink();
                }
                
                if(synths[synthButton[1]].globalHarmony) {
                    hvSlotB.blink();
                }
                
                if(synths[synthButton[2]].globalHarmony) {
                    hvSlotC.blink();
                }
                
            }
            
            else if(  mainInterfaceData[62].isInside(ofVec2f(x,y))) {
                if (  (x-mainInterfaceData[62].minX) > ((mainInterfaceData[62].maxX-mainInterfaceData[62].minX)/2)) {
                    globalScaleVecPos = ofWrap(globalScaleVecPos+1, 0, scaleCollection.scaleVec.size());
                    
                    for (int i = 0; i < 3; i++) {
                        if(synths[synthButton[i]].globalHarmony){
                            synths[synthButton[i]].currentScaleVecPos = globalScaleVecPos;
                            synths[synthButton[i]].userScale = false;
                            synths[synthButton[i]].setMusicScale(scaleCollection, synths[synthButton[i]].currentScaleVecPos%scaleCollection.scaleVec.size() );
                        }
                    }
                    
                } else {
                    globalScaleVecPos = ofWrap(globalScaleVecPos-1, 0, scaleCollection.scaleVec.size());
                    
                    for (int i = 0; i < 3; i++) {
                        if(synths[synthButton[i]].globalHarmony){
                            synths[synthButton[i]].currentScaleVecPos = globalScaleVecPos;
                            synths[synthButton[i]].userScale = false;
                            synths[synthButton[i]].setMusicScale(scaleCollection, synths[synthButton[i]].currentScaleVecPos%scaleCollection.scaleVec.size() );
                        }
                    }
                }
                //setNewGUI();
                markScaleSteps();
                mainInterfaceData[62].elementName = scaleCollection.scaleVec.at(globalScaleVecPos).name;
                mainInterfaceData[62].setStringWidth();
                
                mainInterfaceData[62].blinkOn();
                
                if(synths[synthButton[0]].globalHarmony) {
                    hvSlotA.blink();
                    mainInterfaceData[58].blinkOn();
                }
                
                if(synths[synthButton[1]].globalHarmony) {
                    hvSlotB.blink();
                    mainInterfaceData[59].blinkOn();
                }
                
                if(synths[synthButton[2]].globalHarmony) {
                    hvSlotC.blink();
                    mainInterfaceData[60].blinkOn();
                }
                
            }
            
        }
        
        else if (currentState == STATE_EDIT) {
            
            
            //muster container
            if (mainInterfaceData[39].isInside(ofVec2f(x,y))) {
                int musterIndex =  muster.isInside(ofVec2f(x,y));
                
                if (!muster.saveReady) {
                    if (musterIndex >= 0) {
                        synths[activeSynth].loadMuster(muster.flips.at(musterIndex).layerInfo);
                        markSynthNotes(63);
                    }
                } else if (musterIndex >= 0){
                    synths[activeSynth].getLayerInfo(muster.flips.at(musterIndex).layerInfo);
                    muster.flips.at(musterIndex).makeTex();
                    muster.saveReady = false;
                }
            }
            
            //back to State_default
            else if(  mainInterfaceData[43].isInside(ofVec2f(x,y))) {
                buttonTwoPress();
                mainInterfaceData[43].blinkOn();
            }
            
            /*
             //scale
             if(  mainInterfaceData[5].isInside(ofVec2f(x,y))) {
             if (  (x-mainInterfaceData[5].minX) > ((mainInterfaceData[5].maxX-mainInterfaceData[5].minX)/2)) {
             synths[activeSynth].currentScaleVecPos = ofWrap(synths[activeSynth].currentScaleVecPos+1,0,scaleCollection.scaleVec.size());
             } else {
             synths[activeSynth].currentScaleVecPos = ofWrap(synths[activeSynth].currentScaleVecPos-1,0,scaleCollection.scaleVec.size());
             }
             synths[activeSynth].userScale = false;
             synths[activeSynth].setMusicScale(scaleCollection, synths[activeSynth].currentScaleVecPos%scaleCollection.scaleVec.size() );
             setNewGUI();
             mainInterfaceData[5].blinkOn();
             // detailEditInterfaceOn();
             cout << synths[activeSynth].activeScale.name  << endl;
             }
             */
            
            //synth
            else if(  mainInterfaceData[7].isInside(ofVec2f(x,y))) {
                if (  (x-mainInterfaceData[7].minX) > ((mainInterfaceData[7].maxX-mainInterfaceData[7].minX)/2)) {
                    synths[activeSynth].changePreset(true);
                } else {
                    synths[activeSynth].changePreset(false);
                }
                setNewGUI();
                mainInterfaceData[7].blinkOn();
            }
            
            //toggle state_edit_detail
            else if(  mainInterfaceData[44].isInside(ofVec2f(x,y))) {
                if(!synths[activeSynth].globalHarmony) {
                    buttonEditDetail();
                    mainInterfaceData[44].blinkOn();
                    
                    mainInterfaceData[131].elementName = "LOCAL SETTINGS";
                    mainInterfaceData[131].setStringWidth();
                    mainInterfaceData[131].activateDarkerColor();
                }
            }
            
            //toggle save grid
            else if(  mainInterfaceData[12].isInside(ofVec2f(x,y))) {
                //leer
                muster.saveReady = true;
                mainInterfaceData[12].blinkOn();
            }
            
            //toggle synth-> globalHarmony
            else if(  mainInterfaceData[111].isInside(ofVec2f(x,y))) {
                synths[activeSynth].globalHarmony = !synths[activeSynth].globalHarmony;
                mainInterfaceData[111].blinkOn();
                if (synths[activeSynth].globalHarmony){
                    mainInterfaceData[111].elementName = "GLOBAL HARMONY";
                    mainInterfaceData[111].setStringWidth();
                    mainInterfaceData[44].activateDarkerColor();
                    synths[activeSynth].currentScaleVecPos = globalScaleVecPos;
                    synths[activeSynth].userScale = false;
                    synths[activeSynth].setMusicScale(scaleCollection, synths[activeSynth].currentScaleVecPos%scaleCollection.scaleVec.size() );
                    synths[activeSynth].setKeyNote( (globalKey%12) - (synths[activeSynth].keyNote%12));
                    hvSlotD.changeC(ofColor::fromHsb(255,0,195,255));
                    setNewGUI();
                } else {
                    mainInterfaceData[111].elementName = "LOCAL HARMONY";
                    mainInterfaceData[111].setStringWidth();
                    mainInterfaceData[44].activateOnColor();
                    hvSlotD.changeC(ofColor::fromHsb(synths[activeSynth].colorHue, 235, 180,255));
                    setNewGUI();
                }
                
            }
            
            else if(mainInterfaceData[135].isInside(ofVec2f(x,y))){
                if (  (x-mainInterfaceData[135].minX) > ((mainInterfaceData[135].maxX-mainInterfaceData[135].minX)/2)) {
                    synths[activeSynth].makeScaleStep(1);
                } else {
                    synths[activeSynth].makeScaleStep(-1);
                }
                hvSlotD.blink();
                markScaleSteps(63);
            }
            
            
            
            //toggle get random grid
            /*
             if(  mainInterfaceData[40].isInside(ofVec2f(x,y))) {
             //leer
             for (int i = 0; i < TILES; i++) {
             for (int j = 0; j < TILES; j++) {
             if (ofRandom(100)>60) {
             synths[activeSynth].tapEvent(i, j);
             }
             }
             }
             mainInterfaceData[40].blinkOn();
             markSynthNotes(63);
             }
             */
            
            
        }
        
        else if (currentState == STATE_SAVE){
            
            if(saveManager.touchDown) {
                saveManager.touchDown = false;
            }
            
            if (lastClick.x == x && lastClick.y == y){
                if (!saveManager.slotDetail && !saveManager.animate) {
                    saveManager.isInside(ofVec3f(x,y,0));
                    if(saveManager.animate){
                        openSlotInterface();
                    }
                }
            }
            
            if(!saveManager.slotDetail){
                if (mainInterfaceData[47].isInside(ofVec2f(x,y))) {
                    loadSaveButtonPress();
                    mainInterfaceData[47].blinkOn();
                    
                }
                
                else if (mainInterfaceData[125].isInside(ofVec2f(x,y))) {
                    savePreset();
                    mainInterfaceData[125].blinkOn();
                }
            }
            
            if(saveManager.slotDetail && !saveManager.animate) {
                if (mainInterfaceData[50].isInside(ofVec2f(x,y))) {
                    closeSlotInterface();
                    mainInterfaceData[50].blinkOn();
                    if(saveManager.confirmDel) {
                        mainInterfaceData[124].setColor( ofColor::fromHsb(255,0,195,255) );
                        mainInterfaceData[124].activateOnColor();
                        mainInterfaceData[124].blinkOn();
                        saveManager.confirmDel = false;
                    }
                }
                
                else if (mainInterfaceData[126].isInside(ofVec2f(x,y))) {
                    mainInterfaceData[126].blinkOn();
                    loadPreset();
                    loadSaveButtonPress();
                    closeSlotInterface();
                }
                
                else  if (mainInterfaceData[124].isInside(ofVec2f(x,y))) {
                    if (saveManager.confirmDel){
                        saveManager.deleteSave();
                        mainInterfaceData[124].blinkOn();
                        closeSlotInterface();
                        saveManager.confirmDel = false;
                        mainInterfaceData[124].setColor( ofColor::fromHsb(255,0,195,255) );
                        mainInterfaceData[124].activateOnColor();
                        mainInterfaceData[124].blinkOn();
                    } else {
                        mainInterfaceData[124].setColor(0);
                        mainInterfaceData[124].activateOnColor();
                        mainInterfaceData[124].blinkOn();
                        saveManager.confirmDel = true;
                    }
                    
                } else if (saveManager.selectInnerIt->second.slotInfo.testRect.inside(ofVec2f(x,y-saveManager.scrollLocation))) {
                    saveManager.cycleHighlightColor();
                }
            }
        }
        
        else if (currentState == STATE_SETTINGS){
            
            if (mainInterfaceData[130].isInside(ofVec2f(x,y))) {
                mainInterfaceData[130].blinkOn();
                sendMidi = !sendMidi;
                if(sendMidi) {
                    mainInterfaceData[130].elementName = "MIDI ON";
                    mainInterfaceData[130].setStringWidth();
                    setupMidi();
                } else {
                    for (int i = 0; i < 3; i++){
                        synths[i].setAllNotesOff();
                    }
                    midiOut.closePort();
                    mainInterfaceData[130].elementName = "MIDI OFF";
                    mainInterfaceData[130].setStringWidth();
                }
            } else if (mainInterfaceData[138].isInside(ofVec2f(x,y))) {
                mainInterfaceData[138].blinkOn();
                dynamicVelo = !dynamicVelo;
                if(dynamicVelo) {
                    mainInterfaceData[138].elementName = "DYNAMIC VELOCITY";
                    mainInterfaceData[138].setStringWidth();
                    mainInterfaceData[140].activateDarkerColor();
                    for (int i = 0; i < 3; i++) {
                        synths[i].dynamicVelo = true;
                    }
                    
                } else {
                    mainInterfaceData[138].elementName = "STATIC VELOCITY";
                    mainInterfaceData[138].setStringWidth();
                    mainInterfaceData[140].activateOnColor();
                    for (int i = 0; i < 3; i++) {
                        synths[i].dynamicVelo = false;
                    }
                }
            }
            
            else if (mainInterfaceData[43].isInside(ofVec2f(x,y))) {
                mainInterfaceData[43].blinkOn();
                settingsButtonPress();
            }
            else if (mainInterfaceData[141].isInside(ofVec2f(x,y))) {
                mainInterfaceData[141].blinkOn();
                soundDelay = !soundDelay;
                if(soundDelay) {
                    mainInterfaceData[141].elementName = "DELAY ON";
                    mainInterfaceData[141].setStringWidth();
                    mainInterfaceData[142].activateOnColor();
                    if(!autoDelay){
                        mainInterfaceData[144].activateOnColor();
                    }
                    setupAudio();
                } else {
                    mainInterfaceData[141].elementName = "DELAY OFF";
                    mainInterfaceData[141].setStringWidth();
                    mainInterfaceData[142].activateDarkerColor();
                    if(!autoDelay){
                        mainInterfaceData[144].activateDarkerColor();
                    }
                    setupAudio();
                }
            }
            else if (mainInterfaceData[142].isInside(ofVec2f(x,y))) {
                if(soundDelay){
                    mainInterfaceData[142].blinkOn();
                    autoDelay = !autoDelay;
                    if(autoDelay) {
                        mainInterfaceData[142].elementName = "DYNAMIC DELAY";
                        mainInterfaceData[142].setStringWidth();
                        tonicSynth.setParameter("delay",getBpmValue(dynamicDelayValue));
                        mainInterfaceData[144].activateDarkerColor();
                    } else {
                        mainInterfaceData[142].elementName = "STATIC DELAY";
                        mainInterfaceData[142].setStringWidth();
                        mainInterfaceData[144].activateOnColor();
                        if(!autoDelay){
                            tonicSynth.setParameter("delay",getBpmValue(staticDelayValue));
                        }
                    }
                }
                
            }
            
            else if (mainInterfaceData[145].isInside(ofVec2f(x,y))) {
                mainInterfaceData[145].blinkOn();
                soundReverb = !soundReverb;
                if(soundReverb) {
                    mainInterfaceData[145].elementName = "REVERB ON";
                    mainInterfaceData[145].setStringWidth();
                    mainInterfaceData[147].activateOnColor();
                    mainInterfaceData[149].activateOnColor();
                    setupAudio();
                } else {
                    mainInterfaceData[145].elementName = "REVERB OFF";
                    mainInterfaceData[145].setStringWidth();
                    mainInterfaceData[147].activateDarkerColor();
                    mainInterfaceData[149].activateDarkerColor();
                    setupAudio();
                }
            }
            
            
        }
        }
    }
    
    
    //for the sleeptimer
    if (!sleepMode){
        lastTouch = ofGetElapsedTimef();
    } else {
        sleepMode = false;
        lastTouch = ofGetElapsedTimef();
        for  (int i = 0; i < 8; i++){
            mainInterfaceData.at(defaultStateIndex[i]).switchColor();
        }
    }
    
    
}

#if TARGET_OS_IPHONE
//--------------------------------------------------------------
void ofApp::touchDoubleTap(ofTouchEventArgs & touch){
    
}

//--------------------------------------------------------------
void ofApp::touchCancelled(ofTouchEventArgs & touch){
    
}

//--------------------------------------------------------------
void ofApp::lostFocus(){
    startUp = true;
    volumeRestartTarget = mainVol;
    volumeRestart = 0.0;
    tonicSynth.setParameter("mainVolumeRamp",Tonic::mapLinToLog(0.0,0.0,1.0));
    saveToXml("settings.xml");
    //volumeRampValueChanged(mainVol);
    
    //    mainVol = 0.0;
    
    for (int i = 0; i < 3; i++){
        synths[i].setAllNotesOff();
    }
}

//--------------------------------------------------------------
void ofApp::gotFocus(){
    // startUp = true;
    //volumeRestartTarget = mainVol;
    //volumeRestart = 0.0;
    //tonicSynth.setParameter("mainVolumeRamp",Tonic::mapLinToLog(0.0,0.0,1.0));
    lastTouch = ofGetElapsedTimef();
}

//--------------------------------------------------------------
void ofApp::onSwipe(ofSwipeGestureRecognizerArgs & args){
    
    if(!interfaceMoving && !sleepMode){
        
        if (currentState == STATE_EDIT && !mainInterfaceData[136].touchDown){
            //left
            if(args.direction == 1){
                if(synths[synthButton[0]].trackSwitchOn) {
                    buttonOnePress();
                } else {
                    synths[synthButton[0]].trackSwitchOn = true;
                }
            }
            //right
            else if(args.direction == 2){
                if(synths[synthButton[2]].trackSwitchOn) {
                    buttonThreePress();
                } else {
                    synths[synthButton[2]].trackSwitchOn = true;
                }
            }
            //up
            else if(args.direction == 4 || args.direction == 8){
                buttonTwoPress();
                mainInterfaceData[43].blinkOn();
            }
        }
        else if (currentState == STATE_VOLUME &&
                 !(mainInterfaceData[0].touchDown ||
                   mainInterfaceData[1].touchDown ||
                   mainInterfaceData[2].touchDown ||
                   mainInterfaceData[3].touchDown)
                 ){
            if(args.direction == 4 || args.direction == 8){
                buttonFourPress();
                mainInterfaceData[43].blinkOn();
            }
        }
        else if (currentState == STATE_HARMONY){
            if(args.direction == 4 || args.direction == 8){
                harmonyButtonPress();
                mainInterfaceData[43].blinkOn();
            }
        }
        else if (currentState == STATE_BPM && !mainInterfaceData[45].touchDown){
            if(args.direction == 4 || args.direction == 8){
                bpmButtonPress();
                mainInterfaceData[43].blinkOn();
            }
        }
        else if (currentState == STATE_EDIT_DETAIL &&
                 !(mainInterfaceData[136].touchDown ||
                   mainInterfaceData[49].touchDown)
                 ){
            if(args.direction == 4 || args.direction == 8){
                buttonEditDetail();
                mainInterfaceData[11].blinkOn();
            }
        }
        else if (currentState == STATE_SETTINGS &&
                 !(mainInterfaceData[139].touchDown ||
                   mainInterfaceData[143].touchDown ||
                   mainInterfaceData[146].touchDown ||
                   mainInterfaceData[148].touchDown)
                 ){
            if(args.direction == 1 || args.direction == 2){
                mainInterfaceData[43].blinkOn();
                settingsButtonPress();
            }
        }
        else if (currentState == STATE_SAVE){
            if(!saveManager.slotDetail){
                if(args.direction == 1|| args.direction == 2){
                    loadSaveButtonPress();
                    mainInterfaceData[47].blinkOn();
                    
                }
            }
            else if(saveManager.slotDetail && !saveManager.animate){
                if(args.direction == 4 || args.direction == 8){
                    closeSlotInterface();
                    mainInterfaceData[50].blinkOn();
                }
            }
        }
        else if(currentState == STATE_DEFAULT){
            if(args.direction == 1){
                mainInterfaceData[46].blinkOn();
                loadSaveButtonPress();
            }
            else if(args.direction == 2){
                mainInterfaceData[40].blinkOn();
                settingsButtonPress();
            }
            
            if(args.direction == 4 || args.direction == 8){
                float x = ofGetMouseX();
                float y = ofGetMouseY();
                
                if( (x > designGrid[0][1].x-designGrid[0][0].x && x < designGrid[0][1].x+designGrid[0][0].x) &&
                   (y > designGrid[0][1].y-designGrid[0][0].y && y < designGrid[0][1].y+designGrid[0][0].y) ){
                    if(!synths[synthButton[0]].trackSwitchOn) {
                        synths[synthButton[0]].trackSwitchOn = true;
                    } else {
                        buttonOnePress();
                    }
                }
                
                else if( (x > designGrid[1][1].x-designGrid[0][0].x && x < designGrid[1][1].x+designGrid[0][0].x) &&
                        (y > designGrid[1][1].y-designGrid[0][0].y && y < designGrid[1][1].y+designGrid[0][0].y) ){
                    buttonTwoPress();
                }
                
                else if( (x > designGrid[2][1].x-designGrid[0][0].x && x < designGrid[2][1].x+designGrid[0][0].x) &&
                        (y > designGrid[2][1].y-designGrid[0][0].y && y < designGrid[2][1].y+designGrid[0][0].y) ){
                    if(!synths[synthButton[2]].trackSwitchOn) {
                        synths[synthButton[2]].trackSwitchOn = true;
                    } else {
                        buttonThreePress();
                    }
                }
                
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::gotMemoryWarning(){
    
}

//--------------------------------------------------------------
void ofApp::deviceOrientationChanged(int newOrientation){
    if (newOrientation == OF_ORIENTATION_90_RIGHT) {
        ofSetOrientation(OF_ORIENTATION_90_RIGHT);
    }
    
    if(newOrientation == OF_ORIENTATION_90_LEFT){
        ofSetOrientation(OF_ORIENTATION_90_LEFT);
    }
    
}
#endif

#if !TARGET_OS_IPHONE
//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    //  makeDesignGrid();
    ofSetWindowShape(w, h);
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}

#endif

void ofApp::drawDebug() {
    
    
    if (!debugCam) {
        testCam.begin();
    } else {
        cam.begin();
    }
    
    for (int i = 0; i < synths.size(); i++) {
        synths[i].myNode.transformGL();
        synths[i].drawDebug();
        synths[i].myNode.restoreTransformGL();
        
        synthPos[i].draw();
    }
    
    
    synthActivePos.draw();
    // camActiveSynth.draw();
    camNotActiveSynth.draw();
    camEditDetailPos.draw();
    //camDetailToEdit.draw();
    
    testCam.draw();
    
    oneToActive.draw();
    twoToActive.draw();
    threeToActive.draw();
    
    oneToBack.draw();
    twoToBack.draw();
    threeToBack.draw();
    
    camPath.draw();
    camPathBack.draw();
    camEditToDetail.draw();
    
    centerToThree.draw();
    centerToOne.draw();
    
    TwoVolumeLayerPathOn.draw();
    TwoHarmonyPathOn.draw();
    
    TwoLoadPathOn.draw();
    
    if (!debugCam) {
        testCam.end();
    } else {
        cam.end();
    }
    
    ofPushStyle();
    ofDrawBitmapString(ofToString(ofGetFrameRate()), 20, 20);
    ofDrawBitmapString("Plane Intersect: "+ofToString(intersectPos), 20,40);
    ofDrawBitmapString("Grid X: "+ofToString(vectorPosX), 20, 60);
    ofDrawBitmapString("GridY: "+ofToString(vectorPosY), 20, 80);
    ofDrawBitmapString("BPM Counter: "+ofToString(timeCounter), 20, 120);
    ofPopStyle();
    
    mainInterface.drawWireframe();
    
    for (int i = 1; i < 3; i++) {
        
        ofLine(0, (ofGetHeight()/3)*i, ofGetWidth(), (ofGetHeight()/3)*i );
        ofLine(ofGetWidth()/3*i, 0, ofGetWidth()/3*i, ofGetHeight());
    }
    
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            ofEllipse(designGrid[i][j], 10, 10);
        }
    }
    
    for (int i = 0; i < 4; i++){
        directionClickZonesA[i].draw();
        directionClickZonesB[i].draw();
        directionClickZonesC[i].draw();
    }
}

void ofApp::intersectPlane(float x_,float y_){
    //raytesting
    worldMouse = testCam.screenToWorld(ofVec3f(x_,y_,0.0));
    
    mouseRay.s = worldMouse;
    mouseRay.t = worldMouse-testCam.getPosition();
    
    thisIntersect.intersect(mouseRay, intersectPos);
    
    bool test = thisIntersect.intersect(mouseRay);
    
    ofNode temp;
    temp.setPosition(intersectPos-synths[activeSynth].myNode.getPosition());
    ofQuaternion tempRot = synths[activeSynth].myNode.getOrientationQuat();
    temp.rotateAround(tempRot.inverse(), ofVec3f(0,0,0));
    
    intersectPos = temp.getPosition()+ofVec3f( (TILES*TILESIZE)/2,(TILES*TILESIZE)/2,0);
    
    vectorPosX =  int((intersectPos.x/ float (TILESIZE)));
    vectorPosY = int((intersectPos.y/ float(TILESIZE)));
}

ofVec3f ofApp::intersectPlane(ofVec2f target_) {
    
    ofCamera tempNode;
    tempNode.setNearClip(nearClip);
    tempNode.setFarClip(farClip);
    tempNode.setFov(camFov);
    tempNode.setPosition(camNotActivePos);
    tempNode.lookAt(ofVec3f(0,0,0)-tempNode.getZAxis());
    ofVec3f wMouse = tempNode.screenToWorld( ofVec3f(target_.x,target_.y,0.0), ofRectangle(ofPoint(0,0), sWIDTH, sHEIGHT));
    ofRay ray;
    ray.s = wMouse;
    ray.t = wMouse-tempNode.getPosition();
    
    ofPlanePrimitive planeTemp;
    planeTemp.set(2000, 2000);
    planeTemp.setPosition(0, 0, 0);
    ofPlane iP;
    iP.setFrom(planeTemp);
    ofVec3f cord;
    iP.intersect(ray, cord);
    
    return cord;
}




bool ofApp::pointInsideGrid(ofVec3f p_) {
    bool rVal;
    if( (abs(p_.x) <= (TILES*TILESIZE/2) ) && (abs(p_.y) <= (TILES*TILESIZE/2)) ) {
        rVal = true;
    } else {
        rVal = false;
    }
    return rVal;
}

void ofApp::pulseEvent(int div){
    
    /*
     if (currentState == STATE_BPM) {
     for (int i = 0; i < 3; i++) {
     if (synths[synthButton[i]].pulseDivision == div) {
     mainInterfaceData[55+i].blinkOn();
     }
     }
     }
     */
    
    if(currentState == STATE_HARMONY){
        if (synths[synthButton[0]].pulseDivision == div) {
            hvSlotA.update(synths[synthButton[0]].noteHistory, mainInterfaceData, mainInterface);
        }
        if (synths[synthButton[1]].pulseDivision == div) {
            hvSlotB.update(synths[synthButton[1]].noteHistory, mainInterfaceData, mainInterface);
        }
        if (synths[synthButton[2]].pulseDivision == div) {
            hvSlotC.update(synths[synthButton[2]].noteHistory, mainInterfaceData, mainInterface);
        }
    } else if (currentState == STATE_EDIT_DETAIL || currentState == STATE_EDIT) {
        if (synths[activeSynth].pulseDivision == div) {
            hvSlotD.updateStateEditDetail(synths[activeSynth].noteHistory, mainInterfaceData, mainInterface, 63);
        }
    }
    
    for (int i = 0; i < synths.size(); i++) {
        if (synths[i].pulseDivision == div) {
            
            synths[i].timeCounter++;
            if (synths[i].timeCounter > TILES) {
                synths[i].timeCounter = 0;
                synths[i].nextDirection();
                
            }
            
            synths[i].noteTrigger();
            
            if (synths[i].pulsePlane.stepCounter == ((TILES+1)*4)-1 ){
                synths[i].pulsePlane.stepCounter = 0;
            } else {
                synths[i].pulsePlane.stepCounter++;
            }
            
            synths[i].bpmTick = ofGetElapsedTimeMillis() - synths[i].lastTick;
            synths[i].lastTick = ofGetElapsedTimeMillis();
            
            
            synths[i].pulsePlane.lastTick = synths[i].lastTick;
            
            
            synths[i].tickTimes.pop_back();
            synths[i].tickTimes.push_front(synths[i].bpmTick);
            
            
            
            
            synths[i].bpmTick = 0.0;
            
            for (int j = 0; j < synths[i].tickTimes.size(); j++) {
                synths[i].bpmTick+= synths[i].tickTimes.at(j);
            }
            
            
            synths[i].bpmTick /= synths[i].tickTimes.size();
            
        }
    }
    
}

void ofApp::pulseEventDiv1(float& val) {
    pulseEvent(1);
}


void ofApp::pulseEventDiv2(float & val){
    pulseEvent(2);
}

void ofApp::pulseEventDiv3(float & val){
    pulseEvent(3);
}

void ofApp::pulseEventDiv4(float & val){
    pulseEvent(4);
}




void ofApp::setMainVolume(float & in_) {
        
    tonicSynth.setParameter("mainVolumeRamp", pow(in_,4)   );
    
    cout << "logvol:"  <<   pow(in_,4)  << endl;;
    
}

void ofApp::audioOut (float * output, int bufferSize, int nChannels){
    tonicSynth.fillBufferOfFloats(output, bufferSize, nChannels);
}



void ofApp::updateCamera(float pct_){
    
    if (animCam && pct_ == 1.0) {
        animCam = false;
        testCam.setPosition( camUsePath.getVertices().at(camUsePath.size()-1));
        ofQuaternion tempRot;
        tempRot.slerp(pct_, camQuatDefault,camQuatTarget);
        testCam.setOrientation(tempRot);
        
        
    } else if (animCam) {
        
        float index = camUsePath.getIndexAtPercent(pct_);
        if(index < camUsePath.size()-1){
            ofVec3f tempPos =  (camUsePath.getVertices().at((int)index+1)-camUsePath.getVertices().at((int)index))* (index-floor(index));
            testCam.setPosition( camUsePath.getVertices().at((int)index)+ tempPos);
            testCam.setFov(ofLerp(camDefaultFov, camTargetFov, pct_));
        }
        
        ofQuaternion tempRot;
        tempRot.slerp(pct_, camQuatDefault,camQuatTarget);
        testCam.setOrientation(tempRot);
        
        
    }
    
    
}


float ofApp::easeInOut(float input_, float a_) {
    
    
    float epsilon = 0.00001;
    float min_param_a = 0.0 + epsilon;
    float max_param_a = 1.0 - epsilon;
    a_ = min(max_param_a, max(min_param_a, a_));
    a_ = 1.0-a_; // for sensible results
    
    
    float y = 0;
    if (input_<=0.5){
        y = (pow(2.0*input_, 1.0/a_))/2.0;
    } else {
        y = 1.0 - (pow(2.0*(1.0-input_), 1.0/a_))/2.0;
    }
    return y;
    
}

void ofApp::setupStatesAndAnimation() {
    
    cam.setNearClip(nearClip);
    cam.setFarClip(farClip);
    cam.setFov(20);
    
    
    //startcam settings
    testCam.setNearClip(nearClip);
    testCam.setFarClip(farClip);
    
    float bezierHandleFac = 3.0;
    
    //___---___
    //---___---
    
    //synthlayer and camera for active edit position
    synthActivePos.setPosition(0, -TILES*TILESIZE*2.35, TILESIZE*TILES*2.085);
    camActiveSynth.setPosition(synthActivePos.getGlobalPosition()+ofVec3f(0,-TILESIZE*TILES*4.8,TILES*TILESIZE*1.0));
    camNotActiveSynth.setPosition(camNotActivePos);
    camEditDetailPos.setPosition(camActiveSynth.getPosition()+ofVec3f(0,0,(TILESIZE*TILES)/2.85));
    
    camActiveSynth.lookAt(synthActivePos.getPosition() - camActiveSynth.getZAxis());
    camNotActiveSynth.lookAt(synthPos[1].getPosition()  - camNotActiveSynth.getZAxis());
    synthActivePos.setOrientation(camActiveSynth.getOrientationQuat());
    camEditDetailPos.lookAt(synthActivePos.getPosition() - camEditDetailPos.getZAxis());
    
    
    //movements camera and synths from default to active
    centerToOne.addVertex(synthPos[1].getPosition());
    centerToOne.lineTo(synthPos[0].getPosition());
    centerToOne = centerToOne.getResampledByCount(80);
    
    centerToThree.addVertex(synthPos[1].getPosition());
    centerToThree.lineTo(synthPos[2].getPosition());
    centerToThree = centerToThree.getResampledByCount(80);
    
    //-----------__________----------________
    twoToActive.addVertex(synthPos[1].getPosition());
    twoToActive.bezierTo(synthPos[1].getPosition()+ofVec3f(0,0,TILES*TILESIZE*bezierHandleFac), synthActivePos.getPosition()+ofVec3f(0,TILES*TILESIZE*bezierHandleFac,0), synthActivePos.getPosition());
    twoToActive = twoToActive.getResampledByCount(80);
    
    oneToActive.addVertex(synthPos[0].getPosition());
    oneToActive.bezierTo(synthPos[0].getPosition()+ofVec3f(0,0,TILES*TILESIZE*bezierHandleFac),synthActivePos.getPosition()+ofVec3f(0,TILES*TILESIZE*bezierHandleFac,0), synthActivePos.getPosition());
    oneToActive = oneToActive.getResampledByCount(80);
    
    threeToActive.addVertex(synthPos[2].getPosition());
    threeToActive.bezierTo(synthPos[2].getPosition()+ofVec3f(0,0,TILESIZE*TILES*bezierHandleFac),synthActivePos.getPosition()+ofVec3f(0,TILES*TILESIZE*bezierHandleFac,0), synthActivePos.getPosition());
    threeToActive = threeToActive.getResampledByCount(80);
    
    //------------__________-------------__________
    twoToBack.addVertex(synthActivePos.getPosition());
    twoToBack.bezierTo(synthActivePos.getPosition()+ofVec3f(0,0,-TILES*TILESIZE*bezierHandleFac*0.6), synthPos[1].getPosition()+ofVec3f(0,0,TILES*TILESIZE*bezierHandleFac*0.8), synthPos[1].getPosition());
    twoToBack = twoToBack.getResampledByCount(80);
    
    oneToBack.addVertex(synthActivePos.getPosition());
    oneToBack.bezierTo(synthActivePos.getPosition()+ofVec3f(0,0,-TILES*TILESIZE*bezierHandleFac*0.6), synthPos[0].getPosition()+ofVec3f(0,0,TILES*TILESIZE*bezierHandleFac*0.8), synthPos[0].getPosition());
    oneToBack = oneToBack.getResampledByCount(80);
    
    threeToBack.addVertex(synthActivePos.getPosition());
    threeToBack.bezierTo(synthActivePos.getPosition()+ofVec3f(0,0,-TILES*TILESIZE*bezierHandleFac*0.6), synthPos[2].getPosition()+ofVec3f(0,0,TILESIZE*TILES*bezierHandleFac*0.8), synthPos[2].getPosition());
    threeToBack = threeToBack.getResampledByCount(80);
    
    //_____----------_________________-------------
    camPath.addVertex(camNotActiveSynth.getPosition());
    camPath.bezierTo(camNotActiveSynth.getPosition()+ofVec3f(0,-TILES*TILESIZE*bezierHandleFac*2,0), camActiveSynth.getPosition()+ofVec3f(0,0,TILESIZE*TILES*bezierHandleFac*2) , camActiveSynth.getPosition());
    camPath = camPath.getResampledByCount(80);
    
    camPathBack.addVertex(camActiveSynth.getPosition());
    camPathBack.bezierTo(camActiveSynth.getPosition()+ofVec3f(0,TILESIZE*TILES*bezierHandleFac*0.5,0), camNotActiveSynth.getPosition()+ofVec3f(0,-TILES*TILESIZE*bezierHandleFac*2.5,0), camNotActiveSynth.getPosition());
    camPathBack = camPathBack.getResampledByCount(80);
    
    camEditToDetail.addVertex(camActiveSynth.getPosition());
    camEditToDetail.bezierTo(camActiveSynth.getPosition()+ofVec3f(0,-TILES*TILESIZE/2,0), camEditDetailPos.getPosition()+ofVec3f(0,-TILES*TILESIZE/2), camEditDetailPos.getPosition());
    camEditToDetail = camEditToDetail.getResampledByCount(80);
    
    camDetailToEdit.addVertex(camEditDetailPos.getPosition());
    camDetailToEdit.bezierTo(camEditDetailPos.getPosition()+ofVec3f(0,-TILES*TILESIZE/2,0), camActiveSynth.getPosition()+ofVec3f(0,-TILES*TILESIZE/2,0), camActiveSynth.getPosition());
    camDetailToEdit = camDetailToEdit.getResampledByCount(80);
    
    //startcam position an rotate
    testCam.setPosition(camNotActiveSynth.getPosition());
    testCam.setOrientation(camNotActiveSynth.getOrientationQuat());
    testCam.setFov(camFov);
    
    cam.setPosition(camNotActiveSynth.getPosition());
    cam.setOrientation(camNotActiveSynth.getOrientationQuat());
    cam.setFov(camFov);
    
    //___---___
    //---___---
    //harmony mode
    //from default to volume
    harmonyMatrix.rotate(55, -1, 0, 0);
    
    TwoHarmonyPathOn.addVertex(ofVec3f(0,0,0));
    TwoHarmonyPathOn.bezierTo(ofVec3f(0,0,0), ofVec3f(0,0,0), ofVec3f(0,(TILES*TILESIZE)/5.0,-(TILES*TILESIZE)/4));
    // TwoVolumeLayerPathOn = TwoVolumeLayerPathOn.getResampledByCount(80);
    
    TwoHarmonyPathOff.addVertex(ofVec3f(0,(TILES*TILESIZE)/5.0,-(TILES*TILESIZE)/4));
    TwoHarmonyPathOff.bezierTo(ofVec3f(0,0,0), ofVec3f(0,0,0), ofVec3f(0,0,0));
    // TwoVolumeLayerPathOff = TwoVolumeLayerPathOff.getResampledByCount(80);
    
    OneHarmonyPathOn = TwoHarmonyPathOn;
    OneHarmonyPathOff = TwoHarmonyPathOff;
    ThreeHarmonyPathOn = TwoHarmonyPathOn;
    ThreeHarmonyPathOff = TwoHarmonyPathOff;
    
    for (int i = 0; i < TwoHarmonyPathOn.size() ; i++) {
        OneHarmonyPathOn.getVertices().at(i) = TwoHarmonyPathOn.getVertices().at(i)+synthPos[0].getPosition();
        OneHarmonyPathOff.getVertices().at(i) = TwoHarmonyPathOff.getVertices().at(i)+synthPos[0].getPosition();
        ThreeHarmonyPathOn.getVertices().at(i) = TwoHarmonyPathOn.getVertices().at(i)+synthPos[2].getPosition();
        ThreeHarmonyPathOff.getVertices().at(i) = TwoHarmonyPathOff.getVertices().at(i)+synthPos[2].getPosition();
    }
    
    //___---___
    //---___---
    
    //time mode
    timeMatrix.rotate(-5,-1,0,0);
    
    TwoTimePathOn.addVertex(ofVec3f(0,0,0));
    TwoTimePathOn.bezierTo(ofVec3f(0,0,-(TILESIZE*TILES)/4), ofVec3f(0,(TILES*TILESIZE)/4,-(TILES*TILESIZE)/2), ofVec3f(0,-(TILESIZE*TILES)*0.75,-(TILESIZE*TILES)*0.0));
    
    TwoTimePathOff.addVertex(ofVec3f(0,-(TILESIZE*TILES)*0.75,-(TILESIZE*TILES)*0.0));
    TwoTimePathOff.bezierTo(ofVec3f(0,(TILES*TILESIZE)/4,-(TILES*TILESIZE)/2), ofVec3f(0,0,-(TILESIZE*TILES)/4), ofVec3f(0,0,0));
    
    OneTimePathOn = TwoTimePathOn;
    OneTimePathOff = TwoTimePathOff;
    ThreeTimePathOn = TwoTimePathOn;
    ThreeTimePathOff = TwoTimePathOff;
    
    for (int i = 0; i < TwoTimePathOn.size() ; i++) {
        OneTimePathOn.getVertices().at(i) = TwoTimePathOn.getVertices().at(i)+synthPos[0].getPosition();
        OneTimePathOff.getVertices().at(i) = TwoTimePathOff.getVertices().at(i)+synthPos[0].getPosition();
        ThreeTimePathOn.getVertices().at(i) = TwoTimePathOn.getVertices().at(i)+synthPos[2].getPosition();
        ThreeTimePathOff.getVertices().at(i) = TwoTimePathOff.getVertices().at(i)+synthPos[2].getPosition();
    }
    
    
    //___---___
    //---___---
    
    //volume mode
    volumeMatrix.rotate(70,-1,0,0);
    
    TwoVolumeLayerPathOn.addVertex(ofVec3f(0,0,0));
    TwoVolumeLayerPathOn.bezierTo(ofVec3f(0,0,-(TILESIZE*TILES)/4), ofVec3f(0,(TILES*TILESIZE)/4,-(TILES*TILESIZE)/2), ofVec3f(0,-(TILESIZE*TILES)*1.0,-(TILESIZE*TILES)*0.175));
    
    TwoVolumeLayerPathOff.addVertex(ofVec3f(0,-(TILESIZE*TILES)*1.0,-(TILESIZE*TILES)*0.175));
    TwoVolumeLayerPathOff.bezierTo(ofVec3f(0,(TILES*TILESIZE)/4,-(TILES*TILESIZE)/2), ofVec3f(0,0,-(TILESIZE*TILES)/4), ofVec3f(0,0,0));
    
    OneVolumeLayerPathOn = TwoVolumeLayerPathOn;
    OneVolumeLayerPathOff = TwoVolumeLayerPathOff;
    ThreeVolumeLayerPathOn = TwoVolumeLayerPathOn;
    ThreeVolumeLayerPathOff = TwoVolumeLayerPathOff;
    
    for (int i = 0; i < TwoVolumeLayerPathOn.size() ; i++) {
        OneVolumeLayerPathOn.getVertices().at(i) = TwoVolumeLayerPathOn.getVertices().at(i)+synthPos[0].getPosition();
        OneVolumeLayerPathOff.getVertices().at(i) = TwoVolumeLayerPathOff.getVertices().at(i)+synthPos[0].getPosition();
        ThreeVolumeLayerPathOn.getVertices().at(i) = TwoVolumeLayerPathOn.getVertices().at(i)+synthPos[2].getPosition();
        ThreeVolumeLayerPathOff.getVertices().at(i) = TwoVolumeLayerPathOff.getVertices().at(i)+synthPos[2].getPosition();
    }
    
    //___---___
    //---___---
    
    //load save menu
    TwoLoadPathOn.addVertex(ofVec3f(0,0,0));
    TwoLoadPathOn.bezierTo(ofVec3f(0,0,0), ofVec3f((TILES*TILESIZE*12),0,0), ofVec3f((TILES*TILESIZE*12),0,0));
    
    TwoLoadPathOff.addVertex( ofVec3f((TILES*TILESIZE*12),0,0));
    TwoLoadPathOff.bezierTo( ofVec3f((TILES*TILESIZE*12),0,0), ofVec3f(0,0,0), ofVec3f(0,0,0));
    
    OneLoadPathOn = TwoLoadPathOn;
    OneLoadPathOff = TwoLoadPathOff;
    ThreeLoadPathOn = TwoLoadPathOn;
    ThreeLoadPathOff =TwoLoadPathOff;
    
    for (int i = 0; i < TwoLoadPathOn.size() ; i++) {
        OneLoadPathOn.getVertices().at(i) = TwoLoadPathOn.getVertices().at(i)+synthPos[0].getPosition();
        OneLoadPathOff.getVertices().at(i) = TwoLoadPathOff.getVertices().at(i)+synthPos[0].getPosition();
        ThreeLoadPathOn.getVertices().at(i) = TwoLoadPathOn.getVertices().at(i)+synthPos[2].getPosition();
        ThreeLoadPathOff.getVertices().at(i) = TwoLoadPathOff.getVertices().at(i)+synthPos[2].getPosition();
    }
    
    
    //___---___
    //---___---
    
    //settings menu
    TwoSettingsPathOn.addVertex(ofVec3f(0,0,0));
    TwoSettingsPathOn.bezierTo(ofVec3f(0,0,0), ofVec3f((-TILES*TILESIZE*12),0,0), ofVec3f((-TILES*TILESIZE*12),0,0));
    
    TwoSettingsPathOff.addVertex( ofVec3f((-TILES*TILESIZE*12),0,0));
    TwoSettingsPathOff.bezierTo( ofVec3f((-TILES*TILESIZE*12),0,0), ofVec3f(0,0,0), ofVec3f(0,0,0));
    
    OneSettingsPathOn = TwoSettingsPathOn;
    OneSettingsPathOff = TwoSettingsPathOff;
    ThreeSettingsPathOn = TwoSettingsPathOn;
    ThreeSettingsPathOff =TwoSettingsPathOff;
    
    for (int i = 0; i < TwoSettingsPathOn.size() ; i++) {
        OneSettingsPathOn.getVertices().at(i) = TwoSettingsPathOn.getVertices().at(i)+synthPos[0].getPosition();
        OneSettingsPathOff.getVertices().at(i) = TwoSettingsPathOff.getVertices().at(i)+synthPos[0].getPosition();
        ThreeSettingsPathOn.getVertices().at(i) = TwoSettingsPathOn.getVertices().at(i)+synthPos[2].getPosition();
        ThreeSettingsPathOff.getVertices().at(i) = TwoSettingsPathOff.getVertices().at(i)+synthPos[2].getPosition();
    }
    
    
}

//--------------------------------------------------------------

void ofApp::setupGlobalInterface() {
    notes = ofSplitString("C C# D D# E F F# G G# A A# B"," ");
    for (int i = 0 ; i < 12; i++) {
        pianoKeys[i] = 0;
    }
    pianoKeys[1] = 1;
    pianoKeys[3] = 1;
    
    pianoKeys[6] = 1;
    pianoKeys[8] = 1;
    pianoKeys[10] = 1;
    
    //---------------------------------------------------------------------------
    
    float fontDefault = fontSizeDefault;
    float fontSmall = fontSizeSmall;
    float fontBig = fontSizeBigger;
    
    float farOff = 10.0;
    float lowOff = 6.0;
    
    ofVec3f smallButton = ofVec3f(designGrid[0][0].y,designGrid[0][0].y,0);
    ofVec3f horizontalSlider = ofVec3f(designGrid[0][0].x*2,designGrid[0][0].y,0);
    ofVec3f verticalSlider = ofVec3f(designGrid[0][0].x*2/12,designGrid[0][0].y*2,0);
    
    ofVec3f gridRect = ofVec3f(designGrid[0][0].x*2,designGrid[0][0].y*2,0);
    float hSliderYscale = 0.25;
    float hSLiderHandler = 0.5;
    
    //---------------------------------------------------------------------------
    
    //volume slider ,STATE_VOLUME
    ofVec3f place = ofVec3f(0,+gridRect.y*hSliderYscale/2,0);
    ofVec3f offPlace = ofVec3f(0,-designGrid[0][0].y*farOff,0);
    GlobalGUI temp = GlobalGUI(0,string(""),ofVec3f( (gridRect.x*0.888*2)+gridRect.x,gridRect.y*hSLiderHandler,0), ofColor(50,0,0),place,offPlace,fontDefault,true,&tekoSemibold);
    mainInterfaceData.push_back(temp);
    
    //volume A B C, STATE_VOLUME
    place = ofVec3f(0,-designGrid[0][0].y/4,0);
    offPlace = ofVec3f(0,-designGrid[0][0].y*lowOff,0);
    temp = GlobalGUI(1,string(""),ofVec3f(gridRect.x*0.777, gridRect.y*hSLiderHandler,0),ofColor(51,0,0),place,offPlace,fontDefault,true,&tekoSemibold);
    mainInterfaceData.push_back(temp);
    temp = GlobalGUI(2,string(""),ofVec3f(gridRect.x*0.777, gridRect.y*hSLiderHandler,0),ofColor(52,0,0),place,offPlace,fontDefault,true,&tekoSemibold);
    mainInterfaceData.push_back(temp);
    temp = GlobalGUI(3,string(""),ofVec3f(gridRect.x*0.777, gridRect.y*hSLiderHandler,0),ofColor(53,0,0),place,offPlace,fontDefault,true,&tekoSemibold);
    mainInterfaceData.push_back(temp);
    
    //slot a octave/note, STATE_HARMONY
    place = ofVec3f(0,(designGrid[0][0].y)+(fontSmall),0);
    offPlace = ofVec3f(0,designGrid[0][0].y*lowOff,0);
    temp = GlobalGUI(4,string(""),ofVec3f(smallButton.x,smallButton.y,0),ofColor(54,0,0),place,offPlace,fontSmall,true,&tekoBold);
    mainInterfaceData.push_back(temp);
    
    //Keynote slider background ->#49 slider, STATE_EDIT_DETAIL
    place = ofVec3f(0,gridRect.y*hSliderYscale/2,0);
    offPlace = ofVec3f(0 ,-designGrid[0][0].y*farOff,0);
    temp = GlobalGUI(5,string(""), ofVec3f( (gridRect.x*0.888*2)+gridRect.x, gridRect.y*hSliderYscale,0),ofColor(55,0,0),place,offPlace,fontDefault,false,&tekoLight);
    mainInterfaceData.push_back(temp);
    
    //slot b octave/note , STATE_HARMONY
    place = ofVec3f(0,(designGrid[0][0].y)+(fontSmall),0);
    offPlace = ofVec3f(0,designGrid[0][0].y*lowOff,0);
    temp = GlobalGUI(6,string(""),ofVec3f(smallButton.x,smallButton.y,0),ofColor(56,0,0),place,offPlace,fontSmall,true,&tekoBold);
    mainInterfaceData.push_back(temp);
    
    //synth preset, STATE_DETAIL
    offPlace = ofVec3f(-designGrid[0][0].x*6,0,0);
    place = ofVec3f(0,designGrid[0][0].y-(gridRect.y*hSliderYscale*0.5),0);
    temp = GlobalGUI(7,"INSTRUMENT",ofVec3f(gridRect.x*0.75,gridRect.y*hSliderYscale,0),ofColor(57,0,0),place,offPlace,fontDefault,false,&tekoBold);
    mainInterfaceData.push_back(temp);
    
    //PAUSE A B C, STATE_DEFAULT
    place = ofVec3f(0,-designGrid[0][0].y/2,0);
    offPlace = ofVec3f(0,+designGrid[0][0].y*lowOff,0);
    temp = GlobalGUI(8,string(""),smallButton*0.5,ofColor(59,0,0),place,offPlace,fontBig*0.85,true,&micon);
    mainInterfaceData.push_back(temp);
    offPlace = ofVec3f(0,+designGrid[0][0].y*lowOff,0);
    temp = GlobalGUI(9,string(""),smallButton*0.5,ofColor(60,0,0),place,offPlace,fontBig*0.85,true,&micon);
    mainInterfaceData.push_back(temp);
    offPlace = ofVec3f(0,+designGrid[0][0].y*lowOff,0);
    temp = GlobalGUI(10,string(""),smallButton*0.5,ofColor(61,0,0),place,offPlace,fontBig*0.85,true,&micon);
    mainInterfaceData.push_back(temp);
    
    //toggle detail off, STATE_EDIT
    place = ofVec3f(0,designGrid[0][0].y/2,0);
    offPlace = ofVec3f(0,designGrid[0][0].y*lowOff,0);
    temp = GlobalGUI(11,string("\uE3C8"),smallButton,ofColor(62,0,0),place,offPlace,fontBig,true,&micon);
    mainInterfaceData.push_back(temp);
    
    //save to presets button, STATE_EDIT
    place = ofVec3f(0,-designGrid[0][0].y/2,0);
    offPlace = ofVec3f(0,-designGrid[0][0].y*farOff,0);
    temp = GlobalGUI(12,string("SAVE GRID"),smallButton,ofColor(63,0,0),place,offPlace,fontDefault,true,&tekoSemibold);
    mainInterfaceData.push_back(temp);
    
    //scale, STATE_EDIT_DETAIL
    offPlace = ofVec3f(0,-designGrid[0][0].y*lowOff,0);
    float offset = ((gridRect.x*0.888*2)+gridRect.x)/12;
    ofVec3f scaleButton = ofVec3f(offset,gridRect.y*hSliderYscale,0);
    
    place = ofVec3f( -(offset*6)+(scaleButton.x/2),-designGrid[0][0].y,0);
    temp = GlobalGUI(13,notes[0],scaleButton,ofColor(0,0,0),place,offPlace,fontDefault,false,&tekoBold);
    mainInterfaceData.push_back(temp);
    
    for (int i = 1; i < 12; i++) {
        place = ofVec3f( -(offset*6)+(offset*i)+(scaleButton.x/2),-designGrid[0][0].y,0);
        temp = GlobalGUI(13+i,notes[i],scaleButton,ofColor(0,0,0),place,offPlace,fontDefault,false,&tekoRegular);
        mainInterfaceData.push_back(temp);
    }
    
    //keynote; STATE_EDIT_DETAIL
    offPlace = ofVec3f(0,-designGrid[0][0].y*lowOff,0);
    ofVec3f keyButton = ofVec3f(offset,designGrid[0][0].y/32,0);
    
    for (int i = 0; i < 12; i++) {
        place = ofVec3f( -(offset*6)+(offset*i)+(scaleButton.x/2),-(designGrid[0][0].y)+(scaleButton.y/2)+(keyButton.y/2)+2.0,0);
        temp = GlobalGUI(25+i,notes[i],keyButton,ofColor(0,0,0),place, offPlace, fontSmall,false,&tekoBold);
        mainInterfaceData.push_back(temp);
    }
    
    //TOGLE VOLUME, STATE_DEFAULT
    place = ofVec3f(0,-designGrid[0][0].y/2,0);
    offPlace = ofVec3f(0,-designGrid[0][0].y*farOff,0);
    temp = GlobalGUI(37, string("VOLUME"), smallButton, ofColor(23,23,23), place, offPlace,fontDefault,true,&tekoSemibold);
    mainInterfaceData.push_back(temp);
    
    //bpm slider background, STATE_BPM
    place = ofVec3f(0,+gridRect.y*hSliderYscale/2,0);
    offPlace = ofVec3f(0,-designGrid[0][0].y*farOff,0);
    temp = GlobalGUI(38,"", ofVec3f( (gridRect.x*0.888*2)+gridRect.x,gridRect.y*hSliderYscale,0), ofColor(23,23,23), place, offPlace,fontBig,false,&tekoSemibold);
    mainInterfaceData.push_back(temp);
    
    // muster container STATE_EDIT & STATE_EDIT_DETAIL
    offPlace = ofVec3f(+designGrid[0][0].x*6,0,0);
    place = ofVec3f(0,0,0);
    temp = GlobalGUI(39,string("Container"),ofVec3f( gridRect.x*0.777,designGrid[0][0].y*2,0),ofColor(55,0,0),place,offPlace,fontDefault,true,&tekoLight);
    mainInterfaceData.push_back(temp);
    
    //settings button , STATE_DEFAULT
    place = ofVec3f(designGrid[0][0].x,designGrid[0][0].y/2,0);
    offPlace = ofVec3f(0,designGrid[0][0].y*farOff,0);
    temp = GlobalGUI(40,string("\uE8B8"), smallButton ,ofColor(63,0,0),place,offPlace,fontBig*0.8,true,&micon);
    mainInterfaceData.push_back(temp);
    
    //toggle bpm icon, STATE_DEFFAULT
    place = ofVec3f(0,-designGrid[0][0].y/2,0);
    offPlace = ofVec3f(0,-designGrid[0][0].y*farOff,0);
    temp = GlobalGUI(41, string("TEMPO"), smallButton, ofColor(23,23,23), place, offPlace,fontDefault,true,&tekoSemibold);
    mainInterfaceData.push_back(temp);
    
    //toggle harmony icon, STATE_DEFAULT
    place = ofVec3f(0,-designGrid[0][0].y/2,0);
    offPlace = ofVec3f(0,-designGrid[0][0].y*farOff,0);
    temp = GlobalGUI(42, string("HARMONY"), smallButton, ofColor(23,23,23), place, offPlace,fontDefault,true,&tekoSemibold);
    mainInterfaceData.push_back(temp);
    
    //toogle from state_edit to state_default
    place = ofVec3f(0,designGrid[0][0].y/2,0);
    offPlace = ofVec3f(0,designGrid[0][0].y*farOff,0);
    temp = GlobalGUI(43, string("\uE3C8"), smallButton, ofColor(23,23,23), place, offPlace,fontBig,true,&micon);
    mainInterfaceData.push_back(temp);
    
    //toggle edit detail, STATE_EDIT
    place = ofVec3f(0,-designGrid[0][0].y/2,0);
    offPlace = ofVec3f(0,-designGrid[0][0].y*farOff,0);
    temp = GlobalGUI(44, string("SET NOTES"), smallButton, ofColor(23,23,23), place, offPlace,fontDefault,true,&tekoSemibold);
    mainInterfaceData.push_back(temp);
    
    //bpm slider, STATE_BPM
    place = ofVec3f(0,gridRect.y*hSliderYscale/2,0);
    offPlace = ofVec3f(0,-designGrid[0][0].y*farOff,0);
    temp = GlobalGUI(45, string(""), ofVec3f((gridRect.x*0.888*2)+gridRect.x, gridRect.y*hSLiderHandler,0), ofColor(23,23,23), place, offPlace,fontDefault,true,&tekoLight);
    mainInterfaceData.push_back(temp);
    
    // load save button STATE_DEFAULT
    place = ofVec3f(designGrid[0][0].x,designGrid[0][0].y/2,0);
    offPlace = ofVec3f(0,designGrid[0][0].y*farOff,0);
    temp = GlobalGUI(46, string("\uE2C8"), smallButton, ofColor(23,23,23), place, offPlace,fontBig*0.8,true,&micon);
    mainInterfaceData.push_back(temp);
    
    // back to default, STATE_SAVE
    place = ofVec3f(designGrid[0][0].x/2,designGrid[0][0].y/2,0);
    offPlace = ofVec3f(0,designGrid[0][0].y*farOff,0);
    temp = GlobalGUI(47, string("\uE3C8"), smallButton, ofColor(23,23,23), place, offPlace,fontBig,true,&micon);
    mainInterfaceData.push_back(temp);
    
    // animation data for load save grid, STATE_SAVE
    place = ofVec3f(-designGrid[0][0].x,0,0);
    offPlace = ofVec3f(-designGrid[0][0].x*6 ,0,0);
    temp = GlobalGUI(48, string(""), smallButton, ofColor(23,23,23), place, offPlace,fontDefault,true,&tekoLight);
    mainInterfaceData.push_back(temp);
    
    //keynote slider, STATE_EDIT_DETAIL
    place = ofVec3f(0,gridRect.y*hSliderYscale/2,0);
    offPlace = ofVec3f(0 ,-designGrid[0][0].y*farOff,0);
    temp = GlobalGUI(49, string(""), ofVec3f( (gridRect.x*0.888*2)+gridRect.x,gridRect.y*hSLiderHandler,0), ofColor(23,23,23), place, offPlace,fontDefault,true,&tekoLight);
    mainInterfaceData.push_back(temp);
    
    //return to load grid, STATE_SAVE
    place = ofVec3f( 0,designGrid[0][0].y/2,0);
    offPlace = ofVec3f(0,+designGrid[0][0].y*farOff,0);
    temp = GlobalGUI(50, string("\uE3C8"), smallButton,ofColor(0,0,0),place,offPlace,fontBig,true,&micon);
    mainInterfaceData.push_back(temp);
    
    
    //volume slider, background, global and A B C , STATE_VOLUME
    
    place = ofVec3f(0,+gridRect.y*hSliderYscale/2,0);
    offPlace = ofVec3f(0,-designGrid[0][0].y*farOff,0);
    temp = GlobalGUI(51,"",ofVec3f( (gridRect.x*0.888*2)+gridRect.x, gridRect.y*hSliderYscale,0),ofColor(51,0,0),place,offPlace,fontBig,false,&tekoSemibold);
    mainInterfaceData.push_back(temp);
    
    
    place = ofVec3f(0,-designGrid[0][0].y/4,0);
    offPlace = ofVec3f(0,-designGrid[0][0].y*lowOff,0);
    temp = GlobalGUI(52,"",ofVec3f(gridRect.x*0.777, gridRect.y*hSliderYscale,0),ofColor(51,0,0),place,offPlace,fontBig,false,&tekoSemibold);
    mainInterfaceData.push_back(temp);
    // mainInterfaceData[51].elementColorOn = ofColor(255,255,255,100);
    
    temp = GlobalGUI(53,"",ofVec3f(gridRect.x*0.777, gridRect.y*hSliderYscale,0),ofColor(52,0,0),place,offPlace,fontBig,false,&tekoSemibold);
    mainInterfaceData.push_back(temp);
    // mainInterfaceData[52].elementColorOn = ofColor(255,255,255,100);
    
    temp = GlobalGUI(54,"",ofVec3f(gridRect.x*0.777, gridRect.y*hSliderYscale,0),ofColor(53,0,0),place,offPlace,fontBig,false,&tekoSemibold);
    mainInterfaceData.push_back(temp);
    // mainInterfaceData[53].elementColorOn = ofColor(255,255,255,100);
    
    
    
    //BPM A B C Buttons Background, STATE_BPM
    place = ofVec3f(0,-designGrid[0][0].y,0);
    offPlace = ofVec3f(0,-designGrid[0][0].y*lowOff,0);
    temp = GlobalGUI(55,string("BPM A"),ofVec3f(gridRect.x*0.777, gridRect.y*hSliderYscale,0),ofColor(51,0,0),place,offPlace,fontDefault,true,&tekoLight);
    mainInterfaceData.push_back(temp);
    temp = GlobalGUI(56,string("BPM B"),ofVec3f(gridRect.x*0.777, gridRect.y*hSliderYscale,0),ofColor(52,0,0),place,offPlace,fontDefault,true,&tekoLight);
    mainInterfaceData.push_back(temp);
    temp = GlobalGUI(57,string("BPM C"),ofVec3f(gridRect.x*0.777, gridRect.y*hSliderYscale,0),ofColor(53,0,0),place,offPlace,fontDefault,true,&tekoLight);
    mainInterfaceData.push_back(temp);
    
    //harmony settings, a,b,c, keynote STATE_HARMONY
    offPlace = ofVec3f(0,designGrid[0][0].y*lowOff,0);
    place = ofVec3f(0,designGrid[0][0].y*0.75,0);
    temp = GlobalGUI(58,ofToString(synths[synthButton[0]].keyNote),ofVec3f(gridRect.x*0.75, gridRect.y*hSliderYscale,0),ofColor(57,0,0),place,offPlace,fontDefault,false,&tekoBold);
    mainInterfaceData.push_back(temp);
    
    offPlace = ofVec3f(0,designGrid[0][0].y*lowOff,0);
    place = ofVec3f(0,designGrid[0][0].y*0.75,0);
    temp = GlobalGUI(59,ofToString(synths[synthButton[1]].keyNote),ofVec3f(gridRect.x*0.75, gridRect.y*hSliderYscale,0),ofColor(57,0,0),place,offPlace,fontDefault,false,&tekoBold);
    mainInterfaceData.push_back(temp);
    
    offPlace = ofVec3f(0,designGrid[0][0].y*lowOff,0);
    place = ofVec3f(0,designGrid[0][0].y*0.75,0);
    temp = GlobalGUI(60,ofToString(synths[synthButton[2]].keyNote),ofVec3f(gridRect.x*0.75, gridRect.y*hSliderYscale,0),ofColor(57,0,0),place,offPlace,fontDefault,false,&tekoBold);
    mainInterfaceData.push_back(temp);
    
    //harmony menu,  global keynote, STATE_HARMONY
    offPlace = ofVec3f(0,+designGrid[0][0].y*farOff,0);
    place = ofVec3f(designGrid[0][0].x,-designGrid[0][0].y*0.25,0);
    temp = GlobalGUI(61,ofToString(notes[globalKey%12]),ofVec3f(gridRect.x*0.75, gridRect.y*hSliderYscale,0),ofColor(255,255,255),place,offPlace,fontDefault,false,&tekoBold);
    mainInterfaceData.push_back(temp);
    
    //harmony menu -> global scale, STATE_HARMONY
    offPlace = ofVec3f(0,+designGrid[0][0].y*farOff,0);
    place = ofVec3f(designGrid[0][0].x,-designGrid[0][0].y*0.25,0);
    temp = GlobalGUI(62,scaleCollection.scaleVec.at(globalScaleVecPos%scaleCollection.scaleVec.size()).name,ofVec3f(gridRect.x*0.75, gridRect.y*hSliderYscale,0),ofColor(255,255,255),place,offPlace,fontDefault,false,&tekoBold);
    mainInterfaceData.push_back(temp);
    
    
    //active scale notes display, STATE_EDIT
    for (int i= 0; i < 12; i++) {
        float offset =  (horizontalSlider.x*HARMONY_ROWS_SCALE)/12 ;
        offPlace = ofVec3f(-designGrid[0][0].x*6- ((offset*i) + (offset/2) ),0,0);
        place = ofVec3f( (-offset*6) + (offset*i) + (offset/2) ,-designGrid[0][0].y*0.75,0);
        temp = GlobalGUI(63+i,"o",ofVec3f(offset*0.5,gridRect.y*hSliderYscale,0),ofColor(57*i,0,0),place,offPlace,fontDefault,false,&tekoLight);
        mainInterfaceData.push_back(temp);
    }
    
    //harmony menu scale note display, STATE_HARMONY
    offset =  (horizontalSlider.x*HARMONY_ROWS_SCALE)/12 ;
    
    for (int i= 0; i < 12; i++) {
        offPlace = ofVec3f(0,-designGrid[0][0].y*lowOff,0);
        place = ofVec3f( (-offset*6) + (offset*i) + (offset/2) ,(designGrid[0][0].y*0.75),0);
        temp = GlobalGUI(75+i,"o",ofVec3f(offset*0.5,gridRect.y*hSliderYscale,0),ofColor(57*i,0,0),place,offPlace,fontDefault,false,&tekoLight);
        mainInterfaceData.push_back(temp);
    }
    
    for (int i= 0; i < 12; i++) {
        offPlace = ofVec3f(0,-designGrid[0][0].y*lowOff,0);
        place = ofVec3f( (-offset*6) + (offset*i) + (offset/2),(designGrid[0][0].y*0.75),0);
        temp = GlobalGUI(87+i,"o",ofVec3f(offset*0.5,gridRect.y*hSliderYscale,0),ofColor(57*i,0,0),place,offPlace,fontDefault,false,&tekoLight);
        mainInterfaceData.push_back(temp);
    }
    
    for (int i= 0; i < 12; i++) {
        offPlace = ofVec3f(0,-designGrid[0][0].y*lowOff,0);
        place = ofVec3f( (-offset*6) + (offset*i) + (offset/2),(designGrid[0][0].y*0.75),0);
        temp = GlobalGUI(99+i,"o",ofVec3f(offset*0.5,gridRect.y*hSliderYscale,0),ofColor(57*i,0,0),place,offPlace,fontDefault,false,&tekoLight);
        mainInterfaceData.push_back(temp);
    }
    
    //toggle global harmony on synth, STATE_EDIT
    place = ofVec3f(0,-designGrid[0][0].y/2,0);
    offPlace = ofVec3f(0,-designGrid[0][0].y*farOff,0);
    temp = GlobalGUI(111, string("LOCAL HARMONY"), smallButton, ofColor(23,23,23), place, offPlace,fontDefault,true,&tekoSemibold);
    mainInterfaceData.push_back(temp);
    
    
    
    offset = (gridRect.x*0.777)/4;
    //bpm factor selector buttons
    for (int i = 0; i<4; i++) {
        place = ofVec3f( (-offset*2)+(offset*i)+(offset/2),-designGrid[0][0].y,0);
        offPlace = ofVec3f(0,-designGrid[0][0].y*lowOff,0);
        temp = GlobalGUI(112+i,string("x"+ofToString(i+1)),ofVec3f(offset, gridRect.y*hSliderYscale ,0),ofColor(51,0,0),place,offPlace,fontDefault,false,&tekoBold);
        mainInterfaceData.push_back(temp);
    }
    
    for (int i = 0; i<4; i++) {
        place = ofVec3f( (-offset*2)+(offset*i)+(offset/2),-designGrid[0][0].y,0);
        offPlace = ofVec3f(0,-designGrid[0][0].y*lowOff,0);
        temp = GlobalGUI(116+i,string("x"+ofToString(i+1)),ofVec3f(offset, gridRect.y*hSliderYscale ,0),ofColor(51,0,0),place,offPlace,fontDefault,false,&tekoBold);
        mainInterfaceData.push_back(temp);
    }
    
    for (int i = 0; i<4; i++) {
        place = ofVec3f( (-offset*2)+(offset*i)+(offset/2),-designGrid[0][0].y,0);
        offPlace = ofVec3f(0,-designGrid[0][0].y*lowOff,0);
        temp = GlobalGUI(120+i,string("x"+ofToString(i+1)),ofVec3f(offset, gridRect.y*hSliderYscale ,0),ofColor(51,0,0),place,offPlace,fontDefault,false,&tekoBold);
        mainInterfaceData.push_back(temp);
    }
    
    //delete save STATE_SAVE
    offPlace = ofVec3f(-designGrid[0][0].y*6,0,0);
    place = ofVec3f(0,designGrid[0][0].y/2,0);
    temp = GlobalGUI(124,"DELETE",smallButton,ofColor(57,0,0),place,offPlace,fontDefault,true,&tekoSemibold);
    mainInterfaceData.push_back(temp);
    
    //save button, STATE_SAVE
    offPlace = ofVec3f(0,-designGrid[0][0].y*farOff,0);
    place = ofVec3f(designGrid[0][0].x/2,-designGrid[0][0].y/2,0);
    temp = GlobalGUI(125,"SAVE",smallButton,ofColor(57,0,0),place,offPlace,fontDefault,true,&tekoSemibold);
    mainInterfaceData.push_back(temp);
    
    //load save, STATE_SAVE
    offPlace = ofVec3f(designGrid[0][0].x*6,0,0);
    place = ofVec3f(0,designGrid[0][0].y/2,0);
    temp = GlobalGUI(126,"LOAD",smallButton,ofColor(57,0,0),place,offPlace,fontDefault,true,&tekoSemibold);
    mainInterfaceData.push_back(temp);
    
    //slot c octave/note , STATE_HARMONY
    place = ofVec3f(0,(designGrid[0][0].y)+(fontSmall),0);
    offPlace = ofVec3f(0,designGrid[0][0].y*lowOff,0);
    temp = GlobalGUI(127,string(""),ofVec3f(smallButton.x,smallButton.y,0),ofColor(56,0,0),place,offPlace,fontSmall,true,&tekoBold);
    mainInterfaceData.push_back(temp);
    
    //string global key , STATE_HARMONY
    place = ofVec3f(+designGrid[0][0].x,fontSmall,0);
    offPlace = ofVec3f(0,+designGrid[0][0].y*farOff,0);
    temp = GlobalGUI(128,string("GLOBAL KEY"),ofVec3f(smallButton.x,smallButton.y,0),ofColor(55,0,0),place,offPlace,fontSmall,true,&tekoBold);
    mainInterfaceData.push_back(temp);
    
    //string globa scale , STATE_HARMONY
    place = ofVec3f(+designGrid[0][0].x,fontSmall,0);
    offPlace = ofVec3f(0,+designGrid[0][0].y*farOff,0);
    temp = GlobalGUI(129,string("GLOBAL SCALE"),ofVec3f(smallButton.x,smallButton.y,0),ofColor(55,0,0),place,offPlace,fontSmall,true,&tekoBold);
    mainInterfaceData.push_back(temp);
    
    //midi toggle button, STATE_SETTINGS
    place = ofVec3f(0,designGrid[0][0].y/4,0);
    offPlace = ofVec3f(designGrid[0][0].x*farOff,0,0);
    temp = GlobalGUI(130,string("MIDI ON"),smallButton,ofColor(63,0,0),place,offPlace,fontDefault,true,&tekoSemibold);
    mainInterfaceData.push_back(temp);
    
    //global info String, all
    offPlace = ofVec3f(0,-designGrid[0][0].y*farOff,0);
    place = ofVec3f(0,-designGrid[0][0].y/2,0);
    temp = GlobalGUI(131,"MENU",smallButton,ofColor(57,0,0),place,offPlace,fontDefault,true,&tekoSemibold);
    mainInterfaceData.push_back(temp);
    
    //scale step button, STATE_HARMONY
    for(int i = 0; i < 3; i++){
        offPlace = ofVec3f(0,-designGrid[0][0].y*lowOff,0);
        place = ofVec3f(0,gridRect.y*hSliderYscale,0);
        temp = GlobalGUI(132+i,"",ofVec3f(designGrid[0][0].x*2, gridRect.y*hSliderYscale*2,0),ofColor(57,0,0),place,offPlace,fontDefault,true,&tekoSemibold);
        mainInterfaceData.push_back(temp);
    }
    
    //scale step button, STATE_EDIT
    offPlace = ofVec3f(-designGrid[0][0].x*6,0,0);
    place = ofVec3f( 0 ,-designGrid[0][0].y*0.75,0);
    temp = GlobalGUI(135,"",ofVec3f(designGrid[0][0].x*2, gridRect.y*hSliderYscale,0),ofColor(57,0,0),place,offPlace,fontDefault,true,&tekoSemibold);
    mainInterfaceData.push_back(temp);
    
    //soft<->hard settings , STATE_EDIT
    place = ofVec3f(0,0,0);
    offPlace = ofVec3f(-designGrid[0][0].x*6,0,0);
    temp = GlobalGUI(136,string("ATTACK"),ofVec3f(gridRect.x*0.777, gridRect.y*hSLiderHandler,0),ofColor(51,0,0),place,offPlace,fontDefault,true,&tekoBold);
    mainInterfaceData.push_back(temp);
    
    //slider background, STATE_EDIT
    place = ofVec3f(0,0,0);
    offPlace = ofVec3f(-designGrid[0][0].x*6,0,0);
    temp = GlobalGUI(137,"",ofVec3f(gridRect.x*0.777, gridRect.y*hSliderYscale,0),ofColor(51,0,0),place,offPlace,fontBig,false,&tekoSemibold);
    mainInterfaceData.push_back(temp);
    
    //velocity toggle , STATE_SETTINGS
    place = ofVec3f(0,designGrid[0][0].y/4,0);
    offPlace = ofVec3f(designGrid[0][0].x*farOff,0,0);
    temp = GlobalGUI(138,string("DYNAMIC VELOCITY"),smallButton,ofColor(63,0,0),place,offPlace,fontDefault,true,&tekoSemibold);
    mainInterfaceData.push_back(temp);
    
    //velo slider, STATE_SETTINGS
    place = ofVec3f(0,designGrid[0][0].y/4,0);
    offPlace = ofVec3f(designGrid[0][0].x*farOff,0,0);
    temp = GlobalGUI(139,string("VELOCITY"),ofVec3f(gridRect.x*0.777, gridRect.y*hSLiderHandler,0),ofColor(51,0,0),place,offPlace,fontDefault,true,&tekoSemibold);
    mainInterfaceData.push_back(temp);
    mainInterfaceData.back().activateDarkerColor();
    
    
    //velo slider background, STATE_SETTINGS
    place = ofVec3f(0,designGrid[0][0].y/4,0);
    offPlace = ofVec3f(designGrid[0][0].x*farOff,0,0);
    temp = GlobalGUI(140,"",ofVec3f(gridRect.x*0.777, gridRect.y*hSliderYscale,0),ofColor(51,0,0),place,offPlace,fontBig,false,&tekoSemibold);
    mainInterfaceData.push_back(temp);
    
    //delay toggle , STATE_SETTINGS
    place = ofVec3f(0,-designGrid[0][0].y*0.75,0);
    offPlace = ofVec3f(designGrid[0][0].x*farOff,0,0);
    temp = GlobalGUI(141,string("DELAY ON"),smallButton,ofColor(63,0,0),place,offPlace,fontDefault,true,&tekoSemibold);
    mainInterfaceData.push_back(temp);
    
    //autodelay toggle , STATE_SETTINGS
    place = ofVec3f(0,-designGrid[0][0].y*0.75,0);
    offPlace = ofVec3f(designGrid[0][0].x*farOff,0,0);
    temp = GlobalGUI(142,string(""),smallButton,ofColor(63,0,0),place,offPlace,fontDefault,true,&tekoSemibold);
    mainInterfaceData.push_back(temp);
    
    //delay slider, STATE_SETTINGS
    place = ofVec3f(0,-designGrid[0][0].y*0.75,0);
    offPlace = ofVec3f(designGrid[0][0].x*farOff,0,0);
    temp = GlobalGUI(143,string("DELAY TIME"),ofVec3f(gridRect.x*0.777, gridRect.y*hSLiderHandler,0),ofColor(51,0,0),place,offPlace,fontDefault,true,&tekoSemibold);
    mainInterfaceData.push_back(temp);
    mainInterfaceData.back().activateDarkerColor();
    
    
    //delay slider background, STATE_SETTINGS
    place = ofVec3f(0,-designGrid[0][0].y*0.75,0);
    offPlace = ofVec3f(designGrid[0][0].x*farOff,0,0);
    temp = GlobalGUI(144,"",ofVec3f(gridRect.x*0.777, gridRect.y*hSliderYscale,0),ofColor(51,0,0),place,offPlace,fontBig,false,&tekoSemibold);
    mainInterfaceData.push_back(temp);
    
    //reverb toggle , STATE_SETTINGS
    place = ofVec3f(0,designGrid[0][0].y*0.25,0);
    offPlace = ofVec3f(designGrid[0][0].x*farOff,0,0);
    temp = GlobalGUI(145,string("REV"),smallButton,ofColor(63,0,0),place,offPlace,fontDefault,true,&tekoSemibold);
    mainInterfaceData.push_back(temp);
    
    //reverb time slider, STATE_SETTINGS
    place = ofVec3f(0,designGrid[0][0].y*0.25,0);
    offPlace = ofVec3f(designGrid[0][0].x*farOff,0,0);
    temp = GlobalGUI(146,string("REVERB TIME"),ofVec3f(gridRect.x*0.777, gridRect.y*hSLiderHandler,0),ofColor(51,0,0),place,offPlace,fontDefault,true,&tekoSemibold);
    mainInterfaceData.push_back(temp);
    mainInterfaceData.back().activateDarkerColor();
    
    //reverb time background, STATE_SETTINGS
    place = ofVec3f(0,designGrid[0][0].y*0.25,0);
    offPlace = ofVec3f(designGrid[0][0].x*farOff,0,0);
    temp = GlobalGUI(147,"",ofVec3f(gridRect.x*0.777, gridRect.y*hSliderYscale,0),ofColor(51,0,0),place,offPlace,fontBig,false,&tekoSemibold);
    mainInterfaceData.push_back(temp);
    
    //reverb size slider, STATE_SETTINGS
    place = ofVec3f(0,designGrid[0][0].y*0.25,0);
    offPlace = ofVec3f(designGrid[0][0].x*farOff,0,0);
    temp = GlobalGUI(148,string("REVERB SIZE"),ofVec3f(gridRect.x*0.777, gridRect.y*hSLiderHandler,0),ofColor(51,0,0),place,offPlace,fontDefault,true,&tekoSemibold);
    mainInterfaceData.push_back(temp);
    mainInterfaceData.back().activateDarkerColor();
    
    //reverb size background, STATE_SETTINGS
    place = ofVec3f(0,designGrid[0][0].y*0.25,0);
    offPlace = ofVec3f(designGrid[0][0].x*farOff,0,0);
    temp = GlobalGUI(149,"",ofVec3f(gridRect.x*0.777, gridRect.y*hSliderYscale,0),ofColor(51,0,0),place,offPlace,fontBig,false,&tekoSemibold);
    mainInterfaceData.push_back(temp);
    
    mainInterface.setMode(OF_PRIMITIVE_TRIANGLES);
    interfaceDraw.clear();
    interfaceDraw.setMode(OF_PRIMITIVE_TRIANGLES);
    
    for (int i = 0; i < mainInterfaceData.size(); i++) {
        
        for (int j = 0; j < 4; j++) {
            mainInterface.addVertex(ofVec3f(-500,-500,0));
            // mainInterfaceFbo.addVertex(ofVec3f(0,0,0));
            mainInterface.addColor(ofColor(0,0,0,0));
            mainInterfaceFbo.addColor(ofColor(50+i,0,0));
        }
        
        for (int j = 0; j < 6; j++) {
            mainInterface.addIndex(mainInterfaceData.at(i).index[j]+(4*i));
        }
        
        mainInterfaceData[i].setStringWidth();
        mainInterfaceData[i].stringHeight = mainInterfaceData[i].fsPtr->getBBox("O", mainInterfaceData[i].fontSize, 0, 0).getHeight()*0.5;
        
    }
    
    //offset for scale key string
    // mainInterfaceData[50].stringHeight-=scaleButton.y/3;
    
    
    
    
    pauseInterfaceOn();
    
    aniPct = 0.0;
}

//--------------------------------------------------------------


void ofApp::editInterfaceOn(){
    
    /*
     mainInterfaceData[5].showString = true;
     mainInterfaceData[5].moveDir = 1;
     mainInterfaceData[5].animation = true;
     */
    
    mainInterfaceData[7].showString = true;
    mainInterfaceData[7].moveDir = 1;
    mainInterfaceData[7].animation = true;
    
    mainInterfaceData[39].showString = false;
    mainInterfaceData[39].moveDir = 1;
    mainInterfaceData[39].animation = true;
    
    /*
     mainInterfaceData[40].showString = true;
     mainInterfaceData[40].moveDir = 1;
     mainInterfaceData[40].animation = true;
     */
    
    mainInterfaceData[12].showString = true;
    mainInterfaceData[12].moveDir = 1;
    mainInterfaceData[12].animation = true;
    
    mainInterfaceData[43].showString = true;
    mainInterfaceData[43].moveDir = 1;
    mainInterfaceData[43].animation = true;
    
    mainInterfaceData[44].showString = true;
    mainInterfaceData[44].moveDir = 1;
    mainInterfaceData[44].animation = true;
    
    mainInterfaceData[111].showString = true;
    mainInterfaceData[111].moveDir = 1;
    mainInterfaceData[111].animation = true;
    
    /*
     for (int i = 45; i < 45+4; i++) {
     mainInterfaceData[i].moveDir = 1;
     mainInterfaceData[i].animation = true;
     }
     */
    
    if (currentState != STATE_EDIT && currentState != STATE_EDIT_DETAIL ) {
        mainInterfaceData[37].moveDir = 0;
        mainInterfaceData[37].animation = true;
    }
    
    for (int i = 0; i < 12; i++) {
        mainInterfaceData[63+i].moveDir = 1;
        mainInterfaceData[63+i].animation = true;
    }
    
    mainInterfaceData[135].moveDir = 1;
    mainInterfaceData[135].animation = true;
    
    mainInterfaceData[136].moveDir = 1;
    mainInterfaceData[136].showString = true;
    mainInterfaceData[136].animation = true;
    
    mainInterfaceData[137].moveDir = 1;
    mainInterfaceData[137].animation = true;
    
    
}

//--------------------------------------------------------------

void ofApp::editInterfaceOff(){
    
    
    mainInterfaceData[7].moveDir = 0;
    mainInterfaceData[7].animation = true;
    
    /*
     mainInterfaceData[5].moveDir = 0;
     mainInterfaceData[5].animation = true;
     */
    
    mainInterfaceData[39].moveDir = 0;
    mainInterfaceData[39].animation = true;
    
    /*
     mainInterfaceData[40].moveDir = 0;
     mainInterfaceData[40].animation = true;
     */
    
    mainInterfaceData[12].moveDir = 0;
    mainInterfaceData[12].animation = true;
    
    mainInterfaceData[43].moveDir = 0;
    mainInterfaceData[43].animation = true;
    
    mainInterfaceData[44].moveDir = 0;
    mainInterfaceData[44].animation = true;
    
    mainInterfaceData[111].moveDir = 0;
    mainInterfaceData[111].animation = true;
    
    /*
     mainInterfaceData[37].showString = false;
     mainInterfaceData[37].moveDir = 1;
     mainInterfaceData[37].animation = true;
     */
    
    /*
     for (int i = 45; i < 45+4; i++) {
     mainInterfaceData[i].moveDir = 0;
     mainInterfaceData[i].animation = true;
     }
     */
    
    for (int i = 0; i < 12; i++) {
        mainInterfaceData[63+i].moveDir = 0;
        mainInterfaceData[63+i].animation = true;
    }
    
    mainInterfaceData[135].moveDir = 0;
    mainInterfaceData[135].animation = true;
    
    mainInterfaceData[136].moveDir = 0;
    mainInterfaceData[136].animation = true;
    
    mainInterfaceData[137].moveDir = 0;
    mainInterfaceData[137].animation = true;
}

//--------------------------------------------------------------

void ofApp::detailEditInterfaceOn() {
    
    mainInterfaceData[49].setSlider(mainInterface, ofMap(synths[activeSynth].keyNote, 12, 95, 0.0, 1.0));
    
    
    
    mainInterfaceData[131].showString = true;
    mainInterfaceData[131].moveDir = 1;
    mainInterfaceData[131].animation = true;
    
    mainInterfaceData[11].showString = true;
    mainInterfaceData[11].moveDir = 1;
    mainInterfaceData[11].animation = true;
    
    /*
     mainInterfaceData[5].showString = true;
     mainInterfaceData[5].moveDir = 1;
     mainInterfaceData[5].animation = true;
     */
    
    mainInterfaceData[49].showString = false;
    mainInterfaceData[49].moveDir = 1;
    mainInterfaceData[49].animation = true;
    
    mainInterfaceData[5].showString = false;
    mainInterfaceData[5].moveDir = 1;
    mainInterfaceData[5].animation = true;
    
    mainInterfaceData[43].moveDir = 0;
    mainInterfaceData[43].animation = true;
    
    mainInterfaceData[44].moveDir = 0;
    mainInterfaceData[44].animation = true;
    
    mainInterfaceData[12].moveDir = 0;
    mainInterfaceData[12].animation = true;
    
    mainInterfaceData[111].moveDir = 0;
    mainInterfaceData[111].animation = true;
    
    for (int i = 0; i < 12; i++) {
        mainInterfaceData[13+i].moveDir = 1;
        mainInterfaceData[13+i].showString = true;
        mainInterfaceData[13+i].animation = true;
        
        
        mainInterfaceData[25+i].showString = true;
        mainInterfaceData[25+i].moveDir = 1;
        mainInterfaceData[25+i].animation = true;
        
    }
    
    /*
     mainInterfaceData[50].moveDir = 1;
     mainInterfaceData[50].showString = true;
     mainInterfaceData[50].animation = true;
     */
}


//--------------------------------------------------------------

void ofApp::detailEditInterfaceOff() {
    
    /*
     mainInterfaceData[5].moveDir = 0;
     mainInterfaceData[5].animation = true;
     */
    
    
    
    mainInterfaceData[11].moveDir = 0;
    mainInterfaceData[11].animation = true;
    
    mainInterfaceData[131].moveDir = 0;
    mainInterfaceData[131].animation = true;
    
    mainInterfaceData[49].moveDir = 0;
    mainInterfaceData[49].animation = true;
    
    
    mainInterfaceData[5].moveDir = 0;
    mainInterfaceData[5].animation = true;
    
    for (int i = 0; i < 12; i++) {
        mainInterfaceData[13+i].moveDir = 0;
        mainInterfaceData[13+i].animation = true;
        
    }
    for (int i = 0; i < 12; i++) {
        
        mainInterfaceData[25+i].animation = true;
        mainInterfaceData[25+i].moveDir = 0;
    }
    
    /*
     mainInterfaceData[50].animation = true;
     mainInterfaceData[50].moveDir = 0;
     */
    
    
    mainInterfaceData[43].showString = true;
    mainInterfaceData[43].moveDir = 1;
    mainInterfaceData[43].animation = true;
    
    mainInterfaceData[44].showString = true;
    mainInterfaceData[44].moveDir = 1;
    mainInterfaceData[44].animation = true;
    
    mainInterfaceData[12].showString = true;
    mainInterfaceData[12].moveDir = 1;
    mainInterfaceData[12].animation = true;
    
    
    mainInterfaceData[111].showString = true;
    mainInterfaceData[111].moveDir = 1;
    mainInterfaceData[111].animation = true;
    
    /*
     mainInterfaceData[39].showString = true;
     mainInterfaceData[39].moveDir = 1;
     mainInterfaceData[39].animation = true;
     */
}

//--------------------------------------------------------------

void ofApp::volumeInterfacOn() {
    mainInterfaceData[0].sliderPct = ofMap( mainVol, 0.0,1.0,-1.0,1.0);
    mainInterfaceData[1].sliderPct = ofMap( synths[synthButton[0]].sVolume, 0.0,1.0,-1.0,1.0);
    mainInterfaceData[2].sliderPct = ofMap( synths[synthButton[1]].sVolume, 0.0,1.0,-1.0,1.0);
    mainInterfaceData[3].sliderPct = ofMap( synths[synthButton[2]].sVolume, 0.0,1.0,-1.0,1.0);
    mainInterfaceData[1].showString = false;
    mainInterfaceData[1].animation = true;
    mainInterfaceData[1].moveDir = 1;
    mainInterfaceData[2].showString = false;
    mainInterfaceData[2].animation = true;
    mainInterfaceData[2].moveDir = 1;
    mainInterfaceData[3].showString = false;
    mainInterfaceData[3].animation = true;
    mainInterfaceData[3].moveDir = 1;
    
    mainInterfaceData[51].showString = false;
    mainInterfaceData[51].animation = true;
    mainInterfaceData[51].moveDir = 1;
    mainInterfaceData[52].showString = false;
    mainInterfaceData[52].animation = true;
    mainInterfaceData[52].moveDir = 1;
    mainInterfaceData[53].showString = false;
    mainInterfaceData[53].animation = true;
    mainInterfaceData[53].moveDir = 1;
    mainInterfaceData[54].showString = false;
    mainInterfaceData[54].animation = true;
    mainInterfaceData[54].moveDir = 1;
    
    /*
     mainInterfaceData[37].showString = true;
     mainInterfaceData[37].animation = true;
     mainInterfaceData[37].moveDir = 0;
     */
    /*
     mainInterfaceData[41].showString = false;
     mainInterfaceData[41].animation = true;
     mainInterfaceData[41].moveDir = 0;
     */
    
    mainInterfaceData[0].showString = false;
    mainInterfaceData[0].moveDir = 1;
    mainInterfaceData[0].animation = true;
    
    mainInterfaceData[43].showString = true;
    mainInterfaceData[43].animation = true;
    mainInterfaceData[43].moveDir = 1;
    
    mainInterfaceData[131].showString = true;
    mainInterfaceData[131].animation = true;
    mainInterfaceData[131].moveDir = 1;
    
    
}

//--------------------------------------------------------------

void ofApp::volumeInterfaceOff() {
    
    mainInterfaceData[1].animation = true;
    mainInterfaceData[1].moveDir = 0;
    mainInterfaceData[2].animation = true;
    mainInterfaceData[2].moveDir = 0;
    mainInterfaceData[3].animation = true;
    mainInterfaceData[3].moveDir = 0;
    
    mainInterfaceData[51].animation = true;
    mainInterfaceData[51].moveDir = 0;
    mainInterfaceData[52].animation = true;
    mainInterfaceData[52].moveDir = 0;
    mainInterfaceData[53].animation = true;
    mainInterfaceData[53].moveDir = 0;
    mainInterfaceData[54].animation = true;
    mainInterfaceData[54].moveDir = 0;
    
    mainInterfaceData[43].showString = true;
    mainInterfaceData[43].animation = true;
    mainInterfaceData[43].moveDir = 0;
    
    mainInterfaceData[0].moveDir = 0;
    mainInterfaceData[0].animation = true;
    
    mainInterfaceData[131].moveDir = 0;
    mainInterfaceData[131].animation = true;
    /*
     mainInterfaceData[37].showString = true;
     mainInterfaceData[37].animation = true;
     mainInterfaceData[37].moveDir = 1;
     
     
     mainInterfaceData[41].showString = true;
     mainInterfaceData[41].animation = true;
     mainInterfaceData[41].moveDir = 1;
     */
}

//--------------------------------------------------------------

void ofApp::pauseInterfaceOn() {
    
    mainInterfaceData[8].showString = true;
    mainInterfaceData[8].moveDir = 1;
    mainInterfaceData[8].animation = true;
    
    mainInterfaceData[9].showString = true;
    mainInterfaceData[9].moveDir = 1;
    mainInterfaceData[9].animation = true;
    
    mainInterfaceData[10].showString = true;
    mainInterfaceData[10].moveDir = 1;
    mainInterfaceData[10].animation = true;
    
    //  if (currentState == !STATE_VOLUME){
    mainInterfaceData[37].showString = true;
    mainInterfaceData[37].moveDir = 1;
    mainInterfaceData[37].animation = true;
    // }
    
    
    
    mainInterfaceData[41].showString = true;
    mainInterfaceData[41].moveDir = 1;
    mainInterfaceData[41].animation = true;
    
    mainInterfaceData[42].showString = true;
    mainInterfaceData[42].moveDir = 1;
    mainInterfaceData[42].animation = true;
    
    
    mainInterfaceData[46].showString = true;
    mainInterfaceData[46].moveDir = 1;
    mainInterfaceData[46].animation = true;
    
    
    mainInterfaceData[40].showString = true;
    mainInterfaceData[40].moveDir = 1;
    mainInterfaceData[40].animation = true;
}


//--------------------------------------------------------------

void ofApp::pauseInterfaceOff() {
    
    mainInterfaceData[8].moveDir = 0;
    mainInterfaceData[8].animation = true;
    
    mainInterfaceData[9].moveDir = 0;
    mainInterfaceData[9].animation = true;
    
    mainInterfaceData[10].moveDir = 0;
    mainInterfaceData[10].animation = true;
    
    
    mainInterfaceData[37].animation = true;
    mainInterfaceData[37].moveDir = 0;
    
    mainInterfaceData[41].moveDir = 0;
    mainInterfaceData[41].animation = true;
    
    mainInterfaceData[42].moveDir = 0;
    mainInterfaceData[42].animation = true;
    
    mainInterfaceData[46].moveDir = 0;
    mainInterfaceData[46].animation = true;
    
    mainInterfaceData[40].moveDir = 0;
    mainInterfaceData[40].animation = true;
    
}

//--------------------------------------------------------------
void ofApp::bpmInterfaceOn() {
    
    mainInterfaceData[43].showString = true;
    mainInterfaceData[43].animation = true;
    mainInterfaceData[43].moveDir = 1;
    
    mainInterfaceData[38].animation = true;
    mainInterfaceData[38].moveDir = 1;
    mainInterfaceData[38].showString = false;
    
    mainInterfaceData[45].sliderPct = ofMap( bpm, 20,BPM_MAX+20,-1.0,1.0);
    mainInterfaceData[45].animation = true;
    mainInterfaceData[45].moveDir = 1;
    mainInterfaceData[45].showString = false;
    
    for (int i = 0; i < 12; i++) {
        mainInterfaceData[112+i].showString = true;
        mainInterfaceData[112+i].animation = true;
        mainInterfaceData[112+i].moveDir = 1;
    }
    
    mainInterfaceData[55].showString = false;
    mainInterfaceData[55].animation = true;
    mainInterfaceData[55].moveDir = 1;
    mainInterfaceData[56].showString = false;
    mainInterfaceData[56].animation = true;
    mainInterfaceData[56].moveDir = 1;
    mainInterfaceData[57].showString = false;
    mainInterfaceData[57].animation = true;
    mainInterfaceData[57].moveDir = 1;
    
    
    mainInterfaceData[131].animation = true;
    mainInterfaceData[131].moveDir = 1;
    mainInterfaceData[131].showString = true;
    
    
    /*
     mainInterfaceData[55].sliderPct =ofMap( (1/(4/float(synths[synthButton[0]].nextPulseDivision))), 0.0,1.0,-1.0,1.0);
     mainInterfaceData[56].sliderPct =ofMap( (1/(4/float(synths[synthButton[1]].nextPulseDivision))), 0.0, 1.0, -1.0, 1.0);
     mainInterfaceData[57].sliderPct =ofMap( (1/(4/float(synths[synthButton[2]].nextPulseDivision))), 0.0, 1.0, -1.0, 1.0);
     */
}


//--------------------------------------------------------------
void ofApp::bpmInterfaceOff(){
    
    mainInterfaceData[43].animation = true;
    mainInterfaceData[43].moveDir = 0;
    
    //   mainInterfaceData[37].animation = true;
    //  mainInterfaceData[37].moveDir = 1;
    
    mainInterfaceData[38].animation = true;
    mainInterfaceData[38].moveDir = 0;
    
    mainInterfaceData[45].animation = true;
    mainInterfaceData[45].moveDir = 0;
    
    
    for (int i = 0; i < 12; i++) {
        mainInterfaceData[112+i].animation = true;
        mainInterfaceData[112+i].moveDir = 0;
    }
    
    
    mainInterfaceData[55].animation = true;
    mainInterfaceData[55].moveDir = 0;
    mainInterfaceData[56].animation = true;
    mainInterfaceData[56].moveDir = 0;
    mainInterfaceData[57].animation = true;
    mainInterfaceData[57].moveDir = 0;
    
    mainInterfaceData[131].animation = true;
    mainInterfaceData[131].moveDir = 0;
}

//--------------------------------------------------------------
void ofApp::harmonyInterfaceOn() {
    
    mainInterfaceData[58].showString = true;
    mainInterfaceData[58].animation = true;
    mainInterfaceData[58].moveDir = 1;
    mainInterfaceData[59].showString = true;
    mainInterfaceData[59].animation = true;
    mainInterfaceData[59].moveDir = 1;
    mainInterfaceData[60].showString = true;
    mainInterfaceData[60].animation = true;
    mainInterfaceData[60].moveDir = 1;
    mainInterfaceData[61].showString = true;
    mainInterfaceData[61].animation = true;
    mainInterfaceData[61].moveDir = 1;
    mainInterfaceData[62].showString = true;
    mainInterfaceData[62].animation = true;
    mainInterfaceData[62].moveDir = 1;
    
    mainInterfaceData[4].showString = true;
    mainInterfaceData[4].animation = true;
    mainInterfaceData[4].moveDir = 1;
    
    mainInterfaceData[6].showString = true;
    mainInterfaceData[6].animation = true;
    mainInterfaceData[6].moveDir = 1;
    
    mainInterfaceData[128].showString = true;
    mainInterfaceData[128].animation = true;
    mainInterfaceData[128].moveDir = 1;
    
    mainInterfaceData[129].showString = true;
    mainInterfaceData[129].animation = true;
    mainInterfaceData[129].moveDir = 1;
    
    mainInterfaceData[127].showString = true;
    mainInterfaceData[127].animation = true;
    mainInterfaceData[127].moveDir = 1;
    
    mainInterfaceData[43].showString = true;
    mainInterfaceData[43].animation = true;
    mainInterfaceData[43].moveDir = 1;
    
    
    mainInterfaceData[131].showString = true;
    mainInterfaceData[131].animation = true;
    mainInterfaceData[131].moveDir = 1;
    
    for (int i = 0; i < 12*3; i++) {
        mainInterfaceData[75+i].moveDir = 1;
        mainInterfaceData[75+i].animation = true;
    }
    
    for (int i = 0; i < 3; i++) {
        mainInterfaceData[132+i].moveDir = 1;
        mainInterfaceData[132+i].animation = true;
    }
    markScaleSteps();
}

//--------------------------------------------------------------
void ofApp::harmonyInterfaceOff() {
    
    mainInterfaceData[58].animation = true;
    mainInterfaceData[58].moveDir = 0;
    mainInterfaceData[59].animation = true;
    mainInterfaceData[59].moveDir = 0;
    mainInterfaceData[60].animation = true;
    mainInterfaceData[60].moveDir = 0;
    mainInterfaceData[61].animation = true;
    mainInterfaceData[61].moveDir = 0;
    mainInterfaceData[62].animation = true;
    mainInterfaceData[62].moveDir = 0;
    
    mainInterfaceData[4].animation = true;
    mainInterfaceData[4].moveDir = 0;
    
    mainInterfaceData[6].animation = true;
    mainInterfaceData[6].moveDir = 0;
    
    mainInterfaceData[127].animation = true;
    mainInterfaceData[127].moveDir = 0;
    
    mainInterfaceData[128].animation = true;
    mainInterfaceData[128].moveDir = 0;
    
    mainInterfaceData[129].animation = true;
    mainInterfaceData[129].moveDir = 0;
    
    mainInterfaceData[43].showString = true;
    mainInterfaceData[43].animation = true;
    mainInterfaceData[43].moveDir = 0;
    
    mainInterfaceData[131].animation = true;
    mainInterfaceData[131].moveDir = 0;
    
    for (int i = 0; i < 12*3; i++) {
        mainInterfaceData[75+i].moveDir = 0;
        mainInterfaceData[75+i].animation = true;
    }
    for (int i = 0; i < 3; i++) {
        mainInterfaceData[132+i].moveDir = 0;
        mainInterfaceData[132+i].animation = true;
    }
}

//--------------------------------------------------------------
void ofApp::loadSaveInterfaceOn(){
    mainInterfaceData[47].showString = true;
    mainInterfaceData[47].animation = true;
    mainInterfaceData[47].moveDir = 1;
    
    mainInterfaceData[48].showString = false;
    mainInterfaceData[48].animation = true;
    mainInterfaceData[48].moveDir = 1;
    
    mainInterfaceData[125].showString = true;
    mainInterfaceData[125].animation = true;
    mainInterfaceData[125].moveDir = 1;
}
//--------------------------------------------------------------

void ofApp::loadSaveInterfaceOff(){
    
    if( mainInterfaceData[47].moveDir != 0){
        mainInterfaceData[47].animation = true;
        mainInterfaceData[47].moveDir = 0;
    }
    
    mainInterfaceData[48].animation = true;
    mainInterfaceData[48].moveDir = 0;
    
    if( mainInterfaceData[125].moveDir != 0){
        mainInterfaceData[125].animation = true;
        mainInterfaceData[125].moveDir = 0;
    }
}

/*
 void ofApp::bothEditInterfaceOff() {
 
 
 mainInterfaceData[7].moveDir = 0;
 mainInterfaceData[7].animation = true;
 
 
 
 mainInterfaceData[4].moveDir = 0;
 mainInterfaceData[4].animation = true;
 
 mainInterfaceData[5].moveDir = 0;
 mainInterfaceData[5].animation = true;
 
 mainInterfaceData[6].moveDir = 0;
 mainInterfaceData[6].animation = true;
 
 
 mainInterfaceData[49].moveDir = 0;
 mainInterfaceData[49].animation = true;
 
 for (int i = 0; i < 12; i++) {
 mainInterfaceData[13+i].moveDir = 0;
 mainInterfaceData[13+i].animation = true;
 
 }
 for (int i = 0; i < 12; i++) {
 
 mainInterfaceData[25+i].animation = true;
 mainInterfaceData[25+i].moveDir = 0;
 }
 
 mainInterfaceData[43].moveDir = 0;
 mainInterfaceData[43].animation = true;
 
 mainInterfaceData[44].moveDir = 0;
 mainInterfaceData[44].animation = true;
 
 for (int i = 45; i < 45+4; i++) {
 mainInterfaceData[i].moveDir = 0;
 mainInterfaceData[i].animation = true;
 }
 
 mainInterfaceData[37].showString = false;
 mainInterfaceData[37].moveDir = 1;
 mainInterfaceData[37].animation = true;
 }
 
 */

//--------------------------------------------------------------

void ofApp::openSlotInterface(){
    aniPct = 0.0;
    interfaceMoving = true;
    
    saveManager.animate = true;
    saveManager.moveDir = 0;
    saveManager.slotDetail = true;
    
    mainInterfaceData[47].showString = true;
    mainInterfaceData[47].animation = true;
    mainInterfaceData[47].moveDir = 0;
    
    mainInterfaceData[125].animation = true;
    mainInterfaceData[125].moveDir = 0;
    
    mainInterfaceData[50].animation = true;
    mainInterfaceData[50].moveDir = 1;
    mainInterfaceData[50].showString = true;
    
    mainInterfaceData[124].showString = true;
    mainInterfaceData[124].animation = true;
    mainInterfaceData[124].moveDir = 1;
    
    mainInterfaceData[126].showString = true;
    mainInterfaceData[126].animation = true;
    mainInterfaceData[126].moveDir = 1;
    
    
}

//--------------------------------------------------------------
void ofApp::closeSlotInterface(){
    aniPct = 0.0;
    interfaceMoving = true;
    
    saveManager.animate = true;
    saveManager.moveDir = 1;
    saveManager.slotDetail = false;
    
    
    mainInterfaceData[50].animation = true;
    mainInterfaceData[50].moveDir = 0;
    
    mainInterfaceData[124].animation = true;
    mainInterfaceData[124].moveDir = 0;
    
    
    mainInterfaceData[126].animation = true;
    mainInterfaceData[126].moveDir = 0;
    
    
    if (currentState != STATE_DEFAULT) {
        mainInterfaceData[47].animation = true;
        mainInterfaceData[47].moveDir = 1;
        mainInterfaceData[47].showString = true;
        
        mainInterfaceData[125].animation = true;
        mainInterfaceData[125].moveDir = 1;
        mainInterfaceData[125].showString = true;
    }
    
}

//--------------------------------------------------------------
void ofApp::openSettingsInterface(){
    mainInterfaceData[139].sliderPct = ofMap( globalVelo, 0.0,1.0,-1.0,1.0);
    mainInterfaceData[143].sliderPct = ofMap( staticDelayValue,0.0,1.0,1.0,-1.0);
    mainInterfaceData[146].sliderPct = ofMap( revTime,0.0,1.0,-1.0,1.0);
    mainInterfaceData[148].sliderPct = ofMap( revSize,0.0,1.0,-1.0,1.0);
    
    
    mainInterfaceData[130].animation = true;
    mainInterfaceData[130].moveDir = 1;
    mainInterfaceData[130].showString = true;
    
    mainInterfaceData[138].animation = true;
    mainInterfaceData[138].moveDir = 1;
    mainInterfaceData[138].showString = true;
    
    mainInterfaceData[139].animation = true;
    mainInterfaceData[139].moveDir = 1;
    mainInterfaceData[139].showString = true;
    
    mainInterfaceData[140].animation = true;
    mainInterfaceData[140].moveDir = 1;
    mainInterfaceData[140].showString = false;
    
    mainInterfaceData[141].showString = true;
    mainInterfaceData[141].animation = true;
    mainInterfaceData[141].moveDir = 1;
    
    mainInterfaceData[142].showString = true;
    mainInterfaceData[142].animation = true;
    mainInterfaceData[142].moveDir = 1;
    
    mainInterfaceData[43].showString = true;
    mainInterfaceData[43].animation = true;
    mainInterfaceData[43].moveDir = 1;
    
    mainInterfaceData[143].showString = true;
    mainInterfaceData[143].animation = true;
    mainInterfaceData[143].moveDir = 1;
    
    mainInterfaceData[144].showString = false;
    mainInterfaceData[144].animation = true;
    mainInterfaceData[144].moveDir = 1;
    
    mainInterfaceData[145].showString = true;
    mainInterfaceData[145].animation = true;
    mainInterfaceData[145].moveDir = 1;
    
    mainInterfaceData[146].showString = true;
    mainInterfaceData[146].animation = true;
    mainInterfaceData[146].moveDir = 1;
    
    mainInterfaceData[147].showString = false;
    mainInterfaceData[147].animation = true;
    mainInterfaceData[147].moveDir = 1;
    
    mainInterfaceData[148].showString = true;
    mainInterfaceData[148].animation = true;
    mainInterfaceData[148].moveDir = 1;
    
    mainInterfaceData[149].showString = false;
    mainInterfaceData[149].animation = true;
    mainInterfaceData[149].moveDir = 1;
}

//--------------------------------------------------------------
void ofApp::closeSettingsInterface(){
    
    mainInterfaceData[130].animation = true;
    mainInterfaceData[130].moveDir = 0;
    
    mainInterfaceData[138].animation = true;
    mainInterfaceData[138].moveDir = 0;
    
    mainInterfaceData[139].animation = true;
    mainInterfaceData[139].moveDir = 0;
    
    mainInterfaceData[140].animation = true;
    mainInterfaceData[140].moveDir = 0;
    
    mainInterfaceData[141].animation = true;
    mainInterfaceData[141].moveDir = 0;
    
    mainInterfaceData[142].animation = true;
    mainInterfaceData[142].moveDir = 0;
    
    mainInterfaceData[43].showString = true;
    mainInterfaceData[43].animation = true;
    mainInterfaceData[43].moveDir = 0;
    
    mainInterfaceData[143].animation = true;
    mainInterfaceData[143].moveDir = 0;
    
    mainInterfaceData[144].animation = true;
    mainInterfaceData[144].moveDir = 0;
    
    mainInterfaceData[145].animation = true;
    mainInterfaceData[145].moveDir = 0;
    
    mainInterfaceData[146].animation = true;
    mainInterfaceData[146].moveDir = 0;
    
    mainInterfaceData[147].animation = true;
    mainInterfaceData[147].moveDir = 0;
    
    mainInterfaceData[148].animation = true;
    mainInterfaceData[148].moveDir = 0;
    
    mainInterfaceData[149].animation = true;
    mainInterfaceData[149].moveDir = 0;
}

//--------------------------------------------------------------
void ofApp::makePresetString() {
    presetNames.push_back("AAA");
    presetNames.push_back("BBB");
    presetNames.push_back("CCC");
    presetNames.push_back("DDD");
    
}

void ofApp::makeDesignGrid() {
    
    ofVec3f third = ofVec2f(sWIDTH/3,sHEIGHT/3);
    ofVec3f center = third/2;
    
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            designGrid[i][j] = ofVec2f(third.x*i+center.x,third.y*j+center.y);
        }
    }
    
    
}


//--------------------------------------------------------------
void ofApp::buttonOnePress(){
    int temp = synthButton[0];
    activeSynth = synthButton[0];
    
    if (synths[synthButton[1]].inFocus) {
        
        synths[synthButton[1]].aniPath = oneToBack;
        synths[synthButton[1]].myTarget = synthPos[0].getOrientationQuat();
        synths[synthButton[1]].myDefault = synthActivePos.getOrientationQuat();
        
        
        
        synths[synthButton[1]].inFocus = false ;
        synths[synthButton[1]].animate = true ;
        
        
        synths[temp].inFocus = true;
        synths[temp].animate = true;
        synths[temp].aniPath = oneToActive;
        synths[temp].myTarget = synthActivePos.getOrientationQuat();
        synths[temp].myDefault = synthPos[0].getOrientationQuat();
        
        synths[synthButton[0]].scaling = true;
        synths[synthButton[1]].scaling = true;
        synths[synthButton[0]].myScaleDefault = 0.5;
        synths[synthButton[1]].myScaleDefault = 1.0;
        synths[synthButton[0]].myScaleTarget = 1.0;
        synths[synthButton[1]].myScaleTarget = 0.5;
        
        
        
        synths[synthButton[0]].pulsePlane.positionModDef = ppPosModDefault;
        synths[synthButton[0]].pulsePlane.positionModTar = ppPosModFocus;
        synths[synthButton[0]].pulsePlane.scaleModDef = ppScaleModDefault;
        synths[synthButton[0]].pulsePlane.scaleModTar = ppScaleModFocus;
        
        synths[synthButton[1]].pulsePlane.positionModDef = ppPosModFocus;
        synths[synthButton[1]].pulsePlane.positionModTar = ppPosModDefault;
        synths[synthButton[1]].pulsePlane.scaleModDef = ppScaleModFocus;
        synths[synthButton[1]].pulsePlane.scaleModTar = ppScaleModDefault;
        
        
        synthButton[0] = synthButton[1];
        synths[synthButton[0]].ownSlot = 0;
        
        synthButton[1] = temp;
        synths[synthButton[1]].ownSlot = 1;
        
        aniPct = 0.0;
        interfaceMoving = true;
        
        /*
         if (
         scaleCollection.scaleVec.at(synths[synthButton[0]].currentScaleVecPos%scaleCollection.scaleVec.size()).name !=
         scaleCollection.scaleVec.at(synths[synthButton[1]].currentScaleVecPos%scaleCollection.scaleVec.size()).name
         )
         {
         mainInterfaceData[5].blinkOn();
         }
         */
        
        if (
            presetNames[synths[synthButton[0]].preset%presetNames.size()] !=
            presetNames[synths[synthButton[1]].preset%presetNames.size()]
            )
        {
            mainInterfaceData[7].blinkOn();
        }
        
        
        setNewGUI();
        
        
    } else {
        synths[temp].inFocus = true;
        synths[temp].aniPath = oneToActive;
        synths[temp].myTarget = synthActivePos.getOrientationQuat();
        synths[temp].myDefault = synthPos[0].getOrientationQuat();
        
        
        synths[temp].animate = true;
        
        synths[synthButton[1]].aniPath = centerToOne;
        synths[synthButton[1]].animate = true;
        synths[synthButton[1]].myTarget =   synthPos[1].getOrientationQuat();
        synths[synthButton[1]].myDefault =   synthPos[1].getOrientationQuat();
        
        synths[synthButton[1]].scaling = true;
        synths[synthButton[2]].scaling = true;
        synths[synthButton[1]].myScaleDefault = 1.0;
        synths[synthButton[2]].myScaleDefault = 1.0;
        synths[synthButton[1]].myScaleTarget = 0.5;
        synths[synthButton[2]].myScaleTarget = 0.5;
        
        
        synths[temp].pulsePlane.positionModDef = ppPosModDefault;
        synths[temp].pulsePlane.positionModTar = ppPosModFocus;
        synths[temp].pulsePlane.scaleModDef = ppScaleModDefault;
        synths[temp].pulsePlane.scaleModTar = ppScaleModFocus;
        
        
        
        synthButton[0] = synthButton[1];
        synths[synthButton[0]].ownSlot = 0;
        synthButton[1] = temp;
        synths[synthButton[1]].ownSlot = 1;
        
        
        
        //camani
        camQuatDefault = camNotActiveSynth.getOrientationQuat();
        camQuatTarget = camActiveSynth.getOrientationQuat();
        camUsePath = camPath;
        camTargetFov = camActiveFov;
        camDefaultFov = camFov;
        animCam = true;
        
        aniPct = 0.0;
        interfaceMoving = true;
        
        
        setNewGUI();
        
        if (currentState == STATE_DEFAULT){
            editInterfaceOn();
            pauseInterfaceOff();
        }
        
        currentState = STATE_EDIT;
    }
}


//--------------------------------------------------------------
void ofApp::buttonTwoPress(){
    int temp = synthButton[1];
    activeSynth = synthButton[1];
    
    
    if (synths[synthButton[1]].inFocus) {
        
        synths[synthButton[1]].aniPath = twoToBack;
        synths[synthButton[1]].myTarget = synthPos[1].getOrientationQuat();
        synths[synthButton[1]].myDefault = synthActivePos.getOrientationQuat();
        
        synths[synthButton[1]].inFocus = false ;
        synths[synthButton[1]].animate = true ;
        
        
        synths[synthButton[0]].scaling = true;
        synths[synthButton[2]].scaling = true;
        synths[synthButton[0]].myScaleDefault = 0.5;
        synths[synthButton[2]].myScaleDefault = 0.5;
        synths[synthButton[0]].myScaleTarget = 1.0;
        synths[synthButton[2]].myScaleTarget = 1.0;
        
        
        synths[synthButton[1]].pulsePlane.positionModDef = ppPosModFocus;
        synths[synthButton[1]].pulsePlane.positionModTar = ppPosModDefault;
        synths[synthButton[1]].pulsePlane.scaleModDef = ppScaleModFocus;
        synths[synthButton[1]].pulsePlane.scaleModTar = ppScaleModDefault;
        
        
        //camani
        camQuatDefault = camActiveSynth.getOrientationQuat();
        camQuatTarget = camNotActiveSynth.getOrientationQuat();
        camUsePath = camPathBack;
        camTargetFov = camFov;
        camDefaultFov = camActiveFov;
        animCam = true;
        
        aniPct = 0.0;
        interfaceMoving = true;
        
        pauseInterfaceOn();
        
        if (currentState == STATE_EDIT) {
            editInterfaceOff();
        }
        
        if (currentState == STATE_EDIT_DETAIL) {
            //  bothEditInterfaceOff();
        }
        
        
        currentState = STATE_DEFAULT;
        
        
    } else{
        synths[temp].inFocus = true;
        synths[temp].aniPath = twoToActive;
        synths[temp].myTarget = synthActivePos.getOrientationQuat();
        synths[temp].myDefault = synthPos[1].getOrientationQuat();
        synths[temp].animate = true;
        
        synths[synthButton[0]].scaling = true;
        synths[synthButton[2]].scaling = true;
        synths[synthButton[0]].myScaleDefault = 1.0;
        synths[synthButton[2]].myScaleDefault = 1.0;
        synths[synthButton[0]].myScaleTarget = 0.5;
        synths[synthButton[2]].myScaleTarget = 0.5;
        
        synths[temp].pulsePlane.positionModDef = ppPosModDefault;
        synths[temp].pulsePlane.positionModTar = ppPosModFocus;
        synths[temp].pulsePlane.scaleModDef = ppScaleModDefault;
        synths[temp].pulsePlane.scaleModTar = ppScaleModFocus;
        
        //cam
        camQuatDefault = camNotActiveSynth.getOrientationQuat();
        camQuatTarget = camActiveSynth.getOrientationQuat();
        camUsePath = camPath;
        camDefaultFov = camFov;
        camTargetFov = camActiveFov;
        animCam = true;
        
        aniPct = 0.0;
        interfaceMoving = true;
        
        setNewGUI();
        
        if (currentState == STATE_DEFAULT){
            editInterfaceOn();
            pauseInterfaceOff();
        }
        currentState = STATE_EDIT;
        
    }
    
}



//--------------------------------------------------------------
void ofApp::buttonThreePress(){
    
    int temp = synthButton[2];
    activeSynth = synthButton[2];
    
    
    if (synths[synthButton[1]].inFocus) {
        
        synths[synthButton[1]].aniPath = threeToBack;
        synths[synthButton[1]].myTarget = synthPos[2].getOrientationQuat();
        synths[synthButton[1]].myDefault = synthActivePos.getOrientationQuat();
        
        synths[synthButton[1]].inFocus = false ;
        synths[synthButton[1]].animate = true ;
        
        
        synths[temp].inFocus = true;
        synths[temp].animate = true;
        synths[ temp ].aniPath = threeToActive;
        synths[temp].myTarget = synthActivePos.getOrientationQuat();
        synths[temp].myDefault = synthPos[2].getOrientationQuat();
        
        synths[synthButton[2]].scaling = true;
        synths[synthButton[1]].scaling = true;
        synths[synthButton[2]].myScaleDefault = 0.45;
        synths[synthButton[1]].myScaleDefault = 1.0;
        synths[synthButton[2]].myScaleTarget = 1.0;
        synths[synthButton[1]].myScaleTarget = 0.45;
        
        
        synths[synthButton[2]].pulsePlane.positionModDef = ppPosModDefault;
        synths[synthButton[2]].pulsePlane.positionModTar = ppPosModFocus;
        synths[synthButton[2]].pulsePlane.scaleModDef = ppScaleModDefault;
        synths[synthButton[2]].pulsePlane.scaleModTar = ppScaleModFocus;
        
        synths[synthButton[1]].pulsePlane.positionModDef = ppPosModFocus;
        synths[synthButton[1]].pulsePlane.positionModTar = ppPosModDefault;
        synths[synthButton[1]].pulsePlane.scaleModDef = ppScaleModFocus;
        synths[synthButton[1]].pulsePlane.scaleModTar = ppScaleModDefault;
        
        synthButton[2] = synthButton[1];
        synths[synthButton[2]].ownSlot = 2;
        
        synthButton[1] = temp;
        synths[synthButton[1]].ownSlot = 1;
        
        aniPct = 0.0;
        interfaceMoving = true;
        
        /*
         if (
         scaleCollection.scaleVec.at(synths[synthButton[1]].currentScaleVecPos%scaleCollection.scaleVec.size()).name !=
         scaleCollection.scaleVec.at(synths[synthButton[2]].currentScaleVecPos%scaleCollection.scaleVec.size()).name
         )
         {
         mainInterfaceData[5].blinkOn();
         }
         */
        if (
            presetNames[synths[synthButton[1]].preset%presetNames.size()] !=
            presetNames[synths[synthButton[2]].preset%presetNames.size()]
            )
        {
            mainInterfaceData[7].blinkOn();
        }
        
        setNewGUI();
    }else {
        synths[temp].inFocus = true;
        synths[temp].aniPath = threeToActive;
        synths[temp].myTarget = synthActivePos.getOrientationQuat();
        synths[temp].myDefault = synthPos[2].getOrientationQuat();
        synths[temp].animate = true;
        
        synths[synthButton[1]].aniPath = centerToThree;
        synths[synthButton[1]].animate = true;
        synths[synthButton[1]].myTarget =   synthPos[1].getOrientationQuat();
        synths[synthButton[1]].myDefault =   synthPos[1].getOrientationQuat();
        
        
        synths[synthButton[0]].scaling = true;
        synths[synthButton[1]].scaling = true;
        synths[synthButton[0]].myScaleDefault = 1.0;
        synths[synthButton[1]].myScaleDefault = 1.0;
        synths[synthButton[0]].myScaleTarget = 0.45;
        synths[synthButton[1]].myScaleTarget = 0.45;
        
        synths[temp].pulsePlane.positionModDef = ppPosModDefault;
        synths[temp].pulsePlane.positionModTar = ppPosModFocus;
        synths[temp].pulsePlane.scaleModDef = ppScaleModDefault;
        synths[temp].pulsePlane.scaleModTar = ppScaleModFocus;
        
        synthButton[2] = synthButton[1];
        synths[synthButton[2]].ownSlot = 2;
        
        synthButton[1] = temp;
        synths[synthButton[1]].ownSlot = 1;
        
        //camera
        camQuatDefault = camNotActiveSynth.getOrientationQuat();
        camQuatTarget = camActiveSynth.getOrientationQuat();
        camUsePath = camPath;
        camDefaultFov = camFov;
        camTargetFov = camActiveFov;
        animCam = true;
        
        aniPct = 0.0;
        interfaceMoving = true;
        
        //  mainInterfaceData[2].setSlider(mainInterface, mainInterfaceData[3].sliderPos);
        // mainInterfaceData[3].setSlider(mainInterface, mainInterfaceData[2].sliderPos);
        
        
        setNewGUI();
        
        if (currentState == STATE_DEFAULT){
            editInterfaceOn();
            pauseInterfaceOff();
        }
        
        currentState = STATE_EDIT;
        
    }
}


//--------------------------------------------------------------

void ofApp::buttonFourPress() {
    
    if(currentState == STATE_DEFAULT ) {
        
        synths[synthButton[0]].aniPath = OneVolumeLayerPathOn;
        synths[synthButton[0]].myDefault = synthPos[0].getOrientationQuat();
        synths[synthButton[0]].myTarget = volumeMatrix.getOrientationQuat();
        synths[synthButton[0]].animate = true ;
        synths[synthButton[1]].aniPath = TwoVolumeLayerPathOn;
        synths[synthButton[1]].myDefault = synthPos[1].getOrientationQuat();
        synths[synthButton[1]].myTarget = volumeMatrix.getOrientationQuat();
        synths[synthButton[1]].animate = true ;
        synths[synthButton[2]].aniPath = ThreeVolumeLayerPathOn;
        synths[synthButton[2]].myDefault = synthPos[2].getOrientationQuat();
        synths[synthButton[2]].myTarget = volumeMatrix.getOrientationQuat();
        synths[synthButton[2]].animate = true ;
        
        
        synths[synthButton[0]].pulsePlane.positionModDef = ppPosModDefault;
        synths[synthButton[0]].pulsePlane.positionModTar = ppPosModDefault;
        synths[synthButton[0]].pulsePlane.scaleModDef = ppScaleModDefault;
        synths[synthButton[0]].pulsePlane.scaleModTar = ppScaleModDefault;
        
        synths[synthButton[1]].pulsePlane.positionModDef = ppPosModDefault;
        synths[synthButton[1]].pulsePlane.positionModTar = ppPosModDefault;
        synths[synthButton[1]].pulsePlane.scaleModDef = ppScaleModDefault;
        synths[synthButton[1]].pulsePlane.scaleModTar = ppScaleModDefault;
        
        synths[synthButton[2]].pulsePlane.positionModDef = ppPosModDefault;
        synths[synthButton[2]].pulsePlane.positionModTar = ppPosModDefault;
        synths[synthButton[2]].pulsePlane.scaleModDef = ppScaleModDefault;
        synths[synthButton[2]].pulsePlane.scaleModTar = ppScaleModDefault;
        
        aniPct = 0.0;
        
        pauseInterfaceOff();
        volumeInterfacOn();
        currentState = STATE_VOLUME;
        
    } else if (currentState == STATE_VOLUME) {
        
        synths[synthButton[0]].aniPath = OneVolumeLayerPathOff;
        synths[synthButton[0]].myTarget = synthPos[0].getOrientationQuat();
        synths[synthButton[0]].myDefault = volumeMatrix.getOrientationQuat();
        synths[synthButton[0]].animate = true ;
        synths[synthButton[1]].aniPath = TwoVolumeLayerPathOff;
        synths[synthButton[1]].myTarget = synthPos[0].getOrientationQuat();
        synths[synthButton[1]].myDefault = volumeMatrix.getOrientationQuat();
        synths[synthButton[1]].animate = true ;
        synths[synthButton[2]].aniPath = ThreeVolumeLayerPathOff;
        synths[synthButton[2]].myTarget = synthPos[0].getOrientationQuat();
        synths[synthButton[2]].myDefault = volumeMatrix.getOrientationQuat();
        synths[synthButton[2]].animate = true ;
        
        synths[synthButton[0]].pulsePlane.positionModDef = ppPosModDefault;
        synths[synthButton[0]].pulsePlane.positionModTar = ppPosModDefault;
        synths[synthButton[0]].pulsePlane.scaleModDef = ppScaleModDefault;
        synths[synthButton[0]].pulsePlane.scaleModTar = ppScaleModDefault;
        
        synths[synthButton[1]].pulsePlane.positionModDef = ppPosModDefault;
        synths[synthButton[1]].pulsePlane.positionModTar = ppPosModDefault;
        synths[synthButton[1]].pulsePlane.scaleModDef = ppScaleModDefault;
        synths[synthButton[1]].pulsePlane.scaleModTar = ppScaleModDefault;
        
        synths[synthButton[2]].pulsePlane.positionModDef = ppPosModDefault;
        synths[synthButton[2]].pulsePlane.positionModTar = ppPosModDefault;
        synths[synthButton[2]].pulsePlane.scaleModDef = ppScaleModDefault;
        synths[synthButton[2]].pulsePlane.scaleModTar = ppScaleModDefault;
        
        aniPct = 0.0;
        
        pauseInterfaceOn();
        volumeInterfaceOff();
        currentState = STATE_DEFAULT;
        
    }
    
    
}


//--------------------------------------------------------------
void ofApp::buttonEditDetail() {
    
    if ( currentState == STATE_EDIT) {
        
        //cam
        camQuatDefault = camActiveSynth.getOrientationQuat();
        camQuatTarget = camActiveSynth.getOrientationQuat();
        camUsePath = camEditToDetail;
        camTargetFov = camEditDetailFov;
        camDefaultFov = camActiveFov;
        animCam = true;
        
        //synth
        synths[synthButton[1]].animate = true ;
        synths[synthButton[1]].aniPath = synths[synthButton[1]].getEmptyPath(synths[synthButton[1]].myNode.getPosition());
        synths[synthButton[1]].myTarget = synthActivePos.getOrientationQuat();
        synths[synthButton[1]].myDefault = synthActivePos.getOrientationQuat();
        synths[synthButton[1]].scaling = true;
        synths[synthButton[1]].myScaleDefault = 1.0;
        synths[synthButton[1]].myScaleTarget = 0.38;
        
        
        synths[synthButton[0]].scaling = true;
        synths[synthButton[0]].myScaleDefault = 0.5;
        synths[synthButton[0]].myScaleTarget = 0.0;
        synths[synthButton[2]].scaling = true;
        synths[synthButton[2]].myScaleDefault = 0.5;
        synths[synthButton[2]].myScaleTarget = 0.0;
        
        synths[synthButton[1]].pulsePlane.positionModDef = ppPosModFocus;
        synths[synthButton[1]].pulsePlane.positionModTar = ppPosModDefault;
        synths[synthButton[1]].pulsePlane.scaleModDef = ppScaleModFocus;
        synths[synthButton[1]].pulsePlane.scaleModTar = ppScaleModDefault;
        
        synths[synthButton[0]].pulsePlane.positionModDef = ppPosModDefault;
        synths[synthButton[0]].pulsePlane.positionModTar = ppPosModDefault;
        synths[synthButton[0]].pulsePlane.scaleModDef = ppScaleModDefault;
        synths[synthButton[0]].pulsePlane.scaleModTar = ppScaleModDefault;
        
        synths[synthButton[2]].pulsePlane.positionModDef = ppPosModDefault;
        synths[synthButton[2]].pulsePlane.positionModTar = ppPosModDefault;
        synths[synthButton[2]].pulsePlane.scaleModDef = ppScaleModDefault;
        synths[synthButton[2]].pulsePlane.scaleModTar = ppScaleModDefault;
        
        
        // mainInterfaceData[5].animationB = true;
        mainInterfaceData[7].animationB = true;
        mainInterfaceData[39].animationB = true;
        mainInterfaceData[12].animationB = true;
        mainInterfaceData[40].animationB = true;
        mainInterfaceData[136].animationB = true;
        mainInterfaceData[137].animationB = true;
        
        for (int i = 0; i < 12; i++) {
            mainInterfaceData[63+i].animationB = true;
        }
        mainInterfaceData[135].animationB = true;
        
        
        aniPct = 0.0;
        interfaceMoving = true;
        editDetailMoveDirection = 0;
        
        
        detailEditInterfaceOn();
        aniPct = 0.0;
        currentState = STATE_EDIT_DETAIL;
    } else if ( currentState  == STATE_EDIT_DETAIL) {
        
        
        //cam
        camQuatDefault = camActiveSynth.getOrientationQuat();
        camQuatTarget = camActiveSynth.getOrientationQuat();
        camUsePath = camDetailToEdit;
        camTargetFov = camActiveFov;
        camDefaultFov = camEditDetailFov;
        animCam = true;
        
        
        //synth
        synths[synthButton[1]].animate = true ;
        synths[synthButton[1]].aniPath = synths[synthButton[1]].getEmptyPath(synths[synthButton[1]].myNode.getPosition());
        synths[synthButton[1]].myTarget = synthActivePos.getOrientationQuat();
        synths[synthButton[1]].myDefault = synthActivePos.getOrientationQuat();
        synths[synthButton[1]].scaling = true;
        synths[synthButton[1]].myScaleDefault = 0.38;
        synths[synthButton[1]].myScaleTarget = 1.0;
        
        synths[synthButton[0]].scaling = true;
        synths[synthButton[0]].myScaleDefault = 0.0;
        synths[synthButton[0]].myScaleTarget = 0.5;
        synths[synthButton[2]].scaling = true;
        synths[synthButton[2]].myScaleDefault = 0.0;
        synths[synthButton[2]].myScaleTarget = 0.5;
        
        
        synths[synthButton[1]].pulsePlane.positionModDef = ppPosModDefault;
        synths[synthButton[1]].pulsePlane.positionModTar = ppPosModFocus;
        synths[synthButton[1]].pulsePlane.scaleModDef = ppScaleModDefault;
        synths[synthButton[1]].pulsePlane.scaleModTar = ppScaleModFocus;
        
        synths[synthButton[0]].pulsePlane.positionModDef = ppPosModDefault;
        synths[synthButton[0]].pulsePlane.positionModTar = ppPosModDefault;
        synths[synthButton[0]].pulsePlane.scaleModDef = ppScaleModDefault;
        synths[synthButton[0]].pulsePlane.scaleModTar = ppScaleModDefault;
        
        synths[synthButton[2]].pulsePlane.positionModDef = ppPosModDefault;
        synths[synthButton[2]].pulsePlane.positionModTar = ppPosModDefault;
        synths[synthButton[2]].pulsePlane.scaleModDef = ppScaleModDefault;
        synths[synthButton[2]].pulsePlane.scaleModTar = ppScaleModDefault;
        
        aniPct = 0.0;
        interfaceMoving = true;
        editDetailMoveDirection = 1;
        
        //  mainInterfaceData[5].animationB = true;
        mainInterfaceData[7].animationB = true;
        mainInterfaceData[39].animationB = true;
        mainInterfaceData[12].animationB = true;
        mainInterfaceData[40].animationB = true;
        mainInterfaceData[136].animationB = true;
        mainInterfaceData[137].animationB = true;
        
        for (int i = 0; i < 12; i++) {
            mainInterfaceData[63+i].animationB = true;
        }
        mainInterfaceData[135].animationB = true;
        
        
        detailEditInterfaceOff();
        aniPct = 0.0;
        currentState = STATE_EDIT;
    }
}


//--------------------------------------------------------------
void ofApp::harmonyButtonPress() {
    
    if(currentState != STATE_HARMONY) {
        
        synths[synthButton[0]].aniPath = OneHarmonyPathOn;
        synths[synthButton[0]].myDefault = synthPos[0].getOrientationQuat();
        synths[synthButton[0]].myTarget = harmonyMatrix.getOrientationQuat();
        synths[synthButton[0]].animate = true ;
        synths[synthButton[1]].aniPath = TwoHarmonyPathOn;
        synths[synthButton[1]].myDefault = synthPos[1].getOrientationQuat();
        synths[synthButton[1]].myTarget = harmonyMatrix.getOrientationQuat();
        synths[synthButton[1]].animate = true ;
        synths[synthButton[2]].aniPath = ThreeHarmonyPathOn;
        synths[synthButton[2]].myDefault = synthPos[2].getOrientationQuat();
        synths[synthButton[2]].myTarget = harmonyMatrix.getOrientationQuat();
        synths[synthButton[2]].animate = true ;
        
        synths[synthButton[0]].scaling = true;
        synths[synthButton[1]].scaling = true;
        synths[synthButton[2]].scaling = true;
        synths[synthButton[0]].myScaleDefault = 1.0;
        synths[synthButton[1]].myScaleDefault = 1.0;
        synths[synthButton[2]].myScaleDefault = 1.0;
        synths[synthButton[0]].myScaleTarget = 0.8;
        synths[synthButton[1]].myScaleTarget = 0.8;
        synths[synthButton[2]].myScaleTarget = 0.8;
        
        
        synths[synthButton[0]].pulsePlane.positionModDef = ppPosModDefault;
        synths[synthButton[0]].pulsePlane.positionModTar = ppPosModDefault;
        synths[synthButton[0]].pulsePlane.scaleModDef = ppScaleModDefault;
        synths[synthButton[0]].pulsePlane.scaleModTar = ppScaleModDefault;
        
        synths[synthButton[1]].pulsePlane.positionModDef = ppPosModDefault;
        synths[synthButton[1]].pulsePlane.positionModTar = ppPosModDefault;
        synths[synthButton[1]].pulsePlane.scaleModDef = ppScaleModDefault;
        synths[synthButton[1]].pulsePlane.scaleModTar = ppScaleModDefault;
        
        synths[synthButton[2]].pulsePlane.positionModDef = ppPosModDefault;
        synths[synthButton[2]].pulsePlane.positionModTar = ppPosModDefault;
        synths[synthButton[2]].pulsePlane.scaleModDef = ppScaleModDefault;
        synths[synthButton[2]].pulsePlane.scaleModTar = ppScaleModDefault;
        
        aniPct = 0.0;
        
        pauseInterfaceOff();
        harmonyInterfaceOn();
        currentState = STATE_HARMONY;
        
        
    } else if (currentState == STATE_HARMONY) {
        
        synths[synthButton[0]].aniPath = OneHarmonyPathOff;
        synths[synthButton[0]].myTarget = synthPos[0].getOrientationQuat();
        synths[synthButton[0]].myDefault = harmonyMatrix.getOrientationQuat();
        synths[synthButton[0]].animate = true ;
        synths[synthButton[1]].aniPath = TwoHarmonyPathOff;
        synths[synthButton[1]].myTarget = synthPos[0].getOrientationQuat();
        synths[synthButton[1]].myDefault = harmonyMatrix.getOrientationQuat();
        synths[synthButton[1]].animate = true ;
        synths[synthButton[2]].aniPath = ThreeHarmonyPathOff;
        synths[synthButton[2]].myTarget = synthPos[0].getOrientationQuat();
        synths[synthButton[2]].myDefault = harmonyMatrix.getOrientationQuat();
        synths[synthButton[2]].animate = true ;
        
        synths[synthButton[0]].scaling = true;
        synths[synthButton[1]].scaling = true;
        synths[synthButton[2]].scaling = true;
        synths[synthButton[0]].myScaleDefault = 0.8;
        synths[synthButton[1]].myScaleDefault = 0.8;
        synths[synthButton[2]].myScaleDefault = 0.8;
        synths[synthButton[0]].myScaleTarget = 1.0;
        synths[synthButton[1]].myScaleTarget = 1.0;
        synths[synthButton[2]].myScaleTarget = 1.0;
        
        synths[synthButton[0]].pulsePlane.positionModDef = ppPosModDefault;
        synths[synthButton[0]].pulsePlane.positionModTar = ppPosModDefault;
        synths[synthButton[0]].pulsePlane.scaleModDef = ppScaleModDefault;
        synths[synthButton[0]].pulsePlane.scaleModTar = ppScaleModDefault;
        
        synths[synthButton[1]].pulsePlane.positionModDef = ppPosModDefault;
        synths[synthButton[1]].pulsePlane.positionModTar = ppPosModDefault;
        synths[synthButton[1]].pulsePlane.scaleModDef = ppScaleModDefault;
        synths[synthButton[1]].pulsePlane.scaleModTar = ppScaleModDefault;
        
        synths[synthButton[2]].pulsePlane.positionModDef = ppPosModDefault;
        synths[synthButton[2]].pulsePlane.positionModTar = ppPosModDefault;
        synths[synthButton[2]].pulsePlane.scaleModDef = ppScaleModDefault;
        synths[synthButton[2]].pulsePlane.scaleModTar = ppScaleModDefault;
        
        aniPct = 0.0;
        
        pauseInterfaceOn();
        harmonyInterfaceOff();
        currentState = STATE_DEFAULT;
        
    }
    
}


//--------------------------------------------------------------
void ofApp::bpmButtonPress() {
    
    if(currentState != STATE_BPM) {
        
        synths[synthButton[0]].aniPath = OneTimePathOn;
        synths[synthButton[0]].myDefault = synthPos[0].getOrientationQuat();
        synths[synthButton[0]].myTarget = timeMatrix.getOrientationQuat();
        synths[synthButton[0]].animate = true ;
        synths[synthButton[1]].aniPath = TwoTimePathOn;
        synths[synthButton[1]].myDefault = synthPos[1].getOrientationQuat();
        synths[synthButton[1]].myTarget = timeMatrix.getOrientationQuat();
        synths[synthButton[1]].animate = true ;
        synths[synthButton[2]].aniPath = ThreeTimePathOn;
        synths[synthButton[2]].myDefault = synthPos[2].getOrientationQuat();
        synths[synthButton[2]].myTarget = timeMatrix.getOrientationQuat();
        synths[synthButton[2]].animate = true ;
        
        synths[synthButton[0]].scaling = true;
        synths[synthButton[1]].scaling = true;
        synths[synthButton[2]].scaling = true;
        synths[synthButton[0]].myScaleDefault = 1.0;
        synths[synthButton[1]].myScaleDefault = 1.0;
        synths[synthButton[2]].myScaleDefault = 1.0;
        synths[synthButton[0]].myScaleTarget = 0.45;
        synths[synthButton[1]].myScaleTarget = 0.45;
        synths[synthButton[2]].myScaleTarget = 0.45;
        
        synths[synthButton[0]].pulsePlane.positionModDef = ppPosModDefault;
        synths[synthButton[0]].pulsePlane.positionModTar = ppPosModTempo;
        synths[synthButton[0]].pulsePlane.scaleModDef = ppScaleModDefault;
        synths[synthButton[0]].pulsePlane.scaleModTar = ppScaleModTempo;
        
        synths[synthButton[1]].pulsePlane.positionModDef = ppPosModDefault;
        synths[synthButton[1]].pulsePlane.positionModTar = ppPosModTempo;
        synths[synthButton[1]].pulsePlane.scaleModDef = ppScaleModDefault;
        synths[synthButton[1]].pulsePlane.scaleModTar = ppScaleModTempo;
        
        synths[synthButton[2]].pulsePlane.positionModDef = ppPosModDefault;
        synths[synthButton[2]].pulsePlane.positionModTar = ppPosModTempo;
        synths[synthButton[2]].pulsePlane.scaleModDef = ppScaleModDefault;
        synths[synthButton[2]].pulsePlane.scaleModTar = ppScaleModTempo;
        
        aniPct = 0.0;
        
        for (int i = 0; i < 3; i++){
            synths[i].pulsePlane.animate = true;
            synths[i].pulsePlane.meshBig = true;
        }
        
        pauseInterfaceOff();
        bpmInterfaceOn();
        currentState = STATE_BPM;
        
    } else if (currentState == STATE_BPM) {
        
        synths[synthButton[0]].aniPath = OneTimePathOff;
        synths[synthButton[0]].myTarget = synthPos[0].getOrientationQuat();
        synths[synthButton[0]].myDefault = timeMatrix.getOrientationQuat();
        synths[synthButton[0]].animate = true ;
        synths[synthButton[1]].aniPath = TwoTimePathOff;
        synths[synthButton[1]].myTarget = synthPos[0].getOrientationQuat();
        synths[synthButton[1]].myDefault = timeMatrix.getOrientationQuat();
        synths[synthButton[1]].animate = true ;
        synths[synthButton[2]].aniPath = ThreeTimePathOff;
        synths[synthButton[2]].myTarget = synthPos[0].getOrientationQuat();
        synths[synthButton[2]].myDefault = timeMatrix.getOrientationQuat();
        synths[synthButton[2]].animate = true ;
        
        
        synths[synthButton[0]].scaling = true;
        synths[synthButton[1]].scaling = true;
        synths[synthButton[2]].scaling = true;
        synths[synthButton[0]].myScaleDefault = 0.45;
        synths[synthButton[1]].myScaleDefault = 0.45;
        synths[synthButton[2]].myScaleDefault = 0.45;
        synths[synthButton[0]].myScaleTarget = 1.0;
        synths[synthButton[1]].myScaleTarget = 1.0;
        synths[synthButton[2]].myScaleTarget = 1.0;
        
        synths[synthButton[0]].pulsePlane.positionModDef = ppPosModTempo;
        synths[synthButton[0]].pulsePlane.positionModTar = ppPosModDefault;
        synths[synthButton[0]].pulsePlane.scaleModDef = ppScaleModTempo;
        synths[synthButton[0]].pulsePlane.scaleModTar = ppScaleModDefault;
        
        synths[synthButton[1]].pulsePlane.positionModDef = ppPosModTempo;
        synths[synthButton[1]].pulsePlane.positionModTar = ppPosModDefault;
        synths[synthButton[1]].pulsePlane.scaleModDef = ppScaleModTempo;
        synths[synthButton[1]].pulsePlane.scaleModTar = ppScaleModDefault;
        
        synths[synthButton[2]].pulsePlane.positionModDef = ppPosModTempo;
        synths[synthButton[2]].pulsePlane.positionModTar = ppPosModDefault;
        synths[synthButton[2]].pulsePlane.scaleModDef = ppScaleModTempo;
        synths[synthButton[2]].pulsePlane.scaleModTar = ppScaleModDefault;
        
        aniPct = 0.0;
        
        for (int i = 0; i < 3; i++){
            synths[i].pulsePlane.animate = true;
            synths[i].pulsePlane.meshBig = false;
        }
        
        pauseInterfaceOn();
        bpmInterfaceOff();
        currentState = STATE_DEFAULT;
        
    }
}

//--------------------------------------------------------------

void ofApp::loadSaveButtonPress(){
    
    
    
    
    if(currentState == STATE_DEFAULT) {
        synths[synthButton[0]].aniPath = OneLoadPathOn;
        synths[synthButton[0]].myDefault = synthPos[0].getOrientationQuat();
        synths[synthButton[0]].myTarget = synthPos[0].getOrientationQuat();
        synths[synthButton[0]].animate = true ;
        synths[synthButton[1]].aniPath = TwoLoadPathOn;
        synths[synthButton[1]].myDefault = synthPos[1].getOrientationQuat();
        synths[synthButton[1]].myTarget = synthPos[1].getOrientationQuat();
        synths[synthButton[1]].animate = true ;
        synths[synthButton[2]].aniPath = ThreeLoadPathOn;
        synths[synthButton[2]].myDefault = synthPos[2].getOrientationQuat();
        synths[synthButton[2]].myTarget = synthPos[2].getOrientationQuat();
        synths[synthButton[2]].animate = true ;
        
        pauseInterfaceOff();
        loadSaveInterfaceOn();
        aniPct = 0.0;
        currentState = STATE_SAVE;
        
        for (int i = 0; i < 3; i++){
            synths[i].setAllNotesOff();
        }
        
        
        
    } else if (currentState == STATE_SAVE) {
        synths[synthButton[0]].aniPath = OneLoadPathOff;
        synths[synthButton[0]].myDefault = synthPos[0].getOrientationQuat();
        synths[synthButton[0]].myTarget = synthPos[0].getOrientationQuat();
        synths[synthButton[0]].animate = true ;
        synths[synthButton[1]].aniPath = TwoLoadPathOff;
        synths[synthButton[1]].myDefault = synthPos[1].getOrientationQuat();
        synths[synthButton[1]].myTarget = synthPos[1].getOrientationQuat();
        synths[synthButton[1]].animate = true ;
        synths[synthButton[2]].aniPath = ThreeLoadPathOff;
        synths[synthButton[2]].myDefault = synthPos[2].getOrientationQuat();
        synths[synthButton[2]].myTarget = synthPos[2].getOrientationQuat();
        synths[synthButton[2]].animate = true ;
        
        
        pauseInterfaceOn();
        loadSaveInterfaceOff();
        aniPct = 0.0;
        currentState = STATE_DEFAULT;
        
        
        startUp = true;
        volumeRestartTarget = mainVol;
        volumeRestart = 0.0;
        volumeRamp.value(0.0);
        cout <<"save button :" << volumeRestartTarget << endl;

        
    }
    
    for (int i = 0; i < 3; i++){
        synths[i].pulsePlane.lineMeshQA.clear();
        synths[i].pulsePlane.lineMeshQB.clear();
        synths[i].pulsePlane.lineMesh.clear();
        synths[i].pulsePlane.lineWaitForBeat = true;
    }
}

//--------------------------------------------------------------
void ofApp::settingsButtonPress(){
    
    
    if(currentState == STATE_DEFAULT) {
        synths[synthButton[0]].aniPath = OneSettingsPathOn;
        synths[synthButton[0]].myDefault = synthPos[0].getOrientationQuat();
        synths[synthButton[0]].myTarget = synthPos[0].getOrientationQuat();
        synths[synthButton[0]].animate = true ;
        synths[synthButton[1]].aniPath = TwoSettingsPathOn;
        synths[synthButton[1]].myDefault = synthPos[1].getOrientationQuat();
        synths[synthButton[1]].myTarget = synthPos[1].getOrientationQuat();
        synths[synthButton[1]].animate = true ;
        synths[synthButton[2]].aniPath = ThreeSettingsPathOn;
        synths[synthButton[2]].myDefault = synthPos[2].getOrientationQuat();
        synths[synthButton[2]].myTarget = synthPos[2].getOrientationQuat();
        synths[synthButton[2]].animate = true ;
        
        pauseInterfaceOff();
        openSettingsInterface();
        aniPct = 0.0;
        currentState = STATE_SETTINGS;
        
        
        
    } else if (currentState == STATE_SETTINGS) {
        synths[synthButton[0]].aniPath = OneSettingsPathOff;
        synths[synthButton[0]].myDefault = synthPos[0].getOrientationQuat();
        synths[synthButton[0]].myTarget = synthPos[0].getOrientationQuat();
        synths[synthButton[0]].animate = true ;
        synths[synthButton[1]].aniPath = TwoSettingsPathOff;
        synths[synthButton[1]].myDefault = synthPos[1].getOrientationQuat();
        synths[synthButton[1]].myTarget = synthPos[1].getOrientationQuat();
        synths[synthButton[1]].animate = true ;
        synths[synthButton[2]].aniPath = ThreeSettingsPathOff;
        synths[synthButton[2]].myDefault = synthPos[2].getOrientationQuat();
        synths[synthButton[2]].myTarget = synthPos[2].getOrientationQuat();
        synths[synthButton[2]].animate = true ;
        
        
        pauseInterfaceOn();
        closeSettingsInterface();
        aniPct = 0.0;
        currentState = STATE_DEFAULT;
        
        
    }
    
    
}

//--------------------------------------------------------------
void ofApp::setNewGUI() {
    
    
    
    
    for (int i = 0; i < 12; i++) {
        mainInterfaceData[13+i].setColor(synths[activeSynth].colorHue);
        mainInterfaceData[13+i].activateOnColor();
        
        mainInterfaceData[13+i].elementName = notes[  (synths[activeSynth].keyNote+i)%12 ];
        mainInterfaceData[13+i].setStringWidth();
    }
    
    for (int i = 0; i < 12; i++) {
        if (synths[activeSynth].activeScale.steps[i] ) {
            mainInterfaceData[13+i].setOn();
            mainInterfaceData[25+i].onOff = true;
        } else {
            mainInterfaceData[13+i].setOff();
            mainInterfaceData[25+i].onOff = false;
        }
    }
    
    //preset information
    /*
     mainInterfaceData[7].elementName = presetNames[synths[activeSynth].preset%presetNames.size()];
     mainInterfaceData[7].setStringWidth(mainInterfaceData[7].fsPtr->getBBox(mainInterfaceData[7].elementName, mainInterfaceData[7].fontSize, 0, 0).getWidth());
     */
    
    mainInterfaceData[7].setColor(synths[activeSynth].colorHue);
    mainInterfaceData[7].activateOnColor();
    
    
    //notes row
    for (int i = 0; i < 12; i++) {
        mainInterfaceData[25+i].setColor(synths[activeSynth].colorHue);
        
        if(pianoKeys[(synths[activeSynth].keyNote+i)%12] == 0) {
            mainInterfaceData[25+i].activateOnColor();
        } else {
            mainInterfaceData[25+i].activateDarkerColor();
        }
        
        mainInterfaceData[25+i].elementName = ofToString(synths[activeSynth].keyNote+i);
        mainInterfaceData[25+i].setStringWidth();
    }
    
    
    mainInterfaceData[1].setColor(synths[synthButton[0]].colorHue);
    mainInterfaceData[1].activateOnColor();
    
    mainInterfaceData[2].setColor(synths[synthButton[1]].colorHue);
    mainInterfaceData[2].activateOnColor();
    
    mainInterfaceData[3].setColor(synths[synthButton[2]].colorHue);
    mainInterfaceData[3].activateOnColor();
    
    
    muster.setColor(synths[activeSynth].colorHue);
    
    //volume slider
    mainInterfaceData[52].setColor(synths[synthButton[0]].colorHue);
    mainInterfaceData[52].activateOnColor();
    // mainInterfaceData[52].elementName = ofToString(synths[synthButton[0]].sVolume,2);
    // mainInterfaceData[52].setStringWidth(mainInterfaceData[51].fsPtr->getBBox(mainInterfaceData[52].elementName, mainInterfaceData[52].fontSize, 0, 0).getWidth());
    
    mainInterfaceData[53].setColor(synths[synthButton[1]].colorHue);
    mainInterfaceData[53].activateOnColor();
    // mainInterfaceData[53].elementName = ofToString(synths[synthButton[1]].sVolume,2);
    // mainInterfaceData[53].setStringWidth(mainInterfaceData[52].fsPtr->getBBox(mainInterfaceData[53].elementName, mainInterfaceData[53].fontSize, 0, 0).getWidth());
    
    mainInterfaceData[54].setColor(synths[synthButton[2]].colorHue);
    mainInterfaceData[54].activateOnColor();
    //mainInterfaceData[54].elementName = ofToString(synths[synthButton[2]].sVolume,2);
    //mainInterfaceData[54].setStringWidth(mainInterfaceData[53].fsPtr->getBBox(mainInterfaceData[54].elementName, mainInterfaceData[54].fontSize, 0, 0).getWidth());
    
    
    //sof-hard slider
    mainInterfaceData[137].setColor(synths[activeSynth].colorHue);
    mainInterfaceData[137].activateOnColor();
    
    mainInterfaceData[136].setSlider(mainInterface, ofMap(synths[activeSynth].attackSlider, attSldMax, attSldMin, 0.0, 1.0));
    
    //bpm buttons
    
    for (int i = 0; i < 4; i++) {
        mainInterfaceData[112+i].setColor(synths[synthButton[0]].colorHue);
        if (i == 4-synths[synthButton[0]].nextPulseDivision){
            mainInterfaceData[112+i].setOn();
        } else {
            mainInterfaceData[112+i].setOff();
        }
    }
    
    for (int i = 0; i < 4; i++) {
        mainInterfaceData[116+i].setColor(synths[synthButton[1]].colorHue);
        if (i == 4-synths[synthButton[1]].nextPulseDivision){
            mainInterfaceData[116+i].setOn();
        } else {
            mainInterfaceData[116+i].setOff();
        }
    }
    
    for (int i = 0; i < 4; i++) {
        mainInterfaceData[120+i].setColor(synths[synthButton[2]].colorHue);
        if (i == 4-synths[synthButton[2]].nextPulseDivision){
            mainInterfaceData[120+i].setOn();
        } else {
            mainInterfaceData[120+i].setOff();
        }
    }
    
    
    mainInterfaceData[61].elementName = ofToString(notes[globalKey%12]);
    mainInterfaceData[61].setStringWidth();
    
    mainInterfaceData[62].elementName = scaleCollection.scaleVec.at(globalScaleVecPos).name;
    mainInterfaceData[62].setStringWidth();
    
    
    //state edit global harmony
    if(synths[activeSynth].globalHarmony){
        mainInterfaceData[111].elementName = "GLOBAL HARMONY";
        mainInterfaceData[111].setStringWidth();
        mainInterfaceData[44].activateDarkerColor();
        hvSlotD.changeC(ofColor::fromHsb(255,0,195,255));
    } else {
        mainInterfaceData[111].elementName = "LOCAL HARMONY";
        mainInterfaceData[111].setStringWidth();
        mainInterfaceData[44].activateOnColor();
        hvSlotD.changeC(ofColor::fromHsb(synths[activeSynth].colorHue, 235, 180,255));
    }
    
    //pause play
    if (!synths[synthButton[0]].pause) {
        mainInterfaceData[8].elementName = "\uE036";
        mainInterfaceData[8].setStringWidth();
    } else {
        mainInterfaceData[8].elementName = "\uE039";
        mainInterfaceData[8].setStringWidth();
    }
    
    if (!synths[synthButton[1]].pause) {
        mainInterfaceData[9].elementName = "\uE036";
        mainInterfaceData[9].setStringWidth();
    } else {
        mainInterfaceData[9].elementName = "\uE039";
        mainInterfaceData[9].setStringWidth();
    }
    
    if (!synths[synthButton[2]].pause) {
        mainInterfaceData[10].elementName = "\uE036";
        mainInterfaceData[10].setStringWidth();
    } else {
        mainInterfaceData[10].elementName = "\uE039";
        mainInterfaceData[10].setStringWidth();
    }
    
    for (int i=8; i < 8+3; i++){
        mainInterfaceData[i].setStringWidth();
        mainInterfaceData[i].setColor(synths[synthButton[i-8]].colorHue);
        mainInterfaceData[i].activateOnColor();
    }
    
    
    //STATE_HARMONY
    //set color for harmony menu -> synth keynotes, set string
    
    for (int i=58; i < 58+3; i++){
        if(synths[synthButton[i-58]].globalHarmony){
            mainInterfaceData[i].pickColor = ofColor(255,255,255);
            mainInterfaceData[i].setColor(ofColor::fromHsb(255,0,195,255));
            mainInterfaceData[i].activateOnColor();
        } else {
            mainInterfaceData[i].pickColor = ofColor(0,0,0);
            mainInterfaceData[i].setColor(synths[synthButton[i-58]].colorHue);
            mainInterfaceData[i].activateOnColor();
        }
        mainInterfaceData[i].elementName = ofToString(notes[synths[synthButton[i-58]].keyNote%12]) +" / "+ofToString(synths[synthButton[i-58]].keyNote);
        mainInterfaceData[i].setStringWidth();
    }
    
    if (synths[synthButton[0]].globalHarmony) {
        mainInterfaceData[4].elementName = "GLOBAL";
        mainInterfaceData[4].setColor(ofColor::fromHsb(255,0,195,255));
        mainInterfaceData[4].activateOnColor();
        hvSlotA.changeC(ofColor::fromHsb(255,0,195,255));
    } else {
        mainInterfaceData[4].elementName = "LOCAL";
        mainInterfaceData[4].setColor(synths[synthButton[0]].colorHue);
        mainInterfaceData[4].activateOnColor();
        hvSlotA.changeC(ofColor::fromHsb(synths[synthButton[0]].colorHue, 235, 180,255));
    }
    
    mainInterfaceData[4].setStringWidth();
    
    //b
    if (synths[synthButton[1]].globalHarmony) {
        mainInterfaceData[6].elementName = "GLOBAL";
        mainInterfaceData[6].setColor(ofColor::fromHsb(255,0,195,255));
        mainInterfaceData[6].activateOnColor();
        hvSlotB.changeC(ofColor::fromHsb(255,0,195,255));
    } else {
        mainInterfaceData[6].elementName = "LOCAL";
        mainInterfaceData[6].setColor(synths[synthButton[1]].colorHue);
        mainInterfaceData[6].activateOnColor();
        hvSlotB.changeC(ofColor::fromHsb(synths[synthButton[1]].colorHue, 235, 180,255));
    }
    
    mainInterfaceData[6].setStringWidth();
    
    //c
    if (synths[synthButton[2]].globalHarmony) {
        mainInterfaceData[127].elementName = "GLOBAL";
        mainInterfaceData[127].setColor(ofColor::fromHsb(255,0,195,255));
        mainInterfaceData[127].activateOnColor();
        hvSlotC.changeC(ofColor::fromHsb(255,0,195,255));
    } else {
        mainInterfaceData[127].elementName = "LOCAL";
        mainInterfaceData[127].setColor(synths[synthButton[2]].colorHue);
        mainInterfaceData[127].activateOnColor();
        hvSlotC.changeC(ofColor::fromHsb(synths[synthButton[2]].colorHue, 235, 180,255));
    }
    
    mainInterfaceData[127].setStringWidth();
    
    //set scaleNote Info
    
    markScaleSteps(63);
    
    //settings
    if(dynamicVelo){
        mainInterfaceData[140].activateDarkerColor();
        mainInterfaceData[138].elementName = "DYNAMIC VELOCITY";
        mainInterfaceData[138].setStringWidth();
    } else {
        mainInterfaceData[140].activateOnColor();
        mainInterfaceData[138].elementName = "STATIC VELOCITY";
        mainInterfaceData[138].setStringWidth();
    }
    
    if(soundDelay){
        mainInterfaceData[141].elementName = "DELAY ON";
        mainInterfaceData[141].setStringWidth();
        mainInterfaceData[142].activateOnColor();
    } else {
        mainInterfaceData[141].elementName = "DELAY OFF";
        mainInterfaceData[141].setStringWidth();
        mainInterfaceData[142].activateDarkerColor();
        mainInterfaceData[144].activateDarkerColor();
    }
    
    if(autoDelay){
        mainInterfaceData[142].elementName = "DYNAMIC DELAY";
        mainInterfaceData[142].setStringWidth();
        if(soundDelay){
            mainInterfaceData[144].activateDarkerColor();
        }
        
    } else {
        mainInterfaceData[142].elementName = "STATIC DELAY";
        mainInterfaceData[142].setStringWidth();
        if(soundDelay){
            mainInterfaceData[144].activateOnColor();
        }
    }
    if(soundReverb){
        mainInterfaceData[145].elementName = "REVERB ON";
        mainInterfaceData[145].setStringWidth();
        mainInterfaceData[147].activateOnColor();
        mainInterfaceData[149].activateOnColor();
        
    } else {
        mainInterfaceData[145].elementName = "REVERB OFF";
        mainInterfaceData[145].setStringWidth();
        mainInterfaceData[147].activateDarkerColor();
        mainInterfaceData[149].activateDarkerColor();
        
    }
}

//--------------------------------------------------------------
void ofApp::markScaleSteps(int index_) {
    
    /*
     for (int i = 0; i < 12; i++){
     if (synths[activeSynth].activeScale.steps[i] ) {
     mainInterfaceData[index_+i].setColor(synths[activeSynth].colorHue);
     mainInterfaceData[index_+i].setOn();
     //mainInterfaceData[index_+i].activateDarkerColor();
     }else{
     mainInterfaceData[index_+i].setOff();
     //mainInterfaceData[index_+i].setColor(synths[activeSynth].colorHue);
     //mainInterfaceData[index_+i].activateOnColor(mainInterface);
     }
     }
     */
    markSynthNotes(index_);
}

//--------------------------------------------------------------
void ofApp::markSynthNotes(int index_){
    
    for (int i = 0; i < 12; i++){
        if (synths[activeSynth].activeScale.steps[i] ) {
            mainInterfaceData[index_+i].setColor(synths[activeSynth].colorHue);
            //mainInterfaceData[index_+i].setOn();
            mainInterfaceData[index_+i].activateDarkerColor();
        }else{
            mainInterfaceData[index_+i].setOff();
            //mainInterfaceData[index_+i].setColor(synths[activeSynth].colorHue);
            //mainInterfaceData[index_+i].activateOnColor(mainInterface);
        }
    }
    
    for (map<unsigned long,cubeGroup>::iterator it=synths[activeSynth].soundsMap.begin(); it!=synths[activeSynth].soundsMap.end(); ++it){
        if(it->second.size > 0){
            mainInterfaceData[it->second.groupNote-synths[activeSynth].keyNote+index_].setColor(it->second.groupColor);
            mainInterfaceData[it->second.groupNote-synths[activeSynth].keyNote+index_].activateOnColor();
        }
    }
}

//--------------------------------------------------------------
void ofApp::markScaleSteps() {
    
    /*
     for (int j = 0; j < 3; j++) {
     for (int i = 0; i < 12; i++){
     if (synths[synthButton[j]].activeScale.steps[i] ) {
     mainInterfaceData[75+(synths[synthButton[j]].ownSlot*12)+i].setColor(synths[synthButton[j]].colorHue);
     mainInterfaceData[75+(synths[synthButton[j]].ownSlot*12)+i].setOn();
     // mainInterfaceData[75+(synths[synthButton[j]].ownSlot*12)+i].activateDarkerColor();
     }else{
     mainInterfaceData[75+(synths[synthButton[j]].ownSlot*12)+i].setOff();
     //mainInterfaceData[index_+i].setColor(synths[activeSynth].colorHue);
     //mainInterfaceData[index_+i].activateOnColor(mainInterface);
     }
     }
     }
     */
    markSynthNotes();
}

//--------------------------------------------------------------
void ofApp::markSynthNotes(){
    
    for (int j = 0; j < 3; j++) {
        for (int i = 0; i < 12; i++){
            if (synths[synthButton[j]].activeScale.steps[i] ) {
                //mainInterfaceData[75+(synths[synthButton[j]].ownSlot*12)+i].setColor(synths[synthButton[j]].colorHue);
                //mainInterfaceData[75+(synths[synthButton[j]].ownSlot*12)+i].setOn();
                mainInterfaceData[75+(synths[synthButton[j]].ownSlot*12)+i].activateDarkerColor();
            } else{
                mainInterfaceData[75+(synths[synthButton[j]].ownSlot*12)+i].setOff();
                //mainInterfaceData[index_+i].setColor(synths[activeSynth].colorHue);
                //mainInterfaceData[index_+i].activateOnColor(mainInterface);
            }
        }
        
        for (map<unsigned long,cubeGroup>::iterator it=synths[synthButton[j]].soundsMap.begin(); it!=synths[synthButton[j]].soundsMap.end(); ++it){
            if(it->second.size > 0){
                mainInterfaceData[(it->second.groupNote-synths[synthButton[j]].keyNote)+75+(synths[synthButton[j]].ownSlot*12)].setColor(it->second.groupColor);
                mainInterfaceData[(it->second.groupNote-synths[synthButton[j]].keyNote)+75+(synths[synthButton[j]].ownSlot*12)].activateOnColor();
            }
        }
    }
}



void ofApp::exit(){
    startUp = true;
    for (int i = 0; i < 3; i++){
        synths[i].setAllNotesOff();
    }
    saveToXml("settings.xml");
    ofSoundStreamClose();
    midiOut.closePort();
}

void ofApp::savePreset(){
    saveManager.checkDate();
    
    
    saveToXml("saves/"+saveManager.saveLastYear+saveManager.saveLastMonth+saveManager.saveLastDay+"#"+ofToString(saveManager.saveLastNumber)+".xml");
    
    saveManager.addNewSave(settings);
}

void ofApp::loadPreset(){
    
    loadFromXml(saveManager.loadString, false);
}


void ofApp::saveToXml(string path_){
    settings.clear();
    //date
    settings.addTag("date");
    settings.pushTag("date");
    settings.addValue("year",saveManager.saveLastYear);
    settings.addValue("month",saveManager.saveLastMonth);
    settings.addValue("day",saveManager.saveLastDay);
    settings.addValue("number", saveManager.saveLastNumber);
    settings.addValue("hour", ofGetTimestampString("%H")+":"+ofGetTimestampString("%M"));
    settings.addValue("highlight", 0);
    settings.popTag();
    
    //--------------------------------
    //save grid presets
    settings.addTag("presets");
    settings.pushTag("presets");
    for (int i = 0; i < muster.flips.size(); i++) {
        settings.addTag("muster");
        settings.pushTag("muster",i);
        string temp = "0000000000000000000000000";
        for (int x = 0; x < muster.flips.at(i).layerInfo.size(); x++) {
            for (int y = 0; y < muster.flips.at(i).layerInfo.at(x).size(); y++) {
                if (muster.flips.at(i).layerInfo.at(x).at(y)) {
                    temp.at(x+(TILES*y)) = '1';
                } else {
                    temp.at(x+(TILES*y)) = '0';
                }
            }
        }
        settings.addValue("info", temp);
        settings.popTag();
    }
    settings.popTag();
    
    
    //--------------------------------
    
    //save current grid
    settings.addTag("currentGrids");
    settings.pushTag("currentGrids");
    for (int i = 0; i < 3; i++) {
        int counter = 0;
        settings.addTag("grid");
        settings.pushTag("grid",i);
        string temp = "0000000000000000000000000";
        for (int x  = 0; x < synths[synthButton[i]].layerInfo.size(); x++) {
            for (int y  = 0; y < synths[synthButton[i]].layerInfo.at(x).size(); y++) {
                if (synths[synthButton[i]].layerInfo.at(x).at(y).hasCube) {
                    temp.at(x+(TILES*y)) = '1';
                    settings.addTag("Note");
                    settings.pushTag("Note",counter++);
                    settings.addValue("x", x);
                    settings.addValue("y", y);
                    int pitch =  synths[ synthButton[i] ].soundsMap[ synths[synthButton[i]].layerInfo.at(x).at(y).cubeGroupId ].groupNote;
                    settings.addValue("pitch", pitch);
                    settings.addValue("scalePos", synths[ synthButton[i] ].soundsMap[ synths[synthButton[i]].layerInfo.at(x).at(y).cubeGroupId ].scalePos);
                    settings.popTag();
                } else {
                    temp.at(x+(TILES*y)) = '0';
                }
            }
        }
        settings.addValue("info", temp);
        settings.popTag();
    }
    settings.popTag();
    
    //--------------------------------
    //save volume
    settings.addTag("Volumes");
    settings.pushTag("Volumes");
    settings.addTag("global");
    settings.pushTag("global");
    settings.addValue("volume",mainVol);
    settings.popTag();
    settings.addTag("slots");
    settings.pushTag("slots");
    for (int i = 0; i < 3; i++) {
        settings.addTag("slot");
        settings.pushTag("slot",i);
        settings.addValue("volume", synths[synthButton[i]].sVolume);
        settings.popTag();
    }
    settings.popTag();
    settings.popTag();
    
    
    //--------------------------------
    
    
    
    //synth globals
    settings.addTag("SynthSettings");
    settings.pushTag("SynthSettings");
    for (int i = 0; i < 3; i++) {
        settings.addTag("synth");
        settings.pushTag("synth",i);
        
        settings.addValue("ScaleVecPos", synths[synthButton[i]].currentScaleVecPos);
        settings.addValue("userScaleBool", synths[synthButton[i]].userScale);
        settings.addValue("cHue", synths[synthButton[i]].colorHue);
        settings.addValue("patchPreset", synths[synthButton[i]].preset);
        settings.addValue("StartStatus", synths[synthButton[i]].trackSwitchOn);
        settings.addValue("pauseStatus", synths[synthButton[i]].pause);
        settings.addValue("keyNote", synths[synthButton[i]].keyNote);
        settings.addValue("globalHarmony", synths[synthButton[i]].globalHarmony);
        settings.addValue("attack", synths[synthButton[i]].attackSlider);
        //activeScale Bools[12]
        string scaleBool = "000000000000";
        for (int j = 0; j < 12; j++) {
            if (synths[synthButton[i]].activeScale.steps[j]) {
                scaleBool.at(j) = '1';
            }
        }
        settings.addValue("ActiveScaleBool", scaleBool);
        
        //activeScaleSteps INT
        settings.addTag("scaleSteps");
        settings.pushTag("scaleSteps");
        for (int j = 0; j < synths[synthButton[i]].scaleNoteSteps.size(); j++) {
            settings.addTag("steps");
            settings.pushTag("steps",j);
            settings.addValue("step", synths[synthButton[i]].scaleNoteSteps.at(j));
            settings.popTag();
        }
        settings.popTag();
        
        //activeDirection
        settings.addTag("activeDirection");
        settings.pushTag("activeDirection");
        string activeDirection = "0000";
        for (int j = 0; j < 4; j++) {
            if (synths[synthButton[i]].activeDirection[j]) {
                activeDirection.at(j) = '1';
            }
        }
        settings.addValue("bools", activeDirection);
        settings.popTag();
        
        
        //connectedDirection
        settings.addTag("connectedDirection");
        settings.pushTag("connectedDirection");
        string connectedDirection = "0000";
        for (int j = 0; j < 4; j++) {
            if (synths[synthButton[i]].connectedDirection[j]) {
                connectedDirection.at(j) = '1';
            }
        }
        settings.addValue("bools", connectedDirection);
        settings.popTag();
        
        settings.popTag();
    }
    settings.popTag();
    
    
    //--------------------------------
    //misc , bpm
    
    settings.addTag("BPM");
    settings.pushTag("BPM");
    settings.addTag("global");
    settings.pushTag("global");
    settings.addValue("value", bpm);
    
    
    
    settings.popTag();
    settings.addTag("slots");
    settings.pushTag("slots");
    for (int i = 0; i < 3; i++) {
        settings.addTag("slot");
        settings.pushTag("slot",i);
        settings.addValue("bpm", synths[synthButton[i]].pulseDivision);
        settings.popTag();
    }
    settings.popTag();
    settings.popTag();
    
    settings.addTag("Velocity");
    settings.pushTag("Velocity");
    settings.addValue("velo", globalVelo);
    settings.addValue("toggle", dynamicVelo);
    settings.popTag();
    
    settings.addTag("delay");
    settings.pushTag("delay");
    settings.addValue("toggleDelay", soundDelay);
    settings.addValue("autoDelay", autoDelay);
    settings.addValue("staticDelay", staticDelayValue);
    settings.addValue("dynamicDelay", dynamicDelayValue);
    settings.popTag();
    
    settings.addTag("reverb");
    settings.pushTag("reverb");
    settings.addValue("toggleRev", soundReverb);
    settings.addValue("revTime", revTime);
    settings.addValue("revSize", revSize);
    settings.popTag();
    
    settings.addTag("GlobalKey");
    settings.pushTag("GlobalKey");
    settings.addValue("key", globalKey);
    settings.popTag();
    
    settings.addTag("GlobalScale");
    settings.pushTag("GlobalScale");
    settings.addValue("pos", globalScaleVecPos);
    settings.popTag();
    
    //version
    settings.addTag("Version");
    settings.pushTag("Version");
    settings.addValue("number", appVersion);
    settings.popTag();
    
#if TARGET_OS_IPHONE
    settings.saveFile(ofxiOSGetDocumentsDirectory()+path_);
#else
    settings.saveFile(path_);
#endif
    
}

void ofApp::loadFromXml(string path_, bool settings_){
 
    
    //load grid presets from xml
    
    if (settings.loadFile(path_)) {
        
        
        if(settings_){
            settings.pushTag("Version");
            //dont load old xmlsettings
            
            if (ofToString( settings.getValue("number", "") ) != appVersion) {
                // cout << "old xml settings" << endl;
                saveManager.firstStart = true;
                settings.popTag();
            } else {
                settings.popTag();
            }
        }
        
    }else if (settings.loadFile("settingsDefault.xml")) {
        saveManager.firstStart = true;
        //cout << "loadDefault" << endl;
    }
    
    settings.pushTag("presets");
    int nMuster = settings.getNumTags("muster");
    
    if (nMuster == muster.flips.size()) {
        for (int i = 0; i < nMuster; i++) {
            settings.pushTag("muster",i);
            string temp = settings.getValue("info", "0");
            
            if (temp.size() == TILES*TILES){
                for (int x = 0; x < muster.flips.at(i).layerInfo.size(); x++) {
                    for (int y = 0; y < muster.flips.at(i).layerInfo.at(x).size(); y++) {
                        if (temp.at(x+(TILES*y)) == '1') {
                            muster.flips.at(i).layerInfo.at(x).at(y) = true;
                        }
                        if (temp.at(x+(TILES*y)) == '0') {
                            muster.flips.at(i).layerInfo.at(x).at(y) = false;
                        }
                    }
                }
                muster.flips.at(i).makeTex();
            }
            settings.popTag();
        }
    }
    settings.popTag();
    
    
    //--------------------------------
    
    //load volume
    settings.pushTag("Volumes");
    settings.pushTag("global");
    startUp = true;
  
    volumeRestartTarget = settings.getValue("volume", 1.0);
    volumeRestart = 0.0;
    mainVol = volumeRestartTarget;
    setMainVolume(volumeRestart);

    cout <<"load xml :" << volumeRestartTarget << endl;
    //mainInterfaceData[51].elementName = ofToString(volumeRestartTarget,2);
    //mainInterfaceData[51].setStringWidth(mainInterfaceData[51].fsPtr->getBBox(mainInterfaceData[51].elementName, mainInterfaceData[51].fontSize, 0, 0).getWidth());
    settings.popTag();
    settings.pushTag("slots");
    for (int i = 0; i < 3; i++) {
        settings.pushTag("slot",i);
        synths[synthButton[i]].sVolume = settings.getValue("volume", 0.85);
        synths[synthButton[i]].changeSynthVolume(synths[synthButton[i]].sVolume);
        settings.popTag();
    }
    settings.popTag();
    settings.popTag();
    
    
    
    //--------------------------------
    
    //bpm
    settings.pushTag("BPM");
    settings.pushTag("global");
    bpm =ofClamp(settings.getValue("value", 50), 0, 500);
    tonicSynth.setParameter("BPM", bpm*BPM_DIVISION_MAX);
    settings.popTag();
    settings.pushTag("slots");
    for (int i = 0; i < 3; i++) {
        settings.pushTag("slot",i);
        synths[synthButton[i]].nextPulseDivision = settings.getValue("bpm", 4);
        settings.popTag();
    }
    settings.popTag();
    settings.popTag();
    
    //delay
    settings.pushTag("delay");
    soundDelay = settings.getValue("toggleDelay", true);
    autoDelay = settings.getValue("autoDelay", true);
    staticDelayValue = settings.getValue("staticDelay", 0.5);
    dynamicDelayValue = settings.getValue("dynamicDelay", 0.5);
    if(autoDelay) {
        tonicSynth.setParameter("delay",getBpmValue(dynamicDelayValue));
    } else {
        tonicSynth.setParameter("delay",getBpmValue(staticDelayValue));
    }
    settings.popTag();
    
    //reverb
    settings.pushTag("reverb");
    soundReverb = settings.getValue("toggleRev", false);
    revTime = settings.getValue("revTime", 0.5);
    revSize = settings.getValue("revSize", 0.5);
    settings.popTag();
    
    //globalKey
    settings.pushTag("GlobalKey");
    globalKey =settings.getValue("key",0);
    settings.popTag();
    
    
    //globalVelocity
    settings.pushTag("Velocity");
    globalVelo = settings.getValue("velo", 0.5);
    dynamicVelo = settings.getValue("toggle", true);
    for (int i = 0; i < 3; i++) {
        synths[i].velocity = globalVelo;
        synths[i].dynamicVelo = dynamicVelo;
    }
    settings.popTag();
    
    
    //globalScale
    settings.pushTag("GlobalScale");
    globalScaleVecPos =settings.getValue("pos",0);
    settings.popTag();
    
    //---------------------
    
    //synth global settings
    
    settings.pushTag("SynthSettings");
    for (int i = 0; i < 3; i++) {
        settings.pushTag("synth",i);
        synths[synthButton[i]].currentScaleVecPos = settings.getValue("ScaleVecPos", 0);
        //   synths[synthButton[i]].setMusicScale(scaleCollection, synths[synthButton[i]].currentScaleVecPos);
        
        synths[synthButton[i]].userScale = settings.getValue("userScaleBool", 0);
        
        synths[synthButton[i]].colorHue = settings.getValue("cHue", 9);
        
        synths[synthButton[i]].pulsePlane.setColor(synths[synthButton[i]].colorHue);
        
        for (int k = 0; k < 4; k++){
            synths[synthButton[i]].pulsePlane.blink[k] = true;
            synths[synthButton[i]].pulsePlane.blinkPct[k] = 0.01;
            for (int j = k*synths[synthButton[i]].pulsePlane.dirMeshVerts; j < (k*synths[synthButton[i]].pulsePlane.dirMeshVerts)+synths[synthButton[i]].pulsePlane.dirMeshVerts; j++) {
                if(synths[synthButton[i]].pulsePlane.meshState[k] == 0){
                    synths[synthButton[i]].pulsePlane.directionMesh.setColor(j, synths[synthButton[i]].pulsePlane.pulseColorA);
                } else {
                    synths[synthButton[i]].pulsePlane.directionMesh.setColor(j, synths[synthButton[i]].pulsePlane.pulseColorC);
                }
            }
        }
        
        synths[synthButton[i]].preset = settings.getValue("patchPreset", 0);
        
        synths[synthButton[i]].trackSwitchOn = settings.getValue("StartStatus", 1);
        
        synths[synthButton[i]].pause = settings.getValue("pauseStatus", 0);
        
        synths[synthButton[i]].keyNote = settings.getValue("keyNote", 60);
        
        synths[synthButton[i]].globalHarmony = settings.getValue("globalHarmony", 1);
        
        synths[synthButton[i]].attackSlider = ofClamp(settings.getValue("attack", 1.0), attSldMin, attSldMax );
        
        //set the layer lowFreqVolumeFactor to keynote
        synths[synthButton[i]].mainTonicPtr->setParameter("lfvf"+synths[synthButton[i]].instrumentId, synths[synthButton[i]].getLfvf(synths[synthButton[i]].preset));
        
        //scaleNoteSteps
        string tempActiveScale = settings.getValue("ActiveScaleBool", "100000000000");
        for (int j = 0; j < 12; j++) {
            if (tempActiveScale.at(j) == '1') {
                synths[synthButton[i]].activeScale.steps[j] = true;
            } else {
                synths[synthButton[i]].activeScale.steps[j] = false;
                
            }
        }
        
        
        //activeScaleSteps INT
        settings.pushTag("scaleSteps");
        synths[synthButton[i]].scaleNoteSteps.clear();
        for (int j = 0; j < settings.getNumTags("steps"); j++) {
            settings.pushTag("steps",j);
            synths[synthButton[i]].scaleNoteSteps.push_back(settings.getValue("step", 1));
            settings.popTag();
        }
        settings.popTag();
        
        
        //activeDirection
        settings.pushTag("activeDirection");
        string activeDirection = settings.getValue("bools", "1111");
        synths[synthButton[i]].pulsePlane.activeDirs = 0;
        for (int j = 0; j < 4; j++) {
            if (activeDirection.at(j) == '1') {
                synths[synthButton[i]].activeDirection[j] = true;
                synths[synthButton[i]].pulsePlane.activeDirs++;
            } else{
                synths[synthButton[i]].activeDirection[j] = false;
            }
        }
        settings.popTag();
        
        
        //connectedDirection
        settings.pushTag("connectedDirection");
        string connectedDirection = settings.getValue("bools", "1111");
        for (int j = 0; j < 4; j++) {
            if (connectedDirection.at(j) == '1') {
                synths[synthButton[i]].connectedDirection[j] = true;
            } else {
                synths[synthButton[i]].connectedDirection[j] = false;
            }
        }
        settings.popTag();
        
        
        settings.popTag();
        
        //set bpm icons state
        for (int j = 0; j < 4;j++) {
            if (synths[i].activeDirection[j] && synths[i].connectedDirection[j]) {
                synths[i].pulsePlane.buttonState[j] = 1;
            } else if (synths[i].activeDirection[j] && !synths[i].connectedDirection[j]) {
                synths[i].pulsePlane.buttonState[j] = 0;
            } else if (!synths[i].activeDirection[j] && !synths[i].connectedDirection[j]) {
                synths[i].pulsePlane.buttonState[j] = 2;
            }
        }
    }
    settings.popTag();
    
    
    
    //--------------------------------
    
    //load current grids
    settings.pushTag("currentGrids");
    for (int i = 0; i < 3; i++) {
        settings.pushTag("grid",i);
        string temp = settings.getValue("info", "0");
        if (temp.size() == TILES*TILES){
            
            //clear the grid
            for (int x = 0; x < TILES; x++) {
                for (int y = 0; y <TILES; y++) {
                    if (synths[synthButton[i]].layerInfo.at(x).at(y).hasCube) {
                        synths[synthButton[i]].tapEvent(x, y);
                    }
                }
            }
            
            //load grid
            for (int x = 0; x < TILES; x++) {
                for (int y = 0; y <TILES; y++) {
                    if (temp.at(x+(TILES*y)) == '1') {
                        synths[synthButton[i]].tapEvent(x, y);
                    }
                }
            }
        }
        settings.popTag();
    }
    settings.popTag();
    
    //---------------------------
    
    //set pause status
    
    for (int i = 0; i < 3; i++) {
        if(synths[synthButton[i]].pause) {
            synths[synthButton[i]].setSaturationOff();
        } else {
            synths[synthButton[i]].setSaturationOn();
        }
    }
    
    //---------------------------
    
    //load notes
    
    settings.pushTag("currentGrids");
    for (int i = 0; i < 3; i++) {
        settings.pushTag("grid",i);
        if (settings.getNumTags("Note") > 0) {
            for (int j = 0; j < settings.getNumTags("Note"); j++) {
                settings.pushTag("Note",j);
                
                //laod pitch
                int tempPitch =
                synths[synthButton[i]].soundsMap[
                                                 synths[synthButton[i]].layerInfo.at(settings.getValue("x",0)).at(settings.getValue("y",0)).cubeGroupId
                                                 ].groupNote;
                
                
                if (tempPitch != settings.getValue("pitch", 60)) {
                    //load groupnote
                    synths[synthButton[i]].soundsMap[
                                                     synths[synthButton[i]].layerInfo.at(settings.getValue("x",0)).at(settings.getValue("y",0)).cubeGroupId
                                                     ].groupNote = settings.getValue("pitch", 60);
                    //set groupnote
                    synths[synthButton[i]].soundsMap[
                                                     synths[synthButton[i]].layerInfo.at(settings.getValue("x",0)).at(settings.getValue("y",0)).cubeGroupId
                                                     ].groupSynth.setParameter("rampFreqTarget", Tonic::mtof(settings.getValue("pitch", 60) ));
                }
                
                //load scale position
                int tempScalePos =
                synths[synthButton[i]].soundsMap[
                                                 synths[synthButton[i]].layerInfo.at(settings.getValue("x",0)).at(settings.getValue("y",0)).cubeGroupId
                                                 ].scalePos;
                
                
                if (tempScalePos != settings.getValue("scalePos", 60)) {
                    synths[synthButton[i]].soundsMap[
                                                     synths[synthButton[i]].layerInfo.at(settings.getValue("x",0)).at(settings.getValue("y",0)).cubeGroupId
                                                     ].scalePos = settings.getValue("scalePos", 60);
                }
                settings.popTag();
            }
        }
        settings.popTag();
    }
    settings.popTag();
    
    
    setNewGUI();
}

float ofApp::getBpmValue(float in){
    float bpmFac, temp;
    /*
    bpmFac = (ofClamp(in,0.0,1.0)*BPM_MAX)/BPM_MAX;
    temp = (1.0-bpmFac)*1.0;
    return ofClamp(temp,0.0001,1.0)*0.85;
     */
    bpmFac = ofMap(in, 0.0, 1.0, 20.0, BPM_MAX+20);
    temp = 60000/bpmFac/2;
    return temp/1000;
}

float ofApp::getRevTime(float in){
    float temp;
    temp = ofMap(in, 0.0, 1.0, 0.15, 3.5);
    return ofClamp(temp, 0.0, 3.5);
}

float ofApp::getRevSize(float in){
    float temp;
    temp = ofMap(in, 0.0, 1.0, 0.01, 1.0);
    return ofClamp(temp, 0.0, 1.0);
}

void ofApp::updateSleepTimer(){
    if (currentState == STATE_DEFAULT){
        sleepTimer = ofGetElapsedTimef() - lastTouch;
        if(sleepTimer > 20 && !sleepMode){
            for  (int i = 0; i < 8; i++){
                mainInterfaceData.at(defaultStateIndex[i]).switchColor();
            }
            sleepMode = true;
        }
    }
}

void ofApp::getFboScreen(){
    ofFbo screen;
    screen.allocate(sWIDTH, sHEIGHT, GL_RGBA, 8);
    
    screen.begin();
    ofClear(0,0,0,255);
    ofBackground(19,19,19);
    
    glLineWidth(2);
    
    //glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    
    glEnable(GL_MULTISAMPLE);
    //ofEnableLighting();
    //light.enable();
    //material.begin();
    
    if (!debugCam) {
        testCam.begin();
    } else {
        cam.begin();
    }
    //  thisIntersect.draw();
    for (int i = 0; i < 3; i++) {
        synths[i].myNode.transformGL();
        
        synths[i].draw();
        
        synths[i].myNode.restoreTransformGL();
    }
    
    
    if (!debugCam) {
        testCam.end();
    } else {
        cam.end();
    }
    
    // material.end();
    //ofDisableLighting();
    
    if (drawInfo) {
        drawDebug();
    }
    
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_MULTISAMPLE);
    
    drawInterface();
    

    screen.end();
    
    ofImage pix;
    pix.allocate(sWIDTH, sHEIGHT, OF_IMAGE_COLOR_ALPHA);
    screen.readToPixels(pix);
    
    ofImage out;
    out.allocate(sWIDTH, sHEIGHT, OF_IMAGE_COLOR_ALPHA);
    out.setFromPixels(pix);
    out.save( ofGetTimestampString() +"5_5_screen.png");
}
