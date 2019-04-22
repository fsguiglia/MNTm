/*
trigger was written for a scene with multiple performers but with only one relevant key, and a scene with only one performer and multiple relevant keys
check if it works with multiple performers and multiple keys. it shouldn't.
*/

#ifndef _TRIGGER
#define _TRIGGER
#include "ofMain.h"

class Trigger {
public:
	Trigger();
	void setup(vector<string> keys, string address = "");
	void setup(int numKeys, string address = "");
	void setValues(int dimention, vector<float> values);
	void setMargin(int dimention, float margin);
	void setInverted(int dimention, bool value);
	void setName(int value);
	void setKey(int dimention, string value);
	void setInMin(int dimention, float value);
	void setInMax(int dimention, float value);
	void setAddress(string value);
	void setActive(bool value);
	void clear();
	int update(vector<std::map<string, float>> values);
	vector<string> getKeys();
	vector<bool> getInverted();
	int getName();
	int getChange();
	vector<float> getMargins();
	bool getState();
	string getAddress();
	bool getActive();
	vector <vector<float>> getValues();
private:
	int _numDim, _changed, _name;
	bool _triggered, _active;
	string _osc;
	vector<bool> _invert;
	vector<string> _sKeys;
	vector<vector<float>> _pos;
	vector<float> _margin;
};

#endif