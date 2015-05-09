#include "ofApp.h"
#define TILES 5
#define TILESIZE 100/TILES
#define TILEBORDER 0.075
#define BPM 220
#define ANI_SPEED 0.025;

enum currentState {STATE_DEFAULT,STATE_EDIT,STATE_VOLUME,STATE_EDIT_DETAIL};




//--------------------------------------------------------------
void ofApp::setup(){
    ofSoundStreamSetup(2, 2, this, 44100, 256, 4);
    
    ofSetFrameRate(60);
    //ofDisableAntiAliasing();
    ofSetVerticalSync(true);
    ofEnableDepthTest();
    
   
    robotoLight.setup("fonts/Roboto-Light.ttf", //font file, ttf only
               1.0,					//lineheight percent
               1024,					//texture atlas dimension
               false,					//create mipmaps of the font, useful to scale down the font at smaller sizes
               8,					//texture atlas element padding, shouldbe >0 if using mipmaps otherwise
               2.0f					//dpi scaleup, render textures @2x the reso
               );				//lower res mipmaps wil bleed into each other
    
    
   // robotoLight.setCharacterSpacing(1);
    robotoLight.setKerning(robotoLight.getKerning());
    
    robotoBold.setup("fonts/Roboto-Bold.ttf", //font file, ttf only
                      1.0,					//lineheight percent
                      1024,					//texture atlas dimension
                      false,					//create mipmaps of the font, useful to scale down the font at smaller sizes
                      8,					//texture atlas element padding, shouldbe >0 if using mipmaps otherwise
                      2.0f					//dpi scaleup, render textures @2x the reso
                      );				//lower res mipmaps wil bleed into each other
    
    
   // robotoBold.setCharacterSpacing(1);
    robotoBold.setKerning(robotoBold.getKerning());
    
    robotoCon.setup("fonts/RobotoCondensed-Italic.ttf", //font file, ttf only
                      1.0,					//lineheight percent
                      1024,					//texture atlas dimension
                      false,					//create mipmaps of the font, useful to scale down the font at smaller sizes
                      8,					//texture atlas element padding, shouldbe >0 if using mipmaps otherwise
                      2.0f					//dpi scaleup, render textures @2x the reso
                      );				//lower res mipmaps wil bleed into each other
    
    
   // robotoCon.setCharacterSpacing(1);
    robotoCon.setKerning(robotoCon.getKerning());
    
    scaleCollection.loadScales();
    makeDesignGrid();
    
    synthPos.resize(3);
    
    camActiveFov = 25;
    camFov = 35;
    
    nearClip = 10;
    farClip = 10000;
    
    for (int i = -1; i < 2; i++) {
        ofNode temp;
         //temp.setPosition( i*(TILES*TILESIZE*2), 0, 0);
        temp.setPosition( intersectPlane( designGrid[i+1][1] ) );
        synthPos[i+1]=temp;
    }
    
    synths.resize(3);
    
    synths[0] = Instrument("a",TILES,TILESIZE,TILEBORDER);
    synths[0].setup(&timeCounter, &tonicSynth, synthPos[0], &bpmTick);
    synths[0].setMusicScale(scaleCollection, 0);
    synths[0].setKeyNote(60);
    
    synths[1] = Instrument("b",TILES,TILESIZE,TILEBORDER);
    synths[1].setup(&timeCounter, &tonicSynth, synthPos[1], &bpmTick);
    synths[1].setMusicScale(scaleCollection, 0);
    synths[1].setKeyNote(60);
    
    
    synths[2] = Instrument("c",TILES,TILESIZE,TILEBORDER);
    synths[2].setup(&timeCounter, &tonicSynth, synthPos[2], &bpmTick);
    synths[2].setMusicScale(scaleCollection, 0);
    synths[2].setKeyNote(60);
    
    
    activeSynth = 1;
    
    
    ControlParameter rampTarget = tonicSynth.addParameter("mainVolumeRamp").max(1.0).min(0.0);
    tonicSynth.setParameter("mainVolumeRamp", 1.0);
    volumeRamp = RampedValue().value(0.7).length(0.1).target(rampTarget);
    
    
    bpm = BPM;
    
    ControlParameter bpmpara = tonicSynth.addParameter("BPM");
    tonicSynth.setParameter("BPM", bpm);
    ControlGenerator pulse = ControlMetro().bpm(bpmpara);
    ofEvent<float>* pulseEvent = tonicSynth.createOFEvent(pulse);
    ofAddListener(*pulseEvent, this, &ofApp::pulseEvent );
    
    
    setupAudio();
    
    makePresetString();

    setupStatesAndAnimation();
    setupGlobalInterface();
    
    //intersectplane
    planeForIntersect.set(TILES*TILESIZE,TILES*TILESIZE);
    thisIntersect.setFrom(planeForIntersect);
    
   
    ofBackground( filterColor(ofColor(22,22,22)) );

    ofEnableLighting();
    light.setPosition(synthActivePos.getPosition()+ofVec3f(0,-100,0));
    
    
    //temp sketch
    light.setAmbientColor( filterColor(ofColor::white) );
    
    doubleClickTime = 300;
    curTap = 0;
    lastTap = 0;
    mouseDragging = false;
    tapCounter = 0;
    drawInfo = false;
    
    timeCounter = -1;
    bpmTick = 0.0;
    tickTimes.resize(5);
    
    
    focusCam = false;
    
    
    synthButton[0] = 0;
    synthButton[1] = 1;
    synthButton[2] = 2;
    
    
    //  aniPct = 1.0;
    aniCam = 1.0;
    
    animCam = false;
    interfaceMoving = false;
    debugCam = false;
    currentState = STATE_DEFAULT;
    
    
    
    //load icons
    ofImage temp;
    
    temp.loadImage("icons/back.png");
    back.loadData(temp.getPixelsRef(),GL_RGBA);
    
    temp.loadImage("icons/detail.png");
    detail.loadData(temp.getPixelsRef(), GL_RGBA);
    
    temp.loadImage("icons/volume.png");
    volume.loadData(temp.getPixelsRef(), GL_RGBA);
    
    temp.loadImage("icons/pause.png");
    pause.loadData(temp.getPixelsRef(), GL_RGBA);
    
    temp.loadImage("icons/play.png");
    play.loadData(temp.getPixelsRef(), GL_RGBA);
    
    scaleFac = 1-(1/((designGrid[0][0].y*6)/144));
    scaleFac *= 0.55;
    
    setNewGUI();
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
    }
    
    
    if (interfaceMoving) {
        updateInterfaceMesh();
    }
    
    
    intersectPlane(ofGetMouseX(),ofGetMouseY());
    
}

