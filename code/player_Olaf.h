#pragma once
#include "playerBase.h"
class player_Olaf :	public playerBase
{
private:
	RECT _shield;			//옆일때 방패
	image* _pixelShield;	//방패를 위로 들었을때 픽셀충돌용

	bool _isShieldUp;		//방패를 위로 들고있는지?

	float _shieldX;			//방패 X좌표
	float _shieldY;			//방패 Y좌표

public:
	player_Olaf();
	~player_Olaf();

	HRESULT init();
	void release();
	void update();
	
	void render();

	// ========== 함수추가 ==========

	//올라프 초기화
	void initOlaf();

	//방패 초기화
	void initShield();	

	//방패 상태
	void stateShield();


	// ========== 충돌함수 ==========

	//좌우벽충돌(픽셀용)
	virtual void pixelHorizenWallCollision() override;
	
	//좌우벽충돌(렉트용)
	virtual void rectBrokenWallCollision() override;
	
	//바닥충돌
	virtual void pixelBottomCollision() override;


	// ========== 키입력 ==========

	virtual void keyPressMove() override;
	virtual void keyPressSpace() override;
	virtual void keyPressD() override;




	// ========== GET & SET ==========

	





};

