#ifndef _PREPROCESSOR
#define _PREPROCESSOR
#include "ofMain.h"

class Preprocessor {
public:
	Preprocessor();
	void setName(int value);
	void setKey(string value);
	void setPrint(bool value);
	void setDraw(bool value);
	void setNorm(bool value);
	void setRoi(bool value);
	void setMinMax(bool value);
	void setAverage(bool value);
	void setValues(float inMin, float inMax, float outMin, float outMax);
	vector<float> update(vector<map<string, float>> & values, map< string, float> & procMap);
	void draw();
	string getKey();
	bool getPrint();
	float getValue();
	int getName();
	bool getDraw();
	bool getNorm();
	bool getRoi();
	bool getMinMax();
	bool getAverage();
	vector<float> getValues();

private:
	void normalize(float & value);
	int _name;
	vector<map<string, float>> _newValues;
	vector<float> _keyValues;
	vector<float> _normValues;
	string _key;
	float _currValue, _max, _min, _av;
	bool _bNormalize, _clip, _bDraw, _print, _bMinMax , _bAv, _roi;
};

#endif