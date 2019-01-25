#ifndef _SC_ISCRIPT_H

#define _SC_ISCRIPT_H

#include <stdio.h>
#include "mylist.h"
#include "images.h"

#define ISCRIPTID_BLINK					254
#define ISCRIPTID_MAXVALUE				412

#define ISCRIPTCMD_UNKNOWN				0x45

#define ISCRIPTNR_UNKNOWN				255
#define ISCRIPTNR_INIT					0
#define ISCRIPTNR_DEATH					1
#define ISCRIPTNR_GNDATTKINIT			2
#define ISCRIPTNR_AIRATTKINIT			3
#define ISCRIPTNR_UNUSED1				4
#define ISCRIPTNR_GNDATTKRPT			5
#define ISCRIPTNR_AIRATTKRPT			6
#define ISCRIPTNR_CASTSPELL				7
#define ISCRIPTNR_GNDATTKTOIDLE			8
#define ISCRIPTNR_AIRATTKTOIDLE			9
#define ISCRIPTNR_UNUSED2				10
#define ISCRIPTNR_WALKING				11
#define ISCRIPTNR_WALKINGTOIDLE			12
#define ISCRIPTNR_SPECIALSTATE1			13
#define ISCRIPTNR_SPECIALSTATE2			14
#define ISCRIPTNR_ALMOSTBUILT			15
#define ISCRIPTNR_BUILT					16
#define ISCRIPTNR_LANDING				17
#define ISCRIPTNR_LIFTOFF				18
#define ISCRIPTNR_ISWORKING				19
#define ISCRIPTNR_WORKINGTOIDLE			20
#define ISCRIPTNR_WARPIN				21
#define ISCRIPTNR_UNUSED3				22
#define ISCRIPTNR_INITTURRET			23
#define ISCRIPTNR_DISABLE				24
#define ISCRIPTNR_BURROW				25
#define ISCRIPTNR_UNBURROW				26
#define ISCRIPTNR_ENABLE				27
#define ISCRIPTNR_MAXVALUE				28

#define ISCRIPTNR_ISID					28
#define ISCRIPTNR_TYPE					29

#define ISCRIPTNR_DOODADINITTOP			ISCRIPTNR_ENABLE
#define ISCRIPTNR_DOODADINITBOTTOM		ISCRIPTNR_DISABLE
#define ISCRIPTNR_DOODADGOTOP			ISCRIPTNR_INITTURRET
#define ISCRIPTNR_DOODADGOBOTTOM		ISCRIPTNR_DISABLE

#define ISCRIPTNR_SETONLY				0
#define ISCRIPTNR_EXECUTE				1

#define ISCRIPTCMD_PLAYFRAM				0x00
#define ISCRIPTCMD_PLAYFRAMTILE			0x01

#define ISCRIPTCMD_END					0x16

#define ISCRIPTCMD_UNKNOWN				0x45
#define ISCRIPTCMD_MAXVALUE				0x46
//============================================

struct ISCRIPTID
{
	unsigned short		cmdbufoffs[ISCRIPTNR_MAXVALUE];
};
//============================================
class ISCRIPT
{
	mylist				labels;							//labelname
	mylist				labeloffsets;					//adress of labelname in compilediscripts 

	mylist				needtoaddoffs;					//adress where need to put adress of label
	mylist				nameoflabels;					//namelabel needed to put
	unsigned char		*compilediscripts;
	unsigned short		compilediscriptssize;
	int					CompilePass1(FILE *f);
	int					CompilePass2(FILE *f);
public:

	ISCRIPTID			iscriptsid[ISCRIPTID_MAXVALUE];

						ISCRIPT(void);
						~ISCRIPT();
	int					CompileIScripts(const char *iscriptfile);
	void				AddCompiledCode(int iscriptid,int iscriptnr,char *cmdbuf,int bufsize);
	void				SaveCmdToBuff(char *buf,int offs,int sizedata,int data);
	int					ExecuteScript(MAIN_IMG *img);
	int					ExecuteScript(OVERLAY_IMG *img);

};

void AllImages_ExecuteIScript(void);

//============================================
extern ISCRIPT iscriptinfo;


#endif //_ISCRIPT_W

