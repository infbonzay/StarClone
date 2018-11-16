#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "wmem.h"

#include "objlists.h"
//=========================================
int OBJlists::AddCriteries(int(*func)(int), int maxelem)
{
	if (CritFunc >= MAXCRITFUNC)
	{
		return(-1);//error not enouth memory for criteries
	}
	if (maxelem == -1)
		maxelements[CritFunc] = MAXELEMOFFSET;
	else
		maxelements[CritFunc] = maxelem;
	elems[CritFunc] = (int *)wmalloc(maxelements[CritFunc]);
	memset(elems[CritFunc], 0, maxelements[CritFunc]);
	seekpos[CritFunc] = 0;
	curelements[CritFunc] = 0;
	AllCriteriesFunc[CritFunc++] = func;
	return(CritFunc - 1);
}
//=========================================
void OBJlists::AddElem(int elem_id)
{
	int i;
	for (i = 0;i < CritFunc;i++)
	{
		if ((*AllCriteriesFunc[i])(elem_id))//if criteries is good need to memorize that
		{
			if (curelements[i] >= maxelements[i])
			{
				printf("maxelements reached\n");
				return;
			}
			elems[i][curelements[i]] = elem_id;
			++curelements[i];
		}
	}
}
//=========================================
void OBJlists::SetFirstSeek(int criteria_id)
{
	if (criteria_id < MAXCRITFUNC)
		seekpos[criteria_id] = 0;
}
//=========================================
int OBJlists::FindNextElem(int criteria_id)
{
	int retval;
	if (seekpos[criteria_id] >= curelements[criteria_id])
		return(-1);
	retval = elems[criteria_id][seekpos[criteria_id]];
	seekpos[criteria_id]++;
	return(retval);
}
//=========================================
void OBJlists::DelElem(int elem_id)
{
	int i, j;
	for (i = 0;i < CritFunc;i++)
	{
		if ((*AllCriteriesFunc[i])(elem_id))//if criteries is good
		{
			for (j = 0;j < curelements[i];j++)
			{
				if (elems[i][j] == elem_id)
				{
					if (j != curelements[i] - 1)
					{
						elems[i][j] = elems[i][curelements[i] - 1];
					}
					curelements[i]--;
				}
			}
		}
	}

}
//=========================================
void OBJlists::ClearAll(void)
{
	int i;
	for (i = 0;i < CritFunc;i++)
	{
		curelements[i] = 0;
		maxelements[i] = 0;
		if (elems[i])
			wfree(elems[i]);
		elems[i] = NULL;
	}
	CritFunc = 0;
}
//=========================================
OBJlists::~OBJlists()
{
	ClearAll();
}
