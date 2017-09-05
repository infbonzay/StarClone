#if !defined(_MAN_W)
#define _MAN_W


#include "defs.h"
#include "myfifo.h"
#include "construct.h"
#include "audio.h"
#include "mylist.h"
#include "creep.h"
#include "mpq.h"

#define MAXRANDOMSPR		3
#define MAXSKMTYPEOFPORTRAITS	4
//=======================================
struct SMKPORTRAITS
{
	HANDLE hmpq;
	unsigned char maxscrollSMKNORMAL;
	unsigned char max[MAXSKMTYPEOFPORTRAITS];
	unsigned short int mpqfilenr[MAXSKMTYPEOFPORTRAITS][4];
};
//=======================================
struct SFXPLAY
{
	unsigned char	nrofmusicsplayed;		//max played sounds by unit
	unsigned char	nextmusicplay;			//next sound to play
	signed   char	nrsfxplayed;			//type of play sound 
	signed   char	sfxtypeplayed;			//nr of typesound play
	unsigned char	nrselectplayed;			//nr select play need for pss play
};

/*struct SFXUNIT
{
	short		int sfxfrom[MAXTYPESOFSOUND];
	short		int sfxto[MAXTYPESOFSOUND];
};
*/
struct	ROCKETINFO
{
	int 		info;		//bit 0	- 0-maker,1-destination
	int 		nrmage;
	signed 	char 	mageatroffset;
	unsigned	char 	nrrockets;
	signed 	char 	deltastoronasveta[SPRITE_SIDES];
};
struct CHILDS
{
	signed	char	nrofchildunits;			//nr of childs
//	unsigned short	childids[MAXCHILDS];		//if 0 child was flying
//	unsigned short	timerechargeonbase[MAXCHILDS];	//if !=0 child on base and recharging & childids is !0
	struct	OBJ	*parentof[MAXCHILDS];		//parent of MAXCHILDS units
};
//=======================================
struct LOADED
{
	signed	char	nrofloadedunits;
	signed	char	weight;
	signed	char	nrunittounload;
	signed  char	waittickstounload;		//wait for unload next time
	signed	char	cellobj[MAXUNITSINTRANPORT];
	struct	OBJ	*loadedunits[MAXUNITSINTRANPORT];
};
//=======================================
struct ATROBJ
{
        int 	mageatribute[MAXMAGEATR];   //vremea derjania maghii
	int	x256,y256;
	struct	OBJ	*arbiterrecallobj;
	char	flags;
};
//=======================================
struct 	LOINF
{
    struct	LOFILE  	*lo;
		char		fromloline;
		char		fromlooffset;
		char		status;
};
//=======================================
struct MOVESTRUCT
{
    struct 	LOINF	loinf;
    struct	GRPFILE *grp;
      
    unsigned	short   offsfirstpict;
    unsigned	char    maxsides;
      
    unsigned	char    tabltransp; //0 - none;1-redtable,2-greentable,3-bluetable,4-whitetable,5-blacktable
				    //bit 8 - is inversion picture
		char    anim[MAXSTEP];
};
//=======================================
struct APPEARORDER
{
    GRPFILE *grp;
    unsigned char  addgrppict;
    char  grpmethod;
    char  grptable;
};

