#if !defined(W_DRAWITEM)
#define W_DRAWITEM

#include <stdint.h>
#include "pcx.h"

#define DRAWITEM_FLAG_VISIBILITY		0x01

#define MOVEITEM_FLAG_MOVESCRIPT		0x01

//=================================================================
class DrawItem;

class MoveItem
{
public:
    DrawItem	*parent;
protected:
    uint8_t	flags;
    int32_t	speedx;
    int32_t	speedy;
    int32_t	accelerate;
    int		tempval1;
    int		tempval2;
    int 	(MoveItem::*MoveScriptFunc)(void);
    
public:
		MoveItem();
		~MoveItem();
    inline void	SetFlags(uint8_t flgs) { flags = flgs; };
    inline void	SetMoveScript( int (MoveItem::*FuncAdr)(void) ) { MoveScriptFunc = FuncAdr; };
    inline void EnableMoveScript(void) { SetFlags(flags |= MOVEITEM_FLAG_MOVESCRIPT); };
    inline void DisableMoveScript(void) { SetFlags(flags &= ~MOVEITEM_FLAG_MOVESCRIPT); };
    inline int	IsEnabledMoveScript(void) { return flags & MOVEITEM_FLAG_MOVESCRIPT; }; 
    int  	MoveScript(void);
    void 	SetTempVars(void);
    int		SimpleMoveScript(void);
    int		SimpleScriptCalcMaxDistance(void);
    void	SetParams(uint32_t spx, uint32_t spy, uint32_t acc);
    
};
//=================================================================
class DrawItem
{
protected:
    uint8_t	flags;		
public:
    MoveItem	*moveaction;
    int32_t	xpos;		//screen x position
    int32_t	ypos;		//screen y position

		DrawItem();
		~DrawItem();
    
//    void 	Draw(void);
    void	AddMoveAction(void);
    inline void	SetFlags(uint8_t flgs) { flags = flgs; };
    inline void EnableVisible(void) { SetFlags(flags |= DRAWITEM_FLAG_VISIBILITY); };
    inline void DisableVisible(void) { SetFlags(flags &= ~DRAWITEM_FLAG_VISIBILITY); };
    inline int	IsVisibled(void) { return flags & DRAWITEM_FLAG_VISIBILITY; }; 
    inline void SetXYPos(int x,int y) { xpos = x; ypos = y; };

};
//=================================================================
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
//=================================================================


#endif
