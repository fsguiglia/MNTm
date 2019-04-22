#include "ofApp.h"

/*
¿color picker?
chequear normalizacion en map, funcionaba raro para rgb, por ejemplo
rgb y nni tienen que operar por id para que tenga sentido usarlos con trigger y map <- como preprocessor
*/

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetWindowTitle("Untitled - El mapa no es el territorio");
	ofSetFrameRate(100);
	ofSetVerticalSync(false);
	ofSetEscapeQuitsApp(false);
	
	//modes
	drawMode = 0;
	vModes = { "preprocessors", "nni", "rgb", "maps", "triggers", "osc" };
	//scroll
	for (auto mode : vModes) mScroll[mode] = 0;
	//draw
	curNNI = 0;
	curRGB = 0;
	//gui setup
	font.load("ofxbraitsch/fonts/Verdana.ttf", 24);
	guiSetup();
	//control
	receivedNew = false;
	mouseControl = false;
	lastMouseEvent = 0;
	//network
	rUDP.Create();
	rUDP.Bind(21234);
	rUDP.SetEnableBroadcast(true);
	rUDP.SetNonBlocking(true);
}

void ofApp::update() {
	//stores averages, min and max after preprocessing
	map<string, float> ppOutput, nniOutput, rgbOutput;
	idMap.clear();
	receivedNew = false;
	udpReceive();

	//mouse
	if (mouseControl) {
		if (true) {
			map<string, float> testMap;
			float id = -1;
			float x = (float)mouseX / ofGetWidth();
			float y = 1.0 - (float)mouseY / ofGetHeight();
			testMap["id"] = id;
			testMap["x"] = x;
			testMap["y"] = y;
			idMap.push_back(testMap);
			receivedNew = true;
			lastMouseEvent = ofGetElapsedTimeMillis();
		}
	}

	//dummies
	if (dummies.size() > 0) {
		updateDummies(dummies, 0.25);
		for (int i = 0; i < dummies.size(); i++) {
			map<string, float> dummieMap;
			dummieMap["id"] = -1 * i - 1;
			dummieMap["x"] = dummies[i].x;
			dummieMap["y"] = dummies[i].y;
			idMap.push_back(dummieMap);
		}
		receivedNew = true;
	}

	//preprocessors
	if (!bPreprocessorPrint || receivedNew) sPreprocessorPrint.clear();
	bPreprocessorPrint = false;

	for (auto & component : preGl) {
		component->setVisible(vModes[drawMode] == "preprocessors");
		component->setEnabled(vModes[drawMode] == "preprocessors");
		component->update();
	}
	for (int i = 0; i < gPres.size(); i++) {
		gPres[i]->setVisible(vModes[drawMode] == "preprocessors");
		gPres[i]->setEnabled(vModes[drawMode] == "preprocessors");
		gPres[i]->setPosition(i * (PRE_W + 10) + 10 + mScroll["preprocessors"], GUI_MARGIN);
		gPres[i]->update();
	}

	for (int i = 0; i < vPres.size(); i++) {
		bPreprocessorPrint = bPreprocessorPrint || vPres[i].getPrint();
		if (receivedNew) {
			if (vPres[i].getPrint()) {
				vector <float> vPrint = vPres[i].update(idMap, ppOutput);
				for (int j = 0; j < vPrint.size(); j++) {
					sPreprocessorPrint.append(vPres[i].getKey() + ofToString(j) + ": ");
					sPreprocessorPrint.append(ofToString(vPrint[j]));
					sPreprocessorPrint.append(" ");
				}
				bPreprocessorPrint = true;
			}
			else vPres[i].update(idMap, ppOutput);
			if (plotters.size() > 0 && i < plotters.size()) {
				if (plotters[i]->getEnabled()) plotters[i]->setValue(vPres[i].getValue());
			}
		}
	}

	if (!ppOutput.empty())idMap.push_back(ppOutput);

	//nni
	for (auto & component : nnGl) {
		component->setVisible(vModes[drawMode] == "nni");
		component->setEnabled(vModes[drawMode] == "nni");
		component->update();
	}

	for (int i = 0; i < gNNI.size(); i++) {
		gNNI[i]->setVisible(vModes[drawMode] == "nni");
		gNNI[i]->setEnabled(vModes[drawMode] == "nni");
		gNNI[i]->setPosition(i * (NNI_W + 10) + 10 + mScroll["nni"], GUI_MARGIN);
		gNNI[i]->update();
	}

	for (auto &nni : vNNI) {
		if (nni.getActive() && receivedNew) {
			nni.updateWeights(idMap);
			for (int i = 0; i < nni.getWeights().size(); i++) nniOutput["nni" + ofToString(nni.getName()) + "-" + ofToString(i+1)] = nni.getWeights()[i];
		}
	}
	if (!nniOutput.empty())idMap.push_back(nniOutput);

	//rgb
	for (auto & component : rgbGl) {
		component->setVisible(vModes[drawMode] == "rgb");
		component->setEnabled(vModes[drawMode] == "rgb");
		component->update();
	}

	for (int i = 0; i < gRGB.size(); i++) {
		gRGB[i]->setVisible(vModes[drawMode] == "rgb");
		gRGB[i]->setEnabled(vModes[drawMode] == "rgb");
		gRGB[i]->setPosition(i * (RGB_W + 10) + 10 + mScroll["rgb"], GUI_MARGIN);
		gRGB[i]->update();
	}

	for (auto &rgb : vRGB) {
		if (rgb.getActive() && receivedNew) {
			rgb.updateWeights(idMap);
			for (int i = 0; i < rgb.getWeights().size(); i++) rgbOutput["rgb" + ofToString(rgb.getName()) + "-" + ofToString(i + 1)] = rgb.getWeights()[i];
		}
	}

	if (!rgbOutput.empty())idMap.push_back(rgbOutput);

	//maps
	for (auto & component : mapGl) {
		component->setVisible(vModes[drawMode] == "maps");
		component->setEnabled(vModes[drawMode] == "maps");
		component->update();
	}
	for (int i = 0; i < gMaps.size(); i++) {
		gMaps[i]->setPosition(i * (MAP_W + 10) + 10 + mScroll["maps"], GUI_MARGIN);
		gMaps[i]->setVisible(vModes[drawMode] == "maps");
		gMaps[i]->setEnabled(vModes[drawMode] == "maps");
		gMaps[i]->update();
	}
	if (receivedNew) for (int i = 0; i < vMaps.size(); i++) vMaps[i].update(idMap);

	//triggers
	for (auto & component : trigGl) {
		component->setVisible(vModes[drawMode] == "triggers");
		component->setEnabled(vModes[drawMode] == "triggers");
		component->update();
	}
	for (int i = 0; i < gTrigs.size(); i++) {
		gTrigs[i]->setVisible(vModes[drawMode] == "triggers");
		gTrigs[i]->setEnabled(vModes[drawMode] == "triggers");
		gTrigs[i]->setPosition(i * (TRG_W + 10) + 10 + mScroll["triggers"], GUI_MARGIN);
		gTrigs[i]->update();
	}
	if (receivedNew) for (int i = 0; i < vTriggers.size(); i++) vTriggers[i].update(idMap);

	//osc
	for (auto & component : oscGl) {
		component->setVisible(vModes[drawMode] == "osc");
		component->setEnabled(vModes[drawMode] == "osc");
		component->update();
	}
	for (int i = 0; i < gOsc.size(); i++) {
		gOsc[i]->setPosition(i * (OSC_W + 10) + 10 + mScroll["osc"], GUI_MARGIN);
		gOsc[i]->setVisible(vModes[drawMode] == "osc");
		gOsc[i]->setEnabled(vModes[drawMode] == "osc");
		gOsc[i]->update();
	}
	if(receivedNew && vOSC.size() > 0) sendOsc();

	//scroll
	for (auto component : gScroll) component->update();
	//tabs
	for (auto component : gMod) component->update();
	//load/save
	for (auto component : gLS) component->update();
}

