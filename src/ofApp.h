#pragma once

#include "ofMain.h"
#include "ofxRay.h"
#include "instrument.h"
#include "ofxTonic.h"
#include "ofxGui.h"
#include "GlobalGUI.h"
#include "MusterContainer.h"
#include "GlobalScales.h"
#include "ofxFontStash.h"


using namespace Tonic;




class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    void drawStringAndIcons();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    void updateInterfaceMesh();
    
    void drawDebug();
    void intersectPlane(float,float);
    
    ofVec3f intersectPlane(ofVec2f);
    
    
    
    void setupStatesAndAnimation();
    
    void setupGlobalInterface();
    
    
    void updateCamera(float);
    
    bool pointInsideGrid(ofVec3f);
    
    bool drawInfo;
    
    void pauseInterfaceOn();
    void pauseInterfaceOff();
    
    void volumeInterfacOn();
    void volumeInterfaceOff();
    
    void detailEditInterfaceOn();
    void detailEditInterfaceOff();
    
    void editInterfaceOn();
    void editInterfaceOff();
    
    void bothEditInterfaceOff();
    
    void setNewGUI();
    
    void makeDesignGrid();
    
    void makePresetString();
    
    void buttonOnePress();
    void buttonTwoPress();
    void buttonThreePress();
    void buttonFourPress();
    void buttonEditDetail();
    
    //tonic
    void setupAudio();
    void pulseEvent(float &);
    void audioRequested (float * output, int bufferSize, int nChannels);
    //mainout
    ofxTonicSynth tonicSynth;
    RampedValue volumeRamp;
    Generator mainOut;
    
    //scales
    GlobalScales scaleCollection;
    
    //font
    ofxFontStash robotoLight, robotoBold, robotoCon;
    
    
    //save function
    MusterContainer muster;
    
    vector<string> presetNames;
    //3d scene
    ofEasyCam cam;
    ofCamera testCam;
    
    // ofCamera testCam;
    ofNode globalTranslate;
    ofNode camActiveSynth;
    ofNode camNotActiveSynth;
    ofNode synthActivePos;
    ofNode volumeMatrix;
    ofNode camEditDetailPos;
    float camFov;
    float camActiveFov;
    float camTargetFov;
    float camDefaultFov;
    
    float easeInOut(float,float);
    
    float nearClip, farClip;
    
    ofQuaternion camQuatDefault, camQuatTarget;
    
    //animation paths
    ofPolyline oneToActive, twoToActive, threeToActive;
    ofPolyline oneToBack, twoToBack, threeToBack;
    ofPolyline centerToOne, centerToThree;
    ofPolyline camPath;
    ofPolyline camPathBack;
    ofPolyline camUsePath;
    ofPolyline OneVolumeLayerPathOn,TwoVolumeLayerPathOn,ThreeVolumeLayerPathOn;
    ofPolyline OneVolumeLayerPathOff,TwoVolumeLayerPathOff,ThreeVolumeLayerPathOff;
    ofPolyline camEditToDetail, camDetailToEdit;
    float aniPct;
    float aniCam;
    bool animCam;
    float tweenFloat;
    
    bool interfaceMoving;
    
    //Synthebenen
    int activeSynth;
    vector<Instrument> synths;
    vector<ofNode> synthPos;
    
    //ofx ray testing
    ofRay mouseRay;
    ofVec3f worldMouse, intersectPos;
    int vectorPosX, vectorPosY;
    ofPlane thisIntersect;
    ofPlanePrimitive planeForIntersect;
    
 
    ofColor filterColor(ofColor);
    
    
    //light
    ofLight light;
    ofMaterial material;
    
    //misc tap
    unsigned long doubleClickTime, curTap, lastTap;
    bool mouseDragging;
    unsigned long tapCounter;
    
    unsigned long curMouseId;
    
    
    //music
    
    vector<string> notes;
    float pianoKeys[12];
    
    //sound time
    int timeCounter;
    int bpm;
    float bpmTick;
    float lastTick;
    void getBpmTick();
    deque<float>tickTimes;
    
    void volumeRampValueChanged(float&volumeRampValue);
    
    //GlobalGui
    vector<GlobalGUI> mainInterfaceData;
    ofVboMesh mainInterface;
    ofVboMesh mainInterfaceFbo;
  
   
    
    //interface stuff
    bool focusCam;
    int synthButton[3];
    int currentState;
    bool debugCam;
    bool insideSynth;
    
    //designgrid
    ofVec2f designGrid[3][3];
    
    
    ofShader shader;
    ofTexture border;
    
    int editDetailMoveDirection;
    
    //icons
    ofTexture backIcon, detailIcon, pauseIcon, playIcon, volumeIcon, saveIcon, randomIcon ;
    
    float scaleFac;
   };
