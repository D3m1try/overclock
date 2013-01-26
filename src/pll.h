//
// C++ Interface: pll
//
// Description: PLL IC functions
//
//
// Author: Nikolay Kislitsa <deusexbeer@mail.ru>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef PLL_H
#define PLL_H

//ics9148-04
int ics9148_04_SetFSB(int fsb);
int ics9148_04_GetFSB();
int ics9148_04_CheckFSB(int fsb, float *sdram, float *pci, float *agp);
int ics9148_04_GetFirstFSB();
int ics9148_04_GetNextFSB();

//ics9148-26
int ics9148_26_SetFSB(int fsb);
int ics9148_26_GetFSB();
int ics9148_26_CheckFSB(int fsb, float *sdram, float *pci, float *agp);
int ics9148_26_GetFirstFSB();
int ics9148_26_GetNextFSB();

//ics9148-36
int ics9148_36_SetFSB(int fsb);
int ics9148_36_GetFSB();
int ics9148_36_CheckFSB(int fsb, float *sdram, float *pci, float *agp);
int ics9148_36_GetFirstFSB();
int ics9148_36_GetNextFSB();

//ics9148-58
int ics9148_58_SetFSB(int fsb);
int ics9148_58_GetFSB();
int ics9148_58_CheckFSB(int fsb, float *sdram, float *pci, float *agp);
int ics9148_58_GetFirstFSB();
int ics9148_58_GetNextFSB();

//ics9150-08
int ics9150_08_SetFSB(int fsb);
int ics9150_08_GetFSB();
int ics9150_08_CheckFSB(int fsb, float *sdram, float *pci, float *agp);
int ics9150_08_GetFirstFSB();
int ics9150_08_GetNextFSB();

//ics9248-39
int ics9248_39_SetFSB(int fsb);
int ics9248_39_GetFSB();
int ics9248_39_CheckFSB(int fsb, float *sdram, float *pci, float *agp);
int ics9248_39_GetFirstFSB();
int ics9248_39_GetNextFSB();

//ics9248-64
int ics9248_64_SetFSB(int fsb);
int ics9248_64_GetFSB();
int ics9248_64_CheckFSB(int fsb, float *sdram, float *pci, float *agp);
int ics9248_64_GetFirstFSB();
int ics9248_64_GetNextFSB();

//ics9248-110
int ics9248_110_SetFSB(int fsb);
int ics9248_110_GetFSB();
int ics9248_110_CheckFSB(int fsb, float *sdram, float *pci, float *agp);
int ics9248_110_GetFirstFSB();
int ics9248_110_GetNextFSB();

//ics9248-114
int ics9248_114_SetFSB(int fsb);
int ics9248_114_GetFSB();
int ics9248_114_CheckFSB(int fsb, float *sdram, float *pci, float *agp);
int ics9248_114_GetFirstFSB();
int ics9248_114_GetNextFSB();

//ics9248-141
int ics9248_141_SetFSB(int fsb);
int ics9248_141_GetFSB();
int ics9248_141_CheckFSB(int fsb, float *sdram, float *pci, float *agp);
int ics9248_141_GetFirstFSB();
int ics9248_141_GetNextFSB();

//ics9248-153
int ics9248_153_SetFSB(int fsb);
int ics9248_153_GetFSB();
int ics9248_153_CheckFSB(int fsb, float *sdram, float *pci, float *agp);
int ics9248_153_GetFirstFSB();
int ics9248_153_GetNextFSB();

//ics9248-199
int ics9248_199_SetFSB(int fsb);
int ics9248_199_GetFSB();
int ics9248_199_CheckFSB(int fsb, float *sdram, float *pci, float *agp);
int ics9248_199_GetFirstFSB();
int ics9248_199_GetNextFSB();

//ics9250-08
int ics9250_08_SetFSB(int fsb);
int ics9250_08_GetFSB();
int ics9250_08_CheckFSB(int fsb, float *sdram, float *pci, float *agp);
int ics9250_08_GetFirstFSB();
int ics9250_08_GetNextFSB();

//ics94215
int ics94215_SetFSB(int fsb);
int ics94215_GetFSB();
int ics94215_CheckFSB(int fsb, float *sdram, float *pci, float *agp);
int ics94215_GetFirstFSB();
int ics94215_GetNextFSB();

//ics94228
int ics94228_SetFSB(int fsb);
int ics94228_GetFSB();
int ics94228_CheckFSB(int fsb, float *sdram, float *pci, float *agp);
int ics94228_GetFirstFSB();
int ics94228_GetNextFSB();

//nForce2
int nForce2_SetFSB(int fsb);
int nForce2_GetFSB();
int nForce2_CheckFSB(int fsb, float *sdram, float *pci, float *agp);
int nForce2_GetFirstFSB();
int nForce2_GetNextFSB();

//cy28331
int cy28331_SetFSB(int fsb);
int cy28331_GetFSB();
int cy28331_CheckFSB(int fsb, float *sdram, float *pci, float *agp);
int cy28331_GetFirstFSB();
int cy28331_GetNextFSB();

