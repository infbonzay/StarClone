#ifndef _SC_LEADERBOARD_H

#define _SC_LEADERBOARD_H

#define MAXLEADERBOARDS					2

struct StarMapInfo;

struct LEADERBOARD
{
		unsigned char	leaderboardtype;
		unsigned char	unittype;
		unsigned int	nrofunits;
		short int		actiononplayers;
		short int		stringID; 
		char			*txtstr;
		char			calcready;
};

LEADERBOARD *AddLeaderBoard(int leaderboardnr, int leaderboardtype, StarMapInfo *info, int actiononplayer, int unittype, int nrofunits, int stringID);
void CalcLeaderBoards(StarMapInfo *info);
void ShowLeaderBoards(StarMapInfo *info,int x,int y);
void ShowCountDownTimer(StarMapInfo *info,int x,int y);

#endif

