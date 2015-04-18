#include "instrument.h"

//layer is 100x100
#define CUBE_Z_HEIGHT 12.5
#define EMPTY_Z 2.55
#define SCAN_Z 22

#define COLOR_TARGET 55.0

Instrument::Instrument(){
    
};

Instrument::Instrument(string id_,int gTiles_, float gSize_, float border_) {
    
    cubeMap.clear();
    cubes.clear();
    raster.clear();
    verticesOuter.clear();
    verticesInner.clear();
    cubeVector.clear();
    
    gridTiles = gTiles_;
    gridSize = gSize_;
    borderSize = border_;
    
    
   
    
    soundsCounter = 1;
    synthHasChanged = false;
    instrumentId = id_;
    scanDirection = 0;
    layerZ = CUBE_Z_HEIGHT;
    emptyInnerZ = EMPTY_Z;
    scanZ = SCAN_Z;
    
    colorHue = ofRandom(255);
    animate = false;
    inFocus = false;
    scaling = false;
    keyNote = 0;
    preset = 0;
    
    pause = false;
    myScaleTarget = 1.0;
    currentScaleVecPos = 0;
    sVolume = 1.0;
    pitchMod = 0;
    octaveRange = 0;
    
    if ( ofIsStringInString(instrumentId,"b") ) {
        trackSwitchOn = true;
    } else {
        trackSwitchOn = false;
    }
    
    
}

