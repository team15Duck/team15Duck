#pragma once
#include "gameNode.h"


class testScene : public gameNode
{
private:

	image* _pixelMap;			//여기다가 계속 맵데이터를 복사할것임 여기서 픽셀정보를 가져와서 판정할것.



public:
	testScene();
	~testScene();

	HRESULT init();
	void release();
	void update();
	void render();
};