#if !defined(_TBL_W)
#define _TBL_W

#pragma pack(push,1)

//===========================================
#define MAX_IMAGES_ELEM			999
struct IMAGES_STRUCT
{
	int	 images_tbl[MAX_IMAGES_ELEM];					//pointer to images.tbl (grpfilename) or overlay0
	char Graphic_Turns[MAX_IMAGES_ELEM];
	char Clickable[MAX_IMAGES_ELEM];
	char Use_Full_Iscript[MAX_IMAGES_ELEM];
	char Draw_If_Cloaked[MAX_IMAGES_ELEM];
	char Draw_Function[MAX_IMAGES_ELEM];
	char Remapping[MAX_IMAGES_ELEM];
	int	 Iscript_ID[MAX_IMAGES_ELEM];
	int	 Shield_Overlay[MAX_IMAGES_ELEM];				//extraoverlay1
	int	 Attack_Overlay[MAX_IMAGES_ELEM];				//extraoverlay2
	int	 Damage_Overlay[MAX_IMAGES_ELEM];				//extraoverlay3
	int	 Special_Overlay[MAX_IMAGES_ELEM];				//extraoverlay4
	int	 Landing_Dust_Overlay[MAX_IMAGES_ELEM];			//extraoverlay5
	int	 Lift_Off_Overlay[MAX_IMAGES_ELEM];				//extraoverlay6
};

#define MAX_SPRITES_ELEM1		517
#define MAX_SPRITES_ELEM2		387

struct SPRITES_STRUCT
{
	short				images_id[MAX_SPRITES_ELEM1];					//pointer to images.dat
	unsigned char		health_bar[MAX_SPRITES_ELEM2];					//from 130-516 ids
	char				unknown1[MAX_SPRITES_ELEM1];
	char				is_visible[MAX_SPRITES_ELEM1];
	char				sel_circle_image[MAX_SPRITES_ELEM2];					//from 130-516 ids
	char				sel_circle_offset[MAX_SPRITES_ELEM2];					//from 130-516 ids

};
//===========================================
#define MAX_UNITS_ELEM			228
#define MAX_UNITS_ELEM1			96
#define MAX_UNITS_ELEM2			106


#define SPECIAL_ISBUILD					0x00000001
#define SPECIAL_ADDON					0x00000002
#define SPECIAL_FLY						0x00000004
#define SPECIAL_WORKER					0x00000008
#define SPECIAL_SUBUNIT					0x00000010
#define SPECIAL_FLYBUILD				0x00000020
#define SPECIAL_HERO					0x00000040
#define SPECIAL_REGENERATE				0x00000080
#define SPECIAL_ANIMATEDIDLE			0x00000100
#define SPECIAL_CLOAKABLE				0x00000200
#define SPECIAL_TWOINONEEGG				0x00000400
#define SPECIAL_SINGLESELECT			0x00000800
#define SPECIAL_RESOURCEDEPOT			0x00001000
#define SPECIAL_RESOURCECONTAINER		0x00002000
#define SPECIAL_ROBOTICUNIT				0x00004000
#define SPECIAL_DETECTOR				0x00008000
#define SPECIAL_ORGANICUNIT				0x00010000
#define SPECIAL_REQUIRESCREEP			0x00020000
#define SPECIAL_UNUSED					0x00040000
#define SPECIAL_REQUIRESPYLONAREA		0x00080000
#define SPECIAL_BURROWABLE				0x00100000
#define SPECIAL_SPELLCASTER				0x00200000
#define SPECIAL_PERMANENTCLOAK			0x00400000
#define SPECIAL_PICKUPITEM				0x00800000
#define SPECIAL_IGNORESUPPLYCHECK		0x01000000
#define SPECIAL_USEMEDIUMOVERLAY		0x02000000				//eg.stasis field have medium and large sprite only
#define SPECIAL_USELARGEOVERLAY			0x04000000
#define SPECIAL_BATTLEREACTIONS			0x08000000
#define SPECIAL_FULLAUTOATTACK			0x10000000
#define SPECIAL_INVINCIBLE				0x20000000
#define SPECIAL_MECHANICALUNIT			0x40000000
#define SPECIAL_PRODUCESUNITS			0x80000000

