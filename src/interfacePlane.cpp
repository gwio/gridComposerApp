
#include "interfacePlane.h"

InterfacePlane::InterfacePlane(){
    
}

InterfacePlane::InterfacePlane(int tiles_) {
    tiles = tiles_;
    stepCounter = 0;
    dirCounter = 0;
    
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
    
    lineMesh.clear();
    
    lineMesh.addVertex(ofVec3f(-54,0,0));
    lineMesh.addColor(ofColor(255,255,255,0));
    lineMesh.addVertex(ofVec3f(-54,0,0));
    lineMesh.addColor(ofColor(255,255,255,0));
    lineMesh.addVertex(ofVec3f(0,54,0));
    lineMesh.addColor(ofColor(255,255,255,0));
    lineMesh.addVertex(ofVec3f(0,54,0));
    lineMesh.addColor(ofColor(255,255,255,0));
    lineMesh.addVertex(ofVec3f(54,0,0));
    lineMesh.addColor(ofColor(255,255,255,0));
    lineMesh.addVertex(ofVec3f(54,0,0));
    lineMesh.addColor(ofColor(255,255,255,0));
    lineMesh.addVertex(ofVec3f(0,-54,0));
    lineMesh.addColor(ofColor(255,255,255,0));
    lineMesh.addVertex(ofVec3f(0,-54,0));
    lineMesh.addColor(ofColor(255,255,255,0));


    lineMesh.setMode(OF_PRIMITIVE_LINES);
    
    lineMeshVerticesDefault.clear();
    lineMeshVerticesDefault.reserve(8);
    
    for (int i = 0; i < lineMesh.getNumVertices(); i++) {
        lineMeshVerticesDefault.push_back(lineMesh.getVertex(i));
    }
    
    lineMeshVerticesTarget.clear();
    lineMeshVerticesTarget.reserve(8);
    lineMeshVerticesTarget.push_back( ofVec3f(0,-50,0));
    lineMeshVerticesTarget.push_back( ofVec3f(0,50,0));
        
    lineMeshVerticesTarget.push_back( ofVec3f(-50,0,0));
    lineMeshVerticesTarget.push_back( ofVec3f(50,0,0));

    lineMeshVerticesTarget.push_back( ofVec3f(0,-50,0));
    lineMeshVerticesTarget.push_back( ofVec3f(0,50,0));
    
    lineMeshVerticesTarget.push_back( ofVec3f(50,0,0));
    lineMeshVerticesTarget.push_back( ofVec3f(-50,0,0));
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
    aaa.rotateRad(ofLerp(0.0, (PI+HALF_PI)*4, pctRotate), 0, 0, 1);
    
    float pctScale = ofMap(stepCounter+len,0, (tiles+1)*4,0.0,TWO_PI*2);
    
    
    pulseRot.setRotate(aaa.getRotate().inverse());
    
    posNode.setOrientation(pulseRot.getRotate());
    
    posNode.setPosition(circlePath.getVertices().at(stepCounter)+tempDir);
    
    
    float scalePct = (abs(sin(pctScale-HALF_PI))*1);
    float thisScale =  ofClamp(pow(scalePct, 6),0.0,1.0);

    
    posNode.setPosition( posNode.getPosition()* ((-thisScale*0.15) +1.1) ) ;
    posNode.setScale( (thisScale*1) +0.8 );
    
       // cout <<  tickTime_ << "  " <<  stepCounter << " " << thisTime  << "  " << ofGetElapsedTimeMillis() << "  " << len << endl;
    //cout <<  fmod( double(pctRotate), 0.25) << endl;
    
     linePct =ofClamp( ofMap(fmod( double(pctRotate), 0.25), 0.0, 0.25, 0.0, 1.25)-0.25, 0.0, 1.0);
    
   // cout  << linePct <<  "  "<<  fmod( double(pctRotate), 0.25) << "  " << stepper  << endl;

    for (int i = 0; i < lineMesh.getNumVertices(); i+=2) {
        
        if( (i == dirCounter*2)  && (stepper != 5) ){
        lineMesh.setVertex(i, lineMeshVerticesDefault.at(i)+(lineMeshVerticesTarget.at(i)*linePct) );
            lineMesh.setColor(i, ofColor(255,255,255,100));
        lineMesh.setVertex(i+1, lineMeshVerticesDefault.at(i+1)+(lineMeshVerticesTarget.at(i+1)*linePct) );
            lineMesh.setColor(i+1, ofColor(255,255,255,100));
        } else {
            lineMesh.setVertex(i, lineMeshVerticesDefault.at(i)+(lineMeshVerticesTarget.at(i)*1));
            lineMesh.setColor(i, lineMesh.getColor(i).lerp(ofColor(255,255,255,0), 0.2) );
            lineMesh.setVertex(i+1, lineMeshVerticesDefault.at(i+1)+(lineMeshVerticesTarget.at(i+1)*1 ));
            lineMesh.setColor(i+1, lineMesh.getColor(i+1).lerp(ofColor(255,255,255,0), 0.2) );

   
        }
    }
    
}

void InterfacePlane::draw(){
    
   // circlePath.draw();
    ofPushStyle();
    posNode.transformGL();
    
    ofSetColor(255, 255, 255,100);
    ofLine(-10, 0, 10, 0);
    

    posNode.restoreTransformGL();
    ofPopStyle();
    lineMesh.draw();

}