void ofApp::updateInterfaceMesh() {
    
    mainInterfaceData[0].updateMainMeshSlider(mainInterface, testCam.worldToScreen(synthPos[1].getPosition()),mainInterfaceData[0].sliderWidth,tweenFloat);
    mainInterfaceData[54].updateMainMeshSlider(mainInterface, testCam.worldToScreen(synthPos[1].getPosition()),mainInterfaceData[0].sliderWidth,tweenFloat);

    
    mainInterfaceData[8].updateMainMesh(mainInterface,designGrid[0][1],tweenFloat);
    mainInterfaceData[9].updateMainMesh(mainInterface, designGrid[1][1],tweenFloat);
    mainInterfaceData[10].updateMainMesh(mainInterface, designGrid[2][1],tweenFloat);
    
    mainInterfaceData[11].updateMainMesh(mainInterface, ofVec3f(designGrid[1][0].x, designGrid[0][0].y*6,0),tweenFloat);

    
    mainInterfaceData[37].updateMainMesh(mainInterface, ofVec3f(designGrid[1][0].x, designGrid[0][0].y*6,0), tweenFloat);
    mainInterfaceData[38].updateMainMesh(mainInterface, designGrid[1][0], tweenFloat);
    
   // mainInterfaceData[40].updateMainMesh(mainInterface, designGrid[0][2], tweenFloat);
   // mainInterfaceData[41].updateMainMesh(mainInterface, designGrid[1][2], tweenFloat);
   // mainInterfaceData[42].updateMainMesh(mainInterface, designGrid[2][2], tweenFloat);
    
    
    mainInterfaceData[1].updateMainMeshSlider(mainInterface, testCam.worldToScreen(synthPos[0].getPosition()), mainInterfaceData[1].sliderWidth,tweenFloat);
    mainInterfaceData[2].updateMainMeshSlider(mainInterface, testCam.worldToScreen(synthPos[1].getPosition()), mainInterfaceData[2].sliderWidth,tweenFloat);
    mainInterfaceData[3].updateMainMeshSlider(mainInterface, testCam.worldToScreen(synthPos[2].getPosition()), mainInterfaceData[3].sliderWidth,tweenFloat);
    
    mainInterfaceData[51].updateMainMeshSlider(mainInterface, testCam.worldToScreen(synthPos[0].getPosition()), mainInterfaceData[1].sliderWidth,tweenFloat);
    mainInterfaceData[52].updateMainMeshSlider(mainInterface, testCam.worldToScreen(synthPos[1].getPosition()), mainInterfaceData[2].sliderWidth,tweenFloat);
    mainInterfaceData[53].updateMainMeshSlider(mainInterface, testCam.worldToScreen(synthPos[2].getPosition()), mainInterfaceData[3].sliderWidth,tweenFloat);

    
    
    mainInterfaceData[4].updateMainMesh(mainInterface, ofVec3f(0,designGrid[0][0].y*2,0),tweenFloat);
    mainInterfaceData[6].updateMainMesh(mainInterface,ofVec3f(designGrid[0][0].x*6,designGrid[0][0].y*2,0),tweenFloat);
  //  mainInterfaceData[39].updateMainMesh(mainInterface, designGrid[2][1], tweenFloat);
    
   
    for (int i = 0; i < 12; i++) {
        mainInterfaceData[13+i].updateMainMesh(mainInterface,ofVec3f(0,designGrid[0][0].y*2,0),tweenFloat);
    }
    for (int i = 0; i < 12; i++) {
        mainInterfaceData[25+i].updateMainMesh(mainInterface, ofVec3f(0,designGrid[0][0].y*2,0) ,tweenFloat);
    }
    
    mainInterfaceData[50].updateMainMesh(mainInterface,ofVec3f(0,designGrid[0][0].y*2,0),tweenFloat);

 
    mainInterfaceData[5].updateMainMesh(mainInterface, ofVec3f(designGrid[0][0].x*6, designGrid[2][1].y,0) ,tweenFloat);
    
    mainInterfaceData[7].updateMainMesh(mainInterface, ofVec3f(0, designGrid[0][1].y,0) ,tweenFloat);
    
    //move in edit to detail state
    mainInterfaceData[5].updateMainMeshB(mainInterface, ofVec3f(designGrid[0][0].x*6,designGrid[2][1].y+(abs((editDetailMoveDirection-tweenFloat))*(designGrid[0][0].y*1)),0)
                                        ,tweenFloat);
    
    mainInterfaceData[7].updateMainMeshB(mainInterface,  ofVec3f(0,designGrid[0][1].y+(abs((editDetailMoveDirection-tweenFloat))*(designGrid[0][0].y*1)),0)
                                        ,tweenFloat);

    
    mainInterfaceData[43].updateMainMesh(mainInterface,  ofVec3f( designGrid[1][2].x,designGrid[0][0].y*6,0),tweenFloat);
    mainInterfaceData[44].updateMainMesh(mainInterface,  ofVec3f( designGrid[1][0].x,0,0) ,tweenFloat);
    
    
    for (int i = 45; i < 45+4; i++) {
        mainInterfaceData[i].updateMainMesh(mainInterface, testCam.worldToScreen(synths[activeSynth].myNode.getPosition()) ,tweenFloat);
    }
    
     
    mainInterfaceData[49].updateMainMesh(mainInterface,  ofVec3f(designGrid[1][0].x,designGrid[0][0].y*2,0),tweenFloat);

    
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    
    
    //glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    
    glEnable(GL_MULTISAMPLE);
  //  ofEnableLighting();
   //light.enable();
   
    
    if (!debugCam) {
        testCam.begin();
    } else {
        cam.begin();
    }
    
     //  thisIntersect.draw();
 //   shader.begin();
    for (int i = 0; i < 3; i++) {
        synths[i].myNode.transformGL();
        
        synths[i].draw();
        
        synths[i].myNode.restoreTransformGL();
        
    }
    
  //  shader.end();
    
    if (!debugCam) {
        testCam.end();
    } else {
        cam.end();
    }
    
    
    ofDisableLighting();
    if (drawInfo) {
        drawDebug();
    }
 
    glDisable(GL_DEPTH_TEST);

    
  //  glDisable(GL_MULTISAMPLE);
    
    
    
    mainInterface.draw();

    ofPushStyle();
    ofSetColor(255, 255, 255,255);
    
    for (int i = 0; i < mainInterfaceData.size();i ++){
        
        if(i >= 13 && i <= 36) {
            ofSetColor(255, 255, 255,190);
        } else {
            ofSetColor(255, 255, 255,255);
        }
            mainInterfaceData[i].drawFontString();
    }
    
    float tempTrans = 72*scaleFac;
    
    
    ofSetColor(mainInterfaceData[43].displayColor);
    back.draw(mainInterfaceData[43].drawStringPos.x-tempTrans, mainInterfaceData[43].drawStringPos.y-tempTrans, 144*scaleFac,144*scaleFac);
    ofSetColor(mainInterfaceData[44].displayColor);
    detail.draw(mainInterfaceData[44].drawStringPos.x-tempTrans, mainInterfaceData[44].drawStringPos.y-tempTrans, 144*scaleFac,144*scaleFac);
    
    ofSetColor(mainInterfaceData[11].displayColor);
    back.draw(mainInterfaceData[11].drawStringPos.x-tempTrans, mainInterfaceData[11].drawStringPos.y-tempTrans,144*scaleFac,144*scaleFac);
    
    
    ofSetColor(mainInterfaceData[37].displayColor);

    if(currentState == STATE_DEFAULT){
    volume.draw(mainInterfaceData[37].drawStringPos.x-tempTrans, mainInterfaceData[37].drawStringPos.y-tempTrans,144*scaleFac,144*scaleFac);
        
        
    } else if (currentState == STATE_VOLUME) {
        back.draw(mainInterfaceData[37].drawStringPos.x-tempTrans, mainInterfaceData[37].drawStringPos.y-tempTrans,144*scaleFac,144*scaleFac);
    }
    
    ofSetColor(mainInterfaceData[8].displayColor);

    if(!synths[synthButton[0]].pause) {
        play.draw(mainInterfaceData[8].drawStringPos.x-tempTrans, mainInterfaceData[8].drawStringPos.y-tempTrans,144*scaleFac,144*scaleFac);
    } else {
        pause.draw(mainInterfaceData[8].drawStringPos.x-tempTrans, mainInterfaceData[8].drawStringPos.y-tempTrans,144*scaleFac,144*scaleFac);
    }
    
    ofSetColor(mainInterfaceData[9].displayColor);

    if(!synths[synthButton[1]].pause) {
        play.draw(mainInterfaceData[9].drawStringPos.x-tempTrans, mainInterfaceData[9].drawStringPos.y-tempTrans,144*scaleFac,144*scaleFac);
    } else {
        pause.draw(mainInterfaceData[9].drawStringPos.x-tempTrans, mainInterfaceData[9].drawStringPos.y-tempTrans,144*scaleFac,144*scaleFac);
    }
    
    ofSetColor(mainInterfaceData[10].displayColor);

    if(!synths[synthButton[2]].pause) {
        play.draw(mainInterfaceData[10].drawStringPos.x-tempTrans, mainInterfaceData[10].drawStringPos.y-tempTrans,144*scaleFac,144*scaleFac);
    } else {
        pause.draw(mainInterfaceData[10].drawStringPos.x-tempTrans, mainInterfaceData[10].drawStringPos.y-tempTrans,144*scaleFac,144*scaleFac);
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
    
   
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
    
    if (!interfaceMoving) {
        
        
        if (currentState == STATE_DEFAULT) {
            
            
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
                        tonicSynth.setParameter("BPM",bpm);
                        mainInterfaceData[38].elementName = "BPM "+ ofToString(bpm);
                    }
                    
                    cout <<     mainInterfaceData[38].touchStart.x << endl;
                    
                    
                }
            }
            
            
        }
        
        if (currentState == STATE_VOLUME) {
            
            if (mainInterfaceData[1].isInside(ofVec2f(x,y))) {
                float value = ofClamp(ofMap(x, mainInterfaceData[1].minX, mainInterfaceData[1].maxX, 0.0, 1.0), 0.0, 1.0);
                mainInterfaceData[1].setSlider(mainInterface, mainInterfaceData[1].maxX - x);
                mainInterfaceData[1].sliderPos = mainInterfaceData[1].maxX - x;
                mainInterfaceData[1].elementName ="A "+ofToString( int(value*100));
                synths[synthButton[0]].changeSynthVolume(value);
                cout << value  << endl;
            }
            if (mainInterfaceData[2].isInside(ofVec2f(x,y))) {
                float value = ofClamp(ofMap(x, mainInterfaceData[2].minX, mainInterfaceData[2].maxX, 0.0, 1.0), 0.0, 1.0);
                mainInterfaceData[2].setSlider(mainInterface, mainInterfaceData[2].maxX - x);
                mainInterfaceData[2].sliderPos = mainInterfaceData[1].maxX - x;
                mainInterfaceData[2].elementName ="B "+ofToString( int(value*100));
                synths[synthButton[1]].changeSynthVolume(value);
                cout << value  << endl;
            }
            if (mainInterfaceData[3].isInside(ofVec2f(x,y))) {
                float value = ofClamp(ofMap(x, mainInterfaceData[3].minX, mainInterfaceData[3].maxX, 0.0, 1.0), 0.0, 1.0);
                mainInterfaceData[3].setSlider(mainInterface, mainInterfaceData[3].maxX - x);
                mainInterfaceData[3].sliderPos = mainInterfaceData[1].maxX - x;
                mainInterfaceData[3].elementName ="C "+ofToString(int( value*100));
                synths[synthButton[2]].changeSynthVolume(value);
                cout << value  << endl;
            }
            
            if (mainInterfaceData[0].isInside(ofVec2f(x,y))) {
                float value = ofClamp(ofMap(x, mainInterfaceData[0].minX, mainInterfaceData[0].maxX, 0.0, 1.0), 0.0, 1.0);
                mainInterfaceData[0].setSlider(mainInterface, mainInterfaceData[0].maxX - x);
                mainInterfaceData[0].elementName = "GLOBAL "+ofToString(int( value*100));

                volumeRampValueChanged(value);
                cout << value  << endl;
            }

            
            
        }
        
        if (currentState == STATE_EDIT) {
            
            /*
            if(mainInterfaceData[39].isInside(ofVec2f(x,y))) {
                if (!mainInterfaceData[39].touchDown){
                    mainInterfaceData[39].touchStart = ofVec2f(x,y);
                    mainInterfaceData[39].touchDown = true;
                }
                
                if (mainInterfaceData[39].touchDown) {
                    
                    int keyMod = (mainInterfaceData[39].touchStart.x - x)/20;
                    mainInterfaceData[39].tempInt = keyMod;
                    
                    if ( abs(keyMod) > 0 ){
                        mainInterfaceData[39].touchStart.x = x;
                        synths[activeSynth].setKeyNote(keyMod);
                        //setNewGUI();
                        mainInterfaceData[39].elementName = "Pitch Note " + notes[  (synths[activeSynth].keyNote)%12 ];
                    }
                        
                    cout <<     mainInterfaceData[39].touchStart.x << endl;
                    
                    
                }
            }
             */
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

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
    
    if (!interfaceMoving) {
    if (currentState == STATE_EDIT) {
        intersectPlane(x, y);
        if ( (intersectPos.x < 100 && intersectPos.x > 0) && (intersectPos.y < 100 && intersectPos.y > 0) ) {
            synths[activeSynth].tapEvent(vectorPosX,vectorPosY);
            insideSynth = true;
        } else {
            insideSynth = false;
        }
    }
    }
    
    curTap = ofGetElapsedTimeMillis();
    if ( lastTap != 0 && (curTap-lastTap < doubleClickTime)) {
        
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
                   (y > designGrid[0][2].y-designGrid[0][0].y && y < designGrid[0][2].y+designGrid[0][0].y) ){
                    if(synths[synthButton[0]].trackSwitchOn) {
                        buttonOnePress();
                    } else {
                        synths[synthButton[0]].trackSwitchOn = true;
                    }
                }
                
                if( (x > designGrid[2][2].x-designGrid[0][0].x && x < designGrid[2][2].x+designGrid[0][0].x) &&
                   (y > designGrid[2][2].y-designGrid[0][0].y && y < designGrid[2][2].y+designGrid[0][0].y) ){
                    if(synths[synthButton[2]].trackSwitchOn) {
                        buttonThreePress();
                    } else {
                        synths[synthButton[2]].trackSwitchOn = true;
                    }
                }
                
            }
            
            
        }
        

        
    }
    
    lastTap = curTap;

    if ( (!interfaceMoving) && (!insideSynth) ){
        
        
        if (currentState == STATE_DEFAULT) {
            
            if (mainInterfaceData[8].isInside(ofVec2f(x,y))) {
                //cout << "0 pause"  << endl;
                mainInterfaceData[8].blinkOn();
                synths[synthButton[0]].pause = !synths[synthButton[0]].pause;
            }
            if (mainInterfaceData[9].isInside(ofVec2f(x,y))) {
                //cout << "1 pause"  << endl;
                mainInterfaceData[9].blinkOn();
                synths[synthButton[1]].pause = !synths[synthButton[1]].pause;
                
            }
            if (mainInterfaceData[10].isInside(ofVec2f(x,y))) {
               // cout << "2 pause"  << endl;
                mainInterfaceData[10].blinkOn();
                synths[synthButton[2]].pause = !synths[synthButton[2]].pause;
            }
        
            if (mainInterfaceData[37].isInside(ofVec2f(x,y))) {
                buttonFourPress();
                mainInterfaceData[37].blinkOn();
                cout << "button 4" << endl;
            }
         
            if (mainInterfaceData[40].isInside(ofVec2f(x,y))) {
                if (synths[synthButton[0]].trackSwitchOn){
                    buttonOnePress();
                } else {
                    synths[synthButton[0]].trackSwitchOn = true;
                }
                cout << "toggle1"  << endl;
            }
            if (mainInterfaceData[41].isInside(ofVec2f(x,y))) {
                buttonTwoPress();
                cout << "toggle2"   << endl;
            }
            if (mainInterfaceData[42].isInside(ofVec2f(x,y))) {
                if (synths[synthButton[2]].trackSwitchOn){
                    buttonThreePress();
                } else {
                    synths[synthButton[2]].trackSwitchOn = true;
                }
                cout << "toggle1"  << endl;
            }
        }
        
        else  if (currentState == STATE_VOLUME) {
            
            if (mainInterfaceData[1].isInside(ofVec2f(x,y))) {
                float value = ofClamp(ofMap(x, mainInterfaceData[1].minX, mainInterfaceData[1].maxX, 0.0, 1.0), 0.0, 1.0);
                mainInterfaceData[1].setSlider(mainInterface, mainInterfaceData[1].maxX - x );
                
                synths[synthButton[0]].changeSynthVolume(value);
                cout << value  << endl;
            }
            if (mainInterfaceData[2].isInside(ofVec2f(x,y))) {
                float value = ofClamp(ofMap(x, mainInterfaceData[2].minX, mainInterfaceData[2].maxX, 0.0, 1.0), 0.0, 1.0);
                mainInterfaceData[2].setSlider(mainInterface, mainInterfaceData[2].maxX - x );
                
                synths[synthButton[1]].changeSynthVolume(value);
                cout << value  << endl;
            }
            if (mainInterfaceData[3].isInside(ofVec2f(x,y))) {
                float value = ofClamp(ofMap(x, mainInterfaceData[3].minX, mainInterfaceData[3].maxX, 0.0, 1.0), 0.0, 1.0);
                mainInterfaceData[3].setSlider(mainInterface, mainInterfaceData[3].maxX - x );
                
                synths[synthButton[2]].changeSynthVolume(value);
                cout << value  << endl;
            }
            if (mainInterfaceData[37].isInside(ofVec2f(x,y))) {
                buttonFourPress();
                mainInterfaceData[37].blinkOn();
                cout << "button 4" << endl;
            }
            
        }
        
        
        
        
        else    if (currentState == STATE_EDIT_DETAIL) {
            
            
            /*
            if(  mainInterfaceData[43].isInside(ofVec2f(x,y))) {
                buttonTwoPress();
                mainInterfaceData[43].blinkOn();
                cout << "vvv" << endl;
            }
             */
            
            if(  mainInterfaceData[5].isInside(ofVec2f(x,y))) {
                synths[activeSynth].currentScaleVecPos++;
                synths[activeSynth].setMusicScale(scaleCollection, synths[activeSynth].currentScaleVecPos%scaleCollection.scaleVec.size() );
                setNewGUI();
                mainInterfaceData[5].blinkOn();
               // detailEditInterfaceOn();
                cout << synths[activeSynth].activeScale.name  << endl;
            }
            
            if(  mainInterfaceData[7].isInside(ofVec2f(x,y))) {
                synths[activeSynth].changePreset();
                //mainInterfaceData[7].elementName = presetNames[synths[activeSynth].preset%presetNames.size()];
                setNewGUI();
                mainInterfaceData[7].blinkOn();
            }
            
            if(  mainInterfaceData[11].isInside(ofVec2f(x,y))) {
                //mainInterfaceData[7].elementName = presetNames[synths[activeSynth].preset%presetNames.size()];
                buttonEditDetail();
                mainInterfaceData[11].blinkOn();
            }
            
            if(  mainInterfaceData[4].isInside(ofVec2f(x,y))) {
                synths[activeSynth].setKeyNote(-12);
                setNewGUI();
                mainInterfaceData[4].blinkOn();
                cout << "-12"  << endl;
            }
            if(  mainInterfaceData[6].isInside(ofVec2f(x,y))) {
                synths[activeSynth].setKeyNote(12);
                setNewGUI();
                mainInterfaceData[6].blinkOn();
                cout << "+12"  << endl;
            }
            
            for (int i = 1; i < 12; i++) {
                
                if (   mainInterfaceData[13+i].isInside(ofVec2f(x,y))) {
                    synths[activeSynth].changeMusicScale(i);
                    mainInterfaceData[13+i].switchColor(mainInterface);
                    cout <<   synths[activeSynth].activeScale.steps[i] <<endl;
                }
            }
            
            
            
            //note in use with slider
            /*
            for (int i = 1; i < 12; i++) {
                
                if (   mainInterfaceData[25+i].isInside(ofVec2f(x,y))) {
                    updateKeyNoteInfo(i);
                    synths[activeSynth].setKeyNote(i);
                    cout <<  "midinote " << synths[activeSynth].keyNote << endl;
                }
            }
            */
            
        }
        
        else   if (currentState == STATE_EDIT) {
            
            
            
            
            if(  mainInterfaceData[43].isInside(ofVec2f(x,y))) {
                buttonTwoPress();
                mainInterfaceData[43].blinkOn();
                cout << "vvv" << endl;
                
            }
           
            
            if(  mainInterfaceData[5].isInside(ofVec2f(x,y))) {
                synths[activeSynth].currentScaleVecPos++;
                synths[activeSynth].setMusicScale(scaleCollection, synths[activeSynth].currentScaleVecPos%scaleCollection.scaleVec.size() );
                setNewGUI();
                mainInterfaceData[5].blinkOn();
                // detailEditInterfaceOn();
                cout << synths[activeSynth].activeScale.name  << endl;
            }
            
            if(  mainInterfaceData[7].isInside(ofVec2f(x,y))) {
                synths[activeSynth].changePreset();
                //mainInterfaceData[7].elementName = presetNames[synths[activeSynth].preset%presetNames.size()];
                setNewGUI();
                mainInterfaceData[7].blinkOn();

            }
            
            if(  mainInterfaceData[44].isInside(ofVec2f(x,y))) {
                buttonEditDetail();
                mainInterfaceData[44].blinkOn();
                cout << "<o>" << endl;
            }
            
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
                    cout << mainInterfaceData[i].elementName << endl;
                }
            }
            
        }
    }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
  
    /*
    if (mainInterfaceData[39].touchDown){
        mainInterfaceData[39].touchDown = !mainInterfaceData[39].touchDown;
    }
    */
    
    if (mainInterfaceData[38].touchDown){
        mainInterfaceData[38].touchDown = !mainInterfaceData[38].touchDown;
    }
    
    if (mainInterfaceData[49].touchDown){
        mainInterfaceData[49].touchDown = !mainInterfaceData[49].touchDown;
    }
    
}

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
    ofDrawBitmapString("Tick Time " + ofToString(bpmTick), 20, 140);
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
    ofVec3f wMouse = tempNode.screenToWorld( ofVec3f(target_.x,target_.y,0.0), ofRectangle(ofPoint(0,0), ofGetWindowWidth(), ofGetWindowHeight()));
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

