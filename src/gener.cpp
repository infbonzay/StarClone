//=============================
#include <stdio.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <grplib/gr8.h>
#include <grplib/usegrp.h>

#include "CrossPlatform.h"

#include "defs.h"
#include "vars.h"
#include "market.h"

#include "ScreenDraw.h"
#include "RegenObjMap.h"
#include "commandqueue.h"
#include "debug.h"
#include "auxil.h"
#include "mytime.h"
#include "lists.h"
#include "key.h"
#include "gr.h"
#include "objs.h"
#include "objinfo.h"
#include "putobj.h"
#include "diap.h"
#include "load.h"
#include "tempobj.h"
#include "maps.h"
#include "mage.h"
#include "scripts.h"
#include "mpq.h"
#include "audio.h"
#include "starmap.h"
#include "creep.h"
#include "queue.h"
#include "action.h"
#include "fonts.h"
#include "gamemenu.h"
#include "rand.h"
#include "fog.h"
#include "vision.h"
#include "doodad.h"
#include "stars.h"
#include "mytime.h"
#include "portrait.h"
#include "messages.h"
#include "flingy.h"
#include "weapons.h"
#include "overlay.h"
#include "dialogbin.h"
#include "network_general.h"
#include "netwgame.h"
#include "stringfiles.h"
#include "triggers.h"
#include "music.h"
#include "video.h"
#include "iscript.h"
#include "gener.h"
#include "Controller.h"

#include <stdint.h>
#include "mycycle.h"

//=================================
DIALOGBIN_INFO 	*minimapmenu, *f10menu, *statbtnmenu;
char 			f10_menu, terr_menu, mess_menu, dipl_menu;
FORCE_SLOTS 	force_slots;
NETW_PLAY		netplay;
MENUDRAW		showedmenu;
int 			menustatus, startmission, campaign_id;
HighMouse 		*highMouse;
DestCursor		*destCursor;
ScreenDraw		*screenDraw;
RegenObjMap		*regenObjMap;

