/*
 *  C Implementation: ics9lprs477
 *
 * Description: ics9lprs477
 *
 *
 * Author: Nikolay Kislitsa <deusexbeer@mail.ru>, (C) 2005
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

static int FSBIndex = 0;
static const unsigned int FSB_Min = 200;
static const unsigned int FSB_Max = 300;

int ics9lprs477_SetFSB(int fsb)
{
	int i, file, res;
	unsigned int n, m;
	unsigned char buf[32];

	if(fsb > FSB_Max || fsb < FSB_Min) return -1;

	file = i2c_open();
	if(file < 0) return -1;

	res = i2c_smbus_read_block_data(file, CMD, buf);
	if(res != BYTECOUNT)
	{
#ifdef DEBUG
		printf("DEBUG: %i (should be %i) bytes read : ", res, BYTECOUNT);
		for(i=0; i<res; i++) printf("%02X ", buf[i]);
		printf("\n");
#endif /* DEBUG */
		i2c_close();
		return -1;
	}
#ifdef DEBUG
	else
	{
		printf("DEBUG: %i bytes read : ", res);
		for(i=0; i<res; i++) printf("%02X ", buf[i]);
		printf("\n");
	}
#endif /* DEBUG */

	n = fsb << 1;
	m = 50;

	buf[16] = m & 0x3F;
	buf[16] |= (n & 0x06) << 5;
	buf[17] = (n >> 3) & 0xFF;

	res = i2c_smbus_write_block_data(file, CMD, BYTECOUNT, buf);
	i2c_close();

 if(res < 0) return -1;
#ifdef DEBUG
  else printf("DEBUG: %i bytes written : ", BYTECOUNT);
  for(i=0; i<BYTECOUNT; i++) printf("%02X ", buf[i]);
  printf("\n");
#endif /* DEBUG */

	return 0;
}

int ics9lprs477_GetFSB()
{
	int i, file, res;
	unsigned int n, m;
	unsigned char buf[32];

	file = i2c_open();
	if(file < 0) return -1;
	res = i2c_smbus_read_block_data(file, CMD, buf);
	i2c_close();

	if(res < 0) return -1;
#ifdef DEBUG
	else
	{
		printf("DEBUG: %i bytes read : ", res);
		for(i=0; i<res; i++) printf("%02X ", buf[i]);
		printf("\n");
	}
#endif /* DEBUG */

	n = (buf[17] << 3) | (buf[16] & 0xC0) >> 5;
	m = buf[16] & 0x3F;

	return (int)(25.0f*(float)n/(float)m);
}

int ics9lprs477_CheckFSB(int fsb, float *sdram, float *pci, float *agp)
{
	if(sdram) *sdram = -1.0;
	if(pci) *pci = -1.0;
	if(agp) *agp = -1.0;
	if(fsb <= FSB_Max && fsb >= FSB_Min) return 0;
	return -1;
}

int ics9lprs477_GetFirstFSB()
{
	FSBIndex = FSB_Min;
	return FSBIndex;
}

int ics9lprs477_GetNextFSB()
{
	FSBIndex++;
	if(FSBIndex <= FSB_Max) return FSBIndex;
	else return -1;
}