/*
 *  C Implementation: ics9lprs365
 *
 * Description: ics9lprs365
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

#define BYTECOUNT 32
#define CMD 0x00

static int ics9lprs365_unhide(const int file)
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

int ics9lprs365_SetFSB(int fsb)
{
	return -1;
}

int ics9lprs365_GetFSB()
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

	n = buf[14]| ((buf[13] & 0x80) << 1) | ((buf[13] & 0x40) << 3);
	m = buf[13] & 0x3F;

	return (int)(25.0f*(float)n/(float)m);
}

int ics9lprs365_CheckFSB(int fsb, float *ram, float *pci, float *agp)
{
	if(ram)
		*ram = -1.0;
	if(pci)
		*pci = -1.0;
	if(agp)
		*agp = -1.0;

	if(fsb < 0)
		return -1;

	return -1;
}

int ics9lprs365_GetFirstFSB()
{
	return -1;
}

int ics9lprs365_GetNextFSB()
{
	return -1;
}

int ics9lprs365_CheckTME()
{
	int i, file, res;
	unsigned char buf[BYTECOUNT];

	file = i2c_open();
	if(file < 0)
		return -1;

	ics9lprs365_unhide(file);
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