#define STAREDIT_GF_ZERGRACE			0x01
#define STAREDIT_GF_TERRANRACE			0x02
#define STAREDIT_GF_PROTOSSRACE			0x04
#define STAREDIT_GF_MEN					0x08
#define STAREDIT_GF_BUILDING			0x10
#define STAREDIT_GF_FACTORY				0x20
#define STAREDIT_GF_INDEPENDENT			0x40
#define STAREDIT_GF_NEUTRAL				0x80
#define STAREDIT_GF_NOANYGROUPFLAG		(STAREDIT_GF_MEN|STAREDIT_GF_BUILDING|STAREDIT_GF_FACTORY|STAREDIT_GF_INDEPENDENT|STAREDIT_GF_NEUTRAL)

#define STAREDIT_AF_NONNEUTRAL			0x0001
#define STAREDIT_AF_LISTINGPALLETE		0x0002
#define STAREDIT_AF_MISSIONBRIEFING		0x0004
#define STAREDIT_AF_PLAYERSETTINGS		0x0008
#define STAREDIT_AF_ALLRACES			0x0010
#define STAREDIT_AF_DOODADSTATE			0x0020
#define STAREDIT_AF_NONLOCATIONTRIGGERS 0x0040
#define STAREDIT_AF_UNITHEROSETTINGS	0x0080
#define STAREDIT_AF_LOCATIONTRIGGERS	0x0100
#define STAREDIT_AF_EXPANSIONUNIT		0x0200

#define UNIT_AIACTION_RIGHTCLICK_NOCMD_AUTOATACK				0
#define UNIT_AIACTION_RIGHTCLICK_NORMALMOVE_NORMALATACK			1
#define UNIT_AIACTION_RIGHTCLICK_NORMALMOVE_NOATACK				2
#define UNIT_AIACTION_RIGHTCLICK_NOMOVE_NORMALATACK				3
#define UNIT_AIACTION_RIGHTCLICK_HARVEST						4
#define UNIT_AIACTION_RIGHTCLICK_HARVEST_REPAIR					5
#define UNIT_AIACTION_RIGHTCLICK_NOTHING						6



