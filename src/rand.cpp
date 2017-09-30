
#include <stdlib.h>
#include <limits.h>
#include "rand.h"
#include "mytime.h"

//==========================================
int myseedval;
void setseed(int seedval)
{
    myseedval=seedval;
}
//==========================================
int getseed(void)
{
    return(myseedval);
}
//==========================================
int myrand(void)
{
    return (myrand(INT_MAX));
}
//==========================================
int myrand(int value)
{
    myseedval = ( myseedval*214013 + 2531011 ) & INT_MAX;
    return((myseedval>>16) % value);
}
//==========================================
int myrand(int from,int to)		//get random numbers from..to included
{
    return (myrand(to-from+1)+from);
}
//==========================================
int my2rand(int numb1,int numb2)
{
    if (myrand(2))
	return(numb1);
    return(numb2);
}
//==========================================