//=======================================
/*struct MOVESTRUCTMAGE
{
    struct	GRPFILE *grp[3];
      
    unsigned	short   offsfirstpict;
    signed	char    step;
    unsigned	char    maxsides;
      
    signed	short   time;      //
    unsigned	char    tabltransp;//0 - none;1-redtable,2-greentable,3-bluetable,4-whitetable,5-blacktable,6-distortion
		char    anim[MAXSTEP];
};
*/
//=======================================
#define	MAXDAMAGEBLOODFLAMEOVERLAYS	3
//=======================================
class MAIN_IMG;
class OVERLAY_IMG;
//=======================================
union SOMEINFO
{
    struct
    {
	signed char	rechargetime;
    }battery;
    struct
    {
	MAIN_IMG	*hitdot;
    }nukehitdot;
    struct
    {
	short	int remaintimetoinfest;
	char	actionbit;
    }queeninfest;
    struct
    {
	unsigned char   restype;
	unsigned char	rescnt;
    }resourcechunk;
    struct
    {
	int 		resource_count;
	int 		max_resource_count;
    }resource;
    struct
    {
	signed char	state1;
	signed char	movedirection;
    }doodad;
    struct
    {
	signed char	gathertime;
	signed char	waittoharvest;
	signed char	waitforrepair;
    }gather;
    struct
    {
	OBJ 		*obj;
    }geyserdest;
};
//=======================================
struct OBJ
{
	MAIN_IMG	*mainimage;
	short 		damageoverlayid[MAXDAMAGEBLOODFLAMEOVERLAYS];
	signed char	lastdamageoverlays;
	int 		(*OrderFunc)(OBJ *);
	unsigned char	selectedbits;
	unsigned char	SC_Unit;   	//starcraft unit number
	unsigned char	SC_ConstrUnit;	//starcraft constr unit number 
	unsigned char	SC_FromUnit;	//from what unit nr, this unit morphing, or 255 if none
        unsigned char	timerepair;	//timeme for repair in one position
        unsigned short	castmagenr;

	unsigned short	startx;
	unsigned short	starty;
	int		topspeed;
	int 		haltdistance;
	int		modemoveminusdistance;
	short		deacceleration;
	int		finalx;
	int		finaly;
	int		finalxatack;
	int		finalyatack;
	OBJ		*finalOBJ;
	int		currentspeed;	//for unit move by flingy control
	short		deltavertpos;
	
        OBJ		*whoatack;        	//cto atacuet?||hostobj(rocket)
	OBJ		*carryobj;	  	//kakoi obj nesiom
	OBJ		*replaceobj;		//obj who not work (under me)
	OBJ		*addonobj;		//addon obj)
	
	OBJ		*resourceobj;		//object from what we know get resources
	OBJ		*prevresourceobj;	//prev object from what we get resources
	
	OBJ		*subunit;
	
        OBJ		*constrobj;       //ili kakoi object tebea stroit(terran build)
        ATROBJ		atrobj;           //atribute(inv,slow
                                             //plague,...)
        unsigned short	nrobject;	  //nomer sozdannogo objecta

	unsigned char	searchforatack_tick;	  //
	int		creepbuildlist_ID;
	int 		createcycle;
	int 		serial1;
	int 		serial2;
	char		paried;

	SOMEINFO 	data;

        int  		templife,life;
        int		timeleft,dmatrix;
        int 		shield,tempshield;

        int		shielddamage,lifedamage,dmatrixdamage,psistormdamage;
	
	signed char	atackcooldowntime;	//coolddown after atack
	unsigned char	usedweapon_id;
	
        unsigned char	curentmode;
        unsigned char	mines;
//	unsigned char	corrosiveacidsprnr;

        unsigned char	color;               //tsvet unita
        unsigned char	whenregen;

        unsigned char	playernr;            //nomer igroca
        signed   char	atackernr;           //nomer atakuiushego igroca
        unsigned short	kills;			//nr of killed units
        unsigned short	modemove;            //0-stop,1-move,2-atack,...
	myfifo		*movelist;
	unsigned char	typeofprop;	     //line in OBJstruct modemove
        unsigned short	memoryformage;       //0-stop,1-move,2-atack,...
        int		mana;                //mana

        unsigned char	xkart;               //
        unsigned char	ykart;               //coord.

	SFXPLAY		sfxplay;

        unsigned	char	visibleby;
	CONSTRUCT	*construct;
	SELFCONSTRUCT	selfconstruct;
        CHILDS		*childs;
	OBJ		*myparent;		//my parent
	OBJ		*doubleunit;		//if one unit die, second unit die too
	LOADED		*loaded;
	OBJ		*in_transport;
	OBJ		*baseobj;		//object from what we get resources
	    
	unsigned char	triggerstate;
	unsigned char 	blinkvalue;
	unsigned char   shielddamageenabled;	//if 1 the shield appear on damaged unit
	unsigned char   shieldrechargeenabled;	//if 1 the recharge shield appear