void ofApp::draw() {
	ofBackground(0);
	for (auto component : gScroll) component->draw();

	if (vModes[drawMode] == "preprocessors") {
		for (auto component : preGl) component->draw();
		for (auto gui : gPres) gui->draw();
		if (bPreprocessorPrint) {
			ofPushStyle();
			ofSetColor(45, 45, 45);
			ofDrawRectangle(0, ofGetHeight() - 30, ofGetWidth(), 30);
			ofSetColor(200);
			ofDrawBitmapString(sPreprocessorPrint, 10, ofGetHeight() - 10);
			ofPopStyle();
		}
	}
	else if (vModes[drawMode] == "maps") {
		for (auto component : mapGl) component->draw();
		for (auto gui : gMaps) gui->draw();
	}
	else if (vModes[drawMode] == "triggers") {
		for (auto component : trigGl) component->draw();
		for (auto gui : gTrigs) gui->draw();
	}
	else if (vModes[drawMode] == "nni") {
		for (auto component : nnGl) component->draw();
		for (auto gui : gNNI) gui->draw();
		ofPushStyle();
		ofSetColor(45, 45, 45, 100);
		ofDrawRectangle(DISPLAY_X - 8, 0, ofGetWidth() - DISPLAY_X - MENU_X, ofGetHeight());
		ofPopStyle();
		if (vNNI.size() > 0) {
			ofPushStyle();
			ofSetColor(255);
			ofDrawBitmapString("ID: " + ofToString(curNNI), DISPLAY_X, DISPLAY_Y - 5);
			ofPopStyle();
			vNNI[curNNI].draw(DISPLAY_X, DISPLAY_Y);
		}
	}
	else if (vModes[drawMode] == "rgb") {
		for (auto component : rgbGl) component->draw();
		for (auto gui : gRGB) gui->draw();
		ofPushStyle();
		ofSetColor(45, 45, 45, 100);
		ofDrawRectangle(DISPLAY_X - 8, 0, ofGetWidth() - DISPLAY_X - MENU_X, ofGetHeight());
		ofPopStyle();
		if (vRGB.size() > 0) {
			ofPushStyle();
			ofSetColor(255);
			ofDrawBitmapString("ID: " + ofToString(curRGB), DISPLAY_X, DISPLAY_Y - 5);
			ofPopStyle();
			vRGB[curRGB].draw(DISPLAY_X, DISPLAY_Y);
		}
	}
	else if (vModes[drawMode] == "osc") {
		for (auto component : oscGl) component->draw();
		for (auto gui : gOsc) gui->draw();
	}

	ofPushStyle();
	ofSetColor(100);
	ofDrawRectangle(ofGetWidth() - MENU_X, 0, ofGetWidth() - MENU_X , ofGetHeight() * 0.5 - 2);
	ofSetColor(60);
	ofDrawRectangle(ofGetWidth() - MENU_X, ofGetHeight() * 0.5 + 2, ofGetWidth() - MENU_X, ofGetHeight());
	for (int i = 0; i < gMod.size(); i++) gMod[i]->draw();
	for (int i = 0; i < gLS.size(); i++) gLS[i]->draw();
	ofPopStyle();
	//ofDrawBitmapString("FPS: " + ofToString(ceil(ofGetFrameRate())), ofGetWidth() - 100, ofGetHeight() - 20);
}

void ofApp::udpReceive() {
	char udpMessage[100000];
	rUDP.Receive(udpMessage, 100000);
	if (udpMessage[0] != NULL) {
		receivedNew = true;
		ofJson rJson = ofJson::parse(ofToString(udpMessage).c_str());
		if (rJson.is_array()) {
			for (auto obj : rJson) {
				auto curObj = obj.get<ofJson::object_t>();
				map <string, float> curMap;
				for (auto member : curObj) curMap[member.first] = (float)member.second;
				idMap.push_back(curMap);
			}
		}
	}
	
}

void ofApp::keyPressed(int key) {
	if (key == OF_KEY_PAGE_UP || key == OF_KEY_PAGE_DOWN) {
		int dir = 1;
		if (key == OF_KEY_PAGE_UP) dir = -1;
		if (vModes[drawMode] == "preprocessors") scroll(mScroll["preprocessors"], PRE_W, vPres.size(), dir);
		else if (vModes[drawMode] == "maps") scroll(mScroll["maps"], MAP_W, vMaps.size(), dir);
		else if (vModes[drawMode] == "triggers") scroll(mScroll["triggers"], TRG_W, vTriggers.size(), dir);
		else if (vModes[drawMode] == "nni") scroll(mScroll["nni"], NNI_W, vNNI.size(), dir);
		else if (vModes[drawMode] == "rgb") scroll(mScroll["rgb"], RGB_W, vRGB.size(), dir);
		else if (vModes[drawMode] == "osc") scroll(mScroll["osc"], OSC_W, oscNames.size(), dir);
	}
}

void ofApp::keyReleased(int key){
	if (key == OF_KEY_TAB) {
		drawMode += 1;
		if (drawMode >= vModes.size()) drawMode = 0;
		updateModButtons();
	}
	
	//test osc output with space bar
	if (vModes[drawMode] == "osc" && key == ' ') {
		cout << "osc test" << endl;
		if (vOSC.size() > 0) {
			ofxOscMessage m;
			m.setAddress("/test");
			m.addStringArg("test");
			vOSC[0].sendMessage(m);
		}
	}
}

void ofApp::mouseDragged(int x, int y, int button){
	//nnint xPad should be solved in class. It crashes when out of range...
	if (vModes[drawMode] == "nni" && vNNI.size() > 0) {
		if (!vNNI[curNNI].getActive()) {
			ofRectangle bounds = vNNI[curNNI].getBounds();
			bounds.x += DISPLAY_X;
			bounds.y += DISPLAY_Y;
			if (bounds.inside(x, y)) vNNI[curNNI].previewZone((x - DISPLAY_X) / bounds.width, (y - DISPLAY_Y) / bounds.height);
			vNNI[curNNI].updateFbo();
		}
	}
}

void ofApp::mouseReleased(int x, int y, int button) {
	if (vModes[drawMode] == "nni" && vNNI.size() > 0) {
		if (!vNNI[curNNI].getActive()) {
			ofRectangle bounds = vNNI[curNNI].getBounds();
			bounds.x += DISPLAY_X;
			bounds.y += DISPLAY_Y;
			if (bounds.inside(x, y)) vNNI[curNNI].addZone((x - DISPLAY_X) / bounds.width, (y - DISPLAY_Y) / bounds.height);
			vNNI[curNNI].updateFbo();
		}
	}
}

void ofApp::scroll(ofxDatGuiButtonEvent e) {
	int dir = ofToInt(e.target->getName());
	if (vModes[drawMode] == "preprocessors") scroll(mScroll["preprocessors"], PRE_W, vPres.size(), dir);
	else if (vModes[drawMode] == "maps") scroll(mScroll["maps"], MAP_W, vMaps.size(), dir);
	else if (vModes[drawMode] == "triggers") scroll(mScroll["triggers"], TRG_W, vTriggers.size(), dir);
	else if (vModes[drawMode] == "nni") scroll(mScroll["nni"], NNI_W, vNNI.size(), dir);
	else if (vModes[drawMode] == "rgb") scroll(mScroll["rgb"], RGB_W, vRGB.size(), dir);
	else if (vModes[drawMode] == "osc") scroll(mScroll["osc"], OSC_W, oscNames.size(), dir);
}

void ofApp::scroll(int & scroll, int width, int size, int dir) {
			if (dir == 1) scroll -= (width + 10);
			else if (dir == -1) scroll += (width + 10);
			int scrollMax = -1 * ((size - 1) * (width + 10) + 10);
			if (scroll > 0) scroll = 0;
			else if (scroll < scrollMax) scroll = scrollMax;
}

