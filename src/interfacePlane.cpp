
#include "interfacePlane.h"

InterfacePlane::InterfacePlane(){
    
}

InterfacePlane::InterfacePlane(int tiles_) {
    tiles = tiles_;
    stepCounter = 0;
    
    resolution = 2;

    for (int i = 0; i < 4 ; i++) {
        sideRad[i] = HALF_PI*i;
    }
    
    posNode.setPosition(0, 0, 0);
    
    for (int i = 0; i < (tiles+1)*4; i++) {
        ofVec3f temp = ofVec3f( sin( ofMap(i, 0, (tiles+1)*4, 0.0-HALF_PI, TWO_PI-HALF_PI)), cos( ofMap(i, 0, (tiles+1)*4, 0.0-HALF_PI, TWO_PI-HALF_PI)),0 );
        circlePath.addVertex(temp.x*75,temp.y*75 ,0);
    }
    circlePath.close();
    
    for (int i = 0; i < 20; i++) {
        times.push_front(0.0);
    }
    
}


void InterfacePlane::update(int& stepper, float& tickTime_) {
   
    
    thisTime = (ofGetElapsedTimeMillis()- lastTick) ;
  
     tempDir = circlePath.getVertices().at( (stepCounter+1)%((tiles+1)*4) ) - circlePath.getVertices().at(stepCounter);
    

    if (thisTime >= tickTime_ ) {
        len = 1.0;
    } else {
        len =ofMap( fmod(thisTime, tickTime_ ), 0.0  , tickTime_ , 0.0, 1.0);
    }
    
    tempDir*= len;
    
    pctRotate = ofMap(stepCounter+len, 0, (tiles+1)*4, 0.0, 1.0);

    ofMatrix4x4 aaa;
    aaa.rotateRad(ofLerp(0.0, TWO_PI, pctRotate), 0, 0, 1);
    
    float pctScale = ofMap(stepCounter+len,0, (tiles+1)*4,0.0,TWO_PI*2);
    
    
    pulseRot.setRotate(aaa.getRotate());
    
    posNode.setOrientation(pulseRot.getRotate());
    
    posNode.setPosition(circlePath.getVertices().at(stepCounter)+tempDir);
    posNode.setPosition(posNode.getPosition()* ( (abs(sin(pctScale))*0.4) +0.8) );
    posNode.setScale((abs(sin(pctScale-HALF_PI))*1) +1 );
    
   // cout << sin(pctScale) << endl;
  
//    cout <<  tickTime_ << "  " <<  stepCounter << " " << thisTime  << "  " << ofGetElapsedTimeMillis() << "  " << len << endl;

}

void InterfacePlane::draw(){
    
    
    //circlePath.draw();
    ofPushStyle();
    posNode.transformGL();
    
    ofSetColor(255, 255, 255,100);
    ofLine(-8, 0, 8, 0);
    

    posNode.restoreTransformGL();
    ofPopStyle();
    
}