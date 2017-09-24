#if !defined(_DEFS_W)
#define _DEFS_W
//#define DEBUGOUTPUT

#ifdef WITHSDL
        #include "sdl/keysdl.h"
#endif
#ifdef UNDERDOS
        #include "dos/keydos.h"
#endif

typedef unsigned char SCUNIT;

#include "wmem.h"

#define STARCLONEEXT		".scu"

#define SOUNDONCAST		0
#define SOUNDONHIT		1

#define NOGAMESTATUS		0x00
#define WINGAME			0x01
#define DEFEATGAME		0x02

#define STIMPACKTIME		(5<<8)
#define STIMPACKMINLIFE		(10<<8)
#define MAXATACKHITS		4
//=============================
#define MAXMANA			(200<<8)
//=============================
#define GROUNDOBJ		0
#define AIROBJ			1
//=============================

#define INTERCEPTORREFRESHMYPARENT		200
#define INTERCEPTORSHIELDRETURN			(10*256)
#define INTERCEPTORLIFELAUNCH			(20*256)
#define INTERCEPTORREGENHEALTH			128		//0.5 life per frame
#define INTERCEPTORDESTMOVEAFTERATACK		300
//=================================

#define DEPLEATEDGETGAZ		2

#define NORMALGETRESOURCES	8

#define TIMETOGETMINERALS		85
#define TIMETOGETGAS			36
#define TIMETOINFESTCC			100
#define WAITNUKEFORCOME			150		//minimal wait
#define ADDITIONALWAITNUKEFORCOME	20		//additional add time (depend on distance launch...fly...land)
#define SCANNERSWEEPTIMELEFT		150
#define GHOSTCANCELNUKE			0
#define GHOSTRELEASENUKE		1



#define MAXBEGINPROBE		4	//four probes-unit at begin
//=============================
#define TIMEFORCLICKBUTON	2

#define TIMETOUNLOADUNIT	20			//time for unload units
#define TIMETOMAKEMAGEDAMAGE	5

#define VERYSLOWSPEED		0
#define SLOWEDSPEED		1
#define SLOWSPEED		2
#define NORMALSPEED		3
#define FASTSPEED		4
#define FASTERSPEED		5
#define FASTESTSPEED		6
#define SPEED2X			7
#define SPEED4X			8
#define SPEED6X			9

//=============================
#define GREYNEUTRALCOLORPLAYER	11		//player 12 is neutral - minerals, gas, etc
//=============================
#include "defsunits.h"
#include "defsmodes.h"

#define MAXPLAYERS		12
#define PLAYEDPLAYERS		8

#define NOLOIMAGE		0
//========================================
#define MAXCONSTRUCTINBUILD	5
//==========================================
#define MEDICHEALFACTOR		(4<<8)

#define BATTERYRECHARGEFACTOR	(2<<8)
#define BATTERYRECHARGEWAITTIME	3
#define QUEENINFESTWAITTIME	200

#define MAXUNITSINTRANPORT	8

#define TIMETOREPAIRINONEPOSITION 200
#define SCVREPAIRFACTORUNIT	1
#define SCVREPAIRFACTORBUILD	2
#define SCVREPAIRWAITTIMEUNIT	22
#define SCVREPAIRWAITTIMEBUILD	11
#define DRONETIMETOGATHERRETURN	101
#define REFRESHMOVEAGAIN	20
//==========================================
#define DESENPYLONAREA		1

#define COLORSHIELD            	123
#define REDLIFE                	175
#define YELLOWLIFE              116
#define GREENLIFE              	185
#define COLORMAGE              	163
#define COLORTIMELEFT          	COLORMAGE
#define BLACKCOLOR             	0
#define WHITECOLOR             	85
#define YELLOWCOLOR            	YELLOWLIFE
#define PI 			3.141592658
#define MAXCHILDS		10

#define MAXINVISIBLECOLOR      	255
#define MININVISIBLECOLOR      	0

#define MAXFONTS		8
#define MAXGAMERCOLORS		16

#define COLOR1           ((REDLIFE<<16)+YELLOWLIFE)
#define COLOR2           ((GREENLIFE<<16)+COLORSHIELD)
#define COLOR3           ((COLORMAGE<<16)+GREENLIFE)
#define COLORHR          0x75
#define _COLOR_RAMKA_    114
#define _COLOR_LIT_      REDLIFE

#define MAXSIZEBUILD 256
#define SIZEXRANGEFORBUILD (MAXSIZEBUILD/SIZESPRLANSHX)
#define SIZEYRANGEFORBUILD (MAXSIZEBUILD/SIZESPRLANSHY)
#define SIZERANGEFORBUILD  (SIZEXRANGEFORBUILD*SIZEYRANGEFORBUILD)
#define PATRATEDIVIDE  2//2*2-odin kvadrat

#define FACTORBUILDTELEPORT 0xa0
#define BLACKGRADCOLOR 32
#define WHITEGRADCOLOR 32
#define BEXPLGRADCOLOR 63

#define MAXNEXTMOVES	5	//additional commands after proceed modemove

