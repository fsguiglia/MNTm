#ifndef _RGBINT
#define _RGBINT
#include "ofMain.h"

class RGBInt {
public:
	RGBInt();
	void setup(string xKey, string yKey, int w = 200, int h = 200);
	void loadImage();
	void loadImage(string path);
	string getPath();
	void setName(int name);
	int getName();
	void setSqSize(int sqSize);
	int getSqSize();
	void setKeys(string xKey, string yKey);
	vector<string> getKeys();
	void setActive(bool active = false);
	bool getActive();
	void setAddress(string osc);
	string getAddress();
	void draw(int x = 0, int y = 0);
	void updateWeights(vector<map<string, float>> & values);
	vector<vector<float>> getWeights();
	
private:
	bool _active;
	string _osc, _imgPath;
	int _name, _sqSize;
	vector <string> _keys;
	vector <int> _maxSize;
	vector <float> _size;
	ofImage _image;
	ofPixels _pixels;
	vector<vector<float>> _weights;
	ofFbo _fbo;
};

#endif