
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defsunits.h"
#include "debug.h"
#include "auxil.h"
#include "audio.h"
#include "loadlo.h"
#include "wmem.h"
#include "rand.h"
#include "mylist.h"
#include "ObjChecks.h"
#include "load.h"
#include "maps.h"
#include "objs.h"
#include "overlay.h"
#include "weapons.h"
#include "mage.h"
#include "flingy.h"
#include "sigorder.h"
#include "market.h"
#include "iscript.h"


ISCRIPT iscriptinfo;
//============================================
const char		*iscriptnrstr[ISCRIPTNR_MAXVALUE + 2] =
{
		"Init",
		"Death",
		"GndAttkInit",
		"AirAttkInit",
		"Unused1",
		"GndAttkRpt",
		"AirAttkRpt",
		"CastSpell",
		"GndAttkToIdle",
		"AirAttkToIdle",
		"Unused2",
		"Walking",
		"WalkingToIdle",
		"SpecialState1",
		"SpecialState2",
		"AlmostBuilt",
		"Built",
		"Landing",
		"LiftOff",
		"IsWorking",
		"WorkingToIdle",
		"WarpIn",
		"Unused3",
		"InitTurret",
		"Disable",
		"Burrow",
		"UnBurrow",
		"Enable",
		"IsId",
		"Type"
};
//============================================
const char		*iscriptcmdstr[ISCRIPTCMD_MAXVALUE] =
{
	"playfram",				//0x00
	"playframtile",			//0x01
	"sethorpos",			//0x02
	"setvertpos",			//0x03
	"setpos",				//0x04
	"wait",					//0x05
	"waitrand",				//0x06
	"goto",					//0x07
	"imgol",				//0x08
	"imgul",				//0x09
	"imgolorig",			//0x0a
	"switchul",				//0x0b
	"__0c",					//0x0c
	"imgoluselo",			//0x0d
	"imguluselo",			//0x0e
	"sprol",				//0x0f
	"highsprol",			//0x10
	"lowsprul",				//0x11
	"uflunstable",			//0x12
	"spruluselo",			//0x13
	"sprul",				//0x14
	"sproluselo",			//0x15
	"end",					//0x16
	"setflipstate",			//0x17
	"playsnd",				//0x18
	"playsndrand",			//0x19
	"playsndbtwn",			//0x1a
	"domissiledmg",			//0x1b
	"attackmelee",			//0x1c
	"followmaingraphic",	//0x1d
	"randcondjmp",			//0x1e
	"turnccwise",			//0x1f
	"turncwise",			//0x20
	"turn1cwise",			//0x21
	"turnrand",				//0x22
	"setspawnframe",		//0x23
	"sigorder",				//0x24
	"attackwith",			//0x25
	"attack",				//0x26
	"castspell",			//0x27
	"useweapon",			//0x28
	"move",					//0x29
	"gotorepeatattk",		//0x2a
	"engframe",				//0x2b
	"engset",				//0x2c
	"__2d",					//0x2d
	"nobrkcodestart",		//0x2e
	"nobrkcodeend",			//0x2f
	"ignorerest",			//0x30
	"attkshiftproj",		//0x31
	"tmprmgraphicstart",	//0x32
	"tmprmgraphicend",		//0x33
	"setfldirect",			//0x34
	"call",					//0x35
	"return",				//0x36
	"setflspeed",			//0x37
	"creategasoverlays",	//0x38
	"pwrupcondjmp",			//0x39
	"trgtrangecondjmp",		//0x3a
	"trgtarccondjmp",		//0x3b
	"curdirectcondjmp",		//0x3c
	"imgulnextid",			//0x3d
	"__3e",					//0x3e
	"liftoffcondjmp",		//0x3f
	"warpoverlay",			//0x40
	"orderdone",			//0x41
	"grdsprol",				//0x42
	"__43",					//0x43
	"dogrddamage",			//0x44
	"unknowncmd"			//0x45
};
//============================================
#define MAXCMDSIZE		5
unsigned char	iscriptcmdsizeparam[ISCRIPTCMD_MAXVALUE][MAXCMDSIZE] =
{
		{ 2, 2},										//playfram		0x0000
		{ 2, 2},										//playframtile	0x0000
		{ 1, 1},										//sethorpos		0x00
		{ 1, 1},										//setvertpos	0x00
		{ 2, 1, 1},										//setpos		0x00 0x00
		{ 1, 1},										//wait			0x00
		{ 2, 1, 1},										//waitrand		0x00 0x00
		{ 2, 2 + 0x80},									//goto			0x0000
		{ 4, 2, 1, 1},									//imgol			0x0000 0x00 0x00
		{ 4, 2, 1, 1},									//imgul			0x0000 0x00 0x00
		{ 2, 2},										//imgolorig		0x0000
		{ 2, 2},										//switchul		0x0000
		{ 0},											//__0c
		{ 4, 2, 1, 1},									//imgoluselo	0x0000 0x00 0x00
		{ 4, 2, 1, 1},									//imguluselo	0x0000 0x00 0x00
		{ 4, 2, 1, 1},									//sprol			0x0000 0x00 0x00
		{ 4, 2, 1, 1},									//highsprol		0x0000 0x00 0x00
		{ 4, 2, 1, 1},									//lowsprul		0x0000 0x00 0x00
		{ 2, 2},										//uflunstable	0x0000
		{ 4, 2, 1, 1},									//spruluselo	0x0000 0x00 0x00
		{ 4, 2, 1, 1},									//sprul			0x0000 0x00 0x00
		{ 3, 2, 1},										//sproluselo	0x0000 0x00
		{ 0},											//end
		{ 1, 1},										//setflipstate	0x00
		{ 2, 2},										//playsnd		0x0000
		{ 0x80, 2},										//playsndrand	0x00 0x0000 0x0000 ....
		{ 4, 2, 2},										//playsndbtwn	0x0000 0x0000
		{ 0},											//domissiledmg
		{ 0x80, 2},										//attackmelee	0x00 0x0000 0x0000 ....
		{ 0},											//followmaingraphic
		{ 3, 1, 2 + 0x80},								//randcondjmp	0x00 0x0000
		{ 1, 1},										//turnccwise	0x00
		{ 1, 1},										//turncwise		0x00
		{ 0}   ,										//turn1cwise
		{ 1, 1},										//turnrand		0x00
		{ 1, 1},										//setspawnframe 0x00
		{ 1, 1},										//sigorder		0x00
		{ 1, 1},										//attackwith	0x00
		{ 0},											//attack
		{ 0},											//castspell
		{ 1, 1},										//useweapon		0x00
		{ 1, 1},										//move			0x00
		{ 0},											//gotorepeatattk
		{ 1, 1},										//engframe		0x00
		{ 1, 1},										//engset		0x00
		{ 0},											//__2d
		{ 0},											//nobrkcodestart
		{ 0},											//nobrkcodeend
		{ 0},											//ignorerest
		{ 1, 1},										//attkshiftproj
		{ 0},											//tmprmgraphicstart"
		{ 0},											//tmprmgraphicend
		{ 1, 1},										//setfldirect
		{ 2, 2 + 0x80},									//call 0x0000
		{ 0},											//return
		{ 2, 2},										//setflspeed 0x0000
		{ 1, 1},										//creategasoverlays
		{ 2, 2 + 0x80},									//pwrupcondjmp 0x0000
		{ 3, 1, 2 + 0x80},								//trgtrangecondjmp 0x00 0x0000
		{ 4, 1, 1, 2 + 0x80},								//trgtarccondjmp 0x00 0x00 0x0000
		{ 4, 1, 1, 2 + 0x80},								//curdirectcondjmp 0x00 0x00 0x0000
		{ 2, 1, 1},										//imgulnextid 0x00 0x00
		{ 0},											//__3e
		{ 2, 2 + 0x80},									//liftoffcondjmp 0x0000
		{ 1, 1},										//warpoverlay
		{ 1, 1},										//orderdone		0x00
		{ 4, 2, 1, 1},									//grdsprol
		{ 0},											//__43
		{ 0},											//dogrddamage"
		{ 0}											//0x45 unknown cmd, special for see any errors
};
//============================================
int comparestrings(char *str, const char *strarray[], int nrofstrarray)
{
	int i;
	for (i = 0;i < nrofstrarray;i++)
	{
		if (!strcmp(str, strarray[i]))
			return(i);
	}
	return(-1);
}
//============================================
#define MAXCMDPARAM				5
#define MAXCOMPILEBUFSIZE		65535
int ISCRIPT::CompilePass1(FILE *f)
{
	int i, k = 0, stridlen, retstatus = 0, intval, isheader, nrparams;
	long compilebufoffs;
	unsigned char cmdsize, subcmdsize;
	signed char cmd;
	char *compilebuf;
	char strid[200];
	if (!f)
	{
		DEBUGMESSCR("scriptfile not opened\n");
		return(-1);
	}
	compilebuf = (char *)wmalloc(MAXCOMPILEBUFSIZE);
	memset(compilebuf, 0, MAXCOMPILEBUFSIZE);
	isheader = 0;
	compilebufoffs = 0;
	//lets put compiled code to offset from 0x0001, offset 0x0000 is reserved for BUGS
	//	  compilebuf[compilebufoffs++] = ISCRIPTCMD_UNKNOWN;						//unknown command
	while (1)
	{
		strid[0] = 0;
		fscanf(f, "%s \n", strid);
		stridlen = strlen(strid);
		//		printf("%s\n",strid);
		k++;
		if (!strid[0])
		{
			break;
		}
		if (strid[0] == '#')
		{
			fgets(strid, 199, f);
			continue;
		}
		//skip all directives and header structs for pass2
		if (!strcmp(strid, ".headerstart"))
		{
			isheader = 1;
			continue;
		}
		else if (!strcmp(strid, ".headerend"))
		{
			isheader = 0;
			continue;
		}
		else
		{
			if (isheader)
				continue;
			if (strid[stridlen - 1] == ':') //label founded
			{
				//save label address
				strid[stridlen - 1] = 0;
				labels.AllocAndAddList(strid, stridlen);
				labeloffsets.AddList((void *)compilebufoffs);
				//				printf("%s - [0x%04x]\n",strid,compilebufoffs);
			}
			else
			{
				cmd = comparestrings(strid, iscriptcmdstr, ISCRIPTCMD_MAXVALUE);
				if (cmd == -1)
				{
					printf("[%s] iscript command invalid\n", strid);
					retstatus = -1;
					break;
				}
				//save script cmd
				SaveCmdToBuff(compilebuf, compilebufoffs, 1, cmd);
				if (++compilebufoffs >= MAXCOMPILEBUFSIZE)
				{
					retstatus = -1;
					break;
				}
				cmdsize = iscriptcmdsizeparam[cmd][0];
				if (cmdsize == 0x80)
				{
					//variable size of parameters, depend on param1,  with sizes iscriptcmdsizeparam[1]
					fscanf(f, "%s \n", strid);
					nrparams = atoi(strid);
					SaveCmdToBuff(compilebuf, compilebufoffs, 1, nrparams);				//save nr of params
					if (++compilebufoffs >= MAXCOMPILEBUFSIZE)
					{
						DEBUGMESSCR("compiled script large than %s bytes\n", MAXCOMPILEBUFSIZE);
						retstatus = -1;
						break;
					}
				}
				else
				{
					nrparams = MAXCMDSIZE;
				}
				for (i = 0;i < nrparams;i++)
				{
					if (cmdsize == 0x80)
						subcmdsize = iscriptcmdsizeparam[cmd][1];
					else
						subcmdsize = iscriptcmdsizeparam[cmd][i + 1];
					if (!subcmdsize)
						break;
					fscanf(f, "%s \n", strid);
					stridlen = strlen(strid);
					if (strid[0] == '0' && strid[1] == 'x')
					{
						intval = strtol(strid, NULL, 16);
					}
					else
					{
						intval = atoi(strid);
					}
					//save script cmd param[i]
					SaveCmdToBuff(compilebuf, compilebufoffs, subcmdsize & 0x7f, intval);
					if (subcmdsize & 0x80)
					{
						//this label need to save to pass 2
						needtoaddoffs.AddList((void *)compilebufoffs);
						nameoflabels.AllocAndAddList(strid, stridlen + 1);
						//printf("ref [0x%04x]\n",compilebufoffs);
					}
					compilebufoffs += (subcmdsize & 0x7f);
					if (compilebufoffs >= MAXCOMPILEBUFSIZE)
					{
						retstatus = -1;
						wfree(compilebuf);
						return(retstatus);
					}
				}
			}
		}
	}
	if (!retstatus)
	{
		//move compiled scripts to newly exacted size allocated memory
		compilediscriptssize = (unsigned short)compilebufoffs;
		compilediscripts = (unsigned char *)wmalloc(compilediscriptssize);
		memcpy(compilediscripts, compilebuf, compilediscriptssize);
	}
	wfree(compilebuf);
	return(retstatus);
}
//============================================
//all func ofsset filled with addresses
//all saved label also filled with their addresses
//============================================
int ISCRIPT::CompilePass2(FILE *f)
{
	int i, iscriptid, retstatus = 0, labelelemnr, findlabel, stridlen, maxlabels, isheader, idtype;
	signed char cmd;
	long labeloffs, cmdlabeloffs;
	char *labelname, *cmdlabel;
	char strid[200];
	fseek(f, 0, SEEK_SET);
	isheader = 0;
	iscriptid = -1;
	while (1)
	{
		strid[0] = 0;
		fscanf(f, "%s \n", strid);
		stridlen = strlen(strid);
		if (!strid[0])
		{
			break;
		}
		if (strid[0] == '#')
		{
			fgets(strid, 199, f);
			continue;
		}
		if (!strcmp(strid, ".headerstart"))
		{
			isheader = 1;
			continue;
		}
		else if (!strcmp(strid, ".headerend"))
		{
			isheader = 0;
			iscriptid = -1;
			continue;
		}
		else
		{
			if (!isheader)
				continue;
			cmd = comparestrings(strid, iscriptnrstr, ISCRIPTNR_MAXVALUE + 2);
			if (cmd == -1)
			{
				printf("ERROR:[%s] iscriptnr invalid\n", strid);
				retstatus = -1;
				return(retstatus);
			}
			else if (cmd == ISCRIPTNR_ISID)
			{
				fscanf(f, "%s \n", strid);
				iscriptid = atoi(strid);
			}
			else if (cmd == ISCRIPTNR_TYPE)
			{
				fscanf(f, "%s \n", strid);
				idtype = atoi(strid);
			}
			else
			{
				fscanf(f, "%s \n", strid);		//read label
				if (!strcmp(strid, "[NONE]"))
				{
					//none address - skip
					continue;
				}
				labels.EnumListInit();
				findlabel = 0;
				while ((labelname = (char *)labels.GetNextListElem(&labelelemnr)))
				{
					if (!strcmp(strid, labelname))
					{
						//ok we found label, get the label offset
						labeloffs = (long)labeloffsets.GetElemNr(labelelemnr);
						iscriptinfo.iscriptsid[iscriptid].cmdbufoffs[cmd] = (unsigned short) labeloffs;
						findlabel = 1;
					}
				}
				if (!findlabel)
				{
					printf("ERROR:[%s] label not found\n", strid);
					retstatus = -2;
					return(retstatus);
				}
			}
		}
	}
	//set labels on commands
	maxlabels = nameoflabels.Count();
	for (i = 0;i < maxlabels;i++)
	{
		cmdlabel = (char *)nameoflabels.GetElemNr(i);
		cmdlabeloffs = (long)needtoaddoffs.GetElemNr(i);
		labels.EnumListInit();
		findlabel = 0;
		while ((labelname = (char *)labels.GetNextListElem(&labelelemnr)))
		{
			if (!strcmp(cmdlabel, labelname))
			{
				//ok we found label, get the label offset
				labeloffs = (long)labeloffsets.GetElemNr(labelelemnr);
				*((unsigned short *)(&compilediscripts[cmdlabeloffs])) = (unsigned short) labeloffs;
				findlabel = 1;
			}
		}
		if (!findlabel)
		{
			printf("ERROR:[%s] label not found\n", strid);
			retstatus = -2;
			return(retstatus);
		}
	}
	return(retstatus);
}
//============================================
ISCRIPT::ISCRIPT(void)
{
	memset(iscriptsid, 0, sizeof(iscriptsid));
}
//============================================
int ISCRIPT::CompileIScripts(const char *filename)
{
	int retcode = 0;
	FILE *f = fopen(filename, "rb");
	if (!f)
	{
		printf("file %s not found\n", filename);
		return(-3);
	}
	printf("compile iscript pass 1 ... ");
	if (CompilePass1(f))
	{
		printf("cannot complete pass 1\n");
		retcode = -1;
	}
	else
	{
		//			printf("totallabels=%d, labelreferiences=%d\n",labels.Count(),nameoflabels.Count());
		printf("DONE\ncompile iscript pass 2 ... ");
		if (CompilePass2(f))
		{
			printf("cannot complete pass 2\n");
			retcode = -1;
		}
		else
		{
			printf("DONE\n");
		}
	}
	fclose(f);
	DEBUGMESSCR("compiled iscript size = %d\n", compilediscriptssize);
	//		  savebuff("iscript.bin",compilediscripts,compilediscriptssize);
			//remove all label & offset lists
	/*		for (i=0;i<labels.Count();i++)
			{
				printf("[%03d][%s] = %04x\n",i,(char*)labels.GetElemNr(i),(int) labeloffsets.GetElemNr(i));
			}
			for (i=0;i<ISCRIPTID_MAXVALUE;i++)
			{
				for (j=0;j<ISCRIPTNR_MAXVALUE;j++)
				{
					printf("id=%d nr=%d offset=%04x\n",i,j,iscriptinfo.iscriptsid[i].cmdbufoffs[j]);
				}
			}
	*/
	needtoaddoffs.FlushList();
	nameoflabels.DeallocList();
	labeloffsets.FlushList();
	labels.DeallocList();
	return(retcode);
}
//============================================
//============================================
ISCRIPT::~ISCRIPT()
{
	if (compilediscripts)
	{
		wfree(compilediscripts);
		compilediscripts = NULL;
	}
}
//============================================
void ISCRIPT::SaveCmdToBuff(char *buf, int offs, int sizedata, int data)
{
	switch (sizedata)
	{
	case 1:
		buf[offs] = data;
		break;
	case 2:
		*((short *)&buf[offs]) = data;
		break;
	case 4:
		*((int *)&buf[offs]) = data;
		break;
	}
}
//============================================
#include "iscriptcmds.cpp"
//============================================
void ShowCommand(MAIN_IMG *img, OVERLAY_IMG *img2, uint8_t iscriptcmd, uint8_t *cmdbuf);

