
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
	    return (*ScriptFunc)(this);
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