void ofApp::pulseEvent(float& val) {

    
    timeCounter++;
    
    
    if (timeCounter > TILES) {
        timeCounter = 0;
        for (int i = 0; i < synths.size(); i++) {
            synths[i].nextDirection();
        }
    }
    
    for (int i = 0; i < synths.size(); i++) {
        synths[i].noteTrigger();
        if (synths[i].pulsePlane.stepCounter == ((TILES+1)*4)-1 ){
            synths[i].pulsePlane.stepCounter = 0;
        } else {
        synths[i].pulsePlane.stepCounter++;
        }
    }
    
    getBpmTick();
}

void ofApp::getBpmTick() {
    
    
    bpmTick = ofGetElapsedTimeMillis() - lastTick;
    lastTick = ofGetElapsedTimeMillis();
    
    
    for (int i = 0; i < synths.size(); i++) {
        synths[i].pulsePlane.lastTick = lastTick;
    }
    
    tickTimes.pop_back();
    tickTimes.push_front(bpmTick);
    
    
    
    
    bpmTick = 0.0;
    
    for (int i = 0; i < 5; i++) {
        bpmTick+= tickTimes.at(i);
    }
    
    
    bpmTick /= 5;
    
    

}


void ofApp::volumeRampValueChanged(float & volumeRampValue) {
    tonicSynth.setParameter("mainVolumeRamp", volumeRampValue);
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
    synthActivePos.setPosition(0, -TILES*TILESIZE*4, TILESIZE*TILES*3.5);
    camActiveSynth.setPosition(synthActivePos.getGlobalPosition()+ofVec3f(0,-TILESIZE*TILES*5.5,TILES*TILESIZE*2.2));
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
    
}