int ISCRIPT::ExecuteScript(MAIN_IMG *img)
{
	unsigned char iscriptcmd, iscriptcmdsize;
	int status;
	if (img->flags & (SC_IMAGE_FLAG_DISABLEEXECSCRIPT | SC_IMAGE_FLAG_MARKFORDELETE))
		return(0);
	if (img->flags & SC_IMAGE_FLAG_NEEDROTATIONTODIRECTION)
	{
		if (img->flags & SC_IMAGE_FLAG_ISCRIPTROTATION)
		{
			if (!img->Rotation(img->TurnRadius))
				return(0);
		}
	}
	if (img->waitticks <= 1)
	{
		img->waitticks = 0;
		do{
			iscriptcmd = compilediscripts[img->offsetcmdinbuf++];
			//ShowCommand(img,NULL,iscriptcmd,compilediscripts+img->offsetcmdinbuf);
			iscriptcmdsize = iscriptcmdsizeparam[iscriptcmd][0];
			status = (*IScriptCmd[iscriptcmd])(img, compilediscripts + img->offsetcmdinbuf, iscriptcmdsize);
		}while (!status);
	}
	else
	{
		img->waitticks--;
	}
	return(0);
}
//============================================
int ISCRIPT::ExecuteScript(OVERLAY_IMG *img)
{
	unsigned char iscriptcmd, iscriptcmdsize;
	int status;
	if (img->flags & (SC_IMAGE_FLAG_DISABLEEXECSCRIPT | SC_IMAGE_FLAG_MARKFORDELETE))
		return(0);
	if (img->waitticks <= 1)
	{
		img->waitticks = 0;
		do{
			iscriptcmd = compilediscripts[img->offsetcmdinbuf++];
			//ShowCommand(NULL,img,iscriptcmd,compilediscripts+img->offsetcmdinbuf);
			iscriptcmdsize = iscriptcmdsizeparam[iscriptcmd][0];
			status = (*IScriptCmd[iscriptcmd])(img, compilediscripts + img->offsetcmdinbuf, iscriptcmdsize);
		}while (!status);
	}
	else
	{
		img->waitticks--;
	}
	return(0);
}
//============================================
void AllImages_ExecuteIScript(void)
{
	bool deleteflag;
	MAIN_IMG *img;
	OVERLAY_IMG *oimg;
	mainimageslist.EnumListInit();
	while ((img = (MAIN_IMG *)mainimageslist.GetABSNextListElem(&deleteflag)))
	{
		if (!deleteflag)
			iscriptinfo.ExecuteScript(img);
		if (img->childlists)
		{
			img->childlists->EnumListInit();
			while ((oimg = (OVERLAY_IMG *)img->childlists->GetNextListElem(NULL)))
			{
				iscriptinfo.ExecuteScript(oimg);
			}
		}
	}
	imageslist_doodads.EnumListInit();
	while ((img = (MAIN_IMG *)imageslist_doodads.GetABSNextListElem(&deleteflag)))
	{
		if (!deleteflag)
			iscriptinfo.ExecuteScript(img);
		if (img->childlists)
		{
			img->childlists->EnumListInit();
			while ((oimg = (OVERLAY_IMG *)img->childlists->GetNextListElem(NULL)))
			{
				iscriptinfo.ExecuteScript(oimg);
			}
		}
	}
	imageslist_minimap.EnumListInit();
	while ((img = (MAIN_IMG *)imageslist_minimap.GetNextListElem(NULL)))
	{
		iscriptinfo.ExecuteScript(img);
	}
}
//============================================
void ShowCommand(MAIN_IMG *img, OVERLAY_IMG *img2, uint8_t iscriptcmd, uint8_t *cmdbuf)
{
	const char *typeimg;
	int param[MAXCMDSIZE];
	int paramnr = 0, parametersize;
	int iscriptscmdsize = iscriptcmdsizeparam[iscriptcmd][paramnr++];
	if (iscriptscmdsize & 0x80)
	{
		iscriptscmdsize = 0;	//TODO variable parameters
	}
	while (iscriptscmdsize)
	{
		parametersize = iscriptcmdsizeparam[iscriptcmd][paramnr++];
		if (parametersize & 0x80)		//this is address
			parametersize &= ~0x80;
		switch (parametersize)
		{
		case 1:
			param[paramnr - 2] = *cmdbuf;
			break;
		case 2:
			param[paramnr - 2] = *((short int *)cmdbuf);
			break;
		default:
			break;
		}
		cmdbuf += parametersize;
		iscriptscmdsize -= parametersize;
	}
	int whocreate;
	if (img == NULL)
	{
		typeimg = "Overlay";
		whocreate = img2->parentimg->whocreate;
		img = (MAIN_IMG *)img2;
	}
	else
	{
		typeimg = "Main";
		whocreate = img->whocreate;
	}
	if (whocreate == SC_IMAGE_OBJ_CREATOR)
	{
		//OBJADR SCUNIT IMAGEID cmd [parameter0 ... parametern]
		FILE *f = fopen("/home/vasya/devel/iscripts.cmd", "a");
		if (!f)
			return;
		switch (paramnr - 1)
		{
		case 0:
			fprintf(f, "%s %p %d %d %s\n", typeimg,
				img->parentimg->creator.objcreator.obj,
				img->parentimg->creator.objcreator.obj->SC_Unit,
				img->imageid,
				iscriptcmdstr[iscriptcmd]);
			break;
		case 1:
			fprintf(f, "%s %p %d %d %s %d\n", typeimg,
				img->parentimg->creator.objcreator.obj,
				img->parentimg->creator.objcreator.obj->SC_Unit,
				img->imageid,
				iscriptcmdstr[iscriptcmd],
				param[0]);
			break;
		case 2:
			fprintf(f, "%s %p %d %d %s %d %d\n", typeimg,
				img->parentimg->creator.objcreator.obj,
				img->parentimg->creator.objcreator.obj->SC_Unit,
				img->imageid,
				iscriptcmdstr[iscriptcmd],
				param[0],
				param[1]);
			break;
		case 3:
			fprintf(f, "%s %p %d %d %s %d %d %d\n", typeimg,
				img->parentimg->creator.objcreator.obj,
				img->parentimg->creator.objcreator.obj->SC_Unit,
				img->imageid,
				iscriptcmdstr[iscriptcmd],
				param[0],
				param[1],
				param[2]);
			break;
		case 4:
			fprintf(f, "%s %p %d %d %s %d %d %d %d\n", typeimg,
				img->parentimg->creator.objcreator.obj,
				img->parentimg->creator.objcreator.obj->SC_Unit,
				img->imageid,
				iscriptcmdstr[iscriptcmd],
				param[0],
				param[1],
				param[2],
				param[3]
			);
			break;
		}
		fclose(f);
	}
}
