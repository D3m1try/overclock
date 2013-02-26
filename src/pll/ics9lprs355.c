/*
 *  C Implementation: ics9lprs355
 *
 * Description: ics9lprs355
 *
 *
 * Author: Nikolay Kislitsa <deusexbeer@gmail.com>, (C) 2013
 *
 * Copyright: See COPYING file that comes with this distribution
 *
 */
#include "i2c.h"
#include <math.h>

#ifdef DEBUG
#include <stdio.h>
#endif

typedef struct
{
	unsigned char fsb;
	unsigned char ctrlb;
	unsigned char ctrlb1;
} PLL_t;
#define BYTECOUNT    22
#define CONTROLBYTE  13
#define CONTROLBYTE1 14
#define CMD 0x00

static int FSBIndex = 0;

static const PLL_t const pll[] =
{
	/* Table containing three values
	 * FSB, CONTROLBYTE, CONTROLBYTE1
	 * Gathered by setting FSB using setfsb, then reviewing the Diagnosis
	 * screen to identify what data changed
	 */
	{ 112, 0x88, 0x71},
	{ 117, 0x88, 0x87},
	{ 125, 0x88, 0xA3},
	{ 133, 0x88, 0xBF},
	{ 142, 0x88, 0xDB},
	{ 150, 0x88, 0xF7},
	{ 158, 0x48, 0x13},
	{ 167, 0x48, 0x2F},
	{ 175, 0x48, 0x4B},
	{ 183, 0x48, 0x67},
	{0}
};

static int ics9lprs355_unhide(const int file)
{
	int res;
	unsigned char buf[BYTECOUNT];

	res = i2c_smbus_read_block_data(file, CMD, buf);
	if(res < 0)
		return -1;
#ifdef DEBUG
	else
		printf("unhide DEBUG: %i bytes read : ", res);
	for(int i=0; i<res; i++)
		printf("%02X ", buf[i]);
	printf("\n");
#endif /* DEBUG */

	if(res != 32)
	{
		buf[12] = 0x20;
		res = i2c_smbus_write_block_data(file, CMD, 13, buf);
		if(res)
			return -1;
#ifdef DEBUG
		printf("unhide DEBUG: %i bytes written : ", 13);
		for(int i=0; i<BYTECOUNT; i++)
			printf("%02X ", buf[i]);
		printf("\n");
#endif /* DEBUG */
	}
	else
		return 0;

	res = i2c_smbus_read_block_data(file, CMD, buf);
	if(res < 0)
		return -1;
#ifdef DEBUG
	printf("unhide DEBUG: %i bytes read : ", res);
	for(int i=0; i<res; i++)
		printf("%02X ", buf[i]);
	printf("\n");
#endif /* DEBUG */

	if(res == 0x20 && buf[12] == 0x20)
		return 0;

	return -1;
}

int ics9lprs355_SetFSB(int fsb)
{
	/* default FSB=133.33Mhz table *after* setfsb has done it's TME-workaround
	 * default obtained by clicking setfsb's "Get FSB", then without changing
	 * any of the sliders, click  "Set FSB". Copy 22 bytes of data here
	 */

	int i, file, res;
	unsigned char buf[] = {0x37, 0x85, 0xFC, 0x33, 0xFF, 0xF0, 0x90, 0x11,\
	0xD0, 0x65, 0x7D, 0x00, 0x0D, 0x88, 0xBF, 0xEF,\
	0x2F, 0xA4, 0xF7, 0xF2, 0x23, 0x01 };

	if(fsb < 0) return -1;

	for(i=0; pll[i].fsb; i++)
		if(pll[i].fsb == fsb)
		{
			buf[CONTROLBYTE] = pll[i].ctrlb;
			buf[CONTROLBYTE1] = pll[i].ctrlb1;
			break;
		}
		if(!buf[CONTROLBYTE]) return -1;

		file = i2c_open();
	if(file < 0) return -1;
	res = i2c_smbus_write_block_data(file, CMD, BYTECOUNT, buf);
	res = i2c_smbus_read_block_data(file, CMD, buf);
	if  (buf[CONTROLBYTE]!=pll[i].ctrlb ||  buf[CONTROLBYTE1]!=pll[i].ctrlb1)       {
		printf("ERROR: Frozen PLL - standby/resume and try again\n");
		exit (-1);
	}

	i2c_close();

	if(res < 0) return -1;
					 #ifdef DEBUG
					 else printf("DEBUG: %i bytes written : ", BYTECOUNT);
		for(i=0; i<BYTECOUNT; i++) printf("%02X ", buf[i]);
		printf("\n");
	#endif /* DEBUG */

	return 0;
}

int ics9lprs355_GetFSB()
{
	int i, file, res;
	/* Empty 22 byte buffer */
	unsigned char buf[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

	file = i2c_open();
	if(file < 0) return -1;
	res = i2c_smbus_read_block_data(file, CMD, buf);
	i2c_close();

	if(res < 0) return -1;
	#ifdef DEBUG
	else printf("DEBUG: %i bytes read : ", res);
	for(i=0; i<res; i++) printf("%02X ", buf[i]);
	printf("\n");
	#endif /* DEBUG */

	for(i=0; pll[i].fsb; i++)
		if(pll[i].ctrlb == buf[CONTROLBYTE]) return pll[i].fsb;

		return -1;
}

int ics9lprs355_CheckFSB(int fsb, float *sdram, float *pci, float *agp)
{
	int i;

	if(sdram) *sdram = -1.0;
	if(pci) *pci = -1.0;
	if(agp) *agp = -1.0;

	if(fsb < 0) return -1;

	for(i=0; pll[i].fsb; i++)
		if(pll[i].fsb == fsb) return 0;
		return -1;
}

int ics9lprs355_GetFirstFSB()
{
	FSBIndex = 0;
	if(pll[FSBIndex].fsb) return pll[FSBIndex].fsb;
	else return -1;
}

int ics9lprs355_GetNextFSB()
{
	FSBIndex++;
	if(pll[FSBIndex].fsb) return pll[FSBIndex].fsb;
	else return -1;
}

int ics9lprs355_CheckTME()
{
	int i, file, res;
	unsigned char buf[BYTECOUNT];

	file = i2c_open();
	if(file < 0)
		return -1;

	ics9lprs355_unhide(file);
	res = i2c_smbus_read_block_data(file, CMD, buf);
	i2c_close();

#ifdef DEBUG
	printf("GetFSB DEBUG: %i bytes read : ", res);
	for(i=0; i<res; i++)
		printf("%02X ", buf[i]);
	printf("\n");
#endif /* DEBUG */
	if(res < 0)
		return -1;

	if(buf[9] & 0x40)
		return 1;

	return 0;
}
