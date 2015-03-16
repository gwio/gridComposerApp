#include "ofApp.h"
#define TILES 7
#define TILESIZE 100/TILES
#define TILEBORDER 0.12
#define BPM 130*4
#define ANI_SPEED 0.022;


//--------------------------------------------------------------
void ofApp::setup(){
    ofSoundStreamSetup(2, 0, this, 44100, 256, 4);
    
    synthPos.resize(3);
    
    for (int i = -1; i < 2; i++) {
        ofNode temp;
        temp.setPosition( i*(TILES*TILESIZE*2), 0, 0);
        //temp.setOrientation(ofVec3f(0,0,1));
        synthPos[i+1]=temp;
    }
    
    synths.resize(3);
    
    synths[0] = Instrument("a",TILES,TILESIZE,TILEBORDER);
    synths[0].setup(&timeCounter, &tonicSynth, synthPos[0]);
    
    synths[1] = Instrument("b",TILES,TILESIZE,TILEBORDER);
    synths[1].setup(&timeCounter, &tonicSynth, synthPos[1]);
    
    synths[2] = Instrument("c",TILES,TILESIZE,TILEBORDER);
    synths[2].setup(&timeCounter, &tonicSynth, synthPos[2]);
    
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
    
    ofSetFrameRate(60);
    //ofDisableAntiAliasing();
    ofSetVerticalSync(true);
    ofEnableDepthTest();
    
    
    intersecPlane.resize(3);
    for (int i = 0; i < 3 ; i++) {
        ofPlanePrimitive temp;
        temp.set(TILES*TILESIZE, TILES*TILESIZE);
        temp.setPosition(synthPos[i].getPosition());
        intersecPlane.at(i).setFrom(temp);
    }
    
    
    setupPathAndAnimation();
    
    ofBackground(11, 65, 65);
    fbo.allocate(ofGetWidth(),ofGetHeight(), GL_RGB);
    
    fbo.begin();
    ofClear(0, 0, 0);
    fbo.end();
    
    //   ofEnableLighting();
    light.setPosition(0, 0, 140);
    
    
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
    
    debugCam = false;
    
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
        synths[i].planeMovement(aniPct);
        
        if (synths[i].synthHasChanged == true){
            setupAudio();
            synths[i].synthHasChanged = false;
        }
    }
    
    //for animation
    if (aniPct <= 1.0) {
        aniPct += ANI_SPEED;
    }
    
    if (aniCam <= 1.0) {
        aniCam += ANI_SPEED;
    }
    
    updateCamera();
    intersectPlane();
    
    
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    
    
    glShadeModel(GL_SMOOTH);
   // glDisable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    
    glEnable(GL_MULTISAMPLE);
    
    //  ofEnableLighting();
    
    
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
    
    //  ofDisableLighting();
    if (drawInfo) {
        drawDebug();
    }
    if(drawFboImage) {
        fbo.draw(0, 0);
    }
    
    
    if (showGui) {
        guiFbo.draw(0,0);
    }
    
    
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
            
            aniPct = 0.0;
            
            
            //camani
            camQuatDefault = camNotActiveSynth.getOrientationQuat();
            camQuatTarget = camActiveSynth.getOrientationQuat();
            camUsePath = camPath;
            camTargetFov = camActiveFov;
            camDefaultFov = camFov;
            animCam = true;
            aniCam = 0.0;
            
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
            
            aniPct = 0.0;
            
            
            
            //camani
            camQuatDefault = camActiveSynth.getOrientationQuat();
            camQuatTarget = camNotActiveSynth.getOrientationQuat();
            camUsePath = camPathBack;
            camTargetFov = camFov;
            camDefaultFov = camActiveFov;
            animCam = true;
            aniCam = 0.0;
            
            
            
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
            
            
            aniPct = 0.0;
            
            
            //cam
            camQuatDefault = camNotActiveSynth.getOrientationQuat();
            camQuatTarget = camActiveSynth.getOrientationQuat();
            camUsePath = camPath;
            camDefaultFov = camFov;
            camTargetFov = camActiveFov;
            animCam = true;
            aniCam = 0.0;
            
            
            
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
            
            aniPct = 0.0;
            
            
            //camera
            camQuatDefault = camNotActiveSynth.getOrientationQuat();
            camQuatTarget = camActiveSynth.getOrientationQuat();
            camUsePath = camPath;
            camDefaultFov = camFov;
            camTargetFov = camActiveFov;
            animCam = true;
            aniCam = 0.0;
            
            
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




void ofApp::updateFboMesh(){
    
    
    
    synths[activeSynth].updateFboMesh();
    
    fbo.begin();
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
    fbo.end();
    lastPickColor = ofColor(RGB[0],RGB[1],RGB[2]);
    
    cout << lastPickColor  << endl;
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


void ofApp::updateCamera(){
    
    if (animCam && aniCam < 0.99 ) {
        
        float inOut = easeInOut(aniCam, 0.62);
        
        float index = camUsePath.getIndexAtPercent(inOut);
        ofVec3f tempPos =  (camUsePath.getVertices().at((int)index+1)-camUsePath.getVertices().at((int)index))* (index-floor(index));
        testCam.setPosition( camUsePath.getVertices().at((int)index)+ tempPos);
        
        testCam.setFov(ofLerp(camDefaultFov, camTargetFov, inOut));
        
        ofQuaternion tempRot;
        tempRot.slerp(inOut, camQuatDefault,camQuatTarget);
        testCam.setOrientation(tempRot);
    }
    
    if (animCam && aniCam >=1.0) {
        animCam = false;
     //    testCam.lookAt(synths[activeSynth].myNode.getPosition());
    //     testCam.setPosition(camUsePath.getVertices().at(camUsePath.size()-1));
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

void ofApp::setupPathAndAnimation() {
    cam.setNearClip(10);
    cam.setFarClip(51000);
    cam.setFov(20);
    
    testCam.setNearClip(10);
    testCam.setFarClip(51000);
    camActiveFov = 20;
    camFov = 30;
    
    float bezierHandleFac = 2.5;
    
    synthActivePos.setPosition(0, -TILES*TILESIZE*4, TILESIZE*TILES*3.5);
    camActiveSynth.setPosition(synthActivePos.getGlobalPosition()+ofVec3f(0,-TILESIZE*TILES*5.0,TILES*TILESIZE*2.2));
    camNotActiveSynth.setPosition(0, -TILES*TILESIZE*2, TILES*TILESIZE*7);
    
    camActiveSynth.lookAt(synthActivePos.getPosition() - camActiveSynth.getZAxis());
    camNotActiveSynth.lookAt(synthPos[1].getPosition() - camNotActiveSynth.getZAxis());
    //  synthActivePos.lookAt(  camActiveSynth.getPosition() - synthActivePos.getZAxis() );
    synthActivePos.setOrientation(camActiveSynth.getOrientationQuat());
    
    
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
    
    
    
    
    testCam.setPosition(camNotActiveSynth.getPosition());
    testCam.setOrientation(camNotActiveSynth.getOrientationQuat());
    testCam.setFov(camFov);
    
}