struct UNITS_STRUCT
{
		unsigned char flingy_id[MAX_UNITS_ELEM];								//pointer to flingy.dat
		short Subunit1[MAX_UNITS_ELEM];
		short Subunit2[MAX_UNITS_ELEM];
		short Infestation[MAX_UNITS_ELEM1];
		int	  ConstructionAnimation[MAX_UNITS_ELEM];
		char  UnitDirection[MAX_UNITS_ELEM];
		char  ShieldEnable[MAX_UNITS_ELEM];
unsigned short ShieldAmount[MAX_UNITS_ELEM];
unsigned int  HitPoints[MAX_UNITS_ELEM];
unsigned char ElevationLevel[MAX_UNITS_ELEM];
unsigned char Unknown1[MAX_UNITS_ELEM];
		char  Sublabel[MAX_UNITS_ELEM];
		char  CompAIIdle[MAX_UNITS_ELEM];
		char  HumanAIIdle[MAX_UNITS_ELEM];
		char  ReturntoIdle[MAX_UNITS_ELEM];
		char  AttackUnit[MAX_UNITS_ELEM];
		char  AttackMove[MAX_UNITS_ELEM];
unsigned char  GroundWeapon[MAX_UNITS_ELEM];
unsigned char  MaxGroundHits[MAX_UNITS_ELEM];
unsigned char  AirWeapon[MAX_UNITS_ELEM];
unsigned char  MaxAirHits[MAX_UNITS_ELEM];
		char  AIInternal[MAX_UNITS_ELEM];
unsigned int   SpecialAbilityFlags[MAX_UNITS_ELEM];						//advanced properties
		char  TargetAcquisitionRange[MAX_UNITS_ELEM];
		char  SightRange[MAX_UNITS_ELEM];
		char  ArmorUpgrade[MAX_UNITS_ELEM];								//pointer to upgrades.dat
		char  UnitSize[MAX_UNITS_ELEM];									//0,1,2,3 -independ,small,med,large
		char  Armor[MAX_UNITS_ELEM];
		char  RightClickAction[MAX_UNITS_ELEM];
		short ReadySound[MAX_UNITS_ELEM2];
		short WhatSoundStart[MAX_UNITS_ELEM];
		short WhatSoundEnd[MAX_UNITS_ELEM];
		short PissSoundStart[MAX_UNITS_ELEM2];
		short PissSoundEnd[MAX_UNITS_ELEM2];
		short YesSoundStart[MAX_UNITS_ELEM2];
		short YesSoundEnd[MAX_UNITS_ELEM2];
unsigned short StarEditPlacementBoxWidthAndHeight[MAX_UNITS_ELEM][2];
		short AddonHorizontalVertical[MAX_UNITS_ELEM1][2];
		short UnitSizeLeftUpRightDown[MAX_UNITS_ELEM][4];
		short portdata_id[MAX_UNITS_ELEM];								//pointer to portdata.dat
		short MineralCost[MAX_UNITS_ELEM];
		short VespeneCost[MAX_UNITS_ELEM];
		short BuildTime[MAX_UNITS_ELEM];
		short Unknown2[MAX_UNITS_ELEM];
		unsigned char  StarEditGroupFlags[MAX_UNITS_ELEM];
		unsigned char SupplyProvided[MAX_UNITS_ELEM];
		unsigned char SupplyRequired[MAX_UNITS_ELEM];
		signed char SpaceRequired[MAX_UNITS_ELEM];
		unsigned char SpaceProvided[MAX_UNITS_ELEM];
		short BuildScore[MAX_UNITS_ELEM];
		short DestroyScore[MAX_UNITS_ELEM];
		short UnitMapString[MAX_UNITS_ELEM];
		unsigned char  BroodWarUnitFlag[MAX_UNITS_ELEM];
		unsigned short StarEditAvailabilityFlags[MAX_UNITS_ELEM];
};
//===========================================
#define MAX_FLINGY_ELEM			209
struct FLINGY_STRUCT
{
		short sprites_id[MAX_FLINGY_ELEM];				// The sprites.dat entry corresponding to this flingy ID. (pointer to Sprites.dat) 
		int	  TopSpeed[MAX_FLINGY_ELEM];				// Maximum speed at which the sprite will move. Measured in pixels-per-frame, but written as "Speed*(320/3)" (rounded up, it's weird, but that's how it works). Works ONLY if "Move Control" is set to "Flingy.dat Control". 
		short Acceleration[MAX_FLINGY_ELEM];			// How fast the sprite speeds up or slows down. Added to or subtracted from current speed until it reaches the Top Speed or 0. Measured in pixels-per-frame. Works ONLY if "Move Control" is set to "Flingy.dat Control". 
		int	  HaltDistance[MAX_FLINGY_ELEM];			// Distance from its destination at which the sprite will begin to deccelerate from its Top Speed to a complete halt. Measured in pixels*256. 
		char  TurnRadius[MAX_FLINGY_ELEM];				// The distance the sprite requires to "wipe around" to turn to another direction. Works ONLY if "Move Control" is set to "Flingy.dat Control". 
		char  Unused[MAX_FLINGY_ELEM];					//		Unused. 
		char  MoveControl[MAX_FLINGY_ELEM];				//Indicates the mechanism that is used to control the movement of the flingy.dat entry. "Flingy.dat Control" makes use of the Acceleration, Speed, Turn Style and Turn Radius properties, i.e. the values in this editor will be used. "Iscript.bin Control" ignores these properties and follows only the Iscript opcode sequence. "Partially Mobile/Weapon" is used for various weapons sprites, not completely understood.
};
//===========================================
#define MAX_PORTDATA_ELEM		220
struct PORTDATA_STRUCT
{
	int	 portdata_tbl[MAX_PORTDATA_ELEM];										//pointer to portdata.tbl
	char smk_change[MAX_PORTDATA_ELEM];
	char unknown1[MAX_PORTDATA_ELEM];
};
//===========================================
#define MAX_STARWEAPONS_ELEM	100
#define MAX_BROODWEAPONS_ELEM	130
#define MAX_WEAPONS_ELEM		MAX_BROODWEAPONS_ELEM

#define			WTF_AIR									0x001
#define			WTF_GROUND								0x002
#define			WTF_MECHANICAL							0x004
#define			WTF_ORGANIC								0x008
#define			WTF_NONBUILD							0x010
#define			WTF_NONROBOTIC							0x020
#define			WTF_ONTERRAIN							0x040
#define			WTF_MECHANICALORGANIC					0x080
#define			WTF_OWN									0x100
#define			WTF_STARCLONE_ONSPELLCASTER				0x200


