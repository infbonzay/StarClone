#if !defined(_MENU_W)
#define _MENU_W

#include "const.h"
#include "mylist.h"
#include "network_general.h"
#include "pcx.h"
#include <grplib/grp.h>

#include "smacker/smacker.h"

#ifndef FALSE
    #define FALSE 0
#endif
#ifndef TRUE
    #define TRUE 1
#endif
#ifdef WITHSDL
       #include "sdl/grsdl.h"
#endif
#ifdef UNDERDOS
       #include "dos/gr8dos.h"
       #include "dos/handlers.h"
#endif


#define ITEMSHOW_DISABLED	0
#define ITEMSHOW_NOFOCUS	1
#define ITEMSHOW_CLICK		2
#define ITEMSHOW_FOCUS		3

#define ITEMSHOW_MAXPOS		4


#define NOSELECTLIST		-1
#define NOSELECTMENUBAR		-2
#define CANCELFROMMENU		0x8000		//pressed ESC

#define CALLBACKCANCELFROMMENU	0x8001		//time out to wait from master game
#define CALLBACKCANCELFROMMENU2	0x8002		//the master game has left.
#define CALLBACKCANCELFROMMENU3	0x8003		//master press ok, lets counter;s begin 5,4,3,2,1,play
#define CALLBACKCANCELFROMMENU4	0x8004		//broadcast game info error
#define CALLBACKCANCELFROMMENU5	0x8005		//broadcast game info error
#define CALLBACKCANCELFROMMENU6	0x8006		//player already banned
#define CALLBACKCANCELFROMMENU7	0x8007		//playername already exists
#define CALLBACKCANCELFROMMENU8	0x8008		//downloadedfile with error
#define CALLBACKCANCELFROMMENU9	0x8009		//the master send go go go (let's play)

#define SELECTITEMDOUBLECLICK	0x4000

#define ISNULL		0	//Main Dialog Box
#define ISDEFAULTBUTTON	1	//Default Button(not used)
#define ISBUTTON	2	//Button
#define ISRADIOBUTTON	3	//Option Button
#define ISCHECKBOX	4	//Check box
#define ISIMAGE		5	//Image
#define ISVSCROLLBAR	6	//notused
#define ISHORIZBUTTON	7	//Horizontal Scrollbar
#define ISEDITBOX	8	//textbox
#define ISLABELLEFT	9	//label
#define ISLABELCENTER	10	//label
#define ISLABELRIGHT	11	//label
#define ISLISTBOX	12	//List box
#define ISEXPANDBOX	13	//Combo box
#define ISSMKVIDEO	14	//Button w/ Light-Up smk

#define ITEMNOONEACTIVE 	-2
#define ITEMALLDISABLED 	-3

#define SCROLLUP	0
#define SCROLLDOWN	1

#define ITEM_ONLYFREE		0
#define ITEM_RESTOREANDFREE	1
#define ITEM_NOSAVELOADUNDER	2

#define MENUFLAGS_EMPTY			0x00
#define MENUFLAGS_BYPASS		0x01

#define BF_TEXTSIMPLE			0x00
#define BF_TEXTGLOWED			0x01

#define MENUITEM_MAXSHADOWGRD		32

#define MENUITEM_TIMEACTIVE_MINVALUE	0
#define MENUITEM_TIMEACTIVE_MAXVALUE	255

#define IMAGEFLAGS_GRPIMAGE	0x01
#define IMAGEFLAGS_BOTTOMALIGN	0x02


#define	MBLUECOLORFONT		2
#define	MGREENCOLORFONT		3
#define	MYELLOWCOLORFONT	4
#define	MGREYCOLORFONT		5
#define	MWHITECOLORFONT		6
#define	MREDCOLORFONT		7

#define	MBLUECOLORFONTCHAR		"\x2"
#define	MGREENCOLORFONTCHAR		"\x3"
#define	MYELLOWCOLORFONTCHAR		"\x4"
#define	MGREYCOLORFONTCHAR		"\x5"
#define	MWHITECOLORFONTCHAR		"\x6"
#define	MREDCOLORFONTCHAR		"\x7"