void ofApp::setupGlobalInterface() {
    
    int fontDefault = designGrid[0][0].y*0.38;
    int fontSmall = designGrid[0][0].y*0.165;
    
    ofVec3f smallButton = ofVec3f(designGrid[0][0].y,designGrid[0][0].y,0);
    ofVec3f horizontalSlider = ofVec3f(designGrid[0][0].x*2,designGrid[0][0].y,0);
    ofVec3f verticalSlider = ofVec3f(designGrid[0][0].x*2/12,designGrid[0][0].y*2,0);
    ofVec3f keyNoteButton = ofVec3f(designGrid[0][0].x*6/14 ,designGrid[0][0].y,0);
    ofVec3f scaleButton = ofVec3f(designGrid[0][0].x*6/14 ,designGrid[0][0].y,0);

    
    ofVec3f place = ofVec3f(0,-designGrid[0][0].y*2,0);
    ofVec3f offPlace = ofVec3f(0,-designGrid[0][0].y*12,0);
    GlobalGUI temp = GlobalGUI(0,string("Global 100"),ofVec3f(horizontalSlider.x*0.9, horizontalSlider.y,0), ofColor(50,0,0),place,offPlace,fontDefault,false,&robotoCon);
    mainInterfaceData.push_back(temp);
    
    place = ofVec3f(0,-designGrid[0][0].y/2,0);
    offPlace = ofVec3f(0,-designGrid[0][0].y*6,0);
    temp = GlobalGUI(1,string("A 100"),ofVec3f(horizontalSlider.x*0.9, horizontalSlider.y,0),ofColor(51,0,0),place,offPlace,fontDefault,false,&robotoCon);
    mainInterfaceData.push_back(temp);
    temp = GlobalGUI(2,string("B 100"),ofVec3f(horizontalSlider.x*0.9, horizontalSlider.y,0),ofColor(52,0,0),place,offPlace,fontDefault,false,&robotoCon);
    mainInterfaceData.push_back(temp);
    temp = GlobalGUI(3,string("C 100"),ofVec3f(horizontalSlider.x*0.9, horizontalSlider.y,0),ofColor(53,0,0),place,offPlace,fontDefault,false,&robotoCon);
    mainInterfaceData.push_back(temp);
    
    place = ofVec3f(+scaleButton.x/2,-(scaleButton.y/2),0);
    offPlace = ofVec3f(0, -designGrid[0][0].y*12,0);
    temp = GlobalGUI(4,string("<"),ofVec3f(scaleButton.x,scaleButton.y+keyNoteButton.y,0),ofColor(54,0,0),place,offPlace,fontDefault,false,&robotoBold);
    mainInterfaceData.push_back(temp);
    
    offPlace = ofVec3f(designGrid[0][0].x*6,0,0);
    place = ofVec3f(-(horizontalSlider.x*0.86)/2,0,0);
    temp = GlobalGUI(5,string("activeScale"),ofVec3f(horizontalSlider.x*0.86,horizontalSlider.y,0),ofColor(55,0,0),place,offPlace,fontDefault,false,&robotoLight);
    mainInterfaceData.push_back(temp);
    
    place = ofVec3f(-scaleButton.x/2, -(scaleButton.y/2),0);
    offPlace = ofVec3f(0, -designGrid[0][0].y*12,0);
    temp = GlobalGUI(6,string(">"),ofVec3f(scaleButton.x,scaleButton.y+keyNoteButton.y,0),ofColor(56,0,0),place,offPlace,fontDefault,false,&robotoBold);
    mainInterfaceData.push_back(temp);
    
    offPlace = ofVec3f(-designGrid[0][0].x*6,0,0);
    place = ofVec3f((horizontalSlider.x*0.86)/2,0,0);
    temp = GlobalGUI(7,string(presetNames.at(0)),ofVec3f(horizontalSlider.x*0.86,horizontalSlider.y,0),ofColor(57,0,0),place,offPlace,fontDefault,false,&robotoBold);
    mainInterfaceData.push_back(temp);
    
    
    place = ofVec3f(0,designGrid[0][0].y*1.5,0);
    offPlace = ofVec3f(designGrid[0][0].x*12,0,0);
    temp = GlobalGUI(8,string("OnePlayPause"),smallButton*0.5,ofColor(59,0,0),place,offPlace,fontDefault,true,&robotoCon);
    mainInterfaceData.push_back(temp);
    offPlace = ofVec3f(designGrid[0][0].x*6,0,0);
    temp = GlobalGUI(9,string("TwoPlayPause"),smallButton*0.5,ofColor(60,0,0),place,offPlace,fontDefault,true,&robotoCon);
    mainInterfaceData.push_back(temp);
    offPlace = ofVec3f(designGrid[0][0].x*4,0,0);
    temp = GlobalGUI(10,string("ThreePlayPause"),smallButton*0.5,ofColor(61,0,0),place,offPlace,fontDefault,true,&robotoCon);
    mainInterfaceData.push_back(temp);
    
    
    place = ofVec3f(0,-smallButton.y/2,0);
    offPlace = ofVec3f(0,designGrid[0][0].y*6,0);

    temp = GlobalGUI(11,string(""),smallButton,ofColor(62,0,0),place,offPlace,fontDefault,true,&robotoCon);
    mainInterfaceData.push_back(temp);
    temp = GlobalGUI(12,string("SwitchActiveOptions"),smallButton,ofColor(63,0,0),place,offPlace,fontDefault,false,&robotoCon);
    mainInterfaceData.push_back(temp);
    
    
    notes = ofSplitString("C C# D D# E F F# G G# A A# B"," ");
    for (int i = 0 ; i < 12; i++) {
        pianoKeys[i] = 0;
    }
    pianoKeys[1] = keyNoteButton.y;
    pianoKeys[3] = keyNoteButton.y;
    
    pianoKeys[6] = keyNoteButton.y;
    pianoKeys[8] = keyNoteButton.y;
    pianoKeys[10] = keyNoteButton.y;


    
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
    
    place = ofVec3f(0,-smallButton.y/2,0);
    offPlace = ofVec3f(0,designGrid[0][0].y*6,0);
    temp = GlobalGUI(37, string(""), smallButton, ofColor(23,23,23), place, offPlace,fontDefault,true,&robotoCon);
    mainInterfaceData.push_back(temp);
    
    place = ofVec3f(0,0,0);
    offPlace = ofVec3f(-designGrid[0][0].x*12,0,0);
    temp = GlobalGUI(38,"BPM "+ string(ofToString(bpm)), horizontalSlider, ofColor(23,23,23), place, offPlace,fontDefault*1.2,false,&robotoCon);
    mainInterfaceData.push_back(temp);
    
    
    offPlace = ofVec3f(+designGrid[0][0].x*6,0,0);
    place = ofVec3f(0,0,0);
    temp = GlobalGUI(39,string("Pitch_slider"),horizontalSlider,ofColor(55,0,0),place,offPlace,fontDefault,false,&robotoLight);
    mainInterfaceData.push_back(temp);
    
    //toggle 1,2,3
    
    place = ofVec3f(0,designGrid[0][0].y,0);
    offPlace = ofVec3f(0,designGrid[0][0].y*6,0);
    temp = GlobalGUI(40, string("+++"), smallButton, ofColor(23,23,23), place, offPlace,fontSmall,false,&robotoCon);
    mainInterfaceData.push_back(temp);
    
    place = ofVec3f(0,designGrid[0][0].y,0);
    offPlace = ofVec3f(0,designGrid[0][0].y*6,0);
    temp = GlobalGUI(41, string("^^^^^^"), smallButton, ofColor(23,23,23), place, offPlace,fontSmall,false,&robotoCon);
    mainInterfaceData.push_back(temp);
    
    place = ofVec3f(0,designGrid[0][0].y,0);
    offPlace = ofVec3f(0,designGrid[0][0].y*6,0);
    temp = GlobalGUI(42, string(""), smallButton, ofColor(23,23,23), place, offPlace,fontSmall,false,&robotoCon);
    mainInterfaceData.push_back(temp);
    
    
    //toogle from state_edit to state_default
    
    place = ofVec3f(0,-smallButton.y/2,0);
    offPlace = ofVec3f(0,designGrid[0][0].y*6,0);
    temp = GlobalGUI(43, string(""), smallButton, ofColor(23,23,23), place, offPlace,fontSmall,true,&robotoCon);
    mainInterfaceData.push_back(temp);
    
    //toggle edit detail
    
    place = ofVec3f(0,smallButton.y/2,0);
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
    
     place = ofVec3f(0,-designGrid[0][0].y*2,0);
     offPlace = ofVec3f(0,-designGrid[0][0].y*12,0);
    temp = GlobalGUI(54,"",ofVec3f(horizontalSlider.x*0.9, horizontalSlider.y,0),ofColor(51,0,0),place,offPlace,fontDefault,false,&robotoCon);
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
        
        //offset for scale key string
        mainInterfaceData[50].stringHeight-=scaleButton.y/3;
    }
    
    pauseInterfaceOn();
    aniPct = 0.0;
}