#define			WEFFECT_NONE					0
#define			WEFFECT_NORMALHIT				1
#define			WEFFECT_SPLASHALL				2
#define			WEFFECT_SPLASHENEMY				3
#define			WEFFECT_UNIQUE							4
#define			WEFFECT_LOCKDOWN				4
#define			WEFFECT_NUCLEARMISSILE			5
#define			WEFFECT_PARASITE				6
#define			WEFFECT_BROODLINGS				7
#define			WEFFECT_EMPSHOCKWAVE			8
#define			WEFFECT_IRRADIATE				9
#define			WEFFECT_ENSNARE					10
#define			WEFFECT_PLAGUE					11
#define			WEFFECT_STASISFIELD				12
#define			WEFFECT_DARKSWARM				13
#define			WEFFECT_CONSUME					14
#define			WEFFECT_YAMATOGUN				15
#define			WEFFECT_RESTORATION				16
#define			WEFFECT_DISRUPTIONWEB			17
#define			WEFFECT_CORROSIVEACID			18
#define			WEFFECT_MINDCONTROL				19
#define			WEFFECT_FEEDBACK				20
#define			WEFFECT_OPTICALFLARE			21
#define			WEFFECT_MAELSTROM				22
#define			WEFFECT_UNKNOWN					23
#define			WEFFECT_SPLASHENEMYAIR			24


struct WEAPONS_STRUCT
{
	unsigned short Label[MAX_WEAPONS_ELEM];								//Label The name of the weapon, displayed when you highlight its icon in the control bar (pointer to stat_txt.tbl) 
	unsigned int   flingy_id[MAX_WEAPONS_ELEM];							//Graphics The main graphic that the weapon uses (pointer to flingy.dat}, but 0 = No Image) 
	unsigned char  Technology_id[MAX_WEAPONS_ELEM];						//(depend on researched technology)This property does NOTHING. It was thought to be related to what technology is used together with the weapon, but it turned out to be just a designer's hint. 
	unsigned short TargetFlags[MAX_WEAPONS_ELEM];						// Flags for which type of units are acceptable targets:

																//0x001 = Weapon attacks air targets, i.e. units with the "Flying Target" flag set.
																//0x002 = Weapon attacks ground targets, i.e. units without the "Flying Target" flag set.
																//0x004 = Weapon attacks mechanical targets, i.e. units with the "Mechanical" flag set. (e.g. Lockdown)
																//0x008 = Weapon attacks organic targets, i.e. units with the "Organic" flag set. (e.g. Irradiate)
																//0x010 = Weapon attacks non-building targets, i.e. units without the "Building" flag set. (e.g. Parasite)
																//0x020 = Weapon attacks non-Robotic targets, i.e. units without the "Robotic" flag set. (e.g. Spawn Broodlings)
																//0x040 = Weapon attacks terrain, i.e. it does not require a unit-type target (e.g. Plague).
																//0x080 = Weapon attacks mechanical or organic targets, i.e. units with either the "Organic" or "Mechanical" flag set. (e.g. Consume).
																//0x100 = Weapon attacks only player's own units (e.g. Consume). 
	unsigned int   MinimumRange[MAX_WEAPONS_ELEM];						//Minimal range from which the weapon can be used. 
	unsigned int   MaximumRange[MAX_WEAPONS_ELEM];						//Maximum range from which the weapon can be used. 
	unsigned char  DamageUpgrade[MAX_WEAPONS_ELEM];						//The upgrade that will increase the damage dealt by the weapon by the "Bonus" value.
																//(pointer to upgrades.dat) 

