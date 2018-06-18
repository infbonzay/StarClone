#if !defined(W_DRAWITEM)
#define W_DRAWITEM

#include <stdint.h>
#include "pcx.h"

#define DRAWITEM_FLAG_VISIBILITY		0x01
#define DRAWITEM_FLAG_SCRIPTWORKABILITY		0x02

class DrawItem
{
protected:
    uint8_t	flags;		
    int32_t	xpos;		//screen x position
    int32_t	ypos;		//screen y position
    int32_t	speedx;
    int32_t	speedy;
    int32_t	accelerate;
    int		tempval1;
    int		tempval2;
    int 	(DrawItem::*ScriptFunc)(void);
public:

		DrawItem();
		~DrawItem();
    
//    void 	Draw(void);
    void 	SetFlags(uint8_t flags);
    inline void EnableVisible(void) { SetFlags(flags |= DRAWITEM_FLAG_VISIBILITY); };
    inline void DisableVisible(void) { SetFlags(flags &= ~DRAWITEM_FLAG_VISIBILITY); };
    inline void EnableScriptWork(void) { SetFlags(flags |= DRAWITEM_FLAG_SCRIPTWORKABILITY); };
    inline void DisableScriptWork(void) { SetFlags(flags &= ~DRAWITEM_FLAG_SCRIPTWORKABILITY); };
    inline void	SetScript( int (DrawItem::*FuncAdr)(void) ) { ScriptFunc = FuncAdr; };
    inline int	IfCanDraw(void) { return flags & DRAWITEM_FLAG_VISIBILITY; }; 
    inline int	IfCanScriptWork(void) { return flags & DRAWITEM_FLAG_SCRIPTWORKABILITY; }; 
    inline void SetXYPos(int x,int y) { xpos = x; ypos = y; };
    int  	DoScript(void);
    void	SetParams(uint32_t spx, uint32_t spy, uint32_t acc);
    void 	SetTempVars(void);
    int		SimpleScript(void);
    int		SimpleScriptCalcMaxDistance(void);

};

class DrawItemPcx : public DrawItem
{
protected:
    uint8_t	holecolor;	//hole color
    uint8_t	transpcolor;	//transparentcolor
    uint8_t	grdtransp;	//transparent factor 0-3 25,50,75,100
    PCX		*obj;		//object to draw
public:    
		DrawItemPcx(PCX *pcx);
		~DrawItemPcx();

    void 	Draw(void);
    void 	SetPcxParam(uint8_t color1,uint8_t color2,uint8_t color3);

};


#endif
