#if !defined(_DOSKEY_W)
#define _DOSKEY_W 1

#define CAPSKEY   58
#define F10KEY    68
#define LEFTKEY   75
#define RIGHTKEY  77
#define UPKEY     72
#define DOWNKEY   80
#define TABKEY    15
#define ENTERKEY  28
#define SCROLLKEY 70
#define CTRLLKEY   0x1d
#define SHIFTLKEY 0x2a
#define SHIFTRKEY 0x36
#define ALTKEY    0x38
#define F12       88
#define F10       58
#define NUMB1KEY  2
#define NUMB2KEY  3
#define BACKSPACEKEY 14
#define ESCAPEKEY 1

        extern const char wkeys[2][101];
#define KEYPRESSEDTABLE(table,scancode) wkeys[table][scancode]
#endif

