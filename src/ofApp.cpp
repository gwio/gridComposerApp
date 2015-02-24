#include "ofApp.h"
#define TILES 9
#define TILESIZE 40
#define TILEBORDER 0.15
#define BPM 130*8


//--------------------------------------------------------------
void ofApp::setup(){
    ofSoundStreamSetup(2, 0, this, 44100, 256, 4);

    
    synths.resize(1);
    synths[0] = Instrument(TILES,TILESIZE,TILEBORDER);
    synths[0].setup(&timeCounter);
    setupAudio();
    globalTranslate = ofVec3f(TILES*TILESIZE,TILES*TILESIZE,0)/-2;
    activeSynth = 0;
    
    ControlGenerator pulse = ControlMetro().bpm(BPM);
    ofEvent<float>* pulseEvent = tonicSynth.createOFEvent(pulse);
    ofAddListener(*pulseEvent, this, &ofApp::pulseEvent );
    
    ofSetFrameRate(60);
    ofEnableDepthTest();
    //ofDisableAntiAliasing();
    ofSetVerticalSync(false);
    
    planeTemp.set(TILES*TILESIZE, TILES*TILESIZE);
    planeTemp.setPosition(0, 0, 0);
    intersecPlane.setFrom(planeTemp);
    
    
    cam.setPosition(0, 0, 970);
    cam.lookAt(ofVec3f(0,0,0));
    cam.setFov(42);
    ofBackground(11, 5, 5);
    fbo.allocate(ofGetWidth(),ofGetHeight(), GL_RGB);
    
    fbo.begin();
    ofClear(0, 0, 0);
    fbo.end();
    
    ofEnableLighting();
    light.setPosition(0, 0, 150);
    light.setAmbientColor(ofColor::orangeRed);
    drawFboImage = false;
    
    doubleClickTime = 300;
    curTap = 0;
    lastTap = 0;
    mouseDragging = false;
    tapCounter = 0;
    drawInfo = false;
    
    timeCounter = 0;
}

void ofApp::setupAudio(){
    Generator temp;
    for (int i = 0; i < synths.size(); i++) {
        temp = temp + synths[i].instrumentOut;
    }
    mainOut = temp;
    tonicSynth.setOutputGen(mainOut*0.3);
}

//--------------------------------------------------------------
void ofApp::update(){
    
   
    
    for (int i = 0; i < synths.size(); i++) {
        synths[i].update();
    }
    
    intersectPlane();
    
}

//--------------------------------------------------------------
void ofApp::draw(){

    

    glShadeModel(GL_SMOOTH);
    //glDisable(GL_MULTISAMPLE);
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
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    setupAudio();
    
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
                                           synths[activeSynth].cubeVector[tempInfo.cubeVecNum].zHeight,
                                           synths[activeSynth].cubeVector[tempInfo.cubeVecNum].cubeColor
                                           );
                
                curMouseId = tapCounter;
                tapMap[tapCounter] = temp;
                synths[activeSynth].layerInfo.at(cordTemp.x).at(cordTemp.y).blocked = true;
                tapCounter++;
                
                cout << "added" << tapCounter << " pos:" << cordTemp << endl;
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
    curTap = ofGetElapsedTimeMillis();
    if ( lastTap != 0 && (curTap-lastTap < doubleClickTime)) {
        
        
        updateFboMesh();
        
        if (lastPickColor != ofColor(0,0,0)) {
            if (synths[activeSynth].cubeMap.find(lastPickColor.getHex()) != synths[activeSynth].cubeMap.end() ) {
                
                ofVec2f cordTemp = synths[activeSynth].cubeMap[lastPickColor.getHex()];
                synths[activeSynth].tapEvent(cordTemp.x,cordTemp.y);
            }
            
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
    vectorPosX = (intersectPos.x/TILESIZE)+TILES/2;
    vectorPosY = (intersectPos.y/TILESIZE)+TILES/2;
    
}




void ofApp::updateFboMesh(){
    
    synths[activeSynth].updateFboMesh();
    
    fbo.begin();
    glDisable(GL_MULTISAMPLE);
    ofDisableLighting();
    ofClear(0,0,0);
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
    for (int i = 0; i < synths.size(); i++) {
        synths[i].readNotes = true;;
    }
    timeCounter++;
    if (timeCounter > TILES+2) {
        timeCounter = 0;
    }
    
}
void ofApp::audioRequested (float * output, int bufferSize, int nChannels){
    tonicSynth.fillBufferOfFloats(output, bufferSize, nChannels);
}