	unsigned short	whatbuildconstr;
	signed short	timeflybeforerecharge;	//time left before need to recharge on base
	unsigned char	curentatacknr;		//curent atack number ,if ==0 and maxbullets!=0 need recharge on base

	unsigned char	obj_see;			//8-bit for each players
	unsigned char	obj_invsee;			//8-bit for each players
	unsigned char	obj_notdetect;			//8-bit for each players
	
        unsigned int	prop;          //see man.h

};
//=======================================
#define OBJ_VAR_MASK_SET(a,TYPEPARAM,mask) ((a->TYPEPARAM) |= (mask) )
#define OBJ_VAR_MASK_CLR(a,TYPEPARAM,mask) ((a->TYPEPARAM) &= (~(mask)) )
#define OBJ_VAR_MASK_CHK(a,TYPEPARAM,mask) ((a->TYPEPARAM) && (mask) )

#define OBJ_VAR_SET(a,TYPEPARAM,playernr) OBJ_VAR_MASK_SET(a,TYPEPARAM,(1<<playernr))
#define OBJ_VAR_CLR(a,TYPEPARAM,playernr) OBJ_VAR_MASK_CLR(a,TYPEPARAM,(1<<playernr))
#define OBJ_VAR_CHK(a,TYPEPARAM,playernr) OBJ_VAR_MASK_CHK(a,TYPEPARAM,(1<<playernr))
//=======================================
struct ONEMODEMOVE
{
    struct OBJ *destobj;
    int 	destx;
    int 	desty;
    unsigned short modemove;
    unsigned char showerrflag;
};
//=======================================
#define MAXEFFECT	6
//=======================================
struct OBJstruct
{
//	unsigned short	iscriptid;
	unsigned char	SC_Unit;
	unsigned char	SC_CreateFromUnit;
//	struct	ATROBJ	atrobj;
	unsigned int	timelife;		//vremea zagruzki structuri

	unsigned short	addobj;
	signed   short  additionaltext;		//point to stat_txt.tbl

	signed char	creeptype;		//creeptype have have 0 ,sunken&creep have 1

//	signed char	movebeforecooldown;
//	signed char 	movecooldowntime;

	signed char     armorupgnr;	//special armor ultralisk-only
	signed char 	sightupgnr;	//special sight
	signed char	manaupgnr;	//special mana
	unsigned char	speedupgnr;	//reference toupgrade/technology for increace speed
	int		speeduppercentage;
	signed char	atackspeedupgnr;//reference toupgrade/technology for increace atack speed
	
	int		repairmin;	//need minerals for 1-tick repair
	int		repairgas;	//need gas      for 1-tick repair
	int		repairlife;	//add  life	for 1-tick repair
		
//	float		speed;

//	int		acceleration;

//        unsigned	char	objdamageup;           	//kakim objectom streleaet(Nr)
//	unsigned	char	objdamagedown;         	//kakim objectom streleaet(Nr)

	unsigned	char	maxlineprop;		//see down
	//MAXLINEPROPERTIESx9 sposobnostei(moves,build,advbuild)
	unsigned	short	modemove[MAXLINESPROPERTIES][9];
//	unsigned	char	maxsides;
//	unsigned	char	atacksides;
//	unsigned	short	maxdamageradius,fullradius;


	
        signed	char	nrupgrade;

//        int 		grpfirstnr;
//	int 		applicablelofirstline;

//	unsigned char	atackhitticks[MAXATACKHITS];
//	unsigned char	maxatackhits;

//	SFXUNIT		sfx;

	SMKPORTRAITS	*portraits;
//	ROCKETINFO	*rocketinfo;

	struct	LOFILE	*subunit_lo;
	
	unsigned char	cloakrange;            	//only arbiter

	unsigned short 	maxtimeleft;
	
//	unsigned short	sizex;
//	unsigned short	sizey;

