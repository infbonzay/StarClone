#if !defined(_GRDOSMOUSE_W)
#define _GRDOSMOUSE_W 1

#define WMLEFTKEY  1
#define WMRIGHTKEY 2

int mouseinit(void);
int mousebuttons(void);
void GetMouseMove(void);

extern  int WMouseMoveRelativX,WMouseMoveRelativY;
extern  char mousedetect;
#endif

