
#include "interfacePlane.h"

enum buttonState {STATE_ACTIVE, STATE_CONNECTED, STATE_OFF};


InterfacePlane::InterfacePlane(){
    animate = false;
    meshBig = false;
    positionMod = 1.0;
    scaleMod = 1.0;
    positionModDef = 1.0;
    positionModTar = 1.0;
    scaleModDef = 1.0;
    scaleModTar = 1.0;
    
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
    positionMod = 1.0;
    scaleMod = 1.0;
    positionModDef = 1.0;
    positionModTar = 1.0;
    scaleModDef = 1.0;
    scaleModTar = 1.0;
    thisTime = 0.0;
    lastTime = 0.0;
    
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
    
    //posNode.setPosition(0, 0, 0);
    
    gridSize = (tileSize*tiles)*0.5+10;
    
    lineMesh.clear();
    lineMesh.setMode(OF_PRIMITIVE_LINE_STRIP);
    
    //float zH = 130;

    
    connected = ofColor::fromHsb(73,135,178,255);
    active = ofColor::fromHsb(28,160,170,255);
    offColor = ofColor::fromHsb(255, 0, 51, 200);
    pulseColorA = ofColor::fromHsb(0, 0, 255,255);
    pulseColorC = ofColor::fromHsb(0, 0, 255,255);

    flipCounter = 0;
    
    setupMeshes(connected_, active_);
    
}



void InterfacePlane::setColor(float hue_){
    connected = ofColor::fromHsb(hue_, 235, 230,200);
    active = ofColor::fromHsb(hue_, 150, 100 ,200);
    
    pulseColorC =  ofColor::fromHsb(connected.getHue(), connected.getSaturation()-200, 255, 220);
    pulseColorA =  ofColor::fromHsb(connected.getHue(), connected.getSaturation()-20, 200, 220);

    
}

