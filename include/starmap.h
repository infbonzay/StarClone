#ifndef _STARMAP_W
#define _STARMAP_W

#include "defs.h"
#include "defsmodes.h"
#include "defsunits.h"
#include "vars.h"
#include "man.h"
#include "const.h"
#include "mylist.h"
#include "triggers.h"
#include "tbl.h"
#include "gamemenu.h"

#define MAXTERRAINTYPES			8
#define MAXLEADERBOARDS			2

#define STARMAP_LOADFORPLAY		0x00
#define STARMAP_LOADFORINFO		0x01

#define STARMAP_FLAG_EXPANSION		0x01
#define STARMAP_FLAG_WINGAME		0x02
#define STARMAP_FLAG_HAVECOUNTDOWN	0x04
#define STARMAP_FLAG_COUNTSTOPED	0x08
#define STARMAP_FLAG_AUTOMOVE		0x10
#define STARMAP_FLAG_MAPMOVE		0x20


#define DSWARMEFFECT		0
#define DISRUPTEFFECT		1
#define ALLMAPEFFECTS		2


#define MINIMAPW 128

#define	OWNER_NONE		0
#define	OWNER_NONE1 		1
#define	OWNER_NONE2 		2
#define	OWNER_RESCUABLE 	3
#define	OWNER_NONE3 		4
#define	OWNER_FORCENAME 	4
#define	OWNER_COMPUTER		5
#define	OWNER_HUMAN 		6
#define	OWNER_NEUTRAL 		7

#define LOC_DEFINED_AS_ZERG		ZERGRACE
#define LOC_DEFINED_AS_TERRAN		TERRANRACE
#define LOC_DEFINED_AS_PROTOSS		PROTOSSRACE
#define LOC_DEFINED_AS_INDEPENDENT	3
#define LOC_DEFINED_AS_NEUTRAL		4
#define LOC_DEFINED_AS_USERSELECT	5
#define LOC_DEFINED_AS_NONE1		6
#define LOC_DEFINED_AS_INACTIVE		7
#define LOC_DEFINED_AS_NONE2		8
#define LOC_DEFINED_AS_NONE3		9
#define LOC_DEFINED_AS_HUMAN		10

#define walkflags32 unsigned short


struct terrain_cv5
{
	unsigned short	unknown1;
	unsigned short	flags;
			//0x0001-egde
			//0x0001-egde
			//0x0004-clif
			//0x0040-creep
			//0x0080-unbuildable
			//0x0X00-Depricated ground height
			//0x8000-Buildable for Start Location and Beacons
	unsigned short	LeftEgde;
	unsigned short	TopEdge;
	unsigned short	RightEdge;
	unsigned short	BottomEdge;
	unsigned short	unknown2;
	unsigned short	EdgeUp;
	unsigned short	unknown3;
	unsigned short	EdgeDown;
//	unsigned short	MegaTileRef[16];
};
struct terrain_cv5_doodad
{
	unsigned short	index;
	unsigned short	flags;
			//0x0001-egde
			//0x0001-egde
			//0x0004-clif
			//0x0040-creep
			//0x0080-unbuildable
			//0x1000 = Has Sprites.dat Overlay 
			//0x2000 = Has Units.dat Overlay 
			//0x4000 = Overlay is flipped horizontally
	unsigned short	OverlayID;
	unsigned short	unknown1;
	unsigned short	Doodad_Group;//refer to stat_txt.tbl
	unsigned short	unknown2;
	unsigned short	dddataindex;
	unsigned short	Width;
	unsigned short	Height;
	unsigned short	unknown3;
//	unsigned short	MegaTileRef[16];
};

struct doodad_on_map
{
	int		xpos;
	int 		ypos;
	unsigned short  sprites_id;
	unsigned short 	flags;
	unsigned char  	playernr;
};

/* This structure represents placed units on the map */
#define UNITONMAP_OWNERISVALID			0x01
#define UNITONMAP_HPISVALID			0x02
#define UNITONMAP_SPISVALID			0x04
#define UNITONMAP_EPISVALID			0x08

