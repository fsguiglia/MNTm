#include "preprocessor.h"

Preprocessor::Preprocessor() {
	_key = "";
	_name = 0;
	_normValues = { 0,0,0,0 };
	_max = 0;
	_min = 0;
	_av = 0;
	_bNormalize = false;
	_print = false;
	_bDraw = false;
	_bMinMax = false;
	_bAv = false;
	_roi = false;
}

void Preprocessor::setName(int value) {
	_name = value;
}

void Preprocessor::setKey(string value) {
	_key = value;
}

void Preprocessor::setPrint(bool value) {
	_print = value;
}

void Preprocessor::setDraw(bool value) {
	_bDraw = value;
}

void Preprocessor::setNorm(bool value) {
	_bNormalize = value;
}

void Preprocessor::setRoi(bool value) {
	_roi = value;
}

void Preprocessor::setMinMax(bool value) {
	_bMinMax = value;
}

void Preprocessor::setAverage(bool value) {
	_bAv = value;
}

void Preprocessor::setValues(float inMin, float inMax, float outMin, float outMax) {
	_normValues.clear();
	_normValues = { inMin, inMax, outMin, outMax };
}

void Preprocessor::normalize(float & value) {
	if (_normValues[0] != _normValues[1]) {
		value = (value - _normValues[0]) / (_normValues[1] - _normValues[0]);
		value *= (_normValues[3] - _normValues[2]);
		value += _normValues[2];
	}

	if (value > 1) value = 1;
	if (value < 0) value = 0;
}

vector <float> Preprocessor::update(vector<map<string, float>> & values, map<string, float> & procMap) {
	int matches = 0;
	_newValues.clear();
	_keyValues.clear();
	vector<float> printValues;

	for (int i = 0; i < values.size(); i++) {
		map<string, float>::iterator val = values[i].find(_key);
		if (val != values[i].end()) {
			_currValue = val->second;
			bool inRoi = true;
			if (_roi) inRoi = _currValue > _normValues[0] && _currValue < _normValues[1];
			if (_bNormalize) normalize(_currValue);
			if (inRoi) {
				matches += 1;
				//store all values for matched keys for averaging, etc.
				val->second = _currValue;
				_keyValues.push_back(_currValue);
				_newValues.push_back(values[i]);
			}
		}
	}
	if (matches > 0) {
		if (_bMinMax) {
			_max = NULL;
			_min = NULL;
			for (int i = 0; i < _keyValues.size(); i++) {
				if (_keyValues[i] < _min || _min == NULL) _min = _keyValues[i];
				if (_keyValues[i] > _max || _max == NULL) _max = _keyValues[i];
			}
			procMap[_key + "Max"] = _max;
			procMap[_key + "Min"] = _min;
		}
		if (_bAv) {
			float sum = 0;
			for (int i = 0; i < _keyValues.size(); i++) sum += _keyValues[i];
			_av = sum / matches;
			procMap[_key + "Av"] = _av;
		}
	}
	values = _newValues;
	return _keyValues;
}

void Preprocessor::draw() {
//instead of ofxDat plotter
}

string Preprocessor::getKey() {
	return _key;
}

bool Preprocessor::getPrint() {
	return _print;
}

float Preprocessor::getValue() {
	return _currValue;
}

int Preprocessor::getName() {
	return _name;
}

bool Preprocessor::getDraw() {
	return _bDraw;
}

bool Preprocessor::getNorm() {
	return _bNormalize;
}

bool Preprocessor::getRoi() {
	return _roi;
}

bool Preprocessor::getMinMax() {
	return _bMinMax;
}

bool Preprocessor::getAverage() {
	return _bAv;
}

vector<float> Preprocessor::getValues() {
	return _normValues;
}