//ics9lprs477dkl
int ics9lprs477_SetFSB(int fsb);
int ics9lprs477_GetFSB();
int ics9lprs477_CheckFSB(int fsb, float *sdram, float *pci, float *agp);
int ics9lprs477_GetFirstFSB();
int ics9lprs477_GetNextFSB();

//slg8sp513
int slg8sp513_SetFSB(int fsb);
int slg8sp513_GetFSB();
int slg8sp513_CheckFSB(int fsb, float *sdram, float *pci, float *agp);
int slg8sp513_GetFirstFSB();
int slg8sp513_GetNextFSB();

//////////////
typedef struct
{
	char *name;
	unsigned char flags;

	int (*SetFSB)(int fsb);
	int (*GetFSB)();
	int (*CheckFSB)(int fsb, float *sdram, float *pci, float *agp);
	int (*GetFirstFSB)();
	int (*GetNextFSB)();
} PLL_t;

//flags
#define   Tested  0x01
#define UnTested  0x02
#define  Testing  0x04

const PLL_t const PLL[] =
{
	{"ics9148-04",  UnTested, ics9148_04_SetFSB, ics9148_04_GetFSB, ics9148_04_CheckFSB, ics9148_04_GetFirstFSB, ics9148_04_GetNextFSB},
	{"ics9148-26",  UnTested, ics9148_26_SetFSB, ics9148_26_GetFSB, ics9148_26_CheckFSB, ics9148_26_GetFirstFSB, ics9148_26_GetNextFSB},
	{"ics9148-36",  UnTested, ics9148_36_SetFSB, ics9148_36_GetFSB, ics9148_36_CheckFSB, ics9148_36_GetFirstFSB, ics9148_36_GetNextFSB},
	{"ics9148-58",  UnTested, ics9148_58_SetFSB, ics9148_58_GetFSB, ics9148_58_CheckFSB, ics9148_58_GetFirstFSB, ics9148_58_GetNextFSB},
	{"ics9150-08",  UnTested, ics9150_08_SetFSB, ics9150_08_GetFSB, ics9150_08_CheckFSB, ics9150_08_GetFirstFSB, ics9150_08_GetNextFSB},
	{"ics9248-39",  UnTested, ics9248_39_SetFSB, ics9248_39_GetFSB, ics9248_39_CheckFSB, ics9248_39_GetFirstFSB, ics9248_39_GetNextFSB},
	{"ics9248-64",  UnTested, ics9248_64_SetFSB, ics9248_64_GetFSB, ics9248_64_CheckFSB, ics9248_64_GetFirstFSB, ics9248_64_GetNextFSB},
	{"ics9248-110",   Tested, ics9248_110_SetFSB, ics9248_110_GetFSB, ics9248_110_CheckFSB, ics9248_110_GetFirstFSB, ics9248_110_GetNextFSB},
	{"ics9248-114", UnTested, ics9248_114_SetFSB, ics9248_114_GetFSB, ics9248_114_CheckFSB, ics9248_114_GetFirstFSB, ics9248_114_GetNextFSB},
	{"ics9248-141", UnTested, ics9248_141_SetFSB, ics9248_141_GetFSB, ics9248_141_CheckFSB, ics9248_141_GetFirstFSB, ics9248_141_GetNextFSB},
	{"ics9248-153", UnTested, ics9248_153_SetFSB, ics9248_153_GetFSB, ics9248_153_CheckFSB, ics9248_153_GetFirstFSB, ics9248_153_GetNextFSB},
	{"ics9248-199", UnTested, ics9248_199_SetFSB, ics9248_199_GetFSB, ics9248_199_CheckFSB, ics9248_199_GetFirstFSB, ics9248_199_GetNextFSB},
	{"ics9250-08",  UnTested, ics9250_08_SetFSB, ics9250_08_GetFSB, ics9250_08_CheckFSB, ics9250_08_GetFirstFSB, ics9250_08_GetNextFSB},
	{"ics94215",    UnTested, ics94215_SetFSB, ics94215_GetFSB, ics94215_CheckFSB, ics94215_GetFirstFSB, ics94215_GetNextFSB},
	{"ics94228",    UnTested, ics94228_SetFSB, ics94228_GetFSB, ics94228_CheckFSB, ics94228_GetFirstFSB, ics94228_GetNextFSB},
	{"cy28331",      Testing, cy28331_SetFSB, cy28331_GetFSB, cy28331_CheckFSB, cy28331_GetFirstFSB, cy28331_GetNextFSB},
	{"ics9lprs477dkl", Testing, ics9lprs477_SetFSB, ics9lprs477_GetFSB, ics9lprs477_CheckFSB, ics9lprs477_GetFirstFSB, ics9lprs477_GetNextFSB},
	{"slg8sp513", Testing, slg8sp513_SetFSB, slg8sp513_GetFSB, slg8sp513_CheckFSB, slg8sp513_GetFirstFSB, slg8sp513_GetNextFSB},
	{""}
};

#endif

