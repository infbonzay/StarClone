#ifndef _DIALOGBIN_W
#define _DIALOGBIN_W

#include "menu.h"

#pragma pack(push,1)

struct DIALOGBIN
{
	int			NextEntryOffset;
	short int	LeftEdge;
	short int	TopEdge;
	short int	RightEdge;
	short int	BottomEdge;
	short int	Width;
	short int	Height;
	int			Unknown1;
	int			MainStringOffset;
	int			Flags;
	int			Unknown2;
	short int	ControlId;
	int			ControlType;
	int			Unknown3;
	int			Unknown4;
	int			Unknown5;
	int			Unknown6;
	short int	RespondAreaLeft;
	short int	RespondAreaTop;
	short int	RespondAreaRight;
	short int	RespondAreaBottom;
	int			Unknown7;
	int			SMK_Offset;	   
	short int	TextOffsetX;
	short int	TextOffsetY;
	short int	RespondAreaWidth;
	short int	RespondAreaHeight;
	int			Unknown8;
	int			Unknown9;
};

struct DIALOGBIN_SMK
{
	int			SMK_Overlay_Offset;
	short int	SMK_Flags;
	int			Unknown1;
	int			SMK_StringOffset;
	int			Unknown2;

	short int	SMK_OverlayXPos;
	short int	SMK_OverlayYPos;
	int			Unknown3;
	int			Unknown4;
	
};
#pragma pack(pop)

struct DIALOGBIN_ITEMINFO
{
	short int	xpos;
	short int	ypos;
	short int	xtextpos;
	short int	ytextpos;
	short int	xsize;
	short int	ysize;
	int			Flags;
	unsigned char hotkey;
	signed char fontnr;
	char		*text;
};

struct DIALOGBIN_INFO
{
	int nrofitems;
	struct DIALOGBIN_ITEMINFO iteminfo[];
};


#define DIALOGBIN_MAIN			0
#define DIALOGBIN_DEFAULTBUTTON 1
#define DIALOGBIN_BUTTON		2
#define DIALOGBIN_OPTIONBUTTON	3				//this is radio button
#define DIALOGBIN_CHECKBOX		4
#define DIALOGBIN_IMAGE			5
#define DIALOGBIN_HORZSCROLLBAR 6
#define DIALOGBIN_VERTSCROLLBAR 7
#define DIALOGBIN_TEXTBOX		8
#define DIALOGBIN_LABELLEFT		9
#define DIALOGBIN_LABELCENTER	10
#define DIALOGBIN_LABELRIGHT	11
#define DIALOGBIN_LISTBOX		12
#define DIALOGBIN_COMBOBOX		13
#define DIALOGBIN_SMKBUTTON		14

#define DIALOGBIN_FLAGS_NONEEDRECALCINFO		0x00000001				//temprary flag
#define DIALOGBIN_FLAGS_ITEMDISABLED			0x00000002
#define DIALOGBIN_FLAGS_NOTUSED1				0x00000004
#define DIALOGBIN_FLAGS_ITEMVISIBLED			0x00000008
#define DIALOGBIN_FLAGS_KEYMOUSERESPONDEVENTS	0x00000010
#define DIALOGBIN_FLAGS_NOTUSED2				0x00000020
#define DIALOGBIN_FLAGS_RESPONDTOESCKEY			0x00000040
#define DIALOGBIN_FLAGS_NOHOVERSOUND			0x00000080
#define DIALOGBIN_FLAGS_VIRTHOTKEY				0x00000100
#define DIALOGBIN_FLAGS_HOTKEYFIRSTCHARACTER	0x00000200				//0x200 or 0x100
#define DIALOGBIN_FLAGS_FONT10					0x00000400
#define DIALOGBIN_FLAGS_FONT16					0x00000800
#define DIALOGBIN_FLAGS_FONT50					0x00001000				//my add
#define DIALOGBIN_FLAGS_TRANSPARENTEFFECT		0x00002000
#define DIALOGBIN_FLAGS_FONT16X					0x00004000
#define DIALOGBIN_FLAGS_NOTUSED3				0x00008000
#define DIALOGBIN_FLAGS_FONT14					0x00010000
#define DIALOGBIN_FLAGS_NODLGGRP				0x00020000
#define DIALOGBIN_FLAGS_TRANSLUCENCYEFFECT		0x00040000
#define DIALOGBIN_FLAGS_DEFAULTBUTTON			0x00080000