#define UNITONMAP_STATEFLAGS_CLOAK		0x0001
#define UNITONMAP_STATEFLAGS_BURROW		0x0002
#define UNITONMAP_STATEFLAGS_BUILDINTRANSIT	0x0004
#define UNITONMAP_STATEFLAGS_HALLUCINATION	0x0008
#define UNITONMAP_STATEFLAGS_INVINCIBLE		0x0010

#define NOPARIED				0
#define TERRANPARIED				2
#define ZERGPARIED				4

#pragma pack(push,1)

struct doodad_DD2_starmap
{
	unsigned short sprites_id;
	unsigned short xpos;
	unsigned short ypos;
	unsigned char  playernr;
	unsigned char  state;
};
struct doodad_THG2_starmap
{
	unsigned short sprites_id;
	unsigned short xpos;
	unsigned short ypos;
	unsigned char  playernr;
	unsigned char  sprpos;
	unsigned short flags;
};

struct triggerunit
{
	short int special_props; 	/* Special properties that can
						   be applied to unit:
						   bit 0: cloak can be applied;
						   bit 1: burrowed can be applied;
						   bit 2: in transit can be applied;
						   bit 3: hallucinated can be applied;
						   bit 4: invincible can be applied;
						   bits 5-15: unknown. */
	short int special_prop2;	/*  bit 0 - owner player is valid(unit is not neutral)
					    bit 1 - HP is valid
					    bit 2 - SP is valid
					    bit 3 - energy is valid
					    bit 4 - resource amount is valid(unit is a resource)
					    bit 5 - amount in hanger is valid
					    bit 6-15 - unknown/unused*/
	
	char player;		/* 0-based, player who owns the unit */
	char hitpoints;		/* percent, 0-100 */
	char shieldpoints;	/* percent, 0-100 */
	char energypoints;	/* percent, 0-100 */
	int  resource_count;/* resource amount, for resources only */
	short int units_in_hangar;/* No. of units in hangar */
	short int state_flags; /* unit state flags:
						   bit 0: unit is cloaked;
						   bit 1: unit is burrowed;
						   bit 2: building is in transitl
						   bit 3: unit is hallucinated
						   bit 4: unit is invincible
						   bit 5-15: unknown */
	char unknown2[4];
};


struct unit_on_map
{
	int unit_serialnumberfirst;
	short int xpos;
	short int ypos;
	short int unit_type;
	char  unknown1;
	char  paried;			/*  0 - Unit/building is not paired
					    2 - Paired Terran building/addon
					    4 - Paired Zerg buildings(Nydus Canals)*/
	short int special_props; 	/* Special properties that can
						   be applied to unit:
						   bit 0: cloak can be applied;
						   bit 1: burrowed can be applied;
						   bit 2: in transit can be applied;
						   bit 3: hallucinated can be applied;
						   bit 4: invincible can be applied;
						   bits 5-15: unknown. */
	short int special_prop2;	/*  bit 0 - owner player is valid(unit is not neutral)
					    bit 1 - HP is valid
					    bit 2 - SP is valid
					    bit 3 - energy is valid
					    bit 4 - resource amount is valid(unit is a resource)
					    bit 5 - amount in hanger is valid
					    bit 6-15 - unknown/unused*/
	
	char player;		/* 0-based, player who owns the unit */
	char hitpoints;		/* percent, 0-100 */
	char shieldpoints;	/* percent, 0-100 */
	char energypoints;	/* percent, 0-100 */
	int  resource_count;/* resource amount, for resources only */
	short int units_in_hangar;/* No. of units in hangar */
	short int state_flags; /* unit state flags:
						   bit 0: unit is cloaked;
						   bit 1: unit is burrowed;
						   bit 2: building is in transitl
						   bit 3: unit is hallucinated
						   bit 4: unit is invincible
						   bit 5-15: unknown */
	char unknown2[4];
	int unit_serialnumbersecond;
};

/* Restrictions on production of units. 228 units, 12 players.
 * 0 means forbidden, 1 means allowed. 
 * Use defaults is 1 when global value is taken. */
struct PRODUCTION_RESTRICTIONS
{
	char player_restrictions[MAXPLAYERS][MAX_UNITS_ELEM];
	char global_restrictions[MAX_UNITS_ELEM];
	char use_defaults[MAXPLAYERS][MAX_UNITS_ELEM];
};

/* Restrictions on production of units. 45/61 upgrades, 12 players.
 * Use defaults is 1 when global value is taken. */
