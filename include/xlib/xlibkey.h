#ifndef _XLIBKEY_W
#define _XLIBKEY_W

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/extensions/xf86vmode.h>
#include <X11/XKBlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>


#define ENTERKEY  	XK_Return
#define ENTERKEY2 	XK_KP_Enter
#define ESCAPEKEY 	XK_Escape

#define CAPSKEY	  	XK_Caps_Lock
#define LEFTKEY	  	XK_Left
#define RIGHTKEY  	XK_Right
#define UPKEY	  	XK_Up
#define DOWNKEY	  	XK_Down
#define SCROLLKEY 	XK_Scroll_Lock
#define CTRLLKEY  	XK_Control_L
#define CTRLRKEY  	XK_Control_R
#define SHIFTLKEY 	XK_Shift_L
#define SHIFTRKEY 	XK_Shift_R
#define PAGEUPKEY 	XK_Page_Up
#define PAGEDOWNKEY XK_Page_Down
#define BACKSPACEKEY XK_BackSpace

#define ALTKEY	  	XK_Alt_L

#define TABKEY	  	XK_Tab
#define F11		  	XK_F11
#define F12		  	XK_F12
#define F10		  	XK_F10
#define F1		  	XK_F1
#define F2		  	XK_F2
#define F3		  	XK_F3
#define F4		 	XK_F4
#define F5		  	XK_F5
#define F6		  	XK_F6
#define F7		  	XK_F7
#define F8		  	XK_F8
#define NUMB1KEY  	'1'
#define NUMB2KEY  	'2'

#define WMLEFTKEY  	0x01
#define WMRIGHTKEY 	0x03

#define KEYPRESSEDTABLE(table,scancode) (toupper((scancode) & 0xff)+1)
#define KEYPRESSED 1

#endif

