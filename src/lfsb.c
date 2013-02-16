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
#include <string.h>

#define VERSION "0.4.4a"
#define DATE    "15-Feb-2013"

#define CPUGOVERNOR

#include "pll.h"

static void Usage()
{
	printf("lfsb - linux FSB overclocking tool. Version "VERSION", "DATE"\n"
	       "\n"
	       "   Usage: lsfb [options] PLL [FSBFREQ]\n"
	       "\n"
	       "   -h --help         print this message\n"
	       "   -v --version      display software version & license\n"
	       "   -s --supported    print list of supported PLL\n"
	       "   -f --frequencies  print all supported freqs by PLL\n"
	       "   -y --yes          skip confirmation\n"
	       "\n"
	       "example: lfsb -yf ics9248-110 100\n");
	exit(0);
}

static void Version()
{
	printf("lfsb - linux FSB overclocking tool. Version "VERSION", "DATE"\n"
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
	       "LICENSE file for more details.\n");
	exit(0);
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

static Test_t PLLFlags;
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
			PLLFlags    = pll->flags;
			SetFSB      = pll->SetFSB;
			GetFSB      = pll->GetFSB;
			CheckFSB    = pll->CheckFSB;
			GetFirstFSB = pll->GetFirstFSB;
			GetNextFSB  = pll->GetNextFSB;
			return 0;
		}

	return -1;
}

static const char *GetPLLFlags(const Test_t t)
{
	if(t == Tested)
		return "tested";
	else if(t == Testing)
		return "testing";

	return "untested";
}

static void Supported()
{
	int i;

	printf("Supported PLL:\n");
	for(i=0; PLL[i].name[0]; i++)
	{
		printf("%s (%s)\n", PLL[i].name, GetPLLFlags(PLL[i].flags));
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
			printf(", RAM=%.2f MHz", sdram);
		if(pci > 0)
			printf(", PCI=%.2f MHz", pci);
		if(agp > 0)
			printf(", AGP=%.2f MHz", agp);
		printf("\n");
	}
}

#ifdef CPUGOVERNOR
static char Governor[256];
static void GetGovernor(char *gov, size_t govlen)
{
	FILE *file;
	char *ch;

	file = fopen("/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor", "rt");
	if(!file)
		return;
	memset(gov, 0, govlen);
	fread(gov, 1, govlen-1, file);
	fclose(file);
	if((ch = strrchr(gov, '\n')))
		*ch = 0;
}

static void SetGovernor(const char *gov)
{
	char cmd[256];

	if(!gov)
		return;

	snprintf(cmd, sizeof(cmd), "cpufreq-set -g %s &> /dev/null", gov);
	if(system(cmd))
		printf("cpufreq-set failed\n");
	else
		printf("CPU governor '%s' set.\n", gov);
}

static void Exit()
{
	printf("\n");
	SetGovernor(Governor);
	usleep(300000);
}
#endif

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
	printf("\n");
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

#ifdef CPUGOVERNOR
	GetGovernor(Governor, sizeof(Governor));
	SetGovernor("performance");
	atexit(Exit);
#endif

	printf("\n");
	PrintCPUInfo();

	if(!pllname)
		return 0;

	if(SetPLL(pllname))
	{
		printf("PLL %s is not suppported\n", pllname);
		return 0;
	}
	else
		printf("PLL %s is supported (%s)\n", pllname, GetPLLFlags(PLLFlags));

	PrintFSBInfo(0);
	if(freqs)
		PrintSupportedFrequencies();

	if(fsbfreq)
	{
		printf("\n");
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

		printf("\n");
		PrintCPUInfo();
		PrintFSBInfo(fsb);
	}

	return 0;
}
