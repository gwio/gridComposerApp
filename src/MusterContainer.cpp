
#include "MusterContainer.h"

#define DISPLAY_NUMX 5
#define DISPLAY_NUMY 5
#define FLIP_SIZE_FAC 0.666
#define FLIP_MAX 5*5

MusterContainer::MusterContainer() {
    
}

MusterContainer::MusterContainer(ofVec3f center_, ofVec2f designGrid_,int tiles_) {
    
    gridTiles = tiles_;
    designGrid.x = (designGrid_.x)/(DISPLAY_NUMX);
    designGrid.y = (designGrid_.y)/(DISPLAY_NUMY);
    //designGrid*=0.8;

    flipSize = designGrid.x*FLIP_SIZE_FAC;
    
    displayGrid.clear();
    displayGrid.resize(DISPLAY_NUMX*DISPLAY_NUMY);
    
    for (int x = 0; x < DISPLAY_NUMX; x++) {
        for (int y = 0; y < DISPLAY_NUMY; y++) {
            int index = x+(y*DISPLAY_NUMX);
            displayGrid.at(index).x = (x*designGrid.x)+( (designGrid.x-flipSize)/2);
            displayGrid.at(index).y = (y*designGrid.y)+( (designGrid.y-flipSize)/2);
        }
    }
    
    flips.clear();
    flips.resize(FLIP_MAX);
    
    
    for (int i = 0; i < FLIP_MAX; i++) {
        flips.at(i).setup(designGrid.x*FLIP_SIZE_FAC, gridTiles);
    }
}


void MusterContainer::draw(){
    
    for (int i = 0; i < displayGrid.size(); i++) {
        flips.at(i).draw(displayGrid.at(i)+centerPos);
    }
    
    
}

void MusterContainer::update(ofVec3f pos_) {
    
    centerPos = pos_;
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