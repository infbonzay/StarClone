#ifndef _TRIGGERS_W
#define _TRIGGERS_W

//=====================================================
#pragma pack(push,1)

/*
condition lists
0 = No Conditions
 1 = Countdown Timer
 2 = Command
 3 = Bring
 4 = Accumulate
 5 = Kill
 6 = Command the Most
 7 = Commands the Most At
 8 = Most Kills
 9 = Highest Score
 10 = Most Resources
 11 = Switch
 12 = Elapsed Time
 13 = Unused
 14 = Opponents
 15 = Deaths
 16 = Command the Least
 17 = Command the Least At
 18 = Least Kills
 19 = Lowest Score
 20 = Least Resources
 21 = Score
 22 = Always
 23 = Never
 24 - 255 = Unknown/Unused

action lists

0 = No Actions
 1 = Victory
 2 = Defeat
 3 = Preserve Trigger
 4 = Wait
 5 = Pause Game
 6 = Unpause Game
 7 = Transmission
 8 = Play WAV
 9 = Display Text Message
 10 = Center View
 11 = Create Unit with Properties
 12 = Set Mission Objectives
 13 = Set Switch
 14 = Set Countdown Timer
 15 = Run AI Script
 16 = Run AI Script At Location
 17 = Leader Board (Control)
 18 = Leader Board (Control At Location)
 19 = Leader Board (Resources)
 20 = Leader Board (Kills)
 21 = Leader Board (Points)
 22 = Kill Unit
 23 = Kill Unit At Location
 24 = Remove Unit
 25 = Remove Unit At Location
 26 = Set Resources
 27 = Set Score
 28 = Minimap Ping
 29 = Talking Portrait
 30 = Mute Unit Speech
 31 = Unmute Unit Speech
 32 = Leaderboard Computer Players
 33 = Leaderboard Goal (Control)
 34 = Leaderboard Goal (Control At Location)
 35 = Leaderboard Goal (Resources)
 36 = Leaderboard Goal (Kills)
 37 = Leaderboard Goal (Points)
 38 = Move Location
 39 = Move Unit
 40 = Leaderboard (Greed)
 41 = Set Next Scenario
 42 = Set Doodad State
 43 = Set Invincibility
 44 = Create Unit
 45 - 255 = Unknown/Unused

group lists
 0 = Player 1
 1 = Player 2
 2 = Player 3
 3 = Player 4
 4 = Player 5
 5 = Player 6
 6 = Player 7
 7 = Player 8
 8 = Player 9
 9 = Player 10
 10 = Player 11
 11 = Player 12
 12 = Unknown 1
 13 = Current Player
 14 = Foes
 15 = Allies
 16 = Neutral Players
 17 = All Players
 18 = Force 1
 19 = Force 2
 20 = Force 3
 21 = Force 4
 22 = Unknown 2
 23 = Unknown 3
 24 = Unknown 4
 25 = Unknown 5
 26 = Non-Allied Victory Players

 */

#define ANYWHERELOCATIONNR 		63
#define WAITGAMETICKSBEFORESHOWGAMESTATUSMESSAGE	(MAXGAMECYCLESPERSECOND*2)
#define TRIGGER_ALLPLAYERSBITMASK	0x0fff		//all 12 players is set

#define TRIGGERENTRYFLAG_WAITEXECUTE	0x01
#define TRIGGERENTRYFLAG_STATEDISABLED	0x02
#define TRIGGERENTRYFLAG_ALWAYSDISPLAY	0x04
#define TRIGGERENTRYFLAG_UNKNOWN1	0x08
#define TRIGGERENTRYFLAG_MANYUNITS	0x10
#define TRIGGERENTRYFLAG_UNKNOWN2	0x20
#define TRIGGERENTRYFLAG_UNKNOWN3	0x40
#define TRIGGERENTRYFLAG_EXECUTED	0x80

#define TRIGGERCONDITIONSTATE_DISABLED	0x02
#define TRIGGERCONDITIONSTATE_CHECKED	0x80

