
#include <stdlib.h>

#include "pcx.h"
#include "MenuItem.h"

//=================================================================
//#define MAXACCEL 17
//const int accelertable[MAXACCEL] = {1,1,2,3,5,8,12,16,21,29,40,52,65,79,94,110,128};
#define MAXACCEL 13
const uint8_t accelertable[MAXACCEL] = { 1,1,2,3,5,8,13,21,34,55,89,144,233 };
//=================================================================
MoveItem::MoveItem(MenuItem *parent, uint32_t speedX, uint32_t speedY, uint32_t accelerate)
{
	Parent = parent;
	_moveScriptFunc = &MoveItem::SimpleMoveScript;
	SetParams(speedX, speedY, accelerate);
	_tempval1 = MAXACCEL - 1;
	_tempval2 = -1;
}
//=================================================================
MoveItem::~MoveItem(void)
{
}
//=================================================================
int MoveItem::Move(void)
{
	if (IsEnableMoveScript())
	{
		if (_moveScriptFunc)
		{
			return	(this->*MoveItem::_moveScriptFunc)();
		}
	}
	return(1);	//return end of script work
}
//=================================================================
int MoveItem::SimpleScriptCalcMaxDistance(void)
{
	//	  return(1+1+2+3+4+5+13+21+34+55+89+144);
	return(1 + 1 + 2 + 3 + 5 + 8 + 13 + 21 + 34 + 55 + 89 + 144 + 233);
}
//=================================================================
void MoveItem::SetParams(uint32_t speedX, uint32_t speedY, uint32_t accelerateXY)
{
	_speedx = speedX;
	_speedy = speedY;
	_accelerate = accelerateXY;
}
//=================================================================
int MoveItem::SimpleMoveScript(void)
{
	_accelerate = accelertable[_tempval1];
	if (_speedx > 0)
		Parent->AddX(_accelerate);
	else
		if (_speedx < 0)
			Parent->AddX(-_accelerate);
	if (_speedy > 0)
		Parent->AddY(_accelerate);
	else
		if (_speedy < 0)
			Parent->AddY(-_accelerate);
	_tempval1 += _tempval2;
	if (_tempval1 >= MAXACCEL || _tempval1 < 0)
	{
		_tempval1 -= _tempval2;
	}
	else
		return(0);
	_tempval2 = -_tempval2;
	_speedx = -_speedx;
	_speedy = -_speedy;
	DisableMoveScript();
	return(1);
}
//=================================================================
MenuItem::MenuItem(void)
{
	SetFlags(0x00);
	SetXYPos(0, 0);
	this->moveaction = NULL;
	this->AddMoveAction(&MoveItem::SimpleMoveScript, 0, 0, 0);
}
//=================================================================
MenuItem::~MenuItem()
{
	this->RemoveMoveAction();
}
//=================================================================
void MenuItem::RemoveMoveAction()
{
	if (moveaction)
	{
		delete moveaction;
		moveaction = NULL;
	}
}
//=================================================================
void MenuItem::AddMoveAction(int (MoveItem::*moveFuncAdr)(void), int speedX, int speedY, int accelerate)
{
	this->RemoveMoveAction();
	moveaction = new MoveItem(this, speedX, speedY, accelerate);
	moveaction->SetMoveScript(moveFuncAdr);
}
//=================================================================
MenuItemPcx::MenuItemPcx(PCX *pcx) : MenuItem()
{
	_pcx = pcx;
}
//=================================================================
MenuItemPcx::~MenuItemPcx()
{
}
//=================================================================
void MenuItemPcx::SetPcxParam(uint8_t color1, uint8_t color2, uint8_t color3)
{
	_holecolor = color1;
	_transpcolor = color2;
	_grdtransp = color3;
}
//=================================================================
void MenuItemPcx::Draw(void)
{
	if (IsVisible())
	{
		_pcx->PutPcx(_xpos, _ypos, _holecolor, _transpcolor, _grdtransp);
	}
}
//=================================================================
