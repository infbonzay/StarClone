#if !defined(_MAN_W)
#define _MAN_W


#include <grplib/grp.h>
#include "debug.h"
#include "defs.h"
#include "myfifo.h"
#include "construct.h"
#include "mpq.h"
//#include "images.h"

#define MAXRANDOMSPR			3
#define MAXSMKTYPEOFPORTRAITS	4
//=======================================
struct SMKPORTRAITS
{
		HANDLE hmpq;
		unsigned short int mpqfilenr[MAXSMKTYPEOFPORTRAITS][4];
		unsigned char maxscrollSMKNORMAL;
		unsigned char max[MAXSMKTYPEOFPORTRAITS];
};
//=======================================
struct SFXPLAY
{
		unsigned char	nrofmusicsplayed;				//max played sounds by unit
		unsigned char	nextmusicplay;					//next sound to play
		signed	 char	nrsfxplayed;					//type of play sound 
		signed	 char	sfxtypeplayed;					//nr of typesound play
		unsigned char	nrselectplayed;					//nr select play need for pss play
};

struct CHILDS;
//=======================================
struct LOADED
{
		struct	OBJ		*loadedunits[MAXUNITSINTRANPORT];
		signed	char	nrofloadedunits;
		signed	char	weight;
		signed	char	nrunittounload;
		signed	char	waittickstounload;				//wait for unload next time
		signed	char	cellobj[MAXUNITSINTRANPORT];
};
//=======================================
struct ATROBJ
{
		struct	OBJ		*arbiterrecallobj;
		int				mageatribute[MAXMAGEATR];	//vremea derjania maghii
		int				x256,y256;
		unsigned char	flags;
};
//=======================================
struct	LOINF
{
	struct		LOFILE			*lo;
				char			fromloline;
				char			fromlooffset;
				char			status;
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
#define MAXDAMAGEBLOODFLAMEOVERLAYS		3
//=======================================
class MAIN_IMG;
class OVERLAY_IMG;
//=======================================
union SOMEINFO
{
	struct
	{
		unsigned char	rechargetime;
	}battery;
	struct
	{
		MAIN_IMG		*hitdot;
	}nukehitdot;
	struct
	{
		short			int remaintimetoinfest;
		char			actionbit;
		unsigned char	infestrefreshbit;
	}queeninfest;
	struct
	{
		unsigned char	restype;
		unsigned char	rescnt;
	}resourcechunk;
	struct
	{
		int				resource_count;
		int				max_resource_count;
	}resource;
	struct
	{
		signed char		state1;
		signed char		movedirection;
	}doodad;
	struct
	{
		signed char		gathertime;
		signed char		waittoharvest;
		signed char		waitforrepair;
	}gather;
	struct
	{
		OBJ				*obj;
	}geyserdest;
	struct
	{
		unsigned char	refreshmyparent;
	}interceptor;
	struct
	{
		unsigned char	ishealtime;
	}medic;
	struct
	{
		unsigned char	waitbeforemove;
	}critter;
	struct
	{
		int serial1;
		int serial2;
//		char paried;
	}beforestartplay;
};
//=======================================
struct OBJ
{
		MAIN_IMG		*mainimage;
		int				(*OrderFunc)(OBJ *);
		OBJ				*finalOBJ;
		OBJ				*whoatack;				//cto atacuet?||hostobj(rocket)
		OBJ				*carryobj;				//kakoi obj nesiom
		OBJ				*replaceobj;			//obj who not work (under me)
		OBJ				*addonobj;				//addon obj)
		
		OBJ				*resourceobj;			//object from what we know get resources
		OBJ				*prevresourceobj;		//prev object from what we get resources
		
		OBJ				*subunit;
		
		OBJ				*constrobj;		  //ili kakoi object tebea stroit(terran build)