#define ITEMRELATION_DISABLE	0
#define ITEMRELATION_INVISIBLE	1
#define MAXITEMRELATIONS	2

#define BUTTONTYPELEFT		0
#define BUTTONTYPEFULL		1
#define BUTTONTYPERIGHT		2
#define BUTTONTYPEMIDDLE	3

#define MBUTTONCOLOR1	((MYELLOWCOLORFONT<<24)|(MWHITECOLORFONT<<16)|(MGREENCOLORFONT<<8)|(MGREYCOLORFONT))

#define MTEXTCOLOR1	((MBLUECOLORFONT<<24)|(MBLUECOLORFONT<<16)|(MBLUECOLORFONT<<8)|(MGREYCOLORFONT))
#define MTEXTCOLORWHITE	((MWHITECOLORFONT<<24)|(MWHITECOLORFONT<<16)|(MWHITECOLORFONT<<8)|(MGREYCOLORFONT))
#define MEDITCOLOR1	((MYELLOWCOLORFONT<<24)|(MYELLOWCOLORFONT<<16)|(MYELLOWCOLORFONT<<8)|(MGREYCOLORFONT))

#define MLISTCOLOR1	((MYELLOWCOLORFONT<<24)|(MYELLOWCOLORFONT<<16)|(MGREENCOLORFONT<<8)|(MGREYCOLORFONT))
#define MCHATCOLOR	((MWHITECOLORFONT<<24)|(MWHITECOLORFONT<<16)|(MWHITECOLORFONT<<8)|(MGREYCOLORFONT))
#define MRADIOCOLOR1	((MYELLOWCOLORFONT<<24)|(MYELLOWCOLORFONT<<16)|(MGREENCOLORFONT<<8)|(MGREYCOLORFONT))

#define GETITEMCOLOR(color,nr) (  (  (color) >> ((nr)*8)  ) & 0xff)

#define MENUITEM_LISTBOX_FLUSHLIST	0
#define MENUITEM_LISTBOX_DEALLOCLIST	1
struct BAR
{
    int bar;
    int color;
};

struct MOUSEHOTSPOT
{
    int x;
    int y;
    int sizex;
    int sizey;
    int *dialogflags;
};

struct MOUSEALLSPOTS
{
    int maxhotspots;
    int curenthotspots;
    int callbackwork;
    void (*mousemovecallback)(int barnr);
    struct MOUSEHOTSPOT spot[];
};

struct MENUSTR;
struct TEXTS
{
    char *text[4];		// menu strings for this button
};
#define MAX_SMK_IN_DIALOG	3
struct SMK_ARRAY
{
    short int	posx_smk;
    short int 	posy_smk;
    char 	*smkfile;
};


struct SMKONEITEM
{
    char		*smkfile;
    smk_t 		*smk;
    unsigned int	curpos;
    unsigned int	maxpos;
    int			posx;
    int			posy;
    int			smknrframes;
    unsigned char	**smkallframes;
};

