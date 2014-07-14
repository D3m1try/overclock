/*
 *  C Implementation: ics9lpr427
 *
 * Description: ics9lpr427
 *
 *
 * Author: Andrey Zhornyak <darion76@gmail.com>, (C) 2014
 *
 * Copyright: See COPYING file that comes with this distribution
 *
 */
#include "i2c.h"
#include <math.h>

#ifdef DEBUG
#include <stdio.h>
#endif

#define BYTECOUNT 31
#define CMD 0x00

static int FSBIndex = 0;
static const unsigned int FSB_Min = 100;
static const unsigned int FSB_Max = 400;

static int ics9lpr427_unhide(const int file)
{
	int res;
	unsigned char buf[BYTECOUNT];

	res = i2c_smbus_read_block_data(file, CMD, buf);
	if(res < 0)
		return -1;
#ifdef DEBUG
	else
		printf("unhide DEBUG: %i bytes read :    ", res);
		for(int i=0; i<res; i++)
			printf("%02X ", buf[i]);
		printf("\n");
#endif /* DEBUG */

	if(res != 32)
	{
		buf[8] = 0x1F;
		res = i2c_smbus_write_block_data(file, CMD, 13, buf);
		if(res)
			return -1;
#ifdef DEBUG
			printf("unhide DEBUG: %i bytes written : ", 13);
			for(int i=0; i<13; i++)
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
	printf("unhide DEBUG: %i bytes read :    ", res);
	for(int i=0; i<res; i++)
		printf("%02X ", buf[i]);
	printf("\n");
#endif /* DEBUG */

	if(res == 0x20 && buf[8] == 0x1F)
	return 0;

	return -1;
}


int ics9lpr427_CheckFSB(int fsb, float *ram, float *pci, float *agp)
{
	if(ram)
		*ram = fsb;
	if(pci)
		*pci = -1.0f;
	if(agp)
		*agp = -1.0f;

	if(fsb <= FSB_Max && fsb >= FSB_Min)
		return 0;

	return -1;
}

static const int const DivTable[] =
{
	0x02, 0x03, 0x05, 0x07,
	0x04, 0x06, 0x0A, 0x0E,
	0x08, 0x0C, 0x14, 0x1C,
	0x10, 0x18, 0x28, 0x38
};

static int ics9lpr427_CalcFSB(const unsigned int n, const unsigned int m, const unsigned int dividx)
{
	return 150.0f * (float)n / (float)m / (float)DivTable[dividx & 0x0F];
}

int ics9lpr427_SetFSB(int fsb)
{
	int file, res;
	unsigned int n, m;
	unsigned char buf[32];

	if(fsb > FSB_Max || fsb < FSB_Min)
		return -1;

	file = i2c_open();
	if(file < 0)
		return -1;

	res = i2c_smbus_read_block_data(file, CMD, buf);
	if(res != BYTECOUNT)
	{
#ifdef DEBUG
		printf("SetFSB DEBUG: %i (should be %i) bytes read : ", res, BYTECOUNT);
		for(int i=0; i<res; i++)
			printf("%02X ", buf[i]);
		printf("\n");
#endif /* DEBUG */
		i2c_close();
		return -1;
	}
#ifdef DEBUG
	else
	{
		printf("SetFSB DEBUG: %i bytes read : ", res);
		for(int i=0; i<res; i++)
			printf("%02X ", buf[i]);
		printf("\n");
	}
#endif /* DEBUG */

	n = (buf[12] << 2) | ((buf[11] & 0xC0) >> 6);
	m = buf[11] & 0x3F;
	if(ics9lpr427_CheckFSB(ics9lpr427_CalcFSB(n, m, buf[28]), NULL, NULL, NULL))
	{
#ifdef DEBUG
		printf("SetFSB DEBUG: Read FSB out of range.\nStopping.\n");
#endif /* DEBUG */
		i2c_close();
		return -1;
	}

//	m = 18;
	n = (fsb * m * DivTable[(buf[28] & 0x0F)])/150;

	buf[11] = m & 0x3F;
	buf[11] |= (n & 0x03) << 6;
	buf[12] = (n >> 2) & 0xFF;
//	buf[28] = 0x00;

	res = i2c_smbus_write_block_data(file, CMD, BYTECOUNT, buf);
	i2c_close();

 if(res < 0)
	 return -1;
#ifdef DEBUG
  else
		printf("SetFSB DEBUG: %i bytes written : ", BYTECOUNT);
  for(int i=0; i<BYTECOUNT; i++)
		printf("%02X ", buf[i]);
  printf("\n");
#endif /* DEBUG */

	return 0;
}

int ics9lpr427_GetFSB()
{
	int file, res;
	unsigned int n, m;
	unsigned char buf[32];

	file = i2c_open();
	if(file < 0)
		return -1;
	ics9lpr427_unhide(file);
	res = i2c_smbus_read_block_data(file, CMD, buf);
	i2c_close();

	if(res < 0) return -1;
#ifdef DEBUG
	else
	{
		printf("GetFSB DEBUG: %i bytes read : ", res);
		for(int i=0; i<res; i++)
			printf("%02X ", buf[i]);
		printf("\n");
	}
#endif /* DEBUG */

	n = (buf[12] << 2) | ((buf[11] & 0xC0) >> 6);
	m = buf[11] & 0x3F;

#ifdef DEBUG
	printf("GetFSB DEBUG: n=%i, m=%i, idx=%i \n", n, m, DivTable[(buf[28] & 0x0F)]);
#endif /* DEBUG */


	return ics9lpr427_CalcFSB(n, m, buf[28]);
}

int ics9lpr427_GetFirstFSB()
{
	FSBIndex = FSB_Min;

	return FSBIndex;
}

int ics9lpr427_GetNextFSB()
{
	FSBIndex++;

	if(FSBIndex <= FSB_Max)
		return FSBIndex;

	return -1;
}

int ics9lpr427_CheckTME()
{
	int file, res;
	unsigned char buf[BYTECOUNT];

	file = i2c_open();
	if(file < 0)
		return -1;

	ics9lpr427_unhide(file);
	res = i2c_smbus_read_block_data(file, CMD, buf);
	i2c_close();

#ifdef DEBUG
	printf("CheckTME DEBUG: %i bytes read : ", res);
	for(int i=0; i<res; i++)
		printf("%02X ", buf[i]);
	printf("\n");
#endif /* DEBUG */
	if(res < 0)
		return -1;

	if(buf[1] & 0x01)
		return 1;

	return 0;
}
