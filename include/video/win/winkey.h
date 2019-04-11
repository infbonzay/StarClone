#ifndef _SC_WINKEY_H

#define _SC_WINKEY_H

#include <winuser.h>

#define CAPSKEY		VK_CAPSLOCK
#define LEFTKEY		VK_LEFT
#define RIGHTKEY	VK_RIGHT
#define UPKEY		VK_UP
#define DOWNKEY		VK_DOWN
#define TABKEY		VK_TAB
#define ENTERKEY	VK_RETURN
#define ENTERKEY2	254
#define SCROLLKEY	3
#define CTRLLKEY	VK_LCONTROL
#define CTRLRKEY	VK_RCONTROL
#define SHIFTLKEY	VK_LSHIFT
#define SHIFTRKEY	VK_RSHIFT
#define PAGEUPKEY	VK_PRIOR
#define PAGEDOWNKEY VK_NEXT

#define ALTKEY	  255
#define F1		  VK_F1
#define F2		  VK_F2
#define F3		  VK_F3
#define F4		  VK_F4
#define F5		  VK_F5
#define F6		  VK_F6
#define F7		  VK_F7
#define F8		  VK_F8
#define F10		  VK_F10
#define F11		  VK_F11
#define F12		  VK_F12
#define NUMB1KEY  '1'
#define NUMB2KEY  '2'
#define BACKSPACEKEY VK_BACK
#define ESCAPEKEY VK_ESCAPE

#define WMLEFTKEY  (1)
#define WMRIGHTKEY (3)

#define	SHIFTKEYMASK	0x01
#define LOCKKEYMASK		0x02
#define CTRLKEYMASK		0x04
#define ALTKEYMASK		0x08

//#define VK_SHIFT          0x10
//#define VK_CONTROL        0x11
//#define VK_MENU           0x12


/*
 * WM_KEYUP/DOWN/CHAR HIWORD(lParam) flags
 */
//#define KF_EXTENDED       0x0100
//#define KF_DLGMODE        0x0800
//#define KF_MENUMODE       0x1000
//#define KF_ALTDOWN        0x2000
//#define KF_REPEAT         0x4000
//#define KF_UP             0x8000

#endif



