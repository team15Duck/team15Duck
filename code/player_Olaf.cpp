#include "stdafx.h"
#include "player_Olaf.h"
#include "objectManager.h"
#include "mainUI.h"

player_Olaf::player_Olaf()
	:_isAniStart(false)
{
}

player_Olaf::~player_Olaf()
{
}

HRESULT player_Olaf::init()
{
	playerBase::init();
	//초기화
	initOlaf();
	initShield();
	initImgOlaf();

	_specialTime = 0;

	return S_OK;
}

void player_Olaf::release()
{
}

void player_Olaf::update()
{
	_mainUI->setOlafUIHP(_lifeCount);
	//애니메이션업데이트
	KEYANIMANAGER->update("player_Olaf");
	specialMotion_Olaf();

	//충돌처리
	stateOlaf();
	pixelBottomCollision();
	pixelFireCollision();
	playerCollisionLadder();

	//갱신
	_playerRect = RectMakeCenter(_x, _y, 70, 70);
	_proveBottom = _playerRect.bottom;
	_proveRight = _playerRect.right;
	_proveLeft = _playerRect.left;
	_specialTime += 90 * TIMEMANAGER->getElpasedTime();

	//방패갱신
	stateShield();
}

void player_Olaf::render()
{
	if (_isAlive)
		_player->aniRender(CAMERA->getMemDC(), _playerRect.left, _playerRect.top, _olafMotion);

	//테슷흐
	if (KEYMANAGER->isToggleKey(VK_F9))
	{
		Rectangle(CAMERA->getMemDC(), _playerRect, false);
		Rectangle(CAMERA->getMemDC(), _shieldRect, false);
	}
	if (KEYMANAGER->isToggleKey(VK_F9))
	{
		char str[256];
		SetTextColor(CAMERA->getMemDC(), RGB(255, 255, 255));
		sprintf_s(str, "speed : %.1f", _speed);
		TextOut(CAMERA->getMemDC(), 0, 200, str, strlen(str));
		sprintf_s(str, "time: %.1f", _specialTime);
		TextOut(CAMERA->getMemDC(), 0, 150, str, strlen(str));
	}
}

void player_Olaf::shieldRender(HDC hdc)
{
	if (_isShieldUp && _isFloor && !_isLadder)
		RectangleBrushPen(hdc, _shieldRect, RGB(255, 0, 255), true);
}

void player_Olaf::keyPressMove()
{
	if (!_deathMotion)
	{
		if (!_isShieldUp)
		{
			if (!_isLadder)
			{
				noLadder_KeyRight();
				noLadder_KeyLeft();
			}
			else
			{
				ladder_KeyUp();
				ladder_KeyDown();
				ladder_KeyRight();
				ladder_KeyLeft();
			}
		}
		else if (_isShieldUp)
		{
			if (!_isLadder)
			{
				if (_state != PLAYER_SHIELD_FALL_RIGHT && _state != PLAYER_SHIELD_FALL_LEFT &&
					_state != PLAYER_SHIELD_FALL_MOVE_RIGHT && _state != PLAYER_SHIELD_FALL_MOVE_LEFT)
				{
					noFall_Shield_KeyRight();
					noFall_Shield_KeyLeft();
				}
			}
			else
			{
				noFall_Shield_KeyUp();
				noFall_Shield_KeyDown();
				fall_Shield_KeyRight();
				fall_Shield_KeyLeft();
			}
			if (!_isLadder)
			{
				noLadder_Shield_KeyRight();
				noLadder_Shield_KeyLeft();
			}
			else
			{
				fall_Shield_KeyRight();
				fall_Shield_KeyLeft();
			}
		}
	}
}

void player_Olaf::keyPressSpace()
{
	if (!_isLadder)
		//사타리를 탄 상태에서는 SPACE 눌러도 상태가 바뀌지 않는다.
		//방패를 위로 들고 있지 않다면
	{
		if (!_isShieldUp)
		{
			//스페이스바를 눌렀을때 방패의 위치가 변경된다.
			if (KEYMANAGER->isOnceKeyDown(VK_SPACE))
			{
				//상태가 변경된다.
				_isShieldUp = true;

				//플레이어 상태가 오른쪽을 보고 있다면
				if (_state == PLAYER_IDLE_RIGHT)
				{
					_state = PLAYER_SHIELD_IDLE_RIGHT;					//방패를 위로 들고 오른쪽을 본다.
					startAniOlaf("idle_Shield_Right");
				}
				//왼쪽을 보고 있다면
				if (_state == PLAYER_IDLE_LEFT)
				{
					_state = PLAYER_SHIELD_IDLE_LEFT;					//방패를 위로 들고 왼쪽을 본다.
					startAniOlaf("idle_Shield_Left");
				}
				//오른쪽으로 이동하고 있다면
				if (_state == PLAYER_MOVE_RIGHT)
				{
					_state = PLAYER_SHIELD_MOVE_RIGHT;					//방패를 위로 들고 오른쪽으로 이동한다.
					startAniOlaf("move_Shield_Right");
				}
				//왼쪽으로 이동하고 있다면
				if (_state == PLAYER_MOVE_LEFT)
				{
					_state = PLAYER_SHIELD_MOVE_LEFT;					//방패를 위로 들고 왼쪽으로 이동한다.
					startAniOlaf("move_Shield_Left");
				}
				//오른쪽으로 떨어지고 있다면
				if (_state == PLAYER_FALL_RIGHT)
				{
					_state = PLAYER_SHIELD_FALL_RIGHT;					//방패를 위로 들고 오른쪽으로 천천히 떨어진다.
					startAniOlaf("fall_Shield_Right");
				}
				//왼쪽으로 떨어지고 있다면
				if (_state == PLAYER_FALL_LEFT)
				{
					_state = PLAYER_SHIELD_FALL_LEFT;					//방패를 위로 들고 왼쪽으로 천천히 떨어진다.
					startAniOlaf("fall_Shield_Left");
				}
			}
		}
		else
		{
			//스페이스바를 눌렀을때 방패의 위치가 변경된다.
			if (KEYMANAGER->isOnceKeyDown(VK_SPACE))
			{
				//상태가 변경된다.
				_isShieldUp = false;

				//플레이어 상태가 방패를 위로 들고 오른쪽을 보고 있다면
				if (_state == PLAYER_SHIELD_IDLE_RIGHT)
				{
					_state = PLAYER_IDLE_RIGHT;							//오른쪽을 본다.
					startAniOlaf("idle_Right");
				}
				//방패를 위로 들고 왼쪽을 보고 있다면
				if (_state == PLAYER_SHIELD_IDLE_LEFT)
				{
					_state = PLAYER_IDLE_LEFT;							//왼쪽을 본다.
					startAniOlaf("idle_Left");
				}
				//방패를 위로 들고 오른쪽으로 이동하고 있다면
				if (_state == PLAYER_SHIELD_MOVE_RIGHT)
				{
					_state = PLAYER_MOVE_RIGHT;							//오른쪽으로 이동한다.
					startAniOlaf("move_Right");
				}
				//방패를 위로 들고 왼쪽으로 이동하고 있다면
				if (_state == PLAYER_SHIELD_MOVE_LEFT)
				{
					_state = PLAYER_MOVE_LEFT;							//왼쪽으로 이동한다.
					startAniOlaf("move_Left");
				}
				//방패를 위로 들고 오른쪽으로 떨어지고 있다면
				if (_state == PLAYER_SHIELD_FALL_RIGHT)
				{
					_state = PLAYER_FALL_RIGHT;							//오른쪽으로 떨어진다.
					startAniOlaf("fall_Right");
				}
				//방패를 위로 들고 왼쪽으로 떨어지고 있다면
				if (_state == PLAYER_SHIELD_FALL_LEFT)
				{
					_state = PLAYER_FALL_LEFT;							//왼쪽으로 떨어진다.
					startAniOlaf("fall_Left");
				}
			}
		}
	}
	else 
	{
		if (!_isShieldUp)
		{
			//스페이스바를 눌렀을때 방패의 위치가 변경된다.
			if (KEYMANAGER->isOnceKeyDown(VK_SPACE))
			{
				//상태가 변경된다.
				_isShieldUp = true;
				//오른쪽으로 떨어지고 있다면
				if (_state == PLAYER_FALL_RIGHT)
				{
					_state = PLAYER_SHIELD_FALL_RIGHT;					//방패를 위로 들고 오른쪽으로 천천히 떨어진다.
					startAniOlaf("fall_Shield_Right");
				}
				//왼쪽으로 떨어지고 있다면
				if (_state == PLAYER_FALL_LEFT)
				{
					_state = PLAYER_SHIELD_FALL_LEFT;					//방패를 위로 들고 왼쪽으로 천천히 떨어진다.
					startAniOlaf("fall_Shield_Left");
				}
			}
		}
		else
		{	//스페이스바를 눌렀을때 방패의 위치가 변경된다.
			if (KEYMANAGER->isOnceKeyDown(VK_SPACE))
			{
				//상태가 변경된다.
				_isShieldUp = false;
				//방패를 위로 들고 오른쪽으로 떨어지고 있다면
				if (_state == PLAYER_SHIELD_FALL_RIGHT)
				{
					_state = PLAYER_FALL_RIGHT;							//오른쪽으로 떨어진다.
					startAniOlaf("fall_Right");
				}
				//방패를 위로 들고 왼쪽으로 떨어지고 있다면
				if (_state == PLAYER_SHIELD_FALL_LEFT)
				{
					_state = PLAYER_FALL_LEFT;							//왼쪽으로 떨어진다.
					startAniOlaf("fall_Left");
				}
			}
		}
	}
}

