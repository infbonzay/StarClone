#ifndef _SC_FBKEY_H

#define _SC_FBKEY_H

#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#define ENTERKEY  	0
#define ENTERKEY2 	1
#define ESCAPEKEY 	2

#define CAPSKEY	  	3
#define LEFTKEY	  	4
#define RIGHTKEY  	5
#define UPKEY	  	6
#define DOWNKEY	  	7
#define SCROLLKEY 	8
#define CTRLLKEY  	9
#define CTRLRKEY  	10
#define SHIFTLKEY 	11
#define SHIFTRKEY 	12
#define PAGEUPKEY 	13
#define PAGEDOWNKEY 14
#define BACKSPACEKEY 15

#define ALTKEY	  	16

#define TABKEY	  	17
#define F11		  	18
#define F12		  	19
#define F10		  	20
#define F1		  	21
#define F2		  	22
#define F3		  	23
#define F4		 	24
#define F5		  	25
#define F6		  	26
#define F7		  	27
#define F8		  	28
#define NUMB1KEY  	'1'
#define NUMB2KEY  	'2'


#define WMLEFTKEY  	0x1
#define WMRIGHTKEY 	0x4

#define	SHIFTKEYMASK	0x01
#define LOCKKEYMASK		0x02
#define CTRLKEYMASK		0x04
#define ALTKEYMASK		0x05

#define KEYPRESSEDTABLE(table,scancode) (toupper((scancode)&0xff)+1)

#endif