struct ALLSMKS
{
    char		totalsmks;
    SMKONEITEM		smkid[];
};
	struct LISTBOXBAR
	{
	    short int 	xt_1;		//top
	    short int 	xt_2;		

	    short int 	yt_1;		//top button top_y_coord
	    short int 	yt_2;		//top button bottom_y_coord
	    char	showtypet;
	
	    short int   yh_1;		//horizbar top_y_coord
	    short int   yh_2;		//horizbar bottom_y_coord
	    short int   yh_last;

	    short int 	yb_1;		//bottom button top_y_coord
	    short int 	yb_2;		//bottom button bottom_y_coord
	    char	showtypeb;
	    
	    char	elemtodraw;
	    short int	buttonbary1;	//y-top-position of buttonbar
	    short int	buttonbary2;	//y-bottom-position of buttonbar
	    short int   buttonbarymax;	//maximal y pos for buttonbar
	    char	showbutton;	//
	};
	struct BUTTON
	{
	    TEXTS menustr;		// menu strings for this button
	    char buttonsize;		//size in sprites (not pixels), if 0 no decoration under button, put nothing
	    char buttontype;
	    char buttonflags;
	};
	struct LISTBOX
	{
	    mylist *flist;
	    int  from;
	    int  to;
	    int  selectednr;
	    char pixelsbetweenlines;
	    char lines;
	    void (*onselectitem_callback)(MENUSTR *allmenus,int menuitemnr,int listnr);	//function run, if select item on listbox
	    unsigned char decorcolor;			//around selected list item or -1 if none
	    char scrollbar;		//0 or 1
	    int  sizexarea;
	    LISTBOXBAR	*bar;
	};
	struct EDITBOX
	{
	    char *editstr;
	    int length;
	    int cursorpos;
	    int maxsymbols;
	};
	struct TEXTLABEL
	{
	    char 	*textstr;
	    short 	rowsize;
	    short	skiplinepixels;
	};
	struct EXPANDBOX
	{
	    MENUSTR *showlist;		//menu for expand
	    int  hoty;
	    int  selectednr;
	    int  maxitem;
	    int  rowsize;
	    int  typelistshow;		//1-listdown,-1-listup
	    char arrowbottom[4];
	    char *array[4];		//first 2 byte of every elem is x,y size of expand
	    char **items;
	};
	struct CHECKBOX
	{
	    TEXTS textstr;
	    int state;			//on-off
	};
	struct RADIOBUTTON
	{
	    TEXTS textstr;
	    int state;
	    int nextradiobutton;	//itemnr of next radiobutton in this stack or -1
	    int firstinstack;		//number of first radio instack or -1 if he is first
	    PCX *pcx1;
	    PCX *pcx2;
	    int deltax;
	    int deltay;
	};
	struct HORIZBUTTON
	{
	    int metric;			//metric is number of positions in bar
	    int value;
	    int barxposition;		//from 8 to hotsizex-8
	    int sizexelem;		//size in elements
	    int sizexbar;		//size in pixels
	    char exactposition;
	    char *allsprbar;		//array of sprites from dlggrp
	    int  *xvalues;		//array of x-values of holes
	    int minxvalue;
	    int maxxvalue;
	    void (*onselectitem_callback)(MENUSTR *allmenus,int menuitemnr,int horizpos);	//function run, if select/move+click on hozributton
	};
	struct SMKVIDEO
	{
    	    struct BUTTON 	*smkbutton;
    	    char		buttonalign;
    	    short    int	smkbuttonxpos;
    	    short    int	smkbuttonypos;
	    HANDLE		hmpq;
	    struct ALLSMKS	*smks;
	};
	struct IMAGE
	{
	    HANDLE		hmpq;
	    PCX			*pcx;
	    GRPFILE		*grp;
	    int			grpnr;
	    char		*pcxfilename;
	    int 		visiblex;
	    int 		visibley;
	    char 		color1;
	    char 		color2;
	    char 		transvalue;
	    char 		flags;		//bit0 - 1 - GRP image , 0 -pcx image 
						//bit1 - 0- topalign  1 - bottom-alighn
	};

struct MENUPOS
{
    short int hotdeltax;	//of hotpos delta x & y from lefttop corner of menu
    short int hotdeltay;
    short int hotxsize;		//size x & y of hot bar
    short int hotysize;
    short int xtextpos;
    short int ytextpos;		//internal infos
    short int xtextdelta;	//of textpos delta x & y from lefttop corner of menu
    short int ytextdelta;
    short int textxsize;
    short int textysize;

    char itemtype;			//ISBUTTON,ISLISTBOX,...
    int  fontnr;		// fontid for this item
    char *savedscrunderitem;	//savedscreenunderitem
    int  restoreatexit; 	//0-norestore,1-restoreandfree
    char typeofshow;		//disabled,nofocus,click,focus
    int  showvalue;		//value 0..255 add if cursor on item and sub if cursor leave item area
    unsigned char hotkey;
    unsigned int colors4;

