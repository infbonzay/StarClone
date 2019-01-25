#ifndef _SC_RAND_H

#define _SC_RAND_H

int	 myrand(void);
int	 myrand(int value);
int	 myrand(int from,int to);			  //get random numbers from..to included
void setseed(int value);
int	 getseed(void);
int	 my2rand(int numb1,int numb2);


#endif /* _RAND_W */
