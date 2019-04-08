#ifndef _CCMAP
#define _CCMAP
#include "ofMain.h"

class CCMap {
public:
	CCMap();
	void setup(int numKeys, string address = "");
	void setup(vector<string> newKeys = {}, string address = "");
	void clear();
	void setName(int value);
	void setKeys(vector<string> newKey);
	void setKey(int index, string key);
	void setAddress(string address);
	void setRange(int key, float min, float max);
	void setCrop(int key, bool value);
	void setActive(bool value);
	void update(vector<map<string, float>> values);
	vector<vector <float>> getValues();
	vector<float> getValues(int index);
	string getAddress();
	bool getActive();
	int getName();
	vector<string> getKeys();
	vector<vector<float>> getRange();
	vector<bool> getCrop();

private:
	void scale(int index, float & value, vector<float> range, bool crop);
	
	bool _active;
	int _name;
	vector<vector<float>> _ranges;
	vector<string> _keys;
	vector<vector <float>> _scaled;
	vector<bool> _crop;
	string _osc;
};

#endif