	unsigned short	timerechargeonbase;		//if select4,bit2 is set
	signed	 short	timeflybeforerecharge;		//time left before need to recharge on base
//	unsigned short	maxparalelmodes;
//	unsigned short	paralelmodes[8];
//	unsigned char	accelT;
//	unsigned char	incrementspeed;
//	unsigned char	factorzav;
	unsigned char	race;
//	unsigned char	maxbullets;			//max atacks before recharge on base

	unsigned int	UNITprop; 
    				    //(0-bit)-select
                                    //(1-bit)-build/man ()
                                    //(2-bit)-(build from build(zerg's build))
                                    //(3-bit)-fullinv
                                    
                                    //(4-bit)-see
                                    //(5-bit)-flyterra
                                    //(6-bit)-fireup
                                    //(7-bit)-firedown

                                    //8-(CYBORG)
                                    //9-(LIFE)
                                    //10-(OTHER)-(li4inka,mina,minerals,gas...)
                                    //11-bit)-KA4IANIE
                                    
                                    //12-bit)-ACCELER
                                    //13-afteratackendatack
                                    //14-none1
                                    //15-none2

    				    //16-bit - externshadow
                                    //17-bit - non die unit (minerals,gaz,...)
                                    //18-bit - notdesen
                                    //19-bit - rocketunit
                                    
                                    //20-bit - readydeconstr (dron srazu poiavleaetsea)
                                    //21-bit) - {1-not inertion}
                                    //22-bit) - if 1 - use doneteleportbuild    
                                    //23-bit) - rotation BUILD

    				    //24-bit - VARSELECT4NOTSEEPROP (ne mogu videti properties unita)
				    //25-bit - may have a child
				    //26-bit - limit of charges(need return to base)(like interceptors)
				    //27-bit - if build on other obj that obj is not worked before build not desapperied

				    //28-bit - after rotation the speed don't decrease
				    //29-bit - damage on die
				    //30-bit - noused
				    //31-bit - the mage object
				    
	unsigned char      range[SIZERANGEFORBUILD];
};
//=======================================
//=======================================
struct DEPENDONTYPE
{
    unsigned char	maxdepend;
    unsigned char	type_id[3];
    unsigned char	obj_id[3];
    unsigned char	obj_id2[3];		//for addon types
};
//=======================================
struct MAGEP
{
//	unsigned char		*feedbackvar[MAXPLAYERS];
//	MOVESTRUCTMAGE 		moves[5][2];
        signed   char		timewait[5];
        unsigned short		nsprite;            //nomer tipa
        int			mindistance;
        int			damage;
        signed	 short  	modemoveafter;
        unsigned short  	beginwait;
        signed   short  	icon_id;		//cmdicons.grp
	unsigned short		dependtechnology;
	unsigned char		order_id;		//point to orders.dat
	
	signed   char 		race;			//race if mage is create unit
        signed 	 char		atronobj;		//or -1 if no atr offset
	unsigned short		images_id[3];       	//image_id for small,meduim,large
        unsigned short		sound_id[2];
        signed	 char		soundmode;
        int 			diapazone;
        unsigned char   	keyfororder;
	unsigned short		rechargebuttontime;
        unsigned char  		atr;	
                                    //0-bit - rightway
				    //1-bit - ignore mineral,gas,mana,psi requirements
				    //2-bit - can be queued to list of moves
				    //3-bit - can use/show than hallucinated
				    //4-bit - on terrain

	unsigned int	timemageactive;
	unsigned char	type_id;		//type of command
	short		obj_id;			//depend on type_id
	short		stattxt_id_disable[3];	//disabled 3 strings(for every level of upgrade)
						//if type_id is upgrade from stattxt.tbl
	short		stattxt_id_enable;	///enabled string
	APPEARORDER	*appear;
	unsigned char	createweapon;		//weapon_id or 255
	unsigned char   weapon_elevation;	//elevation nr

	DEPENDONTYPE	depend;
    	char		namemage[50];
};

//extern struct MAGEARRAY *firstmage;
extern struct OBJstruct *allobj[];
extern char   Minimap[];

//====================
#endif /*_MAN_W*/





