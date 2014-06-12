
/*
 *  C Implementation: cv179
 *
 * Description: cv179
 *
 *
 * Author: Andrey Zhornyak <darion76@gmail.com>, (C) 2014
 *
 * Copyright: See COPYING file that comes with this distribution
 *
 */
#include "i2c.h"

#ifdef DEBUG
#include <stdio.h>
#endif

typedef struct
{
	unsigned int fsb;
	unsigned char ctrlb;
	float pci;
} PLL_t;
#define BYTECOUNT   20
#define CONTROLBYTE 17
#define CMD 0x00

static int FSBIndex = 0;

static const PLL_t const pll[] =
{
	{80, 0x78, 33.3},
	{85, 0x80, 33.3},
	{90, 0x88, 33.3},
	{95, 0x8E, 33.3},
	{100, 0x96, 33.3},
	{105, 0x9E, 33.3},
	{110, 0xA5, 33.3},
	{115, 0xAE, 33.3},
	{120, 0xB8, 33.3},
	{125, 0xBC, 33.3},
	{130, 0xC3, 33.3},
	{135, 0xC9, 33.3},
	{145, 0xDA, 33.3},
	{150, 0xF4, 33.3},
	{166, 0xFA, 33.3},
	{0}
};

static int cv179_unhide(const int file)
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

	if(res != BYTECOUNT)
	{
		buf[12] = 0x19;
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

	if(res == 0x19 && buf[12] == 0x19)
		return 0;

	return -1;
}

int cv179_SetFSB(int fsb)
{
	int i, file, res;
	unsigned char buf[BYTECOUNT], ctrl=0;

	if(fsb < 0)
		return -1;

	for(i=0; pll[i].fsb; i++)
		if(pll[i].fsb == fsb)
		{
			ctrl = pll[i].ctrlb;
			break;
		}
	if(!ctrl)
		return -1;

	file = i2c_open();
	if(file < 0)
		return -1;

	cv179_unhide(file);
	res = i2c_smbus_read_block_data(file, CMD, buf);
#ifdef DEBUG
	printf("SetFSB DEBUG: %i bytes read : ", res);
	for(i=0; i<res; i++)
		printf("%02X ", buf[i]);
	printf("\n");
#endif /* DEBUG */
	if((res != BYTECOUNT) && (res != 25))
	{
		i2c_close();
		return -1;
	}

	buf[CONTROLBYTE] = ctrl;
	res = i2c_smbus_write_block_data(file, CMD, 20, buf);
//	res = i2c_smbus_write_block_data(file, CMD, BYTECOUNT, buf);
	i2c_close();
	if(res){
#ifdef DEBUG
	printf("SetFSB DEBUG: wrire error. code=%i", res);
#endif /* DEBUG */
		return -1;
	}

#ifdef DEBUG
	printf("SetFSB DEBUG: %i bytes written : ", BYTECOUNT);
	for(i=0; i<res; i++)
		printf("%02X ", buf[i]);
	printf("\n");
#endif /* DEBUG */

	return 0;
}

int cv179_GetFSB()
{
	int i, file, res;
	unsigned char buf[BYTECOUNT];

	file = i2c_open();
	if(file < 0)
		return -1;

	cv179_unhide(file);
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

	for(i=0; pll[i].fsb; i++)
		if(pll[i].ctrlb == buf[CONTROLBYTE])
			return pll[i].fsb;

	return -1;
}

int cv179_CheckFSB(int fsb, float *sdram, float *pci, float *agp)
{
	int i;

	if(sdram)
		*sdram = -1.0;
	if(pci)
		*pci = -1.0;
	if(agp)
		*agp = -1.0;

	if(fsb < 0)
		return -1;

	for(i=0; pll[i].fsb; i++)
		if(pll[i].fsb == fsb)
		{
			if(pci)
				*pci = pll[i].pci;
			return 0;
		}

	return -1;
}

int cv179_GetFirstFSB()
{
	FSBIndex = 0;
	if(pll[FSBIndex].fsb)
		return pll[FSBIndex].fsb;
	else
		return -1;
}

int cv179_GetNextFSB()
{
	FSBIndex++;
	if(pll[FSBIndex].fsb)
		return pll[FSBIndex].fsb;
	else
		return -1;
}
