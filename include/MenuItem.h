#ifndef _SC_MENUITEM_H

#define _SC_MENUITEM_H

#include <stdint.h>
#include "pcx.h"

#define MENUITEM_FLAG_VISIBILITY				0x01

#define MOVEITEM_FLAG_MOVESCRIPT				0x01

//=================================================================
class MenuItem;

class MoveItem
{
public:
	MenuItem	*Parent;
private:
	uint8_t		_flags;
	int32_t		_speedx;
	int32_t		_speedy;
	int32_t		_accelerate;
	int			_tempval1;
	int			_tempval2;
	int			(MoveItem::*_moveScriptFunc)(void);

public:
				MoveItem(MenuItem *parent, uint32_t spx, uint32_t spy, uint32_t acc);
				~MoveItem();
	inline void SetFlags(uint8_t flgs) { _flags = flgs; };
	inline void SetMoveScript( int (MoveItem::*FuncAdr)(void) ) { _moveScriptFunc = FuncAdr; };
	inline void EnableMoveScript(void) { SetFlags(_flags |= MOVEITEM_FLAG_MOVESCRIPT); };
	inline void DisableMoveScript(void) { SetFlags(_flags &= ~MOVEITEM_FLAG_MOVESCRIPT); };
	inline int	IsEnableMoveScript(void) { return _flags & MOVEITEM_FLAG_MOVESCRIPT; };
	int			Move(void);
	//void		SetTempVars(void);
	int			SimpleMoveScript(void);
	int			SimpleScriptCalcMaxDistance(void);
	void		SetParams(uint32_t speddX, uint32_t speedY, uint32_t accelerate);

};
//=================================================================
class MenuItem
{
private:
	uint8_t		_flags;
protected:
	int32_t		_xpos;			//screen x position
	int32_t		_ypos;			//screen y position
public:
	MoveItem	*moveaction;
				MenuItem();
				~MenuItem();

	void		AddMoveAction(int (MoveItem::*moveFuncAdr)(void), int speedX, int speedY, int accelerate);
	inline void SetFlags(uint8_t flgs) { _flags = flgs; };
	inline void EnableVisible(void) { SetFlags(_flags |= MENUITEM_FLAG_VISIBILITY); };
	inline void DisableVisible(void) { SetFlags(_flags &= ~MENUITEM_FLAG_VISIBILITY); };
	inline int	IsVisible(void) { return _flags & MENUITEM_FLAG_VISIBILITY; };
	void 		RemoveMoveAction();
	inline void SetXYPos(int x,int y) { _xpos = x; _ypos = y; };
	inline void AddX(int deltaX) { _xpos += deltaX; };
	inline void AddY(int deltaY) { _ypos += deltaY; };
};
//=================================================================
class MenuItemPcx : public MenuItem
{
private:
	uint8_t		_holecolor;		//hole color
	uint8_t		_transpcolor;	//transparentcolor
	uint8_t		_grdtransp;		//transparent factor 0-3 25,50,75,100
	PCX			*_pcx;			//pcx to draw
public:
				MenuItemPcx(PCX *pcx);
				~MenuItemPcx();

	void		Draw(void);
	void		SetPcxParam(uint8_t color1, uint8_t color2, uint8_t color3);
};
//=================================================================


#endif
