#include "ofApp.h"
#define TILES 5
#define TILESIZE (100/TILES)
#define TILEBORDER 0.075
#define BPM (220*4)
#define ANI_SPEED 0.025
#define VERSION "0.93.5"


#define SINEA 25.8
#define SINEB 2

#define HISTORY_ROWS 50

enum currentState {STATE_DEFAULT,STATE_EDIT,STATE_VOLUME,STATE_EDIT_DETAIL, STATE_BPM, STATE_HARMONY};




//--------------------------------------------------------------
void ofApp::setup(){
    //11025 samplerate changed in toniccore.h, only ios project
#if TARGET_OS_IPHONE
    setSampleRate(44100);
    ofSetOrientation(OF_ORIENTATION_90_RIGHT);
    ofSoundStreamSetup(2, 1, this, 44100, 256*2, 1);
    pitchCorrect = 0;
#else
    setSampleRate(44100);
    ofSoundStreamSetup(2, 1, this, 44100, 256*2, 4);
    pitchCorrect = 0;
#endif
    
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    ofEnableDepthTest();
    ofEnableAlphaBlending();
    
    // ofSetDataPathRoot("../Resources/data/");
    
#if TARGET_OS_IPHONE
    robotoLight.loadFont("fonts/Roboto-Light.ttf", 120);
#else
    robotoLight.setup("fonts/Roboto-Light.ttf", //font file, ttf only
                      1.0,					//lineheight percent
                      1024,					//texture atlas dimension
                      false,					//create mipmaps of the font, useful to scale down the font at smaller sizes
                      8,					//texture atlas element padding, shouldbe >0 if using mipmaps otherwise
                      2.0f					//dpi scaleup, render textures @2x the reso
                      );				//lower res mipmaps wil bleed into each other
    //robotoLight.setCharacterSpacing(1);
    robotoLight.setKerning(robotoLight.getKerning());
#endif
    
#if TARGET_OS_IPHONE
    robotoBold.loadFont("fonts/Roboto-Bold.ttf", 120);
#else
    robotoBold.setup("fonts/Roboto-Bold.ttf", //font file, ttf only
                     1.0,					//lineheight percent
                     1024,					//texture atlas dimension
                     false,					//create mipmaps of the font, useful to scale down the font at smaller sizes
                     8,					//texture atlas element padding, shouldbe >0 if using mipmaps otherwise
                     2.0f					//dpi scaleup, render textures @2x the reso
                     );				//lower res mipmaps wil bleed into each other
    // robotoBold.setCharacterSpacing(1);
    robotoBold.setKerning(robotoBold.getKerning());
#endif
    
#if TARGET_OS_IPHONE
    robotoCon.loadFont("fonts/RobotoCondensed-Italic.ttf", 120);
#else
    robotoCon.setup("fonts/RobotoCondensed-Italic.ttf", //font file, ttf only
                    1.0,					//lineheight percent
                    1024,					//texture atlas dimension
                    false,					//create mipmaps of the font, useful to scale down the font at smaller sizes
                    8,					//texture atlas element padding, shouldbe >0 if using mipmaps otherwise
                    2.0f					//dpi scaleup, render textures @2x the reso
                    );				//lower res mipmaps wil bleed into each other
    // robotoCon.setCharacterSpacing(1);
    robotoCon.setKerning(robotoCon.getKerning());
#endif
    
    scaleCollection.loadScales();
    globalScaleVecPos = 0;
    
    makeDesignGrid();
    currentState = STATE_DEFAULT;
    appVersion = VERSION;
    
    synthPos.resize(3);
    
    camActiveFov = 26;
    camFov = 38;
    
    nearClip = 10;
    farClip = 10000;
    
    
    
    //global volume
    ControlParameter rampTarget = tonicSynth.addParameter("mainVolumeRamp").max(1.0).min(0.0);
    tonicSynth.setParameter("mainVolumeRamp", 0.0);
    mainVol = 0.0;
    volumeRamp = RampedValue().value(0.0).length(0.1).target(rampTarget);
    startUp = true;
    volumeRestart = 0.0;
    volumeRestartTarget = 0.0;
    
    
    bpm = BPM;
    
    ControlParameter bpmpara = tonicSynth.addParameter("BPM");
    tonicSynth.setParameter("BPM", bpm*4);
    ControlGenerator pulse = ControlMetro().bpm(bpmpara);
    ControlMetroDivider pulseDiv2 = ControlMetroDivider().divisions(2).input(pulse);
    ControlMetroDivider pulseDiv3 = ControlMetroDivider().divisions(3).input(pulse);
    ControlMetroDivider pulseDiv4 = ControlMetroDivider().divisions(4).input(pulse);
    
    TonicFloat sa = SINEA;
    TonicFloat sb = SINEB;
    
    sineA = SineWave().freq(sa);
    sineB = SineWave().freq(sb);
    
    
    ofEvent<float>* pulseEventDiv1 = tonicSynth.createOFEvent(pulse);
    ofAddListener(*pulseEventDiv1, this, &ofApp::pulseEventDiv1 );
    
    ofEvent<float>* pulseEventDiv2 = tonicSynth.createOFEvent(pulseDiv2);
    ofAddListener(*pulseEventDiv2, this, &ofApp::pulseEventDiv2 );
    
    ofEvent<float>* pulseEventDiv3 = tonicSynth.createOFEvent(pulseDiv3);
    ofAddListener(*pulseEventDiv3, this, &ofApp::pulseEventDiv3 );
    
    ofEvent<float>* pulseEventDiv4= tonicSynth.createOFEvent(pulseDiv4);
    ofAddListener(*pulseEventDiv4, this, &ofApp::pulseEventDiv4 );
    
    
    for (int i = -1; i < 2; i++) {
        ofNode temp;
        //temp.setPosition( i*(TILES*TILESIZE*2), 0, 0);
        temp.setPosition( intersectPlane( designGrid[i+1][1] ) );
        synthPos[i+1]=temp;
    }
    
    
    
    
    
    globalKey = 0;
    
    synthButton[0] = 0;
    synthButton[1] = 1;
    synthButton[2] = 2;
    
    synths.resize(3);
    
    synths[0] = Instrument("a",TILES,TILESIZE,TILEBORDER,HISTORY_ROWS);
    synths[0].setup(&timeCounter, &tonicSynth, synthPos[0], &sineA, &sineB,&currentState);
    synths[0].setMusicScale(scaleCollection, 0);
    synths[0].setKeyNote(60+globalKey-12);
    synths[0].ownSlot = 0;
    
    synths[1] = Instrument("b",TILES,TILESIZE,TILEBORDER,HISTORY_ROWS);
    synths[1].setup(&timeCounter, &tonicSynth, synthPos[1], &sineA, &sineB,&currentState);
    synths[1].setMusicScale(scaleCollection, 0);
    synths[1].setKeyNote(60+globalKey);
    synths[1].ownSlot = 1;
    
    synths[2] = Instrument("c",TILES,TILESIZE,TILEBORDER,HISTORY_ROWS);
    synths[2].setup(&timeCounter, &tonicSynth, synthPos[2], &sineA, &sineB,&currentState);
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
    
    hvSlotA.setup(&mainInterfaceData[75],HISTORY_ROWS,75);
    hvSlotB.setup(&mainInterfaceData[87],HISTORY_ROWS,87);
    hvSlotC.setup(&mainInterfaceData[99],HISTORY_ROWS,99);
    
    
    //intersectplane
    planeForIntersect.set(TILES*TILESIZE,TILES*TILESIZE);
    thisIntersect.setFrom(planeForIntersect);
    
    
    ofBackground( filterColor(ofColor(22,22,22)) );
    
    light.setPosition(synthActivePos.getPosition()+ofVec3f(0,-100,1000));
    light.setDiffuseColor( ofColor(100,100,100));
    material.setShininess(55);
    material.setSpecularColor( filterColor(ofColor::white));
    //temp sketch
    light.setAmbientColor( filterColor(ofColor::white) );
    
    doubleClickTime = 300;
    curTap = 0;
    lastTap = 0;
    mouseDragging = false;
    tapCounter = 0;
    drawInfo = false;
    
    timeCounter = -1;
    
    
    focusCam = false;
    
    
    
    //  aniPct = 1.0;
    aniCam = 1.0;
    
    animCam = false;
    interfaceMoving = false;
    debugCam = false;
    insideSynth = false;
    
    //load icons
    ofImage temp;
    
    temp.loadImage("icons/back.png");
    backIcon.loadData(temp.getPixelsRef(),GL_RGBA);
    
    temp.loadImage("icons/detail.png");
    detailIcon.loadData(temp.getPixelsRef(), GL_RGBA);
    
    temp.loadImage("icons/volume.png");
    volumeIcon.loadData(temp.getPixelsRef(), GL_RGBA);
    
    temp.loadImage("icons/pause.png");
    pauseIcon.loadData(temp.getPixelsRef(), GL_RGBA);
    
    temp.loadImage("icons/play.png");
    playIcon.loadData(temp.getPixelsRef(), GL_RGBA);
    
    temp.loadImage("icons/save.png");
    saveIcon.loadData(temp.getPixelsRef(), GL_RGBA);
    
    temp.loadImage("icons/random.png");
    randomIcon.loadData(temp.getPixelsRef(), GL_RGBA);
    
    temp.loadImage("icons/left.png");
    leftIcon.loadData(temp.getPixelsRef(), GL_RGBA);
    
    temp.loadImage("icons/right.png");
    rightIcon.loadData(temp.getPixelsRef(), GL_RGBA);
    
    temp.loadImage("icons/bpm.png");
    bpmIcon.loadData(temp.getPixelsRef(), GL_RGBA);
    
    temp.loadImage("icons/harmony.png");
    harmonyIcon.loadData(temp.getPixelsRef(), GL_RGBA);
    
    
    scaleFac = 1-(1/((designGrid[0][0].y*6)/144));
    scaleFac *= 0.55;
    
    muster = MusterContainer(mainInterfaceData[39].drawStringPos, ofVec2f( mainInterfaceData[39].elementSize), TILES);
    muster.setup();
    
    //load saves
    loadFromXml();
    
    setNewGUI();
    setupAudio();
    
}

void ofApp::setupAudio(){
    
    Generator temp;
    for (int i = 0; i < synths.size(); i++) {
        temp = temp + synths[i].instrumentOut;
    }
    mainOut = temp ;
    
    /*
     Tonic::StereoDelay delay = Tonic::StereoDelay(0.07f,0.18f)
     .delayTimeLeft( 0.7 )
     .delayTimeRight(0.18)
     .feedback(0.18)
     .dryLevel(0.8)
     .wetLevel(0.2);
     */
    
    
    Tonic::StereoDelay delay = Tonic::StereoDelay(0.07f,0.22f)
    .delayTimeLeft( 0.07)
    .delayTimeRight(0.22)
    .feedback(0.1)
    .dryLevel(0.95)
    .wetLevel(0.12);
    
    
    //compressor
    Tonic::Compressor compressor = Compressor()
    .release(0.015)
    .attack(0.0001)
    .threshold( dBToLin(-6) )
    .ratio(6)
    .lookahead(0.001)
    .bypass(false);
    
    tonicSynth.setOutputGen( ((mainOut)*volumeRamp)>>LPF12().cutoff(8000).Q(4) >> delay );
}

//--------------------------------------------------------------
void ofApp::update(){
    //fade in Volume at start
    if (startUp && volumeRestart < volumeRestartTarget) {
        volumeRestart += .012;
        mainVol = volumeRestart;
        volumeRampValueChanged(mainVol);
    } else if (startUp && volumeRestart >= volumeRestartTarget) {
        mainVol = volumeRestartTarget;
        volumeRampValueChanged(mainVol);
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
    }
    
    
    // intersectPlane(ofGetMouseX(),ofGetMouseY());
    
    
    
    // light.setPosition(  synths[activeSynth].myNode.getPosition()+ofVec3f(0,200,150));
}

