
#include "starmap.h"
#include "gener.h"
#include "wmem.h"
#include "fonts.h"
#include "ScreenDraw.h"
#include "LeaderBoard.h"

//=================================================
LEADERBOARD *AddLeaderBoard(int boardnr, int leaderboardtype,
	StarMapInfo *info, int actiononplayers, int unittype, int nrofunits, int fontID, int stringID, int maxtxtsize)
{
	int i;
	//commandsymb+emptycubesymb+cubecolor+space+number(max 10 symbols)+zero termstring
	char *strtxt = getmapSTR(info, stringID);
	int strIdLen = strlen(strtxt);
	if (maxtxtsize > strIdLen)
		strIdLen = maxtxtsize;
		
	if (!info->leaderboards[boardnr])
	{
		info->leaderboards[boardnr] = (LEADERBOARD *)wmalloc(sizeof(LEADERBOARD));
		info->leaderboards[boardnr]->txtstr = (char *)wmalloc(strIdLen + 4 + 10 + 1);
	}
	info->leaderboards[boardnr]->leaderboardtype = leaderboardtype;
	info->leaderboards[boardnr]->unittype = unittype;
	info->leaderboards[boardnr]->nrofunits = nrofunits;
	info->leaderboards[boardnr]->stringID = stringID;
	info->leaderboards[boardnr]->fontID = fontID;
	if (!TRIG_leaderboardcomputerplayers)
	{
		//exclude computerplayers from calculations;
		for (i = 0;i < MAXPLAYERS;i++)
		{
			if (info->pl_iowner[i] != OWNER_HUMAN)
			{
				actiononplayers &= ~(1 << i);
			}
		}
	}
	info->leaderboards[boardnr]->actiononplayers = actiononplayers;
	return(info->leaderboards[boardnr]);
}
//=================================================
void CalcLeaderBoards(StarMapInfo *info)
{
	int i, j, mask, nrunits, ore, gas;
	for (i = 0;i < MAXLEADERBOARDS;i++)
	{
		if (info->leaderboards[i])
		{
			switch (info->leaderboards[i]->leaderboardtype)
			{
			case TRG_ACTIONTYPE_LEADERBOARDCONTROL://17
				nrunits = CheckForUnit(NULL, info->leaderboards[i]->actiononplayers, info->leaderboards[i]->unittype, 0, NULL, NULL);
				sprintf(info->leaderboards[i]->txtstr, COMMANDSYMBCHAR EMPTYCUBE14X14CHAR "%c %d %s",
					PLAYER[NUMBGAMER].colorRACE + 1, nrunits, getmapSTR(info, info->leaderboards[i]->stringID));
				info->leaderboards[i]->calcready = 1;
				break;
			case TRG_ACTIONTYPE_LEADERBOARDRESOURCES://19
				ore = 0;
				gas = 0;
				switch (info->leaderboards[i]->unittype)
				{
				case TRG_RESTYPE_MINERALS:
					for (j = 0, mask = 1;j < MAXPLAYERS;j++, mask <<= 1)
					{
						if (info->leaderboards[i]->actiononplayers & mask)
						{
							ore += PLAYER[j].minerals >> 8;
						}
					}
					sprintf(info->leaderboards[i]->txtstr, COMMANDSYMBCHAR MINERALSYMBCHAR " %d %s",
							ore, getmapSTR(info, info->leaderboards[i]->stringID));
					break;
				case TRG_RESTYPE_GAS:
					for (j = 0, mask = 1;j < MAXPLAYERS;j++, mask <<= 1)
					{
						if (info->leaderboards[i]->actiononplayers & mask)
						{
							gas += PLAYER[j].gas >> 8;
						}
					}
					sprintf(info->leaderboards[i]->txtstr, COMMANDSYMBCHAR GASSYMBCHAR " %d %s",
						gas, getmapSTR(info, info->leaderboards[i]->stringID));
					break;
				case TRG_RESTYPE_MINERALSANDGAS:
					for (j = 0, mask = 1;j < MAXPLAYERS;j++, mask <<= 1)
					{
						if (info->leaderboards[i]->actiononplayers & mask)
						{
							ore = PLAYER[j].minerals >> 8;
							gas = PLAYER[j].gas >> 8;
						}
					}
					sprintf(info->leaderboards[i]->txtstr, COMMANDSYMBCHAR MINERALSYMBCHAR " %d " COMMANDSYMBCHAR GASSYMBCHAR " %d %s",
						ore, gas, getmapSTR(info, info->leaderboards[i]->stringID));
					break;
				}
				info->leaderboards[i]->calcready = 1;
				break;
			case TRG_ACTIONTYPE_LEADERBOARDPOINTS://21
				nrunits = 0;
				for (j = 0, mask = 1;j < MAXPLAYERS;j++, mask <<= 1)
				{
					if (info->leaderboards[i]->actiononplayers&mask)
					{
						nrunits += info->units_stat.score[j][TRG_SCORETYPE_CUSTOM];
					}
				}
				sprintf(info->leaderboards[i]->txtstr, COMMANDSYMBCHAR EMPTYCUBE14X14CHAR "%c %d %s",
					PLAYER[NUMBGAMER].colorRACE + 1, nrunits, getmapSTR(info, info->leaderboards[i]->stringID));
				info->leaderboards[i]->calcready = 1;
				break;
			case TRG_ACTIONTYPE_LEADERBOARDGOALKILLS://36
				nrunits = 0;
				for (j = 0, mask = 1;j < MAXPLAYERS;j++, mask <<= 1)
				{
					if (!(info->leaderboards[i]->actiononplayers&mask))
					{
						nrunits += info->units_stat.stattype[UNITSTAT_DEATH][j][info->leaderboards[i]->unittype];
					}
				}
				nrunits = info->leaderboards[i]->nrofunits - nrunits;
				sprintf(info->leaderboards[i]->txtstr, COMMANDSYMBCHAR EMPTYCUBE14X14CHAR "%c %d %s",
					PLAYER[NUMBGAMER].colorRACE + 1, nrunits, getmapSTR(info, info->leaderboards[i]->stringID));
				info->leaderboards[i]->calcready = 1;
				break;
			case TRG_ACTIONTYPE_LEADERBOARDGOALPOINTS://37
				nrunits = 0;
				for (j = 0, mask = 1;j < MAXPLAYERS;j++, mask <<= 1)
				{
					if (info->leaderboards[i]->actiononplayers&mask)
					{
						nrunits += info->units_stat.score[j][TRG_SCORETYPE_CUSTOM];
					}
				}
				nrunits = info->leaderboards[i]->nrofunits - nrunits;
				sprintf(info->leaderboards[i]->txtstr, COMMANDSYMBCHAR EMPTYCUBE14X14CHAR "%c %d %s",
					PLAYER[NUMBGAMER].colorRACE + 1, nrunits, getmapSTR(info, info->leaderboards[i]->stringID));
				info->leaderboards[i]->calcready = 1;
				break;
			case TRG_ACTIONTYPE_OWNLEADERBOARD://250
				strncpy(info->leaderboards[i]->txtstr, screenGameInfo, sizeof(screenGameInfo));
				info->leaderboards[i]->calcready = 1;
				break;
			default:
				break;
			}
		}
	}
}
//=================================================
void ShowLeaderBoards(StarMapInfo *info, int x, int y)
{
	int i, rowsize, fontnr;
	for (i = 0;i < MAXLEADERBOARDS;i++)
	{
		fontnr = info->leaderboards[i]->fontID;
		getmaxsymbolsize(fontnr, NULL, &rowsize);
		if (info->leaderboards[i] && info->leaderboards[i]->calcready)
		{
			putmessage(x, y, fontnr, info->leaderboards[i]->txtstr, GGREENCOLORFONT, tfontgamp, gamedlggrp);
			y += rowsize + 5;
		}
	}
}
//=================================================
void ShowCountDownTimer(StarMapInfo *info, int x, int y)
{
	char timer[10];
	if (info->flags & STARMAP_FLAG_HAVECOUNTDOWN)
	{
		sprintf(timer, "%02d:%02d", info->CountDownTimer / MAXGAMECYCLESPERSECOND / 60, (info->CountDownTimer / MAXGAMECYCLESPERSECOND) % 60);
		putmessage(x, y, IDFONT16, timer, GWHITECOLORFONT, tfontgamp, gamedlggrp);
	}
}
//=================================================
void LeaderBoardFree(StarMapInfo *info)
{
	int i;
	for (i = 0;i < MAXLEADERBOARDS;i++)
	{
		if (info->leaderboards[i])
		{
			if (info->leaderboards[i]->txtstr)
			{
				wfree(info->leaderboards[i]->txtstr);
			}
			wfree(info->leaderboards[i]);
			info->leaderboards[i] = NULL;
		}
	}
}