void ofApp::guiSetup() {
	//add components
	ofxDatGuiComponent* component;
	ofxDatGuiTheme* theme = new ofxDatGuiThemeAqua();
	//preprocessores
	component = new ofxDatGuiLabel("Preprocessors");
	component->setTheme(theme);
	component->setWidth(100, 1);
	component->setPosition(10, 15);
	preGl.push_back(component);
	component = new ofxDatGuiButton("add");
	component->setTheme(theme);
	component->setWidth(50, 1);
	component->setPosition(112, 15);
	component->onButtonEvent(this, &ofApp::addPreButton);
	preGl.push_back(component);
	//maps
	component = new ofxDatGuiLabel("Maps");
	component->setTheme(theme);
	component->setWidth(50, 1);
	component->setPosition(10, 15);
	mapGl.push_back(component);
	component = new ofxDatGuiTextInput("Keys");
	component->setTheme(theme);
	component->setWidth(100,0.5);
	component->setPosition(62, 15);
	component->onTextInputEvent(this, &ofApp::addMapText);
	mapGl.push_back(component);
	//triggers
	component = new ofxDatGuiLabel("Triggers");
	component->setTheme(theme);
	component->setWidth(70, 1);
	component->setPosition(10, 15);
	trigGl.push_back(component);
	component = new ofxDatGuiTextInput("Keys");
	component->setTheme(theme);
	component->setWidth(100, 0.5);
	component->setPosition(82, 15);
	component->onTextInputEvent(this, &ofApp::addTrigText);
	trigGl.push_back(component);
	//nni
	component = new ofxDatGuiLabel("NNI");
	component->setTheme(theme);
	component->setWidth(50, 1);
	component->setPosition(10, 15);
	nnGl.push_back(component);
	component = new ofxDatGuiButton("add");
	component->setTheme(theme);
	component->setWidth(50, 1);
	component->setPosition(62, 15);
	component->onButtonEvent(this, &ofApp::addNNIButton);
	nnGl.push_back(component);
	//rgb
	component = new ofxDatGuiLabel("RGB");
	component->setTheme(theme);
	component->setWidth(50, 1);
	component->setPosition(10, 15);
	rgbGl.push_back(component);
	component = new ofxDatGuiButton("add");
	component->setTheme(theme);
	component->setWidth(50, 1);
	component->setPosition(62, 15);
	component->onButtonEvent(this, &ofApp::addRGBButton);
	rgbGl.push_back(component);
	//osc
	component = new ofxDatGuiLabel("Osc");
	component->setTheme(theme);
	component->setWidth(100, 1);
	component->setPosition(10, 15);
	oscGl.push_back(component);
	component = new ofxDatGuiButton("add");
	component->setTheme(theme);
	component->setWidth(50, 1);
	component->setPosition(112, 15);
	component->onButtonEvent(this, &ofApp::addOscButton);
	oscGl.push_back(component);
	//modules
	for (int i = 0; i < vModes.size(); i++) {
		component = new ofxDatGuiButton(vModes[i]);
		component->setTheme(theme);
		component->setWidth(110, 0.5);
		component->onButtonEvent(this, &ofApp::modButton);
		component->setPosition(ofGetWidth() - 110, 15 + 35 * i);
		component->setLabelAlignment(ofxDatGuiAlignment::CENTER);
		component->setBackgroundColor(100);
		gMod.push_back(component);
	}
	updateModButtons();
	//settings
	component = new ofxDatGuiButton("Save");
	component->onButtonEvent(this, &ofApp::save);
	gLS.push_back(component);
	component = new ofxDatGuiButton("Load");
	component->onButtonEvent(this, &ofApp::load);
	gLS.push_back(component);
	component = new ofxDatGuiToggle("Mouse in");
	component->onToggleEvent(this, &ofApp::setMouseControl);
	gLS.push_back(component);
	component = new ofxDatGuiButton("add dummy");
	component->onButtonEvent(this, &ofApp::addDummy);
	gLS.push_back(component);
	component = new ofxDatGuiButton("clear dummies");
	component->onButtonEvent(this, &ofApp::clearDummies);
	gLS.push_back(component);

	for (int i = 0; i < gLS.size(); i++) {
		gLS[i]->setTheme(theme);
		gLS[i]->setWidth(110, 0.5);
		gLS[i]->setPosition(ofGetWidth() - 110, 255 + i * 35);
		gLS[i]->setLabelAlignment(ofxDatGuiAlignment::CENTER);
		gLS[i]->setBackgroundColor(60);
	}

	//scroll
	component = new ofxDatGuiButton("<");
	component->setLabelAlignment(ofxDatGuiAlignment::CENTER);
	component->setName("-1");
	component->onButtonEvent(this, &ofApp::scroll);
	component->setTheme(theme);
	component->setWidth(20, 1);
	component->setPosition(10, ofGetHeight() - 30);
	gScroll.push_back(component);

	component = new ofxDatGuiButton(">");
	component->setLabelAlignment(ofxDatGuiAlignment::CENTER);
	component->setName("1");
	component->onButtonEvent(this, &ofApp::scroll);
	component->setTheme(theme);
	component->setWidth(20, 1);
	component->setPosition(32, ofGetHeight() - 30);
	gScroll.push_back(component);
}

void ofApp::addPreButton(ofxDatGuiButtonEvent e) {
	addPreprocessor(vPres, gPres);
}

void ofApp::addPreprocessor(vector<Preprocessor>& pres, vector<ofxDatGui*>& guis, int name) {
	Preprocessor preprocessor;
	
	int curName = name;
	if (name == -1) {
		for (auto pre : pres) if (pre.getName() > curName) curName = pre.getName();
		curName++;
	}
	string sName = ofToString(curName);

	preprocessor.setName(curName);
	pres.push_back(preprocessor);
	
	ofxDatGui* gui = new ofxDatGui(ofxDatGuiAnchor::TOP_RIGHT);
	gui->addLabel("id: " + sName);
	gui->addTextInput("key")->setName(sName + "-key");
	gui->addToggle("normalize")->setName(sName + "-normalize");
	gui->addTextInput("norm")->setName(sName + "-norm");
	ofxDatGuiFolder* folder = gui->addFolder("settings");
	folder->addToggle("minMax")->setName(sName + "-minMax");;
	folder->addToggle("average")->setName(sName + "-average");;
	folder->addToggle("roi")->setName(sName + "-roi");;
	folder->addToggle("draw")->setName(sName + "-draw");;
	folder->addToggle("print")->setName(sName + "-print");;
	folder->collapse();
	gui->addButton("remove")->setName(sName + "-remove");
	ofxDatGuiValuePlotter* plotter =  gui->addValuePlotter("monitor", 0, 1);
	plotter->setDrawMode(ofxDatGuiGraph::LINES);
	plotter->setEnabled(false);
	gui->onToggleEvent(this, &ofApp::preTogInput);
	gui->onTextInputEvent(this, &ofApp::preTextInput);
	gui->onSliderEvent(this, &ofApp::preSlider);
	gui->onButtonEvent(this, &ofApp::preButtonInput);
	gui->setAutoDraw(false);
	gui->setPosition(0, GUI_MARGIN);
	gui->setTheme(new ofxDatGuiThemeAqua(), true);
	gui->setWidth(PRE_W, 0.3);
	gui->getLabel("id: " + sName)->setBackgroundColor(40);
	guis.push_back(gui);
	plotters.push_back(plotter);
}

void ofApp::addTrigText(ofxDatGuiTextInputEvent e) {
		int keys = ofToInt(e.text);
		if (keys > 0 && keys < 10) addTrigger(vTriggers, gTrigs, keys);
		e.target->setText("");
}

void ofApp::addTrigger(vector<Trigger>& trigs, vector<ofxDatGui*> & guis, int dimentions, int name) {
	Trigger trigger;
	vector <string> keys;
	int curName = name;
	
	if (name == -1) {
		for (auto trig : trigs) if (trig.getName() > curName) curName = trig.getName();
		curName++;
	}
	string sName = ofToString(curName);

	for (int i = 0; i < dimentions; i++) {
		keys.push_back(ofToString(i));
	}
	trigger.setup(keys, "");
	trigger.setName(curName);
	vTriggers.push_back(trigger);

	ofxDatGui* gui = new ofxDatGui(ofxDatGuiAnchor::TOP_RIGHT);
	gui->addLabel("id: " + sName);

	for (int i = 0; i < dimentions; i++) {
		string sI = ofToString(i + 1);
		ofxDatGuiFolder* folder = gui->addFolder(sI);
		folder->addTextInput("key:", "")->setName(sName + "-key:" + sI);
		folder->addTextInput("thresh:", "")->setName(sName + "-thresh:" + sI);
		folder->addTextInput("margin:", "")->setName(sName + "-margin:" + sI);
		folder->addToggle("invert:", false)->setName(sName + "-invert:" + sI);
		folder->collapse();
	}
	gui->addTextInput("address", "")->setName(sName + "-address");
	gui->addToggle("active")->setName(sName + "-active");
	gui->addButton("remove")->setName(sName + "-remove");
	gui->onToggleEvent(this, &ofApp::trigTogInput);
	gui->onTextInputEvent(this, &ofApp::trigTextInput);
	gui->onButtonEvent(this, &ofApp::trigButtonInput);
	gui->setAutoDraw(false);
	gui->setPosition(0, GUI_MARGIN);
	gui->setTheme(new ofxDatGuiThemeAqua(),true);
	gui->setWidth(TRG_W, LABEL_WIDTH);
	gui->getLabel("id: " + sName)->setBackgroundColor(40);
	guis.push_back(gui);
}