struct UPGRADE_RESTRICTIONS
{
	char max_upgrade[MAXPLAYERS][MAX_UPGRADES_ELEM];
	char start_upgrade[MAXPLAYERS][MAX_UPGRADES_ELEM];
	char currentmake[MAXPLAYERS][MAX_UPGRADES_ELEM];
};

/* Restrictions on technology. 24/44 technologies, 12 players.
 * Use defaults is 1 when global value is taken. */
struct TECHNOLOGY_RESTRICTIONS
{
	char tech_available[MAXPLAYERS][MAX_TECHDATA_ELEM];
	char tech_researched[MAXPLAYERS][MAX_TECHDATA_ELEM];
	char currentmake[MAXPLAYERS][MAX_TECHDATA_ELEM];
};

struct UNIT_SETTINGS_LIFEPARAMS
{
	char  unknown1;
	short hitpoints;
	char  selectable;
};
struct UNIT_SETTINGS
{
	char	use_defaults[MAX_UNITS_ELEM];
	UNIT_SETTINGS_LIFEPARAMS unitlife[MAX_UNITS_ELEM];
	short	shieldpoints[MAX_UNITS_ELEM];
	char	armorpoints[MAX_UNITS_ELEM];
	short	buildtime[MAX_UNITS_ELEM];	/* in 1s/60 values */
	short	mineralcost[MAX_UNITS_ELEM];
	short	gascost[MAX_UNITS_ELEM];
	short	unitname[MAX_UNITS_ELEM];	/* index into strings */
	short	weaponbasedamage[MAX_WEAPONS_ELEM];
	short	weaponbonusdamage[MAX_WEAPONS_ELEM];		/* bonus damage for each upgrade level */
};

struct UPGRADE_SETTINGS
{
	char  use_defaults[MAX_UPGRADES_ELEM];
	char  unknown;
	short basecostminerals[MAX_UPGRADES_ELEM];
	short upgradecostminerals[MAX_UPGRADES_ELEM];	/* cost factor for next upgrade level */
	short basecostgas[MAX_UPGRADES_ELEM];
	short upgradecostgas[MAX_UPGRADES_ELEM];		/* cost factor for next upgrade level */
	short basecosttime[MAX_UPGRADES_ELEM];
	short upgradecosttime[MAX_UPGRADES_ELEM];	/* cost factor for next upgrade level */
};

struct TECHNOLOGY_SETTINGS
{
	char  use_defaults[MAX_TECHDATA_ELEM];
	short costminerals[MAX_TECHDATA_ELEM];
	short costgas[MAX_TECHDATA_ELEM];
	short researchtime[MAX_TECHDATA_ELEM];
	short energycost[MAX_TECHDATA_ELEM];	/* cost to cas special ability */
};

#pragma pack(pop)

//=====================================================
#define UNDERFOG_MIRROR		0x01
#define UNDERFOG_MAINIMAGE	0x02
//=====================================================
struct UNITFOGSTRUCT
{
	    unsigned short 	nrpicture;
	    unsigned char  	flags;
	    unsigned char  	colortable;
	    unsigned char  	format;
	    unsigned char  	maxcolor;
	    unsigned char 	grcolor;
	    GRPFILE   		*grp;
	    unsigned short 	x;
	    unsigned short 	y;
};
//=====================================================
#define FOGMAXPICTURESLOTS	6
    struct UNITUNDERFOG
    {
	unsigned char	nrofslots;
	UNITFOGSTRUCT pictureslot[FOGMAXPICTURESLOTS];
    };
//=====================================================
struct MAPCELLS
{
    unsigned char		*seedetector;
    unsigned char		*arbiterfield;

    unsigned char		*seedetector2;
    unsigned char		*arbiterfield2;

    unsigned char		*mageeffect[ALLMAPEFFECTS];			//for darkswarm,disruption
    
    unsigned char		*whitefog[PLAYEDPLAYERS];
    unsigned char		*blackfog[PLAYEDPLAYERS];
    unsigned char		*whitefog2[PLAYEDPLAYERS];
    unsigned char		*savedcreep[PLAYEDPLAYERS];
    unsigned short int		*savedtile[PLAYEDPLAYERS];
    UNITUNDERFOG		**underfog[PLAYEDPLAYERS];
    
};
//=====================================================
struct MAPStrings
{
    short int nrofstrings;
    short int offset[];
};
//=====================================================
#pragma pack(push,1)