void player_Olaf::keyPressD()
{
	if (!_isLadder)
		//사타리를 탄 상태에서는 SPACE 눌러도 상태가 바뀌지 않는다.
		//방패를 위로 들고 있지 않다면
	{
		if (!_isShieldUp)
		{
			//스페이스바를 눌렀을때 방패의 위치가 변경된다.
			if (KEYMANAGER->isOnceKeyDown('D'))
			{
				//상태가 변경된다.
				_isShieldUp = true;

				//플레이어 상태가 오른쪽을 보고 있다면
				if (_state == PLAYER_IDLE_RIGHT)
				{
					_state = PLAYER_SHIELD_IDLE_RIGHT;					//방패를 위로 들고 오른쪽을 본다.
					startAniOlaf("idle_Shield_Right");
				}
				//왼쪽을 보고 있다면
				if (_state == PLAYER_IDLE_LEFT)
				{
					_state = PLAYER_SHIELD_IDLE_LEFT;					//방패를 위로 들고 왼쪽을 본다.
					startAniOlaf("idle_Shield_Left");
				}
				//오른쪽으로 이동하고 있다면
				if (_state == PLAYER_MOVE_RIGHT)
				{
					_state = PLAYER_SHIELD_MOVE_RIGHT;					//방패를 위로 들고 오른쪽으로 이동한다.
					startAniOlaf("move_Shield_Right");
				}
				//왼쪽으로 이동하고 있다면
				if (_state == PLAYER_MOVE_LEFT)
				{
					_state = PLAYER_SHIELD_MOVE_LEFT;					//방패를 위로 들고 왼쪽으로 이동한다.
					startAniOlaf("move_Shield_Left");
				}
				//오른쪽으로 떨어지고 있다면
				if (_state == PLAYER_FALL_RIGHT)
				{
					_state = PLAYER_SHIELD_FALL_RIGHT;					//방패를 위로 들고 오른쪽으로 천천히 떨어진다.
					startAniOlaf("fall_Shield_Right");
				}
				//왼쪽으로 떨어지고 있다면
				if (_state == PLAYER_FALL_LEFT)
				{
					_state = PLAYER_SHIELD_FALL_LEFT;					//방패를 위로 들고 왼쪽으로 천천히 떨어진다.
					startAniOlaf("fall_Shield_Left");
				}
			}
		}
		else
		{
			//스페이스바를 눌렀을때 방패의 위치가 변경된다.
			if (KEYMANAGER->isOnceKeyDown('D'))
			{
				//상태가 변경된다.
				_isShieldUp = false;

				//플레이어 상태가 방패를 위로 들고 오른쪽을 보고 있다면
				if (_state == PLAYER_SHIELD_IDLE_RIGHT)
				{
					_state = PLAYER_IDLE_RIGHT;							//오른쪽을 본다.
					startAniOlaf("idle_Right");
				}
				//방패를 위로 들고 왼쪽을 보고 있다면
				if (_state == PLAYER_SHIELD_IDLE_LEFT)
				{
					_state = PLAYER_IDLE_LEFT;							//왼쪽을 본다.
					startAniOlaf("idle_Left");
				}
				//방패를 위로 들고 오른쪽으로 이동하고 있다면
				if (_state == PLAYER_SHIELD_MOVE_RIGHT)
				{
					_state = PLAYER_MOVE_RIGHT;							//오른쪽으로 이동한다.
					startAniOlaf("move_Right");
				}
				//방패를 위로 들고 왼쪽으로 이동하고 있다면
				if (_state == PLAYER_SHIELD_MOVE_LEFT)
				{
					_state = PLAYER_MOVE_LEFT;							//왼쪽으로 이동한다.
					startAniOlaf("move_Left");
				}
				//방패를 위로 들고 오른쪽으로 떨어지고 있다면
				if (_state == PLAYER_SHIELD_FALL_RIGHT)
				{
					_state = PLAYER_FALL_RIGHT;							//오른쪽으로 떨어진다.
					startAniOlaf("fall_Right");
				}
				//방패를 위로 들고 왼쪽으로 떨어지고 있다면
				if (_state == PLAYER_SHIELD_FALL_LEFT)
				{
					_state = PLAYER_FALL_LEFT;							//왼쪽으로 떨어진다.
					startAniOlaf("fall_Left");
				}
			}
		}
	}
	else
	{
		if (!_isShieldUp)
		{
			//스페이스바를 눌렀을때 방패의 위치가 변경된다.
			if (KEYMANAGER->isOnceKeyDown(VK_SPACE))
			{
				//상태가 변경된다.
				_isShieldUp = true;
				//오른쪽으로 떨어지고 있다면
				if (_state == PLAYER_FALL_RIGHT)
				{
					_state = PLAYER_SHIELD_FALL_RIGHT;					//방패를 위로 들고 오른쪽으로 천천히 떨어진다.
					startAniOlaf("fall_Shield_Right");
				}
				//왼쪽으로 떨어지고 있다면
				if (_state == PLAYER_FALL_LEFT)
				{
					_state = PLAYER_SHIELD_FALL_LEFT;					//방패를 위로 들고 왼쪽으로 천천히 떨어진다.
					startAniOlaf("fall_Shield_Left");
				}
			}
		}
		else
		{	//스페이스바를 눌렀을때 방패의 위치가 변경된다.
			if (KEYMANAGER->isOnceKeyDown(VK_SPACE))
			{
				//상태가 변경된다.
				_isShieldUp = false;
				//방패를 위로 들고 오른쪽으로 떨어지고 있다면
				if (_state == PLAYER_SHIELD_FALL_RIGHT)
				{
					_state = PLAYER_FALL_RIGHT;							//오른쪽으로 떨어진다.
					startAniOlaf("fall_Right");
				}
				//방패를 위로 들고 왼쪽으로 떨어지고 있다면
				if (_state == PLAYER_SHIELD_FALL_LEFT)
				{
					_state = PLAYER_FALL_LEFT;							//왼쪽으로 떨어진다.
					startAniOlaf("fall_Left");
				}
			}
		}
	}
}

void player_Olaf::keyPressE()
{
	if (KEYMANAGER->isOnceKeyDown('E'))
	{
		int num = _mainUI->getInvenPos(PLAYER_NAME_OLAF);

		if (_invenItem[num] != nullptr)
		{
			switch (_invenItem[num]->getItemType())
			{
				case ITEM_TYPE_FRUIT_SMALL:
				case ITEM_TYPE_FRUIT_BIG:
				{
					if (_lifeCount == 3) 
						return;
					else
					{
						_lifeCount += 1;
						SAFE_RELEASE(_invenItem[num]);
						SAFE_DELETE(_invenItem[num]);
						_mainUI->setOlafItemInfo(_invenItem);
					}
					break;
				}
				case ITEM_TYPE_MEAT:
				{
					if (_lifeCount == 3)
						return;
					else
					{
						if(_lifeCount == 2)
							_lifeCount += 1;
						else
						{
							_lifeCount += 2;
							SAFE_RELEASE(_invenItem[num]);
							SAFE_DELETE(_invenItem[num]);
							_mainUI->setOlafItemInfo(_invenItem);
						}
					}

					break;
				}
				case ITEM_TYPE_SHIELD:
				{

					break;
				}
				case ITEM_TYPE_KEY_RED:
				case ITEM_TYPE_KEY_YELLOW:
				case ITEM_TYPE_KEY_BLUE:
				{
					RECT temp;
					int size = _objectRc.size();
					for (int i = 0; i < size; i++)
					{
						if(IntersectRect(&temp, _objectRc[i]->getObjectRect(), &_playerRect))
						{
							if (_objectRc[i]->getObjectValue() == _invenItem[num]->getItemValue())
							{
								_objm->interactionObject(_objectRc[i]);
								SAFE_RELEASE(_invenItem[num]);
								SAFE_DELETE(_invenItem[num]);
								_mainUI->setOlafItemInfo(_invenItem);
								break;
							}
						}
					}
					break;
				}

			}
		}
	}
}