#define TORMOZSCROLLA      3
#define MAXMOVEFORROTATION 1
#define REPEATMOVEFORROTATION 1
#define SPEEDDECREMENT   2
#define MINDISTMOVE      30
#define MAXRANGE         15
#define MAXTICKS         256
#define MAXSPEED         20
#define MAXSPEEDRANGE    20
#define HIESTOBJFLY      45
#define NUKE_MAXAIR_YPOS 300
#define INITIAL_NUKE_YPOS 16


#define MAXIMSIZEMINIMAP 128

#define SIZESPRLANSHX 32
#define SIZESPRLANSHY 32

#define MAXBUILDSPRSIZE 8	//=8*sizesprlashx pixels
#define WIDTHRAMKA 2
//#define NUMBRSPRLANSH 500 //max type of lanshaft parts
//#define ROCKETUNITS   500

#define NUMBRSPROBJ   230

#define MAXPASW    70
#define CLEARKEY   1
#define NOCLEARKEY 0
#define UPCASE     0
#define LOWCASE    1
#define SIZE_NAME 10
#define INV 1
#define NOINV 0
#define YES 1
#define NO 0

#define BORDERMOUSE 2

#define MOUSEMODE1 0
#define MOUSEMODE2 1
#define MOUSEMODE3 2
#define MOUSEMODE4 3
#define MAXMOUSEMODE 4

#define MAXMAPONX	256
#define MAXMAPONY	256

#define FACTORMULX  MAXIMSIZEMINIMAP/MAXXMAP
#define FACTORMULY  MAXIMSIZEMINIMAP/MAXYMAP

#define BEGINMAGE    55
#define MAXSELECTMAN 12

#define VARDESTINATIONROCKET 1	//if 0-maker,1-destination
#define VARBUILD     		2
#define VARMOVIE     		4
#define VARFULLLIFE  		4
#define SHADOWS      		0x80

//prop
#define VARNOTWORK  	  	0x00000001
#define VARNOTHERE      	0x00000002
#define VARINTRANSPORT  	0x00000004
#define VARREADY        	0x00000008
#define VARINCRMAGE     	0x00000010 	//1-decr,0-incr
#define VARPYLONAREAACTIVE	0x00000020	//obj have 4 imageoverlays with pylon areas
#define VARBURROW		0x00000040
#define VARPOWEROFF		0x00000080
#define VARONSKY		0x00000100
#define VARMOVETHROWUNITS 	0x00000200
#define VARNOTSEEPROP		0x00000400
#define	VARSPECIALWAIT		0x00000800	//siege/normal action
#define VARCANTSELECT		0x00001000
#define VARMOVEINATACKMODE	0x00002000
#define VARDONOTAPPLYNEXTMOVE	0x00004000
#define VARWAITDISTANCE		0x00008000
#define VARINBASE		0x00010000
#define VARLARVAEONCONSTRUCT	VARINBASE
#define VARAUTOMATRESOURCERET	0x00020000
#define VARIFDIEDESTTOTERRAIN	0x00040000	//move to last known coords of object
#define VARDIESILENTLY		0x00080000
#define VARATACKAIROBJ		0x00100000
#define VARKARTCHANGES		0x00200000
#define VARMARKFORDESTROY	0x00400000
#define VARACCELERATIONBIT	0x00800000
#define VARHOLDPOSBIT		0x01000000
#define VARHAVEGOAL		0x02000000
#define UNITPROP_PREVCLOAK	0x04000000
#define VARINVINCIBILITY	0x08000000
#define VARAUTOMATICDOODAD	0x10000000
#define VARMOVEACT		0x20000000
#define VARMOVEOBJACT		0x40000000
#define VARPATROLFLAG		0x80000000

//=============================
#define MOVEOBJ_NOACT			0
#define MOVEOBJ_DONE			1
#define MOVEOBJ_CONTINUEJOB		2
#define MOVEOBJ_STOPANDCONTINUEJOB	3
#define MOVEOBJ_WAITUNTIL		4
//=============================
#define MINNOMOVE             	1
#define MINDISTFORMOVE        	16
//=======================
#define ONLYTERRA  2
#define DEFAULT    0
//=======================
#define ORDER_RIGHTWAY			0x01
#define ORDER_IGNOREREQ			0x02
#define ORDER_CANBEQUEUED		0x04
#define ORDER_CANNOTBEINTERRUPTED	0x08
#define ORDER_SHOWHALLUCINATED		0x10
#define ORDER_ONTERRAIN			0x20
#define ORDER_IGNOREUNITDESTINATION	0x40
//=======================
#define ENEMYOBJ     	0
#define NEUTRALOBJ   	1
#define ALLIANCEOBJ   	2
#define MYOBJ        	3

#define PLAYER_NOVISION 0
#define PLAYER_VISION	1

//============================= required for file obj.h ==============
#define SIZEXKUBIK	3
#define SIZEYKUBIK	4

#define CIRCLE1                253-1
#define COLOREMYOBJ            GREENLIFE
#define COLORENEUTRALOBJ       YALOWLIFE
#define COLOREENEMYOBJ         REDLIFE
//===========================================
#define TERRAIN                	0x01
#define UNITS                  	0x02

