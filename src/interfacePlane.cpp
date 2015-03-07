//
//  interfacePlane.cpp
//  sound_table_app
//
//  Created by Christian Gwiozda on 06.03.15.
//
//

#include "interfacePlane.h"


InterfacePlane::InterfacePlane() {
    activeOnColor = ofColor::fromHsb(77, 90, 80);
    activeOffColor = ofColor::darkRed;
    onPositionPct = 1.0;
    pctSpeed = 0.1;
}


void InterfacePlane::setup(ofVec3f center_, float planeSize_A, int id_, float gridSize_){
    //ids 1 left, 2 up, 3 right, 4 down
    float breite = gridSize_*0.06;
    float planeSize_ = planeSize_A/2;
    ofVec3f center;
    int indexT[12] = {0,1,4,4,5,0,1,2,3,3,4,1};
    switch (id_)
    {
            //left
        case 1:
        
            activeMeshON = ofVec3f(+gridSize_*0.33,0,0);
            center = center_+ ofVec3f(-gridSize_*1.33,0,0);
            
            activeMesh.clear();
            //0
            activeMesh.addVertex(center+ ofVec3f(-breite/2,-planeSize_/2,0) );
            //1
            activeMesh.addVertex(center+ ofVec3f(-breite/2,0,0) );
            default1 = center+ ofVec3f(-breite/2,0,0);
            //2
            activeMesh.addVertex(center+ ofVec3f(-breite/2,planeSize_/2,0) );
            //3
            activeMesh.addVertex(center+ ofVec3f(+breite/2,planeSize_/2,0) );
            //4
            activeMesh.addVertex(center+ ofVec3f(+breite/2,0,0) );
            default4 = center+ ofVec3f(+breite/2,0,0);
            //5
            activeMesh.addVertex(center+ ofVec3f(+breite/2,-planeSize_/2,0) );
            
           //set on position for start
            activeMesh.setVertex(1,default1+(activeMeshON));
            activeMesh.setVertex(4, default4+(activeMeshON));

            for (int i = 0; i < 12; i++) {
                activeMesh.addIndex(indexT[i]);
            }
            
         
            for (int i = 0; i < 12 ; i++) {
                activeMesh.addColor(activeOnColor);
            }
            
            activeDrawColor = activeOnColor;
            active =true;
            
            activeMesh.setMode(OF_PRIMITIVE_TRIANGLES);
            
            fbo_activeMesh.clear();
            fbo_activeMesh = activeMesh;
            fbo_activeMesh.clearColors();
            
            for (int i = 0; i < 12 ; i++) {
                fbo_activeMesh.addColor(ofColor(id_,0,0));
            }
            
            
            break;
            
            
        case 2:
            
            activeMeshON = ofVec3f(0,0-gridSize_*0.33,0);
            center = center_+ ofVec3f(0,gridSize_*1.33,0);

            activeMesh.clear();
           
            //0
            activeMesh.addVertex(center+ ofVec3f(-planeSize_/2,+breite/2,0) );
            //1
            activeMesh.addVertex(center+ ofVec3f(0,+breite/2,0) );
            default1 = center+ ofVec3f(0,+breite/2,0);
            //2
            activeMesh.addVertex(center+ ofVec3f(+planeSize_/2,+breite/2,0) );
            //3
            activeMesh.addVertex(center+ ofVec3f(+planeSize_/2,-breite/2,0) );
            //4
            activeMesh.addVertex(center+ ofVec3f(0,-breite/2,0) );
            default4 = center+ ofVec3f(0,-breite/2,0) ;
            //5
            activeMesh.addVertex(center+ ofVec3f(-planeSize_/2,-breite/2,0) );
            
            //set on position for start
            activeMesh.setVertex(1,default1+(activeMeshON));
            activeMesh.setVertex(4, default4+(activeMeshON));;

            for (int i = 0; i < 12; i++) {
                activeMesh.addIndex(indexT[i]);
            }
            
            for (int i = 0; i < 12 ; i++) {
                activeMesh.addColor(activeOnColor);
            }
            
            activeDrawColor = activeOnColor;
            active =true;

            
            activeMesh.setMode(OF_PRIMITIVE_TRIANGLES);
            
            fbo_activeMesh.clear();
            fbo_activeMesh = activeMesh;
            fbo_activeMesh.clearColors();
            
            for (int i = 0; i < 12 ; i++) {
                fbo_activeMesh.addColor(ofColor(id_,0,0));
            }
            break;
            
        case 3:
            
            activeMeshON = ofVec3f(-gridSize_*0.33,0,0);
            center = center_+ ofVec3f(+gridSize_*1.33,0,0);

            activeMesh.clear();
           
           
            //0
            activeMesh.addVertex(center+ ofVec3f(+breite/2,+planeSize_/2,0) );
            //1
            activeMesh.addVertex(center+ ofVec3f(+breite/2,0,0) );
            default1 = center+ ofVec3f(+breite/2,0,0);
            //2
            activeMesh.addVertex(center+ ofVec3f(+breite/2,-planeSize_/2,0) );
            //3
            activeMesh.addVertex(center+ ofVec3f(-breite/2,-planeSize_/2,0) );
            //4
            activeMesh.addVertex(center+ ofVec3f(-breite/2,0,0) );
            default4 = center+ ofVec3f(-breite/2,0,0);
            //5
            activeMesh.addVertex(center+ ofVec3f(-breite/2,+planeSize_/2,0) );
           
            //set on position for start
            activeMesh.setVertex(1,default1+(activeMeshON));
            activeMesh.setVertex(4, default4+(activeMeshON));

            for (int i = 0; i < 12; i++) {
                activeMesh.addIndex(indexT[i]);
            }
            
            for (int i = 0; i < 12 ; i++) {
                activeMesh.addColor(activeOnColor);
            }
            activeDrawColor = activeOnColor;
            active =true;

            
            activeMesh.setMode(OF_PRIMITIVE_TRIANGLES);
            
            fbo_activeMesh.clear();
            fbo_activeMesh = activeMesh;
            fbo_activeMesh.clearColors();
            
            for (int i = 0; i < 12 ; i++) {
                fbo_activeMesh.addColor(ofColor(id_,0,0));
            }
            break;
            
        case 4:
            
            activeMeshON = ofVec3f(0,+gridSize_*0.33,0);
            center = center_+ ofVec3f(0,-gridSize_*1.33,0);
            
            activeMesh.clear();
            
            
            //0
            activeMesh.addVertex(center+ ofVec3f(planeSize_/2,-breite/2,0) );
            //1
            activeMesh.addVertex(center+ ofVec3f(0,-breite/2,0) );
            default1 = center+ ofVec3f(0,-breite/2,0);
            //2
            activeMesh.addVertex(center+ ofVec3f(-planeSize_/2,-breite/2,0) );
            //3
            activeMesh.addVertex(center+ ofVec3f(-planeSize_/2,+breite/2,0) );
            //4
            activeMesh.addVertex(center+ ofVec3f(0,+breite/2,0) );
            default4 = center+ ofVec3f(0,+breite/2,0);
            //5
            activeMesh.addVertex(center+ ofVec3f(planeSize_/2,+breite/2,0) );
            
            //set on position for start
            activeMesh.setVertex(1,default1+(activeMeshON));
            activeMesh.setVertex(4, default4+(activeMeshON));
            
            for (int i = 0; i < 12; i++) {
                activeMesh.addIndex(indexT[i]);
            }
            
            for (int i = 0; i < 12 ; i++) {
                activeMesh.addColor(activeOnColor);
            }
            activeDrawColor = activeOnColor;
            active =true;

            
            activeMesh.setMode(OF_PRIMITIVE_TRIANGLES);
            
            fbo_activeMesh.clear();
            fbo_activeMesh = activeMesh;
            fbo_activeMesh.clearColors();
            
            for (int i = 0; i < 12 ; i++) {
                fbo_activeMesh.addColor(ofColor(id_,0,0));
            }
            break;
            
            
    }
}

