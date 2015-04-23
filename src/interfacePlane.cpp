
#include "interfacePlane.h"

InterfacePlane::InterfacePlane(){
    
}

InterfacePlane::InterfacePlane(int tiles_, float tileSize_) {
    for (int i = 0; i < 4; i++) {
        nextDirs[i] = 1;
    }
    
    tiles = tiles_;
    stepCounter = 0;
    barCounter = 0;
    
    tileSize = tileSize_;
    
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
    
    float zH = 2.55;
    //pulse lines
    lineMesh.addVertex(ofVec3f(-50,-50,zH));
    lineMesh.addColor(ofColor(255,255,255,0));
    lineMesh.addVertex(ofVec3f(-50,50,zH));
    lineMesh.addColor(ofColor(255,255,255,0));
    lineMesh.addVertex(ofVec3f(50,+50,zH));
    lineMesh.addColor(ofColor(255,255,255,0));
    lineMesh.addVertex(ofVec3f(50,-50,zH));
    lineMesh.addColor(ofColor(255,255,255,0));
    
    
    
    /*
     lineMesh.addVertex(ofVec3f(-54,0,0));
     lineMesh.addColor(ofColor(255,255,255,100));
     lineMesh.addVertex(ofVec3f(-74,0,0));
     lineMesh.addColor(ofColor(255,255,255,100));
     
     lineMesh.addVertex(ofVec3f(0,-54,0));
     lineMesh.addColor(ofColor(255,255,255,100));
     lineMesh.addVertex(ofVec3f(0,-74,0));
     lineMesh.addColor(ofColor(255,255,255,100));
     
     lineMesh.addVertex(ofVec3f(54,0,0));
     lineMesh.addColor(ofColor(255,255,255,100));
     lineMesh.addVertex(ofVec3f(74,0,0));
     lineMesh.addColor(ofColor(255,255,255,100));
     
     lineMesh.addVertex(ofVec3f(0,54,0));
     lineMesh.addColor(ofColor(255,255,255,100));
     lineMesh.addVertex(ofVec3f(0,74,0));
     lineMesh.addColor(ofColor(255,255,255,100));
     */
    
    
    
    //connect lines
    
    lineMesh.setMode(OF_PRIMITIVE_LINES);
    
    lineMeshVertices.clear();
    lineMeshVertices.reserve(8*2);
    
    for (int i = 0; i < lineMesh.getNumVertices(); i++) {
        lineMeshVertices.push_back(lineMesh.getVertex(i));
    }
    
    
    pulseLine.addVertex(ofVec3f(-50,-50,0));
    pulseLine.addVertex(ofVec3f(-50,-50,0));;
    pulseLine.addColor(ofColor(255,255,255,0));
    pulseLine.addColor(ofColor(255,255,255,100));
    pulseLine.setMode(OF_PRIMITIVE_LINES);
    
    
    //direction lines
    directionMesh.setMode(OF_PRIMITIVE_TRIANGLES);
    
    
    for (int j = 1; j < tiles; j++) {
        directionMesh.addVertex(lineMeshVertices.at(0)+ ofVec3f(-10,0,0) + ofVec3f(0,tileSize*j-4,0) );
        directionMesh.addVertex(lineMeshVertices.at(0)+ ofVec3f(-10,0,0) + ofVec3f(0,tileSize*j+4,0) );
        directionMesh.addVertex(lineMeshVertices.at(0)+ ofVec3f(0,tileSize*j,0));
        
        directionMesh.addColor(ofColor(255,255,255,80));
        directionMesh.addColor(ofColor(255,255,255,80));
        directionMesh.addColor(ofColor(255,255,255,80));
        
    }
    
    for (int j = 1; j < tiles; j++) {
        directionMesh.addVertex(lineMeshVertices.at(1)+ ofVec3f(0,10,0) + ofVec3f(tileSize*j-4,0,0) );
        directionMesh.addVertex(lineMeshVertices.at(1)+ ofVec3f(0,10,0) + ofVec3f(tileSize*j+4,0,0) );
        directionMesh.addVertex(lineMeshVertices.at(1)+ ofVec3f(tileSize*j,0,0));
        
        directionMesh.addColor(ofColor(255,255,255,80));
        directionMesh.addColor(ofColor(255,255,255,80));
        directionMesh.addColor(ofColor(255,255,255,80));
        
    }
    
    for (int j = 1; j < tiles; j++) {
        directionMesh.addVertex(lineMeshVertices.at(2)+ ofVec3f(10,0,0) + ofVec3f(0,-tileSize*j-4,0) );
        directionMesh.addVertex(lineMeshVertices.at(2)+ ofVec3f(10,0,0) + ofVec3f(0,-tileSize*j+4,0) );
        directionMesh.addVertex(lineMeshVertices.at(2)+ ofVec3f(0,-tileSize*j,0));
        
        directionMesh.addColor(ofColor(255,255,255,80));
        directionMesh.addColor(ofColor(255,255,255,80));
        directionMesh.addColor(ofColor(255,255,255,80));
        
    }
    
    
    for (int j = 1; j < tiles; j++) {
        directionMesh.addVertex(lineMeshVertices.at(3)+ ofVec3f(0,-10,0) + ofVec3f(-tileSize*j-4,0,0) );
        directionMesh.addVertex(lineMeshVertices.at(3)+ ofVec3f(0,-10,0) + ofVec3f(-tileSize*j+4,0,0) );
        directionMesh.addVertex(lineMeshVertices.at(3)+ ofVec3f(-tileSize*j,0,0));
        
        directionMesh.addColor(ofColor(255,255,255,80));
        directionMesh.addColor(ofColor(255,255,255,80));
        directionMesh.addColor(ofColor(255,255,255,80));
        
    }
    
}