void Instrument::setup(int *stepperPos_, Tonic::ofxTonicSynth *mainTonicPtr_, ofNode node_, float *bpmTick_) {
    
    myNode = node_;
    
    for (int i = 0; i < 4 ; i++) {
        connectedDirection[i] = true;
        activeDirection[i]= true;
    }
    
    stepperPos = stepperPos_;
    mainTonicPtr = mainTonicPtr_;
    bpmTick = bpmTick_;
    
    layerInfo.resize(gridTiles);
    for (int i = 0; i < layerInfo.size(); i++) {
        layerInfo.at(i).resize(gridTiles);
    }
    
    //mesh grid x,y
    verticesOuter.resize( (gridTiles+1)*(gridTiles+1) );
    int indexCounter = 0;
    for (int i = 0; i < gridTiles+1; i++) {
        for (int j = 0; j < gridTiles+1; j++) {
            ofVec3f tPoint = ofVec3f(i*gridSize,j*gridSize,0);
            raster.addVertex(tPoint+ofVec3f(0,0,0));
                raster.addColor(ofColor(255,255,255,190));
           
            verticesOuter[indexCounter] = tPoint;
            indexCounter++;
        }
    }
    
    //add outer vertices to mesh
    for (int i = 0; i < verticesOuter.size(); i++) {
        cubes.addVertex(verticesOuter[i]);
         //cubes.addColor(ofColor(0,0,44));
        cubes.addColor(ofColor(11, 65, 65));
       
    }
    
    
    //inner vertices, clockwise, lowerLeft, upperLeft, upperRight, lowerRight
    verticesInner.resize( (gridTiles)*(gridTiles)*4 );
    float borderRatio = gridSize*borderSize;
    indexCounter = 0;
    cubeVector.resize(gridTiles*gridTiles);
    for (int i = 0; i < gridTiles; i++) {
        for (int j = 0; j < gridTiles; j++) {
            int index = i*(gridTiles+1)+j;
            
            
            ofVec3f temp;
            ofVec3f dir;
            dir = (ofVec3f(1,1,0) - ofVec3f(0,0,0))*borderRatio;
            temp = verticesOuter[index] + dir;
            verticesInner[indexCounter*4] = temp;
            cubeVector[i*(gridTiles)+j].vec0Ptr = &verticesInner[indexCounter*4];
            
            dir = (ofVec3f(1,-1,0) - ofVec3f(0,0,0))*borderRatio;
            temp = verticesOuter[index+1] + dir;
            verticesInner[indexCounter*4+1] = temp;
            cubeVector[i*(gridTiles)+j].vec1Ptr = &verticesInner[indexCounter*4+1];
            
            dir = (ofVec3f(-1,-1,0) - ofVec3f(0,0,0))*borderRatio;
            temp = verticesOuter[index+1+(gridTiles+1)] + dir;
            verticesInner[indexCounter*4+2] = temp;
            cubeVector[i*(gridTiles)+j].vec2Ptr = &verticesInner[indexCounter*4+2];
            
            dir = (ofVec3f(-1,1,0) - ofVec3f(0,0,0))*borderRatio;
            temp = verticesOuter[index+1+gridTiles] + dir;
            verticesInner[indexCounter*4+3] = temp;
            cubeVector[i*(gridTiles)+j].vec3Ptr = &verticesInner[indexCounter*4+3];
            
            indexCounter++;
            //add starting zHeight
            cubeVector[i*(gridTiles)+j].setDefaultHeight(emptyInnerZ);
            
           // if (trackSwitchOn){
                cubeVector[i*(gridTiles)+j].setColor(ofColor::white);
        //    }
            
            cubeVector[i*(gridTiles)+j].attack = &synthAttack;
        }
    }
    
    //set Inner z
    for (int i = 0; i < verticesInner.size(); i++) {
        verticesInner[i].z = emptyInnerZ;
    }
    
    for (int i = 0; i < verticesInner.size(); i++) {
        cubes.addVertex(verticesInner[i]);
        if (trackSwitchOn){
        cubes.addColor(ofColor(0,0,0));
        } else {
            cubes.addColor(ofColor(11, 65, 65));
        }
    }
    
    for (int i = 0; i < layerInfo.size(); i++) {
        for (int j = 0; j < layerInfo.at(i).size(); j++) {
            layerInfo.at(i).at(j).hasCube = false;
            layerInfo.at(i).at(j).cubeVecNum = (i*gridTiles)+j;
        }
    }
    
    //make index
    cubes.clearIndices();
    int startInner = verticesOuter.size();
    for (int i = 0; i < gridTiles; i++) {
        for (int j = 0; j < gridTiles; j++) {
            int indexOuter = i*(gridTiles+1)+j;
            int indexInner = i*(gridTiles)+j;
            //set inner index for cubeVector
            
            cubeVector[indexInner].vIndex0 = (indexInner*4)+startInner;
            cubeVector[indexInner].vIndex1 = (indexInner*4)+startInner+1;
            cubeVector[indexInner].vIndex2= (indexInner*4)+startInner+2;
            cubeVector[indexInner].vIndex3 = (indexInner*4)+startInner+3;
            
            
            int indexex[] = {
                indexOuter,indexOuter+1,(indexInner*4)+startInner+1,
                (indexInner*4)+startInner,indexOuter,(indexInner*4)+startInner+1,
                
                indexOuter+1,indexOuter+1+(gridTiles+1),(indexInner*4)+startInner+2,
                (indexInner*4)+startInner+1,indexOuter+1,(indexInner*4)+startInner+2,
                
                indexOuter+1+(gridTiles+1),indexOuter+1+gridTiles,(indexInner*4)+startInner+3,
                (indexInner*4)+startInner+2,indexOuter+1+(gridTiles+1),(indexInner*4)+startInner+3,
                
                indexOuter+1+gridTiles,indexOuter,(indexInner*4)+startInner,
                (indexInner*4)+startInner+3,indexOuter+1+gridTiles,(indexInner*4)+startInner,
                
                (indexInner*4)+startInner,(indexInner*4)+startInner+1,(indexInner*4)+startInner+2,
                (indexInner*4)+startInner+2,(indexInner*4)+startInner+3,(indexInner*4)+startInner,
                
                indexOuter, indexOuter+1+gridTiles, indexOuter+1+(gridTiles+1),
                indexOuter+1+(gridTiles+1),indexOuter+1,indexOuter
                
            };
            
            for (int i = 0; i < 36; i++) {
                cubes.addIndex(indexex[i]);
            }
            
        }
        
    }
    
    
    cubes.setMode(OF_PRIMITIVE_TRIANGLES);
    raster.setMode(OF_PRIMITIVE_POINTS);
    
    

       
    pulsePlane = InterfacePlane(gridTiles, gridSize);
    
    ofVec3f tranVec = -ofVec3f((gridTiles*gridSize)/2,(gridTiles*gridSize)/2,0);
    
    //add displacement
    
    for (int i = 0; i < raster.getNumVertices(); i++) {
        raster.setVertex(i, raster.getVertex(i)+tranVec);
    }
    
    for (int i = 0; i < cubes.getNumVertices(); i++) {
        cubes.setVertex(i, cubes.getVertex(i)+tranVec);
    }
    

    for (int i = 0; i < verticesInner.size(); i++) {
        verticesInner[i] = verticesInner[i]+tranVec;
    }
    
    setNormals(cubes);
    
    
    //setup main tonic out
    
    Tonic::ControlParameter rampTarget = mainTonicPtr->addParameter("mainVolumeRamp"+instrumentId).max(1.0).min(0.0);
    mainTonicPtr->setParameter("mainVolumeRamp"+instrumentId, 1.0);
    outputRamp = Tonic::RampedValue().value(0.5).length(0.1).target(rampTarget);
    instrumentOut = instrumentOut * outputRamp;
    
    
    //   cout << cubes.getNumVertices() << endl;
    
    
}


void Instrument::update() {
    
    if (trackSwitchOn) {
    for (int i = 0; i < cubeVector.size(); i++) {
        cubeVector[i].update();
    }
        pulsePlane.update(*stepperPos,*bpmTick,scanDirection);
    }
    
  
    
    updateCubeMesh();
}

void Instrument::draw() {
    // ofPushMatrix();
    if (trackSwitchOn) {
    
    cubes.draw();
        pulsePlane.draw(scanDirection);
    } else {
        
        raster.draw();
    }
    // ofPopMatrix();
}





void Instrument::addCube(int x_, int y_){
    layerInfo.at(x_).at(y_).hasCube = true;
    
    float zH = layerZ;
    cubeVector[layerInfo.at(x_).at(y_).cubeVecNum].setDefaultHeight(zH);
    // cubeVector[layerInfo.at(x_).at(y_).cubeVecNum].cubeColor = ofColor(ofRandom(255),ofRandom(255),ofRandom(255));
    //ofColor tempC = ofColor::fromHsb(  colorHue+ofRandom(-10,10) , 230+ofRandom(-20,20), 200);
    // cubeVector[layerInfo.at(x_).at(y_).cubeVecNum].cubeColor = tempC;
    
    
    updateSoundsMap(x_, y_, false);
    
}