    int  dialogbin_flags;				//from dialogbin
    char dialogbin_smk_flags[MAX_SMK_IN_DIALOG];	//from dialogbin	
    unsigned int decorcolors4;
    char	alwayszero;
    struct RELATION
    {
	int *address[MAXITEMRELATIONS];
	int value[MAXITEMRELATIONS];
    }relation;
    union ITEM
    {
    	struct BUTTON 		*button;
	struct LISTBOX		*listbox;
	struct SCROLLBUTTON 	*scrollbutton;
	struct VERTSCROLLBAR 	*vertscrollbar;
	struct EDITBOX 		*editbox;
	struct TEXTLABEL 	*textitem;
	struct EXPANDBOX	*expandbox;
	struct CHECKBOX		*checkbox;
	struct RADIOBUTTON	*radiobutton;
	struct HORIZBUTTON	*horizbutton;
	struct SMKVIDEO		*smkvideo;
	struct IMAGE		*image;
    }item;
};
struct CALLBACKINFO
{
    mylist CallBackFunc;
    mylist CallBackData;
};

class MENUPARAMS
{
public:
    int params_int1;
    char *params_pchar1;
    int (*onselectmenuitem)(MENUSTR *allmenus,int menuitem);
    MENUPARAMS(int  (*onselectmenuitem)(MENUSTR *allmenus,int menuitem),int param1);
    MENUPARAMS(int  (*onselectmenuitem)(MENUSTR *allmenus,int menuitem),char *param1);
};

class MENUDRAW
{
    MENUPARAMS	*menuparams;
    int 	(*activatedmenu)(MENUDRAW *menudraw,MENUPARAMS *menuparams);

    MENUPARAMS	*nextmenuparams;
    int 	(*nextactivatedmenu)(MENUDRAW *menudraw,MENUPARAMS *menuparams);

public:
    MENUSTR	*menutodraw;

    void prepareforshowmenu(int (*activatedmenu)(MENUDRAW *menudraw,MENUPARAMS *menuparams),MENUPARAMS *params);
    int  IfCanShowMenu(void);
    int  ShowMenu(void);
    void EndShowMenu(void);
    void EndDrawMenu(void);
    void CloseMultiplesMenus(int nrofrecursivemenus);
    void Init(void);
};
struct MENUVARS
{
    int downmenu;
    int prevmenuonbar;
    int selectedmenu;
    int prevactiveitem;
    int activeitem;
    int activeitemchange;
    int dblclick;
    int expanditemnr;
    BAR BarChanges;
};
struct MENUSTR
{
    int x;
    int y;
    int xsize;
    int ysize;
    int elements;
    int defaultbutton;		//action on ENTER
    int defaultlistitem;	//-1 if none,
    short selectmenusnd;
    short mouseoversnd;
    int	mainmenuflags;			//dialogbin flags
    CALLBACKINFO *CallBackInfo;
    unsigned char *palette;
    GRPFILE *dlggrp;
    char *fonttable;
    char *saveunder;
    struct MENUSTR *prevmenu;
    struct MENUVARS vars;
    unsigned char incrementfactor;	//value for increase/decrease 0-255
    unsigned char menuflags;		//menu flags
    struct MENUPOS menu[];
};

#define MENUFLAGS_ALWAYSDRAW		0x01
#define MENUFLAGS_ONETICKDRAW		0x02	//only for drawmenu_ONETICK

#define ADDICONSTOLIST			0x01
#define NOADDDIRTOLIST			0x02

extern char fadeinout,scrollportrait_menu;

void setmenuflags(MENUSTR *allmenus,unsigned int flags);
void selectdecorobj(MENUSTR *allmenus,int itemnr,unsigned int colors4);
void addnewmousehotpos(void (*func)(int),int maxspots);
void delnewmousehotpos(void);
void addmousehotpos(int x,int y,int sizex,int sizey,int *dialogflags);

void incrcallbackwork(void);
void decrcallbackwork(void);
void mymousemoveevent(int x,int y);
void loadunderitem(MENUSTR *allmenus,int itemnr);
void saveunderitem(MENUSTR *allmenus,int itemnr);

char *savepartscreen(int x,int y,int sizex,int sizey);
void loadpartscreen(int x,int y,int sizex,int sizey,char *savedscreen);
void restorepartscreen(int x,int y,int sizex,int sizey,char *savedscreen);
void loadunderitems(MENUSTR *allmenus,int restoreandfree);
int  drawmenu(MENUSTR *allmenusm,int drawandcontinue);
int  showlistmenu(MENUSTR *allmenus);

