#ifndef _SC_LEADERBOARD_H

#define _SC_LEADERBOARD_H


#define MAINLEADERBOARDGAMEINFO				0
#define MAINLEADERBOARD						1
#define MAINLEADERBOARDGOAL					2

#define MAXLEADERBOARDS						3
#define TRG_ACTIONTYPE_OWNLEADERBOARD		250

#include "starmap.h"

struct LEADERBOARD
{
		char			*txtstr;
		unsigned int	nrofunits;
		short int		actiononplayers;
		short int		stringID; 
		unsigned char	leaderboardtype;
		unsigned char	unittype;
		char			calcready;
		char			fontID;
};

LEADERBOARD *AddLeaderBoard(int leaderboardnr, int leaderboardtype,
	StarMapInfo *info, int actiononplayer, int unittype, int nrofunits, int fontID, int stringID, int strsize);
void CalcLeaderBoards(StarMapInfo *info);
void ShowLeaderBoards(StarMapInfo *info,int x,int y);
void ShowCountDownTimer(StarMapInfo *info,int x,int y);
void LeaderBoardFree(StarMapInfo *info);

#endif