void Instrument::removeCube(int x_, int y_){
    layerInfo.at(x_).at(y_).hasCube = false;
    cubeGroup *tPtr = &  soundsMap[layerInfo.at(x_).at(y_).cubeGroupId];
    
    tPtr->size--;
    if (tPtr->size < 1) {
        soundsMap.erase(layerInfo.at(x_).at(y_).cubeGroupId);
        //reallocate all synths to mainout
        updateTonicOut();
    }
    
    layerInfo.at(x_).at(y_).cubeGroupId = 0;
    
    cubeVector[layerInfo.at(x_).at(y_).cubeVecNum].setDefaultHeight(emptyInnerZ);
    
    
    cubeVector[layerInfo.at(x_).at(y_).cubeVecNum].cubeColor = ofColor::white;
    //   cubeVector[layerInfo.at(x_).at(y_).cubeVecNum].displayColor = ofColor::black;
    
    
    
    bool breakTest = false;
    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            //  bool testN = false;
            ofVec2f pos = ofVec2f(x_+x,y_+y);
            if ( ((pos.x >= 0 && pos.x < gridTiles) && (pos.y >= 0 && pos.y < gridTiles)) && !(x==0 && y==0) ) {
                if (layerInfo.at(pos.x).at(pos.y).hasCube) {
                    //bool testN = true;
                    resetCubeGroup( layerInfo.at(pos.x).at(pos.y).cubeGroupId,x_,y_);
                    breakTest = true;
                    break;
                }
            }
        }
        if (breakTest) {
            break;
        }
    }
    
}

void Instrument::replaceCube(int x_, int y_, float zH_, ofColor c_) {
    
    bool hasOld;
    if (layerInfo.at(x_).at(y_).hasCube) {
        hasOld = true;
    } else {
        hasOld = false;
        
    }
    layerInfo.at(x_).at(y_).hasCube = true;
    
    cubeVector[layerInfo.at(x_).at(y_).cubeVecNum].setDefaultHeight(zH_);
    
    updateSoundsMap(x_, y_, hasOld);
    
    
}

void Instrument::noteTrigger() {
    
    
    // cout << scanDirection << endl;
    switch (scanDirection)
    {
            
        case -1:
            break;
            
        case 0:
            if(connectedDirection[0]){
                noteTriggerWest();
                break;
            } else {
                break;
            }
        case 1:
            if(connectedDirection[1]){
                noteTriggerNorth();
                break;
            } else {
                break;
            }
        case 2:
            if(connectedDirection[2]){
                noteTriggerEast();
                break;
            } else {
                break;
            }
        case 3:
            if(connectedDirection[3]){
                noteTriggerSouth();
                break;
            } else {
                break;
            }
            
    }
    
}

void Instrument::nextDirection() {
    bool test = false;
    int counter = 0;
    
    if (!pause) {
        
        while (   test == false   &&  counter !=4 ) {
            
            if (activeDirection[(scanDirection+1)%4]) {
                test = true;
                scanDirection = (scanDirection+1)%4;
            } else {
                scanDirection++;
                counter++;
                if(counter==4){
                    scanDirection = -1;
                }
            }
            
        }
        
    } else {
        scanDirection = -1;
    }
    
    //  cout << scanDirection << endl;
}

void Instrument::noteTriggerWest(){
    
    //scanline
    if (*stepperPos == 0) {
        for (int  i = 0; i < gridTiles; i++) {
            if (layerInfo.at(0).at(i).hasCube) {
                cubeVector[layerInfo.at(0).at(i).cubeVecNum].setDefaultHeight(scanZ);
                cubeVector[layerInfo.at(0).at(i).cubeVecNum].setColor(   cubeVector[layerInfo.at(0).at(i).cubeVecNum].scanColor);
                soundsMap[layerInfo.at(0).at(i).cubeGroupId].groupSynth.setParameter("trigger",1);
            }
        }
    } else if (*stepperPos == gridTiles ) {
        for (int  i = 0; i < gridTiles; i++) {
            if (layerInfo.at(gridTiles-1).at(i).hasCube) {
                cubeVector[layerInfo.at(gridTiles-1).at(i).cubeVecNum].setDefaultHeight(CUBE_Z_HEIGHT);
                cubeVector[layerInfo.at(gridTiles-1).at(i).cubeVecNum].setColor(   cubeVector[layerInfo.at(gridTiles-1).at(i).cubeVecNum].groupColor);
            }
        }
    } else if (*stepperPos > 0 && *stepperPos < gridTiles) {
        for (int  i = 0; i < gridTiles; i++) {
            if (layerInfo.at(*stepperPos).at(i).hasCube) {
                cubeVector[layerInfo.at(*stepperPos).at(i).cubeVecNum].setDefaultHeight(scanZ);
                cubeVector[layerInfo.at(*stepperPos).at(i).cubeVecNum].setColor(   cubeVector[layerInfo.at(*stepperPos).at(i).cubeVecNum].scanColor);
                soundsMap[layerInfo.at(*stepperPos).at(i).cubeGroupId].groupSynth.setParameter("trigger",1);
              

            }
            if (layerInfo.at(*stepperPos-1).at(i).hasCube) {
                cubeVector[layerInfo.at(*stepperPos-1).at(i).cubeVecNum].setDefaultHeight(CUBE_Z_HEIGHT);
                cubeVector[layerInfo.at(*stepperPos-1).at(i).cubeVecNum].setColor(   cubeVector[layerInfo.at(*stepperPos-1).at(i).cubeVecNum].groupColor);
                
            }
        }
        
    }
    
    for (map<unsigned long,cubeGroup>::iterator it=soundsMap.begin(); it!=soundsMap.end(); ++it){
        if (*stepperPos >= it->second.lowX && *stepperPos <= it->second.highX+1)  {
            
            if (it->second.highX+1 == *stepperPos){
                it->second.groupSynth.setParameter("rampVolumeTarget", 0.0);
            } else {
                float rampTarget = float( it->second.y_in_x_elements[*stepperPos]) / float(gridTiles) ;
                it->second.groupSynth.setParameter("rampVolumeTarget", rampTarget);
                
            }
        }
    }
}