void ofApp::editInterfaceOn(){
    
    mainInterfaceData[5].showString = true;
    mainInterfaceData[5].moveDir = 1;
    mainInterfaceData[5].animation = true;
    
    
    mainInterfaceData[7].showString = true;
    mainInterfaceData[7].moveDir = 1;
    mainInterfaceData[7].animation = true;
    
    
    mainInterfaceData[43].showString = false;
    mainInterfaceData[43].moveDir = 1;
    mainInterfaceData[43].animation = true;
    
    mainInterfaceData[44].showString = false;
    mainInterfaceData[44].moveDir = 1;
    mainInterfaceData[44].animation = true;
    
    
    for (int i = 45; i < 45+4; i++) {
        mainInterfaceData[i].moveDir = 1;
        mainInterfaceData[i].animation = true;
    }
    
    if (currentState != STATE_EDIT && currentState != STATE_EDIT_DETAIL ) {
    mainInterfaceData[37].moveDir = 0;
    mainInterfaceData[37].animation = true;
    }
    
}


void ofApp::editInterfaceOff(){
    
    
    mainInterfaceData[7].moveDir = 0;
    mainInterfaceData[7].animation = true;
    
   
    mainInterfaceData[5].moveDir = 0;
    mainInterfaceData[5].animation = true;
    
    
    mainInterfaceData[43].moveDir = 0;
    mainInterfaceData[43].animation = true;
    
    mainInterfaceData[44].moveDir = 0;
    mainInterfaceData[44].animation = true;
    
   
    
    mainInterfaceData[37].showString = false;
    mainInterfaceData[37].moveDir = 1;
    mainInterfaceData[37].animation = true;
    
    for (int i = 45; i < 45+4; i++) {
        mainInterfaceData[i].moveDir = 0;
        mainInterfaceData[i].animation = true;
    }
}

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
    
 
    
    /*
    mainInterfaceData[39].showString = true;
    mainInterfaceData[39].moveDir = 1;
    mainInterfaceData[39].animation = true;
    */
}

