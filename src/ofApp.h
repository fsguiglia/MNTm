#pragma once

#include "ofMain.h"
#include "preprocessor.h"
#include "trigger.h"
#include "ccMap.h"
#include "nnInt.h"
#include "rgb.h"
#include "ofxDatGui.h"
#include "ofxXmlSettings.h"
#include "ofxNetwork.h"
#include "ofxOsc.h"

#define GUI_MARGIN 50
#define TAB_MARGIN 50
#define DISPLAY_X 500
#define DISPLAY_Y 40
#define MENU_X 110
#define PRE_W 300
#define TRG_W 150
#define MAP_W 150
#define NNI_W 150
#define RGB_W 150
#define OSC_W 200

class ofApp : public ofBaseApp{
public:
		void setup();
		void update();
		void draw();
		
		//io
		void udpReceive();

		//input
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseDragged(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		
		//gui
		void scroll(int & scroll, int width, int size, int key);
		void guiSetup();

		//module actions
		void addPreButton(ofxDatGuiButtonEvent e);
		void addPreprocessor(vector <Preprocessor> & pres, vector <ofxDatGui*> & guis, int name = -1);
		void addTrigText(ofxDatGuiTextInputEvent e);
		void addTrigger(vector<Trigger> & trigs, vector<ofxDatGui*> & guis, int dimentions, int name = -1);
		void addMapText(ofxDatGuiTextInputEvent e);
		void addMap(vector<CCMap> & maps, vector<ofxDatGui*> & guis, int dimentions, int name = -1);
		void addNNIButton(ofxDatGuiButtonEvent e);
		void addNNI(vector<NNInt> & vNNI, vector<ofxDatGui*> & guis, int name = -1);
		void addRGBButton(ofxDatGuiButtonEvent e);
		void addRGB(vector<RGBInt> & vRgb, vector<ofxDatGui*> & guis, int name = -1);
		void addOscButton(ofxDatGuiButtonEvent e);
		void addOsc();
		void sendOsc();

		//xml save load clear
		void save(ofxDatGuiButtonEvent e);
		void load(ofxDatGuiButtonEvent e);
		void clear();
		void setMouseControl(ofxDatGuiToggleEvent e);

		//dummies
		void addDummy(ofxDatGuiButtonEvent e);
		void clearDummies(ofxDatGuiButtonEvent e);
		void updateDummies(vector<ofVec2f> & dummies, float speed);

		//gui events
		////modules
		void modButton(ofxDatGuiButtonEvent e);
		void updateModButtons();
		////pre
		void preTextInput(ofxDatGuiTextInputEvent e);
		void preSlider(ofxDatGuiSliderEvent e);
		void preTogInput(ofxDatGuiToggleEvent e);
		void preButtonInput(ofxDatGuiButtonEvent e);
		////map
		void mapTextInput(ofxDatGuiTextInputEvent e);
		void mapTogInput(ofxDatGuiToggleEvent e);
		void mapButtonInput(ofxDatGuiButtonEvent e);
		////trigger
		void trigTextInput(ofxDatGuiTextInputEvent e);
		void trigTogInput(ofxDatGuiToggleEvent e);
		void trigButtonInput(ofxDatGuiButtonEvent e);
		//NNI
		void NNIBInput(ofxDatGuiButtonEvent e);
		void NNITogInput(ofxDatGuiToggleEvent e);
		void NNITextInput(ofxDatGuiTextInputEvent e);
		//RGB
		void RGBBInput(ofxDatGuiButtonEvent e);
		void RGBTogInput(ofxDatGuiToggleEvent e);
		void RGBTextInput(ofxDatGuiTextInputEvent e);
		void RGBSlider(ofxDatGuiSliderEvent e);
		//osc
		void oscTextInput(ofxDatGuiTextInputEvent e);
		void oscButtonInput(ofxDatGuiButtonEvent e);
		//dummy players
		int lastMouseEvent;
		vector<ofVec2f> dummies;
		
		//general layout
		int drawMode;
		vector<string> vModes;
		map<string, int> mScroll;
		int curNNI, curRGB;
		bool bPreprocessorPrint;
		string sPreprocessorPrint;
		ofTrueTypeFont font;
		
		//modules
		vector<Preprocessor> vPres;
		vector<Trigger> vTriggers;
		vector<CCMap> vMaps;
		vector<NNInt> vNNI;
		vector<RGBInt> vRGB;
		vector<ofxOscSender> vOSC;

		//gui
		vector<ofxDatGuiComponent*> gLS, gMod;
		vector<ofxDatGuiComponent*> preGl, trigGl, mapGl, nnGl, rgbGl, oscGl; //tabs
		vector<ofxDatGui*> gPres, gMaps, gTrigs, gNNI, gRGB, gOsc; //modules
		vector<ofxDatGuiValuePlotter*> plotters; //preprocessor plotter

		//io
		bool receivedNew, mouseControl;
		vector<map<string, float>> idMap;
		ofxUDPManager rUDP;
		vector<bool> oscSet;
		vector<int> oscNames; //keeps track of osc destinations
		vector<string> ipPort;

		//xml
		ofxXmlSettings settings;
};