void Instrument::noteTriggerNorth() {
    
    
    //scanline
    if (*stepperPos == 0) {
        for (int  i = 0; i < gridTiles; i++) {
            if (layerInfo.at(i).at(gridTiles-1).hasCube) {
                cubeVector[layerInfo.at(i).at(gridTiles-1).cubeVecNum].setDefaultHeight(scanZ);
                cubeVector[layerInfo.at(i).at(gridTiles-1).cubeVecNum].setColor(   cubeVector[layerInfo.at(i).at(gridTiles-1).cubeVecNum].scanColor);
                soundsMap[layerInfo.at(i).at(gridTiles-1).cubeGroupId].groupSynth.setParameter("trigger",1);

            }
        }
    } else if (*stepperPos == gridTiles ) {
        for (int  i = 0; i < gridTiles; i++) {
            if (layerInfo.at(i).at(0).hasCube) {
                cubeVector[layerInfo.at(i).at(0).cubeVecNum].setDefaultHeight(CUBE_Z_HEIGHT);
                cubeVector[layerInfo.at(i).at(0).cubeVecNum].setColor(   cubeVector[layerInfo.at(i).at(0).cubeVecNum].groupColor);
            }
        }
    } else  if (*stepperPos > 0 && *stepperPos < gridTiles) {
        for (int  i = 0; i < gridTiles; i++) {
            if (layerInfo.at(i).at(gridTiles-*stepperPos-1).hasCube) {
                cubeVector[layerInfo.at(i).at(gridTiles-*stepperPos-1).cubeVecNum].setDefaultHeight(scanZ);
                cubeVector[layerInfo.at(i).at(gridTiles-*stepperPos-1).cubeVecNum].setColor(   cubeVector[layerInfo.at(i).at(gridTiles-*stepperPos-1).cubeVecNum].scanColor);
                soundsMap[layerInfo.at(i).at(gridTiles-*stepperPos-1).cubeGroupId].groupSynth.setParameter("trigger",1);

            }
            if (layerInfo.at(i).at(gridTiles-*stepperPos).hasCube) {
                cubeVector[layerInfo.at(i).at(gridTiles-*stepperPos).cubeVecNum].setDefaultHeight(CUBE_Z_HEIGHT);
                cubeVector[layerInfo.at(i).at(gridTiles-*stepperPos).cubeVecNum].setColor(   cubeVector[layerInfo.at(i).at(gridTiles-*stepperPos).cubeVecNum].groupColor);
            }
        }
    }
    
    
    for (map<unsigned long,cubeGroup>::iterator it=soundsMap.begin(); it!=soundsMap.end(); ++it){
        if (gridTiles-*stepperPos <= it->second.highY+1 && gridTiles-*stepperPos >= it->second.lowY) {
            
            if (it->second.lowY == gridTiles-*stepperPos){
                it->second.groupSynth.setParameter("rampVolumeTarget", 0.0);
            } else {
                float rampTarget = float( it->second.x_in_y_elements[gridTiles-*stepperPos-1]) / float(gridTiles) ;
                it->second.groupSynth.setParameter("rampVolumeTarget", rampTarget);
            }
        }
    }
    
}