void getmouseonmenubar(int value);
void unregmenucallbacks(void);
void regmenu(int x,int y,MENUSTR *allmenus,
	     void (*func)(int));
void checkanddrawmenu(MENUSTR *allmenus,int ItemChanges,int saveunder);
void drawmenuitem(MENUSTR *allmenus,int itemnr);
int  menukeys(MENUSTR *allmenus,int *pressed,int *redraw);
int  editboxaction(MENUSTR *allmenus);
void puthelpfromgame(void);
void puttransptilebox(int x,int y,int sizex,int sizey,GRPFILE *tilegrp);

void setitemrelation(MENUSTR *allmenus,int itemnr,int relation_type,int *address,int addressvalue);
int  getmapinfo(char *filename);
void menuspecialtables(MENUSTR *menu,char *fonttable,GRPFILE *dlggrp);
MENUSTR *createmenuinfo(int x,int y,int sizex,int sizey,int nrofelems,int flags);
void AddPrevMenuShowing(MENUSTR *allmenus,MENUSTR *prevmenu);

void addsoundstomenu(MENUSTR *allmenus,short snd1,short snd2);
void delmenuitem(MENUSTR *allmenus,int itemnr);
void removeallmenuinfo(MENUSTR *allmenus);

int  getexpandbox_selecteditem(MENUSTR *allmenus,int itemnr);
void getmouseonitem(int *activeitemchanges,int *activeitem);
void setmouseonitem(int activeitemchanges);

void changemenunr(MENUSTR *allmenus,int newnrofitems);
void setdefaultbutton(MENUSTR *allmenus,int nrofitem);
void restoreflag(MENUSTR *allmenus,int itemnr,int restoreflag);
void setactiveedititem(MENUSTR *allmenus,int item);
void setitemrelation(MENUSTR *allmenus,int itemnr,int *address,int addressvalue);

void setbuttonflags(MENUSTR *allmenus,int nr,int flags);
void addbuttonitem(MENUSTR *allmenus,int nr,int hotx,int hoty,
				  int hotsizex,int hotsizey,int textyoffset,char hotkey,
				  int fontnr,char *text,unsigned int colors4,int buttonsize,int buttontype);
void drawbuttonitem(MENUSTR *allmenus,int itemnr);

int  geteditboxtextsize(MENUSTR *allmenus,int nr);
char *geteditboxtext(MENUSTR *allmenus,int nr);
void changeeditboxtext(MENUSTR *allmenus,int nr,char *newstr);
void changeeditboxparam(MENUSTR *allmenus,int nr,char *beginstr,int maxsizestr);
void addeditboxitem(MENUSTR *allmenus,int nr,int hotx,int hoty,
				  int hotsizex,int hotsizey,int textx,int texty,
				  int fontnr,char *beginstr,int maxsizestr,unsigned int colors4);
void draweditboxitem(MENUSTR *allmenus,int itemnr);


void changetextitem(MENUSTR *allmenus,int nr,char *textstr);
void changetextskiplinepixels(MENUSTR *allmenus,int nr,int skiplinepixels);
void addtextitem(MENUSTR *allmenus,int nr,int labelid,int hotx,int hoty,int hotsizex,int hotsizey,int textx,int texty,
		    char *textstr,int fontnr,int rowsize,unsigned int colors4);
void drawtextitem(MENUSTR *allmenus,int itemnr);


void changeexpanditemnr(MENUSTR *allmenus,int nr,int selectednr);
int  getexpanditems(MENUSTR *allmenus,int nr,char *charset);
void addexpanditem_lists(MENUSTR *allmenus,int nr,int selectednr,char *charset);
void addexpanditem(MENUSTR *allmenus,int nr,int selectednr,int fontnr,int rowsize,unsigned int colors4);
void addexpanditem(MENUSTR *allmenus,int nr,int hotx,int hoty,int hotsizex,int hotsizey,int selectednr,
			    int fontnr,int rowsize,unsigned int colors4);
void drawexpanditem(MENUSTR *allmenus,int itemnr);


void addcheckboxitem(MENUSTR *allmenus,int nr,int hotx,int hoty,int hotsizex,int hotsizey,int textx,int texty,char hotkey,
		    char *textstr,int fontnr,unsigned int colors4,int state);