#define TRIGGER_DISPLAYMESSAGETIME	10000
#define MAXTRGSWITCH			256
struct TRIG_CONDITION
{
	unsigned short	locationnr;
	unsigned short	TriggerStateChecked;	//for all players		
	unsigned char	actiononplayers;
	unsigned char	unknown1;
	unsigned short	unknown2;
	unsigned int 	seconds;		//seconds
	unsigned char	unittype;		//unitnr or typeof unit if >= e5 (build,men,factory,etc)
	unsigned char	unknown4;
	unsigned char 	subcond;	// - sub condition(at leat at most,...)
	unsigned char 	conditiontype;	// - Condition Type (Byte) - See Conditions List
	unsigned char	ResourceType;		// - type of resource
	unsigned char 	SwitchState;		// - Switch State: 0 = enabled, 2 = disabled (Byte)
	unsigned char 	unknown5;
	unsigned char 	unknown6;
};
struct TRIG_ACTION
{
	unsigned int 		locationnr;
	unsigned int		stringID;	// String # for location, (40 for anywhere) (Long)
	unsigned short		waveID;		// Wave Number
	unsigned short		TriggerStateDone;
	unsigned short		pauseatime;	//pause for milliseconds
	unsigned char		var01;
	unsigned char		var02;
	unsigned char		actiononplayers;
	unsigned char		var03;
	unsigned char		var04;
	unsigned char		var05;

	unsigned short		rescount;
	unsigned short		scriptnr;

	unsigned char		unitorrestype;	//resource type (0-minerals,1-gas,2-minerals&gas)
	unsigned char		var07;
	unsigned char		actiontype;		//action type (modify resources,etc...)
	unsigned char		subaction;		//subactiontype(add,set,substract,etc)
	unsigned char		TriggerEntryFlags;
	unsigned char		var08;
	unsigned short		var09;

};
#define TRIG_MAX_CONDITIONS	16
#define TRIG_MAX_ACTIONS	64
#define TRIG_MAX_GROUPPLAYERS	28
struct TRIG_GROUPINFO
{
	unsigned short	playersmask;		//my own reserved by condition[].actiononplayers
	unsigned short	unknown;		//my own reserved by pause a trigger
	unsigned char	grouplist[TRIG_MAX_GROUPPLAYERS];
};


struct MAP_TRIGS
{
	TRIG_CONDITION		condition[TRIG_MAX_CONDITIONS];
	TRIG_ACTION		action[TRIG_MAX_ACTIONS];
	TRIG_GROUPINFO		groupinfo;
};
#pragma pack(pop)

struct LEADERBOARD
{
	unsigned char	leaderboardtype;
	unsigned char	unittype;
	unsigned int	nrofunits;
	short int 	actiononplayers;
	short int	stringID; 
	char 		*txtstr;
	char		calcready;
};

void Triggers_ReduceMusicVolume(void);
void Triggers_RestoreMusicVolume(void);
void Triggers_SetMusicVolume(int volume);
void Triggers_ReduceSoundVolume(int exceptchannelid);
void Triggers_RestoreSoundVolume(void);

int CheckForUnit(int (*ConditionFunction)(int *, int),
		 int actiononplayers,int unitid,int cntunits,OBJ **retfirst,struct XY *searcharea,int modemove);
int CheckForUnit(int (*ConditionFunction)(int *, int),
		 int actiononplayers,int unitid,int cntunits,OBJ **retfirst,struct XY *searcharea);

int  Triggers_GetPause(void);
void Triggers_SetPause(int pauseval);

void First_Triggers_Prepare(struct mapinfo *info,int cnttrg,MAP_TRIGS *trigs);
void Triggers_Parce(struct mapinfo *info,int cnttrg,MAP_TRIGS *trigs,int ticks);
void Remove_Triggers(struct mapinfo *info);

int  AllGroups_Prepare(struct mapinfo *info,MAP_TRIGS *temptrg);
int  OneGroup_Prepare(struct mapinfo *info,int grouplist,int actionfromplayers);

int  Condition_Prepare(struct mapinfo *info,MAP_TRIGS *temptrg,int trig_nr,int playernr,int playernrmask,int deltatick);
int  Action_Prepare(struct mapinfo *info,MAP_TRIGS *temptrg,int trig_nr,int playernr,int playernrmask);

int  TRG_RunAIScript(struct mapinfo *info,int aiscriptnr,int playernr,int playernrmask);
int  TRG_RunAIScriptAtLocation(struct mapinfo *info,int aiscriptnr,int playernr,int playernrmask,int locnr);