void ofApp::volumeInterfacOn() {
    mainInterfaceData[1].sliderWidth = mainInterfaceData[1].maxX- ofMap(synths[synthButton[0]].sVolume, 0.0, 1.0, mainInterfaceData[1].minX, mainInterfaceData[1].maxX);
    mainInterfaceData[2].sliderWidth = mainInterfaceData[2].maxX- ofMap(synths[synthButton[1]].sVolume, 0.0, 1.0, mainInterfaceData[2].minX, mainInterfaceData[2].maxX);
    mainInterfaceData[3].sliderWidth = mainInterfaceData[3].maxX- ofMap(synths[synthButton[2]].sVolume, 0.0, 1.0, mainInterfaceData[3].minX, mainInterfaceData[3].maxX);
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
    
    
    mainInterfaceData[0].showString = true;
    mainInterfaceData[0].moveDir = 1;
    mainInterfaceData[0].animation = true;
}

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

    
    
    mainInterfaceData[0].moveDir = 0;
    mainInterfaceData[0].animation = true;
}

void ofApp::pauseInterfaceOn() {
   
    
    
    mainInterfaceData[8].moveDir = 1;
    mainInterfaceData[8].animation = true;
    
    mainInterfaceData[9].moveDir = 1;
    mainInterfaceData[9].animation = true;
    
    mainInterfaceData[10].moveDir = 1;
    mainInterfaceData[10].animation = true;
    
    if (currentState == !STATE_VOLUME){
    mainInterfaceData[37].showString = false;
    mainInterfaceData[37].moveDir = 1;
    mainInterfaceData[37].animation = true;
    }
    mainInterfaceData[38].showString = true;
    mainInterfaceData[38].moveDir = 1;
    mainInterfaceData[38].animation = true;
    
    /*
    mainInterfaceData[40].showString = true;
    mainInterfaceData[40].moveDir = 1;
    mainInterfaceData[40].animation = true;
    
    mainInterfaceData[41].showString = true;
    mainInterfaceData[41].moveDir = 1;
    mainInterfaceData[41].animation = true;
    
    mainInterfaceData[42].showString = true;
    mainInterfaceData[42].moveDir = 1;
    mainInterfaceData[42].animation = true;
    
    */
    
    
}

