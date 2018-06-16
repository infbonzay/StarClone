
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
}
//=================================================================
void DrawItem::SetFlags(uint8_t flgs)
{
    flags = flgs;
}
//=================================================================
void DrawItem::DoScript(DrawItem *obj)
{
    if ( IfCanScriptWork() )
    {
	if ( ScriptFunc )
	{
	    (*ScriptFunc)(obj);
	}
    }
};
//=================================================================
void DrawItem::SetSpeedParam(uint32_t spx,uint32_t spy,uint32_t acc)
{
    speedx = spx;
    speedy = spy;
    accelerate = acc;
}
//=================================================================
int DrawItem::CalcMaxDistance(void)
{
    int total = 0;
    int speed = max( abs(speedx), abs(speedy ) );
    while (speed > 0)
    {
	total += speed;
	speed -= accelerate;
    }
    return(total);
}
//=================================================================
//=================================================================
DrawItemPcx::DrawItemPcx(PCX *pcx) : DrawItem()
{
    obj = pcx;
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
int DrawItemPcx::Script1(void)
{
    if (!IfCanScriptWork())
	return(1);
    if (speedx == 0 && speedy == 0)
	return(1);
    xpos += speedx;
    ypos += speedy;
    if (speedx > 0)
    {
	speedx -= accelerate;
	if ( speedx <= 0 )
	{
	    DisableScriptWork();
	    return(1);	    
	}
    }
    else
	if (speedx < 0)
	{
	    speedx += accelerate;
	    if ( speedx >= 0 )
	    {
		DisableScriptWork();
		return(1);	    
	    }
	}
    {
    if (speedy > 0)
    {
	    speedy -= accelerate;
	    if ( speedy <= 0 )
	    {
		DisableScriptWork();
		return(1);
	    }
    }
    else
	if (speedy < 0)
	{
	    speedy += accelerate;
	    if ( speedy >= 0 )
	    {
		DisableScriptWork();
		return(1);
	    }
	}
    }
    return(0);	    
}
//=================================================================