void changecheckboxstate(MENUSTR *allmenus,int itemnr);
void setcheckboxstate(MENUSTR *allmenus,int itemnr,int value);
int  getcheckboxstate(MENUSTR *allmenus,int itemnr);
void drawcheckboxitem(MENUSTR *allmenus,int itemnr);

int addradiobuttonimg(MENUSTR *allmenus,int nr,PCX *pcx1,PCX *pcx2,int deltax,int deltay);
int addradiobuttonitem(MENUSTR *allmenus,int nr,int hotx,int hoty,int hotsizex,int hotsizey,int textx,int texty,char hotkey,
		    char *textstr,int fontnr,unsigned int colors4,int firstinstack);
void drawradiobuttonitem(MENUSTR *allmenus,int itemnr);
void setradiobuttonstate(MENUSTR *allmenus,int itemnr);
int  getradiobuttonstate(MENUSTR *allmenus,int itemnr);

void addhorizbutton_params(MENUSTR *allmenus,int nr,int maxpos,int nrofholes,int spacebetweenholes,
		    void (*func)(MENUSTR *allmenus,int menuitemnr,int horizpos));
void addhorizbutton_params(MENUSTR *allmenus,int nr,int maxpos,int nrofholes,
		    void (*func)(MENUSTR *allmenus,int menuitemnr,int horizpos));
void addhorizbutton(MENUSTR *allmenus,int nr,int hotx,int hoty,int hotsizex,int hotsizey,unsigned int colors4,int maxpos,int nrofholes,
		    void (*func)(MENUSTR *allmenus,int menuitemnr,int horizpos));
void drawhorizbutton(MENUSTR *allmenus,int itemnr);
void sethorizbuttonposfrommouse(MENUSTR *allmenus,int itemnr,int hotxpos);
void sethorizbuttonpos(MENUSTR *allmenus,int itemnr,int value);
int  gethorizvalue(MENUSTR *allmenus,int itemnr);
void addsmkvideo(MENUSTR *allmenus,int nr,int hotx,int hoty,int sizex,int sizey,int nrofsmks,SMK_ARRAY *arrayofsmks);
void addbuttontosmk(MENUSTR *allmenus,int nr,int xpos,int ypos,int buttonareaxsize,int buttonareaysize,
				  int texty,char hotkey,
				  int fontnr,char *text,unsigned int colors4,int buttonsize,int buttontype,int buttonflag);
void drawsmkvideo(MENUSTR *allmenus,int itemnr);
void drawsmkbuttonitem(MENUSTR *allmenus,int itemnr,BUTTON *button,int xpos,int ypos);
void setsmkflags(MENUSTR *allmenus,int itemnr,int smknr,unsigned char smkflags);
void changeimageitem(MENUSTR *allmenus,int nr,char *pcxfile);
void addimageitem(MENUSTR *allmenus,int nr);
void addimageitem(MENUSTR *allmenus,int nr,int hotx,int hoty,int sizex,int sizey,char *pcxfile);
void drawimageitem(MENUSTR *allmenus,int itemnr);
void drawdecorate(MENUSTR *allmenus,int itemnr);
void setincrementvalue(MENUSTR *allmenus,unsigned char value);

