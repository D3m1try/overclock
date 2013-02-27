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

#define BYTECOUNT 22
#define CMD 0x00

static int FSBIndex = 0;
static const unsigned int FSB_Min = 100;
static const unsigned int FSB_Max = 400;

/*
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
*/

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

int ics9lprs355_CheckFSB(int fsb, float *ram, float *pci, float *agp)
{
	if(ram)
		*ram = -1.0f;
	if(pci)
		*pci = -1.0f;
	if(agp)
		*agp = -1.0f;

	if(fsb <= FSB_Max && fsb >= FSB_Min)
		return 0;

	return -1;
}

int ics9lprs355_SetFSB(int fsb)
{
	return -1;
}

int ics9lprs355_GetFSB()
{
	int file, res;
	unsigned int n, m;
	unsigned char buf[32];

	file = i2c_open();
	if(file < 0)
		return -1;
	res = i2c_smbus_read_block_data(file, CMD, buf);
	i2c_close();

	if(res < 0) return -1;
	#ifdef DEBUG
	else
	{
		printf("DEBUG: %i bytes read : ", res);
		for(int i=0; i<res; i++)
			printf("%02X ", buf[i]);
		printf("\n");
	}
	#endif /* DEBUG */

	n = buf[14] | ((buf[13] & 0x80) << 1) | ((buf[13] & 0x40) << 3);
	m = buf[13] & 0x3F;
	//printf("n=%i, m=%i, n/m=%f, 14.318x(n+8)/(m+2)=%f\n", n, m, (double)n/(double)m, 14.318f*(double)(n+8)/(double)(m+2));

	return (int)(14.318f*(float)n/(float)m);
}

int ics9lprs355_GetFirstFSB()
{
	FSBIndex = FSB_Min;

	return FSBIndex;
}

int ics9lprs355_GetNextFSB()
{
	FSBIndex++;

	if(FSBIndex <= FSB_Max)
		return FSBIndex;

	return -1;
}

int ics9lprs355_CheckTME()
{
	int file, res;
	unsigned char buf[BYTECOUNT];

	file = i2c_open();
	if(file < 0)
		return -1;

	ics9lprs355_unhide(file);
	res = i2c_smbus_read_block_data(file, CMD, buf);
	i2c_close();

#ifdef DEBUG
	printf("GetFSB DEBUG: %i bytes read : ", res);
	for(int i=0; i<res; i++)
		printf("%02X ", buf[i]);
	printf("\n");
#endif /* DEBUG */
	if(res < 0)
		return -1;

	if(buf[9] & 0x40)
		return 1;

	return 0;
}