void Instrument::noteTriggerEast() {
    
    
    //scanline
    if (*stepperPos == 0) {
        for (int  i = 0; i < gridTiles; i++) {
            if (layerInfo.at(gridTiles-1).at(i).hasCube) {
                cubeVector[layerInfo.at(gridTiles-1).at(i).cubeVecNum].setDefaultHeight(scanZ);
                cubeVector[layerInfo.at(gridTiles-1).at(i).cubeVecNum].setColor(   cubeVector[layerInfo.at(gridTiles-1).at(i).cubeVecNum].scanColor);
                soundsMap[layerInfo.at(gridTiles-1).at(i).cubeGroupId].groupSynth.setParameter("trigger",1);
            }
        }
    } else if (*stepperPos == gridTiles ) {
        for (int  i = 0; i < gridTiles; i++) {
            if (layerInfo.at(0).at(i).hasCube) {
                cubeVector[layerInfo.at(0).at(i).cubeVecNum].setDefaultHeight(CUBE_Z_HEIGHT);
                cubeVector[layerInfo.at(0).at(i).cubeVecNum].setColor(   cubeVector[layerInfo.at(0).at(i).cubeVecNum].groupColor);
            }
        }
    } else if (*stepperPos > 0 && *stepperPos < gridTiles) {
        for (int  i = 0; i < gridTiles; i++) {
            if (layerInfo.at(gridTiles-*stepperPos-1).at(i).hasCube) {
                cubeVector[layerInfo.at(gridTiles-*stepperPos-1).at(i).cubeVecNum].setDefaultHeight(scanZ);
                cubeVector[layerInfo.at(gridTiles-*stepperPos-1).at(i).cubeVecNum].setColor(   cubeVector[layerInfo.at(gridTiles-*stepperPos-1).at(i).cubeVecNum].scanColor);
                soundsMap[layerInfo.at(gridTiles-*stepperPos-1).at(i).cubeGroupId].groupSynth.setParameter("trigger",1);

            }
            if (layerInfo.at(gridTiles-*stepperPos).at(i).hasCube) {
                cubeVector[layerInfo.at(gridTiles-*stepperPos).at(i).cubeVecNum].setDefaultHeight(CUBE_Z_HEIGHT);
                cubeVector[layerInfo.at(gridTiles-*stepperPos).at(i).cubeVecNum].setColor(   cubeVector[layerInfo.at(gridTiles-*stepperPos).at(i).cubeVecNum].groupColor);
                
            }
        }
    }
    
    for (map<unsigned long,cubeGroup>::iterator it=soundsMap.begin(); it!=soundsMap.end(); ++it){
        if (gridTiles-*stepperPos <= it->second.highX+1 && gridTiles-*stepperPos >= it->second.lowX) {
            
            if (it->second.lowX == gridTiles-*stepperPos){
                it->second.groupSynth.setParameter("rampVolumeTarget", 0.0);
            } else {
                float rampTarget = float( it->second.y_in_x_elements[gridTiles-*stepperPos-1]) / float(gridTiles) ;
                it->second.groupSynth.setParameter("rampVolumeTarget", rampTarget);
            }
        }
    }
}

void Instrument::noteTriggerSouth() {
    //scanline
    if (*stepperPos == 0) {
        for (int  i = 0; i < gridTiles; i++) {
            if (layerInfo.at(i).at(0).hasCube) {
                cubeVector[layerInfo.at(i).at(0).cubeVecNum].setDefaultHeight(scanZ);
                cubeVector[layerInfo.at(i).at(0).cubeVecNum].setColor(   cubeVector[layerInfo.at(i).at(0).cubeVecNum].scanColor);
                soundsMap[layerInfo.at(i).at(0).cubeGroupId].groupSynth.setParameter("trigger",1);
            }
        }
    } else if (*stepperPos == gridTiles ) {
        for (int  i = 0; i < gridTiles; i++) {
            if (layerInfo.at(i).at(gridTiles-1).hasCube) {
                cubeVector[layerInfo.at(i).at(gridTiles-1).cubeVecNum].setDefaultHeight(CUBE_Z_HEIGHT);
                cubeVector[layerInfo.at(i).at(gridTiles-1).cubeVecNum].setColor(   cubeVector[layerInfo.at(i).at(gridTiles-1).cubeVecNum].groupColor);
            }
        }
    } else if (*stepperPos > 0 && *stepperPos < gridTiles) {
        for (int  i = 0; i < gridTiles; i++) {
            if (layerInfo.at(i).at(*stepperPos).hasCube) {
                cubeVector[layerInfo.at(i).at(*stepperPos).cubeVecNum].setDefaultHeight(scanZ);
                cubeVector[layerInfo.at(i).at(*stepperPos).cubeVecNum].setColor(   cubeVector[layerInfo.at(i).at(*stepperPos).cubeVecNum].scanColor);
                soundsMap[layerInfo.at(i).at(*stepperPos).cubeGroupId].groupSynth.setParameter("trigger",1);
            }
            if (layerInfo.at(i).at(*stepperPos-1).hasCube) {
                cubeVector[layerInfo.at(i).at(*stepperPos-1).cubeVecNum].setDefaultHeight(CUBE_Z_HEIGHT);
                cubeVector[layerInfo.at(i).at(*stepperPos-1).cubeVecNum].setColor(   cubeVector[layerInfo.at(i).at(*stepperPos-1).cubeVecNum].groupColor);
            }
        }
        
    }
    
    
    for (map<unsigned long,cubeGroup>::iterator it=soundsMap.begin(); it!=soundsMap.end(); ++it){
        if (*stepperPos >= it->second.lowY && *stepperPos <= it->second.highY+1) {
            if (it->second.highY+1 == *stepperPos){
                it->second.groupSynth.setParameter("rampVolumeTarget", 0.0);
            } else {
                float rampTarget = float( it->second.x_in_y_elements[*stepperPos]) / float(gridTiles) ;
                it->second.groupSynth.setParameter("rampVolumeTarget", rampTarget);
            }
        }
    }
    
}