#define DIALOGBIN_FLAGS_SMKBUTTON_HORIZLEFT		0x00100000
#define DIALOGBIN_FLAGS_SMKBUTTON_HORIZCENTER	0x00200000
#define DIALOGBIN_FLAGS_SMKBUTTON_HORIZRIGHT	0x00400000

#define DIALOGBIN_FLAGS_SMKBUTTON_VERTTOP		0x01000000
#define DIALOGBIN_FLAGS_SMKBUTTON_VERTMIDDLE	0x02000000
#define DIALOGBIN_FLAGS_SMKBUTTON_VERTBOTTOM	0x04000000
#define DIALOGBIN_FLAGS_NOCLICKSOUND			0x40000000
#define DIALOGBIN_FLAGS_ENTERSEMAPHORE			0x80000000				//my add

#define DIALOGBIN_FLAGS_ALLSMKBUTTONALIGN		(DIALOGBIN_FLAGS_SMKBUTTON_HORIZLEFT|DIALOGBIN_FLAGS_SMKBUTTON_HORIZCENTER|DIALOGBIN_FLAGS_SMKBUTTON_HORIZRIGHT|DIALOGBIN_FLAGS_SMKBUTTON_VERTTOP|DIALOGBIN_FLAGS_SMKBUTTON_VERTMIDDLE|DIALOGBIN_FLAGS_SMKBUTTON_VERTBOTTOM)

#define DIALOGBIN_SMKFLAGS_FADEMOUSEOVER				0x01
#define DIALOGBIN_SMKFLAGS_DARKGREYOUT					0x02
#define DIALOGBIN_SMKFLAGS_LOOPANIMATION				0x04
#define DIALOGBIN_SMKFLAGS_SHOWONMOUSEOVER				0x08
#define DIALOGBIN_SMKFLAGS_ADDED_UNPACKALLFRAMES		0x10
#define DIALOGBIN_SMKFLAGS_ADDED_UNPACKREADY			0x20
#define DIALOGBIN_SMKFLAGS_NODRAW						0x40

#define DIALOGBIN_MAINDIALOGS					0x01
#define DIALOGBIN_GAMEDIALOGS					0x02
#define DIALOGBIN_DONOTSAVEUNDERMENU			0x04
//#define DIALOGBIN_TEST								0x04

int	 DialogBin_GetFontNr(int flags);
int	 GetDialogElements(char *dialogdata,int *groupofbuttons);
void ShowMenuEdge(MENUSTR *menu);
void UnloadDialogBin(MENUSTR *menu);
MENUSTR *LoadDialogBin(const char *dialogbinfilename,int dialog_opt,int defaultfontnr);
MENUSTR *LoadDialogBin(const char *dialogbinfilename,int dialog_opt,int addforreserv,int defaultfontnr);
DIALOGBIN_INFO *DialogBin_GetItemCoords(const char *dialogbinfilename);
void DialogBin_FreeItemCoords(DIALOGBIN_INFO *allcoords);
void DialogBin_Save(MENUSTR *menu,char *label,const char *filename);
void DialogBin_Save(MENUSTR *menu,char *label,const char *filename,int *sortedarray);
MENUSTR *DialogBin_ShowPermanent(const char *dialogbinfilename,int dialog_opt,int addforreserv,
								void (*refreshfunc)(void *,int));
void DialogBin_ClosePermanent(MENUSTR *menu);


#endif