#define MAX_PLFORCES			4
#define FORCEFLAG_RANDOMSTARTLOCATION	0x01
#define FORCEFLAG_ALLIES		0x02
#define FORCEFLAG_ALLIESVICTORY		0x04
#define FORCEFLAG_SHAREDVISION		0x08

struct UNITS_STAT
{
	unsigned short  stattype[3][MAXPLAYERS][SC_MAXUNITIDS];		//died,producing,have
	int		score[MAXPLAYERS][TRG_MAXSCORETYPES];
	//scores
};

struct PL_FORCE
{
    unsigned char	force_nr[PLAYEDPLAYERS];
    short int		force_ID[MAX_PLFORCES];
    unsigned char 	force_flag[MAX_PLFORCES];
		        //bit 0 - random start location
			//bit 1 - allies
			//bit 2 - allied victory
			//bit 3 - shared vision
			//bit 4-7 - unknown/unusedbit 0 - random start location
};

struct locations
{
    struct XY coords;
/*    int startx;
    int starty;
    int endx;
    int endy;
*/
    short int stringID;
    short int elevationflag;
};
#pragma pack(pop)
//=====================================================
struct mapinfo
{
    unsigned int secondsplayed;
    char 	played_as_nr;
//    char	scenario_type[5];
    unsigned short format_version[3];
    char 	valid_vcode;
    int		tempscore[MAXPLAYERS][3];		

    int		score[MAXPLAYERS][12];
    int		maxscore[12];
    		    //{"Units","Structures","Resources"},
		    //{"Trained","Killed","Dies"},
		    //{"Builded","Destroyed","Losted"},
		    //{"Gas","Minerals","Used"}
    struct	triggerunit *triggerunitsprop;
    struct	locations *gamelocations;
    short int	maxlocations;
    struct LEADERBOARD *leaderboards[MAXLEADERBOARDS];
    int 	CountDownTimer;
    char	Switch[256];
    
    char	pl_allied[MAXPLAYERS][MAXPLAYERS];
    char	pl_vision[MAXPLAYERS][MAXPLAYERS];
    char	pl_visionbits[MAXPLAYERS];
    PL_FORCE 	pl_force;
    char 	pl_race[MAXPLAYERS];
    char 	pl_owner[MAXPLAYERS];
    char 	pl_iowner[MAXPLAYERS];
    signed char	pl_location[MAXPLAYERS];
    signed char	pl_startlocs;
    signed char	pl_startlocplayernr[MAXPLAYERS];
    
    UNITS_STAT	units_stat;
    char	*STR;		//all strings from map
    short      	TRIGS_NR;
    MAP_TRIGS	*TRIGS;
    short      	BRIEFS_NR;
    MAP_TRIGS	*BRIEFS;
    terrain_type terrain;
    int 	map_width;
    int		map_height;
    HANDLE	mpqid;
    char	mpqclose;

    struct 	PRODUCTION_RESTRICTIONS prod_restrictions;
    struct 	UPGRADE_RESTRICTIONS 	upgr_restrictions;
    struct 	TECHNOLOGY_RESTRICTIONS tech_restrictions;

    struct 	UNIT_SETTINGS 		unit_settings;
    struct	UPGRADE_SETTINGS 	upgr_settings;
    struct 	TECHNOLOGY_SETTINGS	tech_settings;

    struct 	unit_on_map *units_on_map;
    int 	unit_count;

//    char 	player_colors[8]; /* more than 1 player may have the same color */

    unsigned short 	*display_map;				//index of tiles, on a 32x32 grid
    unsigned short 	*level_tiles;				// LEVEL of tile in special format
    char 	*tiles;					// actual graphics of tiles 
    
    char	*creeptiles;				//creep tiles
    int 	creeptilecnt;				//creep tiles count
    int		*creeptileminimapcolors;		//2*2 bytes in tile32(2x2) for creep vision on minimap
    
//    int		*creeptileonminimap;			//4 bytes of color for creep on minimap 
							//0 1  position
							//2 3  bytes in this int
    
