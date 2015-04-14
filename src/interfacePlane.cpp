
#include "interfacePlane.h"

InterfacePlane::InterfacePlane(){
    
}

InterfacePlane::InterfacePlane(int tiles_) {
    tiles = tiles_;
    stepCounter = 0;
    dirCounter = 0;
    
    resolution = 2;

   
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
    
    lineMesh.addVertex(ofVec3f(-54,-50,0));
    lineMesh.addColor(ofColor(255,255,255,0));
    lineMesh.addVertex(ofVec3f(-54,50,0));
    lineMesh.addColor(ofColor(255,255,255,0));
    lineMesh.addVertex(ofVec3f(-50,54,0));
    lineMesh.addColor(ofColor(255,255,255,0));
    lineMesh.addVertex(ofVec3f(50,54,0));
    lineMesh.addColor(ofColor(255,255,255,0));
    lineMesh.addVertex(ofVec3f(54,50,0));
    lineMesh.addColor(ofColor(255,255,255,0));
    lineMesh.addVertex(ofVec3f(54,-50,0));
    lineMesh.addColor(ofColor(255,255,255,0));
    lineMesh.addVertex(ofVec3f(50,-54,0));
    lineMesh.addColor(ofColor(255,255,255,0));
    lineMesh.addVertex(ofVec3f(-50,-54,0));
    lineMesh.addColor(ofColor(255,255,255,0));


    lineMesh.setMode(OF_PRIMITIVE_LINES);
    
    lineMeshVertices.clear();
    lineMeshVertices.reserve(8);
    
    for (int i = 0; i < lineMesh.getNumVertices(); i++) {
        lineMeshVertices.push_back(lineMesh.getVertex(i));
    }
    
   
}


void InterfacePlane::update(int& stepper, float& tickTime_, int& scanDir_) {
   
    
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

    int looper = scanDir_*2;
    
    
            lineMesh.setVertex( (looper+6)%8 , lineMeshVertices.at((looper+7)%8)+ (lineMeshVertices.at((looper+6)%8)-lineMeshVertices.at((looper+7)%8)) * linePct );
            lineMesh.setVertex( (looper+3)%8 , lineMeshVertices.at((looper+2)%8)+ (lineMeshVertices.at((looper+3)%8)-lineMeshVertices.at((looper+2)%8)) * linePct );
    lineMesh.setVertex( (looper+2)%8 , lineMeshVertices.at((looper+2)%8)  + (lineMeshVertices.at((looper+3)%8)-lineMeshVertices.at((looper+2)%8)) * linePct / 4);
    lineMesh.setVertex( (looper+7)%8 , lineMeshVertices.at((looper+7)%8) + (lineMeshVertices.at((looper+6)%8)-lineMeshVertices.at((looper+7)%8)) * linePct / 4);

    lineMesh.setColor((looper+7)%8, ofColor(255,255,255,0));
    lineMesh.setColor((looper+2)%8, ofColor(255,255,255,0));
    
    lineMesh.setColor((looper+6)%8, ofColor(255,255,255,150));
    lineMesh.setColor((looper+3)%8, ofColor(255,255,255,150));


    

            /*
            lineMesh.setVertex(i, lineMeshVertices.at(i));
            lineMesh.setColor(i, ofColor(255,255,255,100));
            lineMesh.setVertex(i+1, lineMeshVertices.at(i+1));
            lineMesh.setColor(i+1, ofColor(255,255,255,100));

            
            lineMesh.setColor((i+4)%8, ofColor(255,255,255,0));
            lineMesh.setColor((i+5)%8, ofColor(255,255,255,0));

            lineMesh.setColor((i+3)%8, ofColor(255,255,255,0));
            lineMesh.setColor((i+6)%8, ofColor(255,255,255,0));
*/
            
      
       
    lineAlpha = abs(sin(pctScale-HALF_PI))*150;
        
    
}

void InterfacePlane::draw(){
    
   // circlePath.draw();
    ofPushStyle();
    posNode.transformGL();
    
    ofSetColor(255, 255, 255,lineAlpha);
    ofLine(-10, 0, 10, 0);
    

    posNode.restoreTransformGL();
    ofPopStyle();
    lineMesh.draw();

}