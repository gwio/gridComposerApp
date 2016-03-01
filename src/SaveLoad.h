#pragma once
#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "ofxFontStash.h"



struct saveSlot{
    ofVec3f pos;
    ofVec3f size;
    ofRectangle testRect;
    string name;
};


struct xmlSave{
    ofxXmlSettings settings;
    string year, month, day;
    int number;
    saveSlot slotInfo;
};





class SaveLoad{
    
public:
    
    SaveLoad();
    void loadSaveFolder();
    void checkDate();
    void setup(ofVec3f, ofxFontStash *);
    void update();
    void draw();
    void addNewSave(ofxXmlSettings&);
    void updatePosition();
    
    ofxFontStash *fsPtr;
    ofVec3f slotSize;

    map<string, map<int,xmlSave>>::reverse_iterator outerIt;
    map<int,xmlSave>::iterator innerIt;
    
    ofDirectory saveDir;
    map<string, map<int,xmlSave>> xmlSavesMap;
    string saveLastYear, saveLastMonth, saveLastDay;
    int saveLastNumber;
    ofVec3f designGrid;
    
};