#include "stdafx.h"
#include "playerBase.h"
#include "objectManager.h"
#include "mainUI.h"


playerBase::playerBase()
{
}


playerBase::~playerBase()
{
}

HRESULT playerBase::init()
{
	_shieldRect = {};
	_proveBottom = (float)_playerRect.bottom;
	for (int i = 0; i < 4; i++)
	{
		_invenItem[i] = nullptr;
	}
	return S_OK;
}

void playerBase::release()
{
}

void playerBase::update()
{
}

void playerBase::render()
{


}

void playerBase::keyUpdate()
{
	keyPressMove();
	keyPressSpace();
	keyPressD();
	keyPressE();
}

void playerBase::keyPressMove()
{
	if (KEYMANAGER->isStayKeyDown(VK_LEFT))
	{
		_x -= _speed;
	}
	if (KEYMANAGER->isStayKeyDown(VK_RIGHT))
	{
		_x += _speed;
	}

}

void playerBase::keyPressSpace()
{
}

void playerBase::keyPressD()
{
}

void playerBase::keyPressE()
{
}

void playerBase::keyPressS()
{
}





void playerBase::pixelLeftWallCollision()
{
}

void playerBase::pixelRightWallCollision()
{
}


void playerBase::rectBrokenWallCollision()
{
}

void playerBase::pixelTopWallCollision()
{
}

void playerBase::pixelBottomCollision()
{

}

void playerBase::playerCollisionLadder(object* ladder)
{

}

void playerBase::setLadderRectAdressLink(RECT * rc)
{
	_ladderRc.push_back(rc);
}

void playerBase::shieldRender(HDC hdc)
{
}

void playerBase::setObjectRectAdressLink(object * rc)
{
	_objectRc.push_back(rc);
}

void playerBase::setObjectManagerAdressLink(objectManager * objm)
{
	_objm = objm;
}

void playerBase::takeDamage(int damage)
{
	
}
