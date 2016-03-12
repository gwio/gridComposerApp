#pragma once
#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "ofxFontStash.h"




struct saveSlot{
    ofVec3f pos, offPos;
    ofVec3f size;
    ofRectangle testRect;
    string name;
    ofImage thumb;
    bool active;
    saveSlot(){
        active = false;
        offPos = ofVec3f(0,0,0);
        name = "";
    }
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
    void loadSaveFolder(string);
    void checkDate();
    void setup(ofVec3f, ofxFontStash *, ofVec3f*);
    void update();
    void draw();
    void addNewSave(ofxXmlSettings&);
    void updatePosition();
    ofImage makePng(ofxXmlSettings&,string);
    
    void isInside(ofVec3f);
    void animateGrid(float&);
    
    void deleteSave();
    
    ofxFontStash *fsPtr;
    ofVec3f slotSize;
    ofVec3f *aniVecPtr;
    ofVec3f offsetDown;
    float touchPos, oldTouchPos, acc ,scrollLocation, velo;
    
    map<string, map<int,xmlSave>>::reverse_iterator outerIt;
    map<int,xmlSave>::iterator innerIt;
    
    map<string, map<int,xmlSave>>::reverse_iterator delOuterIt;
    map<int,xmlSave>::iterator delInnerIt;
    
    ofDirectory saveDir;
    map<string, map<int,xmlSave>> xmlSavesMap;
    string saveLastYear, saveLastMonth, saveLastDay;
    int saveLastNumber;
    ofVec3f designGrid;
    
    bool touchDown;
    bool animate;
    ofVec2f touchStart;
    ofVec3f getOffPos(ofVec3f&, ofVec3f&);
    int moveDir;
    bool slotDetail;
    string loadString;
};