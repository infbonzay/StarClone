
#include <stdlib.h>

#include "auxil.h"
#include "pcx.h"
#include "MenuItem.h"

//=================================================================
MoveItem::MoveItem(void)
{
}
//=================================================================
MoveItem::~MoveItem(void)
{
}
//=================================================================
int MoveItem::MoveScript(void)
{
    if ( IsEnabledMoveScript() )
    {
	if ( MoveScriptFunc )
	{
	    return  (this->*MoveItem::MoveScriptFunc)() ;
	}
    }
    return(1);	//return end of script work
}
//=================================================================
//#define MAXACCEL 17
//const int accelertable[MAXACCEL] = {1,1,2,3,5,8,12,16,21,29,40,52,65,79,94,110,128};
#define MAXACCEL 13
const uint8_t accelertable[MAXACCEL] = {1,1,2,3,5,8,13,21,34,55,89,144,233};
//=================================================================
void MoveItem::SetTempVars(void)
{
    tempval1 = MAXACCEL - 1;
    tempval2 = -1;
}
//=================================================================
int MoveItem::SimpleScriptCalcMaxDistance(void)
{
//    return(1+1+2+3+4+5+13+21+34+55+89+144);
    return(1+1+2+3+5+8+13+21+34+55+89+144+233);
}
//=================================================================
void MoveItem::SetParams(uint32_t spx,uint32_t spy,uint32_t acc)
{
    speedx = spx;
    speedy = spy;
    accelerate = acc;
}
//=================================================================
int MoveItem::SimpleMoveScript(void)
{
    accelerate = accelertable[ tempval1 ];
    if (speedx > 0)
	parent->xpos += accelerate;
    else 
	if (speedx < 0)
	    parent->xpos -= accelerate;
    if (speedy > 0)
	parent->ypos += accelerate;
    else 
	if (speedy < 0)
	    parent->ypos -= accelerate;
    tempval1 += tempval2;
    if (tempval1 >= MAXACCEL)
    {
	tempval2 = -tempval2;
	speedx = -speedx;
	speedy = -speedy;
	tempval1--;
	DisableMoveScript();
	return(1);
    }
    if (tempval1 < 0)
    {
	tempval2 = -tempval2;
	speedx = -speedx;
	speedy = -speedy;
	tempval1++;
	DisableMoveScript();
	return(1);
    }
    return(0);
}
//=================================================================
MenuItem::MenuItem(void)
{
    SetFlags(0x00);
    SetXYPos(0,0);
}
//=================================================================
MenuItem::~MenuItem()
{
    if (moveaction)
	delete moveaction;				//delet move obj
}
//=================================================================
void MenuItem::AddMoveAction(void)
{
    moveaction = new MoveItem();			//add move obj
    moveaction->parent = this;

    moveaction->SetMoveScript(NULL);
    moveaction->SetTempVars();
    moveaction->SetParams(0,0,0);
}
//=================================================================
MenuItemPcx::MenuItemPcx(PCX *pcx) : MenuItem()
{
    obj = pcx;
}
//=================================================================
MenuItemPcx::~MenuItemPcx()
{
}
//=================================================================
void MenuItemPcx::SetPcxParam(uint8_t color1,uint8_t color2,uint8_t color3)
{
    holecolor = color1;
    transpcolor = color2;
    grdtransp = color3;
}
//=================================================================
void MenuItemPcx::Draw(void)
{
    if ( IsVisibled() )
    {
        obj->PutPcx(xpos, ypos, holecolor, transpcolor, grdtransp);
    }
}
//=================================================================