void ofApp::updateInterfaceMesh() {
    
    // for index number reference see ofApp::setupGlobalInterface()
    
    mainInterfaceData[0].updateMainMeshSlider(mainInterface, designGrid[1][0],tweenFloat);
    mainInterfaceData[54].updateMainMeshSlider(mainInterface, designGrid[1][0],tweenFloat);
    
    
    mainInterfaceData[8].updateMainMesh(mainInterface,designGrid[0][2],tweenFloat);
    mainInterfaceData[9].updateMainMesh(mainInterface, designGrid[1][2],tweenFloat);
    mainInterfaceData[10].updateMainMesh(mainInterface, designGrid[2][2],tweenFloat);
    
    mainInterfaceData[11].updateMainMesh(mainInterface, ofVec3f(designGrid[1][0].x, designGrid[1][2].y,0),tweenFloat);
    
    
    mainInterfaceData[37].updateMainMesh(mainInterface, designGrid[0][0], tweenFloat);
    mainInterfaceData[38].updateMainMesh(mainInterface, designGrid[1][0], tweenFloat);
    
    mainInterfaceData[1].updateMainMeshSlider(mainInterface,designGrid[0][1], tweenFloat);
    mainInterfaceData[2].updateMainMeshSlider(mainInterface, designGrid[1][1],tweenFloat);
    mainInterfaceData[3].updateMainMeshSlider(mainInterface, designGrid[2][1],tweenFloat);
    
    
    mainInterfaceData[51].updateMainMesh(mainInterface, designGrid[0][1],tweenFloat);
    mainInterfaceData[52].updateMainMesh(mainInterface, designGrid[1][1],tweenFloat);
    mainInterfaceData[53].updateMainMesh(mainInterface, designGrid[2][1],tweenFloat);
    
    
    
    mainInterfaceData[4].updateMainMesh(mainInterface, ofVec3f(0,designGrid[0][0].y*2,0),tweenFloat);
    mainInterfaceData[6].updateMainMesh(mainInterface,ofVec3f(designGrid[0][0].x*6,designGrid[0][0].y*2,0),tweenFloat);
    mainInterfaceData[39].updateMainMesh(mainInterface,ofVec3f(designGrid[0][0].x*6,designGrid[0][0].y*2,0), tweenFloat);
    mainInterfaceData[39].updateMainMeshB(mainInterface,ofVec3f(designGrid[0][0].x*6, designGrid[0][0].y*2+(abs((editDetailMoveDirection-tweenFloat))*(designGrid[0][0].y*1.5)),0)
                                          ,tweenFloat);
    
    mainInterfaceData[12].updateMainMesh(mainInterface,designGrid[2][0], tweenFloat);
    
    mainInterfaceData[40].updateMainMesh(mainInterface,designGrid[2][0], tweenFloat);
    
    
    
    for (int i = 0; i < 12; i++) {
        mainInterfaceData[13+i].updateMainMesh(mainInterface,ofVec3f(0,designGrid[0][0].y*2,0),tweenFloat);
    }
    for (int i = 0; i < 12; i++) {
        mainInterfaceData[25+i].updateMainMesh(mainInterface, ofVec3f(0,designGrid[0][0].y*2,0) ,tweenFloat);
    }
    
    mainInterfaceData[50].updateMainMesh(mainInterface,ofVec3f(0,designGrid[0][0].y*2,0),tweenFloat);
    
    
    // mainInterfaceData[5].updateMainMesh(mainInterface, ofVec3f(0, designGrid[0][1].y,0) ,tweenFloat);
    
    mainInterfaceData[7].updateMainMesh(mainInterface, ofVec3f(0, designGrid[0][1].y,0) ,tweenFloat);
    
    //move in edit to detail state
    /*
     mainInterfaceData[5].updateMainMeshB(mainInterface, ofVec3f(0,designGrid[0][1].y+(abs((editDetailMoveDirection-tweenFloat))*(designGrid[0][0].y*1.5)),0)
     ,tweenFloat);
     */
    
    mainInterfaceData[7].updateMainMeshB(mainInterface,  ofVec3f(0,designGrid[0][1].y+(abs((editDetailMoveDirection-tweenFloat))*(designGrid[0][0].y*1.5)),0)
                                         ,tweenFloat);
    
    
    mainInterfaceData[43].updateMainMesh(mainInterface, designGrid[1][2],tweenFloat);
    mainInterfaceData[44].updateMainMesh(mainInterface, designGrid[0][0],tweenFloat);
    
    mainInterfaceData[41].updateMainMesh(mainInterface, designGrid[2][0],tweenFloat);
    
    mainInterfaceData[42].updateMainMesh(mainInterface, designGrid[1][0],tweenFloat);
    
    
    
    
    for (int i = 45; i < 45+4; i++) {
        mainInterfaceData[i].updateMainMesh(mainInterface, designGrid[1][1],tweenFloat);
    }
    
    
    mainInterfaceData[49].updateMainMesh(mainInterface,  ofVec3f(designGrid[1][0].x,designGrid[0][0].y*2,0),tweenFloat);
    
    mainInterfaceData[55].updateMainMeshSlider(mainInterface,designGrid[0][1], tweenFloat);
    mainInterfaceData[56].updateMainMeshSlider(mainInterface, designGrid[1][1],tweenFloat);
    mainInterfaceData[57].updateMainMeshSlider(mainInterface, designGrid[2][1],tweenFloat);
    
    
    mainInterfaceData[58].updateMainMeshSlider(mainInterface,designGrid[0][1], tweenFloat);
    mainInterfaceData[59].updateMainMeshSlider(mainInterface, designGrid[1][1],tweenFloat);
    mainInterfaceData[60].updateMainMeshSlider(mainInterface, designGrid[2][1],tweenFloat);
    
    mainInterfaceData[61].updateMainMeshSlider(mainInterface, designGrid[0][0],tweenFloat);
    
    mainInterfaceData[62].updateMainMeshSlider(mainInterface, designGrid[1][0],tweenFloat);
    
    for (int i = 0; i < 12; i++) {
        mainInterfaceData[63+i].updateMainMesh(mainInterface, ofVec3f(0, designGrid[0][1].y,0) ,tweenFloat);
        mainInterfaceData[63+i].updateMainMeshB(mainInterface, ofVec3f(0, designGrid[0][1].y+(abs((editDetailMoveDirection-tweenFloat))*(designGrid[0][0].y*1.5)),0) ,tweenFloat);
    }
    
    for (int i = 0; i < 12; i++) {
        mainInterfaceData[75+i].updateMainMesh(mainInterface, designGrid[0][1] ,tweenFloat);
    }
    for (int i = 0; i < 12; i++) {
        mainInterfaceData[87+i].updateMainMesh(mainInterface,designGrid[1][1] ,tweenFloat);
    }
    for (int i = 0; i < 12; i++) {
        mainInterfaceData[99+i].updateMainMesh(mainInterface, designGrid[2][1] ,tweenFloat);
    }
    
    mainInterfaceData[111].updateMainMesh(mainInterface, designGrid[0][0],tweenFloat);
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    
    
    //glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    
    glEnable(GL_MULTISAMPLE);
    // ofEnableLighting();
    //  light.enable();
    //  material.begin();
    
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
    
    //  material.end();
    //   ofDisableLighting();
    
    if (drawInfo) {
        drawDebug();
    }
    
    glDisable(GL_DEPTH_TEST);
    
    glDisable(GL_MULTISAMPLE);
    
    mainInterface.draw();
    
    if (currentState == STATE_HARMONY){
        hvSlotA.draw();
        hvSlotB.draw();
        hvSlotC.draw();
    }
    
    
    
    
    drawStringAndIcons();
    
    muster.draw();
    
    
}

void ofApp::drawStringAndIcons(){
    
    
    ofPushStyle();
    ofSetColor(255, 255, 255,255);
    
    //drawStrings
    for (int i = 0; i < mainInterfaceData.size();i ++){
        
        if(i >= 13 && i <= 36) {
            ofSetColor(255, 255, 255,190);
        } else {
            ofSetColor(255, 255, 255,255);
        }
        
        if ( mainInterfaceData[i].showString &&
            
            ((mainInterfaceData[i].drawStringPos.x >= (0-200) &&  mainInterfaceData[i].drawStringPos.x <= ((designGrid[0][0].x*6)+200 )) &&
             mainInterfaceData[i].drawStringPos.y >= (0-200) &&  mainInterfaceData[i].drawStringPos.y <= ((designGrid[0][0].y*6)+200 ))
            ) {
            mainInterfaceData[i].drawFontString();
        }
    }
    
    float tempTrans = 72*scaleFac;
    
    //draw icons
    ofSetColor(mainInterfaceData[43].displayColor);
    backIcon.draw(mainInterfaceData[43].drawStringPos.x-tempTrans, mainInterfaceData[43].drawStringPos.y-tempTrans, 144*scaleFac,144*scaleFac);
    ofSetColor(mainInterfaceData[44].displayColor);
    detailIcon.draw(mainInterfaceData[44].drawStringPos.x-tempTrans, mainInterfaceData[44].drawStringPos.y-tempTrans, 144*scaleFac,144*scaleFac);
    
    ofSetColor(mainInterfaceData[11].displayColor);
    backIcon.draw(mainInterfaceData[11].drawStringPos.x-tempTrans, mainInterfaceData[11].drawStringPos.y-tempTrans,144*scaleFac,144*scaleFac);
    
    ofSetColor(mainInterfaceData[12].displayColor);
    saveIcon.draw(mainInterfaceData[12].drawStringPos.x-tempTrans, mainInterfaceData[12].drawStringPos.y-tempTrans,144*scaleFac,144*scaleFac);
    
    ofSetColor(mainInterfaceData[40].displayColor);
    randomIcon.draw(mainInterfaceData[40].drawStringPos.x-tempTrans, mainInterfaceData[12].drawStringPos.y-tempTrans,144*scaleFac,144*scaleFac);
    
    ofSetColor(mainInterfaceData[4].displayColor);
    leftIcon.draw(mainInterfaceData[4].drawStringPos.x-tempTrans, mainInterfaceData[4].drawStringPos.y-tempTrans,144*scaleFac,144*scaleFac);
    
    ofSetColor(mainInterfaceData[6].displayColor);
    rightIcon.draw(mainInterfaceData[6].drawStringPos.x-tempTrans, mainInterfaceData[6].drawStringPos.y-tempTrans,144*scaleFac,144*scaleFac);
    
    ofSetColor(mainInterfaceData[41].displayColor);
    bpmIcon.draw(mainInterfaceData[41].drawStringPos.x-tempTrans, mainInterfaceData[41].drawStringPos.y-tempTrans,144*scaleFac,144*scaleFac);
    
    ofSetColor(mainInterfaceData[37].displayColor);
    volumeIcon.draw(mainInterfaceData[37].drawStringPos.x-tempTrans, mainInterfaceData[37].drawStringPos.y-tempTrans,144*scaleFac,144*scaleFac);
    
    ofSetColor(mainInterfaceData[42].displayColor);
    harmonyIcon.draw(mainInterfaceData[42].drawStringPos.x-tempTrans, mainInterfaceData[42].drawStringPos.y-tempTrans,144*scaleFac,144*scaleFac);
    
    ofSetColor(mainInterfaceData[8].displayColor);
    
    if(!synths[synthButton[0]].pause) {
        pauseIcon.draw(mainInterfaceData[8].drawStringPos.x-tempTrans, mainInterfaceData[8].drawStringPos.y-tempTrans,144*scaleFac,144*scaleFac);
    } else {
        playIcon.draw(mainInterfaceData[8].drawStringPos.x-tempTrans, mainInterfaceData[8].drawStringPos.y-tempTrans,144*scaleFac,144*scaleFac);
    }
    
    ofSetColor(mainInterfaceData[9].displayColor);
    
    if(!synths[synthButton[1]].pause) {
        pauseIcon.draw(mainInterfaceData[9].drawStringPos.x-tempTrans, mainInterfaceData[9].drawStringPos.y-tempTrans,144*scaleFac,144*scaleFac);
    } else {
        playIcon.draw(mainInterfaceData[9].drawStringPos.x-tempTrans, mainInterfaceData[9].drawStringPos.y-tempTrans,144*scaleFac,144*scaleFac);
    }
    
    ofSetColor(mainInterfaceData[10].displayColor);
    
    if(!synths[synthButton[2]].pause) {
        pauseIcon.draw(mainInterfaceData[10].drawStringPos.x-tempTrans, mainInterfaceData[10].drawStringPos.y-tempTrans,144*scaleFac,144*scaleFac);
    } else {
        playIcon.draw(mainInterfaceData[10].drawStringPos.x-tempTrans, mainInterfaceData[10].drawStringPos.y-tempTrans,144*scaleFac,144*scaleFac);
    }
    
    ofPopStyle();
    
    
}

