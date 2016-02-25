
#include "interfacePlane.h"

enum buttonState {STATE_ACTIVE, STATE_CONNECTED, STATE_OFF};


InterfacePlane::InterfacePlane(){
    animate = false;
    meshBig = false;
    
    for (int i = 0; i < 4 ; i++) {
        aniPct[i] = 1.0;
        buttonMoving[i] = false;
        buttonState[i] = -1;
        moveTarget[i] = ofVec3f(0,0,0);
        blink[i] = false;
        blinkPct[i] = 0.0;
        meshState[i] = 1;
    }
}

InterfacePlane::InterfacePlane(int tiles_, float tileSize_, bool connected_[], bool active_[]) {
    for (int i = 0; i < 4; i++) {
        nextDirs[i] = 1;
    }
    
    animate = false;
    meshBig = false;
    
    for (int i = 0; i < 4 ; i++) {
        aniPct[i] = 1.0;
        buttonMoving[i] = false;
        buttonState[i] = -1;
        moveTarget[i] = ofVec3f(0,0,0);
        blink[i] = false;
        blinkPct[i] = 0.0;
        meshState[i] = 1;
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
    
    connected = ofColor::fromHsb(73,135,178,255);
    active = ofColor::fromHsb(28,160,170,255);
    offColor = ofColor::fromHsb(0,127, 175, 255);
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
    
    
    setupMeshes(connected_, active_);
    
}


void InterfacePlane::setupMeshes(bool connected_[], bool active_[]){
    
    float zH = 0;
    float zHharmony = 30;
    float arrow_width = 8.0;
    arrow_width/=2;
    ofMatrix4x4 tempM, tempMB;
    vector<ofVec3f> tempVertices;
    tempVertices.clear();
    tempVertices.resize(10);
    float scaleFac = 10;
    //connected mesh default
    
    //----------------------direction mesh connected
    
    directionMeshCon.clear();
    directionMeshCon.setMode(OF_PRIMITIVE_LINES);
    
    directionMeshConBig.clear();
    directionMeshConBig.setMode(OF_PRIMITIVE_LINES);
    
    
    tempVertices.at(0) = ofVec3f(-arrow_width,-arrow_width,0);
    tempVertices.at(1) = ofVec3f(-arrow_width,arrow_width,0);
    
    tempVertices.at(2) = ofVec3f(-arrow_width,arrow_width,0);
    tempVertices.at(3) = ofVec3f(arrow_width,arrow_width,0);
    
    tempVertices.at(4) = ofVec3f(arrow_width,arrow_width,0);
    tempVertices.at(5) = ofVec3f(arrow_width*2,0,0);
    
    tempVertices.at(6) = ofVec3f(arrow_width*2,0,0);
    tempVertices.at(7) = ofVec3f(arrow_width,-arrow_width,0);
    
    tempVertices.at(8) = ofVec3f(arrow_width,-arrow_width,0);
    tempVertices.at(9) = ofVec3f(-arrow_width,-arrow_width,0);
    
    
    tempM.setTranslation(ofVec3f(-gridSize,0,0));
    tempMB.setTranslation(ofVec3f(-gridSize*1.5,0,0));
    
    
    for (int i=0; i < 10;i++) {
        directionMeshCon.addVertex(tempVertices.at(i)*tempM);
        directionMeshCon.addColor(connected);
        
        directionMeshConBig.addVertex( tempVertices.at(i)*scaleFac *tempMB);
        directionMeshConBig.addColor(connected);
    }
    
    // tempM.newTranslationMatrix(ofVec3f(0,0,0));
    tempM.rotate(90, 0, 0, -1);
    tempMB.rotate(90, 0, 0, -1);
    
    
    for (int i=0; i < 10;i++) {
        directionMeshCon.addVertex(tempVertices.at(i)*tempM);
        directionMeshCon.addColor(connected);
        
        directionMeshConBig.addVertex( tempVertices.at(i)*scaleFac *tempMB);
        directionMeshConBig.addColor(connected);
    }
    
    tempM.rotate(90, 0, 0, -1);
    tempMB.rotate(90, 0, 0, -1);
    
    
    for (int i=0; i < 10;i++) {
        directionMeshCon.addVertex(tempVertices.at(i)*tempM);
        directionMeshCon.addColor(connected);
        
        directionMeshConBig.addVertex( tempVertices.at(i)*scaleFac *tempMB);
        directionMeshConBig.addColor(connected);
    }
    
    tempM.rotate(90, 0, 0, -1);
    tempMB.rotate(90, 0, 0, -1);
    
    
    for (int i=0; i < 10;i++) {
        directionMeshCon.addVertex(tempVertices.at(i)*tempM);
        directionMeshCon.addColor(connected);
        
        directionMeshConBig.addVertex( tempVertices.at(i)*scaleFac *tempMB);
        directionMeshConBig.addColor(connected);
    }
    
    for (int i = 0; i < directionMeshCon.getNumVertices(); i++) {
        directionMeshCon.setVertex(i, directionMeshCon.getVertex(i)+ofVec3f(0,0,zH));
        directionMeshConBig.setVertex(i, directionMeshConBig.getVertex(i)+ofVec3f(0,0,zHharmony));
    }
    
    //----------------------direction mesh active
    
    //connected mesh active
    directionMeshAct.clear();
    directionMeshAct.setMode(OF_PRIMITIVE_LINES);
    
    directionMeshActBig.clear();
    directionMeshActBig.setMode(OF_PRIMITIVE_LINES);
    
    tempVertices.at(0) = ofVec3f(-arrow_width,-arrow_width,0);
    tempVertices.at(1) = ofVec3f(-arrow_width,arrow_width,0);
    
    tempVertices.at(2) = ofVec3f(-arrow_width,arrow_width,0);
    tempVertices.at(3) = ofVec3f(arrow_width,arrow_width,0);
    
    tempVertices.at(4) = ofVec3f(arrow_width,arrow_width,0);
    tempVertices.at(5) = ofVec3f(arrow_width,0,0);
    
    tempVertices.at(6) = ofVec3f(arrow_width,0,0);
    tempVertices.at(7) = ofVec3f(arrow_width,-arrow_width,0);
    
    tempVertices.at(8) = ofVec3f(arrow_width,-arrow_width,0);
    tempVertices.at(9) = ofVec3f(-arrow_width,-arrow_width,0);
    
    
    
    //tempM.setTranslation(ofVec3f(-gridSize,0,0));
    tempM.rotate(90, 0, 0, -1);
    tempMB.rotate(90, 0, 0, -1);
    
    
    for (int i=0; i < 10;i++) {
        directionMeshAct.addVertex(tempVertices.at(i)*tempM);
        directionMeshAct.addColor(active);
        
        directionMeshActBig.addVertex( tempVertices.at(i)*scaleFac *tempMB);
        directionMeshActBig.addColor(active);
    }
    
    // tempM.newTranslationMatrix(ofVec3f(0,0,0));
    tempM.rotate(90, 0, 0, -1);
    tempMB.rotate(90, 0, 0, -1);
    
    
    for (int i=0; i < 10;i++) {
        directionMeshAct.addVertex(tempVertices.at(i)*tempM);
        directionMeshAct.addColor(active);
        
        directionMeshActBig.addVertex( tempVertices.at(i)*scaleFac *tempMB);
        directionMeshActBig.addColor(active);
    }
    
    tempM.rotate(90, 0, 0, -1);
    tempMB.rotate(90, 0, 0, -1);
    
    
    for (int i=0; i < 10;i++) {
        directionMeshAct.addVertex(tempVertices.at(i)*tempM);
        directionMeshAct.addColor(active);
        
        directionMeshActBig.addVertex( tempVertices.at(i)*scaleFac *tempMB);
        directionMeshActBig.addColor(active);
    }
    
    tempM.rotate(90, 0, 0, -1);
    tempMB.rotate(90, 0, 0, -1);
    
    
    for (int i=0; i < 10;i++) {
        directionMeshAct.addVertex(tempVertices.at(i)*tempM);
        directionMeshAct.addColor(active);
        
        directionMeshActBig.addVertex( tempVertices.at(i)*scaleFac *tempMB);
        directionMeshActBig.addColor(active);
    }
    
    
    for (int i = 0; i < directionMeshAct.getNumVertices(); i++) {
        directionMeshAct.setVertex(i, directionMeshAct.getVertex(i)+ofVec3f(0,0,zH));
        directionMeshActBig.setVertex(i, directionMeshActBig.getVertex(i)+ofVec3f(0,0,zHharmony));
    }
    //----------------------direction mesh off
    
    //connected mesh active
    directionMeshOff.clear();
    directionMeshOff.setMode(OF_PRIMITIVE_LINES);
    
    directionMeshOffBig.clear();
    directionMeshOffBig.setMode(OF_PRIMITIVE_LINES);
    
    tempVertices.at(0) = ofVec3f(-arrow_width,-arrow_width,0);
    tempVertices.at(1) = ofVec3f(-arrow_width,arrow_width,0);
    tempVertices.at(2) = ofVec3f(0,arrow_width,0);
    
    tempVertices.at(3) = ofVec3f(0,arrow_width,0);
    tempVertices.at(4) = ofVec3f(0,-arrow_width,0);
    tempVertices.at(5) = ofVec3f(-arrow_width,-arrow_width,0);
    
    tempVertices.at(6) = ofVec3f(0,arrow_width,0);
    tempVertices.at(7) = ofVec3f(0,0,0);
    tempVertices.at(8) = ofVec3f(0,-arrow_width,0);
    
    tempVertices.at(0) = ofVec3f(-arrow_width,-arrow_width,0);
    tempVertices.at(1) = ofVec3f(-arrow_width,arrow_width,0);
    
    tempVertices.at(2) = ofVec3f(-arrow_width,arrow_width,0);
    tempVertices.at(3) = ofVec3f(0,arrow_width,0);
    
    tempVertices.at(4) = ofVec3f(0,arrow_width,0);
    tempVertices.at(5) = ofVec3f(0,0,0);
    
    tempVertices.at(6) = ofVec3f(0,0,0);
    tempVertices.at(7) = ofVec3f(0,-arrow_width,0);
    
    tempVertices.at(8) = ofVec3f(0,-arrow_width,0);
    tempVertices.at(9) = ofVec3f(-arrow_width,-arrow_width,0);
    
    
    
    //tempM.setTranslation(ofVec3f(-gridSize,0,0));
    
    tempM.rotate(90, 0, 0, -1);
    tempMB.rotate(90, 0, 0, -1);
    
    
    for (int i=0; i < 10;i++) {
        directionMeshOff.addVertex(tempVertices.at(i)*tempM);
        directionMeshOff.addColor(offColor);
        
        directionMeshOffBig.addVertex( tempVertices.at(i)*scaleFac *tempMB);
        directionMeshOffBig.addColor(offColor);
    }
    
    // tempM.newTranslationMatrix(ofVec3f(0,0,0));
    tempM.rotate(90, 0, 0, -1);
    tempMB.rotate(90, 0, 0, -1);
    
    
    for (int i=0; i < 10;i++) {
        directionMeshOff.addVertex(tempVertices.at(i)*tempM);
        directionMeshOff.addColor(offColor);
        
        directionMeshOffBig.addVertex( tempVertices.at(i)*scaleFac *tempMB);
        directionMeshOffBig.addColor(offColor);
    }
    
    tempM.rotate(90, 0, 0, -1);
    tempMB.rotate(90, 0, 0, -1);
    
    
    for (int i=0; i < 10;i++) {
        directionMeshOff.addVertex(tempVertices.at(i)*tempM);
        directionMeshOff.addColor(offColor);
        
        directionMeshOffBig.addVertex( tempVertices.at(i)*scaleFac *tempMB);
        directionMeshOffBig.addColor(offColor);
    }
    
    tempM.rotate(90, 0, 0, -1);
    tempMB.rotate(90, 0, 0, -1);
    
    
    for (int i=0; i < 10;i++) {
        directionMeshOff.addVertex(tempVertices.at(i)*tempM);
        directionMeshOff.addColor(offColor);
        
        directionMeshOffBig.addVertex( tempVertices.at(i)*scaleFac *tempMB);
        directionMeshOffBig.addColor(offColor);
    }
    
    for (int i = 0; i < directionMeshOff.getNumVertices(); i++) {
        directionMeshOff.setVertex(i, directionMeshOff.getVertex(i)+ofVec3f(0,0,zH));
        directionMeshOffBig.setVertex(i, directionMeshOffBig.getVertex(i)+ofVec3f(0,0,zHharmony));
    }
    //------------------------------------------------------
    directionMesh.clear();
    directionMesh.setMode(OF_PRIMITIVE_LINES);
    
    
    //setup start position
    for (int i = 0 ; i < directionMeshCon.getNumVertices(); i++) {
        directionMesh.addVertex(directionMeshCon.getVertex(i));
        directionMesh.addColor(connected);
        
        //clear targetMesh
        targetMesh.addVertex(ofVec3f(0,0,0));
    }
    
    
}

void InterfacePlane::animationTransition(float pct_){
    
    if (animate && pct_ == 0.0){
        for (int i = 0; i < 4; i++) {
            if (meshBig) {
                if (meshState[i] == STATE_CONNECTED) {
                    for (int j = i*10; j < (i*10)+10; j++) {
                        tempVec = ((directionMeshConBig.getVertex(j)) - directionMeshCon.getVertex(j));
                        targetMesh.setVertex(j,tempVec);
                    }
                } else if(meshState[i] == STATE_ACTIVE){
                    for (int j = i*10; j < (i*10)+10; j++) {
                        tempVec = ((directionMeshActBig.getVertex(j)) - directionMeshAct.getVertex(j));
                        targetMesh.setVertex(j,tempVec);
                    }
                } else if (meshState[i] == STATE_OFF) {
                    for (int j = i*10; j < (i*10)+10; j++) {
                        tempVec = ((directionMeshOffBig.getVertex(j)) - directionMeshOff.getVertex(j));
                        targetMesh.setVertex(j,tempVec);
                    }
                }
            } else if (!meshBig) {
                if (meshState[i] == STATE_CONNECTED) {
                    for (int j = i*10; j < (i*10)+10; j++) {
                        tempVec = ((directionMeshCon.getVertex(j)) - directionMeshConBig.getVertex(j));
                        targetMesh.setVertex(j,tempVec);
                    }
                } else if(meshState[i] == STATE_ACTIVE){
                    for (int j = i*10; j < (i*10)+10; j++) {
                        tempVec = ((directionMeshAct.getVertex(j)) - directionMeshActBig.getVertex(j));
                        targetMesh.setVertex(j,tempVec);
                    }
                } else if (meshState[i] == STATE_OFF) {
                    for (int j = i*10; j < (i*10)+10; j++) {
                        tempVec = ((directionMeshOff.getVertex(j)) - directionMeshOffBig.getVertex(j));
                        targetMesh.setVertex(j,tempVec );
                    }
                }
            }
        }        
    }
    
    if(animate && meshBig) {
        for (int i = 0; i< 4; i++) {
            if (meshState[i] == STATE_CONNECTED) {
                for (int j = i*10; j < (i*10)+10; j++) {
                    directionMesh.setVertex(j,directionMeshCon.getVertex(j) + (targetMesh.getVertex(j)*pct_));
                }
            } else if(meshState[i] == STATE_ACTIVE){
                for (int j = i*10; j < (i*10)+10; j++) {
                    directionMesh.setVertex(j,directionMeshAct.getVertex(j) + (targetMesh.getVertex(j)*pct_));
                }
            } else if (meshState[i] == STATE_OFF) {
                for (int j = i*10; j < (i*10)+10; j++) {
                    directionMesh.setVertex(j,directionMeshOff.getVertex(j) + (targetMesh.getVertex(j)*pct_));
                }
            }
        }
    } else if (animate && !meshBig) {
        for (int i = 0; i< 4; i++) {
            if (meshState[i] == STATE_CONNECTED) {
                for (int j = i*10; j < (i*10)+10; j++) {
                    directionMesh.setVertex(j,directionMeshConBig.getVertex(j) + (targetMesh.getVertex(j)*pct_));
                }
            } else if(meshState[i] == STATE_ACTIVE){
                for (int j = i*10; j < (i*10)+10; j++) {
                    directionMesh.setVertex(j,directionMeshActBig.getVertex(j) + (targetMesh.getVertex(j)*pct_));
                }
            } else if (meshState[i] == STATE_OFF) {
                for (int j = i*10; j < (i*10)+10; j++) {
                    directionMesh.setVertex(j,directionMeshOffBig.getVertex(j) + (targetMesh.getVertex(j)*pct_));
                }
            }
        }
    }
    
    if (animate && pct_ >= 1.0){
        animate = false;
    }
}

void InterfacePlane::transformButton(bool connected_[], bool active_[], int& globalState_) {
    
    
    for (int i = 0; i < 4; i++) {
        if ( (active_[i] && connected_[i]) && buttonState[i] != STATE_CONNECTED) {
            buttonState[i] = STATE_CONNECTED;
        }
        
        else if ( (active_[i] && !connected_[i] ) && buttonState[i] != STATE_ACTIVE) {
            buttonState[i] = STATE_ACTIVE;
        }
        
        else if ( (!active_[i] && !connected_[i]) && buttonState[i] != STATE_OFF) {
            buttonState[i] = STATE_OFF;
        }
    }
    
    for (int i = 0; i < 4; i++) {
        if (meshState[i] != buttonState[i]){
            if(globalState_ == 4){
                if(buttonState[i] == STATE_CONNECTED){
                    for (int j = i*10; j < (i*10)+10; j++) {
                        tempVec = ((directionMeshConBig.getVertex(j)) - directionMeshOffBig.getVertex(j));
                        targetMesh.setVertex(j,tempVec);
                        aniPct[i] = 0.0;
                        meshState[i] = STATE_CONNECTED;
                        buttonMoving[i] = true;
                    }
                } else if(buttonState[i] == STATE_ACTIVE){
                    for (int j = i*10; j < (i*10)+10; j++) {
                        tempVec = ((directionMeshActBig.getVertex(j)) - directionMeshConBig.getVertex(j));
                        targetMesh.setVertex(j,tempVec);
                        aniPct[i] = 0.0;
                        meshState[i] = STATE_ACTIVE;
                        buttonMoving[i] = true;
                    }
                } else if(buttonState[i] == STATE_OFF){
                    for (int j = i*10; j < (i*10)+10; j++) {
                        tempVec = ((directionMeshOffBig.getVertex(j)) - directionMeshActBig.getVertex(j));
                        targetMesh.setVertex(j,tempVec);
                        aniPct[i] = 0.0;
                        meshState[i] = STATE_OFF;
                        buttonMoving[i] = true;
                    }
                }
            } else if(globalState_ == 0){
                if(buttonState[i] == STATE_CONNECTED){
                    for (int j = i*10; j < (i*10)+10; j++) {
                        tempVec = ((directionMeshCon.getVertex(j)) - directionMeshOff.getVertex(j));
                        targetMesh.setVertex(j,tempVec);
                        aniPct[i] = 0.0;
                        meshState[i] = STATE_CONNECTED;
                        buttonMoving[i] = true;
                    }
                } else if(buttonState[i] == STATE_ACTIVE){
                    for (int j = i*10; j < (i*10)+10; j++) {
                        tempVec = ((directionMeshAct.getVertex(j)) - directionMeshCon.getVertex(j));
                        targetMesh.setVertex(j,tempVec);
                        aniPct[i] = 0.0;
                        meshState[i] = STATE_ACTIVE;
                        buttonMoving[i] = true;
                    }
                } else if(buttonState[i] == STATE_OFF){
                    for (int j = i*10; j < (i*10)+10; j++) {
                        tempVec = ((directionMeshOff.getVertex(j)) - directionMeshAct.getVertex(j));
                        targetMesh.setVertex(j,tempVec);
                        aniPct[i] = 0.0;
                        meshState[i] = STATE_OFF;
                        buttonMoving[i] = true;
                    }
                }
            }
        }
    }
    
    for (int i = 0; i < 4; i++){
        if (aniPct[i] >= 1.0){
            aniPct[i] = 1.0;
        }
    }
    
    for (int i = 0; i< 4; i++) {
        if(buttonMoving[i] && !meshBig){
            if (meshState[i] == STATE_CONNECTED) {
                for (int j = i*10; j < (i*10)+10; j++) {
                    directionMesh.setVertex(j,directionMeshOff.getVertex(j) + (targetMesh.getVertex(j)*aniPct[i]));
                    directionMesh.setColor(j, directionMesh.getColor(j).lerp(connected, aniPct[i]));
                }
            } else if(meshState[i] == STATE_ACTIVE){
                for (int j = i*10; j < (i*10)+10; j++) {
                    directionMesh.setVertex(j,directionMeshCon.getVertex(j) + (targetMesh.getVertex(j)*aniPct[i]));
                    directionMesh.setColor(j, directionMesh.getColor(j).lerp(active, aniPct[i]));
                }
            } else if (meshState[i] == STATE_OFF) {
                for (int j = i*10; j < (i*10)+10; j++) {
                    directionMesh.setVertex(j,directionMeshAct.getVertex(j) + (targetMesh.getVertex(j)*aniPct[i]));
                    directionMesh.setColor(j, directionMesh.getColor(j).lerp(offColor, aniPct[i]));
                }
            }
        }
    }
    for (int i = 0; i< 4; i++) {
        if(buttonMoving[i] && meshBig){
            if (meshState[i] == STATE_CONNECTED) {
                for (int j = i*10; j < (i*10)+10; j++) {
                    directionMesh.setVertex(j,directionMeshOffBig.getVertex(j) + (targetMesh.getVertex(j)*aniPct[i]));
                    directionMesh.setColor(j, directionMesh.getColor(j).lerp(connected, aniPct[i]));
                }
            } else if(meshState[i] == STATE_ACTIVE){
                for (int j = i*10; j < (i*10)+10; j++) {
                    directionMesh.setVertex(j,directionMeshConBig.getVertex(j) + (targetMesh.getVertex(j)*aniPct[i]));
                    directionMesh.setColor(j, directionMesh.getColor(j).lerp(active, aniPct[i]));
                }
            } else if (meshState[i] == STATE_OFF) {
                for (int j = i*10; j < (i*10)+10; j++) {
                    directionMesh.setVertex(j,directionMeshActBig.getVertex(j) + (targetMesh.getVertex(j)*aniPct[i]));
                    directionMesh.setColor(j, directionMesh.getColor(j).lerp(offColor, aniPct[i]));
                }
            }
        }
    }
    
    for (int i = 0; i < 4; i++){
        if (aniPct[i] < 1.0){
            aniPct[i] += 0.12;
        } else if (aniPct[i] >= 1.0){
            buttonMoving[i] = false;
        }
    }

}

void InterfacePlane::blinkP(){
    
    //pulse blink
    
    for (int i = 0; i < 4; i++){
        if(blink[i]) {
            blinkPct[i] *= 1.425;
            if(buttonState[i] == STATE_CONNECTED){
                for (int j = i*10; j < (i*10)+10; j++) {
                    directionMesh.setColor(j, directionMesh.getColor(j).lerp(connected, blinkPct[i]));
                }
            }
            
            if(buttonState[i] == STATE_ACTIVE){
                for (int j = i*10; j < (i*10)+10; j++) {
                    directionMesh.setColor(j, directionMesh.getColor(j).lerp(active, blinkPct[i]));
                }
            }
        }
        
        cout << blinkPct[i]  << endl;
        if (blink[i] && blinkPct[i] >= 1.0){
            blink[i] = false;
            blinkPct[i] = 1.0;
        }
        
    }
}

void InterfacePlane::update(int& stepper, float& tickTime_, int& scanDir_, bool connected_[], bool active_[], bool& pause_, int& globalState_) {
    
    transformButton(connected_, active_, globalState_);
    blinkP();
    
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
        
        
        
        // lineAlpha = abs(sin(pctScale-HALF_PI))*150;
        
        
    }
    
}

void InterfacePlane::draw( bool& pause_){
    
    if (!pause_) {
        
        // ofPushStyle();
        posNode.transformGL();
        
        // ofSetColor(filterColor( ofColor( 255, 255, 255,lineAlpha)) );
        ofSetColor( ofColor( 255, 255, 255,180) );
        
        ofLine(-8, 0, 8, 0);
        
        
        posNode.restoreTransformGL();
        //  ofPopStyle();
        //  lineMesh.draw();
        directionMesh.drawWireframe();
        
        
        
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
    
    blink[dir_] = true;
    blinkPct[dir_] = 0.01;
    for (int j = dir_*10; j < (dir_*10)+10; j++) {
        directionMesh.setColor(j, pulseColor);
    }
    
}