void ofApp::pauseInterfaceOff() {
 
   
    
    mainInterfaceData[8].moveDir = 0;
    mainInterfaceData[8].animation = true;
    
    mainInterfaceData[9].moveDir = 0;
    mainInterfaceData[9].animation = true;
    
    mainInterfaceData[10].moveDir = 0;
    mainInterfaceData[10].animation = true;
    
    /*
    mainInterfaceData[37].moveDir = 0;
    mainInterfaceData[37].animation = true;
    */
    mainInterfaceData[38].moveDir = 0;
    mainInterfaceData[38].animation = true;
    
    /*
    mainInterfaceData[40].moveDir = 0;
    mainInterfaceData[40].animation = true;
    
    mainInterfaceData[41].moveDir = 0;
    mainInterfaceData[41].animation = true;
    
    mainInterfaceData[42].moveDir = 0;
    mainInterfaceData[42].animation = true;
     */
}


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

void ofApp::makePresetString() {
    presetNames.push_back("Sinu");
    presetNames.push_back("simple");
    presetNames.push_back("simple2");
    presetNames.push_back("simple3");
    presetNames.push_back("simple4");
    presetNames.push_back("simple5");
  
    
}

void ofApp::makeDesignGrid() {
    
    ofVec2f third = ofVec2f(ofGetWindowWidth()/3,ofGetWindowHeight()/3);
    ofVec2f center = third/2;
    
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            designGrid[i][j] = ofVec2f(third.x*i+center.x,third.y*j+center.y);
        }
    }
}


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
        synthButton[1] = temp;
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
        
        mainInterfaceData[1].setSlider(mainInterface, mainInterfaceData[2].sliderPos);
        mainInterfaceData[2].setSlider(mainInterface,mainInterfaceData[1].sliderPos);

        setNewGUI();
       
        /*
        editInterfaceOn();
        if (currentState == STATE_EDIT_DETAIL){
            detailEditInterfaceOn();
        }
         */
        
        
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
        interfaceMoving = true;
        setNewGUI();
        
        if (currentState == STATE_DEFAULT){
            editInterfaceOn();
            pauseInterfaceOff();
        }
        
        currentState = STATE_EDIT;
    }
}

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
            bothEditInterfaceOff();
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
        synthButton[1] = temp;
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
        
        mainInterfaceData[2].setSlider(mainInterface, mainInterfaceData[3].sliderPos);
        mainInterfaceData[3].setSlider(mainInterface, mainInterfaceData[2].sliderPos);
        
        setNewGUI();
       
        /*
        editInterfaceOn();

        if (currentState == STATE_EDIT_DETAIL){
            detailEditInterfaceOn();
        }
        */
        
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
        interfaceMoving = true;

        setNewGUI();
        
        if (currentState == STATE_DEFAULT){
            editInterfaceOn();
            pauseInterfaceOff();
        }
        
        currentState = STATE_EDIT;
        
    }
}

