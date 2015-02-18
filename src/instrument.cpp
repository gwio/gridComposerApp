#include "instrument.h"

Instrument::Instrument(){
    
};

Instrument::Instrument(int gTiles_, float gSize_, float border_) {
    
    cubeMap.clear();
    cubes.clear();
    raster.clear();
    verticesOuter.clear();
    verticesInner.clear();
    cubeVector.clear();
    fboMesh.clear();
    
    gridTiles = gTiles_;
    gridSize = gSize_;
    borderSize = border_;
    
    rCounter = 1;
    gCounter = 1;
    bCounter = 1;
    
    soundsCounter = 0;
}

void Instrument::setup() {
    
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
            raster.addVertex(tPoint+ofVec3f(0,0,100));
            raster.addColor(ofColor(255,255,255));
            verticesOuter[indexCounter] = tPoint;
            indexCounter++;
        }
    }
    
    //add outer vertices to mesh
    for (int i = 0; i < verticesOuter.size(); i++) {
        cubes.addVertex(verticesOuter[i]);
        cubes.addColor(ofColor(0,0,0));
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
            cubeVector[i*(gridTiles)+j].zHeight = 10;
            //add pickColor
            ofColor tempColor = ofColor(rCounter,gCounter,bCounter);
            cubeVector[i*(gridTiles)+j].pickColor = tempColor;
            cubeMap[tempColor.getHex()] = ofVec2f(i,j);
            if (rCounter%255==0) {
                rCounter=1;
                gCounter++;
            }
            
            if (gCounter%255==0) {
                gCounter=1;
                bCounter++;
            }
            rCounter++;
        }
    }
    
    //set Inner z
    for (int i = 0; i < verticesInner.size(); i++) {
        verticesInner[i].z = 10;
    }
    
    for (int i = 0; i < verticesInner.size(); i++) {
        cubes.addVertex(verticesInner[i]);
        cubes.addColor(ofColor(255,255,255));
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
    
    rCounter = 1;
    gCounter = 1;
    bCounter = 1;
    
    fboMesh = cubes;
    
    cout << cubes.getNumVertices() << endl;
    
}


void Instrument::update() {
    
    for (int i = 0; i < cubeVector.size(); i++) {
        cubeVector[i].update();
    }
    
    
    updateCubeMesh();
}

void Instrument::draw() {
    ofPushMatrix();
    
    cubes.draw();
    
    ofPopMatrix();
}

void Instrument::drawFbo() {
    ofPushMatrix();
    
    fboMesh.draw();
    
    ofPopMatrix();
}


void Instrument::addCube(int x_, int y_){
    layerInfo.at(x_).at(y_).hasCube = true;
    
    float zH = ofRandom(75);
    cubeVector[layerInfo.at(x_).at(y_).cubeVecNum].zHeight = zH;
    cubeVector[layerInfo.at(x_).at(y_).cubeVecNum].cubeColor = ofColor(ofRandom(255),ofRandom(255),ofRandom(255));
    
    updateSoundsMap(x_, y_);
    
}

void Instrument::removeCube(int x_, int y_){
    layerInfo.at(x_).at(y_).hasCube = false;

    cubeVector[layerInfo.at(x_).at(y_).cubeVecNum].zHeight = 10;

    cubeVector[layerInfo.at(x_).at(y_).cubeVecNum].cubeColor = ofColor::white;
    
}

void Instrument::replaceCube(int x_, int y_, float zH_, ofColor c_) {
    layerInfo.at(x_).at(y_).hasCube = true;

    cubeVector[layerInfo.at(x_).at(y_).cubeVecNum].zHeight = zH_;
    
    //cubeVector[layerInfo.at(x_).at(y_).cubeVecNum].cubeColor = c_;
    
    updateSoundsMap(x_, y_);


}

void Instrument::play(){
    
}

void Instrument::drawDebug() {
    ofPushMatrix();
    ofPushStyle();
    for (int i = 0; i < layerInfo.size(); i++) {
        for (int j = 0; j < layerInfo.at(i).size(); j++) {
            if (layerInfo.at(i).at(j).hasCube) {
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
    ofPopMatrix();
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
        
        cubes.setColor(cubeVector[j].vIndex0, cubeVector[j].cubeColor);
        cubes.setColor(cubeVector[j].vIndex1, cubeVector[j].cubeColor);
        cubes.setColor(cubeVector[j].vIndex2, cubeVector[j].cubeColor);
        cubes.setColor(cubeVector[j].vIndex3, cubeVector[j].cubeColor);
    }
    
}

void Instrument::updateFboMesh(){
    
    for (int j = 0; j < cubeVector.size(); j++) {
        fboMesh.setVertex(cubeVector[j].vIndex0, *cubeVector[j].vec0Ptr);
        fboMesh.setVertex(cubeVector[j].vIndex1, *cubeVector[j].vec1Ptr);
        fboMesh.setVertex(cubeVector[j].vIndex2, *cubeVector[j].vec2Ptr);
        fboMesh.setVertex(cubeVector[j].vIndex3, *cubeVector[j].vec3Ptr);
        
        fboMesh.setColor(cubeVector[j].vIndex0, cubeVector[j].pickColor);
        fboMesh.setColor(cubeVector[j].vIndex1, cubeVector[j].pickColor);
        fboMesh.setColor(cubeVector[j].vIndex2, cubeVector[j].pickColor);
        fboMesh.setColor(cubeVector[j].vIndex3, cubeVector[j].pickColor);
    }
}

void Instrument::updateSoundsMap(int x_, int y_) {
    
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
    
    cout << neighbours.size()<< endl;
    
    //make newsound when alone
    if (neighbours.size() == 0) {
        cubeGroup temp = cubeGroup();
        temp.ownId = soundsCounter;
        temp.size = 1;
        ofColor gColor = ofColor::fromHsb( soundsCounter*10%255, 255, 255);
        temp.groupColor = gColor;
        soundsMap[soundsCounter] = temp;
        layerInfo.at(x_).at(y_).cubeGroupId = soundsCounter;
        cubeVector[layerInfo.at(x_).at(y_).cubeVecNum].cubeColor = gColor;
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
        soundsMap[soundMapIndex].size++;
        layerInfo.at(x_).at(y_).cubeGroupId = soundMapIndex;
        cubeVector[layerInfo.at(x_).at(y_).cubeVecNum].cubeColor = soundsMap[soundMapIndex].groupColor;
        soundsCounter++;
        
        //with different neighbours -> change all neighbours
        if (neighbours.size() > 1) {
            for (int i = 0; i < neighbours.size(); i++) {
                if (neighbours[i] != soundMapIndex) {
                    for (int x = 0; x < gridTiles; x++) {
                        for (int y = 0; y <gridTiles; y++) {
                            if (   layerInfo.at(x).at(y).hasCube && layerInfo.at(x).at(y).cubeGroupId == neighbours[i]) {
                                soundsMap[layerInfo.at(x).at(y).cubeGroupId].size--;
                                layerInfo.at(x).at(y).cubeGroupId = soundMapIndex;
                                cubeVector[layerInfo.at(x).at(y).cubeVecNum].cubeColor = soundsMap[soundMapIndex].groupColor;
                                soundsMap[soundMapIndex].size++;
                            }
                        }
                    }
                }
            }
        }
        
    }
    
    
    //   cout << soundsCounter << endl;
    
    
}

void Instrument::updateSoundsMap() {
    for (int a = 0; a < gridTiles; a++) {
        for (int b = 0; b < gridTiles; b++) {
            if (layerInfo.at(a).at(b).hasCube) {
        
            updateSoundsMap(a, b);
            }
        }
    }
    
    
}