		myfifo			*movelist;
		CONSTRUCT		*construct;
		SELFCONSTRUCT	selfconstruct;
		CHILDS			*childs;
		OBJ				*myparent;				//my parent
		OBJ				*doubleunit;			//if one unit die, second unit die too
		LOADED			*loaded;
		OBJ				*in_transport;
//		OBJ				*baseobj;				//object from what we get resources

		int				topspeed;
		int				haltdistance;
		int				modemoveminusdistance;
		int				finalx;
		int				finaly;
		int				finalxatack;
		int				finalyatack;
		int				currentspeed;	//for unit move by flingy control
		int				creepbuildlist_ID;
		int				createcycle;
//		int				serial1;
//		int				serial2;
		int				temphealth,health;
		int				timeleft,dmatrix;
		int				shield,tempshield;
		int				shielddamage,lifedamage,dmatrixdamage,psistormdamage;
		int				mana;				 //mana
		unsigned int	prop;							//see man.h

		short			damageoverlayid[MAXDAMAGEBLOODFLAMEOVERLAYS];
		short			deacceleration;
		short			deltavertpos;
		unsigned short	castmagenr;
		unsigned short	startx;
		unsigned short	starty;
		unsigned short	nrobject;		  //nomer sozdannogo objecta
		unsigned short	kills;					//nr of killed units
		unsigned short	modemove;			 //0-stop,1-move,2-atack,...
		unsigned short	memoryformage;		 //0-stop,1-move,2-atack,...
		unsigned short	whatbuildconstr;

		signed char		lastdamageoverlays;
		unsigned char	selectedbits;
		unsigned char	SC_Unit;		//starcraft unit number
		unsigned char	SC_ConstrUnit;	//starcraft constr unit number 
		unsigned char	SC_FromUnit;	//from what unit nr, this unit morphing, or 255 if none
		unsigned char	timerepair;		//timeme for repair in one position
		unsigned char	searchforatack_tick;
		unsigned char	movetoobj_tick;
		char			paried;
		signed char		atackcooldowntime;		//coolddown after atack
		unsigned char	usedweapon_id;
//		  unsigned char curentmode;
		unsigned char	ammo;

		unsigned char	color;				 //tsvet unita
		unsigned char	whenregen;

		unsigned char	playernr;			 //nomer igroca
		signed	 char	atackernr;			 //nomer atakuiushego igroca
		unsigned char	typeofprop;			 //line in OBJstruct modemove

		unsigned char	xkart;				 //
		unsigned char	ykart;				 //coord.
		unsigned char	visibleby;
		unsigned char	childnr;

		unsigned char	triggerstate;
		unsigned char	blinkvalue;
		unsigned char	shielddamageenabled;	//if 1 the shield appear on damaged unit
		unsigned char	shieldrechargeenabled;	//if 1 the recharge shield appear

		
		ATROBJ			atrobj;			  //atribute(inv,ensn,plague,...)
		SOMEINFO		data;
		SFXPLAY			sfxplay;


		unsigned char	obj_see;						//8-bit for each players
		unsigned char	obj_invsee;						//8-bit for each players
		unsigned char	obj_notdetect;					//8-bit for each players

};
//=======================================
#define OBJ_VAR_MASK_SET(a,TYPEPARAM,mask) ((a->TYPEPARAM) |= (mask) )
#define OBJ_VAR_MASK_CLR(a,TYPEPARAM,mask) ((a->TYPEPARAM) &= ~(mask) )
#define OBJ_VAR_MASK_CHK(a,TYPEPARAM,mask) ((a->TYPEPARAM) & (mask) )

#define OBJ_VAR_SET(a,TYPEPARAM,playernr) OBJ_VAR_MASK_SET(a,TYPEPARAM,(1<<playernr))
#define OBJ_VAR_CLR(a,TYPEPARAM,playernr) OBJ_VAR_MASK_CLR(a,TYPEPARAM,(1<<playernr))
#define OBJ_VAR_CHK(a,TYPEPARAM,playernr) OBJ_VAR_MASK_CHK(a,TYPEPARAM,(1<<playernr))
//=======================================
struct CHILDS
{
		struct	OBJ		*parentof[MAXCHILDS];			//parent of MAXCHILDS units
		signed	char	nrofchildunits;					//nr of childs
};
//=======================================
struct ONEMODEMOVE
{
	struct OBJ *destobj;
	int			destx;
	int			desty;
	unsigned short modemove;
	unsigned char showerrflag;
};
//=======================================
struct OBJstruct
{
		SMKPORTRAITS	*portraits;
//		struct	LOFILE	*subunit_lo;

