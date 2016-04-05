
#include "MusterContainer.h"

#define DISPLAY_NUMX 3
#define DISPLAY_NUMY 3
#define FLIP_SIZE_FAC 0.8
#define FLIP_MAX 3*3

MusterContainer::MusterContainer() {
    
}

MusterContainer::MusterContainer(ofVec3f center_, ofVec2f designGrid_,int tiles_) {
    
    gridTiles = tiles_;
    designGrid.x = (designGrid_.x)/DISPLAY_NUMX;
    designGrid.y = (designGrid_.y)/DISPLAY_NUMY;
    //designGrid*=0.8;
    
    flipSize = designGrid.y*FLIP_SIZE_FAC;
     offsetX = ((designGrid_.x)-(flipSize*3))/2;
     offsetY = ((designGrid_.y)-(flipSize*3))/2;
    
    displayGrid.clear();
    displayGrid.resize(DISPLAY_NUMX*DISPLAY_NUMY);
    
    for (int x = 0; x < DISPLAY_NUMX; x++) {
        for (int y = 0; y < DISPLAY_NUMY; y++) {
            int index = x+(y*DISPLAY_NUMX);
            displayGrid.at(index).x =  (x*flipSize)+(offsetX*x);
          //  displayGrid.at(index).y = (y*designGrid.x)+( (designGrid.x-flipSize)/2);

            displayGrid.at(index).y =  (y*flipSize)+(offsetY*y);
        }
    }
    
    flips.clear();
    flips.resize(FLIP_MAX);
    
    flipsBackground.clear();
    flipsBackground.resize(FLIP_MAX);
  
    
    saveReady = false;
}

void MusterContainer::setup() {
    //uajjjai, 9presets
    
    
    bool tempA[5][5] = {
        {1,1,1,1,1},
        {1,0,1,0,1},
        {1,0,1,0,1},
        {1,0,1,0,1},
        {1,1,1,1,1}
    };
    
    bool *tempB[gridTiles];
    for (int i = 0; i < gridTiles; i++) {
        tempB[i] = tempA[i];
    }
    
    for (int i = 0; i < FLIP_MAX; i++) {
        
        flips.at(i).setup(designGrid.y*FLIP_SIZE_FAC, gridTiles);
        flips.at(i).loadData(tempB, gridTiles,gridTiles);
        
        flipsBackground.at(i).setup(designGrid.y*FLIP_SIZE_FAC, gridTiles);
        flipsBackground.at(i).makeBackTex();

    }
    
    makeBackgroundTex();
}

void MusterContainer::makeBackgroundTex(){
        
    ofFbo tempB;
    tempB.allocate((DISPLAY_NUMX*flipSize)+(DISPLAY_NUMX*offsetX), (DISPLAY_NUMY*flipSize)+(DISPLAY_NUMY*offsetY),  GL_RGBA);
    
    int rSize = (designGrid.y*FLIP_SIZE_FAC);
    tempB.begin();
    ofClear(0, 0, 0,0);
    for (int i = 0; i < displayGrid.size(); i++) {
        ofSetColor(ofColor::fromHsb(255,0,204,255));
        ofDrawRectangle(displayGrid.at(i).x, displayGrid.at(i).y, rSize, rSize);
    }
    tempB.end();
    backgroundTex.clear();
  backgroundTex =   tempB.getTexture();
    
}



void MusterContainer::draw(){
    //ofPushStyle();
    
    
    ofSetColor(255, 255, 255,255);
    /*
    for (int i = 0; i < displayGrid.size(); i++) {
        flipsBackground.at(i).draw(displayGrid.at(i)+centerPos);
    }
     */
    backgroundTex.draw(centerPos);

    if (saveReady) {
        elementColorTouch.setBrightness( ((sin(ofGetElapsedTimef()*8)+1)/2)*200 );
        if (displayColor != elementColorTouch) {
            displayColor = displayColor.lerp(elementColorTouch, 0.04);
        }
        ofSetColor(displayColor);
        
    } else {
        if (displayColor!= targetColor) {
            displayColor = displayColor.lerp(targetColor, 0.04);
        }
        ofSetColor(displayColor);

    }
    for (int i = 0; i < displayGrid.size(); i++) {
        flips.at(i).draw(displayGrid.at(i)+centerPos);
    }
    //ofPopStyle();
    
}

void MusterContainer::setColor(float hue_) {
    
    elementColorOn = ofColor::fromHsb(hue_, 235, 180,255);
    
    elementColorOff = ofColor::fromHsb(elementColorOn.getHue(), elementColorOn.getBrightness(), elementColorOn.getSaturation(), 0 );
    elementColorDarker = ofColor::fromHsb(elementColorOn.getHue(), elementColorOn.getBrightness()-60, elementColorOn.getSaturation(), 255 );
    elementColorDarkerTrans = ofColor::fromHsb(elementColorOn.getHue(), elementColorOn.getBrightness()-60, elementColorOn.getSaturation(), 100 );
     elementColorTouch = ofColor::fromHsb(elementColorOn.getHue(), elementColorOn.getSaturation()-50, 255, 255);
    
    targetColor = elementColorOn;
    displayColor = elementColorOn;
}




void MusterContainer::update(ofVec3f pos_) {
    
    centerPos = pos_;
    }

void MusterContainer::saveToFlip(int index_) {
    
    
}

int MusterContainer::isInside(ofVec2f click_) {
    
    int temp = -1;
    
    for (int i = 0; i < displayGrid.size(); i++) {
        
        
        if (click_.x >= displayGrid.at(i).x+centerPos.x && click_.x <= displayGrid.at(i).x+centerPos.x+flipSize
            && click_.y >= displayGrid.at(i).y+centerPos.y && click_.y <= displayGrid.at(i).y+centerPos.y+flipSize) {
            temp = i;
            break;
            cout << "flip " << i << endl;
        }
    }
    
    return temp;
}