void ofApp::addMapText(ofxDatGuiTextInputEvent e) {
		int keys = ofToInt(e.text);
		if (keys > 0 && keys < 10) addMap(vMaps, gMaps, keys);
		e.target->setText("");
}

void ofApp::addMap(vector<CCMap> & maps, vector<ofxDatGui*> & guis, int dimentions, int name) {
	CCMap map;
	vector <string> keys;
	int curName = name;
	if (name == -1) {
		for (auto map : maps) if (map.getName() > curName) curName = map.getName();
		curName++;
	}
	string sName = ofToString(curName);

	for (int i = 0; i < dimentions; i++) {
		keys.push_back(ofToString(i));
	}

	map.setup(keys, "");
	map.setName(curName);
	maps.push_back(map);
	
	ofxDatGui* gui = new ofxDatGui(ofxDatGuiAnchor::TOP_RIGHT);
	gui->addLabel("id: " + sName);

	for (int i = 0; i < dimentions; i++) {
		string sI = ofToString(i + 1);
		ofxDatGuiFolder* folder = gui->addFolder(sI);
		folder->addTextInput("key","")->setName(sName + "-key:" + sI);
		folder->addTextInput("range", "0,1")->setName(sName + "-range:" + sI);
		folder->addToggle("crop", true)->setName(sName + "-crop:" + sI);
		folder->collapse();
	}
	gui->addTextInput("address", "")->setName(sName + "-address");
	gui->addToggle("active")->setName(sName + "-active");
	gui->addButton("remove")->setName(sName + "-remove");
	gui->onToggleEvent(this, &ofApp::mapTogInput);
	gui->onTextInputEvent(this, &ofApp::mapTextInput);
	gui->onButtonEvent(this, &ofApp::mapButtonInput);
	gui->setAutoDraw(false);
	gui->setPosition(0, GUI_MARGIN);
	gui->setTheme(new ofxDatGuiThemeAqua(), true);
	gui->setWidth(MAP_W, LABEL_WIDTH);
	gui->getLabel("id: " + sName)->setBackgroundColor(40);
	guis.push_back(gui);
}

void ofApp::addNNIButton(ofxDatGuiButtonEvent e) {
	addNNI(vNNI, gNNI);
}

void ofApp::addNNI(vector<NNInt>& vNNI, vector<ofxDatGui*> & guis, int name) {
	NNInt nnint;
	nnint.setup("", "", 400, 400);
	int curName = name;
	if (name == -1) {
		for (auto nni : vNNI) if (nni.getName() > curName) curName = nni.getName();
		curName++;
	}
	string sName = ofToString(curName);
	
	nnint.setName(curName);
	vNNI.push_back(nnint);

	ofxDatGui* gui = new ofxDatGui(ofxDatGuiAnchor::TOP_RIGHT);
	gui->addLabel("id: " + sName);
	gui->addTextInput("keys", "")->setName(sName + "-keys");
	gui->addTextInput("address", "")->setName(sName + "-address");
	gui->addButton("show")->setName(sName + "-show");
	gui->addToggle("active")->setName(sName + "-active");
	gui->addButton("clear")->setName(sName + "-clear");
	gui->addButton("remove")->setName(sName + "-remove");
	gui->onButtonEvent(this, &ofApp::NNIBInput);
	gui->onToggleEvent(this, &ofApp::NNITogInput);
	gui->onTextInputEvent(this, &ofApp::NNITextInput);
	gui->setAutoDraw(false);
	gui->setPosition(0, GUI_MARGIN);
	gui->setTheme(new ofxDatGuiThemeAqua(), true);
	gui->setWidth(MAP_W, LABEL_WIDTH);
	gui->getLabel("id: " + sName)->setBackgroundColor(40);
	guis.push_back(gui);
}

void ofApp::addRGBButton(ofxDatGuiButtonEvent e) {
	addRGB(vRGB, gRGB);
}

void ofApp::addRGB(vector<RGBInt>& vRgb, vector<ofxDatGui*>& guis, int name) {
	RGBInt rgbint;
	rgbint.setup("", "", 400, 400);
	int curName = name;

	if (name == -1) {
		for (auto rgb : vRGB) if (rgb.getName() > curName) curName = rgb.getName();
		curName++;
	}
	string sName = ofToString(curName);

	rgbint.setName(curName);
	vRGB.push_back(rgbint);

	ofxDatGui* gui = new ofxDatGui(ofxDatGuiAnchor::TOP_RIGHT);
	gui->addLabel("id: " + sName);
	gui->addTextInput("keys", "")->setName(sName + "-keys");
	gui->addTextInput("address", "")->setName(sName + "-address");
	gui->addButton("show")->setName(sName + "-show");;
	gui->addToggle("active")->setName(sName + "-active");;
	gui->addButton("load")->setName(sName + "-load");;
	gui->addButton("remove")->setName(sName + "-remove");;
	gui->addSlider("size", 0, 40, 5)->setName(sName + "-size");;
	gui->onButtonEvent(this, &ofApp::RGBBInput);
	gui->onToggleEvent(this, &ofApp::RGBTogInput);
	gui->onTextInputEvent(this, &ofApp::RGBTextInput);
	gui->onSliderEvent(this, &ofApp::RGBSlider);
	gui->setAutoDraw(false);
	gui->setPosition(0, GUI_MARGIN);
	gui->setTheme(new ofxDatGuiThemeAqua(), true);
	gui->setWidth(MAP_W, LABEL_WIDTH);
	gui->getLabel("id: " + sName)->setBackgroundColor(40);
	guis.push_back(gui);
}

void ofApp::addOscButton(ofxDatGuiButtonEvent e) {
	addOsc();
}

void ofApp::addOsc() {
	oscSet.push_back(false);
	int curName = -1;
	for (auto osc : oscNames) if (osc > curName) curName = osc;
	curName++;
	
	ofxOscSender sender;
	sender.setup("", 0);
	vOSC.push_back(sender);
	ipPort.push_back("");
	oscNames.push_back(curName);
	string sName = ofToString(curName);
	
	ofxDatGui* gui = new ofxDatGui(ofxDatGuiAnchor::TOP_RIGHT);
	gui->addLabel("id: " + sName);
	gui->addTextInput("ip:port", "")->setName(sName + "-ip:port");
	gui->addButton("remove")->setName(sName + "-remove");
	gui->onButtonEvent(this, &ofApp::oscButtonInput);
	gui->onTextInputEvent(this, &ofApp::oscTextInput);
	gui->setAutoDraw(false);
	gui->setPosition(0, GUI_MARGIN);
	gui->setTheme(new ofxDatGuiThemeAqua(), true);
	gui->setWidth(OSC_W, 0.3);
	gui->getLabel("id: " + sName)->setBackgroundColor(40);
	gOsc.push_back(gui);
}

