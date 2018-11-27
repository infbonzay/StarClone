
#if !defined(_KEY_W)
#define _KEY_W

#include <stdint.h>
#define KEYPRESS(ikey) (keystatus[ikey]==KEYPRESSED)

//================================
void keyscroll(void);
int	 readkey(void);
void keyhandler(void);

extern uint8_t *keystatus;

//================================
#endif

