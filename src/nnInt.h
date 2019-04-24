#ifndef _NNINT
#define _NNINT
#include "ofMain.h"
#include "ofxVoronoi.h"

class NNInt {
public:
	NNInt();
	void setup(string xKey, string yKey, int w = 200, int h = 200);
	void setName(int value);
	ofRectangle getBounds();
	void setKeys(string xKey, string yKey);
	void setActive(bool active = false);
	bool getActive();
	void setAddress(string value);
	string getAddress();
	void addZone(float x, float y);
	void addZone(glm::vec3 newPoint);
	void previewZone(float x, float y);
	void previewZone(glm::vec3 newPoint);
	void updateFbo();
	int getName();
	void updateWeights(vector<map<string, float>> & values, int step = 6);
	vector<vector<float>> getWeights();
	void draw(int x, int y);
	void clear();
	vector<string> getKeys();
	vector<glm::vec3> getPoints();
	vector<glm::vec3> getNPoints();

private:
	void fboDraw();
	
	int _selZone, _name;
	bool _go, _prevGo, _bTempPoint;
	string _osc;
	ofRectangle _bounds;
	ofPoint _tempPoint, _trans;
	ofFbo _fbo;
	ofPixels _fboPixels;
	vector<int> _scale;
	vector<string> _keys;
	vector<vector<float>> _fWeights;
	vector<glm::vec3> _points, _nPoints;
	ofxVoronoi _vZones, _vWeights;
};

#endif