void player_Olaf::noLadder_KeyRight()
{
	pixelRightWallCollision();
	if (_state != PLAYER_MOVE_LEFT && _state != PLAYER_PUSH_WALL_LEFT && _state != PLAYER_SHIELD_MOVE_LEFT &&
		_state != PLAYER_LADDER_UP && _state != PLAYER_LADDER_DOWN && _state != PLAYER_LADDER_END)
	{
		if (KEYMANAGER->isOnceKeyUp(VK_RIGHT))
		{
			if (!_isFloor)									//낙하할때
			{
				_state = PLAYER_FALL_RIGHT;
				startAniOlaf("fall_Right");
			}
			else 											//바닥에 있을때
			{
				_state = PLAYER_IDLE_RIGHT;
				startAniOlaf("idle_Right");
			}
		}
		if (KEYMANAGER->isStayKeyDown(VK_RIGHT))
		{
			if (_state == PLAYER_IDLE_RIGHT || _state == PLAYER_IDLE_LEFT)
			{
				_state = PLAYER_MOVE_RIGHT;
				startAniOlaf("move_Right");
			}
			if (!_isFloor)									//낙하할때
			{
				if (_state == PLAYER_FALL_LEFT)
				{
					_state = PLAYER_FALL_RIGHT;
					startAniOlaf("fall_Right");
				}
				else
				{
					_state = PLAYER_FALL_RIGHT;
				}
			}
			else if (_isRightCollision || _pixelData->GetWidth() <= _playerRect.right)	// 오른쪽 벽과 부딪히거나 맵 끝일때
			{
				if (!_isAniStart)
				{
					_state = PLAYER_PUSH_WALL_RIGHT;
					startAniOlaf("push_Wall_Right");
					_isAniStart = true;
				}
			}
			else		//바닥에 있을때
			{
				_state = PLAYER_MOVE_RIGHT;
			}
		}
		if (KEYMANAGER->isOnceKeyDown(VK_RIGHT))
		{
			_isAniStart = false;
			if (_state == PLAYER_IDLE_RIGHT || _state == PLAYER_IDLE_LEFT)
			{
				_state = PLAYER_MOVE_RIGHT;
				startAniOlaf("move_Right");
			}
			if (!_isFloor)						//낙하할때
			{
				_state = PLAYER_FALL_RIGHT;
				startAniOlaf("fall_Right");
			}
			else if (_isRightCollision || _pixelData->GetWidth() <= _playerRect.right)	// 오른쪽 벽과 부딪히거나 맵 끝일때
			{
				_state = PLAYER_PUSH_WALL_RIGHT;
				startAniOlaf("push_Wall_Right");
			}
			else		//바닥에 있을때
			{
				_state = PLAYER_MOVE_RIGHT;
				startAniOlaf("move_Right");
			}
		}
	}
}

void player_Olaf::noLadder_KeyLeft()
{
	pixelLeftWallCollision();
	if (_state != PLAYER_MOVE_RIGHT && _state != PLAYER_PUSH_WALL_RIGHT && _state != PLAYER_SHIELD_MOVE_RIGHT &&
		_state != PLAYER_LADDER_UP && _state != PLAYER_LADDER_DOWN && _state != PLAYER_LADDER_END)
	{
		if (KEYMANAGER->isOnceKeyUp(VK_LEFT))
		{
			if (!_isFloor)									//낙하할때
			{
				_state = PLAYER_FALL_LEFT;
				startAniOlaf("fall_Left");
			}
			else											//바닥에 있을때
			{
				_state = PLAYER_IDLE_LEFT;
				startAniOlaf("idle_Left");
			}
		}
		if (KEYMANAGER->isStayKeyDown(VK_LEFT))
		{
			if (_state == PLAYER_IDLE_LEFT || _state == PLAYER_IDLE_RIGHT || _state == PLAYER_MOVE_RIGHT)
			{
				_state = PLAYER_MOVE_LEFT;
				startAniOlaf("move_Left");
			}
			if (!_isFloor)										//낙하할때
			{
				_state = PLAYER_FALL_LEFT;
			}
			else if (!_isLeftCollision &&_isFloor)				//바닥에 있을때
			{
				_state = PLAYER_MOVE_LEFT;
			}
			if (_isFloor && (_isLeftCollision || _playerRect.left <= 0))	//왼쪽 벽과 부딪히거나 맵 끝일때
			{
				_state = PLAYER_PUSH_WALL_LEFT;
				if (!_isAniStart)
				{
					startAniOlaf("push_Wall_Left");
					_isAniStart = true;
				}
			}
		}
		if (KEYMANAGER->isOnceKeyDown(VK_LEFT))
		{
			_isAniStart = false;
			if (_state == PLAYER_IDLE_LEFT || _state == PLAYER_IDLE_RIGHT)
			{
				_state = PLAYER_MOVE_LEFT;
				startAniOlaf("move_Left");
			}
			if (!_isLeftCollision && _isFloor)					//바닥에 있을때
			{
				_state = PLAYER_MOVE_LEFT;
				startAniOlaf("move_Left");
			}
			if (_isLeftCollision && !_isFloor)
			{
				_state = PLAYER_PUSH_WALL_LEFT;
				startAniOlaf("fall_Left");
			}
			else if (!_isLeftCollision && (_state == PLAYER_IDLE_LEFT || _state == PLAYER_IDLE_RIGHT))
			{
				_state = PLAYER_MOVE_LEFT;
				startAniOlaf("move_Left");
			}
			else if (!_isFloor)											//낙하할때
			{
				_state = PLAYER_FALL_LEFT;
				startAniOlaf("fall_Left");
			}
			else if (_isLeftCollision || _playerRect.left <= 0)		// 오른쪽 벽과 부딪히거나 맵 끝일때
			{
				_state = PLAYER_PUSH_WALL_LEFT;
				startAniOlaf("push_Wall_Left");
			}
		}
	}
}

void player_Olaf::ladder_KeyUp()
{
	if (KEYMANAGER->isOnceKeyUp(VK_UP))
	{
		if (!_isLadderTop && !_isLadderBottom)					// 사다리 타있는 상태
		{
			_state = PLAYER_IDLE_RIGHT;
			_olafMotion->pause();
		}
		if (_isLadderTop && !_isLadderBottom)
		{
			_state = PLAYER_IDLE_RIGHT;
			startAniOlaf("idle_Right");
		}
	}
	if (KEYMANAGER->isStayKeyDown(VK_UP))
	{
		if (!_isLadderTop && !_isLadderBottom)							// 사다리 타있는 상태
		{
			if (_state == PLAYER_IDLE_RIGHT || _state == PLAYER_IDLE_LEFT ||
				_state == PLAYER_SHIELD_IDLE_LEFT || _state == PLAYER_SHIELD_IDLE_RIGHT)
			{
				_state = PLAYER_LADDER_UP;
			}
		}
		else if (_isLadderTop && !_isLadderBottom)		// 사다리 위에 있는 상태
		{
			if (!_isAniStart)
			{
				_state = PLAYER_LADDER_END;
				startAniOlaf("ladder_End");
				_isAniStart = true;
			}
		}
	}
	if (KEYMANAGER->isOnceKeyDown(VK_UP))
	{
		_isAniStart = false;
		if (!_isLadderTop && _isLadderBottom)							// 사다리 바닥에 있는 상태
		{
			_state = PLAYER_LADDER_UP;
			startAniOlaf("ladder_Up");
		}
		if (!_isLadderTop && !_isLadderBottom)							// 사다리 타있는 상태
		{
			if (_state != PLAYER_IDLE_RIGHT || _state != PLAYER_IDLE_LEFT ||
				_state != PLAYER_SHIELD_IDLE_LEFT || _state != PLAYER_SHIELD_IDLE_RIGHT)
			{
				_state = PLAYER_LADDER_UP;
				startAniOlaf("ladder_Up");
			}
		}
		if (_isLadderTop && !_isLadderBottom)							// 사다리 위에 있는 상태
		{
			_state = PLAYER_LADDER_END;
			startAniOlaf("ladder_End");
		}
	}
}

void player_Olaf::ladder_KeyDown()
{
	if (KEYMANAGER->isOnceKeyUp(VK_DOWN))
	{
		if (!_isLadderTop && !_isLadderBottom)							// 사다리 타있는 상태
		{
			_state = PLAYER_IDLE_RIGHT;
			_olafMotion->pause();
		}
		if (_isLadderTop && !_isLadderBottom)							// 사다리 위에 있는 상태
		{
			_state = PLAYER_IDLE_RIGHT;
			startAniOlaf("idle_Right");
		}
		if (!_isLadderTop && _isLadderBottom)
		{
			_state = PLAYER_IDLE_RIGHT;
			startAniOlaf("idle_Right");
		}
	}
	if (KEYMANAGER->isStayKeyDown(VK_DOWN))
	{
		if (!_isLadderTop && !_isLadderBottom)							// 사다리 타있는 상태				
		{
			if (_state == PLAYER_IDLE_RIGHT || _state == PLAYER_IDLE_LEFT ||
				_state == PLAYER_SHIELD_IDLE_LEFT || _state == PLAYER_SHIELD_IDLE_RIGHT)
			{
				_state = PLAYER_LADDER_DOWN;
				startAniOlaf("ladder_Down");
			}
		}
		if (_isLadderTop && !_isLadderBottom)					//사다리 위에 있는 상태
		{
			_state = PLAYER_LADDER_DOWN;
		}
		if (!_isLadderTop && _isLadderBottom)					//사다리 밑인 상태
		{
			_state = PLAYER_IDLE_RIGHT;
			startAniOlaf("idle_Right");
		}
	}
	if (KEYMANAGER->isOnceKeyDown(VK_DOWN))
	{
		_isAniStart = false;
		if (!_isLadderTop && !_isLadderBottom)
		{
			_state = PLAYER_LADDER_DOWN;
			startAniOlaf("ladder_Down");
		}
		if (_isLadderTop && !_isLadderBottom)
		{
			_state = PLAYER_LADDER_DOWN;
			startAniOlaf("ladder_Down");
		}
		if (!_isLadderTop && _isLadderBottom)
		{
			_state = PLAYER_IDLE_RIGHT;
			startAniOlaf("idle_Right");
		}
	}
}

