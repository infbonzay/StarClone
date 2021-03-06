#ifndef _SC_CONST_H

#define _SC_CONST_H

//====================
#define DISPLAYFLAGS_FULLSCREENMODE		0x01
#define DISPLAYFLAGS_EMULATIONMODE		0x02
#define DISPLAYFLAGS_WINDOWACTIVE		0x04

struct DISPLAY{
	unsigned char *videobuff;
	int			x;
	int			y;
	char		s;
	char		flags;
};
//====================
struct XYcoord
{
	int x;
	int y;
};
//====================
struct XY
{
	int x1;
	int y1;
	int x2;
	int y2;
};
//====================
struct BARofMENU
{
	char *mesage;
	short x1;
	short y1;
	short yesselect;
};
//====================
struct _BE_{
	int begin;
	int end;
};

struct POSINFILE
{
	long	pos;
	int		filenr;
};
struct UPGRTEMP
{
	char val;
	char *string;
};
//====================
#endif

