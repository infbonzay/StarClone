#if !defined(_CONST_W)
#define _CONST_W

//====================
struct DISPLAY{
       int	x;
       int	y;
       char	s;
       char	fullscreen;
       char	emulationmode;
       char	windowactive;
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
       short begin;
       short end;
};
struct POSINFILE
{
    long	int pos;
    int		filenr;
};
struct UPGRTEMP
{
    char val;
    char *string;
};
//====================
#endif