void player_Olaf::ladder_KeyRight()
{
	pixelRightWallCollision();
	if (KEYMANAGER->isOnceKeyUp(VK_RIGHT))
	{
		if (!_isLadderTop && !_isLadderBottom)
		{
			_state = PLAYER_FALL_RIGHT;
			startAniOlaf("fall_Right");
		}
		if (!_isLadderTop && _isLadderBottom)
		{
			_state = PLAYER_IDLE_RIGHT;
			startAniOlaf("idle_Right");
		}
		if (_isLadderTop && !_isLadderBottom)
		{
			_state = PLAYER_IDLE_RIGHT;
			startAniOlaf("idle_Right");
		}
	}
	if (KEYMANAGER->isStayKeyDown(VK_RIGHT))
	{
		if (_state == PLAYER_IDLE_RIGHT || _state == PLAYER_IDLE_LEFT)
		{
			_state = PLAYER_MOVE_RIGHT;
			startAniOlaf("move_Right");
		}
		if (!_isLadderTop && !_isLadderBottom)
		{
			if (!_isAniStart)
			{
				_state = PLAYER_FALL_RIGHT;
				startAniOlaf("fall_Right");
				_isAniStart = true;
			}
		}
		if (!_isLadderTop && _isLadderBottom)
		{
			_state = PLAYER_IDLE_RIGHT;
		}
		if (!_isLadderTop && _isFloor)
		{
			if (_state == PLAYER_FALL_RIGHT)
			{
				_state = PLAYER_MOVE_RIGHT;
				startAniOlaf("move_Right");
			}
			else if (_state == PLAYER_IDLE_RIGHT)
			{
				_state = PLAYER_MOVE_RIGHT;
			}
		}
		if (_isLadderTop && _isFloor)
		{
			_state = PLAYER_MOVE_RIGHT;
		}
		if (_isRightCollision)
		{
			_state = PLAYER_FALL_RIGHT;
		}
	}
	if (KEYMANAGER->isOnceKeyDown(VK_RIGHT))
	{
		if (_state == PLAYER_IDLE_RIGHT)
		{
			_state = PLAYER_MOVE_RIGHT;
			startAniOlaf("move_Right");
		}
		if (!_isLadderTop && !_isLadderBottom && _isFloor)
		{
			_state = PLAYER_MOVE_RIGHT;
			startAniOlaf("move_Right");
		}
		if (!_isLadderTop && !_isLadderBottom)
		{
			_state = PLAYER_FALL_RIGHT;
			startAniOlaf("fall_Right");
		}
		if (!_isLadderTop && _isLadderBottom)
		{
			_state = PLAYER_IDLE_RIGHT;
			startAniOlaf("idle_Right");
		}
		if (!_isLadderTop && _isFloor)
		{
			_state = PLAYER_MOVE_RIGHT;
			startAniOlaf("move_Right");
		}
		if (_isLadderTop && _isFloor && _state == PLAYER_IDLE_RIGHT)
		{
			_state = PLAYER_MOVE_RIGHT;
			startAniOlaf("move_Right");
		}
		if (_isRightCollision)
		{
			_state = PLAYER_FALL_RIGHT;
		}
		

	}
}

void player_Olaf::ladder_KeyLeft()
{
	pixelLeftWallCollision();
	if (KEYMANAGER->isOnceKeyUp(VK_LEFT))
	{
		if (!_isLadderTop && !_isLadderBottom)
		{
			_state = PLAYER_FALL_LEFT;
			startAniOlaf("fall_Left");
		}
		if (!_isLadderTop && _isLadderBottom)
		{
			_state = PLAYER_IDLE_LEFT;
			startAniOlaf("idle_Left");
		}
		if (_isLadderTop && !_isLadderBottom)
		{
			_state = PLAYER_IDLE_LEFT;
			startAniOlaf("idle_Left");
		}
	}
	if (KEYMANAGER->isStayKeyDown(VK_LEFT))
	{
		if (!_isLadderTop && !_isLadderBottom)
		{
			if (!_isAniStart)
			{
				_state = PLAYER_FALL_LEFT;
				startAniOlaf("fall_Left");
				_isAniStart = true;
			}
		}
		if (!_isLadderTop && _isLadderBottom)
		{
			_state = PLAYER_IDLE_LEFT;
		}
		if (!_isLadderTop && _isFloor)
		{
			if (_state == PLAYER_FALL_LEFT)
			{
				_state = PLAYER_MOVE_LEFT;
				startAniOlaf("move_Left");
			}
			else if (_state == PLAYER_IDLE_LEFT)
			{
				_state = PLAYER_MOVE_LEFT;
			}
		}
		if (_isLadderTop && _isFloor)
		{
			_state = PLAYER_MOVE_LEFT;
			startAniOlaf("move_Left");
		}
		if (_isLeftCollision)
		{
			_state = PLAYER_FALL_LEFT;
		}
	}
	if (KEYMANAGER->isOnceKeyDown(VK_LEFT))
	{
		if (_state == PLAYER_IDLE_LEFT || _state == PLAYER_IDLE_RIGHT)
		{
			_state = PLAYER_MOVE_LEFT;
			startAniOlaf("move_Left");
		}
		if (!_isLadderTop && !_isLadderBottom)
		{
			_state = PLAYER_FALL_LEFT;
			startAniOlaf("fall_Left");
		}
		if (!_isLadderTop && _isLadderBottom)
		{
			_state = PLAYER_IDLE_LEFT;
			startAniOlaf("idle_Left");
		}
		if (!_isLadderTop && _isFloor)
		{
			_state = PLAYER_MOVE_LEFT;
			startAniOlaf("move_Left");
		}
		if (_isLadderTop && _isFloor && _state == PLAYER_IDLE_LEFT)
		{
			_state = PLAYER_MOVE_LEFT;
			startAniOlaf("move_Left");
		}
		if (_isLeftCollision)
		{
			_state = PLAYER_FALL_LEFT;
		}
	}
}

void player_Olaf::noFall_Shield_KeyRight()
{
	pixelRightWallCollision();
	if (_state != PLAYER_MOVE_LEFT && _state != PLAYER_PUSH_WALL_LEFT && _state != PLAYER_SHIELD_MOVE_LEFT)
	{
		if (KEYMANAGER->isOnceKeyUp(VK_RIGHT))
		{
			if (!_isFloor)									//낙하할때
			{
				_state = PLAYER_SHIELD_FALL_RIGHT;
				startAniOlaf("fall_Shield_Right");
			}
			else 										//바닥에 있을때
			{
				_state = PLAYER_SHIELD_IDLE_RIGHT;
				startAniOlaf("idle_Shield_Right");
			}
		}

		if (KEYMANAGER->isStayKeyDown(VK_RIGHT))
		{
			if (_state == PLAYER_SHIELD_IDLE_RIGHT || _state == PLAYER_SHIELD_IDLE_LEFT)
			{
				_state = PLAYER_SHIELD_MOVE_RIGHT;
				startAniOlaf("move_Shield_Right");
			}
			if (_isFloor && _state == PLAYER_SHIELD_FALL_RIGHT)
			{
				_state = PLAYER_SHIELD_MOVE_RIGHT;
				startAniOlaf("move_Shield_Right");
			}
			if (!_isFloor && !_isAniStart)					//낙하할때
			{
				_state = PLAYER_SHIELD_FALL_RIGHT;
			}
			else if (_isRightCollision || _pixelData->GetWidth() <= _playerRect.right)	// 오른쪽 벽과 부딪히거나 맵 끝일때
			{
				_state = PLAYER_PUSH_WALL_RIGHT;
				if (!_isRightMotion)
				{
					startAniOlaf("push_Wall_Right");
					_isRightMotion = true;
				}
			}
			else		//바닥에 있을때
			{
				_state = PLAYER_SHIELD_MOVE_RIGHT;
			}
		}

		if (KEYMANAGER->isOnceKeyDown(VK_RIGHT))
		{
			_isRightMotion = false;
			if (_state == PLAYER_SHIELD_IDLE_RIGHT || _state == PLAYER_SHIELD_IDLE_LEFT)
			{
				_state = PLAYER_SHIELD_MOVE_RIGHT;
				startAniOlaf("move_Shield_Right");
			}
			if (!_isFloor)											//낙하할때
			{
				_state = PLAYER_SHIELD_FALL_RIGHT;
				startAniOlaf("fall_Shield_Right");
			}
			else if (_isRightCollision || _pixelData->GetWidth() <= _playerRect.right)	// 오른쪽 벽과 부딪히거나 맵 끝일때
			{
				_state = PLAYER_PUSH_WALL_RIGHT;
				startAniOlaf("push_Wall_Right");
			}
			else			//바닥에 있을때
			{
				_state = PLAYER_SHIELD_MOVE_RIGHT;
				startAniOlaf("move_Shield_Right");
			}
		}
	}
}