    unsigned short	*tileswalk;				//passable tiles
    char 	*maplevel;				//evevation value;
    char 	*mapwalk;				//online passable or not 8x8 grid

    unsigned char		*FogMask;
    
    unsigned char		*constrmap;		//passable for construct basebuild
    unsigned char		*buildswalk;		//passable for builds
    unsigned char		*creepflagneeded;	//if here need to be creep
    unsigned char		*creepflagplace;	//0/X - creep existance 0-no/X-yes
    unsigned char		*pylonarea[MAXPLAYERS];		//pylonarea for every player
    
    MAPCELLS			mapbits;

    unsigned short		*vf4mem;			//all used tile32 properites
    
    int		totaldeltax;
    int		totaldeltay;
    int		deltax;
    int 	deltay;
    int		MAPX;
    int		MAPY;
    int		newx;
    int		newy;
    int		MAPXGLOBAL;
    int		MAPYGLOBAL;
    char	clearfog[PLAYEDPLAYERS];		//set if we need to change white&blackfog in calfog
    char	screenfogchange;			//set if fog change on screen (I move kart some object move) and I see that
    int 	tilecount;
    int		flags;
							//bit 0 - expansion scenario (broodwar)
							//bit 1 - win scenario
    							//bit 2 - countdown exist
    							//bit 3 - countdown stoped 
    							//bit 4 - bit indicates to automove map (ignore move from player)
    							//bit 5 - map moved so need to recalc fog

    char	minimap[MINIMAPW*MINIMAPW];
    int  	minimap_startx, minimap_starty;
    int  	minimap_endx, minimap_endy;

    char 	palette[256*4];
//    char 	origpalette[256*4];
    char	palettechanges;
};

#define UNITSTAT_DEATH		0
#define UNITSTAT_PRODUCING	1
#define UNITSTAT_HAVE		2

void add_unit_stat(struct mapinfo *info,int stattype,int playernr,SCUNIT SC_Unit);
void sub_unit_stat(struct mapinfo *info,int stattype,int playernr,SCUNIT SC_Unit);

/* reads map from unpacked chk */
int  read_starmap( const char *fname, struct mapinfo * ,int flags);
/* unpacks and reads map */
int  load_starmap( const char *mapfile,char *fname, struct mapinfo * ,GAMECONF *conf);
void print_mapinfo( struct mapinfo *info );
void unload_starmap(struct mapinfo *info );
void unload_starmapallocated(struct mapinfo *info );
int  starmap_info( char *path,const char *fname, mapinfo *info);
char createcolors8x8(char *bytes32x32,int pos);
int *createminimapcolorsfrom(char *tiles,int tilecnt);
void CreateDefaultAliance(struct mapinfo *info);
void CreateAliance(struct mapinfo *mapinfo,int player1,int player2,int flag);
void CreateVision(struct mapinfo *mapinfo,int player1,int player2,int flag);
void CreateDefaultVision(struct mapinfo *mapinfo);
void starmap_forceslots( mapinfo *info,FORCE_SLOTS *fslots,int gametype);
int  GetForceFirstEmptySlot(FORCE_SLOTS *fslots);
int  GetPlayersInForceNr(mapinfo *info,int forcenr);
int  GetPlayedPlayers(void);
void observers_prepare(mapinfo *info);
void randomizelocations(struct mapinfo *loadedmap);
void randomizerace(struct mapinfo *loadedmap,GAMECONF *conf);
void randomizecolor(void);

int  GetPlayableGameNumber(mapinfo *info);
void free_missionobjectives(void);
void fill_missionobjectives(char *missiontxt);
LEADERBOARD *AddLeaderBoard(int leaderboardnr,int leaderboardtype,struct mapinfo *info,\
			    int actiononplayer,int unittype,int nrofunits,int stringID);
void CalcLeaderBoards(struct mapinfo *info);
void ShowLeaderBoards(struct mapinfo *info,int x,int y);
void ShowCountDownTimer(struct mapinfo *info,int x,int y);
char *getmapSTR(struct mapinfo *testmap,int nrofstr);


extern mylist map_units,map_doodads;
extern int creepfulltilenrs[8][2];
extern char terraintypes[8][10]; 


#endif /*STARMAP_H*/

