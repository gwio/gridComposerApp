//
//  interfacePlane.cpp
//  sound_table_app
//
//  Created by Christian Gwiozda on 06.03.15.
//
//

#include "interfacePlane.h"


InterfacePlane::InterfacePlane() {
    activeOnColor = ofColor::cyan;
    activeOffColor = ofColor::orangeRed;
    
    connectedOnColor = ofColor::cyan;
    connectedOffColor = ofColor::orangeRed;
    
    onPositionPct = 1.0;
    connectedAniPct = 1.0;
    pctSpeed = 0.05;
    connectedPctSpeed = 0.05;
}


void InterfacePlane::setup(ofVec3f center_, float planeSize_A, int id_, float gridSize_, ofVboMesh &i_planeMesh, ofVboMesh &i_planeFboMesh, ofVboMesh &i_connected, ofVec3f translateVec_ ){
    //ids 1 left, 2 up, 3 right, 4 down
    float breite = gridSize_*0.7;
    float planeSize_ = planeSize_A*0.86;
    float abstand = 1.5;
    //depends on tilesize
    float arrow = 0.5;
    ofVec3f center;
    direction = id_;
    active =true;
    connected = true;
    int indexT[12] = {0,1,4,4,5,0,1,2,3,3,4,1};
    
    activeMesh = &i_planeMesh;
    fbo_activeMesh = &i_planeFboMesh;
    connectedMesh = &i_connected;
    
    activeDrawColor = activeOnColor;
    connectedDrawColor = connectedOnColor;
    
    
    switch (id_)
    {
            //left
        case 0:
            
            activeMeshON = ofVec3f(+gridSize_*arrow,0,0);
            center = center_+ ofVec3f(-gridSize_*abstand,0,0) ;
            
            //0
            i_planeMesh.addVertex(center+ ofVec3f(-breite/2,-planeSize_/2,0) );
            //1
            i_planeMesh.addVertex(center+ ofVec3f(-breite/2,0,0) );
            default1 = center+ ofVec3f(-breite/2,0,0);
            //2
            i_planeMesh.addVertex(center+ ofVec3f(-breite/2,planeSize_/2,0) );
            //3
            i_planeMesh.addVertex(center+ ofVec3f(+breite/2,planeSize_/2,0) );
            //4
            i_planeMesh.addVertex(center+ ofVec3f(+breite/2,0,0) );
            default4 = center+ ofVec3f(+breite/2,0,0);
            //5
            i_planeMesh.addVertex(center+ ofVec3f(+breite/2,-planeSize_/2,0) );
            
            //set on position for start
            i_planeMesh.setVertex(1+(direction*6), default1+(activeMeshON));
            i_planeMesh.setVertex(4+(direction*6), default4+(activeMeshON));
            
            for (int i = 0; i < 12; i++) {
                i_planeMesh.addIndex(indexT[i]+(direction*6));
            }
            
            
            for (int i = 0; i < 12 ; i++) {
                i_planeMesh.addColor(activeOnColor);
            }
            
            
            
            i_planeFboMesh.addVertex(center+ ofVec3f(-gridSize_/2,-planeSize_/2,0));
            i_planeFboMesh.addVertex(center+ ofVec3f(-gridSize_/2,planeSize_/2,0));
            i_planeFboMesh.addVertex(center+ ofVec3f(+breite/2,planeSize_/2,0) + activeMeshON);
            i_planeFboMesh.addVertex(center+ ofVec3f(+breite/2,-planeSize_/2,0) + activeMeshON);
            
            i_planeFboMesh.addColor(ofColor(id_+1,0,0));
            i_planeFboMesh.addColor(ofColor(id_+1,0,0));
            i_planeFboMesh.addColor(ofColor(id_+1,0,0));
            i_planeFboMesh.addColor(ofColor(id_+1,0,0));
            
            i_planeFboMesh.addIndex(0);
            i_planeFboMesh.addIndex(1);
            i_planeFboMesh.addIndex(2);
            i_planeFboMesh.addIndex(2);
            i_planeFboMesh.addIndex(3);
            i_planeFboMesh.addIndex(0);
            
            connectedOff1= center_ + (default4+(activeMeshON)-center_)/2 + ofVec3f(0,-planeSize_/2,0);
            connectedOff2= center_ + (default4+(activeMeshON)-center_)/2 + ofVec3f(0,+planeSize_/2,0);
            
            connectedOn2= default4+(activeMeshON);
            connectedOn1=center_;
            
            connectedAni = connectedOn1-connectedOff1;
            
            i_connected.addVertex(connectedOn1);
            i_connected.addVertex(connectedOn2);
            i_connected.addColor(connectedOnColor);
            i_connected.addColor(connectedOnColor);
            
            i_planeFboMesh.addVertex(center+ ofVec3f( breite/2,-planeSize_/2,0 )+ activeMeshON);
            i_planeFboMesh.addVertex(center+ ofVec3f( breite/2,+planeSize_/2,0 )+ activeMeshON);
            i_planeFboMesh.addVertex(center_+ ofVec3f( 0,+planeSize_/2,0 ));
            i_planeFboMesh.addVertex(center_+ ofVec3f( 0,-planeSize_/2,0 ));
            
            i_planeFboMesh.addColor(ofColor(id_+1+4,0,0));
            i_planeFboMesh.addColor(ofColor(id_+1+4,0,0));
            i_planeFboMesh.addColor(ofColor(id_+1+4,0,0));
            i_planeFboMesh.addColor(ofColor(id_+1+4,0,0));
            
            i_planeFboMesh.addIndex(0+16);
            i_planeFboMesh.addIndex(1+16);
            i_planeFboMesh.addIndex(2+16);
            i_planeFboMesh.addIndex(2+16);
            i_planeFboMesh.addIndex(3+16);
            i_planeFboMesh.addIndex(0+16);
            
            default1 += translateVec_;
            default4 += translateVec_;
            
            connectedOn1 += translateVec_;
            connectedOn2 += translateVec_;
            
            break;
            
            
        case 1:
            
            activeMeshON = ofVec3f(0,0-gridSize_*arrow,0) ;
            center = center_+ ofVec3f(0,gridSize_*abstand,0) ;
            
            
            //0
            i_planeMesh.addVertex(center+ ofVec3f(-planeSize_/2,+breite/2,0) );
            //1
            i_planeMesh.addVertex(center+ ofVec3f(0,+breite/2,0) );
            default1 = center+ ofVec3f(0,+breite/2,0);
            //2
            i_planeMesh.addVertex(center+ ofVec3f(+planeSize_/2,+breite/2,0) );
            //3
            i_planeMesh.addVertex(center+ ofVec3f(+planeSize_/2,-breite/2,0) );
            //4
            i_planeMesh.addVertex(center+ ofVec3f(0,-breite/2,0) );
            default4 = center+ ofVec3f(0,-breite/2,0) ;
            //5
            i_planeMesh.addVertex(center+ ofVec3f(-planeSize_/2,-breite/2,0) );
            
            //set on position for start
            i_planeMesh.setVertex(1+(direction*6),default1+(activeMeshON));
            i_planeMesh.setVertex(4+(direction*6), default4+(activeMeshON));;
            
            for (int i = 0; i < 12; i++) {
                i_planeMesh.addIndex(indexT[i]+(direction*6));
            }
            
            for (int i = 0; i < 12 ; i++) {
                i_planeMesh.addColor(activeOnColor);
            }
            
            
            
            i_planeFboMesh.addVertex(center+ ofVec3f(-planeSize_/2,+gridSize_/2,0));
            i_planeFboMesh.addVertex(center+ ofVec3f(+planeSize_/2,+gridSize_/2,0));
            i_planeFboMesh.addVertex(center+ ofVec3f(+planeSize_/2,-breite/2,0)+activeMeshON);
            i_planeFboMesh.addVertex(center+ ofVec3f(-planeSize_/2,-breite/2,0)+activeMeshON);
            
            i_planeFboMesh.addColor(ofColor(id_+1,0,0));
            i_planeFboMesh.addColor(ofColor(id_+1,0,0));
            i_planeFboMesh.addColor(ofColor(id_+1,0,0));
            i_planeFboMesh.addColor(ofColor(id_+1,0,0));
            
            i_planeFboMesh.addIndex(0+(direction*4));
            i_planeFboMesh.addIndex(1+(direction*4));
            i_planeFboMesh.addIndex(2+(direction*4));
            i_planeFboMesh.addIndex(2+(direction*4));
            i_planeFboMesh.addIndex(3+(direction*4));
            i_planeFboMesh.addIndex(0+(direction*4));
            
            
            connectedOff1= center_ + (default4+(activeMeshON)-center_)/2 + ofVec3f(-planeSize_/2,0,0);
            connectedOff2= center_ + (default4+(activeMeshON)-center_)/2 + ofVec3f(+planeSize_/2,0,0);
            
            connectedOn2= default4+(activeMeshON);
            connectedOn1=center_;
            
            connectedAni = connectedOn1-connectedOff1;
            
            i_connected.addVertex(connectedOn1);
            i_connected.addVertex(connectedOn2);
            i_connected.addColor(connectedOnColor);
            i_connected.addColor(connectedOnColor);
            
            i_planeFboMesh.addVertex(center+ ofVec3f(-planeSize_/2, -breite/2,0 )+activeMeshON);
            i_planeFboMesh.addVertex(center+ ofVec3f(+planeSize_/2, -breite/2,0 )+activeMeshON);
            i_planeFboMesh.addVertex(center_+ ofVec3f(+planeSize_/2,0,0 ));
            i_planeFboMesh.addVertex(center_+ ofVec3f(-planeSize_/2,0,0 ));
            
            i_planeFboMesh.addColor(ofColor(id_+1+4,0,0));
            i_planeFboMesh.addColor(ofColor(id_+1+4,0,0));
            i_planeFboMesh.addColor(ofColor(id_+1+4,0,0));
            i_planeFboMesh.addColor(ofColor(id_+1+4,0,0));
            
            i_planeFboMesh.addIndex(0+(direction*4)+16);
            i_planeFboMesh.addIndex(1+(direction*4)+16);
            i_planeFboMesh.addIndex(2+(direction*4)+16);
            i_planeFboMesh.addIndex(2+(direction*4)+16);
            i_planeFboMesh.addIndex(3+(direction*4)+16);
            i_planeFboMesh.addIndex(0+(direction*4)+16);
            
            
            default1 += translateVec_;
            default4 += translateVec_;

            connectedOn1 += translateVec_;
            connectedOn2 += translateVec_;
            
            break;
            
        case 2:
            
            activeMeshON = ofVec3f(-gridSize_*arrow,0,0) ;
            center = center_+ ofVec3f(+gridSize_*abstand,0,0) ;
            
            
            
            //0
            i_planeMesh.addVertex(center+ ofVec3f(+breite/2,+planeSize_/2,0) );
            //1
            i_planeMesh.addVertex(center+ ofVec3f(+breite/2,0,0) );
            default1 = center+ ofVec3f(+breite/2,0,0);
            //2
            i_planeMesh.addVertex(center+ ofVec3f(+breite/2,-planeSize_/2,0) );
            //3
            i_planeMesh.addVertex(center+ ofVec3f(-breite/2,-planeSize_/2,0) );
            //4
            i_planeMesh.addVertex(center+ ofVec3f(-breite/2,0,0) );
            default4 = center+ ofVec3f(-breite/2,0,0);
            //5
            i_planeMesh.addVertex(center+ ofVec3f(-breite/2,+planeSize_/2,0) );
            
            //set on position for start
            i_planeMesh.setVertex(1+(direction*6),default1+(activeMeshON));
            i_planeMesh.setVertex(4+(direction*6), default4+(activeMeshON));
            
            for (int i = 0; i < 12; i++) {
                i_planeMesh.addIndex(indexT[i]+(direction*6));
            }
            
            for (int i = 0; i < 12 ; i++) {
                i_planeMesh.addColor(activeOnColor);
            }
            
            
            
            i_planeFboMesh.addVertex(center+ ofVec3f(+gridSize_/2,+planeSize_/2,0));
            i_planeFboMesh.addVertex(center+ ofVec3f(+gridSize_/2,-planeSize_/2,0));
            i_planeFboMesh.addVertex(center+ ofVec3f(-breite/2,-planeSize_/2,0)+activeMeshON);
            i_planeFboMesh.addVertex(center+ ofVec3f(-breite/2,+planeSize_/2,0)+activeMeshON);
            
            i_planeFboMesh.addColor(ofColor(id_+1,0,0));
            i_planeFboMesh.addColor(ofColor(id_+1,0,0));
            i_planeFboMesh.addColor(ofColor(id_+1,0,0));
            i_planeFboMesh.addColor(ofColor(id_+1,0,0));
            
            i_planeFboMesh.addIndex(0+(direction*4));
            i_planeFboMesh.addIndex(1+(direction*4));
            i_planeFboMesh.addIndex(2+(direction*4));
            i_planeFboMesh.addIndex(2+(direction*4));
            i_planeFboMesh.addIndex(3+(direction*4));
            i_planeFboMesh.addIndex(0+(direction*4));
            
            connectedOff1= center_ + (default4+(activeMeshON)-center_)/2 + ofVec3f(0,+planeSize_/2,0);
            connectedOff2= center_ + (default4+(activeMeshON)-center_)/2 + ofVec3f(0,-planeSize_/2,0);
            
            connectedOn2= default4+(activeMeshON);
            connectedOn1=center_;
            
            connectedAni = connectedOn1-connectedOff1;
            
            i_connected.addVertex(connectedOn1);
            i_connected.addVertex(connectedOn2);
            i_connected.addColor(connectedOnColor);
            i_connected.addColor(connectedOnColor);
            
            
            i_planeFboMesh.addVertex(center+ ofVec3f( -breite/2,+planeSize_/2,0 )+activeMeshON);
            i_planeFboMesh.addVertex(center+ ofVec3f( -breite/2,-planeSize_/2,0 )+activeMeshON);
            i_planeFboMesh.addVertex(center_+ ofVec3f( 0,-planeSize_/2,0 ));
            i_planeFboMesh.addVertex(center_+ ofVec3f( 0,+planeSize_/2,0 ));
            
            
            i_planeFboMesh.addColor(ofColor(id_+1+4,0,0));
            i_planeFboMesh.addColor(ofColor(id_+1+4,0,0));
            i_planeFboMesh.addColor(ofColor(id_+1+4,0,0));
            i_planeFboMesh.addColor(ofColor(id_+1+4,0,0));
            
            i_planeFboMesh.addIndex(0+(direction*4)+16);
            i_planeFboMesh.addIndex(1+(direction*4)+16);
            i_planeFboMesh.addIndex(2+(direction*4)+16);
            i_planeFboMesh.addIndex(2+(direction*4)+16);
            i_planeFboMesh.addIndex(3+(direction*4)+16);
            i_planeFboMesh.addIndex(0+(direction*4)+16);
            
            default1 += translateVec_;
            default4 += translateVec_;
            
            connectedOn1 += translateVec_;
            connectedOn2 += translateVec_;

            
            break;
            
        case 3:
            
            activeMeshON = ofVec3f(0,+gridSize_*arrow,0) ;
            center = center_+ ofVec3f(0,-gridSize_*abstand,0) ;
            
            
            
            //0
            i_planeMesh.addVertex(center+ ofVec3f(planeSize_/2,-breite/2,0) );
            //1
            i_planeMesh.addVertex(center+ ofVec3f(0,-breite/2,0) );
            default1 = center+ ofVec3f(0,-breite/2,0);
            //2
            i_planeMesh.addVertex(center+ ofVec3f(-planeSize_/2,-breite/2,0) );
            //3
            i_planeMesh.addVertex(center+ ofVec3f(-planeSize_/2,+breite/2,0) );
            //4
            i_planeMesh.addVertex(center+ ofVec3f(0,+breite/2,0) );
            default4 = center+ ofVec3f(0,+breite/2,0);
            //5
            i_planeMesh.addVertex(center+ ofVec3f(planeSize_/2,+breite/2,0) );
            
            //set on position for start
            i_planeMesh.setVertex(1+(direction*6),default1+(activeMeshON));
            i_planeMesh.setVertex(4+(direction*6), default4+(activeMeshON));
            
            for (int i = 0; i < 12; i++) {
                i_planeMesh.addIndex(indexT[i]+(direction*6));
            }
            
            for (int i = 0; i < 12 ; i++) {
                i_planeMesh.addColor(activeOnColor);
            }
            
            
            
            i_planeFboMesh.addVertex(center+ ofVec3f(planeSize_/2,-gridSize_/2,0));
            i_planeFboMesh.addVertex(center+ ofVec3f(-planeSize_/2,-gridSize_/2,0));
            i_planeFboMesh.addVertex(center+ ofVec3f(-planeSize_/2,breite/2,0)+activeMeshON);
            i_planeFboMesh.addVertex(center+ ofVec3f(planeSize_/2,breite/2,0)+activeMeshON);
            
            i_planeFboMesh.addColor(ofColor(id_+1,0,0));
            i_planeFboMesh.addColor(ofColor(id_+1,0,0));
            i_planeFboMesh.addColor(ofColor(id_+1,0,0));
            i_planeFboMesh.addColor(ofColor(id_+1,0,0));
            
            i_planeFboMesh.addIndex(0+(direction*4));
            i_planeFboMesh.addIndex(1+(direction*4));
            i_planeFboMesh.addIndex(2+(direction*4));
            i_planeFboMesh.addIndex(2+(direction*4));
            i_planeFboMesh.addIndex(3+(direction*4));
            i_planeFboMesh.addIndex(0+(direction*4));
            
            connectedOff1= center_ + (default4+(activeMeshON)-center_)/2 + ofVec3f(planeSize_/2,0,0);
            connectedOff2= center_ + (default4+(activeMeshON)-center_)/2 + ofVec3f(-planeSize_/2,0,0);
            
            connectedOn2= default4+(activeMeshON);
            connectedOn1=center_;
            
            connectedAni = connectedOn1-connectedOff1;
            
            i_connected.addVertex(connectedOn1);
            i_connected.addVertex(connectedOn2);
            i_connected.addColor(connectedOnColor);
            i_connected.addColor(connectedOnColor);
            
            i_planeFboMesh.addVertex(center+ ofVec3f(planeSize_/2, breite/2,0 )+activeMeshON);
            i_planeFboMesh.addVertex(center+ ofVec3f(-planeSize_/2,breite/2,0 )+activeMeshON);
            i_planeFboMesh.addVertex(center_+ ofVec3f(-planeSize_/2,0,0 ));
            i_planeFboMesh.addVertex(center_+ ofVec3f(+planeSize_/2,0,0 ));
            
            i_planeFboMesh.addColor(ofColor(id_+1+4,0,0));
            i_planeFboMesh.addColor(ofColor(id_+1+4,0,0));
            i_planeFboMesh.addColor(ofColor(id_+1+4,0,0));
            i_planeFboMesh.addColor(ofColor(id_+1+4,0,0));
            
            i_planeFboMesh.addIndex(0+(direction*4)+16);
            i_planeFboMesh.addIndex(1+(direction*4)+16);
            i_planeFboMesh.addIndex(2+(direction*4)+16);
            i_planeFboMesh.addIndex(2+(direction*4)+16);
            i_planeFboMesh.addIndex(3+(direction*4)+16);
            i_planeFboMesh.addIndex(0+(direction*4)+16);
            
            
            default1 += translateVec_;
            default4 += translateVec_;

            connectedOn1 += translateVec_;
            connectedOn2 += translateVec_;
            
            break;
            
            
    }
}