	unsigned char  WeaponType[MAX_WEAPONS_ELEM];								// The type of damage the weapon does. Normal, Explosive and Concussive do different amount of damage to units of different Size (Small, Medium or Large): Normal does equal damage to Small, Medium and Large. Explosive does 50% to Small and 75% to Medium. Concussive does 50% to Medium and 25% to Large. Independent deals 1 point of damage every second attack, regardless of target's armor. 
	unsigned char  WeaponBehavior[MAX_WEAPONS_ELEM];					// Determines how the weapon sprite will "behave" when it attacks the target. Weapon behaviours that "Follow" will track the target as it moves, those that "Don't Follow" will strike the place where the target was at the moment of attack. 
	unsigned char  RemoveAfter[MAX_WEAPONS_ELEM];						// Time until the weapon is removed if it does not hit a target. 1 game second equals: on Fastest-24, on Faster-21, on Fast-18, on Normal-15, on Slow-12, on Slower-9 and on Slowest-6. 
	unsigned char  WeaponEffect[MAX_WEAPONS_ELEM];						// Effect the weapon has on the area around the target after hitting its target. Used to determine different type of spell effects and splash damage. 
	unsigned short InnerSplash[MAX_WEAPONS_ELEM];						// Radius Distance from the target at which the weapon will deal 100% of its base damage. Works ONLY if the "Explosion" is set to "Nuclear Missile", "Splash (Radial)", "Splash (Enemy)" or "Splash (Air)". 
	unsigned short MediumSplash[MAX_WEAPONS_ELEM];						// Radius Distance from the target at which the weapon will deal 50% of its base damage. Works ONLY if the "Explosion" is set to "Nuclear Missile", "Splash (Radial)", "Splash (Enemy)" or "Splash (Air)". 
	unsigned short OuterSplash[MAX_WEAPONS_ELEM];						// Radius Distance from the target at which the weapon will deal 25% of its base damage. Works ONLY if the "Explosion" is set to "Nuclear Missile", "Splash (Radial)", "Splash (Enemy)" or "Splash (Air)". 
	unsigned short DamageAmount[MAX_WEAPONS_ELEM];						//The basic amount of damage the weapon will inflict when it hits. 
	unsigned short DamageBonus[MAX_WEAPONS_ELEM];						//The amount of damage added to the basic value for each level of the upgrade. 
	unsigned char  WeaponCooldown[MAX_WEAPONS_ELEM];					//"Reload time" - time delay between two attacks. Depends on the game speed used. 1 game second equals: on Fastest-24, on Faster-21, on Fast-18, on Normal-15, on Slow-12, on Slower-9 and on Slowest-6. Value of 0 will crash the game. 
	unsigned char  DamageFactor[MAX_WEAPONS_ELEM];						//Usually, multiply this value by the Damage Amount to get the total damage that is DISPLAYED for the weapon. To a degree also the number of weapons used per attack, but anything other than 2 will result in 1 weapon being used. (e.g. Goliath, Scout and Valkyrie use 2 missiles per attack). 
	unsigned char  AttackAngle[MAX_WEAPONS_ELEM];						//Angle within which the weapon can be fired without waiting for the unit's graphics to turn. 128 = 180 degrees. 
	unsigned char  LaunchSpin[MAX_WEAPONS_ELEM];								//Angle by which the weapon's sprite will spin after it is spawned. 128 = 180 degrees. 
	unsigned char  ForwardOffset[MAX_WEAPONS_ELEM];						//Distance (in pixels) from the front of the attacking unit (depending on which direction it is facing), at which the weapon's sprite will be spawned. 
	unsigned char  UpwardOffset[MAX_WEAPONS_ELEM];						//Distance (in pixels) from the top of the attacking unit, at which the weapon's sprite will be spawned. 
	unsigned short TargetErrorMessage[MAX_WEAPONS_ELEM];						//The line displayed when the weapon is to acquire an invalid target (e.g. attacking a Mutalisk with a ground-only weapon, like Flamethrower). (pointer to stat_txt.tbl) 
	unsigned short Icon[MAX_WEAPONS_ELEM];								//The icon used for the weapon. (pointer to a frame in unit\cmdbtns\cmdicons.grp)
};