void Instrument::drawDebug() {
    ofPushStyle();
    for (int i = 0; i < layerInfo.size(); i++) {
        for (int j = 0; j < layerInfo.at(i).size(); j++) {
            
            ofSetColor(255);
            
            if (layerInfo.at(i).at(j).hasCube) {
                ofDrawBitmapString(ofToString( soundsMap[ layerInfo.at(i).at(j).cubeGroupId ].size), cubeVector[layerInfo.at(i).at(j).cubeVecNum].vec0Ptr->x, cubeVector[layerInfo.at(i).at(j).cubeVecNum].vec0Ptr->y);
                
                //ofRect(i*gridSize, j*gridSize, 10, gridSize, gridSize);
                if (layerInfo.at(i).at(j).blocked) {
                    ofSetColor(255, 0, 0);
                } else {
                    ofSetColor(soundsMap[layerInfo.at(i).at(j).cubeGroupId].groupColor);
                }
                ofRect(cubeVector[layerInfo.at(i).at(j).cubeVecNum].vec0Ptr->x, cubeVector[layerInfo.at(i).at(j).cubeVecNum].vec0Ptr->y, 100, gridSize*0.7, gridSize*0.7);
                
            }
        }
    }
    
    ofPopStyle();
}

void Instrument::tapEvent(int x_,int y_) {
    
    if (layerInfo.at(x_).at(y_).hasCube) {
        removeCube(x_, y_);
    } else {
        addCube(x_, y_);
    }
}

void Instrument::moveEvent(int x_, int y_, float zH_, ofColor c_) {
    
    replaceCube(x_, y_,zH_,c_);
}

void Instrument::updateCubeMesh(){
    
    for (int j = 0; j < cubeVector.size(); j++) {
        cubes.setVertex(cubeVector[j].vIndex0, *cubeVector[j].vec0Ptr);
        cubes.setVertex(cubeVector[j].vIndex1, *cubeVector[j].vec1Ptr);
        cubes.setVertex(cubeVector[j].vIndex2, *cubeVector[j].vec2Ptr);
        cubes.setVertex(cubeVector[j].vIndex3, *cubeVector[j].vec3Ptr);
        
        cubes.setColor(cubeVector[j].vIndex0, cubeVector[j].displayColor);
        cubes.setColor(cubeVector[j].vIndex1, cubeVector[j].displayColor);
        cubes.setColor(cubeVector[j].vIndex2, cubeVector[j].displayColor);
        cubes.setColor(cubeVector[j].vIndex3, cubeVector[j].displayColor);
    }
    
}