void player_Olaf::noFall_Shield_KeyLeft()
{
	pixelLeftWallCollision();
	if (_state != PLAYER_MOVE_RIGHT && _state != PLAYER_PUSH_WALL_RIGHT && _state != PLAYER_SHIELD_MOVE_RIGHT)
	{
		if (KEYMANAGER->isOnceKeyUp(VK_LEFT))
		{
			if (!_isFloor)									//낙하할때
			{
				_state = PLAYER_SHIELD_FALL_LEFT;
				startAniOlaf("fall_Shield_Left");
			}
			else											//바닥에 있을때
			{
				_state = PLAYER_SHIELD_IDLE_LEFT;
				startAniOlaf("idle_Shield_Left");
			}
		}
		if (KEYMANAGER->isStayKeyDown(VK_LEFT))
		{
			if (_state == PLAYER_SHIELD_IDLE_LEFT || _state == PLAYER_SHIELD_IDLE_RIGHT)
			{
				_state = PLAYER_SHIELD_MOVE_LEFT;
				startAniOlaf("move_Shield_Left");
			}
			if (_isFloor && _state == PLAYER_SHIELD_FALL_LEFT)
			{
				_state = PLAYER_SHIELD_MOVE_LEFT;
				startAniOlaf("move_Shield_Left");
			}
			if (!_isFloor && !_isAniStart)						//낙하할때
			{
				_state = PLAYER_SHIELD_FALL_LEFT;
			}
			else if (_isLeftCollision &&_isFloor)				//바닥에 있을때
			{
				_state = PLAYER_SHIELD_MOVE_LEFT;
			}
			else if (_isLeftCollision || _playerRect.left <= 0)	//왼쪽 벽과 부딪히거나 맵 끝일때
			{
				_state = PLAYER_PUSH_WALL_LEFT;
				if (!_isAniStart)
				{
					startAniOlaf("push_Wall_Left");
					_isAniStart = true;
				}
			}
		}
		if (KEYMANAGER->isOnceKeyDown(VK_LEFT))
		{
			_isAniStart = false;
			if (_state == PLAYER_SHIELD_IDLE_LEFT || _state == PLAYER_SHIELD_IDLE_RIGHT)
			{
				_state = PLAYER_SHIELD_MOVE_LEFT;
				startAniOlaf("move_Shield_Left");
			}
			if (!_isFloor)											//낙하할때
			{
				_state = PLAYER_SHIELD_FALL_LEFT;
				startAniOlaf("fall_Shield_Left");
			}
			else if (!_isLeftCollision && _isFloor)					//바닥에 있을때
			{
				_state = PLAYER_SHIELD_MOVE_LEFT;
				startAniOlaf("move_Shield_Left");
			}
			else if (_isLeftCollision || _playerRect.left <= 0)		// 오른쪽 벽과 부딪히거나 맵 끝일때
			{
				_state = PLAYER_PUSH_WALL_LEFT;
				startAniOlaf("push_Wall_Left");
			}
		}
	}
}

void player_Olaf::noFall_Shield_KeyUp()
{
	if (KEYMANAGER->isOnceKeyUp(VK_UP))
	{
		if (!_isLadderTop && !_isLadderBottom)							// 사다리 타있는 상태
		{
			_state = PLAYER_SHIELD_IDLE_RIGHT;
			_olafMotion->pause();
		}
		if (_isLadderTop && !_isLadderBottom)
		{
			_state = PLAYER_SHIELD_IDLE_RIGHT;
			startAniOlaf("idle_Shield_Right");
		}
	}
	if (KEYMANAGER->isStayKeyDown(VK_UP))
	{
		if (!_isLadderTop && !_isLadderBottom)							// 사다리 타있는 상태
		{
			if (_state == PLAYER_IDLE_RIGHT || _state == PLAYER_IDLE_LEFT ||
				_state == PLAYER_SHIELD_IDLE_LEFT || _state == PLAYER_SHIELD_IDLE_RIGHT)
			{
				_state = PLAYER_LADDER_UP;
			}
		}
		else if (_isLadderTop && !_isLadderBottom && !_isAniStart)		// 사다리 위에 있는 상태
		{
			_state = PLAYER_LADDER_END;
			startAniOlaf("ladder_End");
			_isAniStart = true;
		}
	}
	if (KEYMANAGER->isOnceKeyDown(VK_UP))
	{
		_isAniStart = false;
		if (!_isLadderTop && _isLadderBottom)							// 사다리 바닥에 있는 상태
		{
			_state = PLAYER_LADDER_UP;
			startAniOlaf("ladder_Up");
		}
		if (!_isLadderTop && !_isLadderBottom)							// 사다리 타있는 상태
		{
			if (_state != PLAYER_IDLE_RIGHT || _state != PLAYER_IDLE_LEFT ||
				_state != PLAYER_SHIELD_IDLE_LEFT || _state != PLAYER_SHIELD_IDLE_RIGHT)
			{
				_state = PLAYER_LADDER_UP;
				startAniOlaf("ladder_Up");
			}
		}
		if (_isLadderTop && !_isLadderBottom)							// 사다리 위에 있는 상태
		{
			_state = PLAYER_LADDER_END;
			startAniOlaf("ladder_End");
		}
	}
}

void player_Olaf::noFall_Shield_KeyDown()
{
	if (KEYMANAGER->isOnceKeyUp(VK_DOWN))
	{
		if (!_isLadderTop && !_isLadderBottom)							// 사다리 타있는 상태
		{
			_state = PLAYER_SHIELD_IDLE_RIGHT;
			_olafMotion->pause();
		}
		if (_isLadderTop && !_isLadderBottom)							// 사다리 위에 있는 상태
		{
			_state = PLAYER_SHIELD_IDLE_RIGHT;
		}
		if (!_isLadderTop && _isLadderBottom)
		{
			_state = PLAYER_SHIELD_IDLE_RIGHT;
		}
	}
	if (KEYMANAGER->isStayKeyDown(VK_DOWN))
	{
		if (!_isLadderTop && !_isLadderBottom)							// 사다리 타있는 상태				
		{
			if (_state == PLAYER_IDLE_RIGHT || _state == PLAYER_IDLE_LEFT ||
				_state == PLAYER_SHIELD_IDLE_LEFT || _state == PLAYER_SHIELD_IDLE_RIGHT)
			{
				_state = PLAYER_LADDER_DOWN;
				startAniOlaf("ladder_Down");
			}
		}
		if (_isLadderTop && !_isLadderBottom)					//사다리 위에 있는 상태
		{
			_state = PLAYER_LADDER_DOWN;
		}
		if (!_isLadderTop && _isLadderBottom)					//사다리 밑인 상태
		{
			_state = PLAYER_SHIELD_IDLE_RIGHT;
			startAniOlaf("idle_Shield_Right");
		}
	}
	if (KEYMANAGER->isOnceKeyDown(VK_DOWN))
	{
		_isAniStart = false;
		if (!_isLadderTop && !_isLadderBottom)
		{
			_state = PLAYER_LADDER_DOWN;
			startAniOlaf("ladder_Down");
		}
		if (_isLadderTop && !_isLadderBottom)
		{
			_state = PLAYER_LADDER_DOWN;
			startAniOlaf("ladder_Down");
		}
		if (!_isLadderTop && _isLadderBottom)
		{
			_state = PLAYER_SHIELD_IDLE_RIGHT;
			startAniOlaf("idle_Shield_Right");
		}
	}
}

void player_Olaf::noLadder_Shield_KeyRight()
{
	pixelRightWallCollision();
	if (KEYMANAGER->isOnceKeyUp(VK_RIGHT))
	{
		if (!_isFloor)
		{
			_state = PLAYER_SHIELD_FALL_RIGHT;
			startAniOlaf("fall_Shield_Right");
		}
		else
		{
			_state = PLAYER_SHIELD_IDLE_RIGHT;
			startAniOlaf("idle_Shield_Right");
		}
	}
	if (KEYMANAGER->isStayKeyDown(VK_RIGHT))
	{
		if (!_isFloor)
		{
			if (!_isAniStart)
			{
				_state = PLAYER_SHIELD_FALL_MOVE_RIGHT;
				startAniOlaf("fall_Shield_Move_Right");
				_isAniStart = true;
			}
		}
		if (_isRightCollision)
		{
			_state = PLAYER_SHIELD_FALL_RIGHT;
		}
	}
	if (KEYMANAGER->isOnceKeyDown(VK_RIGHT))
	{
		if (!_isFloor)
		{
			_state = PLAYER_SHIELD_FALL_MOVE_RIGHT;
			startAniOlaf("fall_Shield_Move_Right");
		}
		if (_state == PLAYER_SHIELD_FALL_RIGHT || _state == PLAYER_SHIELD_FALL_LEFT)
		{
			_state = PLAYER_SHIELD_FALL_MOVE_RIGHT;
			startAniOlaf("fall_Shield_Move_Right");
		}
		if (_isRightCollision)
		{
			_state = PLAYER_SHIELD_FALL_RIGHT;
			startAniOlaf("fall_Shield_Right");
		}

	}
}

void player_Olaf::noLadder_Shield_KeyLeft()
{
	pixelLeftWallCollision();
	if (KEYMANAGER->isOnceKeyUp(VK_LEFT))
	{
		if (!_isFloor)
		{
			_state = PLAYER_SHIELD_FALL_LEFT;
			startAniOlaf("fall_Shield_Left");
		}
		else
		{
			_state = PLAYER_SHIELD_IDLE_LEFT;
			startAniOlaf("idle_Shield_Left");
		}
	}
	if (KEYMANAGER->isStayKeyDown(VK_LEFT))
	{
		if (!_isFloor)
		{
			if (!_isAniStart)
			{
				_state = PLAYER_SHIELD_FALL_MOVE_LEFT;
				startAniOlaf("fall_Shield_Move_Left");
				_isAniStart = true;
			}
		}
		if (_isLeftCollision)
		{
			_state = PLAYER_SHIELD_FALL_LEFT;
		}
	}
	if (KEYMANAGER->isOnceKeyDown(VK_LEFT))
	{
		if (!_isFloor)
		{
			_state = PLAYER_SHIELD_FALL_MOVE_LEFT;
			startAniOlaf("fall_Shield_Move_Left");
		}
		if (_state == PLAYER_SHIELD_FALL_LEFT || _state == PLAYER_SHIELD_FALL_LEFT)
		{
			_state = PLAYER_SHIELD_FALL_MOVE_LEFT;
			startAniOlaf("fall_Shield_Move_Left");
		}
		if (_isLeftCollision)
		{
			_state = PLAYER_SHIELD_FALL_LEFT;
			startAniOlaf("fall_Shield_Left");
		}

	}
}

