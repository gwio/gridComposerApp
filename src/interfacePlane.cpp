
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
    
    gridSize = (tileSize*tiles)*0.5+10;
    
    lineMesh.clear();
    
    float zH = 22.55;
    //pulse lines
    lineMesh.addVertex(ofVec3f(-50,-50,zH));
    lineMesh.addColor(ofColor(255,255,255,0));
    lineMesh.addVertex(ofVec3f(-50,50,zH));
    lineMesh.addColor(ofColor(255,255,255,0));
    lineMesh.addVertex(ofVec3f(50,+50,zH));
    lineMesh.addColor(ofColor(255,255,255,0));
    lineMesh.addVertex(ofVec3f(50,-50,zH));
    lineMesh.addColor(ofColor(255,255,255,0));
    
    connectedDir = ofColor::fromHsb(0,0,255,0);
    notActiveDir = ofColor::fromHsb(0,100,255,100);
    pulseColor = ofColor::fromHsb(0, 0, 255,255);
    
    //connect lines
    
    lineMesh.setMode(OF_PRIMITIVE_TRIANGLES);
    
    lineMeshVertices.clear();
    lineMeshVertices.reserve(8*2);
    
    for (int i = 0; i < lineMesh.getNumVertices(); i++) {
        lineMeshVertices.push_back(lineMesh.getVertex(i));
    }
    
    
    pulseLine.addVertex(ofVec3f(-50,-50,0));
    pulseLine.addVertex(ofVec3f(-50,-50,0));;
    pulseLine.addColor(ofColor(255,255,255,0));
    pulseLine.addColor(ofColor(255,255,255,100));
    pulseLine.setUsage( GL_DYNAMIC_DRAW );
    pulseLine.setMode(OF_PRIMITIVE_LINES);
    
    
    //direction lines
    directionMesh.setMode(OF_PRIMITIVE_TRIANGLES);
    
    
        directionMesh.addVertex(ofVec3f(-gridSize,0,0) + ofVec3f(0,-5,0) );
        directionMesh.addVertex(ofVec3f(-gridSize,0,0) + ofVec3f(0,5,0) );
        directionMesh.addVertex(ofVec3f(-gridSize,0,0));
        
        directionMesh.addColor(connectedDir);
        directionMesh.addColor(connectedDir);
        directionMesh.addColor(connectedDir);
    
    
        directionMesh.addVertex(ofVec3f(0,gridSize,0)  + ofVec3f(-5,0,0) );
        directionMesh.addVertex(ofVec3f(0,gridSize,0)  + ofVec3f(5,0,0) );
        directionMesh.addVertex(ofVec3f(0,gridSize,0));
        
        directionMesh.addColor(connectedDir);
        directionMesh.addColor(connectedDir);
        directionMesh.addColor(connectedDir);
    
    
        directionMesh.addVertex(ofVec3f(gridSize,0,0) + ofVec3f(0,-5,0) );
        directionMesh.addVertex(ofVec3f(gridSize,0,0) + ofVec3f(0,5,0) );
        directionMesh.addVertex(ofVec3f(gridSize,0,0));
        
        directionMesh.addColor(connectedDir);
        directionMesh.addColor(connectedDir);
        directionMesh.addColor(connectedDir);
    
    
    
        directionMesh.addVertex(ofVec3f(0,-gridSize,0) + ofVec3f(-5,0,0) );
        directionMesh.addVertex(ofVec3f(0,-gridSize,0) + ofVec3f(5,0,0) );
        directionMesh.addVertex(ofVec3f(0,-gridSize,0));
        
        directionMesh.addColor(connectedDir);
        directionMesh.addColor(connectedDir);
        directionMesh.addColor(connectedDir);
    
    
}