void delbuttonitem(MENUSTR *allmenus,int i,int fullempty);
void delscrollbuttonitem(MENUSTR *allmenus,int i,int fullempty);
void delvertscrollbaritem(MENUSTR *allmenus,int i,int fullempty);
void deleditboxitem(MENUSTR *allmenus,int i,int fullempty);
void deltextitem(MENUSTR *allmenus,int i,int fullempty);
void delexpanditem(MENUSTR *allmenus,int i,int fullempty);
void delcheckboxitem(MENUSTR *allmenus,int i,int fullempty);
void delradiobuttonitem(MENUSTR *allmenus,int i,int fullempty);
void delhorizbuttonitem(MENUSTR *allmenus,int i,int fullempty);
void delsmkvideoitem(MENUSTR *allmenus,int i,int fullempty);
void delsmkbuttonitem(MENUSTR *allmenus,int i,int fullempty);
void updateimageitem(MENUSTR *allmenus,int nr,char *bufpixels);
void delimageitem(MENUSTR *allmenus,int i,int fullempty);
void addimagearray(MENUSTR *allmenus,int nr,int hotx,int hoty,int sizex,int sizey);
void changegrpitem(MENUSTR *allmenus,int nr,GRPFILE *grp,int grpnr);
void setimgtranslucencyvalue(MENUSTR *allmenus,int nr,char trans);
void setimgtransparentcolors(MENUSTR *allmenus,int nr,char color1,char color2);
void setimageitem_visibilityxy(MENUSTR *allmenus,int nr,int visiblex,int visibley);
void addmenupalette(MENUSTR *allmenus,unsigned char *palette);
int  MenuFindFirstElem(MENUSTR *allmenus,int typeelem);
void setitemflags(MENUSTR *allmenus,int itemnr,int  item_flags);//for dialogbin
void showitemborders(MENUSTR *allmenus,int itemnr);
void setfontnr(MENUSTR *allmenus,int itemnr,int fontid);
int  menuitem_ISVISIBLED(MENUSTR *allmenus,int itemnr);
int  menuitem_ISDISABLED(MENUSTR *allmenus,int itemnr);
int  menuitem_ISENABLED(MENUSTR *allmenus,int itemnr);
void setmenuitem_VISIBLED(MENUSTR *allmenus,int itemnr,int mybool);
void setmenuitem_DISABLED(MENUSTR *allmenus,int itemnr,int mybool);
void setmenuitem_RESPONDEVENTS(MENUSTR *allmenus,int itemnr,int mybool);
void showprogress(int x,int y,int sizex,int percent,int colornr,GRPFILE *grp);
void drawtextpercent(MENUSTR *allmenus,int nr,char *mes,int fontnr,int colorgrp,int colortext);

void addlistboxitem(MENUSTR *allmenus,int nr,int hotx,int hoty,
			int hotsizex,int hotsizey,int textx,int texty,
			int fontnr,unsigned int colors4,int decorcolor);
void listbox_addtext(MENUSTR *allmenus,int nr,char *text);
void setlistbox_lists(MENUSTR *allmenus,int nr,int selectednr,mylist *listbox);
void setlistbox_lists(MENUSTR *allmenus,int nr,int selectednr,mylist *listbox,
			void (*func)(MENUSTR *allmenus,int menuitemnr,int listnr));
void drawlistboxitem(MENUSTR *allmenus,int itemnr);

int  changelistbox_selectednr(MENUSTR *allmenus,int itemnr,int delta);
void changelistbox_fromto(MENUSTR *allmenus,int itemnr,int delta);
void dellistboxitem(MENUSTR *allmenus,int nr,int end,int typeofdel);
void calclistboxbuttonbarpos(MENUSTR *allmenus,int nr);
void changelistbox_buttonbarposdelta(MENUSTR *allmenus,int nr,int mouseypos);
void changelistbox_buttonbarpos(MENUSTR *allmenus,int nr,int mouseypos);
void listboxlineitems(MENUSTR *allmenus,int nr,int lines,int pixelsbetweenlines);
int  getlistbox_selecteditem(MENUSTR *allmenus,int nr);
char *getlistbox_selecteditemname(MENUSTR *allmenus,int nr);
void RemoveMenu_Callback(MENUSTR *allmenus);
void AddMenu_SomeCallback(MENUSTR *allmenus,int (*func)(MENUSTR *,void *),void *somecallbackdata);
void changemenuitemtype(MENUSTR *allmenus,int nr,int newitemtype,int fontnr,int colors4);
int  glu_editmenu(MENUSTR *prevmenu,char *dialogbinfile,char *errormessage,char *fonttable,GRPFILE *dlg,char *str,int maxstrlen);
int  RunCallBackFuncs(MENUSTR *allmenus);
int  drawmenu_ONETICK(MENUSTR *allmenus);
int  drawmenu_LASTTICK(MENUDRAW *menudraw);
int  showlistmenu_ONETICK(MENUSTR *allmenus);
void showlistmenu_LASTTICK(MENUSTR *allmenus);

extern mylist ALLMENUS;




#endif /* _MENU_W*/