#if !TARGET_OS_IPHONE

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    if (key == 's') {
        ofImage pix;
        pix.allocate(ofGetWidth(), ofGetHeight(), OF_IMAGE_COLOR);
        pix.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
        pix.saveImage( ofGetTimestampString()+"debug.png");
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
    
    
    if (key == '1') {
        synths[activeSynth].nextPulseDivision = 4;
    }
    
    if (key == '2') {
        synths[activeSynth].nextPulseDivision = 2;
    }
    if (key == '4') {
        synths[activeSynth].nextPulseDivision = 1;
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
    
    if (!interfaceMoving) {
        
        
        if (currentState == STATE_BPM) {
            
            if(mainInterfaceData[38].isInside(ofVec2f(x,y))) {
                if (!mainInterfaceData[38].touchDown){
                    mainInterfaceData[38].touchStart = ofVec2f(x,y);
                    mainInterfaceData[38].touchDown = true;
                }
                
                if (mainInterfaceData[38].touchDown) {
                    
                    int bpmMod = (mainInterfaceData[38].touchStart.x - x)/6;
                    mainInterfaceData[38].tempInt = bpmMod;
                    
                    if ( abs(bpmMod) > 0 ){
                        mainInterfaceData[38].touchStart.x = x;
                        bpm=ofClamp(bpm+bpmMod,1 ,1000);
                        tonicSynth.setParameter("BPM",bpm*4);
                        mainInterfaceData[38].elementName = "BPM "+ ofToString(bpm);
                    }
                    
                    cout <<     mainInterfaceData[38].touchStart.x << endl;
                    
                }
                
            }
            
            if (mainInterfaceData[55].isInside(ofVec2f(x,y))) {
                float value = ofClamp(ofMap(x, mainInterfaceData[55].minX, mainInterfaceData[55].maxX, 0.0, 1.0), 0.0, 1.0);
                if (value > 0.5){
                    mainInterfaceData[55].setSlider(mainInterface, 1.0);
                    synths[synthButton[0]].nextPulseDivision = 4;
                }else if (value <= 0.5 && value > 0.333 ){
                    mainInterfaceData[55].setSlider(mainInterface, 0.5);
                    synths[synthButton[0]].nextPulseDivision = 3;
                } else if (value <= 0.333 && value > 0.25 ){
                    mainInterfaceData[55].setSlider(mainInterface, 0.333);
                    synths[synthButton[0]].nextPulseDivision = 2;
                } else {
                    mainInterfaceData[55].setSlider(mainInterface, 0.25);
                    synths[synthButton[0]].nextPulseDivision = 1;
                }
                mainInterfaceData[55].elementName = "1/"+ofToString(5-synths[synthButton[0]].nextPulseDivision);
                mainInterfaceData[55].setStringWidth(mainInterfaceData[55].fsPtr->getBBox(mainInterfaceData[55].elementName, mainInterfaceData[55].fontSize, 0, 0).getWidth());
                
            }
            
            if (mainInterfaceData[56].isInside(ofVec2f(x,y))) {
                float value = ofClamp(ofMap(x, mainInterfaceData[56].minX, mainInterfaceData[56].maxX, 0.0, 1.0), 0.0, 1.0);
                if (value > 0.5){
                    mainInterfaceData[56].setSlider(mainInterface, 1.0);
                    synths[synthButton[1]].nextPulseDivision = 4;
                }else if (value <= 0.5 && value > 0.333 ){
                    mainInterfaceData[56].setSlider(mainInterface, 0.5);
                    synths[synthButton[1]].nextPulseDivision = 3;
                } else if (value <= 0.333 && value > 0.25 ){
                    mainInterfaceData[56].setSlider(mainInterface, 0.333);
                    synths[synthButton[1]].nextPulseDivision = 2;
                } else {
                    mainInterfaceData[56].setSlider(mainInterface, 0.25);
                    synths[synthButton[1]].nextPulseDivision = 1;
                }
                mainInterfaceData[56].elementName = "1/"+ofToString(5-synths[synthButton[1]].nextPulseDivision);
                mainInterfaceData[56].setStringWidth(mainInterfaceData[55].fsPtr->getBBox(mainInterfaceData[56].elementName, mainInterfaceData[56].fontSize, 0, 0).getWidth());
                
                
            }
            
            if (mainInterfaceData[57].isInside(ofVec2f(x,y))) {
                float value = ofClamp(ofMap(x, mainInterfaceData[57].minX, mainInterfaceData[57].maxX, 0.0, 1.0), 0.0, 1.0);
                if (value > 0.5){
                    mainInterfaceData[57].setSlider(mainInterface, 1.0);
                    synths[synthButton[2]].nextPulseDivision = 4;
                }else if (value <= 0.5 && value > 0.333 ){
                    mainInterfaceData[57].setSlider(mainInterface, 0.5);
                    synths[synthButton[2]].nextPulseDivision = 3;
                } else if (value <= 0.333 && value > 0.25 ){
                    mainInterfaceData[57].setSlider(mainInterface, 0.333);
                    synths[synthButton[2]].nextPulseDivision = 2;
                } else {
                    mainInterfaceData[57].setSlider(mainInterface, 0.25);
                    synths[synthButton[2]].nextPulseDivision = 1;
                }
                mainInterfaceData[57].elementName = "1/"+ofToString(5-synths[synthButton[2]].nextPulseDivision);
                mainInterfaceData[57].setStringWidth(mainInterfaceData[57].fsPtr->getBBox(mainInterfaceData[57].elementName, mainInterfaceData[57].fontSize, 0, 0).getWidth());
                
            }
            
        }
        
        if (currentState == STATE_VOLUME) {
            
            if (mainInterfaceData[1].isInside(ofVec2f(x,y))) {
                float value = ofClamp(ofMap(x, mainInterfaceData[1].minX, mainInterfaceData[1].maxX, 0.0, 1.0), 0.0, 1.0);
                mainInterfaceData[1].setSlider(mainInterface, value);
                synths[synthButton[0]].changeSynthVolume(value);
            }
            if (mainInterfaceData[2].isInside(ofVec2f(x,y))) {
                float value = ofClamp(ofMap(x, mainInterfaceData[2].minX, mainInterfaceData[2].maxX, 0.0, 1.0), 0.0, 1.0);
                mainInterfaceData[2].setSlider(mainInterface, value);
                synths[synthButton[1]].changeSynthVolume(value);
            }
            if (mainInterfaceData[3].isInside(ofVec2f(x,y))) {
                float value = ofClamp(ofMap(x, mainInterfaceData[3].minX, mainInterfaceData[3].maxX, 0.0, 1.0), 0.0, 1.0);
                mainInterfaceData[3].setSlider(mainInterface, value);
                synths[synthButton[2]].changeSynthVolume(value);
            }
            
            if (mainInterfaceData[0].isInside(ofVec2f(x,y))) {
                float value = ofClamp(ofMap(x, mainInterfaceData[0].minX, mainInterfaceData[0].maxX, 0.0, 1.0), 0.0, 1.0);
                mainInterfaceData[0].setSlider(mainInterface, value);
                volumeRampValueChanged(value);
            }
            
            
            
        }
        
        
        
        if (currentState == STATE_EDIT_DETAIL) {
            
            
            if(mainInterfaceData[49].isInside(ofVec2f(x,y))) {
                if (!mainInterfaceData[49].touchDown){
                    mainInterfaceData[49].touchStart = ofVec2f(x,y);
                    mainInterfaceData[49].touchDown = true;
                }
                
                if (mainInterfaceData[49].touchDown) {
                    
                    int keyMod = (mainInterfaceData[49].touchStart.x - x)/54;
                    mainInterfaceData[49].tempInt = keyMod;
                    
                    if ( abs(keyMod) > 0){
                        // if ( keyMod > 0 || synths[activeSynth].keyNote > 0) {
                        mainInterfaceData[49].touchStart.x = x;
                        synths[activeSynth].setKeyNote(ofClamp(keyMod,-1,1));
                        setNewGUI();
                        //cout <<  "midinote " << synths[activeSynth].keyNote << endl;
                        // } else {
                        
                        // cout << "not under 0 " << endl;
                        
                        //}
                    }
                    
                    // cout <<     mainInterfaceData[49].touchStart.x << endl;
                    
                    
                }
            }
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
    
    if (!interfaceMoving) {
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
    
    
    
    if (!interfaceMoving) {
        
        if (currentState == STATE_DEFAULT) {
            
            if( (x > designGrid[0][1].x-designGrid[0][0].x && x < designGrid[0][1].x+designGrid[0][0].x) &&
               (y > designGrid[0][1].y-designGrid[0][0].y && y < designGrid[0][1].y+designGrid[0][0].y) ){
                if(!synths[synthButton[0]].trackSwitchOn) {
                    synths[synthButton[0]].trackSwitchOn = true;
                } else {
                    buttonOnePress();
                }
            }
            
            if( (x > designGrid[1][1].x-designGrid[0][0].x && x < designGrid[1][1].x+designGrid[0][0].x) &&
               (y > designGrid[1][1].y-designGrid[0][0].y && y < designGrid[1][1].y+designGrid[0][0].y) ){
                buttonTwoPress();
            }
            
            if( (x > designGrid[2][1].x-designGrid[0][0].x && x < designGrid[2][1].x+designGrid[0][0].x) &&
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
            
            if( (x > designGrid[2][2].x-designGrid[0][0].x && x < designGrid[2][2].x+designGrid[0][0].x) &&
               (y > designGrid[2][2].y-designGrid[0][0].y +100 && y < designGrid[2][2].y+designGrid[0][0].y) ){
                if(synths[synthButton[2]].trackSwitchOn) {
                    buttonThreePress();
                } else {
                    synths[synthButton[2]].trackSwitchOn = true;
                }
            }
            
        }
        
        
    }
    
    if ( (!interfaceMoving) && (!insideSynth) ){
        
        
        if (currentState == STATE_DEFAULT) {
            
            if (mainInterfaceData[8].isInside(ofVec2f(x,y))) {
                mainInterfaceData[8].blinkOn();
                
                if (!synths[synthButton[0]].pause) {
                    synths[synthButton[0]].setSaturationOff();
                } else {
                    synths[synthButton[0]].setSaturationOn();
                }
                
                synths[synthButton[0]].pause = !synths[synthButton[0]].pause;
                
                
            }
            if (mainInterfaceData[9].isInside(ofVec2f(x,y))) {
                mainInterfaceData[9].blinkOn();
                
                if (!synths[synthButton[1]].pause) {
                    synths[synthButton[1]].setSaturationOff();
                } else {
                    synths[synthButton[1]].setSaturationOn();
                }
                
                synths[synthButton[1]].pause = !synths[synthButton[1]].pause;
                
            }
            if (mainInterfaceData[10].isInside(ofVec2f(x,y))) {
                mainInterfaceData[10].blinkOn();
                
                if (!synths[synthButton[2]].pause) {
                    synths[synthButton[2]].setSaturationOff();
                } else {
                    synths[synthButton[2]].setSaturationOn();
                }
                
                synths[synthButton[2]].pause = !synths[synthButton[2]].pause;
            }
            
            if (mainInterfaceData[37].isInside(ofVec2f(x,y))) {
                buttonFourPress();
                mainInterfaceData[37].blinkOn();
            }
            
            if (mainInterfaceData[41].isInside(ofVec2f(x,y))) {
                mainInterfaceData[41].blinkOn();
                bpmButtonPress();
            }
            if (mainInterfaceData[42].isInside(ofVec2f(x,y))) {
                mainInterfaceData[42].blinkOn();
                harmonyButtonPress();
            }
            
            
        }
        
        else  if (currentState == STATE_VOLUME) {
            
            if (mainInterfaceData[1].isInside(ofVec2f(x,y))) {
                float value = ofClamp(ofMap(x, mainInterfaceData[1].minX, mainInterfaceData[1].maxX, 0.0, 1.0), 0.0, 1.0);
                mainInterfaceData[1].setSlider(mainInterface, value);
                synths[synthButton[0]].changeSynthVolume(value);
            }
            if (mainInterfaceData[2].isInside(ofVec2f(x,y))) {
                float value = ofClamp(ofMap(x, mainInterfaceData[2].minX, mainInterfaceData[2].maxX, 0.0, 1.0), 0.0, 1.0);
                mainInterfaceData[2].setSlider(mainInterface, value);
                synths[synthButton[1]].changeSynthVolume(value);
            }
            if (mainInterfaceData[3].isInside(ofVec2f(x,y))) {
                float value = ofClamp(ofMap(x, mainInterfaceData[3].minX, mainInterfaceData[3].maxX, 0.0, 1.0), 0.0, 1.0);
                mainInterfaceData[3].setSlider(mainInterface, value);
                synths[synthButton[2]].changeSynthVolume(value);
            }
            
            if (mainInterfaceData[0].isInside(ofVec2f(x,y))) {
                float value = ofClamp(ofMap(x, mainInterfaceData[0].minX, mainInterfaceData[0].maxX, 0.0, 1.0), 0.0, 1.0);
                mainInterfaceData[0].setSlider(mainInterface, value);
                volumeRampValueChanged(value);
            }
            
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
            
            //synth
            if(  mainInterfaceData[7].isInside(ofVec2f(x,y))) {
                if (  (x-mainInterfaceData[7].minX) > ((mainInterfaceData[7].maxX-mainInterfaceData[7].minX)/2)) {
                    synths[activeSynth].changePreset(true);
                } else {
                    synths[activeSynth].changePreset(false);
                }
                setNewGUI();
                mainInterfaceData[7].blinkOn();
            }
            
            if(  mainInterfaceData[11].isInside(ofVec2f(x,y))) {
                buttonEditDetail();
                mainInterfaceData[11].blinkOn();
            }
            
            if(  mainInterfaceData[4].isInside(ofVec2f(x,y))) {
                synths[activeSynth].setKeyNote(-12);
                setNewGUI();
                mainInterfaceData[4].blinkOn();
            }
            if(  mainInterfaceData[6].isInside(ofVec2f(x,y))) {
                synths[activeSynth].setKeyNote(12);
                setNewGUI();
                mainInterfaceData[6].blinkOn();
            }
            
            for (int i = 1; i < 12; i++) {
                
                if (   mainInterfaceData[13+i].isInside(ofVec2f(x,y))) {
                    synths[activeSynth].changeMusicScale(i);
                    mainInterfaceData[13+i].switchColor();
                    mainInterfaceData[5].elementName = "INDIVIDUAL";
                    mainInterfaceData[5].setStringWidth(mainInterfaceData[5].fsPtr->getBBox(mainInterfaceData[5].elementName, mainInterfaceData[5].fontSize, 0, 0).getWidth());
                    synths[activeSynth].userScale = true;
                    
                    markScaleSteps(63);
                    cout <<   synths[activeSynth].activeScale.steps[i] <<endl;
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
        }
        
        else if (currentState == STATE_HARMONY) {
            
            if (mainInterfaceData[43].isInside(ofVec2f(x,y))) {
                harmonyButtonPress();
                mainInterfaceData[43].blinkOn();
            }
            
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
                        }                    }
                    // setNewGUI();
                    mainInterfaceData[58+i].elementName = ofToString(synths[synthButton[i]].keyNote)+" "+ofToString(notes[synths[synthButton[i]].keyNote%12]);
                    mainInterfaceData[58+i].setStringWidth(mainInterfaceData[58+i].fsPtr->getBBox(mainInterfaceData[58+i].elementName, mainInterfaceData[58+i].fontSize, 0, 0).getWidth());
                    
                    mainInterfaceData[58+i].blinkOn();
                }
            }
            
            
            
            if (mainInterfaceData[61].isInside(ofVec2f(x,y))) {
                if (  (x-mainInterfaceData[61].minX) > ((mainInterfaceData[61].maxX-mainInterfaceData[61].minX)/2)) {
                    globalKey = ofWrap(globalKey+1,0,12);
                } else {
                    globalKey = ofWrap(globalKey-1,0,12);
                }
                
                
                for (int i = 0; i < 3; i++) {
                    if(synths[synthButton[i]].globalHarmony){
                        synths[synthButton[i]].setKeyNote( ((synths[synthButton[i]].keyNote)-( (synths[synthButton[i]].keyNote/12)*12 +(globalKey%12) ))*-1 );
                    }
                }
                
                mainInterfaceData[61].elementName = ofToString(notes[globalKey%12]);
                mainInterfaceData[61].setStringWidth(mainInterfaceData[61].fsPtr->getBBox(mainInterfaceData[61].elementName, mainInterfaceData[61].fontSize, 0, 0).getWidth());
                mainInterfaceData[61].blinkOn();
                
                mainInterfaceData[58].elementName = ofToString(synths[synthButton[0]].keyNote)+" "+ofToString(notes[synths[synthButton[0]].keyNote%12]);
                mainInterfaceData[58].setStringWidth(mainInterfaceData[58].fsPtr->getBBox(mainInterfaceData[58].elementName, mainInterfaceData[58].fontSize, 0, 0).getWidth());
                mainInterfaceData[58].blinkOn();
                
                mainInterfaceData[59].elementName = ofToString(synths[synthButton[1]].keyNote)+" "+ofToString(notes[synths[synthButton[1]].keyNote%12]);
                mainInterfaceData[59].setStringWidth(mainInterfaceData[59].fsPtr->getBBox(mainInterfaceData[59].elementName, mainInterfaceData[59].fontSize, 0, 0).getWidth());
                mainInterfaceData[59].blinkOn();
                
                mainInterfaceData[60].elementName = ofToString(synths[synthButton[2]].keyNote)+" "+ofToString(notes[synths[synthButton[2]].keyNote%12]);
                mainInterfaceData[60].setStringWidth(mainInterfaceData[60].fsPtr->getBBox(mainInterfaceData[60].elementName, mainInterfaceData[60].fontSize, 0, 0).getWidth());
                mainInterfaceData[60].blinkOn();
            }
            
            if(  mainInterfaceData[62].isInside(ofVec2f(x,y))) {
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
                mainInterfaceData[62].setStringWidth(mainInterfaceData[62].fsPtr->getBBox(mainInterfaceData[62].elementName, mainInterfaceData[62].fontSize, 0, 0).getWidth());
                
                mainInterfaceData[62].blinkOn();
            }
            
        }
        
        else if (currentState == STATE_EDIT) {
            
            
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
            
            //back to State_default
            if(  mainInterfaceData[43].isInside(ofVec2f(x,y))) {
                buttonTwoPress();
                mainInterfaceData[43].blinkOn();
            }
            
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
            //synth
            if(  mainInterfaceData[7].isInside(ofVec2f(x,y))) {
                if (  (x-mainInterfaceData[7].minX) > ((mainInterfaceData[7].maxX-mainInterfaceData[7].minX)/2)) {
                    synths[activeSynth].changePreset(true);
                } else {
                    synths[activeSynth].changePreset(false);
                }
                setNewGUI();
                mainInterfaceData[7].blinkOn();
                
            }
            
            //toggle state_edit_detail
            if(  mainInterfaceData[44].isInside(ofVec2f(x,y))) {
                buttonEditDetail();
                mainInterfaceData[44].blinkOn();
            }
            
            //toggle save grid
            if(  mainInterfaceData[12].isInside(ofVec2f(x,y))) {
                //leer
                muster.saveReady = true;
                mainInterfaceData[12].blinkOn();
            }
            
            //toggle synth-> globalHarmony
            if(  mainInterfaceData[111].isInside(ofVec2f(x,y))) {
                synths[activeSynth].globalHarmony = !synths[activeSynth].globalHarmony;
                mainInterfaceData[111].blinkOn();
                mainInterfaceData[111].switchColor();
            }
            
            
            //toggle get random grid
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
            
            //pulse direction
            for (int i =  45; i < 45+4; i++) {
                if ( mainInterfaceData[i].isInside(ofVec2f(x,y))) {
                    if(synths[activeSynth].connectedDirection[i-45] && synths[activeSynth].activeDirection[i-45] ) {
                        synths[activeSynth].connectedDirection[i-45] =  false;
                        synths[activeSynth].pulsePlane.pulseDir( abs(45-i));
                        
                    } else if (!synths[activeSynth].connectedDirection[i-45] && synths[activeSynth].activeDirection[i-45]) {
                        synths[activeSynth].activeDirection[i-45] =  false;
                        synths[activeSynth].pulsePlane.pulseDir( abs(45-i));
                        
                    } else {
                        synths[activeSynth].connectedDirection[i-45] =  true;
                        synths[activeSynth].activeDirection[i-45] =  true;
                        synths[activeSynth].pulsePlane.pulseDir( abs(45-i));
                        
                    }
                }
            }
            
        }
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
    if (mainInterfaceData[38].touchDown){
        mainInterfaceData[38].touchDown = !mainInterfaceData[38].touchDown;
    }
    
    if (mainInterfaceData[49].touchDown){
        mainInterfaceData[49].touchDown = !mainInterfaceData[49].touchDown;
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
    saveToXml();
    //volumeRampValueChanged(mainVol);
    
    //    mainVol = 0.0;
    
    cout << "lost focus"  << endl;
}

//--------------------------------------------------------------
void ofApp::gotFocus(){
    // startUp = true;
    //volumeRestartTarget = mainVol;
    //volumeRestart = 0.0;
    //tonicSynth.setParameter("mainVolumeRamp",Tonic::mapLinToLog(0.0,0.0,1.0));
    
    cout <<"gotfocus" << endl;
}

//--------------------------------------------------------------
void ofApp::gotMemoryWarning(){
    
}

//--------------------------------------------------------------
void ofApp::deviceOrientationChanged(int newOrientation){
    
}
#endif

#if !TARGET_OS_IPHONE
//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    makeDesignGrid();
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
    
    
    for (int i = 1; i < 3; i++) {
        
        ofLine(0, (ofGetHeight()/3)*i, ofGetWidth(), (ofGetHeight()/3)*i );
        ofLine(ofGetWidth()/3*i, 0, ofGetWidth()/3*i, ofGetHeight());
    }
    
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            ofEllipse(designGrid[i][j], 10, 10);
        }
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
    tempNode.setPosition(0, -TILES*TILESIZE*2, TILES*TILESIZE*7);
    tempNode.lookAt(ofVec3f(0,0,0)-tempNode.getZAxis());
    ofVec3f wMouse = tempNode.screenToWorld( ofVec3f(target_.x,target_.y,0.0), ofRectangle(ofPoint(0,0), ofGetWidth(), ofGetHeight()));
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
    if (currentState == STATE_BPM) {
        for (int i = 0; i < 3; i++) {
            if (synths[synthButton[i]].pulseDivision == div) {
                mainInterfaceData[55+i].blinkOn();
            }
        }
    }
    
    if(currentState == STATE_HARMONY){
        if (synths[synthButton[0]].pulseDivision == div) {
            hvSlotA.update(synths[synthButton[0]].noteHistory, mainInterfaceData);
        }
        if (synths[synthButton[1]].pulseDivision == div) {
            hvSlotB.update(synths[synthButton[1]].noteHistory, mainInterfaceData);
        }
        if (synths[synthButton[2]].pulseDivision == div) {
            hvSlotC.update(synths[synthButton[2]].noteHistory, mainInterfaceData);
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
            
            for (int j = 0; j < 5; j++) {
                synths[i].bpmTick+= synths[i].tickTimes.at(j);
            }
            
            
            synths[i].bpmTick /= 5;
            
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




void ofApp::volumeRampValueChanged(float & volumeRampValue) {
    
    mainVol = volumeRampValue;
    
    // cout << "main " << Tonic::mapLinToLog(mainVol,0.0,1.0) << endl;
    tonicSynth.setParameter("mainVolumeRamp",Tonic::mapLinToLog(mainVol,0.0,1.0));
    
}

void ofApp::audioRequested (float * output, int bufferSize, int nChannels){
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
        ofVec3f tempPos =  (camUsePath.getVertices().at((int)index+1)-camUsePath.getVertices().at((int)index))* (index-floor(index));
        testCam.setPosition( camUsePath.getVertices().at((int)index)+ tempPos);
        
        testCam.setFov(ofLerp(camDefaultFov, camTargetFov, pct_));
        
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
    
    cam.setNearClip(10);
    cam.setFarClip(51000);
    cam.setFov(20);
    
    
    //startcam settings
    testCam.setNearClip(nearClip);
    testCam.setFarClip(farClip);
    
    float bezierHandleFac = 2.5;
    
    //___---___
    //---___---
    
    //synthlayer and camera for active edit position
    synthActivePos.setPosition(0, -TILES*TILESIZE*4, TILESIZE*TILES*3.3);
    camActiveSynth.setPosition(synthActivePos.getGlobalPosition()+ofVec3f(0,-TILESIZE*TILES*5.5,TILES*TILESIZE*2.1));
    camNotActiveSynth.setPosition(0, -TILES*TILESIZE*2, TILES*TILESIZE*7);
    camEditDetailPos.setPosition(camActiveSynth.getPosition()+ofVec3f(0,0,(TILESIZE*TILES)/2.55));
    
    camActiveSynth.lookAt(synthActivePos.getPosition() - camActiveSynth.getZAxis());
    camNotActiveSynth.lookAt(synthPos[1].getPosition() - camNotActiveSynth.getZAxis());
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
    twoToActive.bezierTo(synthPos[1].getPosition()+ofVec3f(0,0,TILES*TILESIZE*bezierHandleFac*1.2), synthActivePos.getPosition()+ofVec3f(0,TILES*TILESIZE*bezierHandleFac,0), synthActivePos.getPosition());
    twoToActive = twoToActive.getResampledByCount(80);
    
    oneToActive.addVertex(synthPos[0].getPosition());
    oneToActive.bezierTo(synthPos[0].getPosition()+ofVec3f(0,0,TILES*TILESIZE*bezierHandleFac*1.2),synthActivePos.getPosition()+ofVec3f(0,TILES*TILESIZE*bezierHandleFac,0), synthActivePos.getPosition());
    oneToActive = oneToActive.getResampledByCount(80);
    
    threeToActive.addVertex(synthPos[2].getPosition());
    threeToActive.bezierTo(synthPos[2].getPosition()+ofVec3f(0,0,TILESIZE*TILES*bezierHandleFac*1.2),synthActivePos.getPosition()+ofVec3f(0,TILES*TILESIZE*bezierHandleFac,0), synthActivePos.getPosition());
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
    camPath.bezierTo(camNotActiveSynth.getPosition()+ofVec3f(0,-TILES*TILESIZE*bezierHandleFac,0), camActiveSynth.getPosition()+ofVec3f(0,0,TILESIZE*TILES*bezierHandleFac) , camActiveSynth.getPosition());
    camPath = camPath.getResampledByCount(80);
    
    camPathBack.addVertex(camActiveSynth.getPosition());
    camPathBack.bezierTo(camActiveSynth.getPosition()+ofVec3f(0,TILESIZE*TILES*bezierHandleFac,0), camNotActiveSynth.getPosition()+ofVec3f(0,-TILES*TILESIZE*bezierHandleFac,0), camNotActiveSynth.getPosition());
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
    
    //___---___
    //---___---
    
    //from default to volume
    volumeMatrix.rotate(45, -1, 0, 0);
    
    TwoVolumeLayerPathOn.addVertex(ofVec3f(0,0,0));
    TwoVolumeLayerPathOn.bezierTo(ofVec3f(0,0,-(TILESIZE*TILES)/4), ofVec3f(0,(TILES*TILESIZE)/4,-(TILES*TILESIZE)/2), ofVec3f(0,-(TILESIZE*TILES)*0.75,-(TILESIZE*TILES)/2));
    // TwoVolumeLayerPathOn = TwoVolumeLayerPathOn.getResampledByCount(80);
    
    TwoVolumeLayerPathOff.addVertex(ofVec3f(0,-(TILESIZE*TILES)*0.75,-(TILESIZE*TILES)/2));
    TwoVolumeLayerPathOff.bezierTo(ofVec3f(0,(TILES*TILESIZE)/4,-(TILES*TILESIZE)/2), ofVec3f(0,0,-(TILESIZE*TILES)/4), ofVec3f(0,0,0));
    // TwoVolumeLayerPathOff = TwoVolumeLayerPathOff.getResampledByCount(80);
    
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
    
    //time mode
    timeMatrix.rotate(-16,-1,0,0);
    
    TwoTimePathOn.addVertex(ofVec3f(0,0,0));
    TwoTimePathOn.bezierTo(ofVec3f(0,0,-(TILESIZE*TILES)/4), ofVec3f(0,(TILES*TILESIZE)/4,-(TILES*TILESIZE)/2), ofVec3f(0,-(TILESIZE*TILES)*0.5,-(TILESIZE*TILES)*0.45));
    
    TwoTimePathOff.addVertex(ofVec3f(0,-(TILESIZE*TILES)*0.5,-(TILESIZE*TILES)*0.45));
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
    
    //harmony mode
    harmonyMatrix.rotate(75,-1,0,0);
    
    TwoHarmonyPathOn.addVertex(ofVec3f(0,0,0));
    TwoHarmonyPathOn.bezierTo(ofVec3f(0,0,-(TILESIZE*TILES)/4), ofVec3f(0,(TILES*TILESIZE)/4,-(TILES*TILESIZE)/2), ofVec3f(0,-(TILESIZE*TILES)*1,-(TILESIZE*TILES)*0.45));
    
    TwoHarmonyPathOff.addVertex(ofVec3f(0,-(TILESIZE*TILES)*1,-(TILESIZE*TILES)*0.45));
    TwoHarmonyPathOff.bezierTo(ofVec3f(0,(TILES*TILESIZE)/4,-(TILES*TILESIZE)/2), ofVec3f(0,0,-(TILESIZE*TILES)/4), ofVec3f(0,0,0));
    
    OneHarmonyPathOn = TwoHarmonyPathOn;
    OneHarmonePathOff = TwoHarmonyPathOff;
    ThreeHarmonyPathOn = TwoHarmonyPathOn;
    ThreeHarmonyPathOff = TwoHarmonyPathOff;
    
    for (int i = 0; i < TwoHarmonyPathOn.size() ; i++) {
        OneHarmonyPathOn.getVertices().at(i) = TwoHarmonyPathOn.getVertices().at(i)+synthPos[0].getPosition();
        OneHarmonePathOff.getVertices().at(i) = TwoHarmonyPathOff.getVertices().at(i)+synthPos[0].getPosition();
        ThreeHarmonyPathOn.getVertices().at(i) = TwoHarmonyPathOn.getVertices().at(i)+synthPos[2].getPosition();
        ThreeHarmonyPathOff.getVertices().at(i) = TwoHarmonyPathOff.getVertices().at(i)+synthPos[2].getPosition();
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
    
    
    int fontDefault = designGrid[0][0].y*0.38;
    int fontSmall = designGrid[0][0].y*0.165;
    
    ofVec3f smallButton = ofVec3f(designGrid[0][0].y,designGrid[0][0].y,0);
    ofVec3f horizontalSlider = ofVec3f(designGrid[0][0].x*2,designGrid[0][0].y,0);
    ofVec3f verticalSlider = ofVec3f(designGrid[0][0].x*2/12,designGrid[0][0].y*2,0);
    ofVec3f keyNoteButton = ofVec3f(designGrid[0][0].x*6/14 ,designGrid[0][0].y,0);
    ofVec3f scaleButton = ofVec3f(designGrid[0][0].x*6/14 ,designGrid[0][0].y,0);
    
    
    ofVec3f place = ofVec3f(0,0,0);
    ofVec3f offPlace = ofVec3f(0,-designGrid[0][0].y*12,0);
    GlobalGUI temp = GlobalGUI(0,string("GLOBAL"),ofVec3f(horizontalSlider.x*0.9, horizontalSlider.y,0), ofColor(50,0,0),place,offPlace,fontDefault,false,&robotoCon);
    mainInterfaceData.push_back(temp);
    
    place = ofVec3f(0,-designGrid[0][0].y/2,0);
    offPlace = ofVec3f(0,-designGrid[0][0].y*6,0);
    temp = GlobalGUI(1,string("SLOT A"),ofVec3f(horizontalSlider.x*0.9, horizontalSlider.y,0),ofColor(51,0,0),place,offPlace,fontDefault,false,&robotoCon);
    mainInterfaceData.push_back(temp);
    temp = GlobalGUI(2,string("SLOT B"),ofVec3f(horizontalSlider.x*0.9, horizontalSlider.y,0),ofColor(52,0,0),place,offPlace,fontDefault,false,&robotoCon);
    mainInterfaceData.push_back(temp);
    temp = GlobalGUI(3,string("SLOT C"),ofVec3f(horizontalSlider.x*0.9, horizontalSlider.y,0),ofColor(53,0,0),place,offPlace,fontDefault,false,&robotoCon);
    mainInterfaceData.push_back(temp);
    
    //octave down icon
    place = ofVec3f(+scaleButton.x/2,-(scaleButton.y/2),0);
    offPlace = ofVec3f(0, -designGrid[0][0].y*12,0);
    temp = GlobalGUI(4,string(""),ofVec3f(scaleButton.x,scaleButton.y+keyNoteButton.y,0),ofColor(54,0,0),place,offPlace,fontDefault,true,&robotoBold);
    mainInterfaceData.push_back(temp);
    
    //detail view, scaleinfo background
    offPlace = ofVec3f(-designGrid[0][0].x*6,0,0);
    place = ofVec3f((horizontalSlider.x*0.86)/2,-designGrid[0][0].y+((horizontalSlider.y/2)*0.8),0);
    temp = GlobalGUI(5,string("activeScale"),ofVec3f(horizontalSlider.x*0.86,horizontalSlider.y*0.8,0),ofColor(55,0,0),place,offPlace,fontDefault,false,&robotoLight);
    mainInterfaceData.push_back(temp);
    
    //octave up icon
    place = ofVec3f(-scaleButton.x/2, -(scaleButton.y/2),0);
    offPlace = ofVec3f(0, -designGrid[0][0].y*12,0);
    temp = GlobalGUI(6,string(""),ofVec3f(scaleButton.x,scaleButton.y+keyNoteButton.y,0),ofColor(56,0,0),place,offPlace,fontDefault,true,&robotoBold);
    mainInterfaceData.push_back(temp);
    
    offPlace = ofVec3f(-designGrid[0][0].x*6,0,0);
    place = ofVec3f((horizontalSlider.x*0.86)/2,+designGrid[0][0].y-((horizontalSlider.y*0.8)/2),0);
    temp = GlobalGUI(7,string(presetNames.at(0)),ofVec3f(horizontalSlider.x*0.86,horizontalSlider.y*0.8,0),ofColor(57,0,0),place,offPlace,fontDefault,false,&robotoBold);
    mainInterfaceData.push_back(temp);
    
    
    place = ofVec3f(0,designGrid[0][0].y/2,0);
    offPlace = ofVec3f(designGrid[0][0].x*12,0,0);
    temp = GlobalGUI(8,string("OnePlayPause"),smallButton*0.5,ofColor(59,0,0),place,offPlace,fontDefault,true,&robotoCon);
    mainInterfaceData.push_back(temp);
    offPlace = ofVec3f(designGrid[0][0].x*6,0,0);
    temp = GlobalGUI(9,string("TwoPlayPause"),smallButton*0.5,ofColor(60,0,0),place,offPlace,fontDefault,true,&robotoCon);
    mainInterfaceData.push_back(temp);
    offPlace = ofVec3f(designGrid[0][0].x*4,0,0);
    temp = GlobalGUI(10,string("ThreePlayPause"),smallButton*0.5,ofColor(61,0,0),place,offPlace,fontDefault,true,&robotoCon);
    mainInterfaceData.push_back(temp);
    
    
    //toggle detail off
    place = ofVec3f(0,designGrid[0][0].y/2,0);
    offPlace = ofVec3f(0,designGrid[0][0].y*6,0);
    temp = GlobalGUI(11,string(""),smallButton,ofColor(62,0,0),place,offPlace,fontDefault,true,&robotoCon);
    mainInterfaceData.push_back(temp);
    
    
    //save to presets button
    place = ofVec3f(-designGrid[0][0].y/2,-designGrid[0][0].y/2,0);
    offPlace = ofVec3f(0,-designGrid[0][0].y*4,0);
    temp = GlobalGUI(12,string(""),smallButton,ofColor(63,0,0),place,offPlace,fontDefault,true,&robotoCon);
    mainInterfaceData.push_back(temp);
    
    
    
    
    
    //scale
    offPlace = ofVec3f(0,-designGrid[0][0].y*12,0);
    for (int i = 0; i < 12; i++) {
        place = ofVec3f( (scaleButton.x*(i+1))+(scaleButton.x/2),-(scaleButton.y/2)-(keyNoteButton.y/2),0);
        temp = GlobalGUI(13+i,notes[i],scaleButton,ofColor(0,0,0),place,offPlace,fontDefault*1.2,false,&robotoCon);
        mainInterfaceData.push_back(temp);
        
    }
    //keynote
    offPlace = ofVec3f(0,-designGrid[0][0].y*6,0);
    for (int i = 0; i < 12; i++) {
        place = ofVec3f( (keyNoteButton.x*(i+1))+(keyNoteButton.x/2),0,0);
        temp = GlobalGUI(25+i,notes[i],keyNoteButton,ofColor(0,0,0),place, offPlace, fontSmall*1.3,false,&robotoCon);
        mainInterfaceData.push_back(temp);
    }
    
    place = ofVec3f(0,-designGrid[0][0].y/2,0);
    offPlace = ofVec3f(-designGrid[0][0].x*6,0,0);
    temp = GlobalGUI(37, string(""), smallButton, ofColor(23,23,23), place, offPlace,fontDefault,true,&robotoCon);
    mainInterfaceData.push_back(temp);
    
    place = ofVec3f(0,0,0);
    offPlace = ofVec3f(0,-designGrid[0][0].y*12,0);
    temp = GlobalGUI(38,"BPM "+ string(ofToString(bpm)), ofVec3f(horizontalSlider.x*0.9, horizontalSlider.y,0), ofColor(23,23,23), place, offPlace,fontDefault*1.2,false,&robotoCon);
    mainInterfaceData.push_back(temp);
    
    
    offPlace = ofVec3f(+designGrid[0][0].x*6,0,0);
    place = ofVec3f(-(horizontalSlider.x*0.86)/2,(designGrid[0][0].y*2)/2,0);
    temp = GlobalGUI(39,string("Container"),ofVec3f( horizontalSlider.x*0.86,designGrid[0][0].y*2,0),ofColor(55,0,0),place,offPlace,fontDefault,true,&robotoLight);
    mainInterfaceData.push_back(temp);
    
    //toggle 1,2,3
    
    //make random grid icon
    place = ofVec3f(designGrid[0][0].y/2,-designGrid[0][0].y/2,0);
    offPlace = ofVec3f(0,-designGrid[0][0].y*4,0);
    temp = GlobalGUI(40,string(""), smallButton ,ofColor(63,0,0),place,offPlace,fontDefault,true,&robotoCon);
    mainInterfaceData.push_back(temp);
    
    
    //toggle bpm icon
    place = ofVec3f(0,-designGrid[0][0].y/2,0);
    offPlace = ofVec3f(designGrid[0][0].x*6,0,0);
    temp = GlobalGUI(41, string(""), smallButton, ofColor(23,23,23), place, offPlace,fontSmall,true,&robotoCon);
    mainInterfaceData.push_back(temp);
    
    //toggle harmony icon
    place = ofVec3f(0,-designGrid[0][0].y/2,0);
    offPlace = ofVec3f(0,-designGrid[0][0].y*6,0);
    temp = GlobalGUI(42, string(""), smallButton, ofColor(23,23,23), place, offPlace,fontSmall,true,&robotoCon);
    mainInterfaceData.push_back(temp);
    
    
    //toogle from state_edit to state_default
    
    place = ofVec3f(0,designGrid[0][0].y/2,0);
    offPlace = ofVec3f(0,designGrid[0][0].y*6,0);
    temp = GlobalGUI(43, string(""), smallButton, ofColor(23,23,23), place, offPlace,fontSmall,true,&robotoCon);
    mainInterfaceData.push_back(temp);
    
    //toggle edit detail
    
    place = ofVec3f(designGrid[0][0].y/2,-designGrid[0][0].y/2,0);
    offPlace = ofVec3f(0,-designGrid[0][0].y*4,0);
    temp = GlobalGUI(44, string(""), smallButton, ofColor(23,23,23), place, offPlace,fontSmall,true,&robotoCon);
    mainInterfaceData.push_back(temp);
    
    
    //scandirection toogle,
    
    place = ofVec3f(-designGrid[0][0].y*1.45,0,0);
    offPlace = ofVec3f(-designGrid[0][0].x*6,0,0);
    temp = GlobalGUI(45, string("west"), ofVec3f(60,designGrid[0][0].y*2.1,0), ofColor(23,23,23), place, offPlace,fontSmall,true,&robotoLight);
    mainInterfaceData.push_back(temp);
    
    place = ofVec3f(0,-designGrid[0][0].y*1.45,0);
    offPlace = ofVec3f(0, -designGrid[0][0].y*6,0);
    temp = GlobalGUI(46, string("north"), ofVec3f(designGrid[0][0].y*2.1,60,0), ofColor(23,23,23), place, offPlace,fontSmall,true,&robotoLight);
    mainInterfaceData.push_back(temp);
    
    place = ofVec3f(designGrid[0][0].y*1.45,0,0);
    offPlace = ofVec3f(designGrid[0][0].x*6,0,0);
    temp = GlobalGUI(47, string("east"), ofVec3f(60,designGrid[0][0].y*2.1,0), ofColor(23,23,23), place, offPlace,fontSmall,true,&robotoLight);
    mainInterfaceData.push_back(temp);
    
    
    place = ofVec3f(0,designGrid[0][0].y*1.45,0);
    offPlace = ofVec3f(0 ,designGrid[0][0].y*6,0);
    temp = GlobalGUI(48, string("south"), ofVec3f(designGrid[0][0].y*2.1,60,0), ofColor(23,23,23), place, offPlace,fontSmall,true,&robotoLight);
    mainInterfaceData.push_back(temp);
    
    
    place = ofVec3f(0,0,0);
    offPlace = ofVec3f(0 ,-designGrid[0][0].y*6,0);
    temp = GlobalGUI(49, string("keynote_slider"), ofVec3f(designGrid[0][0].x*6/14*12,keyNoteButton.y,0), ofColor(23,23,23), place, offPlace,fontSmall,true,&robotoCon);
    mainInterfaceData.push_back(temp);
    
    place = ofVec3f( (scaleButton.x*(0+1))+(scaleButton.x/2),-(scaleButton.y/2)-(keyNoteButton.y/2),0);
    offPlace = ofVec3f(0,-designGrid[0][0].y*12,0);
    temp = GlobalGUI(50, string("KEY"), scaleButton,ofColor(0,0,0),place,offPlace,fontSmall*1.2,false,&robotoCon);
    mainInterfaceData.push_back(temp);
    mainInterfaceData[50].elementColorOn = ofColor(255,255,255,100);
    
    place = ofVec3f(0,-designGrid[0][0].y/2,0);
    offPlace = ofVec3f(0,-designGrid[0][0].y*6,0);
    temp = GlobalGUI(51,"",ofVec3f(horizontalSlider.x*0.9, horizontalSlider.y,0),ofColor(51,0,0),place,offPlace,fontDefault,false,&robotoCon);
    mainInterfaceData.push_back(temp);
    mainInterfaceData[51].elementColorOn = ofColor(255,255,255,100);
    
    temp = GlobalGUI(52,"",ofVec3f(horizontalSlider.x*0.9, horizontalSlider.y,0),ofColor(52,0,0),place,offPlace,fontDefault,false,&robotoCon);
    mainInterfaceData.push_back(temp);
    mainInterfaceData[52].elementColorOn = ofColor(255,255,255,100);
    
    temp = GlobalGUI(53,"",ofVec3f(horizontalSlider.x*0.9, horizontalSlider.y,0),ofColor(53,0,0),place,offPlace,fontDefault,false,&robotoCon);
    mainInterfaceData.push_back(temp);
    mainInterfaceData[53].elementColorOn = ofColor(255,255,255,100);
    
    place = ofVec3f(0,0,0);
    offPlace = ofVec3f(0,-designGrid[0][0].y*12,0);
    temp = GlobalGUI(54,"",ofVec3f(horizontalSlider.x*0.9, horizontalSlider.y,0),ofColor(51,0,0),place,offPlace,fontDefault,false,&robotoCon);
    mainInterfaceData.push_back(temp);
    
    
    place = ofVec3f(0,-designGrid[0][0].y/2,0);
    offPlace = ofVec3f(0,-designGrid[0][0].y*6,0);
    temp = GlobalGUI(55,string("BPM A"),ofVec3f(horizontalSlider.x*0.9, horizontalSlider.y,0),ofColor(51,0,0),place,offPlace,fontDefault,false,&robotoCon);
    mainInterfaceData.push_back(temp);
    temp = GlobalGUI(56,string("BPM B"),ofVec3f(horizontalSlider.x*0.9, horizontalSlider.y,0),ofColor(52,0,0),place,offPlace,fontDefault,false,&robotoCon);
    mainInterfaceData.push_back(temp);
    temp = GlobalGUI(57,string("BPM C"),ofVec3f(horizontalSlider.x*0.9, horizontalSlider.y,0),ofColor(53,0,0),place,offPlace,fontDefault,false,&robotoCon);
    mainInterfaceData.push_back(temp);
    
    //harmony settings, a,b,c keynote
    
    offPlace = ofVec3f(0,-designGrid[0][0].y*6,0);
    place = ofVec3f(0,designGrid[0][0].y+horizontalSlider.y/4,0);
    temp = GlobalGUI(58,ofToString(synths[synthButton[0]].keyNote),ofVec3f(horizontalSlider.x,horizontalSlider.y/2,0),ofColor(57,0,0),place,offPlace,fontSmall,false,&robotoBold);
    mainInterfaceData.push_back(temp);
    
    offPlace = ofVec3f(0,-designGrid[0][0].y*6,0);
    place = ofVec3f(0,designGrid[0][0].y+horizontalSlider.y/4,0);
    temp = GlobalGUI(59,ofToString(synths[synthButton[1]].keyNote),ofVec3f(horizontalSlider.x,horizontalSlider.y/2,0),ofColor(57,0,0),place,offPlace,fontSmall,false,&robotoBold);
    mainInterfaceData.push_back(temp);
    
    offPlace = ofVec3f(0,-designGrid[0][0].y*6,0);
    place = ofVec3f(0,designGrid[0][0].y+horizontalSlider.y/4,0);
    temp = GlobalGUI(60,ofToString(synths[synthButton[2]].keyNote),ofVec3f(horizontalSlider.x,horizontalSlider.y/2,0),ofColor(57,0,0),place,offPlace,fontSmall,false,&robotoBold);
    mainInterfaceData.push_back(temp);
    
    //harmony menu, keynote
    offPlace = ofVec3f(0,-designGrid[0][0].y*6,0);
    place = ofVec3f(0,0,0);
    temp = GlobalGUI(61,ofToString(notes[globalKey%12]),ofVec3f(horizontalSlider.x,horizontalSlider.y,0),ofColor(57,0,0),place,offPlace,fontDefault,false,&robotoBold);
    mainInterfaceData.push_back(temp);
    
    //harmony menu -> global scale
    offPlace = ofVec3f(0,-designGrid[0][0].y*6,0);
    place = ofVec3f(0,0,0);
    temp = GlobalGUI(62,scaleCollection.scaleVec.at(globalScaleVecPos%scaleCollection.scaleVec.size()).name,ofVec3f(horizontalSlider.x,horizontalSlider.y,0),ofColor(57,0,0),place,offPlace,fontDefault,false,&robotoBold);
    mainInterfaceData.push_back(temp);
    
    
    //active scale notes display
    
    for (int i= 0; i < 12; i++) {
        float offset = ((horizontalSlider.x*0.86)/12);
        offPlace = ofVec3f(-designGrid[0][0].x*6- ((offset*i) + (offset/2) ),0,0);
        place = ofVec3f( (offset*i) + (offset/2),-designGrid[0][0].y+((horizontalSlider.y/2)*0.8),0);
        temp = GlobalGUI(63+i,"o",ofVec3f(offset,horizontalSlider.y*0.8,0),ofColor(57*i,0,0),place,offPlace,fontDefault,false,&robotoBold);
        mainInterfaceData.push_back(temp);
    }
    
    //harmony menu scale note display
    
    float offset = designGrid[0][0].x/6;
    
    for (int i= 0; i < 12; i++) {
        offPlace = ofVec3f(0,-designGrid[0][0].y*6,0);
        place = ofVec3f( (-horizontalSlider.x/2) + (offset*i) + (offset/2),0,0);
        temp = GlobalGUI(75+i,"o",ofVec3f(offset,designGrid[0][0].y*1.33,0),ofColor(57*i,0,0),place,offPlace,fontDefault,false,&robotoBold);
        mainInterfaceData.push_back(temp);
    }
    
    for (int i= 0; i < 12; i++) {
        offPlace = ofVec3f(0,-designGrid[0][0].y*6,0);
        place = ofVec3f( (-horizontalSlider.x/2) + (offset*i) + (offset/2),0,0);
        temp = GlobalGUI(87+i,"o",ofVec3f(offset,designGrid[0][0].y*1.33,0),ofColor(57*i,0,0),place,offPlace,fontDefault,false,&robotoBold);
        mainInterfaceData.push_back(temp);
    }
    
    for (int i= 0; i < 12; i++) {
        offPlace = ofVec3f(0,-designGrid[0][0].y*6,0);
        place = ofVec3f( (-horizontalSlider.x/2) + (offset*i) + (offset/2),0,0);
        temp = GlobalGUI(99+i,"o",ofVec3f(offset,designGrid[0][0].y*1.33,0),ofColor(57*i,0,0),place,offPlace,fontDefault,false,&robotoBold);
        mainInterfaceData.push_back(temp);
    }
    
    //toggle global harmony
    place = ofVec3f(-designGrid[0][0].y/2,-designGrid[0][0].y/2,0);
    offPlace = ofVec3f(0,-designGrid[0][0].y*4,0);
    temp = GlobalGUI(111, string(""), smallButton, ofColor(23,23,23), place, offPlace,fontSmall,false,&robotoCon);
    mainInterfaceData.push_back(temp);
    
    mainInterface.setMode(OF_PRIMITIVE_TRIANGLES);
    
    for (int i = 0; i < mainInterfaceData.size(); i++) {
        
        for (int j = 0; j < 4; j++) {
            mainInterface.addVertex(ofVec3f(0,0,0));
            mainInterfaceFbo.addVertex(ofVec3f(0,0,0));
            mainInterface.addColor(mainInterfaceData[i].elementColorOn);
            mainInterfaceFbo.addColor(ofColor(50+i,0,0));
        }
        
        for (int j = 0; j < 6; j++) {
            mainInterface.addIndex(mainInterfaceData.at(i).index[j]+(4*i));
        }
        
        mainInterfaceData[i].setStringWidth(mainInterfaceData[i].fsPtr->getBBox(mainInterfaceData[i].elementName, mainInterfaceData[i].fontSize, 0, 0).getWidth());
        mainInterfaceData[i].stringHeight = mainInterfaceData[i].fsPtr->getBBox("o", mainInterfaceData[i].fontSize, 0, 0).getHeight()*0.7;
        
    }
    
    //offset for scale key string
    mainInterfaceData[50].stringHeight-=scaleButton.y/3;
    
    
    pauseInterfaceOn();
    aniPct = 0.0;
}

//--------------------------------------------------------------


void ofApp::editInterfaceOn(){
    
    mainInterfaceData[5].showString = true;
    mainInterfaceData[5].moveDir = 1;
    mainInterfaceData[5].animation = true;
    
    
    mainInterfaceData[7].showString = true;
    mainInterfaceData[7].moveDir = 1;
    mainInterfaceData[7].animation = true;
    
    mainInterfaceData[39].showString = false;
    mainInterfaceData[39].moveDir = 1;
    mainInterfaceData[39].animation = true;
    
    mainInterfaceData[40].showString = false;
    mainInterfaceData[40].moveDir = 1;
    mainInterfaceData[40].animation = true;
    
    mainInterfaceData[12].showString = false;
    mainInterfaceData[12].moveDir = 1;
    mainInterfaceData[12].animation = true;
    
    mainInterfaceData[43].showString = false;
    mainInterfaceData[43].moveDir = 1;
    mainInterfaceData[43].animation = true;
    
    mainInterfaceData[44].showString = false;
    mainInterfaceData[44].moveDir = 1;
    mainInterfaceData[44].animation = true;
    
    mainInterfaceData[111].showString = false;
    mainInterfaceData[111].moveDir = 1;
    mainInterfaceData[111].animation = true;
    
    
    for (int i = 45; i < 45+4; i++) {
        mainInterfaceData[i].moveDir = 1;
        mainInterfaceData[i].animation = true;
    }
    
    if (currentState != STATE_EDIT && currentState != STATE_EDIT_DETAIL ) {
        mainInterfaceData[37].moveDir = 0;
        mainInterfaceData[37].animation = true;
    }
    
    for (int i = 0; i < 12; i++) {
        mainInterfaceData[63+i].moveDir = 1;
        mainInterfaceData[63+i].animation = true;
    }
    
}

//--------------------------------------------------------------

void ofApp::editInterfaceOff(){
    
    
    mainInterfaceData[7].moveDir = 0;
    mainInterfaceData[7].animation = true;
    
    
    mainInterfaceData[5].moveDir = 0;
    mainInterfaceData[5].animation = true;
    
    mainInterfaceData[39].moveDir = 0;
    mainInterfaceData[39].animation = true;
    
    mainInterfaceData[40].moveDir = 0;
    mainInterfaceData[40].animation = true;
    
    mainInterfaceData[12].moveDir = 0;
    mainInterfaceData[12].animation = true;
    
    mainInterfaceData[43].moveDir = 0;
    mainInterfaceData[43].animation = true;
    
    mainInterfaceData[44].moveDir = 0;
    mainInterfaceData[44].animation = true;
    
    mainInterfaceData[111].moveDir = 0;
    mainInterfaceData[111].animation = true;
    
    mainInterfaceData[37].showString = false;
    mainInterfaceData[37].moveDir = 1;
    mainInterfaceData[37].animation = true;
    
    for (int i = 45; i < 45+4; i++) {
        mainInterfaceData[i].moveDir = 0;
        mainInterfaceData[i].animation = true;
    }
    
    for (int i = 0; i < 12; i++) {
        mainInterfaceData[63+i].moveDir = 0;
        mainInterfaceData[63+i].animation = true;
    }
}

//--------------------------------------------------------------

void ofApp::detailEditInterfaceOn() {
    mainInterfaceData[6].showString = true;
    mainInterfaceData[6].moveDir = 1;
    mainInterfaceData[6].animation = true;
    
    mainInterfaceData[4].showString = true;
    mainInterfaceData[4].moveDir = 1;
    mainInterfaceData[4].animation = true;
    
    mainInterfaceData[11].showString = false;
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
    
    mainInterfaceData[43].moveDir = 0;
    mainInterfaceData[43].animation = true;
    
    mainInterfaceData[44].moveDir = 0;
    mainInterfaceData[44].animation = true;
    
    mainInterfaceData[40].moveDir = 0;
    mainInterfaceData[40].animation = true;
    
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
    
    mainInterfaceData[50].moveDir = 1;
    mainInterfaceData[50].showString = true;
    mainInterfaceData[50].animation = true;
    
}


//--------------------------------------------------------------

void ofApp::detailEditInterfaceOff() {
    
    mainInterfaceData[4].moveDir = 0;
    mainInterfaceData[4].animation = true;
    
    /*
     mainInterfaceData[5].moveDir = 0;
     mainInterfaceData[5].animation = true;
     */
    
    mainInterfaceData[6].moveDir = 0;
    mainInterfaceData[6].animation = true;
    
    mainInterfaceData[11].moveDir = 0;
    mainInterfaceData[11].animation = true;
    
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
    
    mainInterfaceData[50].animation = true;
    mainInterfaceData[50].moveDir = 0;
    
    mainInterfaceData[43].showString = false;
    mainInterfaceData[43].moveDir = 1;
    mainInterfaceData[43].animation = true;
    
    mainInterfaceData[44].showString = false;
    mainInterfaceData[44].moveDir = 1;
    mainInterfaceData[44].animation = true;
    
    mainInterfaceData[12].showString = false;
    mainInterfaceData[12].moveDir = 1;
    mainInterfaceData[12].animation = true;
    
    mainInterfaceData[40].showString = false;
    mainInterfaceData[40].moveDir = 1;
    mainInterfaceData[40].animation = true;
    
    mainInterfaceData[111].showString = false;
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
    mainInterfaceData[1].showString = true;
    mainInterfaceData[1].animation = true;
    mainInterfaceData[1].moveDir = 1;
    mainInterfaceData[2].showString = true;
    mainInterfaceData[2].animation = true;
    mainInterfaceData[2].moveDir = 1;
    mainInterfaceData[3].showString = true;
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
    
    mainInterfaceData[37].showString = false;
    mainInterfaceData[37].animation = true;
    mainInterfaceData[37].moveDir = 0;
    
    mainInterfaceData[41].showString = false;
    mainInterfaceData[41].animation = true;
    mainInterfaceData[41].moveDir = 0;
    
    
    mainInterfaceData[0].showString = true;
    mainInterfaceData[0].moveDir = 1;
    mainInterfaceData[0].animation = true;
    
    mainInterfaceData[43].showString = false;
    mainInterfaceData[43].animation = true;
    mainInterfaceData[43].moveDir = 1;
    
    
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
    
    mainInterfaceData[43].showString = false;
    mainInterfaceData[43].animation = true;
    mainInterfaceData[43].moveDir = 0;
    
    mainInterfaceData[0].moveDir = 0;
    mainInterfaceData[0].animation = true;
    
    mainInterfaceData[37].showString = false;
    mainInterfaceData[37].animation = true;
    mainInterfaceData[37].moveDir = 1;
    
    mainInterfaceData[41].showString = false;
    mainInterfaceData[41].animation = true;
    mainInterfaceData[41].moveDir = 1;
    
}

//--------------------------------------------------------------

void ofApp::pauseInterfaceOn() {
    
    mainInterfaceData[8].moveDir = 1;
    mainInterfaceData[8].animation = true;
    
    mainInterfaceData[9].moveDir = 1;
    mainInterfaceData[9].animation = true;
    
    mainInterfaceData[10].moveDir = 1;
    mainInterfaceData[10].animation = true;
    
    //  if (currentState == !STATE_VOLUME){
    mainInterfaceData[37].showString = false;
    mainInterfaceData[37].moveDir = 1;
    mainInterfaceData[37].animation = true;
    // }
    
    
    
    mainInterfaceData[41].showString = false;
    mainInterfaceData[41].moveDir = 1;
    mainInterfaceData[41].animation = true;
    
    mainInterfaceData[42].showString = false;
    mainInterfaceData[42].moveDir = 1;
    mainInterfaceData[42].animation = true;
    
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
    
}

//--------------------------------------------------------------
void ofApp::bpmInterfaceOn() {
    
    mainInterfaceData[43].animation = true;
    mainInterfaceData[43].moveDir = 1;
    
    mainInterfaceData[38].animation = true;
    mainInterfaceData[38].moveDir = 1;
    mainInterfaceData[38].showString = true;
    
    mainInterfaceData[55].showString = true;
    mainInterfaceData[55].animation = true;
    mainInterfaceData[55].moveDir = 1;
    mainInterfaceData[56].showString = true;
    mainInterfaceData[56].animation = true;
    mainInterfaceData[56].moveDir = 1;
    mainInterfaceData[57].showString = true;
    mainInterfaceData[57].animation = true;
    mainInterfaceData[57].moveDir = 1;
    
    mainInterfaceData[55].sliderPct =ofMap( (1/(4/float(synths[synthButton[0]].nextPulseDivision))), 0.0,1.0,-1.0,1.0);
    mainInterfaceData[56].sliderPct =ofMap( (1/(4/float(synths[synthButton[1]].nextPulseDivision))), 0.0, 1.0, -1.0, 1.0);
    mainInterfaceData[57].sliderPct =ofMap( (1/(4/float(synths[synthButton[2]].nextPulseDivision))), 0.0, 1.0, -1.0, 1.0);
    
}


//--------------------------------------------------------------
void ofApp::bpmInterfaceOff(){
    
    mainInterfaceData[43].animation = true;
    mainInterfaceData[43].moveDir = 0;
    
    //   mainInterfaceData[37].animation = true;
    //  mainInterfaceData[37].moveDir = 1;
    
    mainInterfaceData[38].animation = true;
    mainInterfaceData[38].moveDir = 0;
    mainInterfaceData[38].showString = true;
    
    mainInterfaceData[55].animation = true;
    mainInterfaceData[55].moveDir = 0;
    mainInterfaceData[56].animation = true;
    mainInterfaceData[56].moveDir = 0;
    mainInterfaceData[57].animation = true;
    mainInterfaceData[57].moveDir = 0;
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
    
    mainInterfaceData[43].showString = false;
    mainInterfaceData[43].animation = true;
    mainInterfaceData[43].moveDir = 1;
    
    for (int i = 0; i < 12*3; i++) {
        mainInterfaceData[75+i].moveDir = 1;
        mainInterfaceData[75+i].animation = true;
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
    
    mainInterfaceData[43].animation = true;
    mainInterfaceData[43].moveDir = 0;
    
    for (int i = 0; i < 12*3; i++) {
        mainInterfaceData[75+i].moveDir = 0;
        mainInterfaceData[75+i].animation = true;
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
void ofApp::makePresetString() {
    presetNames.push_back("AAA");
    presetNames.push_back("BBB");
    presetNames.push_back("CCC");
    presetNames.push_back("DDD");
    
}

void ofApp::makeDesignGrid() {
    
    ofVec3f third = ofVec2f(ofGetWidth()/3,ofGetHeight()/3);
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
        
        
        synthButton[0] = synthButton[1];
        synths[synthButton[0]].ownSlot = 0;
        
        synthButton[1] = temp;
        synths[synthButton[1]].ownSlot = 1;
        
        aniPct = 0.0;
        interfaceMoving = true;
        
        if (
            scaleCollection.scaleVec.at(synths[synthButton[0]].currentScaleVecPos%scaleCollection.scaleVec.size()).name !=
            scaleCollection.scaleVec.at(synths[synthButton[1]].currentScaleVecPos%scaleCollection.scaleVec.size()).name
            )
        {
            mainInterfaceData[5].blinkOn();
        }
        
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
        synths[synthButton[2]].myScaleDefault = 0.5;
        synths[synthButton[1]].myScaleDefault = 1.0;
        synths[synthButton[2]].myScaleTarget = 1.0;
        synths[synthButton[1]].myScaleTarget = 0.5;
        
        
        synthButton[2] = synthButton[1];
        synths[synthButton[2]].ownSlot = 2;
        
        synthButton[1] = temp;
        synths[synthButton[1]].ownSlot = 1;
        
        aniPct = 0.0;
        interfaceMoving = true;
        
        if (
            scaleCollection.scaleVec.at(synths[synthButton[1]].currentScaleVecPos%scaleCollection.scaleVec.size()).name !=
            scaleCollection.scaleVec.at(synths[synthButton[2]].currentScaleVecPos%scaleCollection.scaleVec.size()).name
            )
        {
            mainInterfaceData[5].blinkOn();
        }
        
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
        synths[synthButton[0]].myScaleTarget = 0.5;
        synths[synthButton[1]].myScaleTarget = 0.5;
        
        
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
        camTargetFov = 20;
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
        
        
        mainInterfaceData[5].animationB = true;
        mainInterfaceData[7].animationB = true;
        mainInterfaceData[39].animationB = true;
        mainInterfaceData[12].animationB = true;
        mainInterfaceData[40].animationB = true;
        
        for (int i = 0; i < 12; i++) {
            mainInterfaceData[63+i].animationB = true;
        }
        
        
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
        camDefaultFov = 20;
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
        
        aniPct = 0.0;
        interfaceMoving = true;
        editDetailMoveDirection = 1;
        
        mainInterfaceData[5].animationB = true;
        mainInterfaceData[7].animationB = true;
        mainInterfaceData[39].animationB = true;
        mainInterfaceData[12].animationB = true;
        mainInterfaceData[40].animationB = true;
        
        for (int i = 0; i < 12; i++) {
            mainInterfaceData[63+i].animationB = true;
        }
        
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
        
        
        
        aniPct = 0.0;
        
        pauseInterfaceOff();
        harmonyInterfaceOn();
        currentState = STATE_HARMONY;
        
        
    } else if (currentState == STATE_HARMONY) {
        
        synths[synthButton[0]].aniPath = OneHarmonePathOff;
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
        synths[synthButton[0]].myScaleTarget = 0.5;
        synths[synthButton[1]].myScaleTarget = 0.5;
        synths[synthButton[2]].myScaleTarget = 0.5;
        
        aniPct = 0.0;
        
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
        synths[synthButton[0]].myScaleDefault = 0.5;
        synths[synthButton[1]].myScaleDefault = 0.5;
        synths[synthButton[2]].myScaleDefault = 0.5;
        synths[synthButton[0]].myScaleTarget = 1.0;
        synths[synthButton[1]].myScaleTarget = 1.0;
        synths[synthButton[2]].myScaleTarget = 1.0;
        
        aniPct = 0.0;
        
        pauseInterfaceOn();
        bpmInterfaceOff();
        currentState = STATE_DEFAULT;
        
    }
}


//--------------------------------------------------------------
void ofApp::setNewGUI() {
    
    //scale information
    if(!synths[activeSynth].userScale) {
        mainInterfaceData[5].elementName = scaleCollection.scaleVec.at(synths[activeSynth].currentScaleVecPos%scaleCollection.scaleVec.size()).name;
        mainInterfaceData[5].setStringWidth(mainInterfaceData[5].fsPtr->getBBox(mainInterfaceData[5].elementName, mainInterfaceData[5].fontSize, 0, 0).getWidth());
        
        mainInterfaceData[5].setColor(synths[activeSynth].colorHue);
        mainInterfaceData[5].activateDarkerColor(mainInterface);
    } else {
        mainInterfaceData[5].elementName = "INDIVIDUAL";
        mainInterfaceData[5].setStringWidth(mainInterfaceData[5].fsPtr->getBBox(mainInterfaceData[5].elementName, mainInterfaceData[5].fontSize, 0, 0).getWidth());
        
        mainInterfaceData[5].setColor(synths[activeSynth].colorHue);
        mainInterfaceData[5].activateDarkerColor(mainInterface);
    }
    
    
    for (int i = 0; i < 12; i++) {
        mainInterfaceData[13+i].setColor(synths[activeSynth].colorHue);
        mainInterfaceData[13+i].activateOnColor(mainInterface);
        
        mainInterfaceData[13+i].elementName = notes[  (synths[activeSynth].keyNote+i)%12 ];
        mainInterfaceData[13+i].setStringWidth(mainInterfaceData[13+i].fsPtr->getBBox(mainInterfaceData[13+i].elementName, mainInterfaceData[13+i].fontSize, 0, 0).getWidth());
    }
    
    for (int i = 0; i < 12; i++) {
        if (synths[activeSynth].activeScale.steps[i] ) {
            mainInterfaceData[13+i].setOn();
        } else {
            mainInterfaceData[13+i].setOff();
        }
    }
    
    //preset information
    mainInterfaceData[7].elementName = presetNames[synths[activeSynth].preset%presetNames.size()];
    mainInterfaceData[7].setStringWidth(mainInterfaceData[7].fsPtr->getBBox(mainInterfaceData[7].elementName, mainInterfaceData[7].fontSize, 0, 0).getWidth());
    
    mainInterfaceData[7].setColor(synths[activeSynth].colorHue);
    mainInterfaceData[7].activateOnColor(mainInterface);
    
    
    //notes row
    for (int i = 0; i < 12; i++) {
        mainInterfaceData[25+i].setColor(synths[activeSynth].colorHue);
        
        if(pianoKeys[(synths[activeSynth].keyNote+i)%12] == 0) {
            mainInterfaceData[25+i].activateOnColor(mainInterface);
        } else {
            mainInterfaceData[25+i].activateDarkerColor(mainInterface);
        }
        
        mainInterfaceData[25+i].elementName = ofToString(synths[activeSynth].keyNote+i);
        mainInterfaceData[25+i].setStringWidth(mainInterfaceData[25+i].fsPtr->getBBox(mainInterfaceData[25+i].elementName, mainInterfaceData[25+i].fontSize, 0, 0).getWidth());
    }
    
    
    mainInterfaceData[1].setColor(synths[synthButton[0]].colorHue);
    mainInterfaceData[1].activateOnColor(mainInterface);
    
    mainInterfaceData[2].setColor(synths[synthButton[1]].colorHue);
    mainInterfaceData[2].activateOnColor(mainInterface);
    
    mainInterfaceData[3].setColor(synths[synthButton[2]].colorHue);
    mainInterfaceData[3].activateOnColor(mainInterface);
    
    mainInterfaceData[51].setColor(synths[synthButton[0]].colorHue);
    mainInterfaceData[51].activateOnColor(mainInterface);
    
    mainInterfaceData[52].setColor(synths[synthButton[1]].colorHue);
    mainInterfaceData[52].activateOnColor(mainInterface);
    
    mainInterfaceData[53].setColor(synths[synthButton[2]].colorHue);
    mainInterfaceData[53].activateOnColor(mainInterface);
    
    muster.setColor(synths[activeSynth].colorHue);
    
    //bpm slider
    
    mainInterfaceData[55].setColor(synths[synthButton[0]].colorHue);
    mainInterfaceData[55].activateOnColor(mainInterface);
    mainInterfaceData[55].elementName = "1/"+ofToString(5-synths[synthButton[0]].nextPulseDivision);
    mainInterfaceData[55].setStringWidth(mainInterfaceData[55].fsPtr->getBBox(mainInterfaceData[55].elementName, mainInterfaceData[55].fontSize, 0, 0).getWidth());
    
    
    mainInterfaceData[56].setColor(synths[synthButton[1]].colorHue);
    mainInterfaceData[56].activateOnColor(mainInterface);
    mainInterfaceData[56].elementName = "1/"+ofToString(5-synths[synthButton[1]].nextPulseDivision);
    mainInterfaceData[56].setStringWidth(mainInterfaceData[56].fsPtr->getBBox(mainInterfaceData[56].elementName, mainInterfaceData[56].fontSize, 0, 0).getWidth());
    
    
    mainInterfaceData[57].setColor(synths[synthButton[2]].colorHue);
    mainInterfaceData[57].activateOnColor(mainInterface);
    mainInterfaceData[57].elementName = "1/"+ofToString(5-synths[synthButton[2]].nextPulseDivision);
    mainInterfaceData[57].setStringWidth(mainInterfaceData[57].fsPtr->getBBox(mainInterfaceData[57].elementName, mainInterfaceData[57].fontSize, 0, 0).getWidth());
    
    
    mainInterfaceData[58].elementName = ofToString(synths[synthButton[0]].keyNote)+" "+ofToString(notes[synths[synthButton[0]].keyNote%12]);
    mainInterfaceData[58].setStringWidth(mainInterfaceData[58].fsPtr->getBBox(mainInterfaceData[58].elementName, mainInterfaceData[58].fontSize, 0, 0).getWidth());
    
    mainInterfaceData[59].elementName = ofToString(synths[synthButton[1]].keyNote)+" "+ofToString(notes[synths[synthButton[1]].keyNote%12]);
    mainInterfaceData[59].setStringWidth(mainInterfaceData[59].fsPtr->getBBox(mainInterfaceData[59].elementName, mainInterfaceData[59].fontSize, 0, 0).getWidth());
    
    mainInterfaceData[60].elementName = ofToString(synths[synthButton[2]].keyNote)+" "+ofToString(notes[synths[synthButton[2]].keyNote%12]);
    mainInterfaceData[60].setStringWidth(mainInterfaceData[60].fsPtr->getBBox(mainInterfaceData[60].elementName, mainInterfaceData[60].fontSize, 0, 0).getWidth());
    
    mainInterfaceData[61].elementName = ofToString(notes[globalKey%12]);
    mainInterfaceData[61].setStringWidth(mainInterfaceData[61].fsPtr->getBBox(mainInterfaceData[61].elementName, mainInterfaceData[61].fontSize, 0, 0).getWidth());
    
    mainInterfaceData[62].elementName = scaleCollection.scaleVec.at(globalScaleVecPos).name;
    mainInterfaceData[62].setStringWidth(mainInterfaceData[62].fsPtr->getBBox(mainInterfaceData[62].elementName, mainInterfaceData[62].fontSize, 0, 0).getWidth());
    
    if(synths[activeSynth].globalHarmony){
        mainInterfaceData[111].setOn();
    } else {
        mainInterfaceData[111].setOff();
    }
    
    //set scaleNote Info
    
    markScaleSteps(63);
}

//--------------------------------------------------------------
void ofApp::markScaleSteps(int index_) {
    
    for (int i = 0; i < 12; i++){
        if (synths[activeSynth].activeScale.steps[i] ) {
            mainInterfaceData[index_+i].setColor(synths[activeSynth].colorHue);
            mainInterfaceData[index_+i].setOn();
            mainInterfaceData[index_+i].activateDarkerColor(mainInterface);
        }else{
            mainInterfaceData[index_+i].setOff();
            //mainInterfaceData[index_+i].setColor(synths[activeSynth].colorHue);
            //mainInterfaceData[index_+i].activateOnColor(mainInterface);
        }
    }
    
    markSynthNotes(index_);
}

//--------------------------------------------------------------
void ofApp::markSynthNotes(int index_){
    
    for (int i = 0; i < 12; i++){
        if (synths[activeSynth].activeScale.steps[i] ) {
            mainInterfaceData[index_+i].setColor(synths[activeSynth].colorHue);
            mainInterfaceData[index_+i].setOn();
            mainInterfaceData[index_+i].activateDarkerColor(mainInterface);
        }
    }
    
    for (map<unsigned long,cubeGroup>::iterator it=synths[activeSynth].soundsMap.begin(); it!=synths[activeSynth].soundsMap.end(); ++it){
        if(it->second.size > 0){
            mainInterfaceData[it->second.groupNote-synths[activeSynth].keyNote+index_].setColor(it->second.groupColor);
            mainInterfaceData[it->second.groupNote-synths[activeSynth].keyNote+index_].activateOnColor(mainInterface);
            // cout << it->second.groupNote - synths[activeSynth].keyNote << endl;
        }
    }
}

//--------------------------------------------------------------
void ofApp::markScaleSteps() {
    
    for (int j = 0; j < 3; j++) {
        for (int i = 0; i < 12; i++){
            if (synths[synthButton[j]].activeScale.steps[i] ) {
                mainInterfaceData[75+(synths[synthButton[j]].ownSlot*12)+i].setColor(synths[synthButton[j]].colorHue);
                mainInterfaceData[75+(synths[synthButton[j]].ownSlot*12)+i].setOn();
                mainInterfaceData[75+(synths[synthButton[j]].ownSlot*12)+i].activateDarkerColor(mainInterface);
            }else{
                mainInterfaceData[75+(synths[synthButton[j]].ownSlot*12)+i].setOff();
                //mainInterfaceData[index_+i].setColor(synths[activeSynth].colorHue);
                //mainInterfaceData[index_+i].activateOnColor(mainInterface);
            }
        }
    }
    
    markSynthNotes();
}

//--------------------------------------------------------------
void ofApp::markSynthNotes(){
    
    for (int j = 0; j < 3; j++) {
        for (int i = 0; i < 12; i++){
            if (synths[synthButton[j]].activeScale.steps[i] ) {
                mainInterfaceData[75+(synths[synthButton[j]].ownSlot*12)+i].setColor(synths[synthButton[j]].colorHue);
                mainInterfaceData[75+(synths[synthButton[j]].ownSlot*12)+i].setOn();
                mainInterfaceData[75+(synths[synthButton[j]].ownSlot*12)+i].activateDarkerColor(mainInterface);
            }
        }
        
        for (map<unsigned long,cubeGroup>::iterator it=synths[synthButton[j]].soundsMap.begin(); it!=synths[synthButton[j]].soundsMap.end(); ++it){
            if(it->second.size > 0){
                mainInterfaceData[(it->second.groupNote-synths[synthButton[j]].keyNote)+75+(synths[synthButton[j]].ownSlot*12)].setColor(it->second.groupColor);
                mainInterfaceData[(it->second.groupNote-synths[synthButton[j]].keyNote)+75+(synths[synthButton[j]].ownSlot*12)].activateOnColor(mainInterface);
                // cout << it->second.groupNote - synths[activeSynth].keyNote << endl;
            }
        }
    }
}

ofColor ofApp::filterColor(ofColor c_){
    ofColor temp;
    temp.r = ofClamp(c_.r+20, 20, 230);
    temp.g = ofClamp(c_.g+5, 20, 230);
    temp.b = ofClamp(c_.b-20, 20, 230);
    temp.a = ofClamp(c_.a, 10, 255);
    
    
    return temp;
}

void ofApp::exit(){
    startUp = true;
    saveToXml();
}


void ofApp::saveToXml(){
    
    settings.clear();
    
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
    settings.saveFile(ofxiOSGetDocumentsDirectory()+"settings.xml");
#else
    settings.saveFile("settings.xml");
#endif
    
}

void ofApp::loadFromXml(){
    startUp = true;
    
    //load grid presets from xml
#if TARGET_OS_IPHONE
    if (settings.loadFile(ofxiOSGetDocumentsDirectory()+"settings.xml")) {
#else
        if (settings.loadFile("settings.xml")) {
#endif
            settings.pushTag("Version");
            //dont load old xmlsettings
            if (ofToString( settings.getValue("number", "") ) != appVersion) {
                cout << "old xml settings" << endl;
                settings.loadFile("settingsDefault.xml");
            } else {
                settings.popTag();
            }
            
        }else if (settings.loadFile("settingsDefault.xml")) {
            cout << "loadDefault" << endl;
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
        // volumeRampValueChanged(mainVol);
        volumeRestartTarget = settings.getValue("volume", 1.0);
        settings.popTag();
        settings.pushTag("slots");
        for (int i = 0; i < 3; i++) {
            settings.pushTag("slot",i);
            synths[synthButton[i]].sVolume = settings.getValue("volume", 1.0);
            synths[synthButton[i]].changeSynthVolume(synths[synthButton[i]].sVolume);
            settings.popTag();
        }
        settings.popTag();
        settings.popTag();
        
        
        
        //--------------------------------
        
        
        //synth global settings
        
        settings.pushTag("SynthSettings");
        for (int i = 0; i < 3; i++) {
            settings.pushTag("synth",i);
            synths[synthButton[i]].currentScaleVecPos = settings.getValue("ScaleVecPos", 0);
            //   synths[synthButton[i]].setMusicScale(scaleCollection, synths[synthButton[i]].currentScaleVecPos);
            
            synths[synthButton[i]].userScale = settings.getValue("userScaleBool", 0);
            
            synths[synthButton[i]].colorHue = settings.getValue("cHue", 0);
            
            synths[synthButton[i]].preset = settings.getValue("patchPreset", 0);
            
            synths[synthButton[i]].trackSwitchOn = settings.getValue("StartStatus", 0);
            
            synths[synthButton[i]].pause = settings.getValue("pauseStatus", 0);
            
            synths[synthButton[i]].keyNote = settings.getValue("keyNote", 0);
            
            synths[synthButton[i]].globalHarmony = settings.getValue("globalHarmony", 1);
            
            //set the layer lowFreqVolumeFactor to keynote
            synths[synthButton[i]].mainTonicPtr->setParameter("lfvf"+synths[synthButton[i]].instrumentId, pow( 1-(1-ofMap(float(synths[synthButton[i]].keyNote), 12, 127, 1.0, 0.0)),4 ) );
            
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
            for (int j = 0; j < 4; j++) {
                if (activeDirection.at(j) == '1') {
                    synths[synthButton[i]].activeDirection[j] = true;;
                } else{
                    synths[synthButton[i]].activeDirection[j] = false;;
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
        }
        settings.popTag();
        
        
        
        //--------------------------------
        
        //load current grids
        settings.pushTag("currentGrids");
        for (int i = 0; i < 3; i++) {
            settings.pushTag("grid",i);
            string temp = settings.getValue("info", "0");
            if (temp.size() == TILES*TILES){
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
        
        //load notes
        
        settings.pushTag("currentGrids");
        for (int i = 0; i < 3; i++) {
            settings.pushTag("grid",i);
            if (settings.getNumTags("Note") > 0) {
                for (int j = 0; j < settings.getNumTags("Note"); j++) {
                    settings.pushTag("Note",j);
                    
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
                    settings.popTag();
                }
            }
            settings.popTag();
        }
        settings.popTag();
        
        //bpm
        settings.pushTag("BPM");
        settings.pushTag("global");
        bpm =ofClamp(settings.getValue("value", 200), 1, 2000);
        tonicSynth.setParameter("BPM", bpm*4);
        mainInterfaceData[38].elementName = "BPM "+ ofToString(bpm);
        settings.popTag();
        settings.pushTag("slots");
        for (int i = 0; i < 3; i++) {
            settings.pushTag("slot",i);
            synths[synthButton[i]].nextPulseDivision = settings.getValue("bpm", 4);
            settings.popTag();
        }
        settings.popTag();
        settings.popTag();
        
        //globalKey
        settings.pushTag("GlobalKey");
        globalKey =settings.getValue("key",0);
        settings.popTag();
        
        //globalKey
        settings.pushTag("GlobalScale");
        globalScaleVecPos =settings.getValue("pos",0);
        settings.popTag();
    }
    
    
    
    