void InterfacePlane::update() {
    
    
    //active effect
    
    if(active) {
        if (activeOnColor != activeDrawColor) {
            activeDrawColor = activeDrawColor.lerp(activeOnColor, 0.1);
            for (int i = 0; i < 6; i++) {
                activeMesh->setColor(i+(direction*6), activeDrawColor);
            }
        }
        
        if(onPositionPct < 1){
            onPositionPct+=pctSpeed;
            pctTemp = (sin(ofMap(onPositionPct, 0.0, 1.0, HALF_PI+PI, HALF_PI))*0.5)+0.5;
            activeMesh->setVertex(1+(direction*6),default1+(activeMeshON * pctTemp ));
            activeMesh->setVertex(4+(direction*6), default4+(activeMeshON * pctTemp ));
            
        }
        
    }
    
    if (!active) {
        if ( activeOffColor != activeDrawColor) {
            activeDrawColor = activeDrawColor.lerp(activeOffColor, 0.1);
            for (int i = 0; i < 6; i++) {
                activeMesh->setColor(i+(direction*6), activeDrawColor);
            }
        }
        
        
        if(onPositionPct > 0){
            onPositionPct-=pctSpeed;
            pctTemp = (sin(ofMap(onPositionPct, 0.0, 1.0, HALF_PI+PI, HALF_PI))*0.5)+0.5;
            activeMesh->setVertex(1+(direction*6),default1+(activeMeshON * pctTemp ));
            activeMesh->setVertex(4+(direction*6), default4+(activeMeshON * pctTemp ));
            
        }
    }
    
    //connected effect
    
    if (connected) {
        
        
        if (!active) {
            if (connectedOffColor != connectedDrawColor) {
                connectedDrawColor = connectedDrawColor.lerp(connectedOffColor, 0.1);
                connectedMesh->setColor(0+(direction*2), connectedDrawColor);
                connectedMesh->setColor(1+(direction*2), connectedDrawColor);
            }
        } else {
            if (connectedOnColor != connectedDrawColor) {
                connectedDrawColor = connectedDrawColor.lerp(connectedOnColor, 0.1);
                connectedMesh->setColor(0+(direction*2), connectedDrawColor);
                connectedMesh->setColor(1+(direction*2), connectedDrawColor);
            }
        }
        
        
        
        
        //hack with 1- float error 0.0000111???
        if(1-connectedAniPct > 0){
            
            connectedAniPct+=connectedPctSpeed;
            connectedPctTemp = (sin(ofMap(1-connectedAniPct, 0.0, 1.0,HALF_PI+PI, HALF_PI))*0.5)+0.5;
            connectedMesh->setVertex(0+(direction*2), connectedOn1-(connectedAni*connectedPctTemp));
            connectedMesh->setVertex(1+(direction*2), connectedOn2+(connectedAni*connectedPctTemp));
            
        }
        
    }
    
    if (!connected) {
        
        if (connectedOffColor != connectedDrawColor) {
            connectedDrawColor = connectedDrawColor.lerp(connectedOffColor, 0.1);
            connectedMesh->setColor(0+(direction*2), connectedDrawColor);
            connectedMesh->setColor(1+(direction*2), connectedDrawColor);
            
        }
        
        if(1-connectedAniPct < 1){
            connectedAniPct-=connectedPctSpeed;
            connectedPctTemp = (sin(ofMap(1-connectedAniPct, 0.0, 1.0,HALF_PI+PI, HALF_PI))*0.5)+0.5;
            connectedMesh->setVertex(0+(direction*2), connectedOn1-(connectedAni*connectedPctTemp));
            connectedMesh->setVertex(1+(direction*2), connectedOn2+(connectedAni*connectedPctTemp));
        }
    }
}

void InterfacePlane::draw() {
    
    
}

void InterfacePlane::pulse() {
    activeDrawColor = ofColor::white;
    if(connected) {
        connectedDrawColor = ofColor::white;
    }
}

void InterfacePlane::drawFbo(){
}