void AI_SetSearchForAtacks(int playernr);

#define TRG_CONDITIONTYPE_NONE			0
#define TRG_CONDITIONTYPE_COUNTDOWNTIMER	1
#define TRG_CONDITIONTYPE_COMMAND		2
#define TRG_CONDITIONTYPE_BRING			3
#define TRG_CONDITIONTYPE_ACCUMULATE		4
#define TRG_CONDITIONTYPE_KILL			5
#define TRG_CONDITIONTYPE_COMMANDTHEMOST	6
#define TRG_CONDITIONTYPE_COMMANDTHEMOSTAT	7
#define TRG_CONDITIONTYPE_MOSTKILLS		8
#define TRG_CONDITIONTYPE_HIGHESTSCORE		9
#define TRG_CONDITIONTYPE_MOSTRESOURCES		10
#define TRG_CONDITIONTYPE_SWITCH		11
#define TRG_CONDITIONTYPE_ELAPSEDTIME		12
#define TRG_CONDITIONTYPE_MISSIONBRIEFING	13
#define TRG_CONDITIONTYPE_OPPONENTS		14
#define TRG_CONDITIONTYPE_DEATHS		15
#define TRG_CONDITIONTYPE_COMMANDTHELEAST	16
#define TRG_CONDITIONTYPE_COMMANDTHELEASTAT	17
#define TRG_CONDITIONTYPE_LEASTKILLS		18
#define TRG_CONDITIONTYPE_LOWESTSCORE		19
#define TRG_CONDITIONTYPE_LEASTRESOURCES	20
#define TRG_CONDITIONTYPE_SCORE			21
#define TRG_CONDITIONTYPE_ALWAYS		22
#define TRG_CONDITIONTYPE_NEVER			23

#define TRG_ACTIONTYPE_NONE				0
#define TRG_ACTIONTYPE_VICTORY				1
#define TRG_ACTIONTYPE_DEFEAT				2
#define TRG_ACTIONTYPE_PRESERVETRIGGER			3
#define TRG_ACTIONTYPE_WAIT				4
#define TRG_ACTIONTYPE_PAUSEGAME			5
#define TRG_ACTIONTYPE_UNPAUSEGAME			6
#define TRG_ACTIONTYPE_TRANSMISSION			7
#define TRG_ACTIONTYPE_PLAYWAV				8
#define TRG_ACTIONTYPE_DISPLAYTEXTMESSAGE		9
#define TRG_ACTIONTYPE_CENTERVIEW			10
#define TRG_ACTIONTYPE_CREATEUNITWITHPROPERTIES		11
#define TRG_ACTIONTYPE_SETMISSIONOBJECTIVES		12
#define TRG_ACTIONTYPE_SETSWITCH			13
#define TRG_ACTIONTYPE_SETCOUNTDOWNTIMER		14
#define TRG_ACTIONTYPE_RUNAISCRIPT			15
#define TRG_ACTIONTYPE_RUNAISCRIPTATLOCATION		16
#define TRG_ACTIONTYPE_LEADERBOARDCONTROL		17
#define TRG_ACTIONTYPE_LEADERBOARDCONTROLAT		18
#define TRG_ACTIONTYPE_LEADERBOARDRESOURCES		19
#define TRG_ACTIONTYPE_LEADERBOARDKILLS			20
#define TRG_ACTIONTYPE_LEADERBOARDPOINTS		21
#define TRG_ACTIONTYPE_KILLUNIT				22
#define TRG_ACTIONTYPE_KILLUNITATLOCATION		23
#define TRG_ACTIONTYPE_REMOVEUNIT			24
#define TRG_ACTIONTYPE_REMOVEUNITATLOCATION		25
#define TRG_ACTIONTYPE_SETRESOURCES			26
#define TRG_ACTIONTYPE_SETSCORE				27
#define TRG_ACTIONTYPE_MINIMAPPING			28
#define TRG_ACTIONTYPE_TALKINGPORTRAIT			29
#define TRG_ACTIONTYPE_MUTEUNITSPEACH			30
#define TRG_ACTIONTYPE_UNMUTEUNITSPEACH			31
#define TRG_ACTIONTYPE_LEADERBOARDCOMPUTERPLAYERS	32
#define TRG_ACTIONTYPE_LEADERBOARDGOALCONTROL		33
#define TRG_ACTIONTYPE_LEADERBOARDGOALCONTROLAT		34
#define TRG_ACTIONTYPE_LEADERBOARDGOALRESOURCES		35
#define TRG_ACTIONTYPE_LEADERBOARDGOALKILLS		36
#define TRG_ACTIONTYPE_LEADERBOARDGOALPOINTS		37
#define TRG_ACTIONTYPE_MOVELOCATION			38
#define TRG_ACTIONTYPE_MOVEUNITS			39
#define TRG_ACTIONTYPE_LEADERBOARDGREED			40
#define TRG_ACTIONTYPE_SETNEXTSCENARIO			41
#define TRG_ACTIONTYPE_SETDOODADSTATE			42
#define TRG_ACTIONTYPE_SETINVINCIBILITY			43
#define TRG_ACTIONTYPE_CREATEUNIT			44
#define TRG_ACTIONTYPE_SETDEATHS			45
#define TRG_ACTIONTYPE_ORDER				46
#define TRG_ACTIONTYPE_COMMENT				47
#define TRG_ACTIONTYPE_GIVEUNITSTOPLAYER		48
#define TRG_ACTIONTYPE_MODIFYUNITHITPOINT		49
#define TRG_ACTIONTYPE_MODIFYUNITENERGY			50
#define TRG_ACTIONTYPE_MODIFYUNITSHIELDPOINTS		51
#define TRG_ACTIONTYPE_MODIFYUNITRESOURCEAMOUNT		52
#define TRG_ACTIONTYPE_MODIFYUNITHANGERCOUNT		53
#define TRG_ACTIONTYPE_PAUSETIMER			54
#define TRG_ACTIONTYPE_UNPAUSETIMER			55
#define TRG_ACTIONTYPE_DRAW				56
#define TRG_ACTIONTYPE_SETALLIANCESTATUS		57
#define TRG_ACTIONTYPE_DISABLEDEBUGMODE			58
#define TRG_ACTIONTYPE_ENABLEDEBUGMODE			59

