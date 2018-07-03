
#include <stdlib.h>

//#include "auxil.h"
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
int MoveItem::Move(void)
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
    if (tempval1 >= MAXACCEL || tempval1 < 0)
    {
	tempval1 -= tempval2;
    }
    else
	return(0);
    tempval2 = -tempval2;
    speedx = -speedx;
    speedy = -speedy;
    DisableMoveScript();
    return(1);
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
    {
	delete moveaction;				//delet move obj
	moveaction = NULL;
    }
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
/*int SimpleMoveScript(MoveItem *item)
{
    item->accelerate = accelertable[ item->tempval1 ];
    if (item->speedx > 0)
	item->parent->xpos += item->accelerate;
    else 
	if (item->speedx < 0)
	    item->parent->xpos -= item->accelerate;
    if (item->speedy > 0)
	item->parent->ypos += item->accelerate;
    else 
	if (item->speedy < 0)
	    item->parent->ypos -= item->accelerate;
    item->tempval1 += item->tempval2;
    if (item->tempval1 >= MAXACCEL)
    {
	item->tempval1--;
    }else if (item->tempval1 < 0)
    {
	item->tempval1++;
    }
    else
	return(0);
    item->tempval2 = -item->tempval2;
    item->speedx = -item->speedx;
    item->speedy = -item->speedy;
    item->DisableMoveScript();
    return(1);
}
*/
//=================================================================


