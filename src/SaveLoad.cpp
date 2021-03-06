#include "SaveLoad.h"

SaveLoad::SaveLoad(){
    touchPos = 0.0;
    oldTouchPos = 0.0;
    acc = 0.0;
    scrollLocation = 0;
    velo = 0;
    animate = false;
    slotDetail = false;
    confirmDel = false;
    moveDir = 0;
    loadString = "";
    highlight.clear();
    highlight.setMode(OF_PRIMITIVE_TRIANGLES);
    datePosVec.clear();
    firstStart = false;
    
    colorDef = ofColor::fromHsb(255,0,195,255);
    colorA = ofColor::fromHsb(9, 235, 180,255);
    colorB = ofColor::fromHsb(232, 235, 180,255);
    colorC = ofColor::fromHsb(35, 235, 180,255);
    colorD = ofColor::fromHsb(137, 235, 180,255);
}

//--------------------------------------------------------------
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
    ofxXmlSettings loader;
    for (int i = 0; i < saveDir.getFiles().size(); i++) {
        
        
        loader.loadFile(saveDir.getAbsolutePath()+"/"+saveDir.getFiles().at(i).getFileName());
        loader.pushTag("date");
        
        tempXml.year = loader.getValue("year", "");
        tempXml.month = loader.getValue("month", "");
        tempXml.day = loader.getValue("day", "");
        tempXml.number = loader.getValue("number", 1);
        tempXml.hour = loader.getValue("hour","");
        tempXml.slotInfo.highlight = loader.getValue("highlight", 0);
        tempXml.dateDisplay = getDateString(tempXml.day, tempXml.month, tempXml.year);
        if(tempXml.slotInfo.highlight == 0){
            tempXml.slotInfo.displayC = colorDef;
        } else if (tempXml.slotInfo.highlight == 1){
            tempXml.slotInfo.displayC = colorA;
        } else if (tempXml.slotInfo.highlight == 2){
            tempXml.slotInfo.displayC = colorB;
        } else {
            tempXml.slotInfo.displayC = colorC;
        }
        
        loader.popTag();
        string xmlKeyDay = tempXml.year+tempXml.month+tempXml.day;
        
        cout << tempXml.slotInfo.thumb.load(saveDir.getAbsolutePath()+"/"+xmlKeyDay+"#"+ofToString(tempXml.number)+".png") << endl;
        
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
    
    
    //add tutorial to documents folder on first start
    iosFolder = iosFolder_;
    checkFirstStart();
}

//--------------------------------------------------------------
void SaveLoad::checkFirstStart(){
    
    
#if TARGET_OS_IPHONE
    
    if(firstStart){
        
        int exCount = 15;
        ofxXmlSettings example;
        for (int i = 0; i < exCount; i++){
            if(example.loadFile("ex"+ofToString(i+1)+".xml")){;
                example.pushTag("date");
                example.setValue("year", "2000");
                example.setValue("month", "01");
                example.setValue("day", "01");
                example.setValue("number", i+1);
                string exFileName = ofToString(example.getValue("year", ""))+
                ofToString(example.getValue("month", ""))+
                ofToString(example.getValue("day", ""))+"#"+
                ofToString(example.getValue("number", i+1))+".xml";
                example.popTag();
                example.saveFile(iosFolder+"saves/"+exFileName);
                addNewSave(example);
            }
        }
    }
    
    
    
#else
    
    if(firstStart){
        
        int exCount = 15;
        ofxXmlSettings example;
        for (int i = 0; i < exCount; i++){
            if(example.loadFile("ex"+ofToString(i+1)+".xml")){;
                example.pushTag("date");
                example.setValue("year", "2000");
                example.setValue("month", "01");
                example.setValue("day", "01");
                example.setValue("number", i+1);
                string exFileName = ofToString(example.getValue("year", ""))+
                ofToString(example.getValue("month", ""))+
                ofToString(example.getValue("day", ""))+"#"+
                ofToString(example.getValue("number", i+1))+".xml";
                example.popTag();
                example.saveFile("saves/"+exFileName);
                addNewSave(example);
            }
        }
    }
    
#endif
    
    
}


