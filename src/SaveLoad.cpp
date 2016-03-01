
#include "SaveLoad.h"

SaveLoad::SaveLoad(){
    
}

void SaveLoad::loadSaveFolder(){
    
#if TARGET_OS_IPHONE
    //ios doc dir
#else
    saveDir.open("saves/");
    if (!saveDir.exists()) {
        saveDir.create();
    }
    saveDir.allowExt("xml");
    saveDir.listDir();
#endif
    cout << saveDir.getFiles().size() << endl;
    
    xmlSave tempXml;
    for (int i = 0; i < saveDir.getFiles().size(); i++) {
        tempXml.settings.loadFile("saves/"+saveDir.getFiles().at(i).getFileName());
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
}

void SaveLoad::updatePosition(){
    
    int counterOut = 0;
    int counterIn = 0;
    ofVec3f offsetDown = ofVec3f(0,0,0);
    for (outerIt = xmlSavesMap.rbegin(); outerIt != xmlSavesMap.rend(); ++outerIt){
        
        counterIn = 0;
         offsetDown.y += (((outerIt->second.size()/3))* slotSize.y) + (designGrid.y/3);
        cout << counterOut << offsetDown.y << endl;
        for (innerIt = outerIt->second.begin(); innerIt != outerIt->second.end(); ++innerIt) {
            saveSlot &slot = innerIt->second.slotInfo;
            slot.name = outerIt->first+" #"+ofToString(innerIt->first);
            slot.size = slotSize;
            slot.pos = ofVec3f((counterIn%3)*slotSize.x,-(counterIn/3)*slotSize.y,0)+offsetDown;
            slot.testRect.set(slot.pos, slotSize.x, slotSize.y);
            slot.name ="SKETCH #"+ofToString(innerIt->second.number);
            counterIn++;
        }
        counterOut++;
    }
}
void SaveLoad::setup(ofVec3f dGrid_, ofxFontStash* fsPtr_){
   
    designGrid = dGrid_;
    fsPtr = fsPtr_;
    slotSize = ofVec3f(designGrid.x*4/3, designGrid.y,0);
}

void SaveLoad::update(){
    
}

void SaveLoad::draw(){
    ofPushStyle();
    ofSetColor(255, 255, 255);
    for (outerIt = xmlSavesMap.rbegin(); outerIt != xmlSavesMap.rend(); ++outerIt){
        fsPtr->draw(outerIt->first, 40, 0,outerIt->second.rbegin()->second.slotInfo.pos.y);

        for (innerIt = outerIt->second.begin(); innerIt != outerIt->second.end(); ++innerIt) {
            ofNoFill();

            ofDrawRectangle(innerIt->second.slotInfo.testRect);
            ofFill();
            fsPtr->draw(innerIt->second.slotInfo.name, 40, innerIt->second.slotInfo.pos.x,innerIt->second.slotInfo.pos.y+42);
        }
    }
    ofPopStyle();
}