void Instrument::updateSoundsMap(int x_, int y_, bool replace_) {
    
    //test neighbouring cubes 3x3
    int tester[9];
    int testerStart= 0;
    int cCounter = 0;
    vector<unsigned long> neighbours;
    neighbours.clear();
    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            
            ofVec2f pos = ofVec2f(x_+x,y_+y);
            if ( ((pos.x >= 0 && pos.x < gridTiles) && (pos.y >= 0 && pos.y < gridTiles)) && !(x==0 && y==0) ) {
                if (layerInfo.at(pos.x).at(pos.y).hasCube) {
                    bool testN = false;
                    for (int i = 0; i < neighbours.size(); i++) {
                        if (neighbours[i] == layerInfo.at(pos.x).at(pos.y).cubeGroupId) {
                            testN = true;
                            break;
                        }
                        
                    }
                    if (testN == false) {
                        neighbours.push_back(layerInfo.at(pos.x).at(pos.y).cubeGroupId);
                    }
                }
                
            }
        }
        
    }
    
    
    //make newsound when alone
    if (neighbours.size() == 0) {
        cubeGroup temp = cubeGroup(gridTiles);
        temp.ownId = soundsCounter;
        temp.size = 1;
        ofColor gColor = ofColor::fromHsb(   ofWrap(colorHue+ofRandom(-12,12),0,255), 150+ofRandom(-50,50), 120+ofRandom(-40,40));;
        temp.groupColor = gColor;
        temp.lowX = x_;
        temp.highX = x_;
        temp.lowY = y_;
        temp.highY = y_;
        temp.y_in_x_elements.at(x_) = 1;
        temp.x_in_y_elements.at(y_) = 1;
        
        
        //set tonic synth
        setupOneSynth(&temp);
        
        soundsMap[soundsCounter] = temp;
        
        //reallocate all synths to mainout
        updateTonicOut();
        
        layerInfo.at(x_).at(y_).cubeGroupId = soundsCounter;
        
        cubeVector[layerInfo.at(x_).at(y_).cubeVecNum].changeGroupColor(gColor);
        
        soundsCounter++;
        //add to neightbours, here to the biggest neighbouring group, or random
    } else {
        int biggestN = 0;
        unsigned long soundMapIndex = 0;
        for (int i = 0; i < neighbours.size(); i++) {
            if (soundsMap[neighbours[i]].size > biggestN) {
                biggestN = soundsMap[neighbours[i]].size;
                soundMapIndex = neighbours[i];
            }
        }
        cubeGroup *tempPtr = &soundsMap[soundMapIndex];
        if (!replace_) {
            tempPtr->size++;
            //check for max,min, x,y
            layerInfo.at(x_).at(y_).cubeGroupId = soundMapIndex;
            
            cubeVector[layerInfo.at(x_).at(y_).cubeVecNum].changeGroupColor(tempPtr->groupColor);
            
            
            updateGroupInfo(soundMapIndex, x_, y_);
        } else {
            layerInfo.at(x_).at(y_).cubeGroupId = soundMapIndex;
            
            cubeVector[layerInfo.at(x_).at(y_).cubeVecNum].changeGroupColor(tempPtr->groupColor);
            
        }
        
        //with different neighbours -> change all neighbours
        if (neighbours.size() > 1) {
            for (int i = 0; i < neighbours.size(); i++) {
                if (neighbours[i] != soundMapIndex) {
                    for (int x = 0; x < gridTiles; x++) {
                        for (int y = 0; y <gridTiles; y++) {
                            if (   layerInfo.at(x).at(y).hasCube && layerInfo.at(x).at(y).cubeGroupId == neighbours[i]) {
                                cubeGroup *aPtr = &soundsMap[layerInfo.at(x).at(y).cubeGroupId];
                                aPtr->size--;
                                
                                layerInfo.at(x).at(y).cubeGroupId = soundMapIndex;
                                
                                cubeVector[layerInfo.at(x).at(y).cubeVecNum].changeGroupColor(tempPtr->groupColor);
                                
                                
                                tempPtr->size++;
                                //check for max,min, x,y
                                updateGroupInfo(soundMapIndex, x, y);
                                if (aPtr->size < 1) {
                                    soundsMap.erase(neighbours[i]);
                                    updateTonicOut();
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
        
    }
    
}

void Instrument::resetCubeGroup(unsigned long group_, int originX, int originY) {
    
    int minusCouter = 0;
    cubeGroup *cgPtr = &soundsMap[group_];
    vector<ofVec2f> tempPosis;
    bool breakTest = false;
    for (int x = 0; x < gridTiles; x++) {
        for (int y = 0; y <gridTiles; y++) {
            
            if (layerInfo.at(x).at(y).cubeGroupId == group_) {
                layerInfo.at(x).at(y).hasCube = false;
                layerInfo.at(x).at(y).cubeGroupId = 0;
                cgPtr->size--;
                tempPosis.push_back(ofVec2f(x,y));
                
                
                if (cgPtr->size < 1) {
                    soundsMap.erase(group_);
                    //reallocate all synths to mainout
                    updateTonicOut();
                    breakTest = true;
                    break;
                }
            }
            
        }
        if (breakTest) {
            break;
        }
    }
    
    for (int i = 0; i < tempPosis.size(); i++) {
        addCube(tempPosis[i].x, tempPosis[i].y);
    }
    
    
}

void Instrument::updateGroupInfo(unsigned long key_, int x_, int y_) {
    
    //get noteOn noteOff infos for cubeGroup
    
    cubeGroup *groupPtr = &soundsMap[key_];
    
    int lx = groupPtr->lowX;
    int ly = gridTiles+1;
    int hx = -1;
    int hy = -1;
    
    if(x_ >  groupPtr->highX){
        groupPtr->highX = x_;
    }
    if(y_ > groupPtr->highY){
        groupPtr->highY = y_;
    }
    if(x_ < groupPtr->lowX){
        groupPtr->lowX= x_;
    }
    if(y_ < groupPtr->lowY){
        groupPtr->lowY = y_;
    }
    
    int yInxEleCounter = 0;
    for (int i = 0; i < gridTiles; i++) {
        if (  layerInfo.at(x_).at(i).hasCube && layerInfo.at(x_).at(i).cubeGroupId == key_ ){
            yInxEleCounter++;
        }
    }
    
    groupPtr->y_in_x_elements.at(x_) = yInxEleCounter;
    
    
    int xInyEleCounter = 0;
    for (int i = 0; i < gridTiles; i++) {
        if (  layerInfo.at(i).at(y_).hasCube && layerInfo.at(i).at(y_).cubeGroupId == key_ ){
            xInyEleCounter++;
        }
    }
    
    groupPtr->x_in_y_elements.at(y_) = xInyEleCounter;
    
    
}

void Instrument::setupOneSynth(cubeGroup *cgPtr) {
    
    
    float rampLength = 0.22;
    float freqRamp = 0.16;
    
    //create volume ramp
    Tonic::ControlParameter rampVolumeTarget = cgPtr->groupSynth.addParameter("rampVolumeTarget");
    cgPtr->groupSynth.setParameter("rampVolumeTarget",0.0);
    cgPtr->rampVol = Tonic::RampedValue().value(0.0).length(rampLength).target(rampVolumeTarget);
    
    //create freq ramp
    Tonic::ControlParameter rampFreqTarget = cgPtr->groupSynth.addParameter("rampFreqTarget");
    cgPtr->freqRamp = Tonic::RampedValue(0.0).value( 0 ).length(freqRamp).target(rampFreqTarget);
    cgPtr->groupNote = getRandomNote();
    cgPtr->groupSynth.setParameter("rampFreqTarget", Tonic::mtof(cgPtr->groupNote ));
    
    cgPtr->trigger = cgPtr->groupSynth.addParameter("trigger");
    
    presetManager.createSynth(preset%presetManager.count, cgPtr->groupSynth, cgPtr->output, cgPtr->freqRamp, cgPtr->rampVol, cgPtr->trigger);
    synthAttack = presetManager.attack;

    
}

void Instrument::changePreset() {
    
    preset++;
    
    for (map<unsigned long,cubeGroup>::iterator it=soundsMap.begin(); it!=soundsMap.end(); ++it){
        if(it->second.size > 0){
            presetManager.createSynth(preset%presetManager.count, it->second.groupSynth, it->second.output, it->second.freqRamp, it->second.rampVol, it->second.trigger);
        }
    }
    synthAttack = presetManager.attack;
    
    updateTonicOut();
}


void Instrument::updateTonicOut(){
    Tonic::Generator temp;
    
    for (map<unsigned long,cubeGroup>::iterator it=soundsMap.begin(); it!=soundsMap.end(); ++it){
        if(it->second.size > 0){
            temp = temp + it->second.output;
        }
    }
    
    
    Tonic::StereoDelay delay = Tonic::StereoDelay(0.6f,0.6f)
    .delayTimeLeft( 0.2 )
    .delayTimeRight(0.2)
    .feedback(0.3)
    .dryLevel(0.90)
    .wetLevel(0.10);
    
    Tonic::Reverb reverb = Tonic::Reverb().inputHPFCutoff(2000).decayTime(0.2).roomSize(0.5).wetLevel(-2);
    
    instrumentOut = (temp >> delay ) * outputRamp;
    synthHasChanged = true;
}


void Instrument::setTranslate(ofVec3f trans_) {
    myNode.setPosition(trans_);
}

void Instrument::setRotate(ofQuaternion rot_){
    myNode.setOrientation(rot_);
}

void Instrument::setScale(float scale_){
    myNode.setScale(scale_);
}


void Instrument::changeSynthVolume(float & vol_) {
    sVolume = vol_;
    mainTonicPtr->setParameter("mainVolumeRamp"+instrumentId, vol_);
    //  scanZ =  (30*vol_);
}



void Instrument::changeMusicScale(int noteNum_){
    activeScale.steps[noteNum_] = !activeScale.steps[noteNum_];
    
    scaleNoteSteps.clear();
    for (int i = 0; i < 12; i++) {
        if (activeScale.steps[i]) {
            scaleNoteSteps.push_back(i);
        }
    }
    
    applyNewScale();
}


void Instrument::applyNewScale(){
    
    for (map<unsigned long,cubeGroup>::iterator it=soundsMap.begin(); it!=soundsMap.end(); ++it){
        if(it->second.size > 0){
            
            it->second.groupNote = getRandomNote();
            it->second.groupSynth.setParameter("rampFreqTarget", Tonic::mtof(it->second.groupNote ));
        }
    }
}


void Instrument::setMusicScale(GlobalScales& scale_,int num_){
    scaleNoteSteps.clear();
    scaleNoteSteps = scale_.scaleNotes.at(num_);
    activeScale = scale_.scaleVec.at(num_);
    
    applyNewScale();
}

void Instrument::setKeyNote(int keyNote_) {
    
    
    int change = keyNote_;
    keyNote += change;
    
    for (map<unsigned long,cubeGroup>::iterator it=soundsMap.begin(); it!=soundsMap.end(); ++it){
        if(it->second.size > 0){
            it->second.groupNote+=keyNote_;
            it->second.groupSynth.setParameter("rampFreqTarget", Tonic::mtof(it->second.groupNote ));
        }
    }
    
}

void Instrument::applyPitchMod(float mod_) {
    
    pitchMod = mod_;
    for (map<unsigned long,cubeGroup>::iterator it=soundsMap.begin(); it!=soundsMap.end(); ++it){
        if(it->second.size > 0){
            
            it->second.groupSynth.setParameter("rampFreqTarget", Tonic::mtof(it->second.groupNote )+pitchMod);
        }
    }
}

void Instrument::planeMovement(float pct_){
    
    
    if (animate && pct_==1.0) {
        animate = false;
        setTranslate( aniPath.getVertices().at(aniPath.size()-1));
        
        ofQuaternion tempRot;
        tempRot.slerp(pct_, myDefault,myTarget);
        setRotate( tempRot );
        
                
    } else if (animate) {
        if (aniPath.size() > 1) {
            float index = aniPath.getIndexAtPercent(pct_);
            ofVec3f tempPos =  (aniPath.getVertices().at((int)index+1)-aniPath.getVertices().at((int)index))* (index-floor(index));
            setTranslate( aniPath.getVertices().at((int)index)+ tempPos);
        }
        ofQuaternion tempRot;
        tempRot.slerp(pct_, myDefault,myTarget);
        setRotate( tempRot );
        
    }
    
    
    if(scaling ) {
        
        setScale( ofLerp(myScaleDefault, myScaleTarget, pct_));
        
        if (scaling && pct_== 1.0) {
            scaling = false;
        }
    }
    
    
    
    
}

int Instrument::getRandomNote(){
    int note;
    if ( ofRandom(100) > 80) {
        note = scaleNoteSteps[ 0 ] + keyNote;
    } else {
        int ranPos = ofRandom(scaleNoteSteps.size());
        //cout << ranPos << endl;
        note = scaleNoteSteps[ ranPos ] + keyNote;
    }
    return note;
}

ofPolyline Instrument::getEmptyPath(ofVec3f pos_) {
    
    ofPolyline temp;
    temp.addVertex(pos_);
    return temp;
}

void Instrument::setNormals(ofVboMesh& mesh_) {
    
    mesh_.clearNormals();
    
    for (int i = 1; i< mesh_.getNumVertices(); i+=3) {
        
        ofVec3f temp = mesh_.getVertex(i);
        ofVec3f tempA = temp - mesh_.getVertex(i-1) ;
        ofVec3f tempB = temp - mesh_.getVertex(i+1);
        
        temp = tempA.getCrossed(tempB);
        
        temp.normalize();
        
        mesh_.addNormal(temp);
        mesh_.addNormal(temp);
        mesh_.addNormal(temp);
        
        
    }
    

}