void InterfacePlane::update(int& stepper, float& tickTime_, int& scanDir_, bool connected_[], bool active_[], bool& pause_) {
    
    if (!pause_) {
        
        thisTime = (ofGetElapsedTimeMillis()- lastTick) ;
        scanDir = scanDir_;
        
        if (thisTime >= tickTime_ ) {
            len = 1.0;
        } else {
            len =ofMap( fmod(thisTime, tickTime_ ), 0.0  , tickTime_ , 0.0, 1.0);
        }
        
        
        alphaPart = TWO_PI/((tiles+1)*4);
        
        pctBar = ofMap(stepper+len, 0, tiles+1, 0.0, 1.0);
        
        ofMatrix4x4 aaa;
        
        
        if (nextDirs[1] && nextDirs[0]) {
            alpha = ofMap( stepper, 0,  (tiles+1), 0.0, HALF_PI) + scanDir_*HALF_PI;
            alpha -= HALF_PI;
            alpha += len*alphaPart;
            aaa.rotateRad(ofLerp(0.0, PI+HALF_PI,pctBar)+scanDir_*HALF_PI, 0, 0, 1) ;
        }else if (!nextDirs[1]){
            if(nextDirs[2]) {
                alpha =   ofMap( stepper, 0,  (tiles+1), 0.0, HALF_PI)  + scanDir_*HALF_PI;
                alpha += ofMap( stepper, 0,  (tiles+1), 0.0, HALF_PI);
                alpha -= HALF_PI;
                alpha+= len*(alphaPart*2);
                aaa.rotateRad(ofLerp(0.0, PI+HALF_PI,pctBar)+(scanDir_*HALF_PI)+(HALF_PI*pctBar), 0, 0, 1) ;
                
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
            }
            
        }
        
        
        alphaPos = ofVec3f(sin(alpha)*80, cos(alpha)*80,0 );
        
        
        float pctScale = ofMap(stepCounter+len,0, (tiles+1)*4,0.0,TWO_PI*2);
        
        
        pulseRot.setRotate(aaa.getRotate().inverse());
        
        posNode.setOrientation(pulseRot.getRotate());
        
        posNode.setPosition(alphaPos);
        
        float scalePct = (abs(sin(pctScale-(HALF_PI*1.25)))*1);
        float thisScale =  ofClamp(pow(scalePct, 6),0.0,1.0);
        
        linePct = ofClamp(ofMap(pctBar, 0.0, 1.0, 0.0, (1.0+(1/tiles))), 0.0, 1.0);
        
        
        posNode.setPosition( posNode.getPosition()* ((-thisScale*0.15) +1.0) ) ;
        posNode.setScale( (thisScale*1) +0.65 );
        
        //cout << linePct << endl;
        
        if (scanDir == 0 && (active_[0] && connected_[0]) ) {
            pulseLine.setVertex(0, lineMeshVertices.at(0) +  (lineMeshVertices.at(3)-lineMeshVertices.at(0))*linePct/2);
            pulseLine.setVertex(1, lineMeshVertices.at(0) + (lineMeshVertices.at(3)-lineMeshVertices.at(0))*linePct);
            
        } else if (scanDir == 1 && (active_[1] && connected_[1])) {
            pulseLine.setVertex(0, lineMeshVertices.at(1)+ (lineMeshVertices.at(0)-lineMeshVertices.at(1))*linePct/2);
            pulseLine.setVertex(1, lineMeshVertices.at(1) + (lineMeshVertices.at(0)-lineMeshVertices.at(1))*linePct);
            
            
        } else if (scanDir == 2 && (active_[2] && connected_[2])) {
            pulseLine.setVertex(0, lineMeshVertices.at(2)+ (lineMeshVertices.at(1)-lineMeshVertices.at(2))*linePct/2);
            pulseLine.setVertex(1, lineMeshVertices.at(2) + (lineMeshVertices.at(1)-lineMeshVertices.at(2))*linePct);
            
            
        } else if (scanDir == 3 && (active_[3] && connected_[3])) {
            pulseLine.setVertex(0, lineMeshVertices.at(3)+ (lineMeshVertices.at(2)-lineMeshVertices.at(3))*linePct/2);
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
            pulseLine.setColor(1, ofColor::fromHsb(0, 0, 255,200));
        }
        
        // cout <<  tickTime_ << "  " <<  stepCounter << " " << thisTime  << "  " << ofGetElapsedTimeMillis() << "  " << len << endl;
        //cout <<  fmod( double(pctBar), 0.25) << endl;
        
        
        //  if (stepper != 5) {
        
        
        
        
        //set active[]
        if (!active_[0]) {
            for (int i = (0*3) ; i < ((0*3)+3); i++) {
                directionMesh.setColor(i,filterColor(  directionMesh.getColor(i).lerp(notActiveDir, 0.02))) ;
            }
        } else {
            for (int i = (0*3) ; i < ((0*3)+3); i++) {
                directionMesh.setColor(i, filterColor(directionMesh.getColor(i).lerp(connectedDir, 0.02)));
            }
        }
        
        if (!active_[1]) {
            for (int i = (1*3) ; i < ((1*3)+3); i++) {
                directionMesh.setColor(i, filterColor( directionMesh.getColor(i).lerp(notActiveDir, 0.02)));
            }
        } else {
            for (int i = (1*3) ; i < ((1*3)+3); i++) {
                directionMesh.setColor(i, filterColor(directionMesh.getColor(i).lerp(connectedDir, 0.02)));
            }
        }
        
        
        if (!active_[2]) {
            for (int i = (2*3) ; i < ((2*3)+3); i++) {
                directionMesh.setColor(i, filterColor( directionMesh.getColor(i).lerp(notActiveDir, 0.02)));
            }
        } else {
            for (int i = (2*3) ; i < ((2*3)+3); i++) {
                directionMesh.setColor(i,filterColor( directionMesh.getColor(i).lerp(connectedDir, 0.02)));
            }
        }
        
        if (!active_[3]) {
            for (int i = (3*3) ; i < ((3*3)+3); i++) {
                directionMesh.setColor(i, filterColor( directionMesh.getColor(i).lerp(notActiveDir, 0.02)));
            }
        } else {
            for (int i = (3*3) ; i < ((3*3)+3);i++) {
                directionMesh.setColor(i,filterColor( directionMesh.getColor(i).lerp(connectedDir, 0.02)));
            }
        }
        
        
        //set connected[]
        if (!connected_[0]) {
            for (int i = (0*3) ; i < ((0*3)+3);i+=3) {
                //directionMesh.setVertex(i+2, lineMeshVertices.at(0)+ ofVec3f(-10,tileSize*((i/3)+1),+10));
                directionMesh.setVertex(i+2, ofVec3f(-gridSize,0,0) + ofVec3f(-5,0,0));
            }
        } else {
            for (int i = (0*3) ; i < ((0*3)+3); i+=3) {
                directionMesh.setVertex(i+2, ofVec3f(-gridSize,0,0) + ofVec3f(5,0,0));
            }
        }
        
        if (!connected_[1]) {
            for (int i = (1*3) ; i < ((1*3)+3); i+=3) {
                //directionMesh.setVertex(i+2,lineMeshVertices.at(1)+ ofVec3f(tileSize* (((i/3)+1)-(tiles*1)+1),10,10));
                directionMesh.setVertex(i+2, ofVec3f(0,gridSize,0) + ofVec3f(0,5,0));
            }
        } else {
            for (int i = (1*3) ; i < ((1*3)+3); i+=3) {
                directionMesh.setVertex(i+2, ofVec3f(0,gridSize,0) + ofVec3f(0,-5,0));
            }
        }
        
        
        if (!connected_[2]) {
            for (int i = (2*3) ; i < ((2*3)+3); i+=3) {
                //            directionMesh.setVertex(i+2,lineMeshVertices.at(2)+ ofVec3f(10,-tileSize* (((i/3)+1)-(tiles*2)+2),10));
                directionMesh.setVertex(i+2, ofVec3f(gridSize,0,0) + ofVec3f(5,0,0));
                
            }
        } else {
            for (int i = (2*3) ; i < ((2*3)+3); i+=3) {
                directionMesh.setVertex(i+2, ofVec3f(gridSize,0,0) + ofVec3f(-5,0,0));
            }
        }
        
        if (!connected_[3]) {
            for (int i = (3*3) ; i < ((3*3)+3); i+=3) {
                //            directionMesh.setVertex(i+2,lineMeshVertices.at(3)+ ofVec3f(-tileSize* (((i/3)+1)-(tiles*3)+3),-10,10));
                directionMesh.setVertex(i+2, ofVec3f(0,-gridSize,0) + ofVec3f(0,-5,0));
                
            }
        } else {
            for (int i = (3*3) ; i < ((3*3)+3); i+=3) {
                directionMesh.setVertex(i+2, ofVec3f(0,-gridSize,0) + ofVec3f(0,5,0));
            }
        }
        
        
        
        
        // lineAlpha = abs(sin(pctScale-HALF_PI))*150;
        
        
    }
    
}