void ofApp::sendOsc() {
	for (int i = 0; i < vOSC.size(); i++) {
		if (oscSet[i]) {
			ofxOscMessage m;
			for (int j = 0; j < vMaps.size(); j++) {
				if (vMaps[j].getActive()) {
					vector<vector <float>> mapValues = vMaps[j].getValues();
					m.setAddress(vMaps[j].getAddress());
					for (int k = 0; k < mapValues.size(); k++) {
						for (int l = 0; l < mapValues[k].size(); l++) {
							m.addFloatArg(mapValues[k][l]);
						}
					}
					if(m.getNumArgs() > 0) vOSC[i].sendMessage(m);
					m.clear();
				}
			}
			
			for (int j = 0; j < vTriggers.size(); j++) {
				if (vTriggers[j].getActive()) {
					if (vTriggers[j].getChange() != -1) {
						m.setAddress(vTriggers[j].getAddress());
						m.addIntArg(vTriggers[j].getChange());
						vOSC[i].sendMessage(m);
						m.clear();
					}
				}
			}

			for(int j = 0; j < vNNI.size(); j++) {
				if (vNNI[j].getActive()) {
					m.setAddress(vNNI[j].getAddress());
					for (auto &weight : vNNI[j].getWeights()) {
						m.addFloatArg(weight);
					}
					vOSC[i].sendMessage(m);
					m.clear();
				}
			}

			for (int j = 0; j < vRGB.size(); j++) {
				if (vRGB[j].getActive()) {
					m.setAddress(vRGB[j].getAddress());
					for (auto &weight : vRGB[j].getWeights()) {
						m.addFloatArg(weight);
					}
					vOSC[i].sendMessage(m);
					m.clear();
				}
			}
		}
	}
}

void ofApp::save(ofxDatGuiButtonEvent e) {
	ofFileDialogResult openFileResult = ofSystemSaveDialog("untitled.xml", "save");
	if (openFileResult.bSuccess) {
		settings.loadFile(openFileResult.getPath());
		settings.clear();
		ofSetWindowTitle(openFileResult.getName() +  " - El mapa no es el territorio");
		//preprocessors
		settings.addTag("preprocessors");
		settings.pushTag("preprocessors");
		for (int i = 0; i < vPres.size(); i++) {
			settings.addTag("preprocessor");
			settings.pushTag("preprocessor", i);
			settings.addValue("name", vPres[i].getName());
			settings.addValue("key", vPres[i].getKey());
			settings.addValue("norm", vPres[i].getNorm());
			settings.addValue("minMax", vPres[i].getMinMax());
			settings.addValue("average", vPres[i].getAverage());
			settings.addValue("roi", vPres[i].getRoi());
			settings.addValue("inMin", vPres[i].getValues()[0]);
			settings.addValue("inMax", vPres[i].getValues()[1]);
			settings.addValue("outMin", vPres[i].getValues()[2]);
			settings.addValue("outMax", vPres[i].getValues()[3]);
			settings.popTag();
		}
		settings.popTag();

		//maps
		settings.addTag("maps");
		settings.pushTag("maps");
		for (int i = 0; i < vMaps.size(); i++) {
			settings.addTag("map");
			settings.pushTag("map", i);
			settings.addValue("name", vMaps[i].getName());
			vector<string> keys = vMaps[i].getKeys();
			for (int j = 0; j < keys.size(); j++) {
				settings.addTag("dim");
				settings.pushTag("dim", j);
				settings.addValue("key", vMaps[i].getKeys()[j]);
				settings.addValue("rMin", vMaps[i].getRange()[j][0]);
				settings.addValue("rMax", vMaps[i].getRange()[j][1]);
				settings.addValue("crop", vMaps[i].getCrop()[j]);
				settings.popTag();
			}
			settings.addValue("active", vMaps[i].getActive());
			settings.addValue("address", vMaps[i].getAddress());
			settings.popTag();
		}
		settings.popTag();

		//triggers
		settings.addTag("triggers");
		settings.pushTag("triggers");
		for (int i = 0; i < vTriggers.size(); i++) {
			settings.addTag("trigger");
			settings.pushTag("trigger", i);
			settings.addValue("name", vTriggers[i].getName());
			vector<string> keys = vTriggers[i].getKeys();
			for (int j = 0; j < keys.size(); j++) {
				settings.addTag("dim");
				settings.pushTag("dim", j);
				settings.addValue("key", keys[j]);
				settings.addValue("invert", vTriggers[i].getInverted()[j]);
				settings.addValue("inMin", vTriggers[i].getValues()[j][0]);
				settings.addValue("inMax", vTriggers[i].getValues()[j][1]);
				settings.addValue("margin", vTriggers[i].getMargins()[j]);
				settings.popTag();
			}
			settings.addValue("active", vTriggers[i].getActive());
			settings.addValue("address", vTriggers[i].getAddress());
			settings.popTag();
		}
		settings.popTag();

		//nnInt
		settings.addTag("nnints");
		settings.pushTag("nnints");
		for (int i = 0; i < vNNI.size(); i++) {
			settings.addTag("nnint");
			settings.pushTag("nnint", 0);
			settings.addValue("name", vNNI[i].getName());
			settings.addValue("xKey", vNNI[i].getKeys()[0]);
			settings.addValue("yKey", vNNI[i].getKeys()[1]);
			settings.addTag("points");
			settings.pushTag("points");
			vector<glm::vec3> points = vNNI[i].getNPoints();
			for (int i = 0; i < points.size(); i++) {
				settings.addTag("point");
				settings.pushTag("point", i);
				settings.addValue("x", points[i].x);
				settings.addValue("y", points[i].y);
				settings.popTag();
			}
			settings.popTag();
			settings.addValue("address", vNNI[i].getAddress());
			settings.addValue("running", vNNI[i].getActive());
			settings.popTag();
		}
		settings.popTag();

		//RGB
		settings.addTag("rgbs");
		settings.pushTag("rgbs");
		for (int i = 0; i < vRGB.size(); i++) {
			settings.addTag("rgb");
			settings.pushTag("rgb", 0);
			settings.addValue("name", vRGB[i].getName());
			settings.addValue("xKey", vRGB[i].getKeys()[0]);
			settings.addValue("yKey", vRGB[i].getKeys()[1]);
			settings.addValue("path", vRGB[i].getPath());
			settings.addValue("path", vRGB[i].getSqSize());
			settings.addValue("address", vRGB[i].getAddress());
			settings.addValue("running", vRGB[i].getActive());
			settings.popTag();
		}
		settings.popTag();

		//osc
		settings.addTag("senders");
		settings.pushTag("senders");
		for (int i = 0; i < ipPort.size(); i++) {
			if (oscSet[i] = true) {
				settings.addTag("sender");
				settings.pushTag("sender", i);
				vector<string> vIpPort = ofSplitString(ipPort[i], ":");
				if (vIpPort.size() == 2) {
					settings.addValue("ip", vIpPort[0]);
					settings.addValue("port", ofToInt(vIpPort[1]));
				}
				settings.popTag();
			}
		}
		settings.popTag();
		settings.saveFile(openFileResult.getPath());
	}
}