void InterfacePlane::update(int& stepper, float& tickTime_, int& scanDir_, bool connected_[], bool active_[]) {
    
    
    thisTime = (ofGetElapsedTimeMillis()- lastTick) ;
    
    tempDir = circlePath.getVertices().at( (stepCounter+1)%((tiles+1)*4) ) - circlePath.getVertices().at(stepCounter);
    
    
    if (thisTime >= tickTime_ ) {
        len = 1.0;
    } else {
        len =ofMap( fmod(thisTime, tickTime_ ), 0.0  , tickTime_ , 0.0, 1.0);
    }
    
    tempDir*= len;
    
    float alphaPart = TWO_PI/((tiles+1)*4);
    
    pctBar = ofMap(stepper+len, 0, tiles+1, 0.0, 1.0);
    float pctQuarter = fmod(double(pctBar), 0.25);
    
    ofMatrix4x4 aaa;
    
    float alpha;

    int forwardTiles = 0;
    float lastAlpha;
    if (nextDirs[1] && nextDirs[0]) {
        alpha = ofMap( stepper, 0,  (tiles+1), 0.0, HALF_PI) + scanDir_*HALF_PI;
        alpha -= HALF_PI;
          alpha += len*alphaPart;
        aaa.rotateRad(ofLerp(0.0, PI+HALF_PI,pctBar)+scanDir_*HALF_PI, 0, 0, 1) ;
        // move normal +1
    }else if (!nextDirs[1]){
        if(nextDirs[2]) {
            //move +2
            alpha =   ofMap( stepper, 0,  (tiles+1), 0.0, HALF_PI)  + scanDir_*HALF_PI;
            alpha += ofMap( stepper, 0,  (tiles+1), 0.0, HALF_PI);
            alpha -= HALF_PI;
            alpha+= len*(alphaPart*2);
            //alpha +=  ofMap( stepper, 0,  (tiles+1), -HALF_PI, 0.0) ;
            aaa.rotateRad(ofLerp(0.0, PI+HALF_PI,pctBar)+(scanDir_*HALF_PI)+(HALF_PI*pctBar), 0, 0, 1) ;
            // alphaPos = ofVec3f(sin(alpha)*75, cos(alpha)*75,0 );
            
        } else if (nextDirs[3]) {
            alpha =   ofMap( stepper, 0,  (tiles+1), 0.0, HALF_PI)  + scanDir_*HALF_PI;
            alpha += ofMap( stepper, 0,  (tiles+1), 0.0, HALF_PI)*2;
            alpha -= HALF_PI;
            alpha+= len*(alphaPart*3);
            aaa.rotateRad(ofLerp(0.0, PI+HALF_PI,pctBar)+(scanDir_*HALF_PI)+((HALF_PI*pctBar)*2), 0, 0, 1) ;

        } else {
            alpha =   ofMap( stepper, 0,  (tiles+1), 0.0, HALF_PI)  + scanDir_*HALF_PI;
            alpha += ofMap( stepper, 0,  (tiles+1), 0.0, HALF_PI)*3;
            alpha -= HALF_PI;
            alpha+= len*(alphaPart*4);
            aaa.rotateRad(ofLerp(0.0, PI+HALF_PI,pctBar)+(scanDir_*HALF_PI)+((HALF_PI*pctBar)*3), 0, 0, 1) ;
            
            //no move
        }
        
    }
    
    
    for (int i = 0; i < 4; i++){
        
        cout << nextDirs[i] << "  " ;
    }
    cout << endl;
    
    
  //  aaa.rotateRad(ofLerp(0.0, (HALF_PI+PI)*4, pctBar)-alphaPart, 0, 0, 1);
    ofVec3f alphaPos = ofVec3f(sin(alpha)*75, cos(alpha)*75,0 );

    
    float pctScale = ofMap(stepCounter+len,0, (tiles+1)*4,0.0,TWO_PI*2);
    
    
    pulseRot.setRotate(aaa.getRotate().inverse());
    
    posNode.setOrientation(pulseRot.getRotate());
    
   //posNode.setPosition(circlePath.getVertices().at(stepCounter)+tempDir);
    posNode.setPosition(alphaPos);
    
    float scalePct = (abs(sin(pctScale-HALF_PI))*1);
    float thisScale =  ofClamp(pow(scalePct, 6),0.0,1.0);
    

    
  //  posNode.setPosition( posNode.getPosition()* ((-thisScale*0.15) +1.1) ) ;
   // posNode.setScale( (thisScale*1) +0.8 );
    
    
    
    if (scanDir_ == 0 && (active_[0] && connected_[0]) ) {
        pulseLine.setVertex(0, lineMeshVertices.at(0) +  (lineMeshVertices.at(3)-lineMeshVertices.at(0))*linePct/4);
        pulseLine.setVertex(1, lineMeshVertices.at(0) + (lineMeshVertices.at(3)-lineMeshVertices.at(0))*linePct);
        
    } else if (scanDir_ == 1 && (active_[1] && connected_[1])) {
        pulseLine.setVertex(0, lineMeshVertices.at(1)+ (lineMeshVertices.at(0)-lineMeshVertices.at(1))*linePct/4);
        pulseLine.setVertex(1, lineMeshVertices.at(1) + (lineMeshVertices.at(0)-lineMeshVertices.at(1))*linePct);
        
        
    } else if (scanDir_ == 2 && (active_[2] && connected_[2])) {
        pulseLine.setVertex(0, lineMeshVertices.at(2)+ (lineMeshVertices.at(1)-lineMeshVertices.at(2))*linePct/4);
        pulseLine.setVertex(1, lineMeshVertices.at(2) + (lineMeshVertices.at(1)-lineMeshVertices.at(2))*linePct);
        
        
    } else if (scanDir_ == 3 && (active_[3] && connected_[3])) {
        pulseLine.setVertex(0, lineMeshVertices.at(3)+ (lineMeshVertices.at(2)-lineMeshVertices.at(3))*linePct/4);
        pulseLine.setVertex(1, lineMeshVertices.at(3) + (lineMeshVertices.at(2)-lineMeshVertices.at(3))*linePct);
        
    } else {
        pulseLine.setVertex(0, ofVec3f(2000,2000,2000));
        pulseLine.setVertex(1, ofVec3f(2000,2000,2000));
        pulseLine.setColor(0, ofColor(255,255,255,0));
        pulseLine.setColor(1, ofColor(255,255,255,0));
    }
    
    
    
    
    if (stepper == 5) {
        
        // pulseLine.setColor(0, pulseLine.getColor(0).lerp(ofColor(255,255,255,0), 0.2));
        pulseLine.setColor(1, pulseLine.getColor(1).lerp(ofColor(255,255,255,0), 0.1));
    } else {
        pulseLine.setColor(0, ofColor(255,255,255,0));
        pulseLine.setColor(1, ofColor(255,255,255,100));
    }
    
    // cout <<  tickTime_ << "  " <<  stepCounter << " " << thisTime  << "  " << ofGetElapsedTimeMillis() << "  " << len << endl;
    //cout <<  fmod( double(pctBar), 0.25) << endl;
    
    
    //  if (stepper != 5) {
    linePct =ofClamp( ofMap(fmod( double(pctBar), 0.25), 0.0, 0.25, 0.0, 1.25)-0.25, 0.0, 1.0);
    
    
    
    //set active[]
    if (!active_[0]) {
        for (int i = (0*((tiles-1)*3)) ; i < (0*((tiles-1)*3))+((tiles-1)*3); i++) {
            directionMesh.setColor(i,  directionMesh.getColor(i).lerp(ofColor(255,100,100,30), 0.02));
        }
    } else {
        for (int i = (0*((tiles-1)*3)) ; i < (0*((tiles-1)*3))+((tiles-1)*3); i++) {
            directionMesh.setColor(i, directionMesh.getColor(i).lerp(ofColor(200,255,200,0), 0.02));
        }
    }
    
    if (!active_[1]) {
        for (int i = (1*((tiles-1)*3)) ; i < (1*((tiles-1)*3))+((tiles-1)*3); i++) {
            directionMesh.setColor(i,  directionMesh.getColor(i).lerp(ofColor(255,100,100,30), 0.02));
        }
    } else {
        for (int i = (1*((tiles-1)*3)) ; i < (1*((tiles-1)*3))+((tiles-1)*3); i++) {
            directionMesh.setColor(i, directionMesh.getColor(i).lerp(ofColor(200,255,200,0), 0.02));
        }
    }
    
    
    if (!active_[2]) {
        for (int i = (2*((tiles-1)*3)) ; i < (2*((tiles-1)*3))+((tiles-1)*3); i++) {
            directionMesh.setColor(i,  directionMesh.getColor(i).lerp(ofColor(255,100,100,30), 0.02));
        }
    } else {
        for (int i = (2*((tiles-1)*3)) ; i < (2*((tiles-1)*3))+((tiles-1)*3); i++) {
            directionMesh.setColor(i, directionMesh.getColor(i).lerp(ofColor(200,255,200,0), 0.02));
        }
    }
    
    if (!active_[3]) {
        for (int i = (3*((tiles-1)*3)) ; i < (3*((tiles-1)*3))+((tiles-1)*3); i++) {
            directionMesh.setColor(i,  directionMesh.getColor(i).lerp(ofColor(255,100,100,30), 0.02));
        }
    } else {
        for (int i = (3*((tiles-1)*3)) ; i < (3*((tiles-1)*3))+((tiles-1)*3); i++) {
            directionMesh.setColor(i, directionMesh.getColor(i).lerp(ofColor(200,255,200,0), 0.02));
        }
    }
    
    
    //set connected[]
    if (!connected_[0]) {
        for (int i = (0*((tiles-1)*3)) ; i < (0*((tiles-1)*3))+((tiles-1)*3); i+=3) {
            //directionMesh.setVertex(i+2, lineMeshVertices.at(0)+ ofVec3f(-10,tileSize*((i/3)+1),+10));
            directionMesh.setVertex(i+2, lineMeshVertices.at(0)+ ofVec3f(-20,tileSize*((i/3)+1),0));
        }
    } else {
        for (int i = (0*((tiles-1)*3)) ; i < (0*((tiles-1)*3))+((tiles-1)*3); i+=3) {
            directionMesh.setVertex(i+2,lineMeshVertices.at(0)+ ofVec3f(0,tileSize*((i/3)+1),0));
        }
    }
    
    if (!connected_[1]) {
        for (int i = (1*((tiles-1)*3)) ; i < (1*((tiles-1)*3))+((tiles-1)*3); i+=3) {
            //directionMesh.setVertex(i+2,lineMeshVertices.at(1)+ ofVec3f(tileSize* (((i/3)+1)-(tiles*1)+1),10,10));
            directionMesh.setVertex(i+2,lineMeshVertices.at(1)+ ofVec3f(tileSize* (((i/3)+1)-(tiles*1)+1),20,0));
        }
    } else {
        for (int i = (1*((tiles-1)*3)) ; i < (1*((tiles-1)*3))+((tiles-1)*3); i+=3) {
            directionMesh.setVertex(i+2,lineMeshVertices.at(1)+ ofVec3f(tileSize* (((i/3)+1)-(tiles*1)+1),0,0));
        }
    }
    
    
    if (!connected_[2]) {
        for (int i = (2*((tiles-1)*3)) ; i < (2*((tiles-1)*3))+((tiles-1)*3); i+=3) {
            //            directionMesh.setVertex(i+2,lineMeshVertices.at(2)+ ofVec3f(10,-tileSize* (((i/3)+1)-(tiles*2)+2),10));
            directionMesh.setVertex(i+2,lineMeshVertices.at(2)+ ofVec3f(20,-tileSize* (((i/3)+1)-(tiles*2)+2),0));
            
        }
    } else {
        for (int i = (2*((tiles-1)*3)) ; i < (2*((tiles-1)*3))+((tiles-1)*3); i+=3) {
            directionMesh.setVertex(i+2,lineMeshVertices.at(2)+ ofVec3f(0,-tileSize* (((i/3)+1)-(tiles*2)+2),0));
        }
    }
    
    if (!connected_[3]) {
        for (int i = (3*((tiles-1)*3)) ; i < (3*((tiles-1)*3))+((tiles-1)*3); i+=3) {
            //            directionMesh.setVertex(i+2,lineMeshVertices.at(3)+ ofVec3f(-tileSize* (((i/3)+1)-(tiles*3)+3),-10,10));
            directionMesh.setVertex(i+2,lineMeshVertices.at(3)+ ofVec3f(-tileSize* (((i/3)+1)-(tiles*3)+3),-20,0));
            
        }
    } else {
        for (int i = (3*((tiles-1)*3)) ; i < (3*((tiles-1)*3))+((tiles-1)*3); i+=3) {
            directionMesh.setVertex(i+2,lineMeshVertices.at(3)+ ofVec3f(-tileSize* (((i/3)+1)-(tiles*3)+3),0,0));
        }
    }
    
    
    
    // } else {
    // linePct = 1.0;
    //}
    
    int looper = scanDir_*2;
    
    /*
     lineMesh.setVertex( (looper+6)%8 , lineMeshVertices.at((looper+7)%8)+ (lineMeshVertices.at((looper+6)%8)-lineMeshVertices.at((looper+7)%8)) * linePct );
     lineMesh.setVertex( (looper+3)%8 , lineMeshVertices.at((looper+2)%8)+ (lineMeshVertices.at((looper+3)%8)-lineMeshVertices.at((looper+2)%8)) * linePct );
     lineMesh.setVertex( (looper+2)%8 , lineMeshVertices.at((looper+2)%8)  + (lineMeshVertices.at((looper+3)%8)-lineMeshVertices.at((looper+2)%8)) * linePct / 4);
     lineMesh.setVertex( (looper+7)%8 , lineMeshVertices.at((looper+7)%8) + (lineMeshVertices.at((looper+6)%8)-lineMeshVertices.at((looper+7)%8)) * linePct / 4);
     
     lineMesh.setColor((looper+7)%8, ofColor(255,255,255,0));
     lineMesh.setColor((looper+2)%8, ofColor(255,255,255,0));
     
     lineMesh.setColor((looper+6)%8, ofColor(255,255,255,150));
     lineMesh.setColor((looper+3)%8, ofColor(255,255,255,150));
     */
    
    
    
    lineAlpha = abs(sin(pctScale-HALF_PI))*150;
    
    //  cout << linePct << "  " << stepper  << endl;
    
    
    
}

void InterfacePlane::draw(int& scanDir_){
    
    // circlePath.draw();
    ofPushStyle();
    posNode.transformGL();
    
    ofSetColor(255, 255, 255,lineAlpha);
    ofLine(-10, 0, 10, 0);
    
    
    posNode.restoreTransformGL();
    ofPopStyle();
    //  lineMesh.draw();
    directionMesh.draw();
    
    ofPushMatrix();
    
    for (int i = 1; i < tiles; i++) {
        
        if (scanDir_ == 0) {
            ofTranslate(0, tileSize);
            pulseLine.draw();
        } else if (scanDir_ == 1) {
            ofTranslate(tileSize,0);
            pulseLine.draw();
        } else if (scanDir_ == 2) {
            ofTranslate(0, -tileSize);
            pulseLine.draw();
        } else if (scanDir_ == 3) {
            ofTranslate(-tileSize,0);
            pulseLine.draw();
        }
    }
    
    ofPopMatrix();
    
}


void InterfacePlane::pulseDir(int dir_) {
    
    
    for (int i = (dir_*((tiles-1)*3)) ; i < (dir_*((tiles-1)*3))+((tiles-1)*3); i++) {
        directionMesh.setColor(i, ofColor(200,255,200,120));
    }
}