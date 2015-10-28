#pragma once

#include "ofMain.h"
#include "ofxRay.h"
#include "instrument.h"
#include "ofxTonic.h"
#include "GlobalGUI.h"
#include "MusterContainer.h"
#include "GlobalScales.h"
#include "HistoryView.h"
#include "ofxFontStash.h"
#include "ofxXmlSettings.h"

#if TARGET_OS_IPHONE
#include "ofxiOS.h"
#include "ofxiOSExtras.h"
#endif

using namespace Tonic;



#if TARGET_OS_IPHONE
class ofApp : public ofxiOSApp{
#else
    class ofApp : public ofBaseApp{
#endif
        
    public:
        void setup();
        void update();
        void draw();
        void exit();
        void drawStringAndIcons();
        
#if TARGET_OS_IPHONE
        void touchDown(ofTouchEventArgs & touch);
        void touchMoved(ofTouchEventArgs & touch);
        void touchUp(ofTouchEventArgs & touch);
        void touchDoubleTap(ofTouchEventArgs & touch);
        void touchCancelled(ofTouchEventArgs & touch);
        void lostFocus();
        void gotFocus();
        void gotMemoryWarning();
        void deviceOrientationChanged(int newOrientation);
#else
        void keyPressed(int key);
        void keyReleased(int key);
        void mouseMoved(int x, int y );
        void mouseDragged(int x, int y, int button);
        void mousePressed(int x, int y, int button);
        void mouseReleased(int x, int y, int button);
        void windowResized(int w, int h);
        void dragEvent(ofDragInfo dragInfo);
        void gotMessage(ofMessage msg);
        
#endif
        
        void replaceMousePressed(int x,int y);
        void replaceMouseDragged(int x,int y);
        void replaceMouseReleased(int x,int y);
        
        
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
        
        void bpmInterfaceOn();
        void bpmInterfaceOff();
        
        void harmonyInterfaceOn();
        void harmonyInterfaceOff();
        
        //void bothEditInterfaceOff();
        
        void setNewGUI();
        
        //edit menu
        void markScaleSteps(int);
        void markSynthNotes(int);
        
        //harmony menu
        void markScaleSteps();
        void markSynthNotes();
        
        void makeDesignGrid();
        
        void makePresetString();
        
        void buttonOnePress();
        void buttonTwoPress();
        void buttonThreePress();
        void buttonFourPress();
        void buttonEditDetail();
        void bpmButtonPress();
        void harmonyButtonPress();
        
        void loadFromXml();
        void saveToXml();
        
        //xml
        ofxXmlSettings settings;
        
        //tonic
        void setupAudio();
        void pulseEvent(int);
        void pulseEventDiv1(float &);
        void pulseEventDiv2(float &);
        void pulseEventDiv3(float &);
        void pulseEventDiv4(float &);
        
        void audioRequested (float * output, int bufferSize, int nChannels);
        //mainout
        ofxTonicSynth tonicSynth;
        RampedValue volumeRamp;
        
        float mainVol;
        Generator mainOut;
        Generator sineA, sineB;
        
        //scales
        GlobalScales scaleCollection;
        int globalScaleVecPos;
        
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
        ofNode harmonyMatrix;
        ofNode timeMatrix;
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
        ofPolyline OneHarmonyPathOn, TwoHarmonyPathOn, ThreeHarmonyPathOn;
        ofPolyline OneHarmonePathOff, TwoHarmonyPathOff, ThreeHarmonyPathOff;
        ofPolyline OneTimePathOn, TwoTimePathOn, ThreeTimePathOn;
        ofPolyline OneTimePathOff, TwoTimePathOff, ThreeTimePathOff;
        
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
        int globalKey;
        
        //sound time
        int timeCounter;
        int bpm;
     
        
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
        ofVec3f designGrid[3][3];
        
        
        ofShader shader;
        ofTexture border;
        
        int editDetailMoveDirection;
        
        //icons
        ofTexture backIcon, detailIcon, pauseIcon, playIcon, volumeIcon, saveIcon, randomIcon, leftIcon, rightIcon, bpmIcon, harmonyIcon ;
        
        float scaleFac;
        
        bool startUp;
        
        float volumeRestart, volumeRestartTarget;
        
        string appVersion;
        
        int pitchCorrect;
        
        HistoryView hvSlotA, hvSlotB, hvSlotC;
    };
