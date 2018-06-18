
#include <stdlib.h>

#include "auxil.h"
#include "pcx.h"
#include "DrawItem.h"

//=================================================================
DrawItem::DrawItem(void)
{
    SetScript(NULL);
    SetFlags(0x00);
    SetXYPos(0,0);
    speedx = 0;
    speedy = 0;
    accelerate = 0;
    tempval1 = 0;
    tempval2 = 0;
}
//=================================================================
DrawItem::~DrawItem()
{
}
//=================================================================
void DrawItem::SetFlags(uint8_t flgs)
{
    flags = flgs;
}
//=================================================================
int DrawItem::DoScript(void)
{
    if ( IfCanScriptWork() )
    {
	if ( ScriptFunc )
	{
	    return  (this->*DrawItem::ScriptFunc)() ;
	}
    }
    return(0);
}
//=================================================================
void DrawItem::SetSpeedParam(uint32_t spx,uint32_t spy,uint32_t acc)
{
    speedx = spx;
    speedy = spy;
    accelerate = acc;
}
//=================================================================
//#define MAXACCEL 17
//const int accelertable[MAXACCEL] = {1,1,2,3,5,8,12,16,21,29,40,52,65,79,94,110,128};
#define MAXACCEL 12
const uint8_t accelertable[MAXACCEL] = {1,1,2,3,5,8,13,21,34,55,89,144};
//=================================================================
int DrawItem::SimpleScriptCalcMaxDistance(void)
{
    tempval1 = MAXACCEL - 1;
    tempval2 = -1;
//    return(1+1+2+3+4+5+13+21+34+55+89+144);
    return(1+1+2+3+5+8+13+21+34+55+89+144);
}
//=================================================================
int DrawItem::SimpleScript(void)
{
    accelerate = accelertable[ tempval1 ];
    if (speedx > 0)
	xpos += accelerate;
    else 
	if (speedx < 0)
	    xpos -= accelerate;
    if (speedy > 0)
	ypos += accelerate;
    else 
	if (speedy < 0)
	    ypos -= accelerate;
    tempval1 += tempval2;
    if (tempval1 >= MAXACCEL)
    {
	tempval2 = -tempval2;
	speedx = -speedx;
	speedy = -speedy;
	tempval1--;
	DisableScriptWork();
	return(1);
    }
    if (tempval1 < 0)
    {
	tempval2 = -tempval2;
	speedx = -speedx;
	speedy = -speedy;
	tempval1++;
	DisableScriptWork();
	return(1);
    }
    return(0);
}
//=================================================================
//=================================================================
DrawItemPcx::DrawItemPcx(PCX *pcx) : DrawItem()
{
    obj = pcx;
}
//=================================================================
DrawItemPcx::~DrawItemPcx()
{
}
//=================================================================
void DrawItemPcx::SetPcxParam(uint8_t color1,uint8_t color2,uint8_t color3)
{
    holecolor = color1;
    transpcolor = color2;
    grdtransp = color3;
}
//=================================================================
void DrawItemPcx::Draw(void)
{
    if ( IfCanDraw() )
    {
        obj->PutPcx(xpos, ypos, holecolor, transpcolor, grdtransp);
    }
}
//=================================================================