#define TRG_RESTYPE_MINERALS				0
#define TRG_RESTYPE_GAS					1
#define TRG_RESTYPE_MINERALSANDGAS			2

#define TRG_TYPEFUNC_ATLEAST				0
#define TRG_TYPEFUNC_ATMOST				1
#define TRG_TYPEFUNC_ISSET				2
#define TRG_TYPEFUNC_ISCLEARED				3
#define TRG_TYPEFUNC_SWITCHSET				4		//enabled (closed door)
#define TRG_TYPEFUNC_SWITCHCLEAR			5		//disabled (opened door)
#define TRG_TYPEFUNC_SWITCHTOGGLE			6
#define TRG_TYPEFUNC_SET				7
#define TRG_TYPEFUNC_ADD				8
#define TRG_TYPEFUNC_SUB				9
#define TRG_TYPEFUNC_EXACTLY				10
#define TRG_TYPEFUNC_SWITCHRANDOMIZE			11
#define TRG_MAXTYPEFUNC					12

#define TRG_SCORETYPE_TOTAL				0
#define TRG_SCORETYPE_UNITS				1
#define TRG_SCORETYPE_BUILDINGS				2
#define TRG_SCORETYPE_UNITSANDBUILDS			3
#define TRG_SCORETYPE_KILLS				4
#define TRG_SCORETYPE_RASINGS				5
#define TRG_SCORETYPE_KILLSANDRASINGS			6
#define TRG_SCORETYPE_CUSTOM				7
#define TRG_MAXSCORETYPES				8


