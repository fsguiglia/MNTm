#include "ccMap.h"

CCMap::CCMap(){
	_keys = { "" };
	_ranges = { {0, 1} };
	_crop = { true };
	_scaled = { { 0 } };
	_name = -1;
	_active = false;
}

void CCMap::setup(int numKeys, string address) {
	_keys.clear();
	_keys.assign(numKeys, "");
	_osc = address;
	_ranges.clear();
	vector <float> initRange = { 0, 1 };
	_ranges.assign(numKeys, initRange);
	_crop.clear();
	_crop.assign(numKeys, true);
}

void CCMap::setup(vector<string> newKeys, string address){
	_keys = newKeys;
	_osc = address;
	_ranges.clear();
	vector <float> initRange = { 0, 1 };
	_ranges.assign(_keys.size(), initRange);
	_crop.clear();
	_crop.assign(_keys.size(), true);
}

void CCMap::clear() {
	_keys = { "" };
	_scaled = { { 0 } };
	_ranges = { {0,1} };
	_crop = { true };
	_name = -1;
	_active = false;
}

void CCMap::setName(int value) {
	_name = value;
}

void CCMap::setKeys(vector<string> newKeys) {
	_keys = newKeys;
}

void CCMap::setKey(int index, string key) {
	_keys[index] = key;
}

void CCMap::setAddress(string address) {
	_osc = address;
}

void CCMap::setRange(int key, float min, float max) {
	_ranges[key] = { min,max };
}

void CCMap::setCrop(int key, bool value) {
	_crop[key] = value;
}

void CCMap::setActive(bool value) {
	_active = value;
}

void CCMap::update(vector<map<string, float>> values) {
	_scaled.clear();
	if (_active) {
		for (int i = 0; i < values.size(); i++) {
			vector <float> newValues;
			newValues.assign(_keys.size(), NULL);
			bool keyFound = true;
			for (int j = 0; j < _keys.size(); j++) {
				map<string, float>::iterator val = values[i].find(_keys[j]);
				keyFound = keyFound && val != values[i].end();
				if (val != values[i].end()) {
					float value = val->second;
					scale(j, value, _ranges[j], _crop[j]);
					newValues[j] = value;
				}
			}
			if(keyFound) _scaled.push_back(newValues);
		}
	}
}

vector<vector<float>> CCMap::getValues(){
	return _scaled;
}

vector<float> CCMap::getValues(int index) {
	return _scaled[index];
}

string CCMap::getAddress() {
	return _osc;
}

bool CCMap::getActive() {
	return _active;
}

int CCMap::getName() {
	return _name;
}

vector<string> CCMap::getKeys() {
	return _keys;
}

vector<vector <float>> CCMap::getRange() {
	return _ranges;
}

vector<bool> CCMap::getCrop() {
	return _crop;
}

void CCMap::scale(int index, float & value, vector<float> range, bool crop) {
	/*
	switch (_curves[index]) {
	}
	*/
	if (range[0] != range[1]) {
		value = (range[1] - range[0]) * value;
		value += range[0];
		if (crop) {
			if (value < range[0]) value = range[0];
			if (value > range[1]) value = range[1];
		}
	}
}