void ofApp::buttonFourPress() {
    
    if(currentState != STATE_VOLUME && currentState != STATE_EDIT ) {
        
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
        synths[synthButton[0]].myScaleTarget = 0.2;
        synths[synthButton[2]].scaling = true;
        synths[synthButton[2]].myScaleDefault = 0.5;
        synths[synthButton[2]].myScaleTarget = 0.2;


        mainInterfaceData[5].animationB = true;
        mainInterfaceData[7].animationB = true;


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
        synths[synthButton[0]].myScaleDefault = 0.2;
        synths[synthButton[0]].myScaleTarget = 0.5;
        synths[synthButton[2]].scaling = true;
        synths[synthButton[2]].myScaleDefault = 0.2;
        synths[synthButton[2]].myScaleTarget = 0.5;
        
        aniPct = 0.0;
        interfaceMoving = true;
        editDetailMoveDirection = 1;

        mainInterfaceData[5].animationB = true;
        mainInterfaceData[7].animationB = true;
        
        detailEditInterfaceOff();
        aniPct = 0.0;
        currentState = STATE_EDIT;
    }
}

void ofApp::setNewGUI() {

    //scale information
    mainInterfaceData[5].elementName = scaleCollection.scaleVec.at(synths[activeSynth].currentScaleVecPos%scaleCollection.scaleVec.size()).name;
    mainInterfaceData[5].setStringWidth(mainInterfaceData[5].fsPtr->getBBox(mainInterfaceData[5].elementName, mainInterfaceData[5].fontSize, 0, 0).getWidth());
    
    
    for (int i = 0; i < 12; i++) {
        mainInterfaceData[13+i].setColor(synths[activeSynth].colorHue);
        mainInterfaceData[13+i].activateOnColor(mainInterface);
        
        mainInterfaceData[13+i].elementName = notes[  (synths[activeSynth].keyNote+i)%12 ];
        mainInterfaceData[13+i].setStringWidth(mainInterfaceData[13+i].fsPtr->getBBox(mainInterfaceData[13+i].elementName, mainInterfaceData[13+i].fontSize, 0, 0).getWidth());
    }

    for (int i = 0; i < 12; i++) {
        if (synths[activeSynth].activeScale.steps[i] ) {
            mainInterfaceData[13+i].setOn(mainInterface);
        } else {
            mainInterfaceData[13+i].setOff(mainInterface);
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


    
}

ofColor ofApp::filterColor(ofColor c_){
    ofColor temp;
    temp.r = ofClamp(c_.r+20, 20, 230);
    temp.g = ofClamp(c_.g+5, 20, 230);
    temp.b = ofClamp(c_.b-20, 20, 230);
    temp.a = ofClamp(c_.a, 10, 255);

    
    return temp;
}