void ofApp::load(ofxDatGuiButtonEvent e) {
	ofFileDialogResult openFileResult = ofSystemLoadDialog("load", false, "..");
	if (openFileResult.bSuccess) {
		clear();
		settings.clear();
		settings.load(openFileResult.filePath);
		ofSetWindowTitle(openFileResult.getName() + " - El mapa no es el territorio");

		//Preprocessors
		settings.pushTag("preprocessors");
		for (int i = 0; i < settings.getNumTags("preprocessor"); i++) {
			settings.pushTag("preprocessor", i);
			addPreprocessor(vPres, gPres, settings.getValue("name", -1));
			
			Preprocessor & curPre = vPres[vPres.size() - 1];
			string sName = ofToString(curPre.getName());
			vector <float> nV = {
				(float)settings.getValue("inMin", 0.0),
				(float)settings.getValue("inMax", 1.0),
				(float)settings.getValue("outMin", 0.0),
				(float)settings.getValue("outMax", 1.0)
			};
			string nvString = "";
			for (int j = 0; j < nV.size(); j++) {
				nvString += ofToString(nV[j]);
				if (j < nV.size() - 1) nvString += ",";
			}

			curPre.setKey(settings.getValue("key", ""));
			curPre.setNorm(settings.getValue("norm", false));
			curPre.setValues(nV[0], nV[1], nV[2], nV[3]);
			curPre.setMinMax(settings.getValue("minMax", false));
			curPre.setRoi(settings.getValue("roi", false));
			curPre.setAverage(settings.getValue("average", false));

			gPres[gPres.size() - 1]->getTextInput(sName + "-key")->setText(curPre.getKey());
			gPres[gPres.size() - 1]->getTextInput(sName + "-norm")->setText(nvString);
			gPres[gPres.size() - 1]->getToggle(sName + "-normalize")->setChecked(curPre.getNorm());
			gPres[gPres.size() - 1]->getToggle(sName + "-minMax", "settings")->setChecked(curPre.getMinMax());
			gPres[gPres.size() - 1]->getToggle(sName + "-average", "settings")->setChecked(curPre.getAverage());
			gPres[gPres.size() - 1]->getToggle(sName + "-roi", "settings")->setChecked(curPre.getRoi());

			settings.popTag();
		}
		settings.popTag();
		
		//Maps
		settings.pushTag("maps");
		for (int i = 0; i < settings.getNumTags("map"); i++) {
			settings.pushTag("map", i);
			int dimentions = settings.getNumTags("dim");
			addMap(vMaps, gMaps, dimentions, settings.getValue("name", -1));

			CCMap & curMap= vMaps[vMaps.size() - 1];
			string sName = ofToString(curMap.getName());

			for (int j = 0; j < dimentions; j++) {
				settings.pushTag("dim", j);
				curMap.setKey(j, settings.getValue("key", ""));
				curMap.setRange(j, settings.getValue("rMin", 0.0f), settings.getValue("rMax", 0.0f));
				curMap.setCrop(j, settings.getValue("crop", true));
				settings.popTag();
			}

			curMap.setAddress(settings.getValue("address", ""));
			curMap.setActive(settings.getValue("active", false));

			for (int j = 0; j < dimentions; j++) {
				string sI = ofToString(j + 1);
				gMaps[gMaps.size() - 1]->getTextInput(sName + "-key:" + sI, sI)->setText(curMap.getKeys()[j]);
				gMaps[gMaps.size() - 1]->getTextInput(sName + "-range:" + sI, sI)->setText(ofToString(curMap.getRange()[j][0]) + "," + ofToString(curMap.getRange()[j][1]));
				gMaps[gMaps.size() - 1]->getToggle(sName + "-crop:" + sI, sI)->setChecked(curMap.getCrop()[j]);
			}
			gMaps[gMaps.size() - 1]->getTextInput(sName + "-address")->setText(curMap.getAddress());
			gMaps[gMaps.size() - 1]->getToggle(sName + "-active")->setChecked(curMap.getActive());
			settings.popTag();
		}
		settings.popTag();

		//triggers
		settings.pushTag("triggers");
		for (int i = 0; i < settings.getNumTags("trigger"); i++) {
			settings.pushTag("trigger", i);
			int dimentions = settings.getNumTags("dim");
			addTrigger(vTriggers, gTrigs, dimentions, settings.getValue("name", -1));

			Trigger & curTrigger = vTriggers[vTriggers.size() - 1];
			string sName = ofToString(curTrigger.getName());
			
			for (int j = 0; j < dimentions; j++) {
				settings.pushTag("dim", j);
				curTrigger.setKey(j, settings.getValue("key", ""));
				curTrigger.setInMin(j, settings.getValue("inMin", 0.0f));
				curTrigger.setInMax(j, settings.getValue("inMax", 1.0f));
				curTrigger.setMargin(j, settings.getValue("margin", 0.0f));
				curTrigger.setInverted(j, settings.getValue("invert", false));
				settings.popTag();
			}
			curTrigger.setAddress(settings.getValue("address", ""));
			curTrigger.setActive(settings.getValue("active", true));
			
			vector<vector<float>> trigValues = curTrigger.getValues();
	
			for (int j = 0; j < trigValues.size(); j++) {
				string sValues;
				for (int k = 0; k < trigValues[j].size(); k++) {
					sValues += ofToString(curTrigger.getValues()[j][k]);
					if (k < trigValues[j].size() - 1) sValues += ",";
				}
				string sI = ofToString(j + 1);
				gTrigs[gTrigs.size() - 1]->getTextInput(sName + "-key:" + sI, sI)->setText(curTrigger.getKeys()[j]);
				gTrigs[gTrigs.size() - 1]->getTextInput(sName + "-thresh:" + sI, sI)->setText(sValues);
				gTrigs[gTrigs.size() - 1]->getTextInput(sName + "-margin:" + sI, sI)->setText(ofToString(curTrigger.getMargins()[j]));
				gTrigs[gTrigs.size() - 1]->getToggle(sName + "-invert:" + sI, sI)->setChecked(curTrigger.getInverted()[j]);
			}
			gTrigs[gTrigs.size() - 1]->getTextInput(sName + "-address")->setText(curTrigger.getAddress());
			gTrigs[gTrigs.size() - 1]->getToggle(sName + "-active")->setChecked(curTrigger.getActive());

			settings.popTag();
		}
		settings.popTag();

		//NNI
		settings.pushTag("nnints");
		for (int i = 0; i < settings.getNumTags("nnint"); i++) {
			settings.pushTag("nnint", i);
			addNNI(vNNI, gNNI, settings.getValue("name", -1));
			
			NNInt & curNNI = vNNI[vNNI.size() - 1];
			string name = ofToString(curNNI.getName());
			
			curNNI.setKeys(settings.getValue("xKey", ""), settings.getValue("yKey", ""));
			curNNI.setAddress(settings.getValue("address", ""));
			
			settings.pushTag("points");
			for (int i = 0; i < settings.getNumTags("point"); i++) {
				settings.pushTag("point", i);
				vNNI[vNNI.size() - 1].addZone(settings.getValue("x", 0.0f), settings.getValue("y", 0.0f));
				settings.popTag();
			}
			settings.popTag();
			curNNI.updateFbo();
			curNNI.setActive(settings.getValue("running", false));

			gNNI[gNNI.size() - 1]->getTextInput(name + "-keys")->setText(settings.getValue("xKey", "") + "," + settings.getValue("yKey", ""));
			gNNI[gNNI.size() - 1]->getTextInput(name + "-address")->setText(curNNI.getAddress());
			gNNI[gNNI.size() - 1]->getToggle(name + "-active")->setChecked(curNNI.getActive());
			
			settings.popTag();
		}
		settings.popTag();

		//RGB
		settings.pushTag("rgbs");
		for (int i = 0; i < settings.getNumTags("rgb"); i++) {
			settings.pushTag("rgb", i);
			addRGB(vRGB, gRGB, settings.getValue("name", -1));

			RGBInt & curRGB = vRGB[vRGB.size() - 1];
			string name = ofToString(curRGB.getName());
			
			curRGB.setKeys(settings.getValue("xKey", ""), settings.getValue("yKey", ""));
			curRGB.loadImage(settings.getValue("path", ""));
			curRGB.setAddress(settings.getValue("address", ""));
			curRGB.setSqSize(settings.getValue("size", 1));
			curRGB.setActive(settings.getValue("running", false));

			gRGB[vRGB.size() - 1]->getTextInput(name + "-keys")->setText(settings.getValue("xKey", "") + "," + settings.getValue("yKey", ""));
			gRGB[gRGB.size() - 1]->getTextInput(name + "-address")->setText(curRGB.getAddress());
			gRGB[gRGB.size() - 1]->getSlider(name + "-size")->setValue(curRGB.getSqSize());
			gRGB[gRGB.size() - 1]->getToggle(name + "-active")->setChecked(curRGB.getActive());
			settings.popTag();
		}
		settings.popTag();

		//OSC
		settings.pushTag("senders");
		for (int i = 0; i < settings.getNumTags("sender"); i++) {
			settings.pushTag("sender", i);
			string sIpPort = settings.getValue("ip", "0") + ":" + ofToString(settings.getValue("port", 0));
			addOsc();
			vOSC[vOSC.size() - 1].setup(settings.getValue("ip", "0"), settings.getValue("port", 0));
			gOsc[gOsc.size() - 1]->getTextInput(ofToString(oscNames[oscNames.size() - 1]) + "-ip:port")->setText(sIpPort);
			settings.popTag();
		}
		settings.popTag();
	}
}

void ofApp::clear() {
	//preprocessor
	vPres.clear();
	plotters.clear();
	gPres.clear();
	//triggers
	vTriggers.clear();
	gTrigs.clear();
	//maps
	vMaps.clear();
	gMaps.clear();
	//nni
	vNNI.clear();
	gNNI.clear();
	//rgb
	vRGB.clear();
	gRGB.clear();
	//osc
	vOSC.clear();
	gOsc.clear();
	oscSet.clear();
	oscNames.clear();
	//draw
	curNNI = 0;
	curRGB = 0;
	//scroll
	for (auto & scroll : mScroll) scroll.second = 0;
}

