#include "ofApp.h"
#define TILES 6
#define TILESIZE 100/TILES
#define TILEBORDER 0.12
#define BPM 130*4
#define ANI_SPEED 0.028;

enum currentState {STATE_DEFAULT,STATE_EDIT,STATE_VOLUME,STATE_EDIT_DETAIL};




//--------------------------------------------------------------
void ofApp::setup(){
    ofSoundStreamSetup(2, 0, this, 44100, 256, 4);
    
    ofSetFrameRate(60);
    //ofDisableAntiAliasing();
    ofSetVerticalSync(true);
    ofEnableDepthTest();
    
   font.setup( "sspb.ttf" //font file, ttf only
    );					//lower res mipmaps wil bleed into each other
    
    font.setKerning(font.getKerning());
    font.setLodBias(-1);
    
    scaleCollection.loadScales();
    makeDesignGrid();
    
    synthPos.resize(3);
    
    camActiveFov = 20;
    camFov = 30;
    
    nearClip = 10;
    farClip = 10000;
    
    for (int i = -1; i < 2; i++) {
        ofNode temp;
       // temp.setPosition( i*(TILES*TILESIZE*2), 0, 0);
        temp.setPosition( intersectPlane( designGrid[i+1][1] ) );
        //temp.setOrientation(ofVec3f(0,0,1));
        synthPos[i+1]=temp;
    }
    
    synths.resize(3);
    
    synths[0] = Instrument("a",TILES,TILESIZE,TILEBORDER);
    synths[0].setup(&timeCounter, &tonicSynth, synthPos[0]);
    synths[0].setMusicScale(scaleCollection, 0);
    // synths[0].setKeyNote(40-12);
    
    synths[1] = Instrument("b",TILES,TILESIZE,TILEBORDER);
    synths[1].setup(&timeCounter, &tonicSynth, synthPos[1]);
    synths[1].setMusicScale(scaleCollection, 0);
    // synths[1].setKeyNote(40);
    
    
    synths[2] = Instrument("c",TILES,TILESIZE,TILEBORDER);
    synths[2].setup(&timeCounter, &tonicSynth, synthPos[2]);
    synths[2].setMusicScale(scaleCollection, 0);
    // synths[2].setKeyNote(40+12);
    
    
    globalTranslate.setPosition(ofVec3f(TILES*TILESIZE,TILES*TILESIZE,0)/-2);
    activeSynth = 1;
    
    
    ControlParameter rampTarget = tonicSynth.addParameter("mainVolumeRamp").max(1.0).min(0.0);
    tonicSynth.setParameter("mainVolumeRamp", 1.0);
    volumeRamp = RampedValue().value(0.7).length(0.1).target(rampTarget);
    
    ControlGenerator pulse = ControlMetro().bpm(BPM);
    ofEvent<float>* pulseEvent = tonicSynth.createOFEvent(pulse);
    ofAddListener(*pulseEvent, this, &ofApp::pulseEvent );
    
    
    setupAudio();
    
    setupOfxGui();
    
    
    
    
    intersecPlane.resize(3);
    for (int i = 0; i < 3 ; i++) {
        ofPlanePrimitive temp;
        temp.set(TILES*TILESIZE, TILES*TILESIZE);
        temp.setPosition(synthPos[i].getPosition());
        intersecPlane.at(i).setFrom(temp);
    }
    
    setupStatesAndAnimation();
    setupGlobalInterface();
    

   
    
    ofBackground(11, 65, 65);
    fbo.allocate(ofGetWidth(),ofGetHeight(), GL_RGB);
    
    fbo.begin();
    ofClear(0, 0, 0);
    fbo.end();
    
    //   ofEnableLighting();
    light.setPosition(0, 800, 840);
    
    
    //temp sketch
    light.setAmbientColor(ofColor::fireBrick);
    drawFboImage = false;
    
    doubleClickTime = 300;
    curTap = 0;
    lastTap = 0;
    mouseDragging = false;
    tapCounter = 0;
    drawInfo = false;
    showGui = false;
    
    timeCounter = -1;
    
    guiFbo.allocate(400, 800, GL_RGBA);
    
    focusCam = false;
    
    
    synthButton[0] = 0;
    synthButton[1] = 1;
    synthButton[2] = 2;
    
    
    aniPct = 1.0;
    aniCam = 1.0;
    
    animCam = false;
    interfaceMoving = false;
    debugCam = false;
    currentState = STATE_DEFAULT;
    
}

void ofApp::setupAudio(){
    Generator temp;
    for (int i = 0; i < synths.size(); i++) {
        temp = temp + synths[i].instrumentOut;
    }
    mainOut = temp ;
    
    tonicSynth.setOutputGen(mainOut*volumeRamp);
}

