
#include "MusterContainer.h"

#define DISPLAY_NUMX 5
#define DISPLAY_NUMY 4
#define FLIP_SIZE_FAC 0.8
#define FLIP_MAX 5*4

MusterContainer::MusterContainer() {
    
}

MusterContainer::MusterContainer(ofVec3f center_, ofVec2f designGrid_,int tiles_) {
    
    gridTiles = tiles_;
    designGrid.x = (designGrid_.x)/(DISPLAY_NUMX);
    designGrid.y = (designGrid_.y)/(DISPLAY_NUMY);
    //designGrid*=0.8;
    
    flipSize = designGrid.y*FLIP_SIZE_FAC;
    
    displayGrid.clear();
    displayGrid.resize(DISPLAY_NUMX*DISPLAY_NUMY);
    
    for (int x = 0; x < DISPLAY_NUMX; x++) {
        for (int y = 0; y < DISPLAY_NUMY; y++) {
            int index = x+(y*DISPLAY_NUMX);
            displayGrid.at(index).x = (x*designGrid.x)+( (designGrid.x-flipSize)/2);
          //  displayGrid.at(index).y = (y*designGrid.x)+( (designGrid.x-flipSize)/2);

            displayGrid.at(index).y = (y*designGrid.y);
        }
    }
    
    flips.clear();
    flips.resize(FLIP_MAX);
    
  
    
    saveReady = false;
}

void MusterContainer::setup() {
    //uajjjai, 9presets
    
    
    bool tempA[5][5] = {
        {1,1,1,1,1},
        {1,0,1,0,1},
        {1,0,1,0,1},
        {1,0,1,0,1},
        {0,0,0,0,0}
    };
    
    bool *tempB[5];
    for (int i = 0; i < 5; i++) {
        tempB[i] = tempA[i];
    }
    
    for (int i = 0; i < FLIP_MAX; i++) {
        
        flips.at(i).setup(designGrid.y*FLIP_SIZE_FAC, gridTiles);
        flips.at(i).loadData(tempB, gridTiles,gridTiles);
    }
    
    
}
void MusterContainer::draw(){
    
    ofPushStyle();
    if (saveReady) {
    ofSetColor(filterColor(elementColorDarkerTrans));
    } else {
        ofSetColor(filterColor(elementColorDarker));
    }
    for (int i = 0; i < displayGrid.size(); i++) {
        flips.at(i).draw(displayGrid.at(i)+centerPos);
    }
    ofPopStyle();
    
}

void MusterContainer::setColor(float hue_) {
    
    elementColorOn = ofColor::fromHsb(hue_, 100, 160,255);
    
    elementColorOff = ofColor::fromHsb(elementColorOn.getHue(), elementColorOn.getBrightness(), elementColorOn.getSaturation(), 0 );
    elementColorDarker = ofColor::fromHsb(elementColorOn.getHue(), elementColorOn.getBrightness()-60, elementColorOn.getSaturation(), 255 );
    elementColorDarkerTrans = ofColor::fromHsb(elementColorOn.getHue(), elementColorOn.getBrightness()-60, elementColorOn.getSaturation(), 100 );
    elementColorTouch = ofColor::fromHsb(elementColorOn.getHue(), elementColorOn.getSaturation()-50, elementColorOn.getBrightness()+90, 255);
    
    targetColor = elementColorOn;
}

ofColor MusterContainer::filterColor(ofColor c_){
    ofColor temp;
    temp.r = ofClamp(c_.r+15, 30, 220);
    temp.g = ofClamp(c_.g-5, 30, 220);
    temp.b = ofClamp(c_.b-10, 30, 220);
    temp.a = ofClamp(c_.a-10, 10, 230);
    return temp;
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