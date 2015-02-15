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
    
    gridTiles = gTiles_;
    gridSize = gSize_;
    borderSize = border_;
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
            raster.addVertex(tPoint);
            raster.addColor(ofColor(255,255,255));
            verticesOuter[indexCounter] = tPoint;
            indexCounter++;
        }
    }
    
    //add outer vertices to mesh
    for (int i = 0; i < verticesOuter.size(); i++) {
        cubes.addVertex(verticesOuter[i]);
        cubes.addColor(ofColor(255,255,255));
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
        }
    }
    
    //set Inner z
    for (int i = 0; i < verticesInner.size(); i++) {
        verticesInner[i].z = 10;
    }
    
    for (int i = 0; i < verticesInner.size(); i++) {
        cubes.addVertex(verticesInner[i]);
        cubes.addColor(ofColor(255,0,255));
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
                (indexInner*4)+startInner+1,(indexInner*4)+startInner,indexOuter,
                
                indexOuter+1,indexOuter+1+(gridTiles+1),(indexInner*4)+startInner+2,
                (indexInner*4)+startInner+2,(indexInner*4)+startInner+1,indexOuter+1,
                
                indexOuter+1+(gridTiles+1),indexOuter+1+gridTiles,(indexInner*4)+startInner+3,
                (indexInner*4)+startInner+3,(indexInner*4)+startInner+2,indexOuter+1+(gridTiles+1),
                
                indexOuter+1+gridTiles,indexOuter,(indexInner*4)+startInner,
                (indexInner*4)+startInner,(indexInner*4)+startInner+3,indexOuter+1+gridTiles,
                
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
    
    
    cout << cubes.getNumVertices() << endl;

}


void Instrument::update() {
    
    for (int i = 0; i < cubeVector.size(); i++) {
        cubeVector[i].update();
    }
    
    
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

void Instrument::draw() {
    ofPushMatrix();
    
    cubes.draw();
  
    ofPopMatrix();
}

void Instrument::drawFbo() {
   
}


void Instrument::addCube(int x_, int y_){
    layerInfo.at(x_).at(y_).hasCube = true;
    ofColor pickColor = ofColor(rCounter,gCounter,bCounter);
    
    float zH = ofRandom(75);
    cubeVector[layerInfo.at(x_).at(y_).cubeVecNum].vec0Ptr->z = zH;
    cubeVector[layerInfo.at(x_).at(y_).cubeVecNum].vec1Ptr->z = zH;
    cubeVector[layerInfo.at(x_).at(y_).cubeVecNum].vec2Ptr->z = zH;
    cubeVector[layerInfo.at(x_).at(y_).cubeVecNum].vec3Ptr->z = zH;

    cubeVector[layerInfo.at(x_).at(y_).cubeVecNum].cubeColor = ofColor::mediumVioletRed;
    
   // cubeMap[pickColor.getHex()] = temp;
    // layerInfo.at(x_).at(y_).cubePtr = &cubes[cubes.size()-1];
    //increase pickingcolor
    
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

void Instrument::removeCube(int x_, int y_){
    layerInfo.at(x_).at(y_).hasCube = false;
    cubeVector[layerInfo.at(x_).at(y_).cubeVecNum].vec0Ptr->z = 10;
    cubeVector[layerInfo.at(x_).at(y_).cubeVecNum].vec1Ptr->z = 10;
    cubeVector[layerInfo.at(x_).at(y_).cubeVecNum].vec2Ptr->z = 10;
    cubeVector[layerInfo.at(x_).at(y_).cubeVecNum].vec3Ptr->z = 10;

    cubeVector[layerInfo.at(x_).at(y_).cubeVecNum].cubeColor = ofColor::white;

}

void Instrument::play(){
    
}

void Instrument::drawDebug() {
    for (int i = 0; i < layerInfo.size(); i++) {
        for (int j = 0; j < layerInfo.at(i).size(); j++) {
            if (layerInfo.at(i).at(j).hasCube) {
                //ofRect(i*gridSize, j*gridSize, 10, gridSize, gridSize);
                ofRect(*cubeVector[layerInfo.at(i).at(j).cubeVecNum].vec0Ptr, gridSize*0.7, gridSize*0.7);
            }
        }
    }
    
}

void Instrument::clickEvent(int x_,int y_) {
    
    if (layerInfo.at(x_).at(y_).hasCube) {
        removeCube(x_, y_);
    } else {
        addCube(x_, y_);
    }
}