#define MAPREGENERATION        	60

#define BEGINCONSTRDELTA	20

//OBJstruct
#define NOADDSTATSATCONSTR	0x01
#define VARSELECT4NOTSEEPROP	0x02
#define VARNEEDRECHARGE      	0x04



//===========================================
#define MAXOBJDEFSCR    3000               //maxobj for desen on screen
#define MAXDAMDEFSCR    3000               //max 'vzrivov' for desen on screen
#define MAXOBJECTONMAP  (MAXOBJDEFSCR*2)   //max objects on Karta(obj&builds)

//===========================================
#define MAXPSI    (200*2)
//===========================================
//#define NOCLEARPOSINPOS -2
//#define CLEARPOSINPOS   -1
//#define BEGINPOS (0)
//===========================================
#define MAXFACEDIRECTIONS	32
//===========================================
#define ZERGRACE	0
#define TERRANRACE	1
#define PROTOSSRACE	2
#define MAXRACES	3
#define NEUTRALRACE	6
//===========================================
#define ATRPARASITEFROM 	0
#define ATRHALLUCINATION 	1
#define ATRCORROSIVEACID 	2
#define ATRBLIND        	3
#define ATRDETECTOR     	4
#define ATRENSNARE      	5
#define ATRMATRIX       	6
#define ATRSTIMPACK		7
#define ATRINVISIBLE    	8
#define ATRLOCKDOWN     	9
#define ATRMAELSTROM    	10
#define ATRSTASIS       	11
#define ATRPLAGUE       	12
#define ATRIRRADIATE    	13
#define ATRRECALL		14
//#define ATRRECHARGESHIELD	15

#define MAXMAGEATR		15
//===========================================
#define MAXATRMAGE 		(250<<8)
#define TERMINATEATRMAGE 	(251<<8)
#define ATR_ONE_DECR_MAGE	(252<<8)
#define WRAITHEPODARBITEROM   	(253<<8)
#define NEWRAITHEPODARBITEROM 	(254<<8)
#define ATRMAGEINFINITE 	(255<<8)
//===========================================

#define MAGEATRONLIFE		0x1
#define MAGEATRONCYBORG		0x2
#define MAGEATRONMAGED		0x4
#define MAGEATRONBUILD		0x8
#define MAGEATRONMYSTERY	0x10
#define MAGEATRFLYUNIT		0x20
#define MAGEATRTERRAUNIT	0x40
#define MAGEATRRESOURCEUNIT	0x80
#define MAGEATRPUTONNOUNDER	0x100
#define MAGEATRPUTONOBJ		0x200
#define MAGEATRMYUNITS		0x400
#define MAGEATRMYRACE		0x800
#define MAGEATRPUTNOTECHNIC	0x1000
#define MAGEATRNOMYUNITS	0x2000
#define MAGEATRONOTHER		0x4000

#define BLINKTICKS		0x10
#define BLINKTIME		BLINKTICKS*5

#define MAPSCROLLTICKS 		3
#define NEXTLARVAEMOVETICK	100
//=========================================
//lo statuses
#define LOFIRSTINV		0x01
#define LONOINV			0x02
#define LOCOPYPREVMIRROR	0x04
#define LONOCLEAR		0x08

#define LOMALLOCKED		0x80
//=========================================
//=====================line properties
#define PROPDECONSTRUCT		0
#define PROPNORMAL1		1
#define PROPNORMAL2		2
#define PROPDEUPGRADE		3
#define PROPONAIR		4
#define PROPBUILDSIMPLE		5
#define PROPBUILDADVANCED	6
#define PROPNUKECANCEL		7
#define PROPEMPTY		8
#define MAXLINESPROPERTIES	9

#define MAXUNITPROPERTIES	9

//=====================misc sounds
#define PUTMAGEERROR		3
#define PUTLIFTDOWNERROR	2

//====================================
//output for in-build construction units
#define YDECICONS 120
#define YADDICONS (GRP_screensizey-480)
#define XADDICONS 0

#define XOUT (248+XADDICONS)
#define YOUT (402+YADDICONS)

#define XUNITBAR (XOUT-70)
#define YUNITBAR (YOUT+3)


#define XUNITTRANSP (XOUT )
#define YUNITTRANSP (YOUT+3)

#define XUNITCONSTR XUNITTRANSP
#define YUNITCONSTR (YUNITTRANSP-3)
//#define XUNITCONSTR (XOUT-2)
//#define YUNITCONSTR (YOUT-2)

#define POSX(x) 	((x+SIZESPRLANSHX-1)/SIZESPRLANSHX)
#define POSY(y) 	((y+SIZESPRLANSHY-1)/SIZESPRLANSHY)


//=====================================
#define PLUSFACTOR  (1)
#define MINUSFACTOR (-1)

//=====================================
//movetypes


#define MAXGEYSERGASNR			4000
#define MAXMINERALSNR			1500

#define DRONEBUILD_STORONASVETA		8

#define MAXCHATSYMB			50
#define SYSTEMCHAT			-1

#endif
