#include "nnInt.h"

NNInt::NNInt(){
}

void NNInt::setup(string xKey, string yKey, int w, int h) {
	_keys = { xKey, yKey };
	_go = false;
	_prevGo = false;
	_bTempPoint = false;
	_fbo.allocate(w, h, GL_RGB);
	_fbo.begin();
	ofClear(255, 255, 255);
	_fbo.end();
	_fboPixels.allocate(_fbo.getWidth(), _fbo.getHeight(), GL_RGB);
	_bounds = ofRectangle(0, 0, w, h);
	_scale = { w, h };
	_osc = "";
	_selZone = 0;
}

void NNInt::setName(int value) {
	_name = value;
}

ofRectangle NNInt::getBounds(){
	return _bounds;
}

void NNInt::setKeys(string xKey, string yKey) {
	_keys[0] = xKey;
	_keys[1] = yKey;
}

void NNInt::setActive(bool active) {
	if (!active && _go) updateFbo();
	if(active && !_go) _fbo.readToPixels(_fboPixels);
	_go = active;
}

bool NNInt::getActive() {
	return _go;
}

void NNInt::setAddress(string value) {
	_osc = value;
}

string NNInt::getAddress(){
	return _osc;
}

void NNInt::addZone(float x, float y) {
	_nPoints.push_back(glm::vec3(x, y, 0));
	x *= _bounds.width;
	y *= _bounds.height;
	_points.push_back(glm::vec3((int)x, (int)y, 0));
	_bTempPoint = false;
}

void NNInt::addZone(glm::vec3 newPoint) {
	_nPoints.push_back(newPoint);
	newPoint.x *= _bounds.width;
	newPoint.y *= _bounds.width;
	_points.push_back(newPoint);
	_bTempPoint = false;
}

void NNInt::previewZone(float x, float y) {
	_bTempPoint = true;
	x *= _bounds.width;
	y *= _bounds.height;
	_tempPoint.set((int)x, (int)y);
}

void NNInt::previewZone(glm::vec3 newPoint) {
	_bTempPoint = true;
	newPoint.x *= _bounds.width;
	newPoint.y *= _bounds.width;
	_tempPoint = newPoint;
}

void NNInt::updateFbo() {
	if (_points.size() > 0) {
		_vZones.clear();
		_vZones.setBounds(_bounds);
		_vZones.addPoints(_points);
		if (_bTempPoint) _vZones.addPoint(_tempPoint);
		_vZones.generate();
		fboDraw();
	}
}

int NNInt::getName() {
	return _name;
}

bool NNInt::updateWeights(vector<map<string, float>> values, int step) {
	bool newValue = false;
	if (_go) {
		ofPoint vPoint = { 0, 0 };
		bool bKeys[] = { false, false };
		int numSamples = 0;
		_fWeights.clear();
		for (int i = 0; i < _points.size(); i++) {
			_fWeights.push_back(0);
		}

		//only 2d for now
		for (int i = 0; i < values.size(); i++) {
			for (int j = 0; j < _keys.size(); j++) {
				map<string, float>::iterator val = values[i].find(_keys[j]);
				if (val != values[i].end()) vPoint[j] = int(val->second * _scale[j]);
				if (vPoint[j] < 5) vPoint[j] = 5;
				if (vPoint[j] > _scale[j] - 5) vPoint[j] = _scale[j] - 5;
				bKeys[j] = bKeys[j] || val != values[i].end();
			}
		}

		vPoint[0] = (int)(_scale[0] - vPoint[0]);

		if (_points.size() > 1 && bKeys[0] && bKeys[1]) {
			newValue = true;
			_vWeights.clear();
			_vWeights.setBounds(_bounds);
			_vWeights.addPoints(_points);
			_vWeights.addPoint(vPoint);
			_vWeights.generate();

			/*
			me quedo con el canal rojo. 254 es el mayor valor. 255 corresponde al negro del borde o el
			rojo del tag. inside es muy lento. buscar una mejor solucion.
			*/
			
			ofPolyline poly;
			vector<ofxVoronoiCell> cells = _vWeights.getCells();
			poly.addVertices(cells[cells.size() - 1].points);
			poly.close();
			ofRectangle polyRect = poly.getBoundingBox();
			
			int downW = polyRect.width - ((int)polyRect.width % step);
			int downH = polyRect.height - ((int)polyRect.height % step);

			for (int i = 0; i < downW; i += step) {
				for (int j = 0; j < downH; j += step) {
					int xIn = i + polyRect.getMinX();
					int yIn = j + polyRect.getMinY();
					if (poly.inside(xIn, yIn)) {
						int zone = 254 - (int)_fboPixels.getColor(xIn, yIn).r;
						if (zone >= 0 && zone < _fWeights.size()) {
							_fWeights[zone] += 1;
							numSamples += 1;
						}
					}
				}
			}
			float maxWeight = -1;
			for (int i = 0; i < _fWeights.size(); i++) {
				_fWeights[i] /= numSamples;
				if (_fWeights[i] > maxWeight) {
					_selZone = i;
					maxWeight = _fWeights[i];
				}
			}
		}
	}
	fboDraw();

	return newValue;
}

vector<float> NNInt::getWeights() {
	return _fWeights;
}

void NNInt::draw(int x, int y){
	_fbo.draw(x, y);
}

void NNInt::clear() {
	_points.clear();
	_nPoints.clear();
	_vZones.clear();
	_vWeights.clear();
	fboDraw();
}

vector<string> NNInt::getKeys() {
	return _keys;
}

vector<glm::vec3> NNInt::getPoints() {
	return _points;
}

vector<glm::vec3> NNInt::getNPoints() {
	return _nPoints;
}

void NNInt::fboDraw() {
	//esto no tendria que pasar una vez por ciclo, para algo tengo el fbo
	_fbo.begin();
	if (!_go) ofBackground(255);
	ofDrawRectangle(_bounds);
	if (_points.size() > 0) {
		vector<ofxVoronoiCell> cells = _vZones.getCells();
		for (int i = 0; i < cells.size(); i++) {
			ofFill();
			//255 coincidiria con el negro de los bordes y el rojo de las etiquetas
			ofSetColor(255 - (i + 1));
			ofBeginShape();
			for (int j = 0; j < cells[i].points.size(); j++) {
				ofVertex(cells[i].points[j]);
			}
			ofEndShape();
			ofSetColor(0);
			ofPolyline poly;
			poly.addVertices(cells[i].points);
			poly.close();
			poly.draw();
			ofSetColor(255, 0, 0);
			ofFill();
			if (i < _points.size()) ofDrawBitmapString(ofToString(i + 1), _points[i]);

			if (_go) {
				vector<ofxVoronoiCell> cells = _vWeights.getCells();
				if (cells.size() > 0) {
					ofSetColor(255, 0, 0);
					ofPolyline poly;
					poly.addVertices(cells[cells.size() - 1].points);
					poly.close();
					poly.draw();
				}
				ofSetColor(0);
				ofDrawBitmapString(ofToString(_selZone + 1), 10, _fbo.getHeight() - 10);
			}
		}
	}
	_fbo.end();
}