//==========================================
#define MAX_STARUPGRADES_ELEM	46
#define MAX_BROODUPGRADES_ELEM	61
#define MAX_UPGRADES_ELEM		MAX_BROODUPGRADES_ELEM
struct UPGRADES_STRUCT
{
	unsigned short BaseMineralCost[MAX_UPGRADES_ELEM];
	unsigned short FactorMineralCost[MAX_UPGRADES_ELEM];
	unsigned short BaseVespeneCost[MAX_UPGRADES_ELEM]; 
	unsigned short FactorVespeneCost[MAX_UPGRADES_ELEM];
	unsigned short BaseResearchTime[MAX_UPGRADES_ELEM];
	unsigned short FactorResearchTime[MAX_UPGRADES_ELEM];
	unsigned short Unknown[MAX_UPGRADES_ELEM];
	unsigned short Icon[MAX_UPGRADES_ELEM];
	unsigned short Label[MAX_UPGRADES_ELEM];
	unsigned char  Race[MAX_UPGRADES_ELEM];
	unsigned char  MaxRepeats[MAX_UPGRADES_ELEM];
	unsigned char  BroodWar[MAX_UPGRADES_ELEM];
};
//==========================================
#define MAX_STARTECHDATA_ELEM	24
#define MAX_BROODTECHDATA_ELEM	44
#define MAX_TECHDATA_ELEM		MAX_BROODTECHDATA_ELEM
struct TECHDATA_STRUCT
{
	unsigned short MineralCost[MAX_TECHDATA_ELEM];
	unsigned short VespeneCost[MAX_TECHDATA_ELEM];
	unsigned short ResearchTime[MAX_TECHDATA_ELEM];
	unsigned short EnergyCost[MAX_TECHDATA_ELEM];
	unsigned int   Unknown[MAX_TECHDATA_ELEM];
	unsigned short Icon[MAX_TECHDATA_ELEM];
	unsigned short Label[MAX_TECHDATA_ELEM];
	unsigned char  Race[MAX_TECHDATA_ELEM];
	unsigned char  Unused[MAX_TECHDATA_ELEM];
	unsigned char  BroodWar[MAX_TECHDATA_ELEM];
};
//==========================================
#define MAX_ORDERS_ELEM			189
struct ORDERS_STRUCT
{
		unsigned short	stattxt_id[MAX_ORDERS_ELEM];
		unsigned char	UseWeaponTargeting[MAX_ORDERS_ELEM];
		unsigned char	Unused0[MAX_ORDERS_ELEM];
		unsigned char	Unused1[MAX_ORDERS_ELEM];
		unsigned char	Unknown0[MAX_ORDERS_ELEM];
		unsigned char	Unused2[MAX_ORDERS_ELEM];
		unsigned char	CanBeInterrupted[MAX_ORDERS_ELEM];
		unsigned char	Unknown1[MAX_ORDERS_ELEM];
		unsigned char	CanBeQueued1[MAX_ORDERS_ELEM];
		unsigned char	Unknown2[MAX_ORDERS_ELEM];
		unsigned char	CanBeObstructed[MAX_ORDERS_ELEM];
		unsigned char	Unknown3[MAX_ORDERS_ELEM];
		unsigned char	Unused3[MAX_ORDERS_ELEM];
		unsigned char	weapon_id[MAX_ORDERS_ELEM];
		unsigned char	techdata_id[MAX_ORDERS_ELEM];
		unsigned char	Animation[MAX_ORDERS_ELEM];
		short			Highlight[MAX_ORDERS_ELEM];
		short			Unknown[MAX_ORDERS_ELEM];
		unsigned char	orders_id[MAX_ORDERS_ELEM];
};
//==========================================
#define MAX_SFXDATA_ELEM		1144
struct SFXDATA_STRUCT
{
		int				sfxdata_tbl[MAX_SFXDATA_ELEM];
		char			Unknown1[MAX_SFXDATA_ELEM];
		char			Unknown2[MAX_SFXDATA_ELEM];
		short			Unknown3[MAX_SFXDATA_ELEM];
		char			Unknown4[MAX_SFXDATA_ELEM];
};
//==========================================
struct TBL_FILE
{
	unsigned short strnr;
	unsigned short offsetfrombegin[];
};

#pragma pack(pop)

class TBL
{
				TBL_FILE *tbl;
public:
	int			loadTBL(const char *filenr);
	int			get_TBL_STRINGS(int stringnr);
	char		*get_TBL_STR(int stringnr);
	char		*get_TBL_SUBSTR(int stringnr,int substrnr);
	int			get_TBL_STRSize(int stringnr);
	int			get_STRS(void);
	void		log_STRS(void);
	
				~TBL();
};

int checkfordublicate(char *textdata,int sizex,char *textcmp);
void	saveTBL(int strnr,const char *filenr,char *arrayofstrs[]);

//==========================================
struct DATTBLSTRUCT
{
	IMAGES_STRUCT		*images_dat;
	SPRITES_STRUCT		*sprites_dat;
	UNITS_STRUCT		*units_dat;
	FLINGY_STRUCT		*flingy_dat;
	PORTDATA_STRUCT		*portdata_dat;
	WEAPONS_STRUCT		*weapons_dat;
	UPGRADES_STRUCT		*upgrades_dat;
	TECHDATA_STRUCT		*techdata_dat;
	ORDERS_STRUCT		*orders_dat;
	SFXDATA_STRUCT		*sfxdata_dat;
	SFXDATA_STRUCT		*mapdata_dat;
	TBL					*images_tbl;
	TBL					*portdata_tbl;
	TBL					*sfxdata_tbl;
	TBL					*stattxt_tbl;
	TBL					*gluall_tbl;
	TBL					*network_tbl;
	TBL					*mapdata_tbl;
	TBL					*myinfo_tbl;
	TBL					*campaign_tbl;
	TBL					*mycheats_tbl;
};
//==========================================


#endif

