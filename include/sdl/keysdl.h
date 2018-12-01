#if !defined(_SDLKEY_W)
#define _SDLKEY_W 1

#define CAPSKEY	  SDLK_CAPSLOCK
#define F10KEY	  SDLK_F10
#define LEFTKEY	  SDLK_LEFT
#define RIGHTKEY  SDLK_RIGHT
#define UPKEY	  SDLK_UP
#define DOWNKEY	  SDLK_DOWN
#define TABKEY	  SDLK_TAB
#define ENTERKEY  SDLK_RETURN
#define ENTERKEY2 SDLK_KP_ENTER
#define SCROLLKEY SDLK_SCROLLOCK
#define CTRLLKEY  SDLK_LCTRL
#define CTRLRKEY  SDLK_RCTRL
#define SHIFTLKEY SDLK_LSHIFT
#define SHIFTRKEY SDLK_RSHIFT
#define PAGEUPKEY SDLK_PAGEUP
#define PAGEDOWNKEY SDLK_PAGEDOWN

#define ALTKEY	  SDLK_LALT
#define F11		  SDLK_F11
#define F12		  SDLK_F12
#define F10		  SDLK_F10
#define F1		  SDLK_F1
#define F2		  SDLK_F2
#define F3		  SDLK_F3
#define F4		  SDLK_F4
#define F5		  SDLK_F5
#define F6		  SDLK_F6
#define F7		  SDLK_F7
#define F8		  SDLK_F8
#define NUMB1KEY  '1'
#define NUMB2KEY  '2'
#define BACKSPACEKEY SDLK_BACKSPACE
#define ESCAPEKEY SDLK_ESCAPE

#define KEYPRESSEDTABLE(table,scancode) (toupper((scancode)&0xff)+1)
#define KEYPRESSED SDL_PRESSED

#endif