void InterfacePlane::setupMeshes(bool connected_[], bool active_[]){
    
    float zH = 0;
    float zHharmony = 32;
    float arrow_width = 6.0;
    arrow_width/=2;
    ofMatrix4x4 tempM, tempMB;
    vector<ofVec3f> tempVertices;
    tempVertices.clear();
    tempVertices.resize(9);
    float scaleFac = 16.25;
    //connected mesh default
    
    dirMeshVerts = 9;
    //----------------------direction mesh connected
    
    directionMeshCon.clear();
    directionMeshCon.setMode(OF_PRIMITIVE_TRIANGLES);
    
    directionMeshConBig.clear();
    directionMeshConBig.setMode(OF_PRIMITIVE_TRIANGLES);
    
    
    tempVertices.at(0) = ofVec3f(-arrow_width,-arrow_width,0);
    tempVertices.at(1) = ofVec3f(-arrow_width,arrow_width,0);
    tempVertices.at(2) = ofVec3f(arrow_width,arrow_width,0);
    
    tempVertices.at(3) = ofVec3f(arrow_width,arrow_width,0);
    tempVertices.at(4) = ofVec3f(arrow_width,-arrow_width,0);
    tempVertices.at(5) = ofVec3f(-arrow_width,-arrow_width,0);
    
    tempVertices.at(6) = ofVec3f(arrow_width,arrow_width,0);
    tempVertices.at(7) = ofVec3f(arrow_width*2,0,0);
    tempVertices.at(8) = ofVec3f(arrow_width,-arrow_width,0);

    
    
    tempM.setTranslation(ofVec3f(-gridSize*1.08,0,0));
    tempMB.setTranslation(ofVec3f(-gridSize*2.0,0,0));
    
    
    for (int i=0; i < dirMeshVerts;i++) {
        directionMeshCon.addVertex(tempVertices.at(i)*tempM);
        directionMeshCon.addColor(connected);
        
        directionMeshConBig.addVertex( tempVertices.at(i)*scaleFac *tempMB);
        directionMeshConBig.addColor(connected);
    }
    
    // tempM.newTranslationMatrix(ofVec3f(0,0,0));
    tempM.rotate(90, 0, 0, -1);
    tempMB.rotate(90, 0, 0, -1);
    
    
    for (int i=0; i < dirMeshVerts;i++) {
        directionMeshCon.addVertex(tempVertices.at(i)*tempM);
        directionMeshCon.addColor(connected);
        
        directionMeshConBig.addVertex( tempVertices.at(i)*scaleFac *tempMB);
        directionMeshConBig.addColor(connected);
    }
    
    tempM.rotate(90, 0, 0, -1);
    tempMB.rotate(90, 0, 0, -1);
    
    
    for (int i=0; i < dirMeshVerts;i++) {
        directionMeshCon.addVertex(tempVertices.at(i)*tempM);
        directionMeshCon.addColor(connected);
        
        directionMeshConBig.addVertex( tempVertices.at(i)*scaleFac *tempMB);
        directionMeshConBig.addColor(connected);
    }
    
    tempM.rotate(90, 0, 0, -1);
    tempMB.rotate(90, 0, 0, -1);
    
    
    for (int i=0; i < dirMeshVerts;i++) {
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
    directionMeshAct.setMode(OF_PRIMITIVE_TRIANGLES);
    
    directionMeshActBig.clear();
    directionMeshActBig.setMode(OF_PRIMITIVE_TRIANGLES);
    
    tempVertices.at(0) = ofVec3f(-arrow_width,-arrow_width,0);
    tempVertices.at(1) = ofVec3f(-arrow_width,arrow_width,0);
    tempVertices.at(2) = ofVec3f(arrow_width,arrow_width,0);
    
    tempVertices.at(3) = ofVec3f(arrow_width,arrow_width,0);
    tempVertices.at(4) = ofVec3f(arrow_width,-arrow_width,0);
    tempVertices.at(5) = ofVec3f(-arrow_width,-arrow_width,0);
    
    tempVertices.at(6) = ofVec3f(arrow_width,arrow_width,0);
    tempVertices.at(7) = ofVec3f(arrow_width,0,0);
    tempVertices.at(8) = ofVec3f(arrow_width,-arrow_width,0);
    
    
    //tempM.setTranslation(ofVec3f(-gridSize,0,0));
    tempM.rotate(90, 0, 0, -1);
    tempMB.rotate(90, 0, 0, -1);
    
    
    for (int i=0; i < dirMeshVerts;i++) {
        directionMeshAct.addVertex(tempVertices.at(i)*tempM);
        directionMeshAct.addColor(active);
        
        directionMeshActBig.addVertex( tempVertices.at(i)*scaleFac *tempMB);
        directionMeshActBig.addColor(active);
    }
    
    // tempM.newTranslationMatrix(ofVec3f(0,0,0));
    tempM.rotate(90, 0, 0, -1);
    tempMB.rotate(90, 0, 0, -1);
    
    
    for (int i=0; i < dirMeshVerts;i++) {
        directionMeshAct.addVertex(tempVertices.at(i)*tempM);
        directionMeshAct.addColor(active);
        
        directionMeshActBig.addVertex( tempVertices.at(i)*scaleFac *tempMB);
        directionMeshActBig.addColor(active);
    }
    
    tempM.rotate(90, 0, 0, -1);
    tempMB.rotate(90, 0, 0, -1);
    
    
    for (int i=0; i < dirMeshVerts;i++) {
        directionMeshAct.addVertex(tempVertices.at(i)*tempM);
        directionMeshAct.addColor(active);
        
        directionMeshActBig.addVertex( tempVertices.at(i)*scaleFac *tempMB);
        directionMeshActBig.addColor(active);
    }
    
    tempM.rotate(90, 0, 0, -1);
    tempMB.rotate(90, 0, 0, -1);
    
    
    for (int i=0; i < dirMeshVerts;i++) {
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
    directionMeshOff.setMode(OF_PRIMITIVE_TRIANGLES);
    
    directionMeshOffBig.clear();
    directionMeshOffBig.setMode(OF_PRIMITIVE_TRIANGLES);
    
    tempVertices.at(0) = ofVec3f(-arrow_width,-arrow_width,0);
    tempVertices.at(1) = ofVec3f(-arrow_width,arrow_width,0);
    tempVertices.at(2) = ofVec3f(0,arrow_width,0);
    
    tempVertices.at(3) = ofVec3f(0,arrow_width,0);
    tempVertices.at(4) = ofVec3f(0,-arrow_width,0);
    tempVertices.at(5) = ofVec3f(-arrow_width,-arrow_width,0);
    
    tempVertices.at(6) = ofVec3f(0,arrow_width,0);
    tempVertices.at(7) = ofVec3f(0,0,0);
    tempVertices.at(8) = ofVec3f(0,-arrow_width,0);
    
    
    
    //tempM.setTranslation(ofVec3f(-gridSize,0,0));
    
    tempM.rotate(90, 0, 0, -1);
    tempMB.rotate(90, 0, 0, -1);
    
    
    for (int i=0; i < dirMeshVerts;i++) {
        directionMeshOff.addVertex(tempVertices.at(i)*tempM);
        directionMeshOff.addColor(offColor);
        
        directionMeshOffBig.addVertex( tempVertices.at(i)*scaleFac *tempMB);
        directionMeshOffBig.addColor(offColor);
    }
    
    // tempM.newTranslationMatrix(ofVec3f(0,0,0));
    tempM.rotate(90, 0, 0, -1);
    tempMB.rotate(90, 0, 0, -1);
    
    
    for (int i=0; i < dirMeshVerts;i++) {
        directionMeshOff.addVertex(tempVertices.at(i)*tempM);
        directionMeshOff.addColor(offColor);
        
        directionMeshOffBig.addVertex( tempVertices.at(i)*scaleFac *tempMB);
        directionMeshOffBig.addColor(offColor);
    }
    
    tempM.rotate(90, 0, 0, -1);
    tempMB.rotate(90, 0, 0, -1);
    
    
    for (int i=0; i < dirMeshVerts;i++) {
        directionMeshOff.addVertex(tempVertices.at(i)*tempM);
        directionMeshOff.addColor(offColor);
        
        directionMeshOffBig.addVertex( tempVertices.at(i)*scaleFac *tempMB);
        directionMeshOffBig.addColor(offColor);
    }
    
    tempM.rotate(90, 0, 0, -1);
    tempMB.rotate(90, 0, 0, -1);
    
    
    for (int i=0; i < dirMeshVerts;i++) {
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
    directionMesh.setMode(OF_PRIMITIVE_TRIANGLES);
    
    
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
                    for (int j = i*dirMeshVerts; j < (i*dirMeshVerts)+dirMeshVerts; j++) {
                        tempVec = ((directionMeshConBig.getVertex(j)) - directionMeshCon.getVertex(j));
                        targetMesh.setVertex(j,tempVec);
                    }
                } else if(meshState[i] == STATE_ACTIVE){
                    for (int j = i*dirMeshVerts; j < (i*dirMeshVerts)+dirMeshVerts; j++) {
                        tempVec = ((directionMeshActBig.getVertex(j)) - directionMeshAct.getVertex(j));
                        targetMesh.setVertex(j,tempVec);
                    }
                } else if (meshState[i] == STATE_OFF) {
                    for (int j = i*dirMeshVerts; j < (i*dirMeshVerts)+dirMeshVerts; j++) {
                        tempVec = ((directionMeshOffBig.getVertex(j)) - directionMeshOff.getVertex(j));
                        targetMesh.setVertex(j,tempVec);
                    }
                }
            } else if (!meshBig) {
                if (meshState[i] == STATE_CONNECTED) {
                    for (int j = i*dirMeshVerts; j < (i*dirMeshVerts)+dirMeshVerts; j++) {
                        tempVec = ((directionMeshCon.getVertex(j)) - directionMeshConBig.getVertex(j));
                        targetMesh.setVertex(j,tempVec);
                    }
                } else if(meshState[i] == STATE_ACTIVE){
                    for (int j = i*dirMeshVerts; j < (i*dirMeshVerts)+dirMeshVerts; j++) {
                        tempVec = ((directionMeshAct.getVertex(j)) - directionMeshActBig.getVertex(j));
                        targetMesh.setVertex(j,tempVec);
                    }
                } else if (meshState[i] == STATE_OFF) {
                    for (int j = i*dirMeshVerts; j < (i*dirMeshVerts)+dirMeshVerts; j++) {
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
                for (int j = i*dirMeshVerts; j < (i*dirMeshVerts)+dirMeshVerts; j++) {
                    directionMesh.setVertex(j,directionMeshCon.getVertex(j) + (targetMesh.getVertex(j)*pct_));
                }
            } else if(meshState[i] == STATE_ACTIVE){
                for (int j = i*dirMeshVerts; j < (i*dirMeshVerts)+dirMeshVerts; j++) {
                    directionMesh.setVertex(j,directionMeshAct.getVertex(j) + (targetMesh.getVertex(j)*pct_));
                }
            } else if (meshState[i] == STATE_OFF) {
                for (int j = i*dirMeshVerts; j < (i*dirMeshVerts)+dirMeshVerts; j++) {
                    directionMesh.setVertex(j,directionMeshOff.getVertex(j) + (targetMesh.getVertex(j)*pct_));
                }
            }
        }
    } else if (animate && !meshBig) {
        for (int i = 0; i< 4; i++) {
            if (meshState[i] == STATE_CONNECTED) {
                for (int j = i*dirMeshVerts; j < (i*dirMeshVerts)+dirMeshVerts; j++) {
                    directionMesh.setVertex(j,directionMeshConBig.getVertex(j) + (targetMesh.getVertex(j)*pct_));
                }
            } else if(meshState[i] == STATE_ACTIVE){
                for (int j = i*dirMeshVerts; j < (i*dirMeshVerts)+dirMeshVerts; j++) {
                    directionMesh.setVertex(j,directionMeshActBig.getVertex(j) + (targetMesh.getVertex(j)*pct_));
                }
            } else if (meshState[i] == STATE_OFF) {
                for (int j = i*dirMeshVerts; j < (i*dirMeshVerts)+dirMeshVerts; j++) {
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
                    for (int j = i*dirMeshVerts; j < (i*dirMeshVerts)+dirMeshVerts; j++) {
                        tempVec = ((directionMeshConBig.getVertex(j)) - directionMeshOffBig.getVertex(j));
                        targetMesh.setVertex(j,tempVec);
                        aniPct[i] = 0.0;
                        meshState[i] = STATE_CONNECTED;
                        buttonMoving[i] = true;
                    }
                } else if(buttonState[i] == STATE_ACTIVE){
                    for (int j = i*dirMeshVerts; j < (i*dirMeshVerts)+dirMeshVerts; j++) {
                        tempVec = ((directionMeshActBig.getVertex(j)) - directionMeshConBig.getVertex(j));
                        targetMesh.setVertex(j,tempVec);
                        aniPct[i] = 0.0;
                        meshState[i] = STATE_ACTIVE;
                        buttonMoving[i] = true;
                    }
                } else if(buttonState[i] == STATE_OFF){
                    for (int j = i*dirMeshVerts; j < (i*dirMeshVerts)+dirMeshVerts; j++) {
                        tempVec = ((directionMeshOffBig.getVertex(j)) - directionMeshActBig.getVertex(j));
                        targetMesh.setVertex(j,tempVec);
                        aniPct[i] = 0.0;
                        meshState[i] = STATE_OFF;
                        buttonMoving[i] = true;
                    }
                }
            } else if(globalState_ == 0){
                if(buttonState[i] == STATE_CONNECTED){
                    for (int j = i*dirMeshVerts; j < (i*dirMeshVerts)+dirMeshVerts; j++) {
                        tempVec = ((directionMeshCon.getVertex(j)) - directionMeshOff.getVertex(j));
                        targetMesh.setVertex(j,tempVec);
                        aniPct[i] = 0.0;
                        meshState[i] = STATE_CONNECTED;
                        buttonMoving[i] = true;
                    }
                } else if(buttonState[i] == STATE_ACTIVE){
                    for (int j = i*dirMeshVerts; j < (i*dirMeshVerts)+dirMeshVerts; j++) {
                        tempVec = ((directionMeshAct.getVertex(j)) - directionMeshCon.getVertex(j));
                        targetMesh.setVertex(j,tempVec);
                        aniPct[i] = 0.0;
                        meshState[i] = STATE_ACTIVE;
                        buttonMoving[i] = true;
                    }
                } else if(buttonState[i] == STATE_OFF){
                    for (int j = i*dirMeshVerts; j < (i*dirMeshVerts)+dirMeshVerts; j++) {
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
                for (int j = i*dirMeshVerts; j < (i*dirMeshVerts)+dirMeshVerts; j++) {
                    directionMesh.setVertex(j,directionMeshOff.getVertex(j) + (targetMesh.getVertex(j)*aniPct[i]));
                    directionMesh.setColor(j, directionMesh.getColor(j).lerp(connected, aniPct[i]));
                }
            } else if(meshState[i] == STATE_ACTIVE){
                for (int j = i*dirMeshVerts; j < (i*dirMeshVerts)+dirMeshVerts; j++) {
                    directionMesh.setVertex(j,directionMeshCon.getVertex(j) + (targetMesh.getVertex(j)*aniPct[i]));
                    directionMesh.setColor(j, directionMesh.getColor(j).lerp(active, aniPct[i]));
                }
            } else if (meshState[i] == STATE_OFF) {
                for (int j = i*dirMeshVerts; j < (i*dirMeshVerts)+dirMeshVerts; j++) {
                    directionMesh.setVertex(j,directionMeshAct.getVertex(j) + (targetMesh.getVertex(j)*aniPct[i]));
                    directionMesh.setColor(j, directionMesh.getColor(j).lerp(offColor, aniPct[i]));
                }
            }
        }
    }
    for (int i = 0; i< 4; i++) {
        if(buttonMoving[i] && meshBig){
            if (meshState[i] == STATE_CONNECTED) {
                for (int j = i*dirMeshVerts; j < (i*dirMeshVerts)+dirMeshVerts; j++) {
                    directionMesh.setVertex(j,directionMeshOffBig.getVertex(j) + (targetMesh.getVertex(j)*aniPct[i]));
                    directionMesh.setColor(j, directionMesh.getColor(j).lerp(connected, aniPct[i]));
                }
            } else if(meshState[i] == STATE_ACTIVE){
                for (int j = i*dirMeshVerts; j < (i*dirMeshVerts)+dirMeshVerts; j++) {
                    directionMesh.setVertex(j,directionMeshConBig.getVertex(j) + (targetMesh.getVertex(j)*aniPct[i]));
                    directionMesh.setColor(j, directionMesh.getColor(j).lerp(active, aniPct[i]));
                }
            } else if (meshState[i] == STATE_OFF) {
                for (int j = i*dirMeshVerts; j < (i*dirMeshVerts)+dirMeshVerts; j++) {
                    directionMesh.setVertex(j,directionMeshActBig.getVertex(j) + (targetMesh.getVertex(j)*aniPct[i]));
                    directionMesh.setColor(j, directionMesh.getColor(j).lerp(offColor, aniPct[i]));
                }
            }
        }
    }
    
    for (int i = 0; i< 4; i++) {
        if(blink[i] && !meshBig){
            if (meshState[i] == STATE_CONNECTED) {
                for (int j = i*dirMeshVerts; j < (i*dirMeshVerts)+dirMeshVerts; j++) {
                    directionMesh.getVertices().at(j).z = directionMeshCon.getVertex(j).z - ((1.0-blinkPct[i])*6);
                    //directionMesh.setVertex(j,directionMeshCon.getVertex(j) *(1+ (1-blinkPct[i])*0.1 ));
                }
            }
        }
    }
    for (int i = 0; i< 4; i++) {
        if(blink[i] && meshBig){
            if (meshState[i] == STATE_CONNECTED) {
                for (int j = i*dirMeshVerts; j < (i*dirMeshVerts)+dirMeshVerts; j++) {
                    directionMesh.getVertices().at(j).z = directionMeshConBig.getVertex(j).z - ((1.0-blinkPct[i])*12);
                    //directionMesh.setVertex(j,directionMeshConBig.getVertex(j) *(1+ (1-blinkPct[i])*0.1 ));
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
            blinkPct[i] *= 1.325;
            if(buttonState[i] == STATE_CONNECTED){
                for (int j = i*dirMeshVerts; j < (i*dirMeshVerts)+dirMeshVerts; j++) {
                    directionMesh.setColor(j, directionMesh.getColor(j).lerp(connected, blinkPct[i]));
                }
            }
            
            if(buttonState[i] == STATE_ACTIVE){
                for (int j = i*dirMeshVerts; j < (i*dirMeshVerts)+dirMeshVerts; j++) {
                    directionMesh.setColor(j, directionMesh.getColor(j).lerp(active, blinkPct[i]));
                }
            }
            
            if(buttonState[i] == STATE_OFF){
                for (int j = i*dirMeshVerts; j < (i*dirMeshVerts)+dirMeshVerts; j++) {
                    directionMesh.setColor(j, directionMesh.getColor(j).lerp(offColor, blinkPct[i]));
                }
            }
        }
        
        //cout << blinkPct[i]  << endl;
        if (blink[i] && blinkPct[i] >= 1.0){
            blink[i] = false;
            blinkPct[i] = 1.0;
        }
        
    }
}

void InterfacePlane::update(int& stepper, float& tickTime_, int& scanDir_, bool connected_[], bool active_[], bool& pause_, int& globalState_, float &hue_, int& pulseDiv_) {
    
    transformButton(connected_, active_, globalState_);
    blinkP();
    
    ofNode tempNode;
    lineMesh.clear();
    
    lastStepper = curStepper;
    curStepper = stepper;
    stepperArg = stepper;
    
    lastTime = thisTime;
    thisTime = (ofGetElapsedTimeMillis()- lastTick) ;


    if (!pause_) {
        
       
        scanDir = scanDir_;

        
        //cout << linePct << endl;
        
        
        float meshZ;
        
        if(meshBig){
            meshZ = 32;
        }
        else {
            meshZ = 0;
        }
        
        //linemesh
        for (int i = (4-pulseDiv_); i >= 0; i--) {
            float temptick =len(tickTime_, i, pulseDiv_);
            tempNode = getRotNode(stepperArg, temptick, scanDir_, connected_, active_);
            ofVec3f tempA = ofVec3f(4,0,0) * tempNode.getGlobalTransformMatrix();
            tempA.z = 18 + meshZ - (18*thisScale) + 1;
            ofVec3f tempB = ofVec3f(-4,0,0) * tempNode.getGlobalTransformMatrix();
            tempB.z = 18 + meshZ - (18*thisScale) + 1;
            if (flipCounter%2==0){
                lineMeshQA.push_back(tempA);
                lineMeshQB.push_back(tempB);
            }else {
                lineMeshQA.push_back(tempB);
                lineMeshQB.push_back(tempA);
            }
            
            
            
        }
        
        while (lineMeshQA.size() > (20  * (5-pulseDiv_) ) ) {
            lineMeshQA.pop_front();
            lineMeshQB.pop_front();
        }
        
        for (int i = 0; i < lineMeshQA.size(); i++) {
            lineMesh.addVertex(lineMeshQA.at(i));
            trailColor.a = (255/lineMeshQA.size())*i;
            lineMesh.addColor(trailColor);
            
            
        }
        
        
        for (int i = lineMeshQA.size()-1; i > 0; i--) {
            lineMesh.addVertex(lineMeshQB.at(i));
            trailColor.a = (255/lineMeshQA.size())*i;
            lineMesh.addColor(trailColor);
        }
        
        
    }
    
    
    
    
    
}

float InterfacePlane::len(float tickTime_, int div_, int& pDiv_){
    float temp;
 
    if(thisTime >= tickTime_){
        temp = 1.0;
    } else {
        float timeDivision = thisTime-lastTime;
        if(timeDivision < 0.0){
            timeDivision = thisTime;
           // stepperArg = (curStepper-1)%5;
        } else {
           // stepperArg = curStepper;
        }
        float useTime = thisTime -  ((timeDivision/(5-pDiv_))*div_) ;
 
        useTime = ofWrap(useTime, 0.0, tickTime_);
        temp =ofMap( fmod(useTime, tickTime_ ), 0.0  , tickTime_ , 0.0, 1.0);
        
        
        cout << "tickTime " << tickTime_ << "   timeDivision "<< timeDivision <<  "   thisTime " << thisTime <<   "   iterator " << div_ <<"   useTime " << useTime << "  map  " << temp << endl;
    }
    
    
     return temp;
}

ofNode InterfacePlane::getRotNode(int& stepper, float& len, int& scanDir_ , bool connected_[],bool active_[]){
    
   
    
    alphaPart = TWO_PI/((tiles+1)*4);
    
    pctBar = ofMap(stepper+len, 0, tiles+1, 0.0, 1.0);
    
    ofMatrix4x4 aaa;
    
    
    
    if (nextDirs[1] && nextDirs[0]) {
        alpha = ofMap( stepper, 0,  (tiles+1), 0.0, HALF_PI) + scanDir_*HALF_PI;
        alpha -= HALF_PI;
        alpha += len*alphaPart;
        aaa.rotateRad(ofLerp(0.0, PI+HALF_PI,pctBar)+scanDir_*HALF_PI, 0, 0, 1);
        linePowMod = 3;
        if(connected_[scanDir]){
            if (connected_[(scanDir+1)%4]){
                trailColor = connected;
            } else if(active_[(scanDir+1)%4]){
                trailColor = connected.getLerped(active, pctBar);
            }
        } else if(active_[scanDir]){
            if(connected_[(scanDir+1)%4]){
                trailColor = active.getLerped(connected, pctBar);
            } else if(active_[(scanDir+1)%4]){
                trailColor = active;
            }
        }
        
    }else if (!nextDirs[1]){
        if(nextDirs[2]) {
            linePowMod = 5;
            alpha =   ofMap( stepper, 0,  (tiles+1), 0.0, HALF_PI)  + scanDir_*HALF_PI;
            alpha += ofMap( stepper, 0,  (tiles+1), 0.0, HALF_PI);
            alpha -= HALF_PI;
            alpha+= len*(alphaPart*2);
            aaa.rotateRad(ofLerp(0.0, PI+HALF_PI,pctBar)+(scanDir_*HALF_PI)+(HALF_PI*pctBar), 0, 0, 1) ;
            
            if(connected_[scanDir]){
                if (connected_[(scanDir+2)%4]){
                    trailColor = connected;
                } else if(active_[(scanDir+2)%4]){
                    trailColor = connected.getLerped(active, pctBar);
                }
            } else if(active_[scanDir]){
                if(connected_[(scanDir+2)%4]){
                    trailColor = active.getLerped(connected, pctBar);
                } else if(active_[(scanDir+2)%4]){
                    trailColor = active;
                }
            }
            
        } else if (nextDirs[3]) {
            linePowMod = 10;
            alpha =   ofMap( stepper, 0,  (tiles+1), 0.0, HALF_PI)  + scanDir_*HALF_PI;
            alpha += ofMap( stepper, 0,  (tiles+1), 0.0, HALF_PI)*2;
            alpha -= HALF_PI;
            alpha+= len*(alphaPart*3);
            aaa.rotateRad(ofLerp(0.0, PI+HALF_PI,pctBar)+(scanDir_*HALF_PI)+((HALF_PI*pctBar)*2), 0, 0, 1);
            
            if(connected_[scanDir]){
                if (connected_[(scanDir+3)%4]){
                    trailColor = connected;
                } else if(active_[(scanDir+3)%4]){
                    trailColor = connected.getLerped(active, pctBar);
                }
            } else if(active_[scanDir]){
                if(connected_[(scanDir+3)%4]){
                    trailColor = active.getLerped(connected, pctBar);
                } else if(active_[(scanDir+3)%4]){
                    trailColor = active;
                }
            }
            
        } else {
            linePowMod = 18;
            alpha =   ofMap( stepper, 0,  (tiles+1), 0.0, HALF_PI)  + scanDir_*HALF_PI;
            alpha += ofMap( stepper, 0,  (tiles+1), 0.0, HALF_PI)*3;
            alpha -= HALF_PI;
            alpha+= len*(alphaPart*4);
            aaa.rotateRad(ofLerp(0.0, PI+HALF_PI,pctBar)+(scanDir_*HALF_PI)+((HALF_PI*pctBar)*3), 0, 0, 1);
            
            if(connected_[scanDir]){
                if (connected_[(scanDir+4)%4]){
                    trailColor = connected;
                } else if(active_[(scanDir+4)%4]){
                    trailColor = connected.getLerped(active, pctBar);
                }
            } else if(active_[scanDir]){
                if(connected_[(scanDir+4)%4]){
                    trailColor = active.getLerped(connected, pctBar);
                } else if(active_[(scanDir+4)%4]){
                    trailColor = active;
                }
            }
        }
        
    }
    
    
    alphaPos = ofVec3f(sin(alpha)*gridSize*(1.3 +( (scaleMod-1.0)*0.4 )), cos(alpha)*gridSize*(1.3  +( (scaleMod-1.0)*0.4)),0 );
    
    
    float pctScale = ofMap(stepCounter+len,0, (tiles+1)*4,0.0,TWO_PI*2);
    
    
    pulseRot.setRotate(aaa.getRotate().inverse());
    
    ofNode posNode;
    
    posNode.setOrientation(pulseRot.getRotate());
    
    posNode.setPosition(alphaPos);
    
    float scalePct = (abs(sin(pctScale-(HALF_PI*1.3333)))*1);
    thisScale =  ofClamp(pow(scalePct, linePowMod),0.0,1.0);
    
    linePct = ofClamp(ofMap(pctBar, 0.0, 1.0, 0.0, (1.0+(1/tiles))), 0.0, 1.0);
    
    
    posNode.setPosition( posNode.getPosition()*((-thisScale*0.18*positionMod)+1.0)) ;
    posNode.setScale( (thisScale*1.25*scaleMod) +0.45 );
    
    return posNode;
}


void InterfacePlane::draw( bool& pause_){
    
    if (!pause_) {
        
        // ofPushStyle();
       // posNode.transformGL();
        
        // ofSetColor(filterColor( ofColor( 255, 255, 255,lineAlpha)) );
        
        glLineWidth(2);

       // ofLine(-8, 0, 8, 0);
        
   
        
       // posNode.restoreTransformGL();
        //  ofPopStyle();
        directionMesh.draw();
        
        lineMesh.draw();

        /*
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
        */
    }
}


void InterfacePlane::pulseDir(int dir_) {
    
    blink[dir_] = true;
    blinkPct[dir_] = 0.01;
    for (int j = dir_*dirMeshVerts; j < (dir_*dirMeshVerts)+dirMeshVerts; j++) {
        if(meshState[dir_] == STATE_ACTIVE){
        directionMesh.setColor(j, pulseColorA);
        } else {
            directionMesh.setColor(j, pulseColorC);
        }
    }
    
}



