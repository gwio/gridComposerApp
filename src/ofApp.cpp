#include "ofApp.h"
#define TILES 18
#define TILESIZE 20
#define TILEBORDER 0.15


//--------------------------------------------------------------
void ofApp::setup(){
    
    synths.resize(1);
    synths[0] = Instrument(TILES,TILESIZE,TILEBORDER);
    synths[0].setup();
    globalTranslate = ofVec3f(TILES*TILESIZE,TILES*TILESIZE,0)/-2;
    activeSynth = 0;
    
    ofSetFrameRate(60);
    ofEnableDepthTest();
    //ofDisableAntiAliasing();
    ofSetVerticalSync(false);
    
    planeTemp.set(TILES*TILESIZE, TILES*TILESIZE);
    planeTemp.setPosition(0, 0, 0);
    intersecPlane.setFrom(planeTemp);
    
    
    cam.setPosition(0, 0, 1570);
    cam.lookAt(ofVec3f(0,0,0));
    cam.setFov(22);
    ofBackground(11, 5, 5);
    fbo.allocate(ofGetWidth(),ofGetHeight(), GL_RGB);
    
    fbo.begin();
    ofClear(0, 0, 0);
    fbo.end();
    
    ofEnableLighting();
    light.setPosition(0, 0, 300);
    drawFboImage = false;
    
    
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
    
    ofDisableLighting();
    
    glShadeModel(GL_SMOOTH);
    
    ofPushStyle();
    ofDrawBitmapString(ofToString(ofGetFrameRate()), 20, 20);
    ofDrawBitmapString(ofToString(intersectPos), 20,40);
    ofDrawBitmapString(ofToString(vectorPosX), 20, 60);
    ofDrawBitmapString(ofToString(vectorPosY), 20, 80);
    ofDrawBitmapString(ofToString(int(RGB[0]))+" "+ofToString(int(RGB[1]))+" "+ofToString(int(RGB[2])), 20, 100);
    ofPopStyle();
    
    ofEnableLighting();
    
    cam.begin();
    //ofDrawAxis(120);
    //ofRect(0, 0, 800, 800);
    light.enable();
    // planeTemp.draw();
    
    ofPushMatrix();
    ofTranslate(globalTranslate);
    
    for (int i = 0; i < synths.size(); i++) {
        synths[i].draw();
        //synths[i].drawDebug();
    }
    
    ofPopMatrix();
    
    cam.end();
    
    
    // mousePick.draw(ofGetMouseX(),ofGetMouseY());
    
    
    //drawDebug();
    
    ofDisableLighting();
    if(drawFboImage) {
        fbo.draw(0, 0);
    }
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    
    if (key == 's') {
        ofImage pix;
        pix.allocate(ofGetWidth(), ofGetHeight(), OF_IMAGE_COLOR);
        pix.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
        pix.saveImage( ofGetTimestampString()+"debug.png");
    }
    
    if (key == 'd') {
        drawFboImage = !drawFboImage;
    }
    
    if (key == 'r') {
        
        for (int i = 0; i < TILES; i++) {
            for (int j = 0; j < TILES; j++) {
                synths[0].addCube(i, j);
            }
        }
    }
    
    if (key == 'f') {
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
    
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
    updateFboMesh();
    
    ofColor tempC = ofColor(RGB[0],RGB[1],RGB[2]);
    if (tempC != ofColor(0,0,0)) {
        if (synths[activeSynth].cubeMap.find(tempC.getHex()) != synths[activeSynth].cubeMap.end() ) {
            
            
            synths[activeSynth].clickEvent(
                                           synths[activeSynth].cubeMap[tempC.getHex()].x,
                                           synths[activeSynth].cubeMap[tempC.getHex()].y
                                           );
        }
        
    }
    
    /*
     if(pointInsideGrid(intersectPos)) {
     //cout << " ssad"  << endl;
     synths[activeSynth].clickEvent(vectorPosX, vectorPosY);
     }
     */
    
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
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
    
    fbo.allocate(ofGetWidth(),ofGetHeight(), GL_RGB);
    fbo.begin();
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
