#ifndef _XLIBKEY_W
#define _XLIBKEY_W

#include <X11/Xlib.h>
#include<X11/keysym.h>

#define CAPSKEY	  	29//XK_Caps_Lock
#define LEFTKEY	  	28//XK_Left
#define RIGHTKEY  	27//XK_Right
#define UPKEY	  	26//XK_Up
#define DOWNKEY	  	25//XK_Down
#define ENTERKEY  	13//XK_Return
#define ENTERKEY2 	14//XK_KP_Enter
#define SCROLLKEY 	23//XK_Scroll_Lock
#define CTRLLKEY  	22//XK_Control_L
#define CTRLRKEY  	21//XK_Control_R
#define SHIFTLKEY 	20//XK_Shift_L
#define SHIFTRKEY 	19//XK_Shift_R
#define PAGEUPKEY 	18//XK_Page_Up
#define PAGEDOWNKEY 17//XK_Page_Down
#define BACKSPACEKEY 15//XK_BackSpace
#define ESCAPEKEY 	16//XK_Escape

#define ALTKEY	  	30//XK_Alt_L

#define TABKEY	  	1//XK_Tab
#define F11		  	2//XK_F11
#define F12		  	3//XK_F12
#define F10		  	4//XK_F10
#define F1		  	5//XK_F1
#define F2		  	6//XK_F2
#define F3		  	7//XK_F3
#define F4		 	8//XK_F4
#define F5		  	9//XK_F5
#define F6		  	10//XK_F6
#define F7		  	11//XK_F7
#define F8		  	12//XK_F8
#define NUMB1KEY  	'1'
#define NUMB2KEY  	'2'

#define WMLEFTKEY  	0x01
#define WMRIGHTKEY 	0x03

#define KEYPRESSEDTABLE(table,scancode) (toupper((scancode) & 0xff)+1)
#define KEYPRESSED 1

#endif

