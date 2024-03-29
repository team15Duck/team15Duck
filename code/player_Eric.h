#pragma once
#include "playerBase.h"


class player_Eric : public playerBase
{
private:

	int _aniCount;
	int _ladderIndex;		//사다리인덱스

	float _jumpPower;

	bool _isJump;			//점프 체크용

	bool _isladderup;		//사다리 올라가는 중
	bool _isladderdown;		//사다리 내려가는 중
	bool _isUsladder;		//올라갈때시작점 사다리체크
	bool _isUeladder;		//올라갈때끝점 사다리 체크
	bool _isDsladder;		//내려올때 시작점 사다리체크
	bool _isDeladder;		//내려올때 끝점 사다리체크
	bool _isladderUse;		//자네 지금 사다리 사용중인가? 매달려있는가?

	bool _isRCollision;		//오른쪽 충돌 체크용 
	bool _isLCollision;		//왼쪽 충돌 체크용
	bool _isBCollision;		//바닥 충돌 체크용
	bool _isFCollision;		//죽는 바닥 충돌 체크용

	//이동 예외처리용 불값
	bool _isRightMove;		//오른쪽이동
	bool _isLeftMove;		//왼쪽이동
	bool _isUpMove;			//위로이동
	bool _isDownMove;		//아래로 이동

	//스킬 확인용 불값
	bool _isRSkill;
	bool _isLSkill;
	bool _isSign;


	POINT _size;

	animation* _EricMotion;	//에릭모션
	object* _ladder;


public:
	player_Eric();
	~player_Eric();

	HRESULT init();
	void release();
	void update();
	void render();

	//얘네는 공통적으로 작동되는거 이외에 작성하면됨
	virtual void keyPressMove()		override;
	virtual void keyPressSpace()	override;
	virtual void keyPressD()		override;
	virtual void keyPressE()		override;
	virtual void keyPressS()		override;



	//좌우체크
	virtual void rectBrokenWallCollision()			override; // << 렉트 부숴지는 벽 충돌 
	//위에벽체크(천장) 
	virtual void pixelTopWallCollision()			override; // 픽셀 천장 충돌 
	//바닥체크
	virtual void pixelBottomCollision()				override; // << 바닥 픽셀 충돌 

	void jumpRightPixelCollision();
	void jumpLeftPixelCollision();
	//죽는 함수	
	void pixelDieCollision();

	//초기화 함수
	void ericInit();
	void collisioninit();
	//이동관련 함수
	void upMove(object * ladder);
	void downMove(object * ladder);

	void jump();

	//애니메이션관련 함수
	void EricAniinit();
	void EricAni();
	void EricAniStart(string key);

	//몬스터와 충돌했을 경우 생명 깎는 함수
	virtual void takeDamage(int damage) override;


};