		unsigned int	timelife;				//vremea zagruzki structuri
		int				speeduppercentage;
		int				repairmin;		//need minerals for 1-tick repair
		int				repairgas;		//need gas		for 1-tick repair
		int				repairlife;		//add  life		for 1-tick repair

		unsigned short	addobj;
		signed	 short	additionaltext;			//point to stat_txt.tbl
		unsigned		short	modemove[MAXLINESPROPERTIES][9];
		unsigned short	maxtimeleft;
//		unsigned short	timerechargeonbase;				//if select4,bit2 is set
//		signed	 short	timeflybeforerecharge;			//time left before need to recharge on base

		unsigned char	range[SIZERANGEFORBUILD];
		unsigned char	SC_Unit;
		unsigned char	SC_CreateFromUnit;
		signed char		creeptype;				//creeptype have have 0 ,sunken&creep have 1
		signed char		armorupgnr;		//special armor ultralisk-only
		signed char		sightupgnr;		//special sight
		signed char		manaupgnr;		//special mana
		unsigned char	speedupgnr;		//reference toupgrade/technology for increace speed
		signed char		atackspeedupgnr;//reference toupgrade/technology for increace atack speed
		unsigned		char	maxlineprop;			//see down
//		  signed		char	nrupgrade;
		unsigned char	cloakrange;				//only arbiter
//		unsigned char	race;
		unsigned char	UNITprop; 
};
//=======================================
//=======================================
struct DEPENDONTYPE
{
	unsigned char		maxdepend;
	unsigned char		type_id[3];
	unsigned char		obj_id[3];
	unsigned char		obj_id2[3];				//for addon types
};
//=======================================
struct MAGEP
{
		APPEARORDER		*appear;

		int						diapazone;
		int						mindistance;
		unsigned int	timemageactive;

		signed	 short			icon_id;				//cmdicons.grp
		unsigned short			dependtechnology;
		unsigned short			images_id[3];			//image_id for small,meduim,large
		unsigned short			sound_id[2];
				 short			obj_id;					//depend on type_id
				 short			stattxt_id_disable[3];	//disabled 3 strings(for every level of upgrade)
				 short			stattxt_id_enable;		///enabled string
		
		//unsigned char			orderdatid;				//point to orders.dat
		unsigned char			techid;					//use technology id
		signed	 char			race;					//race if mage is create unit
		signed	 char			atronobj;				//or -1 if no atr offset
		signed	 char			soundmode;
		unsigned char			keyfororder;
		unsigned char			atr;	
									//0-bit - rightway
									//1-bit - ignore mineral,gas,mana,psi requirements
									//2-bit - can be queued to list of moves
									//3-bit - can use/show than hallucinated
									//4-bit - on terrain
		unsigned char	type_id;				//type of command
												//if type_id is upgrade from stattxt.tbl
		unsigned char	createweapon;			//weapon_id or 255
		unsigned char	weapon_elevation;		//elevation nr

		DEPENDONTYPE	depend;
#ifdef DEBUGOUTPUT
		char			namemage[50];
#endif
};

//extern struct MAGEARRAY *firstmage;
extern struct OBJstruct *allobj[];
extern char	  Minimap[];

//====================
#endif /*_MAN_W*/





