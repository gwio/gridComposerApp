#include "ofApp.h"
#define TILES 18
#define TILESIZE 10
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
    // ofDisableAntiAliasing();
    ofSetVerticalSync(false);
    setupPickingGrid();
    
    planeTemp.set(TILES*TILESIZE, TILES*TILESIZE);
    planeTemp.setPosition(0, 0, 0);
    intersecPlane.setFrom(planeTemp);
    
    glShadeModel(GL_FLAT);
    
    cam.setPosition(0, 0, 1570);
    cam.lookAt(ofVec3f(0,0,0));
    cam.setFov(10);
    ofBackground(11, 5, 5);
    
  //  selection.allocate(100,100, GL_RGB);
    fbo.allocate(1280,768, GL_RGB);
    
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
    
    
    fbo.begin();
    ofClear(0);
    cam.begin();
    ofDisableLighting();
    //ofDrawAxis(120);
    //ofRect(0, 0, 800, 800);
    
    // planeTemp.draw();
    
    ofPushMatrix();
    ofTranslate(globalTranslate);
    
    for (int i = 0; i < synths.size(); i++) {
      //  synths[i].drawFbo();
    }
    
    ofPopMatrix();
    cam.end();
    
    fbo.end();
    
    fbo.getTextureReference().readToPixels(selection);
    
    fboImage.setFromPixels(selection);
    fboImage.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofPushStyle();
    ofDrawBitmapString(ofToString(ofGetFrameRate()), 20, 20);
    ofDrawBitmapString(ofToString(intersectPos), 20,40);
    ofDrawBitmapString(ofToString(vectorPosX), 20, 60);
    ofDrawBitmapString(ofToString(vectorPosY), 20, 80);
    
    ofPopStyle();
    
    cam.begin();
    //ofDrawAxis(120);
    //ofRect(0, 0, 800, 800);
   // light.enable();
    // planeTemp.draw();
    
    ofPushMatrix();
    ofTranslate(globalTranslate);
    //drawDebug();
    
    for (int i = 0; i < synths.size(); i++) {
        synths[i].draw();
        //synths[i].drawDebug();
    }
    
    ofPopMatrix();
    cam.end();
    
    
    if (drawFboImage)
    fboImage.draw(0, 0);
    
    
    
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
    
    if(pointInsideGrid(intersectPos)) {
        //cout << " ssad"  << endl;
        synths[activeSynth].clickEvent(vectorPosX, vectorPosY);
        
        
        cout <<  selection.getColor(x, y).getHex() << endl;
        
    }
    
    
  
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

void ofApp::setupPickingGrid(){
    pickingRaster.clear();
    pickingRaster.setMode(OF_PRIMITIVE_TRIANGLES);
    int rCounter = 1;
    int gCounter = 1;
    int bCounter = 1;
    for (int i = 0; i < TILES+1; i++) {
        for (int j = 0; j < TILES+1; j++) {
            ofVec3f tPoint = ofVec3f(i*TILESIZE,j*TILESIZE,0);
            pickingRaster.addVertex(tPoint);
            pickingRaster.addColor(ofColor(rCounter,gCounter,bCounter));
            if (rCounter%255==0) {
                rCounter=1;
                gCounter++;
            }
            
            if (gCounter%255==0) {
                gCounter=1;
                bCounter++;
            }
            rCounter++;
        }
    }
    
    
    for (int i = 0; i < TILES; i++) {
        for (int j = 0; j < TILES; j++) {
            int numVert = i*(TILES+1)+j;
            
            pickingRaster.addIndex(numVert);
            pickingRaster.addIndex(numVert+1);
            pickingRaster.addIndex(numVert+1+TILES);
            
            pickingRaster.addIndex(numVert+1);
            pickingRaster.addIndex(numVert+1+(TILES+1));
            pickingRaster.addIndex(numVert+1+TILES);
            
        }
    }
}

void ofApp::drawDebug() {
    pickingRaster.draw();
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

bool ofApp::pointInsideGrid(ofVec3f p_) {
    bool rVal;
    if( (abs(p_.x) <= (TILES*TILESIZE/2) ) && (abs(p_.y) <= (TILES*TILESIZE/2)) ) {
        rVal = true;
    } else {
        rVal = false;
    }
    return rVal;
}