#define TRG_PLAYERGROUP_PLAYER0				0
#define TRG_PLAYERGROUP_PLAYER1				1
#define TRG_PLAYERGROUP_PLAYER2				2
#define TRG_PLAYERGROUP_PLAYER3				3
#define TRG_PLAYERGROUP_PLAYER4				4
#define TRG_PLAYERGROUP_PLAYER5				5
#define TRG_PLAYERGROUP_PLAYER6				6
#define TRG_PLAYERGROUP_PLAYER7				7
#define TRG_PLAYERGROUP_PLAYER8				8
#define TRG_PLAYERGROUP_PLAYER9				9
#define TRG_PLAYERGROUP_PLAYER10			10
#define TRG_PLAYERGROUP_PLAYER11			11
#define TRG_PLAYERGROUP_UNKNOWN1			12
#define TRG_PLAYERGROUP_CURRENTPLAYER			13
#define TRG_PLAYERGROUP_FOES				14
#define TRG_PLAYERGROUP_ALLIES				15
#define TRG_PLAYERGROUP_NEUTRALPLAYERS			16
#define TRG_PLAYERGROUP_ALLPLAYERS			17
#define TRG_PLAYERGROUP_FORCE1				18
#define TRG_PLAYERGROUP_FORCE2				19
#define TRG_PLAYERGROUP_FORCE3				20
#define TRG_PLAYERGROUP_FORCE4				21
#define TRG_PLAYERGROUP_UNKNOWN2			22
#define TRG_PLAYERGROUP_UNKNOWN3			23
#define TRG_PLAYERGROUP_UNKNOWN4			24
#define TRG_PLAYERGROUP_UNKNOWN5			25
#define TRG_PLAYERGROUP_NONALLIEDVICTORY		26


#define TRG_AISCRIPT_TURNONSHAREDVISIONPLAYER0		0x3069562b
#define TRG_AISCRIPT_TURNONSHAREDVISIONPLAYER1		0x3169562b
#define TRG_AISCRIPT_TURNONSHAREDVISIONPLAYER2		0x3269562b
#define TRG_AISCRIPT_TURNONSHAREDVISIONPLAYER3		0x3369562b
#define TRG_AISCRIPT_TURNONSHAREDVISIONPLAYER4		0x3469562b
#define TRG_AISCRIPT_TURNONSHAREDVISIONPLAYER5		0x3569562b
#define TRG_AISCRIPT_TURNONSHAREDVISIONPLAYER6		0x3669562b
#define TRG_AISCRIPT_TURNONSHAREDVISIONPLAYER7		0x3769562b

#define TRG_AISCRIPT_TURNOFFSHAREDVISIONPLAYER0		0x3069562d
#define TRG_AISCRIPT_TURNOFFSHAREDVISIONPLAYER1		0x3169562d
#define TRG_AISCRIPT_TURNOFFSHAREDVISIONPLAYER2		0x3269562d
#define TRG_AISCRIPT_TURNOFFSHAREDVISIONPLAYER3		0x3369562d
#define TRG_AISCRIPT_TURNOFFSHAREDVISIONPLAYER4		0x3469562d
#define TRG_AISCRIPT_TURNOFFSHAREDVISIONPLAYER5		0x3569562d
#define TRG_AISCRIPT_TURNOFFSHAREDVISIONPLAYER6		0x3669562d
#define TRG_AISCRIPT_TURNOFFSHAREDVISIONPLAYER7		0x3769562d

#define TRG_AISCRIPT_FILLSILOWITHNUKE_TYPEA		0x41324254
#define TRG_AISCRIPT_FILLSILOWITHNUKE_TYPEB		0x42324254
#define TRG_AISCRIPT_CAST_DISRUPTIONWEB			0x46334250
#define TRG_AISCRIPT_SETALLUNITSTORANDOMSUICIDEMISSION	0x52697553
#define TRG_AISCRIPT_SHOW_RECALL			0x65486552
#define TRG_AISCRIPT_ENTERCLOSESTBUNKER			0x6b426e45
#define TRG_AISCRIPT_SWITCHCOMPPLAYERTORESCUEDPASSIVE	0x75637352
#define TRG_AISCRIPT_SETPLAYERTOENEMYHERE		0x796d6e45
#define TRG_AISCRIPT_SETPLAYERTOALLIESHERE		0x796c6c41

extern char 	TRIG_ChangeStat,TRIG_active;
extern char	TRIG_leaderboardcomputerplayers;
extern char	TRIG_stopalltriggers;
extern char	TRIG_inittriggers;
extern int	(*ConditionFunctions[])(int *var1,int var2);
extern int	(*DoorConditionFunctions[])(int *var1,int var2);

#endif /*TRIGGERS_H*/