void InterfacePlane::draw( bool& pause_){
    
    if (!pause_) {
        
        // ofPushStyle();
        posNode.transformGL();
        
        // ofSetColor(filterColor( ofColor( 255, 255, 255,lineAlpha)) );
        ofSetColor(filterColor( ofColor( 255, 255, 255,180)) );
        
        ofLine(-8, 0, 8, 0);
        
        
        posNode.restoreTransformGL();
        //  ofPopStyle();
        //  lineMesh.draw();
        directionMesh.draw();
        
        
        if (scanDir >=0) {
            ofPushMatrix();
            
            for (int i = 1; i < tiles; i++) {
                
                if (scanDir == 0) {
                    ofTranslate(0, tileSize);
                    pulseLine.draw();
                } else if (scanDir == 1) {
                    ofTranslate(tileSize,0);
                    pulseLine.draw();
                } else if (scanDir == 2) {
                    ofTranslate(0, -tileSize);
                    pulseLine.draw();
                } else if (scanDir == 3) {
                    ofTranslate(-tileSize,0);
                   pulseLine.draw();
                }
            }
            
            ofPopMatrix();
        }
        
    }
}


void InterfacePlane::pulseDir(int dir_) {
    
    for (int i = (dir_*3) ; i < (dir_*3)+3; i++) {
        directionMesh.setColor(i,filterColor( pulseColor));
    }
}

ofColor InterfacePlane::filterColor(ofColor c_){
    ofColor temp;
    temp.r = ofClamp(c_.r, 20, 230);
    temp.g = ofClamp(c_.g, 20, 230);
    temp.b = ofClamp(c_.b, 20, 230);
    temp.a = ofClamp(c_.a, 10, 255);
    
    
    return temp;
}