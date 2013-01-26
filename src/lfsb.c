/*
*  C Implementation: lfsb
*
* Description: FSB speed manipulating programm.
*
*
* Author: Nikolay Kislitsa <deusexbeer@gmail.com>, (C) 2013
*
* Copyright: See COPYING file that comes with this distribution
*
*/
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <inttypes.h>
#include <strings.h>
#include <getopt.h>

#define VERSION "0.4.3d"
#define DATE    "26-Jan-2013"

#include "pll.h"

static void Usage()
{
	printf("lfsb - FSB speed manipulating programm. Version "VERSION", "DATE"\n"
			"\n"
			"   Usage: lsfb [options] PLL [FSBFREQ]\n"
			"\n"
			"   -h --help         print this message\n"
			"   -v --version      display software version & license\n"
			"   -s --supported    print list of supported PLL\n"
			"   -f --frequencies  print all supported freqs by PLL\n"
			"   -y --yes          skip confirmation\n"
			"\n"
			"example: lfsb -yf ics9248-110 100\n"
			"\n");
}

static void Version()
{
	printf("lfsb - FSB speed manipulating programm. Version "VERSION", "DATE"\n"
	       "\n"
	       "Copyright (C) 2013 by Nikolay Kislitsa <deusexbeer@gmail.com>\n"
	       "\n"
	       "This program is free software; you can redistribute it and/or modify\n"
	       "it under the terms set out in the LICENSE file, which is included\n"
	       "in the source distribution.\n"
	       "\n"
	       "This program is distributed in the hope that it will be useful,\n"
	       "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
	       "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
	       "LICENSE file for more details.\n"
	       "\n");
}

static inline int64_t rdtsc(void)
{
	unsigned int i, j;
	__asm__ __volatile__("rdtsc" : "=a"(i), "=d"(j) : );
	return ((int64_t)j<<32) + (int64_t)i;
}

static float GetCPUFreq()
{
	int64_t tsc_start, tsc_end;
	struct timeval tv_start, tv_end;
	int usec_delay;

	tsc_start = rdtsc();
	gettimeofday(&tv_start, NULL);

	usleep(300000);

	tsc_end = rdtsc();
	gettimeofday(&tv_end, NULL);

	usec_delay = 1000000 * (tv_end.tv_sec - tv_start.tv_sec) + (tv_end.tv_usec - tv_start.tv_usec);

	return ((float)(tsc_end-tsc_start) / usec_delay);
}

static int (*SetFSB)(int fsb);
static int (*GetFSB)();
static int (*CheckFSB)(int fsb, float *sdram, float *pci, float *agp);
static int (*GetFirstFSB)();
static int (*GetNextFSB)();

static int SetPLL(const char *name)
{
	const PLL_t *pll;

	for(pll=PLL; pll->name[0]; pll++)
		if(!strcasecmp(name, pll->name))
		{
			SetFSB      = pll->SetFSB;
			GetFSB      = pll->GetFSB;
			CheckFSB    = pll->CheckFSB;
			GetFirstFSB = pll->GetFirstFSB;
			GetNextFSB  = pll->GetNextFSB;
			return 0;
		}

	return -1;
}

static void Supported()
{
	int i;

	printf("\nSupported PLL:\n");
	for(i=0; PLL[i].name[0]; i++)
	{
		printf("%s", PLL[i].name);
		if(PLL[i].flags & UnTested)
			printf("(untested)");
		else if(PLL[i].flags & Tested)
			printf("(tested)");
		else if(PLL[i].flags & Testing)
			printf("(testing)");
		printf("\n");
	}
	printf("\n");
}

void PrintCPUInfo()
{
	printf("CPU frequency : %.2f MHz (estimated)\n", GetCPUFreq());
}

void PrintFSBInfo(int fsb)
{
	float sdram, pci, agp;

	if(!fsb)
		fsb = GetFSB();
	if(!CheckFSB(fsb, &sdram, &pci, &agp))
	{
		printf("FSB=%i MHz", fsb);
		if(sdram > 0)
			printf(", SDRAM=%.2f MHz", sdram);
		if(pci > 0)
			printf(", PCI=%.2f MHz", pci);
		if(agp > 0)
			printf(", AGP=%.2f MHz", agp);
		printf("\n");
	}
}

void PrintSupportedFrequencies()
{
	int fsb, prev, start;

	printf("\nSupported FSB frequencies (MHz):\n");
	fsb = GetFirstFSB();
	start = 0;
	prev = 0;
	while(fsb > 0)
	{
		if(fsb - prev == 1)
		{
			if(!start)
				start = prev;
		}
		else
		{
			if(start)
			{
				printf("-%i", prev);
				start = 0;
			}
			else
				printf(" %i", fsb);
		}
		prev = fsb;
		fsb = GetNextFSB();
	}
	if(start)
		printf("-%i", prev);
	printf("\n\n");
}

int main(int argc, char *argv[])
{
	int c, fsb, freqs=0, yes=0;
	char *pllname=NULL, *fsbfreq=NULL;
	struct option long_options[] =
	{
		{"help", 0, 0, 'h'},
		{"version", 0, 0, 'v'},
		{"supported", 0, 0, 's'},
		{"frequencies", 0, 0, 'f'},
		{"yes", 0, 0, 'y'},
		{0, 0, 0, 0}
	};

	while((c = getopt_long(argc, argv, "hvsfy", long_options, NULL)) != -1)
	{
		switch(c)
		{
			case 'h': Usage(); break;
			case 'v': Version(); break;
			case 's': Supported(); break;
			case 'f': freqs = 1; break;
			case 'y': yes = 1; break;
			default: Usage(); return 0; break;
		}
	}
	while(optind < argc)
	{
		if(!pllname)
			pllname = argv[optind];
		else
			fsbfreq = argv[optind];
		optind++;
	}

	printf("-------------------------------------------------------------\n");
	PrintCPUInfo();

	if(!pllname)
		return 0;

	if(SetPLL(pllname))
	{
		printf("PLL %s is not suppported.\n", pllname);
		return 0;
	}
	else
		printf("PLL %s is supported.\n", pllname);

	PrintFSBInfo(0);
	if(freqs)
		PrintSupportedFrequencies();

	if(fsbfreq)
	{
		printf("-------------------------------------------------------------\n");
		fsb = atoi(fsbfreq);
		if(CheckFSB(fsb, NULL, NULL, NULL))
		{
			printf("FSB %i MHz is not supported.\n", fsb);
			return 0;
		}
		printf("Changing to:\n");
		PrintFSBInfo(fsb);

		if(!yes)
		{
			printf("WARNING: Too high FSB/PCI/AGP speed can DAMAGE your hardware!\nContinue [y/N]: ");
			yes = getc(stdin);
			if(yes != 'y' && yes != 'Y')
			{
				printf("Aborted.\n");
				return 0;
			}
		}

		if(SetFSB(fsb))
			perror("Can't change frequency, SetFSB error");
		else
			printf("FSB frequency changed.\n");

		printf("-------------------------------------------------------------\n");

		PrintCPUInfo();
		PrintFSBInfo(fsb);
	}
	printf("-------------------------------------------------------------\n");

	return 0;
}