char			select_aria, karta_aria, mode_aria;
bool			movieminikarta;
int				mousehotpos;
//=================================
/*====================================================
======================================================
					 Begin of program
======================================================
======================================================*/
int main(int c, char **parm, char **env)
{

	int i, err, firsttimelaunch = 0, videook = 0;
	int status, status2, gamequitstatus, mission_id, selected_id, missionfrommenu, runonemission;
	char temppath[MAXFILENAMESYMBOLS];
	mapinfo *testmap;
#ifdef TESTMALLOC
	char *testmem = (char *)wmalloc(16);
	wfree(testmem);
#endif

	inivars();
	fflush(stdout);
	getcwd(GAMEPATH, sizeof(GAMEPATH) - 1);//get temp pwd directory, it will be change after load config
	printf("SIZEOF SOMEINFO UNION=%d\n", (int)sizeof(SOMEINFO));
	printf("OBJ=%d OBJstruct=%d\n", (int)sizeof(OBJ), (int)sizeof(OBJstruct));
	do {
		printf("Loading config file " SC_CONFIGFILE "\n");
		UnLoadAllMpqs();
		unloadcfg();
		i = loadcfg(SC_CONFIGFILE, &firsttimelaunch);
		if (i)
		{
			if (i == -2)
			{
				printf(SC_CONFIGFILE " not found.\nCreate default " SC_CONFIGFILE " configuration.\n");
				i = loadcfg(SC_CONFIGFILE, &firsttimelaunch);
			}
			else
			{
				printf("Error in line %d (" SC_CONFIGFILE ")\n", i);
				exit(-1);
			}
		}
		//	  printf("%lld\n",gettimeticks2());

		//	  getcwd(GAMEPATH,sizeof(GAMEPATH)-1);
		//	  strcpy(GAMEMAPPATH,GAMEPATH);
		//	  strcat(GAMEMAPPATH,"/maps");

		mkdir("maps", 0755);
		mkdir(USERS_DIRECTORY, 0755);
		mkdir(SAVES_DIRECTORY, 0755);
		mkdir(REPLAYS_DIRECTORY, 0755);
		mkdir(TEMPDIR, 0755);

		if (chdir(GAMEMAPPATH))
		{
			printf("WARNING: the MAP %s do not exist\n", GAMEMAPPATH);
		}
		//	  printf("%lld\n",gettimeticks2());
		int readedsymb = readlink(GAMEMAPPATH, temppath, sizeof(temppath) - 1);
		if (readedsymb != -1)
		{
			getcwd(GAMEMAPPATH, sizeof(GAMEMAPPATH) - 1);
		}
		err = chdir(GAMEPATH);
		if (err)
		{
			printf("cannot chdir %s, path not found.\n", GAMEPATH);
			exit(-1);
		}
		//	  printf("%lld\n",gettimeticks2());
		loadallfonts();
		highMouse = new HighMouse();
		printf("set graph mode...\n");
		if (!videook)
		{
			if (mainController.Init())
			{
				gameend("can not initialize graph");
			}
			i = setmode(gameconf.grmode.x, gameconf.grmode.y, gameconf.grmode.s, gameconf.grmode.flags & DISPLAYFLAGS_FULLSCREENMODE);
			if (!i)
			{
				printf("cannot initialize %dx%dx%d video mode\n",
					gameconf.grmode.x, gameconf.grmode.y, gameconf.grmode.s);
				exit(-1);
			}
			if (i == 2)
				gameconf.grmode.flags |= DISPLAYFLAGS_EMULATIONMODE;
			highMouse->SetPos(gameconf.grmode.x / 2, gameconf.grmode.y / 2);
			videook = 1;
		}
		if (firsttimelaunch & 0xff)
		{
			LoadDatTblFiles(&alldattbl);
			if (DownloadMpqMenu(firsttimelaunch) == -1)
				exit(-1);
			UnloadDatTblFiles(&alldattbl);
		}
	} while (firsttimelaunch & 0xff);
	screenMapInfo = new ScreenMapInfo();

	DEBUG_INIT(0);

	gameconf.audioconf.audioflags = 0;
	printf("initialization sound system ...\n");
	i = initsoundengine(MAXSOUNDS);
	if (i < 0)
	{
		gameconf.audioconf.audioflags |= AUDIOFLAGS_MUSICDISABLE;
		gameconf.audioconf.audioflags |= AUDIOFLAGS_SOUNDDISABLE;
		printf("Can not initialize sound. Disabled\n");
	}
	volumeallsfx(gameconf.audioconf.soundvolume);

	ShowFirstRunVideo(0);

	PlayMusic("music\\title.wav", -1);

	MENUSTR *title = showtitle();
	if (LoadDatTblFiles(&alldattbl))
	{
		//		  gameend("Problem with loading dat,tbl files");
	}
	LoadPatchTbl();
	if (loadznak())
	{
		//		exit(-1);
	}
	loadlang();
	fflush(stdout);
	printf("parce units table ...\n");
	printf("	  %d units parced\n", createmantable());
	err = loadweapons();
	if (err < 0)
	{
		printf("weapons:Error in file nr:%d\n", err);
	}

	printf("Press '1' for change player nr,F12 - exit\n");
	addvars();
	SetVisionTables();

	createtemptables();
	//	  setseed(mytimer.GetCurrentTimerTick());
	setseed(0);

	if (iscriptinfo.CompileIScripts("data/iscript"))
		return(-1);

	int sleepNr = gameconf.videoconf.titledelay + 1;
	do {
		mytimer.CallTimer(MYTIMER_ASINCHRONMODE);
		usleep(125000);
	} while (--sleepNr);
	cleartitle(title);

	GetINOUTsmk();

	/*	  int array[5]={4,0,1,2,3};

		int deltax=(640-380)/2;
		int deltay=(480-251)/2;
		MENUSTR *temp=LoadDialogBin("rez\\gluhist.bin",DIALOGBIN_MAIN,1,IDFONT16);
		temp->menu[0].itemtype=ISLABELCENTER;
		temp->menu[0].fontnr=IDFONT16;
		addimageitem(temp,4,0,0,380,251,"glue\\campaign\\phist.pcx");
		setmenuitem_DISABLED(temp,4,FALSE); //ok
		setmenuitem_VISIBLED(temp,4,TRUE);	//ok

		temp->x+=deltax;
		temp->y+=deltay;
		for (int i=0;i<5;i++)
		{
		temp->menu[i].hotdeltax+=deltax;
		temp->menu[i].hotdeltay+=deltay;
		}
		DialogBin_Save(temp,"BZHistory","rez/gluhist2.bin",array);

		temp=LoadDialogBin("rez\\gluhist2.bin",DIALOGBIN_MAIN,0,IDFONT16);
	*/

	do {
		int selected = mainmenu();
		do {
			gamequitstatus = CONTINUEGAME;
			switch (selected)
			{
			case SINGLEGAME:
				NETWORKGAME = 0;
				selected = SINGLEGAME;
				NUMBGAMER = 0;//
				status = glu_login();
				switch (status)
				{
				case 0://user selected ok pressed
					do {
						if (!EXPANSIONSET)
							status = campaignselect();
						else
							status = xcampaignselect();
						missionfrommenu = 1;
						switch (status)
						{
						case STAR_PROTOSS:
						case BROOD_PROTOSS:
						case STAR_TERRAN:
						case BROOD_TERRAN:
						case STAR_ZERG:
						case BROOD_ZERG:
							StopMusic(MUSIC_STOPWITHFADE);
						playmission:
							GAMETYPE = MAP_GAMETYPE_USEMAPSETTINGS;
							selected_id = (startmission >> 16) & 0xff;
							campaign_id = (startmission >> 8) & 0xff;
							mission_id = startmission & 0xff;
							//DEBUGMESSCR("startmission=0x%x\n",startmission);
							status = getcampaignname(campaign_id, selected_id);
							if (status == SHOWVIDEO)
							{
								//show the video
								PlayCampaignVideo(campaign_id, selected_id);
								if (missionfrommenu)
								{
									gamequitstatus = PREVIOUSMENU;
									break;
								}
								goto gonextmission;
							}
							missionfrommenu = 0;
						repeatmissionagain:
							ShowPreviewMission(campaign_id, selected_id, 0);//show info before the mission
							if (status == ENDCAMPAIGN)
							{
								gamequitstatus = PREVIOUSMENU;
								break;
							}
							if (status == PREVIEWTEXT)
							{
								goto gonextmission;
							}
							clearplayersconfig();
							clearplayernames();
							testmap = (mapinfo *)wmalloc(sizeof(mapinfo));
							memset(testmap, 0, sizeof(mapinfo));

							err = starmap_info(SELECTMAP, NULL, testmap);
							if (err)
							{
								gamequitstatus = PREVIOUSMENU;
								break;
							}
							copytempowners(testmap);
							preparegameconf_ums();
							force_slots.CreatePlayersNr();
							selected = SINGLEGAME;
							status = glu_briefing(gameconf.pl_race[NUMBGAMER], NETWORKGAME, testmap, SELECTMAP, startmission == 0);
							unload_starmapallocated(testmap);
							wfree(testmap);
							testmap = NULL;
							switch (status)
							{
							case STARTGAME://let's play
								gamequitstatus = letsplaygame(gameconf.pl_race[NUMBGAMER], SELECTMAP);
								free_missionobjectives();
							case SKIPMISSION://go next mission
								if (status == SKIPMISSION)
								{
									gamestatus = WINGAME;
									gamequitstatus = QUITGAME;
								}
								if (gamequitstatus == EXITGAME || gamequitstatus == QUITMISSION)
									break;
								//gamestatus = WINGAME; //for test
								if (gamestatus != WINGAME || gamequitstatus == RESTARTGAME)
									goto repeatmissionagain;
							gonextmission:
								ShowPreviewMission(campaign_id, selected_id, 1);//show info after mission(if exist)
								//get the next mission
								selected_id = curplayer[0].missions[campaign_id].next_missions[selected_id];
								if (nextscenario)
								{
									runonemission = openmission(campaign_id, nextscenario);
									selected_id = nextscenario;
								}
								else
								{
									runonemission = openmission(campaign_id, selected_id);
								}
								nextscenario = 0;
								if (runonemission)
								{
									//complete campaign go to the menu
									gamequitstatus = PREVIOUSMENU;
									break;
								}
								//campaign not done, go next mission;
								mission_id = curplayer[0].missions[campaign_id].seq_missions[selected_id] & 0x7f;
								startmission = (selected_id << 16) | (campaign_id << 8) | mission_id;
								goto playmission;
								break;
							case CANCELGAME://cancel play
								selected = SINGLEGAME;
								gamequitstatus = PREVIOUSMENU;
								free_missionobjectives();
								break;
							}
							break;
						case STAR_LOADSAVED:
						case BROOD_LOADSAVED:
							selected = SINGLEGAME;
							gamequitstatus = glu_loadgame();
							break;
						case STAR_LOADREPLAY:
						case BROOD_LOADREPLAY:
							selected = SINGLEGAME;
							gamequitstatus = glu_loadreplay();
							break;
						case STAR_PLAYCUSTOM:
						case BROOD_PLAYCUSTOM:
							do {
								//						printf("single game go to SELECTMAP MENU\n");
								clearplayersconfig();
								clearplayernames();
								setplayername(NUMBGAMER, nickname);
								status = selectmapmenu();
								do {
									if (GAMETYPE == MAP_GAMETYPE_USEMAPSETTINGS)
										preparegameconf_ums();
									testmap = (mapinfo *)wmalloc(sizeof(mapinfo));
									memset(testmap, 0, sizeof(mapinfo));

									err = starmap_info(NULL, SELECTMAP, testmap);
									if (!err)
									{
										starmap_forceslots(testmap, &force_slots, GAMETYPE);
									}
									force_slots.CreatePlayersNr();
									selected = SINGLEGAME;
									gamequitstatus = CONTINUEGAME;
									switch (status)
									{
									case 0:
										status = glu_briefing(gameconf.pl_race[NUMBGAMER], NETWORKGAME, testmap, NULL, 0);
										unload_starmapallocated(testmap);
										wfree(testmap);
										testmap = NULL;
										switch (status)
										{
										case STARTGAME://let's play
											gamequitstatus = letsplaygame(gameconf.pl_race[NUMBGAMER], NULL);
											break;
										case CANCELGAME://cancel play
											selected = SINGLEGAME;
											gamequitstatus = PREVIOUSMENU;
											break;
										}
										free_missionobjectives();
										break;
									case 1:
										//									printf("return to menu SINGLEGAME\n");
										unload_starmapallocated(testmap);
										wfree(testmap);
										testmap = NULL;
										gamequitstatus = PREVIOUSMENU;
										selected = SINGLEGAME;
										break;
									default:
										unload_starmapallocated(testmap);
										wfree(testmap);
										testmap = NULL;
										selected = SINGLEGAME;
										gamequitstatus = PREVIOUSMENU;
										printf("Error code %d\n", status);
										gameend("error in selectmap() menu");
										break;
									}
								} while (gamequitstatus == RESTARTGAME);
								if (gamequitstatus != EXITGAME)
									PlayMusic("music\\title.wav", -1);
							} while (gamequitstatus != PREVIOUSMENU && gamequitstatus != EXITGAME);
							break;
						case STAR_ERROR:
						case BROOD_ERROR:
							printf("gametype error(missed some files)\n");
							gameend("");
							break;
						case STAR_CANCEL:
						case BROOD_CANCEL:
							selected = SINGLEGAME;
							gamequitstatus = CONTINUEGAME;
							break;
						default:
							selected = SINGLEGAME;
							gamequitstatus = CONTINUEGAME;
							break;
						}//gametype
						if (gamequitstatus != EXITGAME)
							PlayMusic("music\\title.wav", -1);
					} while (gamequitstatus == PREVIOUSMENU);
					break;
				case 2://exit to mainmenu
					selected = -1;
					break;
				default:
					printf("Error code %d\n", status);
					gameend("error in singlegame() menu");
					break;
				}
				break;
			case MULTIPLAYERGAME:
				NETWORKGAME = 1;
				selected = MULTIPLAYERGAME;
				NUMBGAMER = 0;
				do {
					status = glu_conn();
					switch (status)
					{
					case NETWORKGAMETYPE1:
					case NETWORKGAMETYPE2:
					case NETWORKGAMETYPE3:
					case NETWORKGAMETYPE4:
						status2 = glu_login();
						if (status2 == 2)
						{
							gamequitstatus = PREVIOUSMENU;
							break;
						}
						do {
							status2 = glu_join(&force_slots);
							switch (status2)
							{
							case CREATEGAME:
								do
								{
									clearplayernames();
									status2 = glu_creat(&force_slots);
									switch (status2)
									{
									case CREATEDGAME:
										NUMBGAMER = GetForceFirstEmptySlot(&force_slots);
										HOSTGAMER = OWNERGAMER;
										setplayername(NUMBGAMER, nickname);
										status2 = glu_chat(HOSTGAMER, NUMBGAMER, &force_slots);
										switch (status2)
										{
										case PLAYNETWORKGAME:
											printf("numbgamer=%d\n", NUMBGAMER);
											status = glu_briefing(gameconf.pl_race[NUMBGAMER], NETWORKGAME, NULL, NULL, 0);
											switch (status)
											{
											case STARTGAME://let's play
												gamequitstatus = letsplaygame(gameconf.pl_race[NUMBGAMER], NULL);
												break;
											case CANCELGAME://cancel play
												gamequitstatus = PREVIOUSMENU;
												break;
											}
											free_missionobjectives();
											break;
										case EXITCREATEDGAME:
										default:
											gamequitstatus = CONTINUEGAME;
											break;
										}
										break;
									default:
										gamequitstatus = PREVIOUSMENU;
										break;
									}
								} while (gamequitstatus != PREVIOUSMENU && gamequitstatus != EXITGAME);
								if (gamequitstatus != EXITGAME)
								{
									PlayMusic("music\\title.wav", -1);
									gamequitstatus = CONTINUEGAME;
								}
								break;
							case JOINGAME:
								//					printf("joined slot=%d\n",JOINSLOT);
								NUMBGAMER = JOINSLOT;
								setplayername(NUMBGAMER, nickname);
								HOSTGAMER = GUESTGAMER;
								force_slots.Clear();//empty any previous garbage
								status2 = glu_chat(HOSTGAMER, NUMBGAMER, &force_slots);
								switch (status2)
								{
								case PLAYNETWORKGAME:
									printf("numbgamer=%d\n", NUMBGAMER);
									status = glu_briefing(gameconf.pl_race[NUMBGAMER], NETWORKGAME, NULL, NULL, 0);
									switch (status)
									{
									case STARTGAME://let's play
										gamequitstatus = letsplaygame(gameconf.pl_race[NUMBGAMER], NULL);
										break;
									case CANCELGAME://cancel play
										gamequitstatus = CONTINUEGAME;
										break;
									}
									free_missionobjectives();
									break;
								case EXITCREATEDGAME:
								default:
									gamequitstatus = CONTINUEGAME;
									break;
								}
								break;
							default:
								gamequitstatus = PREVIOUSMENU;
								break;
							}
						} while (gamequitstatus != PREVIOUSMENU && gamequitstatus != EXITGAME);
						if (gamequitstatus != EXITGAME)
						{
							PlayMusic("music\\title.wav", -1);
							gamequitstatus = CONTINUEGAME;
						}
						break;
					default:
						gamequitstatus = PREVIOUSMENU;
						selected = -1;
						break;
					}
				} while (gamequitstatus != PREVIOUSMENU && gamequitstatus != EXITGAME);
				if (gamequitstatus != EXITGAME)
					gamequitstatus = CONTINUEGAME;
				break;
			case EXITGAME:
				StopMusic(MUSIC_STOPWITHFADE);
				selected = EXITGAME;
				gameend("Normal exit");
				return 0;
				break;
			case VIEWCREDITS:
				//			StopMusic(MUSIC_STOPWITHFADE);
				//			if (EXPANSIONSET)
				//				ShowPreviewFile("rez\\crdt_exp.txt");
				ShowPreviewFile("rez\\crdt_lst.txt", ANYKEYQUITSHOWTITLES);
				gamequitstatus = CONTINUEGAME;
				break;
			case VIEWINTRO:
				StopMusic(MUSIC_STOPWITHFADE);
				ShowFirstRunVideo(1);
				PlayMusic("music\\title.wav", -1);
				gamequitstatus = CONTINUEGAME;
				break;
			case EDITORGAME:
				gamequitstatus = CONTINUEGAME;
				break;
			default:
				printf("Error code %d\n", status);
				gameend("error in mainmenu() menu");
				break;
			}
		} while ((selected == SINGLEGAME || selected == MULTIPLAYERGAME) && gamequitstatus != EXITGAME);
	} while (gamequitstatus != EXITGAME);
	StopMusic(MUSIC_STOP);
	iscriptinfo.~ISCRIPT();
	gameend("end from menu");
	return (0);
}
//==========================
#define MAXGAMEMUSIC	3
signed char gamemusicnr[MAXGAMEMUSIC];
signed char gamemusicstart;
//==========================
void PreparePlayGameMusic(void)
{
	gamemusicstart = -1;
	gamemusicnr[0] = myrand(MAXGAMEMUSIC);
	gamemusicnr[1] = myrand(MAXGAMEMUSIC);
	if (gamemusicnr[0] == gamemusicnr[1])
		if (++gamemusicnr[1] >= MAXGAMEMUSIC)
			gamemusicnr[1] = 0;
	gamemusicnr[2] = MAXGAMEMUSIC - gamemusicnr[0] - gamemusicnr[1];
}
//==========================
void GoPlayNextMusic(void)
{
	if (audiostream && !audiostream->stopchannel)
		return;
	int race = gameconf.pl_race[NUMBGAMER];
	if (++gamemusicstart >= MAXGAMEMUSIC)
		gamemusicstart = 0;
	sprintf(FULLFILENAME, "music\\%s%c.wav", racename[race], gamemusicnr[gamemusicstart] + '1');
	PlayMusic(FULLFILENAME, 0, &GoPlayNextMusic);
}
//==========================
int letsplaygame(int race, char *mypath)
{
	int status, i;

	CHEATERBIT = 0;
	CODEFORSCREEN = 0;
	CODEFORGOD = 0;
	CODEFORQUICKMAKE = 1;
	CODEFORWITHOUTGOODS = 0;
	CODEFORWITHOUTTECHTREE = 0;
	CODEFORPSIUNLIMIT = 0;
	SHOWCELLS = 0;

	if (race == RACE_OBSERVER)
		race = RACE_TERRAN;

	clearallplayerinfo();
	loadbeforestarmap(race);
	i = load_starmap(SELECTMAP, mypath, &map, &gameconf);
	if (i)
	{
		printf("error load %s\n", SELECTMAP);
		if (i == 1)
			printf("error load MPQ-archive ,errorcode=%d\n", i);
		else
			printf("error load %s,errorcode=%d\n", SELECTMAP, i);
		status = PREVIOUSMENU;
	}
	else
	{
		calcsomespellinfo();

		DEBUGMESSCR("loaded %s\n", SELECTMAP);
		if (map.flags & STARMAP_FLAG_EXPANSION)
			DEBUGMESSCR("(EXPANSION MAP)\n", SELECTMAP);
		map.played_as_nr = NUMBGAMER;

		mytimer.ClearGameTimer();
		clearshowgoods();

		PreparePlayGameMusic();
		GoPlayNextMusic();	//first start play music

		status = gogame(&map);
		map.secondsplayed = mytimer.GetCurrentGameTime();
		if (status != EXITGAME)
			glu_score(&map);
		if (!map.valid_vcode)
		{
			printf("ERROR with map(valid_vcode=0,may be already freed)");
		}
	}
	unload_starmap(&map);
	unloadafterstarmap();
	if (NETWORKGAME)
		DestroyAllConnectionsWithPlayers(&playersconn);
	//	  NUMBGAMER = 0;
	return(status);
}
//==========================================
void mouseonhotpos(int value)
{
	mousehotpos = value;
}
//==========================================
void setmainscreenmouseevents(void)
{
	int i, x, y;
	addnewmousehotpos(&mouseonhotpos, 4 + MAXSELECTMAN + 11);//total mousehotspots
	addmousehotpos(highMouse->RestrictXY[MOUSEMODE4].x1,
		highMouse->RestrictXY[MOUSEMODE4].y1,
		highMouse->RestrictXY[MOUSEMODE4].x2 - highMouse->RestrictXY[MOUSEMODE4].x1 + 1,
		highMouse->RestrictXY[MOUSEMODE4].y2 - highMouse->RestrictXY[MOUSEMODE4].y1 + 1,
		NULL);
	for (i = 0;i < MAXSELECTMAN;i++)
	{
		//	x = XUNITBAR+i/2*36;
		//	y = YUNITBAR+(i&1)*36;
		x = 172 + DELTASCREENX + i / 2 * 36;
		y = YUNITBAR + (i & 1) * 36;
		addmousehotpos(x, y, 36, 36, NULL);
	}
	for (i = 0;i < 9;i++)
	{
		addmousehotpos(statbtnmenu->iteminfo[i + 1].xpos + statbtnmenu->iteminfo[0].xpos,
			statbtnmenu->iteminfo[i + 1].ypos + statbtnmenu->iteminfo[0].ypos,
			statbtnmenu->iteminfo[i + 1].xsize,
			statbtnmenu->iteminfo[i + 1].ysize,
			NULL);
	}
	addmousehotpos(f10menu->iteminfo[F10DIALOG_MENUBUTTON].xpos + f10menu->iteminfo[F10DIALOG_MAIN].xpos,
		f10menu->iteminfo[F10DIALOG_MENUBUTTON].ypos + f10menu->iteminfo[F10DIALOG_MAIN].ypos,
		f10menu->iteminfo[F10DIALOG_MENUBUTTON].xsize,
		f10menu->iteminfo[F10DIALOG_MENUBUTTON].ysize,
		&f10menu->iteminfo[F10DIALOG_MENUBUTTON].Flags);
	addmousehotpos(minimapmenu->iteminfo[MINIMAPDIALOG_TERRAINBUTTON].xpos + minimapmenu->iteminfo[MINIMAPDIALOG_MAIN].xpos,
		minimapmenu->iteminfo[MINIMAPDIALOG_TERRAINBUTTON].ypos + minimapmenu->iteminfo[MINIMAPDIALOG_MAIN].ypos,
		minimapmenu->iteminfo[MINIMAPDIALOG_TERRAINBUTTON].xsize,
		minimapmenu->iteminfo[MINIMAPDIALOG_TERRAINBUTTON].ysize,
		&minimapmenu->iteminfo[MINIMAPDIALOG_TERRAINBUTTON].Flags);
	addmousehotpos(minimapmenu->iteminfo[MINIMAPDIALOG_MESSAGEBUTTON].xpos + minimapmenu->iteminfo[MINIMAPDIALOG_MAIN].xpos,
		minimapmenu->iteminfo[MINIMAPDIALOG_MESSAGEBUTTON].ypos + minimapmenu->iteminfo[MINIMAPDIALOG_MAIN].ypos,
		minimapmenu->iteminfo[MINIMAPDIALOG_MESSAGEBUTTON].xsize,
		minimapmenu->iteminfo[MINIMAPDIALOG_MESSAGEBUTTON].ysize,
		&minimapmenu->iteminfo[MINIMAPDIALOG_MESSAGEBUTTON].Flags);
	addmousehotpos(minimapmenu->iteminfo[MINIMAPDIALOG_DIPLOMACYBUTTON].xpos + minimapmenu->iteminfo[MINIMAPDIALOG_MAIN].xpos,
		minimapmenu->iteminfo[MINIMAPDIALOG_DIPLOMACYBUTTON].ypos + minimapmenu->iteminfo[MINIMAPDIALOG_MAIN].ypos,
		minimapmenu->iteminfo[MINIMAPDIALOG_DIPLOMACYBUTTON].xsize,
		minimapmenu->iteminfo[MINIMAPDIALOG_DIPLOMACYBUTTON].ysize,
		&minimapmenu->iteminfo[MINIMAPDIALOG_DIPLOMACYBUTTON].Flags);
	addmousehotpos(0, 0, gameconf.grmode.x, gameconf.grmode.y - YDECICONS - 13, NULL);
	incrcallbackwork();
}
//==========================
void deletemainscreenmouseevents(void)
{
	decrcallbackwork();
	delnewmousehotpos();
}
//==========================
int needredesen = 0;
unsigned char buton2 = 0;
//==========================
void mouseonkartaarea(void)
{
	if (karta_aria)
	{
		if (highMouse->GetButtonStatus() & WMLEFTKEY)
		{
			if (highMouse->WaitToPressLeftButton && highMouse->WaitToReleaseLeftButton)
			{
				//action on leftbutton on minimap
				highMouse->WaitToPressLeftButton = 0;
				highMouse->WaitToReleaseLeftButton = 0;
				highMouse->DoRightClickAction(NULL,
					(int)((highMouse->PosX - screenMapInfo->MinimapPosX - screenMapInfo->MinimapStartX) / factorx)*SIZESPRLANSHX,
					(int)((highMouse->PosY - screenMapInfo->MinimapPosY - screenMapInfo->MinimapStartY) / factory)*SIZESPRLANSHY,
					0);
				//action
			}
			else
			{
				if (!destCursor->Presence)
				{
					if (MAPDEF && !highMouse->MouseOnSelectionMode)
					{
						movieminikarta = SetVisualMapPosition((int)(((highMouse->PosX - screenMapInfo->MinimapPosX - screenMapInfo->MinimapStartX) / factorx) - screenMapInfo->SizeX32 / 2)*SIZESPRLANSHX,
							(int)(((highMouse->PosY - screenMapInfo->MinimapPosY - screenMapInfo->MinimapStartY) / factory) - screenMapInfo->SizeY32 / 2)*SIZESPRLANSHY);
					}
				}
			}
		}
		else
		{
			if ((!buton2) && (highMouse->GetButtonStatus() & WMRIGHTKEY) && !canceloperation)
			{
				buton2 = 1;
				highMouse->MouseModeMove = MODEMOVE;
				highMouse->DoRightClickAction(NULL,
					(int)((highMouse->PosX - screenMapInfo->MinimapPosX - screenMapInfo->MinimapStartX) / factorx)*SIZESPRLANSHX,
					(int)((highMouse->PosY - screenMapInfo->MinimapPosY - screenMapInfo->MinimapStartY) / factory)*SIZESPRLANSHY,
					0);
			}
		}
	}
	if (!(highMouse->GetButtonStatus() & WMRIGHTKEY))
	{
		buton2 = 0;
	}

}
//==========================
void redesenscreen(void)
{
	calculatefog(bitsplayer);			//calculate all fog
	drawMAP(0);
	CreatePylonSelectArea();
	RemovePylonSelectArea();
	putfog();			//show fogofwar
	showgoods();
	int pc = desenbuildifconstr();
	if (pc != 0)
		highMouse->Construct.CanDo = pc;
	highMouse->DrawSelectionArea();
	//		desenlocation();
	showramka();
	drawMINIMAP();
	desenproperties(properties, selectableproperties);//draw properties of selected unit(s) on screen
	printobjparam();		//draw parameters of selected objects
}
//==========================
int totable, bitsplayer;
Queue QueueGame(&QueueAction, MAXQUEUEDELEM);
long commandqueuetick;
CommandQueue UnitsCommandQueue(&CommandQueueAction, MAXCOMMANDELEM);
//==========================
void FreeQueues(void)
{
	QueueGame.EmptyQueue();
	UnitsCommandQueue.EmptyQueue();
}
//==========================
void GoNextCommandTick()
{
	if (!NETWORKGAME)
		commandqueuetick += 1;
	else
		commandqueuetick += 1;//TODO need to add something else
}
//==========================
void pregameQUEUE(void)
{
	MakeQueueAction(SELFCREATEACTION, SPECIALMAPID, &MAPREGENERATIONBIT, MINIMAPREFRESHCYCLE, 0, 0, 1);
	MakeQueueAction(SELFCREATEACTION, SPECIALMAPUNITSID, &MAPUNITSREGENERATIONBIT, MINIMAPUNITSREFRESHCYCLE, 0, 0, 1);
	MakeQueueAction(SELFCREATEACTION, SPECIALCREEPID, &NEEDTOCHANGECREEPBIT, CHANGECREEPCYCLE, 0, 0, 1);
}
//==========================
void clearactionBITS(void)
{
	MAPREGENERATIONBIT = 0;
	MAPUNITSREGENERATIONBIT = 0;
	NEEDTOCHANGECREEPBIT = 0;
}
//==========================
#define MOUSEDESTELEVATION	255
int gogame(struct mapinfo *info)
{
	int resettimerforplayers, prevgameticks = -1;
	int activatedwaittimer;

	int pc;
	SCREEN_REGION	lastmenuregion;
	int retmenu;
	int i, retnet, timeid;
	char desenonlymouseflag;
	changegoods = 1;
	highMouse->LoadAllCursors();
	showedmenu.Init();
	highMouse->SetRestrictCoords(MOUSEMODE2);
	chatbar.clearallmessages();
	infobar.clearallmessages();
	GAME = 1;
	PAUSEGAME = 0;
	PAUSEINTRIG = 0;
	commandqueuetick = 0;
	for (i = 0;i < PLAYEDPLAYERS;i++)
		map.clearfog[i] = 1;
	UnitsMAPRefresh();
	regenObjMap->OpenMapByRange(0, MaxObjects - 1);
	calcfullinvandsee();
	bitsplayer = GetVisionBitsPlayer(NUMBGAMER);
	MAPREGENERATIONBIT = 1;
	MAPUNITSREGENERATIONBIT = 1;
	calculatefog(bitsplayer);			//calculate all fog
	drawMAP(1);

	highMouse->MouseOnObjClear();
	destCursor = new DestCursor();
	screenDraw = new ScreenDraw();
	//totalimgs = 0;
	//drawedimgs = 0;
	AllImages_Draw();
	//DEBUGMESSCR("At begin totalimgs=%d drawedimgs=%d\n", totalimgs, drawedimgs);		//to check for leaks

	putfog();			//show fogofwar
	drawMINIMAP();
	showramka();
	mainController.UpdateScreen();
	setmainscreenmouseevents();
	//make minimap regeneration queue
	gamecycle = 0;
	pregameQUEUE();

	regenObjMap->Init();

	netplay.MakeFirstNetworkCycle();
	menustatus = CONTINUEGAME;
	resettimerforplayers = 1 << 16;
	activatedwaittimer = 0;
	gamestatus = NOGAMESTATUS;
	//	  gameconf.speedconf.gamespeed=SPEED6X;
	clearopenseeKarta();
	clearopenseeKarta();
	karta_aria = 0;
	select_aria = 0;
	movieminikarta = false;


	do {
		screenDraw->ClearRegions();
		needredesen = 0;
		clearactionBITS();
		mainController.EventsLoop();


		screenMapInfo->ScrollX = 0;
		screenMapInfo->ScrollY = 0;

		if (needredesen)
			redesenscreen();
		if (!PAUSEGAME && !PAUSEINTRIG)
		{
			mainController.KeyPressRefresh();					//refresh array of keys
			highMouse->RefreshMouseType(map.MAPXGLOBAL, map.MAPYGLOBAL);
			mouseonkartaarea();
		}
		if (PAUSEINTRIG)
		{
			mainController.KeyActive = 0;
			mainController.LastKey = 0;
		}
		timeid = mytimer.TimeIsCome(gameconf.speedconf.gamespeed);
		if (PAUSEGAME)
		{
			timeid = TIMETONOSLEEP;
		}
		if (timeid == TIMETOACT)//|| timeid==TIMETONOSLEEP)
		{
			scrnew = 1;
		}
		else
		{
			scrnew = 0;
		}
		retnet = netplay.DoNetworkGameCycle();
		if (retnet)
		{
			//show wait menu
			printf("error! wait players!!!retnet=%x\n", retnet);
			/*		if (!activatedwaittimer)
					{
					//show waitingplayermenu
					activatedwaittimer=1;
					mp=new MENUPARAMS(NULL,resettimerforplayers|retnet);//set timer 60sec once per game
					resettimerforplayers=0;
					showedmenu.prepareforshowmenu(&WaitingPlayersMenu,mp);
					}
			*/
		}
		else
		{
			if (activatedwaittimer)
			{
				//close waitingplayermenu
				activatedwaittimer = 0;
				drawmenu_LASTTICK(&showedmenu);
			}
		}
		if (movieminikarta && karta_aria)
			scrnew = 1;
		if (scrnew && !retnet)
		{
			keyhandler();		//analyze if some key action
			karta_aria = (mousehotpos == MOUSEONMINIMAP);
			select_aria = (mousehotpos == MOUSEONMAP);
			if (movieminikarta && karta_aria)
				highMouse->SetRestrictCoords(MOUSEMODE4);
			else
				highMouse->SetRestrictCoords(MOUSEMODE2);
			movieminikarta = false;
			switch (timeid)
			{
			case TIMETONOSLEEP:
				break;
			case TIMETOACT:		//timetoprocess
				if (!PAUSEGAME && !PAUSEINTRIG)
				{
					//				printf("aloc=%d max=%d maxmark=%d\nn",mainimageslist.allocatedelem,mainimageslist.totalelem,mainimageslist.totalmarked);
					QueueGame.QueueMain(INCREMENTTICKS);

					UnitsCommandQueue.QueueMain(commandqueuetick);
					GoNextCommandTick();

					weaponflingy.MoveAllWeaponFlingy();

					AllImages_ExecuteIScript();

					AllOBJMoving();

					AdditionalUnitsProceed();

					invisiblestick();
					if (MINIMAPREFRESHCYCLE)
						applyrescuableunits();

					if (destCursor)
						destCursor->ExecuteScript();
					if (TRIG_inittriggers)
						SearchForAtacks();	//search unit to atack unit if triggers activated

					allobjdecrmtimemage();

					allobj_dieheal();

					allobjconstr();
					if (NEEDTOCHANGECREEPBIT)
					{
						RemoveAddOneCreep();
					}
					regenObjMap->OpenMapCycle();
				}
				IfTimeForTrigger(info, &prevgameticks);
				if (!PAUSEGAME && !PAUSEINTRIG)
				{
					revealMap->AdditionalOpen();	//TODO need to remove objs if they exist in diapazone[nrobjregen] :)
					if (MAPREGENERATIONBIT)
					{
						calcfullinvandsee();
					}
				}
				gamecycle++;
				break;
			case TIMETOSLEEP:
				usleep(CNTTIMETOSLEEP);
				break;
			default:
				break;
			}//switch timeid
			if (gamestatus != NOGAMESTATUS)
			{
				highMouse->SetRestrictCoords(MOUSEMODE2);
			}
			AutoMoveMap();
			if (!PAUSEGAME)
			{
				calculatefog(bitsplayer);			//calculate all fog
				drawMAP(0);
				showStars(map.MAPXGLOBAL, map.MAPYGLOBAL);//show stars if spacemap

				CreatePylonSelectArea();
				RemovePylonSelectArea();

				weaponflingy.DeleteMarked();

				AllImages_DeleteMarked();		//destroy images previously marked

				DestroyMarked();			//destroy unit previously marked

				//totalimgs = 0;
				//drawedimgs = 0;
				highMouse->MouseOnObjClear();
				AllImages_Draw();

				DrawBuildPlace();

				highMouse->GetMouseOnObj();

				putobjsonminimap();
				if (MAPUNITSREGENERATIONBIT && (MAPDEF & UNITS))
					minimap_showobjs();

				putfog();			//show fogofwar
				showgoods();
				ShowLeaderBoards(&map, 5, 15);
				ShowCountDownTimer(&map, 290, 15);
				pc = desenbuildifconstr();
				if (pc != 0)
					highMouse->Construct.CanDo = pc;
				highMouse->DrawSelectionArea();
				if (destCursor)
					destCursor->DrawDestinationCursor();
				showramka();

				drawMINIMAP();			//put minimap

				MinimapImages_Draw();

				DetectIfAnyPylonOnSelected();
				//analize properties of selected objects
				CreateMenuProperties(properties, selectableproperties, NUMBGAMER);

				desenproperties(properties, selectableproperties);//draw properties of selected unit(s) on screen
			}
			makeallblinking();
			showportrait();
			drawGAMEMENUbutton(&dipl_menu, minimapmenu, MINIMAPDIALOG_DIPLOMACYBUTTON,
				DIPLOMACYBUTTON_PICT, MOUSEONDIPLOMACYBUTTON, STATTXT_TBL_DIPLOMACY);
			drawGAMEMENUbutton(&terr_menu, minimapmenu, MINIMAPDIALOG_TERRAINBUTTON,
				TERRAINBUTTON_PICT, MOUSEONTERRAINBUTTON, STATTXT_TBL_TERRAINONTEXT);
			drawGAMEMENUbutton(&mess_menu, minimapmenu, MINIMAPDIALOG_MESSAGEBUTTON,
				MESSAGEBUTTON_PICT, MOUSEONMESSAGEBUTTON, STATTXT_TBL_MESSAGING);
			drawGAMEMENUbutton(&f10_menu, f10menu, F10DIALOG_MENUBUTTON,
				F10BUTTON_PICT, MOUSEONMENUBUTTON, STATTXT_TBL_GAMEMENUTEXT);
			if (!PAUSEGAME)
				printobjparam();		//draw parameters of selected objects
			desenonlymouseflag = 0;
			//update entirescreen

			screenDraw->AddRegion(0, 0, 0, gameconf.grmode.x, gameconf.grmode.y);
			screenDraw->SetRegions(1);
		}
		else//scrnew
		{
			menustatus = CONTINUEGAME;
			if (PAUSEGAME)
			{
				desenonlymouseflag = 0;
				screenDraw->AddRegion(0, 0, 0, gameconf.grmode.x, gameconf.grmode.y);
				screenDraw->SetRegions(1);
			}
			else
			{
				desenonlymouseflag = 1;
				//update mousecursor
				screenDraw->AddRegion(0,
					highMouse->SavedUnder.PosX,
					highMouse->SavedUnder.PosY,
					highMouse->SavedUnder.SizeX,
					highMouse->SavedUnder.SizeY);
				screenDraw->SetRegions(1);
			}
		}
		ShowGameStatusMenu(&prevgameticks);
		if (!PAUSEINTRIG)
		{
			if (!activatedwaittimer)
			{
				//not show any menus if waitplayermenu activated
				ShowButtonMenu(&f10_menu, &putgamemenu);
				ShowButtonMenu(&dipl_menu, &diplomacymenu);
				if (ShowButtonMenu(&terr_menu, NULL))
					ChangeTerrain();
				ShowButtonMenu(&mess_menu, &messagingmenu);

				//showF10menu(&f10_menu);
				//showDIPLOMACYmenu(&dipl_menu);
				//showTERRAINmenu(&terr_menu);
				//showMESSAGEmenu(&mess_menu);
			}
		}
		retmenu = 0;
		if (scrnew && !retnet)
			drawmonoifpaused();

		if (scrnew)
		{
			chatbar.showallmessages(5 + DELTASCREENX, 125 + DELTASCREENY, 440, 13);
			infobar.showallmessages(150 + DELTASCREENX, 290 + DELTASCREENY, 340, 12);
		}
		if (showedmenu.IfCanShowMenu())
		{
			karta_aria = 0;
			select_aria = 0;
			movieminikarta = false;
			mousehotpos = MOUSEONNONE;
			highMouse->WaitToPressLeftButton = 0;
			highMouse->MouseOnSelectionMode = 0;
			highMouse->DestMouseOBJ = NULL;
			highMouse->MouseType = NORMALMOUSE;
			retmenu = showedmenu.ShowMenu();
			mainController.KeyActive = 0;
			menustatus = ChangeMenuStatus(menustatus);
			if (retmenu)
			{
				if (desenonlymouseflag)
				{
					lastmenuregion.x = showedmenu.menutodraw->x;
					lastmenuregion.y = showedmenu.menutodraw->y;
					lastmenuregion.w = showedmenu.menutodraw->xsize;
					lastmenuregion.h = showedmenu.menutodraw->ysize;
				}
				retmenu = drawmenu_LASTTICK(&showedmenu);
				if (desenonlymouseflag)
				{
					if (!showedmenu.menutodraw)
					{
						screenDraw->CloneRegion(1, 0);
						screenDraw->AddRegion(0, &lastmenuregion);
						screenDraw->SetRegions(2);
					}
				}
			}
			if (desenonlymouseflag)
			{
				//if only mouse region update
				//add and menu region update
				if (showedmenu.menutodraw)
				{
					screenDraw->CloneRegion(1, 0);
					screenDraw->AddRegion(0,
						showedmenu.menutodraw->x,
						showedmenu.menutodraw->y,
						showedmenu.menutodraw->xsize,
						showedmenu.menutodraw->ysize);
					screenDraw->SetRegions(2);
				}
			}
		}
#ifdef DEBUG
		if (SHOWCELLS)
			_putcells();		//put borders
#endif
	//	saveunderpatr();
		highMouse->SaveImageUnder();
		//	desenpatr();
		highMouse->DrawMouse();
		screenDraw->TopMessage();
		screenDraw->UpdateScreen();
		//wscreenonmem(scrregions,scrparts);
		highMouse->LoadImageUnder();
		//	loadunderpatr();
		showedmenu.EndDrawMenu();
		if (retmenu)
		{
			showedmenu.EndShowMenu();
		}
		//	DEBUGMESSCR("%d\n",MENUACTIVE);
		if (mainController.RefreshAtEnd & scrnew)
		{
			mainController.RefreshAtEnd = 0;
			scrnew = 0;
		}
		if (gamestatus != NOGAMESTATUS)
			PAUSEGAME = 1;

	} while (menustatus == CONTINUEGAME);
	if (ALLMENUS.Count() > 0)
		DEBUGMESSCR("REMAIN ALLMENUS lists\n");
	ALLMENUS.DeallocList();
	netplay.DeInitNetworkTicks();
	deletemainscreenmouseevents();
	GAME = 0;
	highMouse->UnloadCursors();
	scrnew = 0;

	AllImages_FreeAndEmpty();
	delete screenDraw;
	if (destCursor)
	{
		delete destCursor;
		destCursor = NULL;
	}
	return(menustatus);
}
//==========================
int ChangeMenuStatus(int mstatus)
{
	switch (mstatus)
	{
	case BECOMEOBSERVER:
		MakeObserver(NUMBGAMER);
		return(CONTINUEGAME);
	case MAINMENUSTATUS_LOADGAMEOK:
		//not done yet
		return(CONTINUEGAME);
	case MAINMENUSTATUS_LOADGAMEFAILED:
		return(CONTINUEGAME);
	case MAINMENUSTATUS_SAVEGAMEFAILED:
		return(CONTINUEGAME);
	}
	return(mstatus);
}
//==========================
void clearplayersconfig(void)
{
	for (int i = 0;i < PLAYEDPLAYERS;i++)
	{
		gameconf.pl_race[i] = RACE_INDEPENDENT;		//random
		gameconf.pl_owner[i] = OWNER_COMPUTER;		//computer
	}
}
//==========================
void clearallplayerinfo(void)
{
	memset(&PLAYER, 0, sizeof(PLAYER));
}
//==========================
void drawGAMEMENUbutton(char *button, DIALOGBIN_INFO *menuinfo, int buttonnr,
	int dlggrp_firstpict, int mousehotnr, int stattxt_nr)
{
	int color, press = 0;
	if (menuinfo->iteminfo[buttonnr].Flags&DIALOGBIN_FLAGS_ITEMVISIBLED)
	{
		if (menuinfo->iteminfo[buttonnr].Flags & DIALOGBIN_FLAGS_ITEMDISABLED)
		{
			putgrp_nopacked(menuinfo->iteminfo[buttonnr].xpos + menuinfo->iteminfo[0].xpos,
				menuinfo->iteminfo[buttonnr].ypos + menuinfo->iteminfo[0].ypos,
				gamedlggrp, dlggrp_firstpict + MENUBUTTON_DISABLED);
			color = GGREYCOLORFONT;
		}
		else
		{
			if (mousehotpos == mousehotnr && !highMouse->MouseOnSelectionMode)
			{
				if (highMouse->GetButtonStatus() & WMLEFTKEY)
				{
					*button = GAMEBUTTON_MOUSEPRESS;
					*button &= ~GAMEBUTTON_MOUSERELEASE;
				}
				else
					if (*button & GAMEBUTTON_MOUSEPRESS)
					{
						*button &= ~GAMEBUTTON_MOUSEPRESS;
						*button |= GAMEBUTTON_MOUSERELEASE;
					}
			}
			else
			{
				*button &= ~GAMEBUTTON_MOUSEPRESS;
				*button &= ~GAMEBUTTON_MOUSERELEASE;
			}
			if ((mousehotpos == mousehotnr && !highMouse->MouseOnSelectionMode) || (*button & GAMEBUTTON_KEYPRESS))
			{
				if ((highMouse->GetButtonStatus() & WMLEFTKEY) || (*button & GAMEBUTTON_KEYPRESS))
				{
					putgrp_nopacked(menuinfo->iteminfo[buttonnr].xpos + menuinfo->iteminfo[0].xpos,
						menuinfo->iteminfo[buttonnr].ypos + menuinfo->iteminfo[0].ypos,
						gamedlggrp, dlggrp_firstpict + MENUBUTTON_CLICKED);

					*button |= GAMEBUTTON_SHOW;
					press = 1;
				}
				else
				{
					putgrp_nopacked(menuinfo->iteminfo[buttonnr].xpos + menuinfo->iteminfo[0].xpos,
						menuinfo->iteminfo[buttonnr].ypos + menuinfo->iteminfo[0].ypos,
						gamedlggrp, dlggrp_firstpict + MENUBUTTON_ENABLED);
					putboxmessage(FONTCOLOR(tfontgamp, GBLUECOLORFONT, 2),
						FONTCOLOR(tfontgamp, GBLUECOLORFONT, 4),
						menuinfo->iteminfo[buttonnr].xpos + menuinfo->iteminfo[0].xpos + menuinfo->iteminfo[buttonnr].xsize,
						menuinfo->iteminfo[buttonnr].ypos + menuinfo->iteminfo[0].ypos + 12,
						2, IDFONT8, alldattbl.stattxt_tbl->get_TBL_STR(stattxt_nr),
						GBLUECOLORFONT, tfontgamp, gamedlggrp);
				}
				color = GWHITECOLORFONT;
			}
			else
			{
				color = GYELLOWCOLORFONT;
				putgrp_nopacked(menuinfo->iteminfo[buttonnr].xpos + menuinfo->iteminfo[0].xpos,
					menuinfo->iteminfo[buttonnr].ypos + menuinfo->iteminfo[0].ypos,
					gamedlggrp, dlggrp_firstpict + MENUBUTTON_ENABLED);
			}
		}//else disabled
		if (menuinfo->iteminfo[buttonnr].text)
			putmessage(menuinfo->iteminfo[buttonnr].xtextpos + menuinfo->iteminfo[0].xpos + press * 2,
				menuinfo->iteminfo[buttonnr].ytextpos + menuinfo->iteminfo[0].ypos - press * 2,
				menuinfo->iteminfo[buttonnr].fontnr,
				menuinfo->iteminfo[buttonnr].text, color, tfontgamp, gamedlggrp);
	}//visibled
}
//==========================
void ChangeTerrain(void)
{
	MAPDEF++;
	if (MAPDEF > 3)
		MAPDEF = 2;
	switch (MAPDEF)
	{
	case 2://only units
		setmemd(Minimap, MAXIMSIZEMINIMAP*MAXIMSIZEMINIMAP / 4, 0);
		break;
	case 3:// all infos
		drawMINIMAP();
		break;
	}

}
//==========================
bool ShowButtonMenu(char *button, int(*adrmenu)(MENUDRAW *menudraw, MENUPARAMS *menuparams))
{
	if ((*button & GAMEBUTTON_SHOW) && (*button & GAMEBUTTON_RELEASE))
	{
		*button = GAMEBUTTON_CLEARALL;
		if (adrmenu)
			showedmenu.prepareforshowmenu(adrmenu, NULL);
		return true;
	}
	return false;
}
//==========================
void showramka(void)
{
	putstasprite8(DELTASCREENX, 0, packedconsole);
}
//==========================
void showiconramka(void)
{
	putstasprite8(DELTASCREENX, DELTASCREENY, packedconsoleover);
}
//==========================
void gameend(const char *mes)
{
	logend();
	printf("%s\n", mes);
	if (map.valid_vcode)
		unload_starmap(&map);

	deletemainscreenmouseevents();
	highMouse->UnloadCursors();
	delete highMouse;
	delete screenMapInfo;
	//	  freeOBJstructs(1);
	//	  freeMAGEstructs();
	usleep(300000);//wait to finish playng sounds
	deinit();
	deinitsoundengine();
	unloadcircles();
	unloadweapons();
	//	  UnloadAllOverlaysGRP();
	unloadznak();
	UnloadDatTblFiles(&alldattbl);
	unloadallfonts();
	UnloadVisionBinTables();

	//	  unloadtexturegrp();
	//	  Unload_SC_Images_List();
	QuitGrpLib();
	mainController.QuitVideoMode();
	mainController.DeInit();
	UnLoadAllMpqs();
	unloadcfg();
	exit(0);
}
//===================================================
#include "walk.h"
void logend(void)
{
	static int filenr = 0;
	char fn[200];
	int i, j, ii, jj, k;
	unsigned short kk;
	FILE *f;
	/*	  f=fopen("MAPvision0.txt","wb");
		for (i=0;i<MAXVISY;i++)
		{
			for (j=0;j<MAXVISX;j++)
			{
			fprintf(f,"%02x",MAPvision[0][i][j]);
		}
		fprintf(f,"\n");
		}
		fclose(f);
	*/
	/*	  f=fopen("pylonarea.txt","wb");
		for (i=0;i<MAXYMAP;i++)
		{
		for (j=0;j<MAXXMAP;j++)
		{
			fprintf(f,"%03d ",map.pylonarea[2][i*MAXXMAP+j]);
		}
		fprintf(f,"\n");
		}
		fclose(f);
		f=fopen("seedetector.txt","wb");
		for (i=0;i<MAXYMAP;i++)
		{
		for (j=0;j<MAXXMAP;j++)
		{
			fprintf(f,"%03d ",map.mapbits.seedetector[i*MAXXMAP+j]);
		}
		fprintf(f,"\n");
		}
		fclose(f);
		f=fopen("whitefog.txt","wb");
		for (i=0;i<MAXYMAP;i++)
		{
		for (j=0;j<MAXXMAP;j++)
		{
			fprintf(f,"%03d ",map.mapbits.whitefog[1][i*MAXXMAP+j]);
		}
		fprintf(f,"\n");
		}
		fclose(f);
	*/
	/*	  f=fopen("whitefog.txt","wb");
		for (i=0;i<MAXYMAP;i++)
		{
		for (j=0;j<MAXXMAP;j++)
		{
			fprintf(f,"%02d ",map.mapbits.whitefog[1][i*MAXXMAP+j]);
		}
		fprintf(f,"\n");
		}
		fclose(f);
		f=fopen("blackfog.txt","wb");
		for (i=0;i<MAXYMAP;i++)
		{
		for (j=0;j<MAXXMAP;j++)
		{
			fprintf(f,"%02d ",map.mapbits.blackfog[1][i*MAXXMAP+j]);
		}
		fprintf(f,"\n");
		}
		fclose(f);

		f=fopen("screep.txt","wb");
		for (i=0;i<MAXYMAP;i++)
		{
		for (j=0;j<MAXXMAP;j++)
		{
			fprintf(f,"%03d ",map.mapbits.savedcreep[NUMBGAMER][i*MAXXMAP+j]);
		}
		fprintf(f,"\n");
		}
		fclose(f);
		f=fopen("txt.elevation","wb");
		for (i=0;i<MAXYMAP;i++)
		{
		for (j=0;j<MAXXMAP;j++)
		{
			fprintf(f,"%01d",map.maplevel[i*MAXXMAP+j]);
		}
		fprintf(f,"\n");
		}
		fclose(f);
	*/
	/*	  f=fopen("txt.creepflagplace","wb");
		for (i=0;i<MAXYMAP;i++)
		{
		for (j=0;j<MAXXMAP;j++)
		{
			fprintf(f,"%01d",map.creepflagplace[i*MAXXMAP+j]);
		}
		fprintf(f,"\n");
		}
		fclose(f);
	*/
	/*
		f=fopen("txt.creepflagneed","wb");
		for (i=0;i<MAXYMAP;i++)
		{
		for (j=0;j<MAXXMAP;j++)
		{
			fprintf(f,"%01d",map.creepflagneeded[i*MAXXMAP+j]);
		}
		fprintf(f,"\n");
		}
		fclose(f);
	*/

	/*	  f=fopen("txt.buildswalk","wb");
		for (i=0;i<MAXYMAP;i++)
		{
		for (j=0;j<MAXXMAP;j++)
		{
			fprintf(f,"%01d",map.buildswalk[i*MAXXMAP+j]);
		}
		fprintf(f,"\n");
		}
		fclose(f);
	*/
	/*	  f=fopen("txt.display_map.1","wb");
		for (i=0;i<MAXYMAP;i++)
		{
		for (j=0;j<MAXXMAP;j++)
		{
			fprintf(f,"%04x ",map.display_map[i*MAXXMAP+j]);
		}
		fprintf(f,"\n");
		}
		fclose(f);

		sprintf(fn,"txt.walk.%d",filenr++);
		f=fopen(fn,"wb");
		for (i=0;i<MAXYMAP8;i++)
		{
		for (j=0;j<MAXXMAP8;j++)
			fprintf(f,"%01d",map.mapwalk[i*MAXXMAP8+j]);
		fprintf(f,"\n");
		}
		fclose(f);
	*/
}
//===================================================
void ShowGameStatusMenu(int *prevgameticks)
{
	if (gamestatus != NOGAMESTATUS)
	{
		if (gamestatusremainticks != 0)
		{
			int gameticks;
			gameticks = mytimer.GetCurrentGameTimeTick();
			if (gameticks == *prevgameticks)
				return;
			*prevgameticks = gameticks;
			if (--gamestatusremainticks == 0)
			{
				StopMusic(MUSIC_STOPWITHFADE);
				if (gamestatus == WINGAME)
				{
					Play_sfxdata_id(NULL, SFXDATA_YOUWIN, 1, 0);
					showedmenu.prepareforshowmenu(&wmission, NULL);
				}
				else
				{
					if (gamestatus == DEFEATGAME)
					{
						Play_sfxdata_id(NULL, SFXDATA_YOULOSE, 1, 0);
						showedmenu.prepareforshowmenu(&lmission, NULL);
					}
				}
			}
		}
	}

}
//===================================================
int IfTimeForTrigger(struct mapinfo *info, int *prevgameticks)
{
	int gameticks;
	gameticks = mytimer.GetCurrentGameTimeTick();
	if (gameticks - *prevgameticks >= MAXGAMECYCLESPERSECOND / 2)
	{
		*prevgameticks = gameticks;
		if (info->flags & STARMAP_FLAG_HAVECOUNTDOWN)
		{
			if (!(info->flags & STARMAP_FLAG_COUNTSTOPED))
			{
				map.CountDownTimer -= MAXGAMECYCLESPERSECOND / 2;
				if (map.CountDownTimer <= 0)
				{
					map.CountDownTimer = 0;
					info->flags &= ~STARMAP_FLAG_HAVECOUNTDOWN;
				}
			}
		}
		//		if (gamestatus == NOGAMESTATUS)
		if (!TRIG_stopalltriggers)
		{
			Triggers_Parce(info, info->TRIGS_NR, info->TRIGS, 1000 / 2);
		}
		CalcLeaderBoards(&map);
		return(1);
	}
	return(0);
}
//===================================================
void ShowFirstRunVideo(int ignorefirstrunbit)
{
	if (!EXPANSIONSET)
	{
		if (!(gameconf.videoconf.preview & 0x01) || ignorefirstrunbit)
		{
			PlayVideoSmk("smk\\starintr.smk");
			if (!ignorefirstrunbit)
			{
				gameconf.videoconf.preview |= 0x01;
				saveconf();
			}
		}
	}
	else
	{
		if (!(gameconf.videoconf.preview & 0x02) || ignorefirstrunbit)
		{
			PlayVideoSmk("smk\\starxintr.smk");
			if (!ignorefirstrunbit)
			{
				gameconf.videoconf.preview |= 0x02;
				saveconf();
			}
		}
	}
}
//===================================================
void ActivateCheat(int cheatid)
{
	int i, fogvalue;
	CHEATERBIT = 1;
	DEBUGMESSCR("cheat nr %d\n", cheatid);
	OBJ *o;
	switch (cheatid)
	{
	case 0:
		break;
	case 1:		//showall
		CODEFORSCREEN = CODEFORSCREEN ^ 1;
		changegoods = 1;
		map.clearfog[NUMBGAMER] = 1;
		bitsplayer = GetVisionBitsPlayer(NUMBGAMER);
		MAPREGENERATIONBIT = 1;
		MAPUNITSREGENERATIONBIT = 1;
		if (CODEFORSCREEN)
		{
			fogvalue = 0x04;
		}
		else
		{
			fogvalue = 0x00;
		}
		for (i = 0;i < PLAYEDPLAYERS;i++)
		{
			if (IfPlayerHaveStartLocation(&map, i) != -1)
			{
				memset(map.mapbits.whitefog[i], fogvalue, map.map_width*map.map_height);
				memset(map.mapbits.whitefog2[i], fogvalue, map.map_width*map.map_height);
				if (fogvalue)
					memset(map.mapbits.blackfog[i], fogvalue, map.map_width*map.map_height);
			}
		}
		break;
	case 2:		//givemin
		changegoods = 1;
		ChangeResourcePlayer(NUMBGAMER, PLUSFACTOR, 1000, 0);
		break;
	case 3:		//givegas
		changegoods = 1;
		ChangeResourcePlayer(NUMBGAMER, PLUSFACTOR, 0, 1000);
		break;
	case 4:		//quickmake
		CODEFORQUICKMAKE = 32;
		break;
	case 5:		//nogoods
		CODEFORWITHOUTGOODS = 1;
		break;
	case 6:		//notech
		CODEFORWITHOUTTECHTREE = 1;
		break;
	case 7:		//nopsi
		CODEFORPSIUNLIMIT = 1;
		break;
	case 8:		//showcells
		SHOWCELLS = SHOWCELLS ^ 1;
		break;
	case 9:		//kill
		SelectedUnits.EnumListInit();
		while ((o = (OBJ *)SelectedUnits.GetNextListElem(NULL)))
		{
			dieobj(o);
		}
		break;
	case 10:	//quit
		menustatus = EXITGAME;
		break;
	}
}

