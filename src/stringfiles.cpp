
#include "mouse.h"

char *ires[3][3]={
	{"icons\\zmin.pcx","icons\\zgas.pcx","icons\\zpsi.pcx"},
	{"icons\\tmin.pcx","icons\\tgas.pcx","icons\\tpsi.pcx"},
	{"icons\\pmin.pcx","icons\\pgas.pcx","icons\\ppsi.pcx"}
	};
char *imana="icons\\mana.pcx";
char *iemptycube="icons\\cube.pcx";
char    *filemans[]={
		    "./data/units",
		    ""
		    };
char    filemageprop[]="./data/orders";
char    fileweaponsconf[]="./data/weapons";

char    tselect[]="game\\tselect.pcx";
char    tminimap[]="game\\tminimap.pcx";
char    tunit[]="game\\tunit.pcx";
char    icons[]="unit\\cmdbtns\\ticon.pcx";
char	TEMPDIR[]="temp/";
char    transp[]="\\trans50.pcx";
char    blackgrd[]="\\dark.pcx";
char    whitegrd[]="\\bexpl.pcx";
char    redgrd[]="\\ofire.pcx";
char    greengrd[]="\\gfire.pcx";
char    bluegrd[]="\\bfire.pcx";
char    twire[]="game\\twire.pcx";
char    thpbar[]="game\\thpbar.pcx";
char    tfontgam[]="game\\tfontgam.pcx";
char	tblink[]="game\\tblink.pcx";

char    redtr[]="\\red.pcx";
char    greentr[]="\\green.pcx";
char    bluetr[]="\\shift.pcx";

char *allfilescursors[TYPEOFCURSORS]={	
					"drag.grp",
					"illegal.grp",
					"magg.grp",
					"magr.grp",
					"magy.grp",
					"scrolld.grp",
		    			"scrolldl.grp",
					"scrolldr.grp",
					"scrolll.grp",
					"scrollr.grp",
					"scrollu.grp",
					"scrollul.grp",
					"scrollur.grp",
					"targg.grp",
					"targn.grp",
					"targr.grp",
					"targy.grp",
					"time.grp",
					""		//arrow.grp
				      };

char	*palettedir[]={
			"tileset\\badlands",
			"tileset\\platform",
			"tileset\\install",
			"tileset\\ashworld",
			"tileset\\jungle",
			"tileset\\desert",
			"tileset\\ice",
			"tileset\\twilight"
		      };
char	*fontname[MAXFONTS]={
			"font\\font8.fnt",
			"font\\font10.fnt",
			"font\\font12.fnt",
			"font\\font14.fnt",
			"font\\font16.fnt",
			"font\\font16x.fnt",
			"font\\font32.fnt",
			"font\\font50.fnt"
			};
char    iconsgrp[]="unit\\cmdbtns\\cmdicons.grp";
//unit in bar with shp&hp
char	wire1[]="unit\\wirefram\\grpwire.grp";
char	wire2[]="unit\\wirefram\\tranwire.grp";
char	wire3[]="unit\\wirefram\\wirefram.grp";
char	blinkgrpfn[]="game\\blink.grp";
char	GLUCHAT_DOWNLOADPROGRESSICON[]="icons\\dwnlmap.pcx";
char	MAINMENU_DOWNLOADPROGRESSICON[]="icons\\dwnlbar.pcx";
char	STAREDITCHK[]="staredit\\scenario.chk";

char	RACE_CHAR[3]={'Z','T','P'};
char	LOSEWIN_STR[2]={'D','V'};
char	*DEFEAT_VICTORY[2]={"defeat","vict"};

char GAMECURSOR_NAME[30]="cursor";
char ARROW_STR[]="\\arrow.grp";