//--------------------------------------------------------------
void ofApp::update(){
    
    if (showGui) {
        updateGuiFbo();
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
        // cout << tweenFloat << endl;
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
        interfaceMoving = false;
    }
    
    
    intersectPlane();
    
    //global interfac
    if (currentState == STATE_DEFAULT) {
        mainInterfaceData[0].updateMainMeshSlider(mainInterface, testCam.worldToScreen(synthPos[1].getPosition()),mainInterfaceData[0].sliderWidth);
        
        mainInterfaceData[8].updateMainMesh(mainInterface, testCam.worldToScreen(synthPos[0].getPosition()));
        mainInterfaceData[9].updateMainMesh(mainInterface, testCam.worldToScreen(synthPos[1].getPosition()));
        mainInterfaceData[10].updateMainMesh(mainInterface, testCam.worldToScreen(synthPos[2].getPosition()));
    }
    
    if (currentState == STATE_VOLUME) {
        mainInterfaceData[1].updateMainMeshSlider(mainInterface, testCam.worldToScreen(synthPos[0].getPosition()), mainInterfaceData[1].sliderWidth);
        mainInterfaceData[2].updateMainMeshSlider(mainInterface, testCam.worldToScreen(synthPos[1].getPosition()), mainInterfaceData[2].sliderWidth);
        mainInterfaceData[3].updateMainMeshSlider(mainInterface, testCam.worldToScreen(synthPos[2].getPosition()), mainInterfaceData[3].sliderWidth);
    }
    
    if (currentState == STATE_EDIT_DETAIL) {
        mainInterfaceData[5].updateMainMesh(mainInterface, testCam.worldToScreen(synthActivePos.getPosition()));
        mainInterfaceData[4].updateMainMesh(mainInterface, testCam.worldToScreen(synthActivePos.getPosition()));
        mainInterfaceData[6].updateMainMesh(mainInterface, testCam.worldToScreen(synthActivePos.getPosition()));
        mainInterfaceData[7].updateMainMesh(mainInterface, testCam.worldToScreen(synthActivePos.getPosition()));

        
        for (int i = 0; i < 12; i++) {
            mainInterfaceData[13+i].updateMainMesh(mainInterface, testCam.worldToScreen(synthActivePos.getPosition()));
        }
        for (int i = 0; i < 12; i++) {
            mainInterfaceData[25+i].updateMainMesh(mainInterface, testCam.worldToScreen(synthActivePos.getPosition()));
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    
    
    glShadeModel(GL_SMOOTH);
    // glDisable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    
    glEnable(GL_MULTISAMPLE);
    
   // ofEnableLighting();
    
    
    if (!debugCam) {
        testCam.begin();
    } else {
        cam.begin();
    }
    
  //  light.enable();
    // planeTemp.draw();
    
    // globalTranslate.transformGL();
    
    
    for (int i = 0; i < 3; i++) {
        synths[i].myNode.transformGL();
        
        synths[i].draw();
        
        synths[i].myNode.restoreTransformGL();
        
    }
    
    
    // globalTranslate.restoreTransformGL();
    
    if (!debugCam) {
        testCam.end();
    } else {
        cam.end();
    }
    
    // mousePick.draw(ofGetMouseX(),ofGetMouseY());
    
    ofDisableLighting();
    if (drawInfo) {
        drawDebug();
    }
    if(drawFboImage) {
        fbo.draw(0, 0);
    }
    
    
    if (showGui) {
        guiFbo.draw(0,0);
    }
    
    
    glDisable(GL_DEPTH_TEST);

    mainInterface.draw();
    
    ofPushStyle();
    ofSetColor(255, 255, 255);
    for (int i = 0; i < mainInterfaceData.size();i ++){
        if (mainInterfaceData[i].showString){
            font.draw(mainInterfaceData[i].elementName, 42, mainInterfaceData[i].drawStringPos.x, mainInterfaceData[i].drawStringPos.y);
        }
    }
    ofPopStyle();
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    //setupAudio();
    if (key == 's') {
        ofImage pix;
        pix.allocate(ofGetWidth(), ofGetHeight(), OF_IMAGE_COLOR);
        pix.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
        pix.saveImage( ofGetTimestampString()+"debug.png");
    }
    
    if (key == 'f') {
        drawFboImage = !drawFboImage;
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
    
    if (key == 'o') {
        showGui = !showGui;
    }
    
    
    if (!interfaceMoving) {
        
        if (key == '5' && currentState == STATE_EDIT) {
            
            detailEditInterfaceOn();
            currentState = STATE_EDIT_DETAIL;
        } else if (key == '5' && currentState  == STATE_EDIT_DETAIL) {
            
            detailEditInterfaceOff();
            currentState = STATE_EDIT;
        }
        
        
        if(key == '4'  && currentState != STATE_VOLUME && currentState != STATE_EDIT ) {
            
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
            
        } else if(key == '4'  && currentState == STATE_VOLUME) {
            
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
        
        
        if (key =='1') {
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
                synths[ temp ].aniPath = oneToActive;
                synths[temp].myTarget = synthActivePos.getOrientationQuat();
                synths[temp].myDefault = synthPos[0].getOrientationQuat();
                
                synths[synthButton[0]].scaling = true;
                synths[synthButton[1]].scaling = true;
                synths[synthButton[0]].myScaleDefault = 0.5;
                synths[synthButton[1]].myScaleDefault = 1.0;
                synths[synthButton[0]].myScaleTarget = 1.0;
                synths[synthButton[1]].myScaleTarget = 0.5;
                
                
                synthButton[0] = synthButton[1];
                synthButton[1] = temp;
                aniPct = 0.0;
                
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
                synthButton[1] = temp;
                
                
                
                //camani
                camQuatDefault = camNotActiveSynth.getOrientationQuat();
                camQuatTarget = camActiveSynth.getOrientationQuat();
                camUsePath = camPath;
                camTargetFov = camActiveFov;
                camDefaultFov = camFov;
                animCam = true;
                
                aniPct = 0.0;
                
                pauseInterfaceOff();
                currentState = STATE_EDIT;
            }
            
        }
        if (key =='2') {
            
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
                
                pauseInterfaceOn();
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
                
                pauseInterfaceOff();
                currentState = STATE_EDIT;
                
            }
            
            
        }
        if (key =='3') {
            
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
                synthButton[1] = temp;
                aniPct = 0.0;
                
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
                synthButton[1] = temp;
                
                //camera
                camQuatDefault = camNotActiveSynth.getOrientationQuat();
                camQuatTarget = camActiveSynth.getOrientationQuat();
                camUsePath = camPath;
                camDefaultFov = camFov;
                camTargetFov = camActiveFov;
                animCam = true;
                
                aniPct = 0.0;
                
                pauseInterfaceOff();
                currentState = STATE_EDIT;
                
            }
            
        }
        
    }
    
    
    if (key == 'c') {
        debugCam = !debugCam;
    }
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
    if  (!mouseDragging) {
        updateFboMesh();
        if (synths[activeSynth].cubeMap.find(lastPickColor.getHex()) != synths[activeSynth].cubeMap.end() ) {
            ofVec2f cordTemp = synths[activeSynth].cubeMap[lastPickColor.getHex()];
            //copy cube info
            synthInfo tempInfo = synths[activeSynth].layerInfo.at(cordTemp.x).at(cordTemp.y);
            if (tempInfo.hasCube && !tempInfo.blocked) {
                mouseDragging = true;
                
                TapHelper temp = TapHelper(
                                           tapCounter,
                                           cordTemp,
                                           synths[activeSynth].cubeVector[tempInfo.cubeVecNum].defaultZ,
                                           synths[activeSynth].cubeVector[tempInfo.cubeVecNum].groupColor
                                           );
                
                curMouseId = tapCounter;
                tapMap[tapCounter] = temp;
                synths[activeSynth].layerInfo.at(cordTemp.x).at(cordTemp.y).blocked = true;
                tapCounter++;
                
                //cout << "added" << tapCounter << " pos:" << cordTemp << endl;
            }
        }
    }
    
    if (!interfaceMoving) {
        
        
        if (currentState == STATE_DEFAULT) {
            if (mainInterfaceData[0].isInside(ofVec2f(x,y))) {
                float value = ofClamp(ofMap(x, mainInterfaceData[0].minX, mainInterfaceData[0].maxX, 0.0, 1.0), 0.0, 1.0);
                mainInterfaceData[0].sliderWidth = mainInterfaceData[0].maxX - x;
                volumeRampValueChanged(value);
                cout << value  << endl;
            }
        }
        
        if (currentState == STATE_VOLUME) {
            
            if (mainInterfaceData[1].isInside(ofVec2f(x,y))) {
                float value = ofClamp(ofMap(x, mainInterfaceData[1].minX, mainInterfaceData[1].maxX, 0.0, 1.0), 0.0, 1.0);
                mainInterfaceData[1].sliderWidth = mainInterfaceData[1].maxX - x;
                synths[synthButton[0]].changeSynthVolume(value);
                cout << value  << endl;
            }
            if (mainInterfaceData[2].isInside(ofVec2f(x,y))) {
                float value = ofClamp(ofMap(x, mainInterfaceData[2].minX, mainInterfaceData[2].maxX, 0.0, 1.0), 0.0, 1.0);
                mainInterfaceData[2].sliderWidth = mainInterfaceData[2].maxX - x;
                synths[synthButton[1]].changeSynthVolume(value);
                cout << value  << endl;
            }
            if (mainInterfaceData[3].isInside(ofVec2f(x,y))) {
                float value = ofClamp(ofMap(x, mainInterfaceData[3].minX, mainInterfaceData[3].maxX, 0.0, 1.0), 0.0, 1.0);
                mainInterfaceData[3].sliderWidth = mainInterfaceData[3].maxX - x;
                synths[synthButton[2]].changeSynthVolume(value);
                cout << value  << endl;
            }

            
        }
        
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
    updateFboMesh();
    
    
    curTap = ofGetElapsedTimeMillis();
    if ( lastTap != 0 && (curTap-lastTap < doubleClickTime)) {
        
        
        
        if (lastPickColor != ofColor(255,255,255)) {
            if (synths[activeSynth].cubeMap.find(lastPickColor.getHex()) != synths[activeSynth].cubeMap.end() ) {
                
                ofVec2f cordTemp = synths[activeSynth].cubeMap[lastPickColor.getHex()];
                synths[activeSynth].tapEvent(cordTemp.x,cordTemp.y);
            }
            
        }
        
        
        
    }
    
    if (!pointInsideGrid(intersectPos)) {
        if (lastPickColor == ofColor(1,0,0) ) {
            synths[activeSynth].activeDirection[0] =  !synths[activeSynth].activeDirection[0] ;
            synths[activeSynth].planes[0].active =  !synths[activeSynth].planes[0].active ;
        } else if (lastPickColor == ofColor(2,0,0)){
            synths[activeSynth].activeDirection[1] =  !synths[activeSynth].activeDirection[1] ;
            synths[activeSynth].planes[1].active =  !synths[activeSynth].planes[1].active ;
        } else if (lastPickColor == ofColor(3,0,0)) {
            synths[activeSynth].activeDirection[2] =  !synths[activeSynth].activeDirection[2] ;
            synths[activeSynth].planes[2].active =  !synths[activeSynth].planes[2].active ;
        }else if (lastPickColor == ofColor(4,0,0)) {
            synths[activeSynth].activeDirection[3] =  !synths[activeSynth].activeDirection[3] ;
            synths[activeSynth].planes[3].active =  !synths[activeSynth].planes[3].active ;
        }
        
        else if (lastPickColor == ofColor(5,0,0) ) {
            synths[activeSynth].connectedDirection[0] =  !synths[activeSynth].connectedDirection[0] ;
            synths[activeSynth].planes[0].connected =  !synths[activeSynth].planes[0].connected ;
        } else if (lastPickColor == ofColor(6,0,0)){
            synths[activeSynth].connectedDirection[1] =  !synths[activeSynth].connectedDirection[1] ;
            synths[activeSynth].planes[1].connected =  !synths[activeSynth].planes[1].connected ;
        } else if (lastPickColor == ofColor(7,0,0)) {
            synths[activeSynth].connectedDirection[2] =  !synths[activeSynth].connectedDirection[2] ;
            synths[activeSynth].planes[2].connected =  !synths[activeSynth].planes[2].connected ;
        }else if (lastPickColor == ofColor(8,0,0)) {
            synths[activeSynth].connectedDirection[3] =  !synths[activeSynth].connectedDirection[3] ;
            synths[activeSynth].planes[3].connected =  !synths[activeSynth].planes[3].connected ;
        }
    }
    
    lastTap = curTap;
    
    /*
     if(pointInsideGrid(intersectPos)) {
     //cout << " ssad"  << endl;
     synths[activeSynth].clickEvent(vectorPosX, vectorPosY);
     }
     */
    if (!interfaceMoving) {
        
        
        if (currentState == STATE_DEFAULT) {
            
            if (mainInterfaceData[8].isInside(ofVec2f(x,y))) {
                cout << "0 pause"  << endl;
               mainInterfaceData[8].switchColor(mainInterface);
                synths[synthButton[0]].pause = !synths[synthButton[0]].pause;
            }
            if (mainInterfaceData[9].isInside(ofVec2f(x,y))) {
                cout << "1 pause"  << endl;
                mainInterfaceData[9].switchColor(mainInterface);
                synths[synthButton[1]].pause = !synths[synthButton[1]].pause;
                
            }
            if (mainInterfaceData[10].isInside(ofVec2f(x,y))) {
                cout << "2 pause"  << endl;
                mainInterfaceData[10].switchColor(mainInterface);
                synths[synthButton[2]].pause = !synths[synthButton[2]].pause;
            }
            if (mainInterfaceData[0].isInside(ofVec2f(x,y))) {
                synths[synthButton[2]].pause = !synths[synthButton[2]].pause;
                mainInterfaceData[0].sliderWidth = mainInterfaceData[0].maxX - x;
                float value = ofClamp(ofMap(x, mainInterfaceData[0].minX, mainInterfaceData[0].maxX, 0.0, 1.0), 0.0, 1.0);
                volumeRampValueChanged(value);
                cout << value  << endl;
            }
        }
        
        if (currentState == STATE_VOLUME) {
            
            if (mainInterfaceData[1].isInside(ofVec2f(x,y))) {
                float value = ofClamp(ofMap(x, mainInterfaceData[1].minX, mainInterfaceData[1].maxX, 0.0, 1.0), 0.0, 1.0);
                mainInterfaceData[1].sliderWidth = mainInterfaceData[1].maxX - x;
                synths[synthButton[0]].changeSynthVolume(value);
                cout << value  << endl;
            }
            if (mainInterfaceData[2].isInside(ofVec2f(x,y))) {
                float value = ofClamp(ofMap(x, mainInterfaceData[2].minX, mainInterfaceData[2].maxX, 0.0, 1.0), 0.0, 1.0);
                mainInterfaceData[2].sliderWidth = mainInterfaceData[2].maxX - x;
                synths[synthButton[1]].changeSynthVolume(value);
                cout << value  << endl;
            }
            if (mainInterfaceData[3].isInside(ofVec2f(x,y))) {
                float value = ofClamp(ofMap(x, mainInterfaceData[3].minX, mainInterfaceData[3].maxX, 0.0, 1.0), 0.0, 1.0);
                mainInterfaceData[3].sliderWidth = mainInterfaceData[3].maxX - x;
                synths[synthButton[2]].changeSynthVolume(value);
                cout << value  << endl;
            }
            
        }
        
        if (currentState == STATE_EDIT_DETAIL) {
            
            if(  mainInterfaceData[5].isInside(ofVec2f(x,y))) {
                synths[activeSynth].setMusicScale(scaleCollection, synths[activeSynth].currentScaleVecPos%scaleCollection.scaleVec.size() );
                mainInterfaceData[5].elementName = scaleCollection.scaleVec.at(synths[activeSynth].currentScaleVecPos%scaleCollection.scaleVec.size()).name;
                synths[activeSynth].currentScaleVecPos++;
                detailEditInterfaceOn();
                cout << synths[activeSynth].activeScale.name  << endl;
            }
            if(  mainInterfaceData[4].isInside(ofVec2f(x,y))) {
                synths[activeSynth].setKeyNote(-12);
                cout << "-12"  << endl;
            }
            if(  mainInterfaceData[6].isInside(ofVec2f(x,y))) {
                synths[activeSynth].setKeyNote(12);
                cout << "+12"  << endl;
            }
            if(  mainInterfaceData[7].isInside(ofVec2f(x,y))) {
                synths[activeSynth].changePreset();
               // cout << presetNames.at(synths[activeSynth].preset)  << endl;
            }
            for (int i = 1; i < 12; i++) {
                
                if (   mainInterfaceData[13+i].isInside(ofVec2f(x,y))) {
                    synths[activeSynth].changeMusicScale(i);
                    mainInterfaceData[13+i].switchColor(mainInterface);
                    cout <<   synths[activeSynth].activeScale.steps[i] <<endl;
                }
            }
            
            for (int i = 1; i < 12; i++) {
                
                if (   mainInterfaceData[25+i].isInside(ofVec2f(x,y))) {
                    synths[activeSynth].setKeyNote(i);
                    cout <<  "+ " << i << endl;
                }
            }
            
        }
    }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
    if (mouseDragging && pointInsideGrid(intersectPos) ) {
        TapHelper* tempPtr = &tapMap[curMouseId];
        
        if ( tempPtr->tapOrigin.x == vectorPosX  && tempPtr->tapOrigin.y == vectorPosY) {
            tempPtr->old = true;
            synths[activeSynth].layerInfo.at(tempPtr->tapOrigin.x).at(tempPtr->tapOrigin.y).blocked = false;
        }else{
            tempPtr->old = true;
            synths[activeSynth].tapEvent(tempPtr->tapOrigin.x, tempPtr->tapOrigin.y);
            synths[activeSynth].moveEvent(vectorPosX, vectorPosY, tempPtr->zH, tempPtr->cColor);
            synths[activeSynth].layerInfo.at(tempPtr->tapOrigin.x).at(tempPtr->tapOrigin.y).blocked = false;
        }
        mouseDragging = false;
    } else {
        TapHelper* tempPtr = &tapMap[curMouseId];
        
        tempPtr->old = true;
        synths[activeSynth].layerInfo.at(tempPtr->tapOrigin.x).at(tempPtr->tapOrigin.y).blocked = false;
        mouseDragging = false;
        
    }
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    fbo.allocate(ofGetWidth(),ofGetHeight(), GL_RGB);
    makeDesignGrid();
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}



void ofApp::drawDebug() {
    
    
    if (!debugCam) {
        testCam.begin();
    } else {
        cam.begin();
    }
    // globalTranslate.transformGL();
    
    for (int i = 0; i < synths.size(); i++) {
        synths[i].myNode.transformGL();
        synths[i].drawDebug();
        synths[i].myNode.restoreTransformGL();
        
        synthPos[i].draw();
    }
    
    //   globalTranslate.restoreTransformGL();
    
    
    synthActivePos.draw();
    // camActiveSynth.draw();
    camNotActiveSynth.draw();
    
    
    
    testCam.draw();
    
    oneToActive.draw();
    twoToActive.draw();
    threeToActive.draw();
    
    oneToBack.draw();
    twoToBack.draw();
    threeToBack.draw();
    
    camPath.draw();
    camPathBack.draw();
    
    centerToThree.draw();
    centerToOne.draw();
    
    //volumeLayerPathOff.draw();
    
    //ofDrawGrid(2500);
    
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
    ofDrawBitmapString("Pick RGB: "+ofToString(int(RGB[0]))+" "+ofToString(int(RGB[1]))+" "+ofToString(int(RGB[2])), 20, 100);
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

void ofApp::intersectPlane(){
    //raytesting
    worldMouse = testCam.screenToWorld(ofVec3f(ofGetMouseX(),ofGetMouseY(),0.0));
    
    mouseRay.s = worldMouse;
    mouseRay.t = worldMouse-testCam.getPosition();
    
    intersecPlane[activeSynth].intersect(mouseRay, intersectPos);
    vectorPosX = (intersectPos.x/TILESIZE)+ float(TILES)/2;
    vectorPosY = (intersectPos.y/TILESIZE)+ float(TILES)/2;
    
}

ofVec3f ofApp::intersectPlane(ofVec2f target_) {
    
    ofCamera tempNode;
    tempNode.setNearClip(nearClip);
    tempNode.setFarClip(farClip);
    tempNode.setFov(camFov);
    tempNode.setPosition(0, -TILES*TILESIZE*2, TILES*TILESIZE*7);
    tempNode.lookAt(ofVec3f(0,0,0)-tempNode.getZAxis());
    ofVec3f wMouse = tempNode.screenToWorld( ofVec3f(target_.x,target_.y,0.0));
    ofRay ray;
    ray.s = wMouse;
    ray.t = wMouse-tempNode.getPosition();
    
    ofPlanePrimitive planeTemp;
    planeTemp.set(2000, 2000);
    ofPlane iP;
    iP.setFrom(planeTemp);
    ofVec3f cord;
    iP.intersect(ray, cord);
    return cord;
}


void ofApp::updateFboMesh(){
    
    
    
    synths[activeSynth].updateFboMesh();
    
    fbo.begin();
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_MULTISAMPLE);
    ofDisableLighting();
    ofClear(255,255,255);
    glShadeModel(GL_FLAT);
    
    testCam.begin();
    ofDisableLighting();
    light.disable();
    
    //  globalTranslate.transformGL();
    
    synths[activeSynth].myNode.transformGL();
    synths[activeSynth].drawFbo();
    synths[activeSynth].myNode.restoreTransformGL();
    
    
    //  globalTranslate.restoreTransformGL();
    
    testCam.end();
    
    glReadPixels(ofGetMouseX(),ofGetMouseY(), 1,1, GL_RGB, GL_UNSIGNED_BYTE, RGB);
    glDisable(GL_DEPTH_TEST);

    fbo.end();
    lastPickColor = ofColor(RGB[0],RGB[1],RGB[2]);
    
    //  cout << lastPickColor  << endl;
}

void ofApp::updateTapMap() {
    
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

void ofApp::pulseEvent(float& val) {
    // cout << "pulse" << val << endl;
    
    
    
    
    
    timeCounter++;
    
    
    if (timeCounter > TILES) {
        timeCounter = 0;
        for (int i = 0; i < synths.size(); i++) {
            synths[i].nextDirection();
        }
    }
    
    for (int i = 0; i < synths.size(); i++) {
        synths[i].noteTrigger();
    }
    
    
    
    
    
    
}

void ofApp::setupOfxGui() {
    volumeRampValue.addListener(this, &ofApp::volumeRampValueChanged);
    gui.setup("gui");
    gui.add(volumeRampValue.set("Main Volume", 1.0, 0.0, 1.0));
    
    
}

void ofApp::volumeRampValueChanged(float & volumeRampValue) {
    tonicSynth.setParameter("mainVolumeRamp", volumeRampValue);
    //    cout << volumeRampValue << endl;
}

void ofApp::audioRequested (float * output, int bufferSize, int nChannels){
    tonicSynth.fillBufferOfFloats(output, bufferSize, nChannels);
}

void ofApp::updateGuiFbo() {
    
    guiFbo.begin();
    ofClear(0, 0, 0, 0);
    glDisable(GL_DEPTH_TEST);
    gui.draw();
    glEnable(GL_DEPTH_TEST);
    guiFbo.end();
    
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
    synthActivePos.setPosition(0, -TILES*TILESIZE*4, TILESIZE*TILES*3.5);
    camActiveSynth.setPosition(synthActivePos.getGlobalPosition()+ofVec3f(0,-TILESIZE*TILES*5.0,TILES*TILESIZE*2.2));
    camNotActiveSynth.setPosition(0, -TILES*TILESIZE*2, TILES*TILESIZE*7);
    
    camActiveSynth.lookAt(synthActivePos.getPosition() - camActiveSynth.getZAxis());
    camNotActiveSynth.lookAt(synthPos[1].getPosition() - camNotActiveSynth.getZAxis());
    synthActivePos.setOrientation(camActiveSynth.getOrientationQuat());
    
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
    
    
    
    //startcam position an rotate
    testCam.setPosition(camNotActiveSynth.getPosition());
    testCam.setOrientation(camNotActiveSynth.getOrientationQuat());
    testCam.setFov(camFov);
    //___---___
    //---___---
    
    //from default to volume
    volumeMatrix.rotate(45, -1, 0, 0);
    
    TwoVolumeLayerPathOn.addVertex(ofVec3f(0,0,0));
    TwoVolumeLayerPathOn.bezierTo(ofVec3f(0,0,-(TILESIZE*TILES)/4), ofVec3f(0,(TILES*TILESIZE)/4,-(TILES*TILESIZE)/2), ofVec3f(0,(TILESIZE*TILES)/2,-(TILESIZE*TILES)/2));
    // TwoVolumeLayerPathOn = TwoVolumeLayerPathOn.getResampledByCount(80);
    
    TwoVolumeLayerPathOff.addVertex(ofVec3f(0,(TILESIZE*TILES)/2,-(TILESIZE*TILES)/2));
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
    
}


void ofApp::setupGlobalInterface() {
    ofVec3f smallButton = ofVec3f(20,20,0);
    ofVec3f horizontalSlider = ofVec3f(100,20,0);
    ofVec3f verticalSlider = ofVec3f(20,100,0);
    
    ofVec3f place = ofVec3f(0,-200,0);
    GlobalGUI temp = GlobalGUI(0,string("mainVolume"), horizontalSlider, ofColor(50,0,0),place);
    mainInterfaceData.push_back(temp);
    
    place = ofVec3f(0,+150,0);
    temp = GlobalGUI(1,string("OneVolume"),horizontalSlider,ofColor(51,0,0),place);
    mainInterfaceData.push_back(temp);
    temp = GlobalGUI(2,string("TwoVolume"),horizontalSlider,ofColor(52,0,0),place);
    mainInterfaceData.push_back(temp);
    temp = GlobalGUI(3,string("ThreeVolume"),horizontalSlider,ofColor(53,0,0),place);
    mainInterfaceData.push_back(temp);
    
    place = ofVec3f(+320, 40,0);
    temp = GlobalGUI(4,string("OctaveDown"),smallButton,ofColor(54,0,0),place);
    mainInterfaceData.push_back(temp);
    
    place = ofVec3f(+250,-250,0);
    temp = GlobalGUI(5,string("activeScale"),horizontalSlider,ofColor(55,0,0),place);
    mainInterfaceData.push_back(temp);
    
    place = ofVec3f(+320, -40,0);
    temp = GlobalGUI(6,string("OctaveUp"),smallButton,ofColor(56,0,0),place);
    mainInterfaceData.push_back(temp);
    
    place = ofVec3f(+250, -210,0);
    temp = GlobalGUI(7,string("activePreset"),horizontalSlider,ofColor(57,0,0),place);
    mainInterfaceData.push_back(temp);
    
    
    place = ofVec3f(0,+200,0);
    temp = GlobalGUI(8,string("OnePlayPause"),smallButton,ofColor(59,0,0),place);
    mainInterfaceData.push_back(temp);
    temp = GlobalGUI(9,string("TwoPlayPause"),smallButton,ofColor(60,0,0),place);
    mainInterfaceData.push_back(temp);
    temp = GlobalGUI(10,string("ThreePlayPause"),smallButton,ofColor(61,0,0),place);
    mainInterfaceData.push_back(temp);
    
    temp = GlobalGUI(11,string("SwitchVolume"),smallButton,ofColor(62,0,0),place);
    mainInterfaceData.push_back(temp);
    temp = GlobalGUI(12,string("SwitchActiveOptions"),smallButton,ofColor(63,0,0),place);
    mainInterfaceData.push_back(temp);
    
    
    string notes[] = {"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};
    for (int i = 0; i < 12; i++) {
        place = ofVec3f(+250,150-(i*20),0);
        temp = GlobalGUI(13+i,notes[i],smallButton,ofColor(0,0,0),place);
        mainInterfaceData.push_back(temp);
        
    }
    
    for (int i = 0; i < 12; i++) {
        place = ofVec3f(+290,150-(i*20),0);
        temp = GlobalGUI(25+i,notes[i],smallButton,ofColor(0,0,0),place);
        mainInterfaceData.push_back(temp);
    }
    
    mainInterface.setMode(OF_PRIMITIVE_TRIANGLES);
    
    for (int i = 0; i < mainInterfaceData.size(); i++) {
        
        for (int j = 0; j < 4; j++) {
            mainInterface.addVertex(ofVec3f(0,0,0));
            mainInterfaceFbo.addVertex(ofVec3f(0,0,0));
            mainInterface.addColor(mainInterfaceData[i].elementColor);
            mainInterfaceFbo.addColor(ofColor(50+i,0,0));
        }
        
        for (int j = 0; j < 6; j++) {
            mainInterface.addIndex(mainInterfaceData.at(i).index[j]+(4*i));
        }
    }
    
}

void ofApp::detailEditInterfaceOn() {
    mainInterfaceData[5].showString = true;

    for (int i = 0; i < 12; i++) {
        
        if (   synths[activeSynth].activeScale.steps[i] ) {
            mainInterfaceData[13+i].setOn(mainInterface);
        } else {
            mainInterfaceData[13+i].setOff(mainInterface);
        }
        
        mainInterfaceData[25+i].showString = true;
    }
}


void ofApp::detailEditInterfaceOff() {
    mainInterfaceData[5].updateMainMesh(mainInterface,ofVec3f( -1000-1000,0));
    mainInterfaceData[5].showString = false;
    mainInterfaceData[4].updateMainMesh(mainInterface,ofVec3f( -1000-1000,0));
    mainInterfaceData[6].updateMainMesh(mainInterface,ofVec3f( -1000-1000,0));
    mainInterfaceData[7].updateMainMesh(mainInterface,ofVec3f( -1000-1000,0));

    
    for (int i = 0; i < 12; i++) {
        mainInterfaceData[13+i].updateMainMesh(mainInterface,ofVec3f( -1000-1000,0));
    }
    for (int i = 0; i < 12; i++) {
        mainInterfaceData[25+i].updateMainMesh(mainInterface,ofVec3f( -1000-1000,0));
        mainInterfaceData[25+i].showString = false;

    }
}

void ofApp::volumeInterfacOn() {
    mainInterfaceData[1].sliderWidth = mainInterfaceData[1].maxX- ofMap(synths[synthButton[0]].sVolume, 0.0, 1.0, mainInterfaceData[1].minX, mainInterfaceData[1].maxX);
    mainInterfaceData[2].sliderWidth = mainInterfaceData[2].maxX- ofMap(synths[synthButton[1]].sVolume, 0.0, 1.0, mainInterfaceData[2].minX, mainInterfaceData[2].maxX);
    mainInterfaceData[3].sliderWidth = mainInterfaceData[3].maxX- ofMap(synths[synthButton[2]].sVolume, 0.0, 1.0, mainInterfaceData[3].minX, mainInterfaceData[3].maxX);
    mainInterfaceData[1].showString = true;
    mainInterfaceData[2].showString = true;
    mainInterfaceData[3].showString = true;

}

void ofApp::volumeInterfaceOff() {
    mainInterfaceData[1].updateMainMesh(mainInterface,ofVec3f( -1000-1000,0));
    mainInterfaceData[2].updateMainMesh(mainInterface,ofVec3f( -1000-1000,0));
    mainInterfaceData[3].updateMainMesh(mainInterface,ofVec3f( -1000-1000,0));
    mainInterfaceData[1].showString = false;
    mainInterfaceData[2].showString = false;
    mainInterfaceData[3].showString = false;

}

void ofApp::pauseInterfaceOn() {
    mainInterfaceData[0].showString = true;
}

void ofApp::pauseInterfaceOff() {
    mainInterfaceData[0].updateMainMesh(mainInterface,ofVec3f( -1000-1000,0));
    mainInterfaceData[0].showString = false;

    
    mainInterfaceData[8].updateMainMesh(mainInterface,ofVec3f( -1000-1000,0));
    mainInterfaceData[9].updateMainMesh(mainInterface,ofVec3f( -1000-1000,0));
    mainInterfaceData[10].updateMainMesh(mainInterface,ofVec3f( -1000-1000,0));
    
}

void ofApp::makePresetString() {
    presetNames.push_back("Sinu");
    presetNames.push_back("simple");
    presetNames.push_back("simple2");
}

void ofApp::makeDesignGrid() {
    
    ofVec2f third = ofVec2f(ofGetWidth()/3,ofGetHeight()/3);
    ofVec2f center = third/2;
    
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            designGrid[i][j] = ofVec2f(third.x*i+center.x,third.y*j+center.y);
        }
    }
}