void player_Olaf::fall_Shield_KeyRight()
{
	pixelRightWallCollision();
	if (KEYMANAGER->isOnceKeyUp(VK_RIGHT))
	{
		if (!_isLadderTop && !_isLadderBottom)
		{
			_state = PLAYER_SHIELD_FALL_RIGHT;
			startAniOlaf("fall_Shield_Right");
		}
		if (!_isLadderTop && _isLadderBottom)
		{
			_state = PLAYER_SHIELD_IDLE_RIGHT;
			startAniOlaf("idle_Shield_Right");
		}
		if (_isLadderTop && !_isLadderBottom)
		{
			_state = PLAYER_SHIELD_IDLE_RIGHT;
			startAniOlaf("idle_Shield_Right");
		}
	}
	if (KEYMANAGER->isStayKeyDown(VK_RIGHT))
	{
		if (!_isLadderTop && !_isLadderBottom)
		{
			if (!_isAniStart)
			{
				_state = PLAYER_SHIELD_FALL_MOVE_RIGHT;
				startAniOlaf("fall_Shield_Move_Right");
				_isAniStart = true;
			}
		}
		if (!_isLadderTop && _isLadderBottom)
		{
			_state = PLAYER_SHIELD_IDLE_RIGHT;
		}
		if (_isFloor && (_state == PLAYER_SHIELD_IDLE_LEFT || _state == PLAYER_SHIELD_IDLE_RIGHT))
		{
			_state = PLAYER_SHIELD_MOVE_RIGHT;
			startAniOlaf("move_Shield_Right");
		}
		if (_isLadderTop && _isFloor && _state == PLAYER_SHIELD_IDLE_RIGHT)
		{
			_state = PLAYER_SHIELD_MOVE_RIGHT;
			startAniOlaf("move_Shield_Right");
		}
		if (_isRightCollision)
		{
			_state = PLAYER_SHIELD_FALL_RIGHT;
		}
	}
	if (KEYMANAGER->isOnceKeyDown(VK_RIGHT))
	{
		if (!_isLadderTop && !_isLadderBottom)
		{
			_state = PLAYER_SHIELD_FALL_MOVE_RIGHT;
			startAniOlaf("fall_Shield_Move_Right");
		}
		if (!_isLadderTop && _isLadderBottom)
		{
			_state = PLAYER_SHIELD_IDLE_RIGHT;
			startAniOlaf("idle_Shield_Right");
		}
		if (_isFloor && (_state == PLAYER_SHIELD_IDLE_LEFT || _state == PLAYER_SHIELD_IDLE_RIGHT))
		{
			_state = PLAYER_SHIELD_MOVE_RIGHT;
			startAniOlaf("move_Shield_Right");
		}
		if (_isLadderTop && _isFloor && _state == PLAYER_SHIELD_IDLE_RIGHT)
		{
			_state = PLAYER_SHIELD_MOVE_RIGHT;
			startAniOlaf("move_Shield_Right");
		}
		if (_isRightCollision)
		{
			_state = PLAYER_SHIELD_FALL_RIGHT;
			startAniOlaf("fall_Shield_Right");
		}

	}
}

void player_Olaf::fall_Shield_KeyLeft()
{
	pixelLeftWallCollision();
	if (KEYMANAGER->isOnceKeyUp(VK_LEFT))
	{
		if (!_isLadderTop && !_isLadderBottom)
		{
			_state = PLAYER_SHIELD_FALL_LEFT;
			startAniOlaf("fall_Shield_Left");
		}
		if (!_isLadderTop && _isLadderBottom)
		{
			_state = PLAYER_SHIELD_IDLE_LEFT;
			startAniOlaf("idle_Shield_Left");
		}
		if (_isLadderTop && !_isLadderBottom)
		{
			_state = PLAYER_SHIELD_IDLE_LEFT;
			startAniOlaf("idle_Shield_Left");
		}
	}
	if (KEYMANAGER->isStayKeyDown(VK_LEFT))
	{
		if (!_isLadderTop && !_isLadderBottom && !_isAniStart)
		{
			_state = PLAYER_SHIELD_FALL_MOVE_LEFT;
			startAniOlaf("fall_Shield_Left");
			_isAniStart = true;
		}
		if (!_isLadderTop && _isLadderBottom)
		{
			_state = PLAYER_SHIELD_IDLE_LEFT;
		}
		if (_isFloor && (_state == PLAYER_SHIELD_IDLE_LEFT || _state == PLAYER_SHIELD_IDLE_RIGHT))
		{
			_state = PLAYER_SHIELD_MOVE_LEFT;
		}
		if (_isLadderTop && _isFloor && _state == PLAYER_SHIELD_IDLE_LEFT)
		{
			_state = PLAYER_SHIELD_MOVE_LEFT;
		}
		if (_isLeftCollision && _isFloor)
		{
			_state = PLAYER_SHIELD_FALL_LEFT;
		}
		if ((_state == PLAYER_SHIELD_FALL_MOVE_LEFT || _state == PLAYER_SHIELD_FALL_LEFT) && _isFloor)
		{
			_state = PLAYER_SHIELD_FALL_LEFT;
		}
	}
	if (KEYMANAGER->isOnceKeyDown(VK_LEFT))
	{
		if (!_isLadderTop && !_isLadderBottom)
		{
			_state = PLAYER_SHIELD_FALL_MOVE_LEFT;
			startAniOlaf("fall_Shield_Move_Left");
		}
		if (!_isLadderTop && _isLadderBottom)
		{
			_state = PLAYER_SHIELD_IDLE_LEFT;
			startAniOlaf("idle_Left");
		}
		if (_isFloor && (_state == PLAYER_SHIELD_IDLE_LEFT || _state == PLAYER_SHIELD_IDLE_RIGHT))
		{
			_state = PLAYER_SHIELD_MOVE_LEFT;
			startAniOlaf("move_Shield_Left");
		}
		if (_isLadderTop && _isFloor && _state == PLAYER_SHIELD_IDLE_LEFT)
		{
			_state = PLAYER_SHIELD_MOVE_LEFT;
			startAniOlaf("move_Shield_Left");
		}
		if (_isLeftCollision && _isFloor)
		{
			_state = PLAYER_SHIELD_FALL_LEFT;
			startAniOlaf("fall_Shield_Left");
		}
		if ((_state == PLAYER_SHIELD_FALL_MOVE_LEFT || _state == PLAYER_SHIELD_FALL_LEFT) && _isFloor)
		{
			_state = PLAYER_SHIELD_FALL_LEFT;
			startAniOlaf("fall_Shield_Left");
		}
	}
}

void player_Olaf::initOlaf()
{
	_x = 50;								   //X좌표
	_y = 1370;								   //Y좌표
	_speed = 0.f;							   //스피드
	_lifeCount = 3;							   //체력

	_isAlive = true;						   //살았니?
	_isFloor = true;						   //바닥이니?
	_isLadder = false;						   //사다리 탔니?
	_isLadderTop = false;					   //사다리 위에 있니?
	_isLadderBottom = false;				   //사다리 밑에 있니?
	_deathMotion = false;					   //죽는모션했니?
	_isAniStart = false;					   //모션시작했니?
	_isRightMotion = false;


	_accleration = 3.5f;					   //가속도
	_minSpeed = 1.0f;						   //최저속도
	_maxSpeed = 4.0f;						   //최고속도
	_shieldGravity = 100.0f;					   //방패중력

	_proveBottom = _playerRect.bottom + 5;		//충돌처리
	_proveRight = _playerRect.right;			//충돌처리
	_proveLeft = _playerRect.left;				//충돌처리


	_playerRect = RectMakeCenter(_x, _y, 70, 70); //올라프렉트생성
}