void ofApp::setMouseControl(ofxDatGuiToggleEvent e) {
	mouseControl = e.checked;
}

void ofApp::addDummy(ofxDatGuiButtonEvent e) {
	dummies.push_back(ofVec2f(0, 0));
}

void ofApp::clearDummies(ofxDatGuiButtonEvent e) {
	dummies.clear();
}

void ofApp::updateDummies(vector<ofVec2f>& dummies, float speed) {
	for (int i = 0; i < dummies.size(); i++) {
		dummies[i].x = ofNoise(ofGetElapsedTimef() * speed, (i * 2) * 1000);
		dummies[i].y = ofNoise(ofGetElapsedTimef() * speed, (i * 2 + 1) * 1000);
	}
}

void ofApp::modButton(ofxDatGuiButtonEvent e) {
	for (int i = 0; i < vModes.size(); i++) {
		if (vModes[i] == e.target->getLabel()) {
			drawMode = i;
			break;
		}
	}
	updateModButtons();
}

void ofApp::updateModButtons() {
	for (auto & mod : gMod) {
		if (mod->getLabel() == vModes[drawMode]) mod->setBackgroundColor(0);
		else mod->setBackgroundColor(100);
	}
}

void ofApp::preTextInput(ofxDatGuiTextInputEvent e) {
	string name = e.target->getName();
	int index = ofToInt(ofSplitString(name, "-")[0]);
	string prop = ofSplitString(name, "-")[1];
	string sValue = e.target->getText();

	bool bInd = false;
	for (int i = 0; i < vPres.size(); i++) {
		if (vPres[i].getName() == index) {
			index = i;
			bInd = true;
			break;
		}
	}

	if (bInd) {
		if (prop == "key") {
			vPres[index].setKey(sValue);
		}
		if (prop == "norm") {
			vector <string> values = ofSplitString(sValue, ",");
			if (values.size() == 4) vPres[index].setValues(ofToFloat(values[0]), ofToFloat(values[1]), ofToFloat(values[2]), ofToFloat(values[3]));
		}
	}
}

void ofApp::preSlider(ofxDatGuiSliderEvent e) {
	string name = e.target->getName();
	int index = ofToInt(ofSplitString(name, "-")[0]);
	string prop = ofSplitString(name, "-")[1];
	float value = e.value;
}

void ofApp::preTogInput(ofxDatGuiToggleEvent e) {
	string name = e.target->getName();
	int index = ofToInt(ofSplitString(name, "-")[0]);
	string prop = ofSplitString(name, "-")[1];
	bool bInd = false;
	bool value = e.checked;

	for (int i = 0; i < vPres.size(); i++) {
		if (vPres[i].getName() == index) {
			index = i;
			bInd = true;
			break;
		}
	}

	if (bInd) {
		if (prop == "normalize") vPres[index].setNorm(value);
		if (prop == "draw") plotters[index]->setEnabled(value);
		if (prop == "print") vPres[index].setPrint(value);
		if (prop == "minMax") vPres[index].setMinMax(value);
		if (prop == "average") vPres[index].setAverage(value);
		if (prop == "roi") vPres[index].setRoi(value);
	}
}

void ofApp::preButtonInput(ofxDatGuiButtonEvent e) {
	string name = e.target->getName();
	
	string prop = ofSplitString(name, "-")[1];
	int index = ofToInt(ofSplitString(name, "-")[0]);
	bool bInd = false;

	for (int i = 0; i < vPres.size(); i++) {
		if (vPres[i].getName() == index) {
			index = i;
			bInd = true;
			break;
		}
	}

	if (bInd) {
		if (prop == "remove") {
			vPres.erase(vPres.begin() + index);
			//disabled and invisible. Gui keeps listening even if not updated or drawn.
			gPres[index]->setEnabled(false);
			gPres[index]->setVisible(false);
			gPres.erase(gPres.begin() + index);
			plotters.erase(plotters.begin() + index);
		}
	}
}

void ofApp::mapTextInput(ofxDatGuiTextInputEvent e) {
	string name = e.target->getName();
	string prop = ofSplitString(name, "-")[1];
	prop = ofSplitString(prop, ":")[0];
	string sValue = e.target->getText();
	
	int index = ofToInt(ofSplitString(name, "-")[0]);
	bool bInd = false;
	for (int i = 0; i < vMaps.size(); i++) {
		if (vMaps[i].getName() == index) {
			index = i;
			bInd = true;
			break;
		}
	}
	
	if (bInd) {
		vector<string> params = ofSplitString(name, ":");
		if (params.size() > 1) {
			int dim = ofToInt(params[1]) - 1;
			if (prop == "key") vMaps[index].setKey(dim, sValue);
			if (prop == "range") {
				vector<string> values = ofSplitString(sValue, ",");
				if(values.size() == 2) vMaps[index].setRange(dim, ofToFloat(values[0]), ofToFloat(values[1]));
			}
		}
		if (prop == "address") vMaps[index].setAddress(sValue);
	}
}

void ofApp::mapTogInput(ofxDatGuiToggleEvent e) {
	string label = e.target->getName();
	string param = ofSplitString(label, "-")[1];
	int index = ofToInt(ofSplitString(label, "-")[0]);
	bool bInd = false;
	for (int i = 0; i < vMaps.size(); i++) {
		if (vMaps[i].getName() == index) {
			index = i;
			bInd = true;
			break;
		}
	}

	if (bInd) {
		vector<string> params = ofSplitString(label, ":");
		int dim = 0;
		if(params.size() > 1) dim = ofToInt(params[1]) - 1;
		bool value = e.checked;
		if (param == "active") vMaps[index].setActive(value);
		if (param == "crop") vMaps[index].setCrop(dim, value);
	}
}

void ofApp::mapButtonInput(ofxDatGuiButtonEvent e) {
	string label = e.target->getName();
	string param = ofSplitString(label, "-")[1];
	int index = ofToInt(ofSplitString(label, "-")[0]);
	bool bInd = false;
	for (int i = 0; i < vMaps.size(); i++) {
		if (vMaps[i].getName() == index) {
			index = i;
			bInd = true;
			break;
		}
	}

	if (bInd) {
		if (param == "remove") {
			vMaps.erase(vMaps.begin() + index);
			//disabled and invisible. Gui keeps listening even if not updated or drawn.
			gMaps[index]->setEnabled(false);
			gMaps[index]->setVisible(false);
			gMaps.erase(gMaps.begin() + index);
		}
	}
}

void ofApp::trigTextInput(ofxDatGuiTextInputEvent e){
	string name = e.target->getName();
	string prop = ofSplitString(name, "-")[1];
	prop = ofSplitString(prop, ":")[0];
	string sValue = e.target->getText();

	int index = ofToInt(ofSplitString(name, "-")[0]);
	bool bInd = false;
	for (int i = 0; i < vTriggers.size(); i++) {
		if (vTriggers[i].getName() == index) {
			index = i;
			bInd = true;
			break;
		}
	}

	if (bInd) {
		if (prop == "key") {
			int dim = ofToInt(ofSplitString(name, ":")[1]) - 1;
			vTriggers[index].setKey(dim, sValue);
		}

		if (prop == "thresh") {
			vector<string> values = ofSplitString(sValue, ",");
			vector<float> fValues;
			for (int i = 0; i < values.size(); i++) fValues.push_back(ofToFloat(values[i]));
			int dim = ofToInt(ofSplitString(name, ":")[1]) - 1;
			if (fValues.size() == 1) fValues.push_back(fValues[0]);
			if (fValues.size() == 2) vTriggers[index].setValues(dim, fValues);
		}
		if (prop == "margin") {
			int dim = ofToInt(ofSplitString(name, ":")[1]) - 1;
			cout << sValue << ", " << ofToFloat(sValue) << endl;
			vTriggers[index].setMargin(dim, ofToFloat(sValue));
		}
		if (prop == "address") vTriggers[index].setAddress(sValue);
	}
}