//--------------------------------------------------------------
void SaveLoad::checkDate(){
    
    string cYear = ofGetTimestampString("%Y");
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

//--------------------------------------------------------------
void SaveLoad::addNewSave(ofxXmlSettings &xml_){
    xmlSave tempXml;
    
    xml_.pushTag("date");
    
    tempXml.year = xml_.getValue("year", "");
    tempXml.month = xml_.getValue("month", "");
    tempXml.day = xml_.getValue("day", "");
    tempXml.number = xml_.getValue("number", 1);
    tempXml.hour = xml_.getValue("hour", "");
    tempXml.dateDisplay = getDateString(tempXml.day, tempXml.month, tempXml.year);
    
    xml_.popTag();
    string xmlKey = tempXml.year+tempXml.month+tempXml.day;
    
    tempXml.slotInfo.thumb = makePng(xml_, xmlKey+"#"+ofToString(tempXml.number),slotSize);
    tempXml.slotInfo.displayC = ofColor::fromHsb(255,0,195,255);
    
    xmlSavesMap[xmlKey][tempXml.number] = tempXml;
    
    updatePosition();
    velo += (scrollLocation*-1*0.3);
}

//--------------------------------------------------------------
ofImage SaveLoad::makePng(ofxXmlSettings &xml_, string fileName_, ofVec3f slotSize_){
    ofPixels pix;
    
    ofFbo fbo;
    fbo.allocate(slotSize_.x, slotSize_.y,GL_RGBA);
    
    float offset = (rSize*5)+rSize;
    
    fbo.begin();
    ofClear(0, 0, 0, 0);
    
    for (int i = 0; i < 3; i++) {
        
        xml_.pushTag("currentGrids");
        xml_.pushTag("grid",i);
        string temp = xml_.getValue("info", "0");
        xml_.popTag();
        xml_.popTag();
        xml_.pushTag("SynthSettings");
        xml_.pushTag("synth",i);
        ofColor tc = ofColor::fromHsb(xml_.getValue("cHue", 0), 235, 180, 255);
        
        ofSetColor(colorDef);
        glLineWidth(2);
        ofFill();
        ofDrawRectangle(offset*i,0, rSize*5,rSize*5);
        
        ofSetColor(tc);
        for (int x = 0; x < 5; x++) {
            for (int y = 0; y < 5; y++) {
                if (temp.at(x+(5*y)) == '1') {
                    
                    ofVec2f point = ofVec2f(x*rSize+(offset*i), (rSize*4) - (y*rSize) );
                    ofDrawRectangle(point, rSize, rSize);
            
                }
            }
        }
        
        xml_.popTag();
        xml_.popTag();
        
    }
    
    fbo.end();
    
    pix.allocate(slotSize_.x,slotSize_.y,OF_PIXELS_RGBA);
    fbo.readToPixels(pix);
    
    ofImage tempImg;
    tempImg.setFromPixels(pix);
    
    tempImg.save(saveDir.getAbsolutePath()+"/"+fileName_+".png");
    return tempImg;
}

//--------------------------------------------------------------
void SaveLoad::updatePosition(){
    float slotOffset = 0.333;
    int counterOut = 0;
    int counterIn = 0;
    offsetDown = ofVec3f(0,designGrid.y,0);
    for (outerIt = xmlSavesMap.rbegin(); outerIt != xmlSavesMap.rend(); ++outerIt){
        counterIn = 0;
        offsetDown.y += ((((outerIt->second.size()-1)/3))*(slotSize.y+(slotSize.y*slotOffset))  );
        xmlSave& xmlName = outerIt->second.begin()->second;
        for (innerIt = outerIt->second.begin(); innerIt != outerIt->second.end(); ++innerIt) {
            saveSlot& slot = innerIt->second.slotInfo;
            
            slot.size = slotSize;
            
            slot.pos = ofVec3f((counterIn%3)*slotSize.x,-floor((counterIn/3))* (slotSize.y+slotSize.y*slotOffset),0);
            slot.pos+=offsetDown;
            //border left
            slot.pos+=ofVec3f(designGrid.x*2*0.133*2,0,0);
            //spacing x
            slot.pos+=ofVec3f(counterIn%3* (designGrid.x*4*0.133),  0,0);
            
            slot.testRect.set((int)slot.pos.x, (int)slot.pos.y, slotSize.x, slotSize.y);
            slot.name =ofToString(innerIt->second.number);
            counterIn++;
        }
        offsetDown.y +=  designGrid.y+ (slotSize.y+(slotSize.y*slotOffset));
        
        if( datePosVec.size() != counterOut+1 ){
            dateInfo diTemp;
            datePosVec.push_back(diTemp);
        }
        
        
        if(outerIt == --xmlSavesMap.rend()){
            //examples name string instead date
            datePosVec.at(counterOut).defaultPos = ofVec3f( (2*slotSize.x)+(designGrid.x*2*0.133*2)+(designGrid.x*4*0.133*2)+slotSize.x-fsPtrBold->getBBox("EXAMPLES", fontDefault, 0, 0).width-rSize
                                                           , outerIt->second.rbegin()->second.slotInfo.testRect.position.y-(designGrid.y/2)+(fsPtrBold->getBBox("EXAMPLES", fontDefault, 0, 0).height/2)
                                                           ,0);
        } else {
            datePosVec.at(counterOut).defaultPos = ofVec3f( (2*slotSize.x)+(designGrid.x*2*0.133*2)+(designGrid.x*4*0.133*2)+slotSize.x-fsPtrBold->getBBox(xmlName.dateDisplay, fontDefault, 0, 0).width-rSize
                                                           , outerIt->second.rbegin()->second.slotInfo.testRect.position.y-(designGrid.y/2)+(fsPtrBold->getBBox(xmlName.dateDisplay, fontDefault, 0, 0).height/2)
                                                           ,0);
            
        }
        datePosVec.at(counterOut).displayPos =  datePosVec.at(counterOut).defaultPos;
        counterOut++;
        
        
    }
    while (datePosVec.size() > counterOut+1){
        datePosVec.pop_back();
    }
    
}

//--------------------------------------------------------------
void SaveLoad::setup(float fs_, float fd_, float fb_,ofVec3f dGrid_,
                     ofxFontStash* fsPtrLight_,
                     ofxFontStash* fsPtrReg_,
                     ofxFontStash* fsPtrMedium_,
                     ofxFontStash* fsPtrSemi_,
                     ofxFontStash* fsPtrBold_,
                     ofVec3f *aniPtr_){
    
    designGrid = dGrid_;
    fsPtrLight = fsPtrLight_;
    fsPtrRegular = fsPtrReg_;
    fsPtrMedium = fsPtrMedium_;
    fsPtrSemi = fsPtrSemi_;
    fsPtrBold = fsPtrBold_;
    slotSize = ofVec3f( (designGrid.x*4)/3.5, (designGrid.x*4)/3.5/3, 0);
    rSize = slotSize.x/18;
    highlightLine = rSize/2;
    
    aniVecPtr = aniPtr_;
    
    fontSmall = fs_;
    fontDefault = fd_;
    fontBig = fb_;
}

//--------------------------------------------------------------
void SaveLoad::update(){
    
    velo+=ofClamp(pow((acc*0.5),1),-55,55);
    
    if (-offsetDown.y+(designGrid.y*7) < 0 ){
        if(!touchDown){
            scrollLocation = ofClamp(scrollLocation+velo,-offsetDown.y+(designGrid.y*7),0);
        }else {
            scrollLocation = ofClamp(scrollLocation+acc,-offsetDown.y+(designGrid.y*7),0);
        }
    }
    acc = 0.0;
    
    if ( abs(velo) > 0.0001){
        velo *= 0.751;
    } else {
        velo = 0.0;
    }
    
    updateHighlightVertices();
    
}

//--------------------------------------------------------------
void SaveLoad::draw(){
    ofPushStyle();
    ofPushMatrix();
    ofTranslate(aniVecPtr->x,(int)scrollLocation,0);
    int counter =0;
    for (outerIt = xmlSavesMap.rbegin(); outerIt != xmlSavesMap.rend(); ++outerIt){
        xmlSave &temp = outerIt->second.rbegin()->second;
        ofSetColor(ofColor::fromHsb(255, 0, 51, 255));
        
        if(outerIt == --xmlSavesMap.rend()){
            //example string instead date
            fsPtrBold->draw("EXAMPLES", fontDefault, int(datePosVec.at(counter).displayPos.x), int(datePosVec.at(counter).displayPos.y));
        } else {
            fsPtrBold->draw(temp.dateDisplay, fontDefault, int(datePosVec.at(counter).displayPos.x), int(datePosVec.at(counter).displayPos.y));
        }
        
        for (innerIt = outerIt->second.begin(); innerIt != outerIt->second.end(); ++innerIt) {
            float tempLoc = innerIt->second.slotInfo.pos.y+scrollLocation;
            if(tempLoc > -designGrid.y*3 && tempLoc < designGrid.y*7) {
                ofSetColor(ofColor(255,255,255,255));
                innerIt->second.slotInfo.thumb.draw(innerIt->second.slotInfo.testRect.position);
                ofSetColor(innerIt->second.slotInfo.displayC);
                fsPtrBold->draw(innerIt->second.slotInfo.name, fontDefault*1.25,
                                int(innerIt->second.slotInfo.testRect.position.x-(fsPtrBold->getBBox(innerIt->second.slotInfo.name, fontDefault*1.25, 0, 0).width)-(rSize*2.5)),
                                int(innerIt->second.slotInfo.testRect.position.y+slotSize.y-rSize));
                
            }
        }
        counter++;
    }
    highlight.draw();
    
    ofPopStyle();
    ofPopMatrix();
    
}

//--------------------------------------------------------------
void SaveLoad::isInside(ofVec3f pos_) {
    bool breaking = false;
    int counter = 0;
    for (outerIt = xmlSavesMap.rbegin(); outerIt != xmlSavesMap.rend(); ++outerIt){
        for (innerIt = outerIt->second.begin(); innerIt != outerIt->second.end(); ++innerIt) {
            float tempLoc = innerIt->second.slotInfo.pos.y+scrollLocation;
            if(tempLoc > -designGrid.y*3 && tempLoc < designGrid.y*7) {
                if(innerIt->second.slotInfo.testRect.inside(pos_+ofVec3f(0,-scrollLocation,0))){
                    
                    cout << innerIt->second.slotInfo.name << endl;
                    innerIt->second.slotInfo.active = true;
                    animate = true;
                    datePosVec.at(counter).active = true;
                    breaking = true;
                    
                    selectOuterIt = outerIt;
                    selectInnerIt = innerIt;
                    
                    loadString = saveDir.getAbsolutePath()+"/"+selectInnerIt->second.year+selectInnerIt->second.month+selectInnerIt->second.day+"#"+ofToString(selectInnerIt->second.number)+".xml";
                    
                    xmlTemp.clear();
                    xmlTemp.load(loadString);
                    
                    break;
                }
            }
        }
        counter++;
        if(breaking){
            break;
        }
    }
    if(breaking){
        counter = 0;
        for (outerIt = xmlSavesMap.rbegin(); outerIt != xmlSavesMap.rend(); ++outerIt){
            for (innerIt = outerIt->second.begin(); innerIt != outerIt->second.end(); ++innerIt) {
                saveSlot& slot = innerIt->second.slotInfo;
                if (!slot.active){
                    slot.offPos = getOffPos(pos_, slot.pos);
                } else {
                    slot.offPos = ofVec3f(designGrid.x*3,designGrid.y*3,0)
                    -(slotSize/2)
                    -(slot.pos+ofVec3f(0,scrollLocation,0))
                    +ofVec3f( (fsPtrSemi->getBBox(slot.name, fontDefault*1.25, 0, 0).width/2)+(rSize*0.95),0,0);
                }
            }
            datePosVec.at(counter).offPos = getOffPos(pos_, datePosVec.at(counter).defaultPos);
            
            counter++;
        }
    }
}

//--------------------------------------------------------------
void SaveLoad::deleteSave(){
    //dont delete examples
    if(selectOuterIt != --xmlSavesMap.rend()){
        ofFile temp;
        cout << saveDir.getAbsolutePath()+"/"+selectInnerIt->second.year+selectInnerIt->second.month+selectInnerIt->second.day+"#"+ofToString(selectInnerIt->second.number)+".xml" << endl;
        //remove xml
        temp.open(saveDir.getAbsolutePath()+"/"+
                  selectInnerIt->second.year+selectInnerIt->second.month+
                  selectInnerIt->second.day+"#"+
                  ofToString(selectInnerIt->second.number)+".xml");
        temp.remove();
        //remove png
        temp.open(saveDir.getAbsolutePath()+"/"+
                  selectInnerIt->second.year+selectInnerIt->second.month+
                  selectInnerIt->second.day+"#"+
                  ofToString(selectInnerIt->second.number)+".png");
        temp.remove();
        
        selectOuterIt->second.erase(selectInnerIt);
        if(selectOuterIt->second.size() == 0){
            xmlSavesMap.erase(selectOuterIt->first);
        }
        updatePosition();
    }
}

//--------------------------------------------------------------
ofVec3f SaveLoad::getOffPos(ofVec3f& clicktarget_, ofVec3f& pos_) {
    ofVec3f temp = ofVec3f(0,0,0);

    if(clicktarget_.y-scrollLocation > pos_.y){
        temp.y = -(designGrid.y*10);
    } else {
        temp.y = +(designGrid.y*10);
    }
    
    return temp;
}

//--------------------------------------------------------------
void SaveLoad::animateGrid(float& tween_){
    
    ofVec3f tempPos;
    
    if(animate && tween_>= 1.0){
        animate = false;
        
        int counter = 0;
        for (outerIt = xmlSavesMap.rbegin(); outerIt != xmlSavesMap.rend(); ++outerIt){
            for (innerIt = outerIt->second.begin(); innerIt != outerIt->second.end(); ++innerIt) {
                saveSlot& slot = innerIt->second.slotInfo;
                tempPos = slot.pos+(slot.offPos*abs(moveDir-1.0));
                slot.testRect.setPosition((int)tempPos.x,(int)tempPos.y);
                slot.active = false;
            }
            datePosVec.at(counter).displayPos = datePosVec.at(counter).defaultPos + (datePosVec.at(counter).offPos*abs(moveDir-1.0));
            datePosVec.at(counter).active = false;
            counter++;
        }
        
    }
    
    if(animate){
        int counter = 0;
        for (outerIt = xmlSavesMap.rbegin(); outerIt != xmlSavesMap.rend(); ++outerIt){
            for (innerIt = outerIt->second.begin(); innerIt != outerIt->second.end(); ++innerIt) {
                saveSlot& slot = innerIt->second.slotInfo;
                tempPos = slot.pos+(slot.offPos*abs(moveDir-tween_));
                slot.testRect.setPosition((int)tempPos.x,(int)tempPos.y);
            }
            datePosVec.at(counter).displayPos = datePosVec.at(counter).defaultPos + (datePosVec.at(counter).offPos*abs(moveDir-tween_));
            counter++;
        }
        
    }
}

//--------------------------------------------------------------
void SaveLoad::updateHighlightVertices(){
    highlight.clear();
    ofVec3f temp;
    for (outerIt = xmlSavesMap.rbegin(); outerIt != xmlSavesMap.rend(); ++outerIt){
        for (innerIt = outerIt->second.begin(); innerIt != outerIt->second.end(); ++innerIt) {
            float tempLoc = innerIt->second.slotInfo.pos.y+scrollLocation;
            if(tempLoc > -designGrid.y*3 && tempLoc < designGrid.y*7) {
                saveSlot &slot = innerIt->second.slotInfo;
                temp = slot.testRect.position;
                temp.y += slotSize.y-rSize;
                temp.x -= rSize+(rSize/2);

                highlight.addVertex(temp+ofVec3f(0,0,0));
                highlight.addVertex(temp+ofVec3f(0,-rSize*5,0));
                highlight.addVertex(temp+ofVec3f(rSize/2,-rSize*5,0));
                
                highlight.addVertex(temp+ofVec3f(rSize/2,-rSize*5,0));
                highlight.addVertex(temp+ofVec3f(rSize/2,0,0));
                highlight.addVertex(temp+ofVec3f(0,0,0));
                
                for (int i = 0; i < 6; i++) {
                    highlight.addColor(slot.displayC);
                }
                
                if (slot.cycleColorNext) {
                    slot.myTween = (slot.myTween*1.12)+0.01;
                    
                    if (slot.displayC != slot.targetC) {
                        slot.displayC = slot.displayC.lerp(slot.targetC, slot.myTween);
                    }
                    
                    if (slot.myTween >= 1.0) {
                        slot.displayC = slot.targetC;
                        slot.myTween = 0.0;
                        slot.cycleColorNext = false;
                    }
                }
            }
        }
    }
}

//--------------------------------------------------------------
void SaveLoad::cycleHighlightColor(){
    
    saveSlot &slot = selectInnerIt->second.slotInfo;
    
    slot.highlight = (slot.highlight+1)%5;
    
    xmlTemp.pushTag("date");
    xmlTemp.setValue("highlight", slot.highlight);
    xmlTemp.popTag();
    
    
    xmlTemp.save(loadString);
    
    if(slot.highlight == 0){
        slot.targetC = colorDef;
    } else if (slot.highlight == 1){
        slot.targetC = colorA;
    } else if (slot.highlight == 2){
        slot.targetC = colorB;
    } else if (slot.highlight == 3){
        slot.targetC = colorC;
    } else {
        slot.targetC = colorD;
    }
    slot.displayC = ofColor::fromHsb(slot.displayC.getHue(), slot.displayC.getSaturation()-50, 255,255);
    slot.cycleColorNext = true;
    
}

//--------------------------------------------------------------
string SaveLoad::getDateString(string day_, string month_, string year_){
    string retString;
    string m;
    switch(ofToInt(month_)) {
        case 1 : m = "JANUARY";
            break;
        case 2 : m = "FEBRUARY";
            break;
        case 3 : m = "MARCH";
            break;
        case 4 : m = "APRIL";
            break;
        case 5 : m = "MAY";
            break;
        case 6 : m = "JUNE";
            break;
        case 7 : m = "JULY";
            break;
        case 8 : m = "AUGUST";
            break;
        case 9 : m = "SEPTEMBER";
            break;
        case 10 : m = "OCTOBER";
            break;
        case 11 : m = "NOVEMBER";
            break;
        case 12 : m = "DECEMBER";
            break;
    }
    
    retString = day_+"/"+m+"/"+year_;
    return retString;
}



