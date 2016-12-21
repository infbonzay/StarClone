#if !defined(_HANDLERS_W)
#define _HANDLERS_W 1

#define KEYPRESSED 1
#define KEYPRESS(ikey) (keystatus[ikey]==KEYPRESSED)
//==========================================
static void interrupt far new9h(void);
static void interrupt far new8h(void);
int installvectors(void);
void uninstallvectors(void);
void speedtimer(int countdown);

extern volatile char keyactive,lastkey;
extern char keystatus[256];
void keyrefresh(void);

/*extern char GAME;
extern char mousedetect,timemousewait;
extern void scrollmouse(void);
*/

#endif

