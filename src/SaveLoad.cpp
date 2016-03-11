#include "SaveLoad.h"

SaveLoad::SaveLoad(){
    touchPos = 0.0;
    oldTouchPos = 0.0;
    acc = 0.0;
    scrollLocation = 0;
    velo = 0;
    animate = false;
    slotDetail = false;
    moveDir = 0;
}

void SaveLoad::loadSaveFolder(string iosFolder_){
    
#if TARGET_OS_IPHONE
    saveDir.open(iosFolder_+"saves/");
    if (!saveDir.exists()) {
        saveDir.create();
    }
    
    saveDir.allowExt("xml");
    saveDir.listDir();
    
    cout <<"dir:" <<saveDir.exists() << endl;
    cout << "dir files size " << saveDir.size() << endl;
#else
    saveDir.open("saves/");
    if (!saveDir.exists()) {
        saveDir.create();
    }
    saveDir.allowExt("xml");
    saveDir.listDir();
    cout << "dir files size " << saveDir.size() << endl;
#endif
    
    xmlSavesMap.clear();
    xmlSave tempXml;
    for (int i = 0; i < saveDir.getFiles().size(); i++) {
#if TARGET_OS_IPHONE
        tempXml.settings.loadFile(iosFolder_+"saves/"+saveDir.getFiles().at(i).getFileName());
#else
        tempXml.settings.loadFile("saves/"+saveDir.getFiles().at(i).getFileName());
#endif
        tempXml.settings.pushTag("date");
        
        tempXml.year = tempXml.settings.getValue("year", "");
        tempXml.month = tempXml.settings.getValue("month", "");
        tempXml.day = tempXml.settings.getValue("day", "");
        tempXml.number = tempXml.settings.getValue("number", 1);
        
        tempXml.settings.popTag();
        string xmlKeyDay = tempXml.year+tempXml.month+tempXml.day;
        
        xmlSavesMap[xmlKeyDay][tempXml.number] = tempXml;
    }
    //check if map is not empty
    
    if (!xmlSavesMap.empty()){
        
        saveLastYear = xmlSavesMap.rbegin()->second.rbegin()->second.year;
        
        saveLastMonth = xmlSavesMap.rbegin()->second.rbegin()->second.month;
        
        saveLastDay = xmlSavesMap.rbegin()->second.rbegin()->second.day;
        
        saveLastNumber = xmlSavesMap.rbegin()->second.rbegin()->second.number;
        
    } else {
        saveLastDay = "";
        saveLastMonth = "";
        saveLastDay = "";
        saveLastNumber = 0;
    }
    cout << saveLastNumber << endl;
    cout << " xmlmap size" << xmlSavesMap.size() << endl;
    updatePosition();
}


void SaveLoad::checkDate(){
    
    string cYear = ofGetTimestampString("%y");
    string cMonth = ofGetTimestampString("%m");
    string cDay = ofGetTimestampString("%d");
    
    if( (cYear == saveLastYear) && (cMonth == saveLastMonth) && (cDay == saveLastDay)){
        saveLastNumber ++;
    } else if(xmlSavesMap.find(cYear+cMonth+cDay) != xmlSavesMap.end() ) {
        saveLastYear = xmlSavesMap[cYear+cMonth+cDay].rbegin()->second.year;
        saveLastMonth = xmlSavesMap[cYear+cMonth+cDay].rbegin()->second.month;
        saveLastDay = xmlSavesMap[cYear+cMonth+cDay].rbegin()->second.day;
        saveLastNumber = xmlSavesMap[cYear+cMonth+cDay].rbegin()->second.number + 1;
    }else {
        saveLastYear = cYear;
        saveLastMonth = cMonth;
        saveLastDay = cDay;
        saveLastNumber = 1;
    }
    
    
    
}

void SaveLoad::addNewSave(ofxXmlSettings &xml_){
    xmlSave tempXml;
    tempXml.settings = xml_;
    tempXml.settings.pushTag("date");
    
    tempXml.year = tempXml.settings.getValue("year", "");
    tempXml.month = tempXml.settings.getValue("month", "");
    tempXml.day = tempXml.settings.getValue("day", "");
    tempXml.number = tempXml.settings.getValue("number", 1);
    
    tempXml.settings.popTag();
    string xmlKey = tempXml.year+tempXml.month+tempXml.day;
    xmlSavesMap[xmlKey][tempXml.number] = tempXml;
    
    updatePosition();
    velo += (scrollLocation*-1*0.3);
}

void SaveLoad::updatePosition(){
    
    int counterOut = 0;
    int counterIn = 0;
    offsetDown = ofVec3f(0,-slotSize.y,0);
    for (outerIt = xmlSavesMap.rbegin(); outerIt != xmlSavesMap.rend(); ++outerIt){
        
        counterIn = 0;
        offsetDown.y += ((((outerIt->second.size()-1)/3))* slotSize.y) + slotSize.y*2;

        for (innerIt = outerIt->second.begin(); innerIt != outerIt->second.end(); ++innerIt) {
            saveSlot &slot = innerIt->second.slotInfo;
            slot.name = outerIt->first+" #"+ofToString(innerIt->first);
            slot.size = slotSize;
            slot.pos = ofVec3f((counterIn%3)*slotSize.x,-floor((counterIn/3))*slotSize.y,0)+offsetDown;
            slot.testRect.set(slot.pos, slotSize.x, slotSize.y);
            slot.name ="SKETCH #"+ofToString(innerIt->second.number);
            counterIn++;
        }
        counterOut++;
    }
    
    saveDir.listDir();
    cout << "new sizue" << saveDir.size() << endl;
    
}
void SaveLoad::setup(ofVec3f dGrid_, ofxFontStash* fsPtr_, ofVec3f *aniPtr_){
    
    designGrid = dGrid_;
    fsPtr = fsPtr_;
    slotSize = ofVec3f(designGrid.x*4/3, designGrid.y,0);
    aniVecPtr = aniPtr_;
}