void ofApp::trigTogInput(ofxDatGuiToggleEvent e) {
	string label = e.target->getName();
	string param = ofSplitString(label, "-")[1];
	int index = ofToInt(ofSplitString(label, "-")[0]);
	bool bInd = false;
	for (int i = 0; i < vTriggers.size(); i++) {
		if (vTriggers[i].getName() == index) {
			index = i;
			bInd = true;
			break;
		}
	}

	if (bInd) {
		vector <string> params = ofSplitString(param, ":");
		if (params.size() > 1) {
			if (params[0] == "invert") {
				vTriggers[index].setInverted(ofToInt(params[1]) - 1, e.checked);
			}
		}
		else if (param == "active") vTriggers[index].setActive(e.target->getChecked());
	}
}

void ofApp::trigButtonInput(ofxDatGuiButtonEvent e) {
	string label = e.target->getName();
	string param = ofSplitString(label, "-")[1];
	int index = ofToInt(ofSplitString(label, "-")[0]);
	
	bool bInd = false;
	for (int i = 0; i < vTriggers.size(); i++) {
		if (vTriggers[i].getName() == index) {
			index = i;
			bInd = true;
			break;
		}
	}

	if (bInd) {
		if (param == "remove") {
			vTriggers.erase(vTriggers.begin() + index);
			//disabled and invisible. Gui keeps listening even if not updated or drawn.
			gTrigs[index]->setEnabled(false);
			gTrigs[index]->setVisible(false);
			gTrigs.erase(gTrigs.begin() + index);
		}
	}
}

void ofApp::NNIBInput(ofxDatGuiButtonEvent e) {
	string label = e.target->getName();
	string param = ofSplitString(label, "-")[1];
	int index = ofToInt(ofSplitString(label, "-")[0]);
	
	bool bInd = false;
	for (int i = 0; i < vNNI.size(); i++) {
		if (vNNI[i].getName() == index) {
			index = i;
			bInd = true;
			break;
		}
	}

	if (bInd) {
		if (param == "show") curNNI = index;
		else if (param == "clear") vNNI[index].clear();
		if (param == "remove") {
			vNNI.erase(vNNI.begin() + index);
			//disabled and invisible. Gui keeps listening even if not updated or drawn.
			gNNI[index]->setEnabled(false);
			gNNI[index]->setVisible(false);
			gNNI.erase(gNNI.begin() + index);
		}
	}
}

void ofApp::NNITogInput(ofxDatGuiToggleEvent e) {
	string label = e.target->getName();
	string param = ofSplitString(label, "-")[1];
	int index = ofToInt(ofSplitString(label, "-")[0]);
	bool bInd = false;
	for (int i = 0; i < vNNI.size(); i++) {
		if (vNNI[i].getName() == index) {
			index = i;
			bInd = true;
			break;
		}
	}

	if (bInd) {
		vector<string> params = ofSplitString(label, ":");
		int dim = 0;
		if (params.size() > 1) dim = ofToInt(params[1]) - 1;
		bool value = e.checked;
		if (param == "active") vNNI[index].setActive(value);
	}
}

void ofApp::NNITextInput(ofxDatGuiTextInputEvent e) {
	string name = e.target->getName();
	int index = ofToInt(ofSplitString(name, "-")[0]);
	string prop = ofSplitString(name, "-")[1];
	string sValue = e.target->getText();

	bool bInd = false;
	for (int i = 0; i < vNNI.size(); i++) {
		if (vNNI[i].getName() == index) {
			index = i;
			bInd = true;
			break;
		}
	}

	if (bInd) {
		if (prop == "keys") {
			vector<string> keys = ofSplitString(e.text, ",");
			if (ofSplitString(e.text, ",").size() == 2) vNNI[index].setKeys(keys[0], keys[1]);
		}
		else if (prop == "address") {
			vNNI[index].setAddress(sValue);
		}
	}
}

void ofApp::RGBBInput(ofxDatGuiButtonEvent e) {
	string label = e.target->getName();
	string param = ofSplitString(label, "-")[1];
	int index = ofToInt(ofSplitString(label, "-")[0]);

	bool bInd = false;
	for (int i = 0; i < vRGB.size(); i++) {
		if (vRGB[i].getName() == index) {
			index = i;
			bInd = true;
			break;
		}
	}

	if (bInd) {
		if (param == "show") curRGB = index;
		else if (param == "load") vRGB[index].loadImage();
		if (param == "remove") {
			vRGB.erase(vRGB.begin() + index);
			//disabled and invisible. Gui keeps listening even if not updated or drawn.
			gRGB[index]->setEnabled(false);
			gRGB[index]->setVisible(false);
			gRGB.erase(gRGB.begin() + index);
		}
	}
}

void ofApp::RGBTogInput(ofxDatGuiToggleEvent e) {
	string label = e.target->getName();
	string param = ofSplitString(label, "-")[1];
	int index = ofToInt(ofSplitString(label, "-")[0]);
	bool bInd = false;
	for (int i = 0; i < vRGB.size(); i++) {
		if (vRGB[i].getName() == index) {
			index = i;
			bInd = true;
			break;
		}
	}

	if (bInd) {
		vector<string> params = ofSplitString(label, ":");
		int dim = 0;
		if (params.size() > 1) dim = ofToInt(params[1]) - 1;
		bool value = e.checked;
		if (param == "active") vRGB[index].setActive(value);
	}
}

void ofApp::RGBTextInput(ofxDatGuiTextInputEvent e) {
	string name = e.target->getName();
	int index = ofToInt(ofSplitString(name, "-")[0]);
	string prop = ofSplitString(name, "-")[1];
	string sValue = e.target->getText();

	bool bInd = false;
	for (int i = 0; i < vRGB.size(); i++) {
		if (vRGB[i].getName() == index) {
			index = i;
			bInd = true;
			break;
		}
	}

	if (bInd) {
		if (prop == "keys") {
			vector<string> keys = ofSplitString(e.text, ",");
			if (ofSplitString(e.text, ",").size() == 2) vRGB[index].setKeys(keys[0], keys[1]);
		}
		else if (prop == "address") {
			vRGB[index].setAddress(sValue);
		}
	}
}

void ofApp::RGBSlider(ofxDatGuiSliderEvent e) {
	string name = e.target->getName();
	int index = ofToInt(ofSplitString(name, "-")[0]);
	string prop = ofSplitString(name, "-")[1];

	bool bInd = false;
	for (int i = 0; i < vRGB.size(); i++) {
		if (vRGB[i].getName() == index) {
			index = i;
			bInd = true;
			break;
		}
	}

	if (bInd) {
		if (prop == "size") vRGB[index].setSqSize(e.value);
	}
}

void ofApp::oscTextInput(ofxDatGuiTextInputEvent e) {
	string name = e.target->getName();
	string prop = ofSplitString(name, "-")[1];
	int index = ofToInt(ofSplitString(name, "-")[0]);
	string sValue = e.target->getText();

	bool bInd = false;
	for (int i = 0; i < oscNames.size(); i++) {
		if (oscNames[i] == index) {
			index = i;
			bInd = true;
			break;
		}
	}

	if (bInd) {
		if (prop == "ip:port") {
			ipPort[index] = sValue;
			vector<string> split = ofSplitString(sValue, ":");
			if (split.size() == 2) {
				if (ofSplitString(split[0], ".").size() == 4) {
					vOSC[index].setup(split[0], ofToInt(split[1]));
					oscSet[index] = true;
				}
			}
		}
	}
}

void ofApp::oscButtonInput(ofxDatGuiButtonEvent e) {
	string name = e.target->getName();
	string prop = ofSplitString(name, "-")[1];
	int index = ofToInt(ofSplitString(name, "-")[0]);

	bool bInd = false;
	for (int i = 0; i < oscNames.size(); i++) {
		if (oscNames[i] == index) {
			index = i;
			bInd = true;
			break;
		}
	}

	if (prop == "remove") {
		oscSet.erase(oscSet.begin() + index);
		oscNames.erase(oscNames.begin() + index);
		vOSC.erase(vOSC.begin() + index);
		ipPort.erase(ipPort.begin() + index);
		//disabled and invisible. Gui keeps listening even if not updated or drawn.
		gOsc[index]->setEnabled(false);
		gOsc[index]->setVisible(false);
		gOsc.erase(gOsc.begin() + index);
	}
}