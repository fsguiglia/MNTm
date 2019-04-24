#include "rgb.h"

RGBInt::RGBInt() {
	_sqSize = 5;
	_active = false;
}

void RGBInt::setup(string xKey, string yKey, int w, int h) {
	_keys = { xKey, yKey };
	_maxSize = { w, h };;
	_size = { (float)w, (float)h };
	_fbo.allocate(w, h, GL_RGB);
	_fbo.begin();
	ofClear(255, 255, 255);
	_fbo.end();
}

void RGBInt::loadImage() {
	ofFileDialogResult openFileResult = ofSystemLoadDialog("load image", false);
	if (openFileResult.bSuccess) loadImage(openFileResult.filePath);
}

void RGBInt::loadImage(string path) {
	_imgPath = path;
	_image.load(_imgPath);

	int newW = _maxSize[0];
	int newH = _maxSize[1];
	if (_image.getWidth() > 400 || _image.getHeight() > 400) {
		if (_image.getWidth() > _image.getHeight()) newH = _image.getHeight() * _maxSize[0] / _image.getWidth();
		else newW = _image.getWidth() * _maxSize[1] / _image.getHeight();
	}
	_image.resize(newW, newH);
	_fbo.clear();
	_fbo.allocate(newW, newH, GL_RGB);
	_fbo.begin();
	ofClear(255, 255, 255);
	_fbo.end();
	_size = { _image.getWidth(), _image.getHeight() };
	_fbo.begin();
	ofClear(0);
	_image.draw(0, 0);
	_fbo.end();

}

string RGBInt::getPath() {
	return _imgPath;
}

void RGBInt::setName(int name) {
	_name = name;
}

int RGBInt::getName() {
	return _name;
}

void RGBInt::setSqSize(int sqSize) {
	_sqSize = sqSize;
}

int RGBInt::getSqSize() {
	return _sqSize;
}

void RGBInt::setKeys(string xKey, string yKey) {
	_keys = { xKey, yKey };
}

vector<string> RGBInt::getKeys() {
	return _keys;
}

void RGBInt::setActive(bool active) {
	_active = active;
}

bool RGBInt::getActive() {
	return _active;
}

void RGBInt::setAddress(string osc) {
	_osc = osc;
}

string RGBInt::getAddress() {
	return _osc;
}

void RGBInt::draw(int x, int y) {
	_fbo.draw(x, y);
}

void RGBInt::updateWeights(vector<map<string, float>> & values) {
	_weights.clear();
	if (_image.isAllocated()) {
		int radius = ceil(_sqSize * 0.5);
		bool drawn = false;
		//only 2d for now
		for (auto & value : values) {
			bool bKeys[] = { false, false };
			ofPoint vPoint = { 0, 0 };
			vector <float> curWeights = { 0,0,0 };

			for (int i = 0; i < _keys.size(); i++) {
				map<string, float>::iterator val = value.find(_keys[i]);
				if (val != value.end()) vPoint[i] = val->second * _size[i];
				if (vPoint[i] < radius) vPoint[i] = radius;
				if (vPoint[i] > _size[i] - radius) vPoint[i] = _size[i] - radius;
				bKeys[i] = bKeys[i] || val != value.end();
			}

			if (bKeys[0] && bKeys[1]) {
				int samples = 0;
				for (int i = 0; i < _sqSize; i++) {
					for (int j = 0; j < _sqSize; j++) {
						samples++;
						ofColor curColor = _image.getColor(vPoint[0] - radius + i, vPoint[1] - radius + j);
						for (int k = 0; k < curWeights.size(); k++) curWeights[k] += curColor[k];
					}
				}
				for (int i = 0; i < curWeights.size(); i++) {
					curWeights[i] /= (float)samples;
					curWeights[i] /= 255;
					value["rgb" + ofToString(_name) + "-" + ofToString(i+1)] = curWeights[i];
				}
				_weights.push_back(curWeights);

				if (!drawn) {
					_fbo.begin();
					ofClear(0);
					_image.draw(0, 0);
					if (bKeys[0] && bKeys[1]) {
						ofPushStyle();
						ofSetColor(0);
						ofDrawLine(vPoint.x, 0, vPoint.x, _size[1]);
						ofDrawLine(0, vPoint.y, _size[0], vPoint.y);
						ofDrawRectangle(vPoint.x - radius - 1, vPoint.y - radius - 1, _sqSize + 2, _sqSize + 2);
						ofSetColor(_weights[0][0] * 255, _weights[0][1] * 255, _weights[0][2] * 255);
						ofDrawRectangle(vPoint.x - radius, vPoint.y - radius, _sqSize, _sqSize); 
						ofSetColor(255, 75);
						ofDrawRectangle(0, _fbo.getHeight() - 20, _fbo.getWidth(), 20);
						ofSetColor(0);
						string sColor = ofToString(_weights[0][0]) + ", " + ofToString(_weights[0][1]) + ", " + ofToString(_weights[0][2]);
						ofDrawBitmapString(sColor, 10, _fbo.getHeight() - 5);
						ofPopStyle();
					}
					_fbo.end();
					drawn = true;
				}
			}
		}
	}
}

vector<vector<float>> RGBInt::getWeights() {
	return _weights;
}
