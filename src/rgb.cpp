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

void RGBInt::updateWeights(vector<map<string, float>> values) {
	if (_image.isAllocated()) {
		ofPoint vPoint = { 0, 0 };
		bool bKeys[] = { false, false };
		_weights = { 0,0,0 };
		int radius = ceil(_sqSize * 0.5);
		//only 2d for now
		for (int i = 0; i < values.size(); i++) {
			for (int j = 0; j < _keys.size(); j++) {
				map<string, float>::iterator val = values[i].find(_keys[j]);
				if (val != values[i].end()) vPoint[j] = val->second * _size[j];
				if (vPoint[j] < radius) vPoint[j] = radius;
				if (vPoint[j] > _size[j] - radius) vPoint[j] = _size[j] - radius;
				bKeys[j] = bKeys[j] || val != values[i].end();
			}
		}

		if (bKeys[0] && bKeys[1]) {
			int samples = 0;
			for (int i = 0; i < _sqSize; i++) {
				for (int j = 0; j < _sqSize; j++) {
					samples++;
					ofColor curColor = _image.getColor(vPoint[0] - radius + i, vPoint[1] - radius + j);
					for (int k = 0; k < _weights.size(); k++) {
						_weights[k] += curColor[k];
					}
				}
			}

			for (auto & weight : _weights) weight /= (float)samples;
		}

		_fbo.begin();
		ofClear(0);
		_image.draw(0, 0);
		if (bKeys[0] && bKeys[1]) {
			ofPushStyle();
			ofSetColor(0);
			ofDrawLine(vPoint.x, 0, vPoint.x, _size[1]);
			ofDrawLine(0, vPoint.y, _size[0], vPoint.y);
			ofDrawRectangle(vPoint.x - radius - 1, vPoint.y - radius - 1, _sqSize + 2, _sqSize + 2);
			ofSetColor(_weights[0], _weights[1], _weights[2]);
			ofDrawRectangle(vPoint.x - radius, vPoint.y - radius, _sqSize, _sqSize);
			ofPopStyle();
		}
		_fbo.end();
	}
}

vector<float> RGBInt::getWeights() {
	return _weights;
}
