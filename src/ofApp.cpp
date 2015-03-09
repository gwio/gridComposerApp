#include "ofApp.h"
#define TILES 9
#define TILESIZE 45
#define TILEBORDER 0.1
#define BPM 130*16


//--------------------------------------------------------------
void ofApp::setup(){
    ofSoundStreamSetup(2, 0, this, 44100, 256, 4);
    
    //setup synth layers first
    
    synths.resize(1);
    synths[0] = Instrument("a",TILES,TILESIZE,TILEBORDER);
    synths[0].setup(&timeCounter, &tonicSynth);
    globalTranslate = ofVec3f(TILES*TILESIZE,TILES*TILESIZE,0)/-2;
    activeSynth = 0;
    
    
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
    
    planeTemp.set(TILES*TILESIZE, TILES*TILESIZE);
    planeTemp.setPosition(0, 0, 0);
    intersecPlane.setFrom(planeTemp);
    
    
    cam.setPosition(0, 0, 180);
    cam.lookAt(ofVec3f(0,0,0));
    cam.setFov(48);
    ofBackground(11, 65, 65);
    fbo.allocate(ofGetWidth(),ofGetHeight(), GL_RGB);
    
    fbo.begin();
    ofClear(0, 0, 0);
    fbo.end();
    
    ofEnableLighting();
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
    
    intersectPlane();

    
   
}

//--------------------------------------------------------------
void ofApp::draw(){
   
    
    
    glShadeModel(GL_SMOOTH);
    //glDisable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_MULTISAMPLE);

    ofEnableLighting();
    
    cam.begin();
    light.enable();
    // planeTemp.draw();
    
    ofPushMatrix();
    ofTranslate(globalTranslate);
    
    for (int i = 0; i < synths.size(); i++) {
        synths[i].draw();
        // synths[i].drawDebug();
    }
    
    ofPopMatrix();
    
    cam.end();
    
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
                    synths[0].tapEvent(i, j);
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
        synths[activeSynth].activeDirection[0] =  !synths[activeSynth].activeDirection[0] ;
    }
    if (key =='2') {
        synths[activeSynth].activeDirection[1] =  !synths[activeSynth].activeDirection[1] ;
    }
    if (key =='3') {
        synths[activeSynth].activeDirection[2] =  !synths[activeSynth].activeDirection[2] ;
    }
    if (key =='4') {
        synths[activeSynth].activeDirection[3] =  !synths[activeSynth].activeDirection[3] ;
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
    
    cam.begin();
    ofPushMatrix();
    ofTranslate(globalTranslate);
    synths[activeSynth].drawDebug();
    ofPopMatrix();
    cam.end();
    
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
    worldMouse = cam.screenToWorld(ofVec3f(ofGetMouseX(),ofGetMouseY(),0.0));
    
    mouseRay.s = worldMouse;
    mouseRay.t = worldMouse-cam.getPosition();
    
    intersecPlane.intersect(mouseRay, intersectPos);
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
    
    cam.begin();
    ofDisableLighting();
    light.disable();
    
    ofPushMatrix();
    ofTranslate(globalTranslate);
    synths[activeSynth].drawFbo();
    ofPopMatrix();
    cam.end();
    
    glReadPixels(ofGetMouseX(),ofGetMouseY(), 1,1, GL_RGB, GL_UNSIGNED_BYTE, RGB);
    fbo.end();
    lastPickColor = ofColor(RGB[0],RGB[1],RGB[2]);
    
    //cout << lastPickColor << endl;
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
