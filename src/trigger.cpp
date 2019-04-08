#include "trigger.h"

Trigger::Trigger() {
	_name = -1;
	_numDim = 0;
	_changed = -1;
	_triggered = false;
	_active = false;
}

void Trigger::setup(vector<string> keys, string address) {
	_sKeys.clear();
	_pos.clear();
	_invert.clear();

	for (int i = 0; i < keys.size(); i++) {
		vector<float> empty = { 0,0,0,0 };
		_sKeys.push_back(keys[i]);
		_pos.push_back(empty);
		_invert.push_back(false);
	}
	_numDim = keys.size();
	_osc = address;
}

void Trigger::setup(int numKeys, string address) {
	_sKeys.clear();
	_pos.clear();
	_invert.clear();

	for (int i = 0; i < numKeys; i++) {
		vector<float> empty = { 0,0,0,0 };
		_sKeys.push_back("");
		_pos.push_back(empty);
		_invert.push_back(false);
	}
	_numDim = numKeys;
	_osc = address;
}

void Trigger::setValues(int dimention, vector<float> values) {
	if (dimention < _numDim && values.size() == 4) _pos[dimention] = values;
}

void Trigger::setInverted(int dimention, bool value) {
	if(dimention < _numDim) _invert[dimention] = value;
}

void Trigger::setName(int value) {
	_name = value;
}

void Trigger::setKey(int dimention, string value) {
	if (dimention < _numDim) _sKeys[dimention] = value;
}

void Trigger::setInMin(int dimention, float value) {
	if (dimention < _numDim) _pos[dimention][0] = value;
}

void Trigger::setInMax(int dimention, float value) {
	if (dimention < _numDim) _pos[dimention][1] = value;
}

void Trigger::setOutMin(int dimention, float value) {
	if (dimention < _numDim) _pos[dimention][2] = value;
}

void Trigger::setOutMax(int dimention, float value) {
	if (dimention < _numDim) _pos[dimention][3] = value;
}

void Trigger::setOutRatio(int dimention, float value) {
	//thresholds as ratio
}

void Trigger::setAddress(string value) {
	_osc = value;
}

void Trigger::setActive(bool value) {
	_active = value;	
}

void Trigger::clear(){
	_sKeys.clear();
	_pos.clear();
	_numDim = 0;
	_triggered = false;
}

int Trigger::update(vector<map<string, float>> values){
	_changed = -1;
	
	if (!_triggered) {
		for (auto curMap : values) {
			bool inside = true;
			for (int i = 0; i < _sKeys.size(); i++) {
				map<string, float>::iterator member = curMap.find(_sKeys[i]);
				if (member != curMap.end()) {
					bool curInside = false;
					if (_invert[i]) curInside = member->second < _pos[i][2] || member->second > _pos[i][3];
					else curInside = member->second > _pos[i][0] && member->second < _pos[i][1];
					inside = inside && curInside;
				}
				else {
					inside = false;
					break;
				}
			}
			if (inside) {
				_changed = 1;
				_triggered = true;
				break;
			}
		}
	}
	else {
		bool outside = true;
		for (auto curMap : values) {
			bool mapOutside = false;
			for (int i = 0; i < _sKeys.size(); i++) {
				map<string, float>::iterator member = curMap.find(_sKeys[i]);
				if (member != curMap.end()) {
					bool keyOutside;
					if (_invert[i]) keyOutside = member->second > _pos[i][0] && member->second < _pos[i][1];
					else keyOutside = member->second < _pos[i][2] || member->second > _pos[i][3];
					mapOutside = mapOutside || keyOutside;
				}
				else {
					mapOutside = true;
					break;
				}
			}
			if (!mapOutside) {
				outside = false;
				break;
			}
		}
		if (outside) {
			_changed = 0;
			_triggered = false;
		}
	}

	return _changed;
}

vector<string> Trigger::getKeys() {
	return _sKeys;
}

vector<bool> Trigger::getInverted() {
	return _invert;
}

int Trigger::getName() {
	return _name;
}

int Trigger::getChange(){
	return _changed;
}

bool Trigger::getState()
{
	return _triggered;
}

string Trigger::getAddress(){
	return _osc;
}

bool Trigger::getActive() {
	return _active;
}

vector<vector<float>> Trigger::getValues() {
	return _pos;
}