void SaveLoad::update(){
    
    velo+=ofClamp( pow((acc*0.5),1),-55,55);
    
    if (-offsetDown.y+(designGrid.y*5) < 0 ){
        if(!touchDown){
            scrollLocation = ofClamp(scrollLocation+velo,-offsetDown.y+(designGrid.y*5) ,0);
        }else {
            scrollLocation = ofClamp(scrollLocation+acc,-offsetDown.y+(designGrid.y*5),0);
        }
    }
    acc = 0.0;
    
    if ( abs(velo) > 0.0001){
        velo *= 0.751;
    } else {
        velo = 0.0;
    }
}

void SaveLoad::draw(){
    ofPushStyle();
    ofPushMatrix();
    ofSetColor(255, 255, 255);
    ofTranslate(aniVecPtr->x,scrollLocation,0);
    
    for (outerIt = xmlSavesMap.rbegin(); outerIt != xmlSavesMap.rend(); ++outerIt){
        fsPtr->draw(outerIt->first, 40, 0,outerIt->second.rbegin()->second.slotInfo.testRect.position.y-slotSize.y+40);
        
        for (innerIt = outerIt->second.begin(); innerIt != outerIt->second.end(); ++innerIt) {
            float tempLoc = innerIt->second.slotInfo.pos.y+scrollLocation;
            if(tempLoc > -designGrid.y*3 && tempLoc < designGrid.y*6) {
                ofNoFill();
                ofDrawRectangle(innerIt->second.slotInfo.testRect);
                ofDrawEllipse(innerIt->second.slotInfo.testRect.position,10,10);
                ofFill();
                fsPtr->draw(innerIt->second.slotInfo.name, 40, innerIt->second.slotInfo.testRect.position.x,innerIt->second.slotInfo.testRect.position.y+42);
            }
        }
    }
    ofPopStyle();
    ofPopMatrix();
}

void SaveLoad::isInside(ofVec3f pos_) {
    bool breaking = false;
    for (outerIt = xmlSavesMap.rbegin(); outerIt != xmlSavesMap.rend(); ++outerIt){
        for (innerIt = outerIt->second.begin(); innerIt != outerIt->second.end(); ++innerIt) {
            if(innerIt->second.slotInfo.testRect.inside(pos_+ofVec3f(0,-scrollLocation,0))){
                cout << innerIt->second.slotInfo.name << endl;
                innerIt->second.slotInfo.active = true;
                animate = true;
                breaking = true;
                
                delOuterIt = outerIt;
                delInnerIt = innerIt;
                
                break;
            }
        }
        if(breaking){
            break;
        }
    }
    if(breaking){
        bool breaking = false;
        for (outerIt = xmlSavesMap.rbegin(); outerIt != xmlSavesMap.rend(); ++outerIt){
            for (innerIt = outerIt->second.begin(); innerIt != outerIt->second.end(); ++innerIt) {
                saveSlot& slot = innerIt->second.slotInfo;
                if (!slot.active){
                    slot.offPos = getOffPos(pos_, slot.pos);
                } else {
                    slot.offPos = ofVec3f(designGrid.x*2,designGrid.y*2,0) - (slot.pos+ofVec3f(0,scrollLocation,0));
                }
            }
        }
    }
}

void SaveLoad::deleteSave(){
    ofFile temp;
    cout << saveDir.getAbsolutePath()+"/"+delInnerIt->second.year+delInnerIt->second.month+delInnerIt->second.day+"#"+ofToString(delInnerIt->second.number)+".xml" << endl;
    temp.open(saveDir.getAbsolutePath()+"/"+delInnerIt->second.year+delInnerIt->second.month+delInnerIt->second.day+"#"+ofToString(delInnerIt->second.number)+".xml");
    temp.remove();
    delOuterIt->second.erase(delInnerIt);
    if(delOuterIt->second.size() == 0){
        xmlSavesMap.erase(delOuterIt->first);
    }
    updatePosition();
}

ofVec3f SaveLoad::getOffPos(ofVec3f& clicktarget_, ofVec3f& pos_) {
    ofVec3f temp;
    /*
     if(clicktarget_.x > pos_.x){
     temp.x = -(designGrid.x*10);
     } else {
     temp.x = +(designGrid.x*10);
     }
     */
    
    if(clicktarget_.y-scrollLocation > pos_.y){
        temp.y = -(designGrid.y*10);
    } else {
        temp.y = +(designGrid.y*10);
    }
    
    return temp;
}

void SaveLoad::animateGrid(float& tween_){
    
    if(animate && tween_>= 1.0){
        animate = false;
        
        for (outerIt = xmlSavesMap.rbegin(); outerIt != xmlSavesMap.rend(); ++outerIt){
            for (innerIt = outerIt->second.begin(); innerIt != outerIt->second.end(); ++innerIt) {
                saveSlot& slot = innerIt->second.slotInfo;
                slot.testRect.setPosition(slot.pos+(slot.offPos*abs(moveDir-1.0)) );
                slot.active = false;
            }
        }
        
    }
    
    if(animate){
        for (outerIt = xmlSavesMap.rbegin(); outerIt != xmlSavesMap.rend(); ++outerIt){
            for (innerIt = outerIt->second.begin(); innerIt != outerIt->second.end(); ++innerIt) {
                saveSlot& slot = innerIt->second.slotInfo;
                slot.testRect.setPosition(slot.pos+(slot.offPos*abs(moveDir-tween_)) );
            }
        }
        
    }
}



