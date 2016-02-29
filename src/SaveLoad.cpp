
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
    } else {
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
    
    for (outerIt = xmlSavesMap.rbegin(); outerIt != xmlSavesMap.rend(); ++outerIt){
        
        counterIn = 0;
        for (innerIt = outerIt->second.rbegin(); innerIt != outerIt->second.rend(); ++innerIt) {
            saveSlot &slot = innerIt->second.slotInfo;
            slot.name = outerIt->first+" #"+ofToString(innerIt->first);
            slot.size = slotSize;
            slot.pos = ofVec3f(counterIn*20,counterOut*100,0);
            counterIn++;
        }
        counterOut++;
    }
}
void SaveLoad::setup(map<string, map<int,xmlSave>>* slPtr_,map<string, map<int,xmlSave>>::reverse_iterator* out_,map<int,xmlSave>::reverse_iterator* in_, ofxFontStash* fsPtr_){
   
    slotSize = ofVec3f(10,10,0);
   
}

void SaveLoad::update(){
    
}

void SaveLoad::draw(){
    
    for (outerIt = xmlSavesMap.rbegin(); outerIt != xmlSavesMap.rend(); ++outerIt){
        
        for (innerIt = outerIt->second.rbegin(); innerIt != outerIt->second.rend(); ++innerIt) {
            ofDrawEllipse(innerIt->second.slotInfo.pos.x, innerIt->second.slotInfo.pos.y, 10, 10);
                    }
    }
}