void InterfacePlane::update() {
    
  //  activeMesh.setVertex(1,default1+(activeMeshON*onPositionPct));
   // activeMesh.setVertex(4, default4+(activeMeshON*onPositionPct));
    if(active) {
        if (activeOnColor != activeDrawColor) {
            activeDrawColor = activeDrawColor.lerp(activeOnColor, 0.15);
            for (int i = 0; i < 6; i++) {
                activeMesh.setColor(i, activeDrawColor);
            }
        }
        
        if(onPositionPct < 1){
            onPositionPct+=pctSpeed;
            // pctTemp = (pow(onPositionPct, 4));
            pctTemp = -(sin(ofMap(onPositionPct, 0.0, 1.0, HALF_PI, HALF_PI+PI))*0.5)+0.5;
            activeMesh.setVertex(1,default1+(activeMeshON * pctTemp ));
            activeMesh.setVertex(4, default4+(activeMeshON * pctTemp ));
            fbo_activeMesh.setVertex(1,default1+(activeMeshON * pctTemp ));
            fbo_activeMesh.setVertex(4, default4+(activeMeshON * pctTemp ));
        }
        
    }
    
    if (!active) {
        if ( activeOffColor != activeDrawColor) {
            activeDrawColor = activeDrawColor.lerp(activeOffColor, 0.15);
            for (int i = 0; i < 6; i++) {
                activeMesh.setColor(i, activeDrawColor);
            }
        }
        
        
        if(onPositionPct > 0){
            onPositionPct-=pctSpeed;
            pctTemp = -(sin(ofMap(onPositionPct, 0.0, 1.0, HALF_PI, HALF_PI+PI))*0.5)+0.5;
            activeMesh.setVertex(1,default1+(activeMeshON * pctTemp ));
            activeMesh.setVertex(4, default4+(activeMeshON * pctTemp ));
            fbo_activeMesh.setVertex(1,default1+(activeMeshON * pctTemp ));
            fbo_activeMesh.setVertex(4, default4+(activeMeshON * pctTemp ));
        }
    }

}

void InterfacePlane::draw() {
    
    activeMesh.draw();
    
}

void InterfacePlane::pulse() {
    activeDrawColor = ofColor::white;
}

void InterfacePlane::drawFbo(){
    fbo_activeMesh.draw();
}