void player_Olaf::initImgOlaf()
{
	KEYANIMANAGER->addAnimationType("player_Olaf");
	_player = IMAGEMANAGER->addFrameImage("olaf", "image/olaf.bmp", 1960, 420, 28, 6, true, RGB(255, 0, 255));
	_state = PLAYER_IDLE_RIGHT;

	int idle_Right[] = { 1 };
	KEYANIMANAGER->addArrayFrameAnimation("player_Olaf", "idle_Right", "olaf", idle_Right, 1, 10, true);
	int idle_Left[] = { 29 };
	KEYANIMANAGER->addArrayFrameAnimation("player_Olaf", "idle_Left", "olaf", idle_Left, 1, 10, true);
	int move_Right[] = { 13, 14, 15, 16, 17, 18, 19, 20 };
	KEYANIMANAGER->addArrayFrameAnimation("player_Olaf", "move_Right", "olaf", move_Right, 8, 10, true);
	int move_Left[] = { 41, 42, 43, 44, 45, 46, 47, 48 };
	KEYANIMANAGER->addArrayFrameAnimation("player_Olaf", "move_Left", "olaf", move_Left, 8, 10, true);
	int idle_Shield_Right[] = { 99 };
	KEYANIMANAGER->addArrayFrameAnimation("player_Olaf", "idle_Shield_Right", "olaf", idle_Shield_Right, 1, 10, true);
	int idle_Shield_Left[] = { 127 };
	KEYANIMANAGER->addArrayFrameAnimation("player_Olaf", "idle_Shield_Left", "olaf", idle_Shield_Left, 1, 10, true);
	int move_Shield_Right[] = { 103, 104, 105, 106, 107, 108, 109, 110 };
	KEYANIMANAGER->addArrayFrameAnimation("player_Olaf", "move_Shield_Right", "olaf", move_Shield_Right, 8, 10, true);
	int move_Shield_Left[] = { 131, 132, 133, 134, 135, 136, 137, 138 };
	KEYANIMANAGER->addArrayFrameAnimation("player_Olaf", "move_Shield_Left", "olaf", move_Shield_Left, 8, 10, true);
	int push_Wall_Right[] = { 58, 59, 60, 61 };
	KEYANIMANAGER->addArrayFrameAnimation("player_Olaf", "push_Wall_Right", "olaf", push_Wall_Right, 4, 5, true);
	int push_Wall_Left[] = { 62, 63, 64, 65 };
	KEYANIMANAGER->addArrayFrameAnimation("player_Olaf", "push_Wall_Left", "olaf", push_Wall_Left, 4, 5, true);
	int ladder_Up[] = { 66, 67, 68, 69 };
	KEYANIMANAGER->addArrayFrameAnimation("player_Olaf", "ladder_Up", "olaf", ladder_Up, 4, 10, true);
	int ladder_Down[] = { 69, 68, 67, 66 };
	KEYANIMANAGER->addArrayFrameAnimation("player_Olaf", "ladder_Down", "olaf", ladder_Down, 4, 10, true);
	int ladder_End[] = { 70, 71 };
	KEYANIMANAGER->addArrayFrameAnimation("player_Olaf", "ladder_End", "olaf", ladder_End, 2, 5, false);
	int fall_Right[] = { 21, 22 };
	KEYANIMANAGER->addArrayFrameAnimation("player_Olaf", "fall_Right", "olaf", fall_Right, 2, 10, false);
	int fall_Left[] = { 49, 50 };
	KEYANIMANAGER->addArrayFrameAnimation("player_Olaf", "fall_Left", "olaf", fall_Left, 2, 10, false);
	int fire_Death_Right[] = { 91, 92, 93, 94, 95, 96, 97, 98 };
	KEYANIMANAGER->addArrayFrameAnimation("player_Olaf", "fire_Death_Right", "olaf", fire_Death_Right, 8, 10, false);
	int fire_Death_Left[] = { 119, 120, 121, 122, 123, 124, 125, 126 };
	KEYANIMANAGER->addArrayFrameAnimation("player_Olaf", "fire_Death_Left", "olaf", fire_Death_Left, 8, 10, false);
	int fall_Shield_Right[] = { 141, 142 };
	KEYANIMANAGER->addArrayFrameAnimation("player_Olaf", "fall_Shield_Right", "olaf", fall_Shield_Right, 2, 5, true);
	int fall_Shield_Left[] = { 145, 146 };
	KEYANIMANAGER->addArrayFrameAnimation("player_Olaf", "fall_Shield_Left", "olaf", fall_Shield_Left, 2, 5, true);
	int fall_Shield_Move_Right[] = { 143, 144 };
	KEYANIMANAGER->addArrayFrameAnimation("player_Olaf", "fall_Shield_Move_Right", "olaf", fall_Shield_Move_Right, 2, 5, true);
	int fall_Shield_Move_Left[] = { 147, 148 };
	KEYANIMANAGER->addArrayFrameAnimation("player_Olaf", "fall_Shield_Move_Left", "olaf", fall_Shield_Move_Left, 2, 5, true);
	int idle_Special_Right[] = { 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
	KEYANIMANAGER->addArrayFrameAnimation("player_Olaf", "idle_Special_Right", "olaf", idle_Special_Right, 11, 5, false);
	int idle_Special_Left[] = { 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40 };
	KEYANIMANAGER->addArrayFrameAnimation("player_Olaf", "idle_Special_Left", "olaf", idle_Special_Left, 11, 5, false);
	int idle_Shield_Special_Right[] = { 100, 101, 102 };
	KEYANIMANAGER->addArrayFrameAnimation("player_Olaf", "idle_Shield_Special_Right", "olaf", idle_Shield_Special_Right, 3, 5, false);
	int idle_Shield_Special_Left[] = { 128, 129, 130 };
	KEYANIMANAGER->addArrayFrameAnimation("player_Olaf", "idle_Shield_Special_Left", "olaf", idle_Shield_Special_Left, 3, 5, false);


	_olafMotion = KEYANIMANAGER->findAnimation("player_Olaf", "idle_Right");
}

void player_Olaf::initShield()
{
	_isShieldUp = false;																//방패들었니?
	_shieldRect = RectMake(_playerRect.right - 10, _playerRect.top, 10, 70);				//방패렉트생성
}

void player_Olaf::stateOlaf()
{
	switch (_state)
	{
		case PLAYER_IDLE_RIGHT:
		case PLAYER_IDLE_LEFT:
		case PLAYER_SHIELD_IDLE_RIGHT:
		case PLAYER_SHIELD_IDLE_LEFT:
			break;

		case PLAYER_PUSH_WALL_RIGHT:
		case PLAYER_PUSH_WALL_LEFT:
		{
			_speed = 0.f;
			break;
		}

		case PLAYER_MOVE_RIGHT:
		case PLAYER_SHIELD_MOVE_RIGHT:
		{
			if (!_isRightCollision)
			{
				if (_speed < _maxSpeed)
					_speed += _accleration * TIMEMANAGER->getElpasedTime();
				else
					_speed = _maxSpeed;
			}
			_x += _speed;
			break;
		}
		case PLAYER_MOVE_LEFT:
		case PLAYER_SHIELD_MOVE_LEFT:
		{
			if (!_isLeftCollision)
			{
				if (_speed < _maxSpeed)
					_speed += _accleration * TIMEMANAGER->getElpasedTime();
				else
					_speed = _maxSpeed;
			}
			_x -= _speed;
			break;
		}
		case PLAYER_LADDER_UP:
		{
			_speed = _maxSpeed;
			_y -= _speed;
			break;
		}
		case PLAYER_LADDER_DOWN:
		{
			_speed = _maxSpeed;
			_y += _speed;
			break;
		}
		case PLAYER_FALL_RIGHT:
		{

			if (_isRightCollision)
				_speed = 0;
			else
				_speed = _minSpeed;
			_gravity = GRAVITY;
			_y += _gravity * TIMEMANAGER->getElpasedTime();
			if (_state == PLAYER_FALL_RIGHT && KEYMANAGER->isStayKeyDown(VK_RIGHT))
				_x += _speed;
			if (_isFloor)
			{
				if (_state == PLAYER_FALL_RIGHT)
				{
					_state = PLAYER_IDLE_RIGHT;
					startAniOlaf("idle_Right");
				}
			}
			break;
		}
		case PLAYER_FALL_LEFT:
		{
			pixelBottomCollision();
			if (_isLeftCollision)
				_speed = 0;
			else
				_speed = _minSpeed;
			_gravity = GRAVITY;
			_y += _gravity * TIMEMANAGER->getElpasedTime();
			if (_state == PLAYER_FALL_LEFT && KEYMANAGER->isStayKeyDown(VK_LEFT))
				_x -= _speed;
			if (_isFloor)
			{
				if (_state == PLAYER_FALL_LEFT)
				{
					_state = PLAYER_IDLE_LEFT;
					startAniOlaf("idle_Left");
				}
			}
			break;
		}
		case PLAYER_SHIELD_FALL_RIGHT:
		case PLAYER_SHIELD_FALL_LEFT:
		{
			if (!_isFloor)
			{
				_gravity = _shieldGravity;
				_y += _gravity * TIMEMANAGER->getElpasedTime();
			}
			else if (_isFloor)
			{
				if (_state == PLAYER_SHIELD_FALL_RIGHT)
				{
					_state = PLAYER_SHIELD_IDLE_RIGHT;
					startAniOlaf("idle_Shield_Right");
				}
				else if (_state == PLAYER_SHIELD_FALL_LEFT)
				{
					_state = PLAYER_SHIELD_IDLE_LEFT;
					startAniOlaf("idle_Shield_Left");
				}
			}
			break;
		}
		case PLAYER_SHIELD_FALL_MOVE_RIGHT:
		{
			pixelRightWallCollision();
			if (_isRightCollision)
				_speed = 0;
			else
				_speed = _maxSpeed;
			_gravity = _shieldGravity;
			_y += _gravity * TIMEMANAGER->getElpasedTime();
			_x += _speed;
			if (_isFloor && _state == PLAYER_SHIELD_FALL_MOVE_RIGHT)
			{
				_state = PLAYER_SHIELD_IDLE_RIGHT;
				startAniOlaf("idle_Shield_Right");

			}
			break;
		}
		case PLAYER_SHIELD_FALL_MOVE_LEFT:
		{
			pixelLeftWallCollision();
			if (_isLeftCollision)
				_speed = 0;
			else
				_speed = _maxSpeed;
			_gravity = _shieldGravity;
			_y += _gravity * TIMEMANAGER->getElpasedTime();
			_x -= _speed;
			if (_isFloor && _state == PLAYER_SHIELD_FALL_MOVE_LEFT)
			{
				_state = PLAYER_SHIELD_IDLE_LEFT;
				startAniOlaf("idle_Shield_Left");

			}
			break;
		}
	}
}

void player_Olaf::specialMotion_Olaf()
{
	if (_specialTime >= 900)
	{
		_specialTime = 0;
		if (!_isAniStart && !_isLadder)
		{
			if (_state == PLAYER_IDLE_RIGHT)
			{
				_state = PLAYER_IDLE_SPECIAL_RIGHT;
				startAniOlaf("idle_Special_Right");
				_isAniStart = true;
			}
			else if (_state == PLAYER_IDLE_LEFT)
			{
				_state = PLAYER_IDLE_SPECIAL_LEFT;
				startAniOlaf("idle_Special_Left");
				_isAniStart = true;
			}
			else if (_state == PLAYER_SHIELD_IDLE_RIGHT)
			{
				_state = PLAYER_SHIELD_IDLE_SPECIAL_RIGHT;
				startAniOlaf("idle_Shield_Special_Right");
				_isAniStart = true;
			}
			else if (_state == PLAYER_SHIELD_IDLE_LEFT)
			{
				_state = PLAYER_SHIELD_IDLE_SPECIAL_LEFT;
				startAniOlaf("idle_Shield_Special_Left");
				_isAniStart = true;
			}
		}
	}
	else if (_isAniStart && _olafMotion->isPlay() == false)
	{
		_specialTime = 0;
		_isAniStart = false;
		if (_state == PLAYER_IDLE_SPECIAL_RIGHT)
		{
			_state = PLAYER_IDLE_RIGHT;
			startAniOlaf("idle_Right");
			_isAniStart = false;

		}
		else if (_state == PLAYER_IDLE_SPECIAL_LEFT)
		{
			_state = PLAYER_IDLE_LEFT;
			startAniOlaf("idle_Left");
			_isAniStart = false;
		}
		else if (_state == PLAYER_SHIELD_IDLE_SPECIAL_RIGHT)
		{
			_state = PLAYER_SHIELD_IDLE_RIGHT;
			startAniOlaf("idle_Shield_Right");
			_isAniStart = false;
		}
		else if (_state == PLAYER_SHIELD_IDLE_SPECIAL_LEFT)
		{
			_state = PLAYER_SHIELD_IDLE_LEFT;
			startAniOlaf("idle_Shield_Left");
			_isAniStart = false;
		}
	}
}

void player_Olaf::stateShield()
{
	//방패를 위로 안들었다면
	if (!_isShieldUp)
	{
		//플레이어가 오른쪽을 보고 있다면
		if (_state == PLAYER_IDLE_RIGHT || _state == PLAYER_MOVE_RIGHT)
		{
			_shieldRect = RectMake(_playerRect.right - 10, _playerRect.top, 10, 70);			//오른쪽으로 방패렉트를 갱신한다.
		}
		//플레이어가 왼쪽을 보고 있다면
		if (_state == PLAYER_IDLE_LEFT || _state == PLAYER_MOVE_LEFT)
		{
			_shieldRect = RectMake(_playerRect.left, _playerRect.top, 10, 70);				//왼쪽으로 방패렉트를 갱신한다.
		}
	}
	//방패를 위로 들었다면
	else
	{
		//플레이어가 오른쪽을 보고 있다면
		if (_state == PLAYER_SHIELD_IDLE_RIGHT || _state == PLAYER_SHIELD_MOVE_RIGHT)
		{
			_shieldRect = RectMake(_playerRect.right - 60, _playerRect.top + 5, 55, 10);			//오른쪽으로 방패렉트를 갱신한다.
		}
		//플레이어가 왼쪽을 보고 있다면
		if (_state == PLAYER_SHIELD_IDLE_LEFT || _state == PLAYER_SHIELD_MOVE_LEFT)
		{
			_shieldRect = RectMake(_playerRect.left + 5, _playerRect.top + 5, 55, 10);				//왼쪽으로 방패렉트를 갱신한다.
		}
	}
}

void player_Olaf::startAniOlaf(string aniKey)
{
	_olafMotion = KEYANIMANAGER->findAnimation("player_Olaf", aniKey);
	KEYANIMANAGER->start("player_Olaf", aniKey);

}

void player_Olaf::pixelLeftWallCollision()
{
	for (int i = _proveLeft + 5; i > _proveLeft; --i)
	{
		COLORREF color = GetPixel(_pixelData->getMemDC(), i, _y);

		int r = GetRValue(color);
		int g = GetGValue(color);
		int b = GetBValue(color);

		if (r == 0 && g == 255 && b == 255)
		{										//스피드를 0으로 내려 이동하지 않도록 한다.
			_isLeftCollision = true;
		}
		else
		{
			_isLeftCollision = false;
		}
	}
}

void player_Olaf::pixelRightWallCollision()
{
	for (int i = _proveRight - 5; i < _proveRight; ++i)
	{
		COLORREF color = GetPixel(_pixelData->getMemDC(), i, _y);

		int r = GetRValue(color);
		int g = GetGValue(color);
		int b = GetBValue(color);

		if (r == 0 && g == 255 && b == 255)
		{
			//스피드를 0으로 내려 이동하지 않도록 한다.
			_isRightCollision = true;
		}
		else
		{
			_isRightCollision = false;
		}
	}
}

void player_Olaf::pixelBottomCollision()
{
	if (_state == PLAYER_LADDER_UP || _state == PLAYER_LADDER_DOWN || _state == PLAYER_LADDER_END)
	{
		return;
	}
	for (int i = _proveBottom - 10; i < _proveBottom + 10; ++i)
	{
		COLORREF color = GetPixel(_pixelData->getMemDC(), _x, i);

		int r = GetRValue(color);
		int g = GetGValue(color);
		int b = GetBValue(color);

		if (r == 255 && g == 0 && b == 255)
		{
			_y = i - 35;										   //플레이어 좌표 보정
			_isFloor = true;
			break;
		}
		else if (!(r == 255 && g == 0 && b == 255))
		{
			_isFloor = false;
			_isLadder = false;
		}
	}
}

void player_Olaf::pixelFireCollision()
{
	for (int i = _playerRect.bottom - 10; i < _playerRect.bottom + 10; ++i)
	{
		COLORREF color = GetPixel(_pixelData->getMemDC(), _x, i);

		int r = GetRValue(color);
		int g = GetGValue(color);
		int b = GetBValue(color);

		if (r == 255 && g == 0 && b == 0)
		{
			if (_state == PLAYER_IDLE_RIGHT || _state == PLAYER_MOVE_RIGHT ||
				_state == PLAYER_FALL_RIGHT || _state == PLAYER_HIGH_FALL_RIGHT ||
				_state == PLAYER_SHIELD_IDLE_RIGHT || _state == PLAYER_SHIELD_MOVE_RIGHT ||
				_state == PLAYER_SHIELD_FALL_RIGHT || _state == PLAYER_SHIELD_FALL_MOVE_RIGHT)
			{
				_state = PLAYER_FIRE_DEATH_RIGHT;
				startAniOlaf("fire_Death_Right");
			}
			if (_state == PLAYER_IDLE_LEFT || _state == PLAYER_MOVE_LEFT ||
				_state == PLAYER_FALL_LEFT || _state == PLAYER_HIGH_FALL_LEFT ||
				_state == PLAYER_SHIELD_IDLE_LEFT || _state == PLAYER_SHIELD_MOVE_LEFT ||
				_state == PLAYER_SHIELD_FALL_LEFT || _state == PLAYER_SHIELD_FALL_MOVE_LEFT)
			{
				_state = PLAYER_FIRE_DEATH_LEFT;
				startAniOlaf("fire_Death_Left");
			}
			_y = i - 35;

			_isFloor = false;
			_isLadder = false;
			_lifeCount = 0;
			_speed = 0;
			_deathMotion = true;

			if (_deathMotion && _olafMotion->isPlay() == false)
			{
				_isAlive = false;
			}
			break;
		}
	}
}

void player_Olaf::playerCollisionLadder()
{
	RECT temp;
	int size = _ladderRc.size();
	for (int i = 0; i < size; i++)
	{

		if (IntersectRect(&temp, _ladderRc[i], &_playerRect))
		{
			//사다리 맨위에 있을때
			if (_playerRect.bottom - 5 <= (*_ladderRc[i]).top)
			{
				
					_isLadderTop = true;
					_isLadderBottom = false;
					_isLadder = true;
			}
			//사다리 밑에 있을때
			else if (_playerRect.bottom + 5 >= (*_ladderRc[i]).bottom)
			{
				_isLadderTop = false;
				_isLadderBottom = true;
				_isLadder = true;
			}
			//그 외(오르락 내리락 할때)
			else
			{
				if ((*_ladderRc[i]).left)
				{
					if (_state != PLAYER_FALL_RIGHT && _state != PLAYER_FALL_LEFT && _state != PLAYER_SHIELD_FALL_RIGHT &&
						_state != PLAYER_SHIELD_FALL_LEFT && _state != PLAYER_SHIELD_FALL_MOVE_RIGHT &&
						_state != PLAYER_SHIELD_FALL_MOVE_LEFT)
						_x = (*_ladderRc[i]).left;
				}
				_isLadderTop = false;
				_isLadderBottom = false;
				_isLadder = true;
			}
		}
	}
}

void player_Olaf::takeDamage(int damage)
{
	_lifeCount -= damage;
	if (_lifeCount <= 0)
	{
		_isAlive = false;
		_mainUI->setOlafUIIsAlive(false);
	}
	_mainUI->setOlafUIHP(_lifeCount);
}
