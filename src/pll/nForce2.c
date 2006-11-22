/*
*  C Implementation: nForce2
*
* Description: nForce2
*
* Based on code of cpufreq-nforce2.c by Sebastian Witt <se.witt@gmx.net>
*
*
* Author: Nikolay Kislitsa <deusexbeer@mail.ru>, (C) 2005
*
* Copyright: See COPYING file that comes with this distribution
*
*/

#ifdef DEBUG
#include <stdio.h>
#endif

typedef struct
{
	unsigned char fsb;
	unsigned char ctrlb;
	float pci;
} PLL_t;

static int FSBIndex = 0;

static const PLL_t const pll[] =
{
	{0}
};

int nForce2_SetFSB(int fsb)
{
	return -1;
}

int nForce2_GetFSB()
{
	return -1;
}

int nForce2_CheckFSB(int fsb, float *sdram, float *pci, float *agp)
{
	int i;

	if(sdram) *sdram = -1.0;
	if(pci) *pci = -1.0;
	if(agp) *agp = -1.0;

	if(fsb < 0) return -1;

	for(i=0; pll[i].fsb; i++)
		if(pll[i].fsb == fsb)
		{
			if(pci) *pci = pll[i].pci;
			return 0;
		}

	return -1;
}

int nForce2_GetFirstFSB()
{
	FSBIndex = 0;
  if(pll[FSBIndex].fsb) return pll[FSBIndex].fsb;
  else return -1;
}

int nForce2_GetNextFSB()
{
	FSBIndex++;
	if(pll[FSBIndex].fsb) return pll[FSBIndex].fsb;
	else return -1;
}

