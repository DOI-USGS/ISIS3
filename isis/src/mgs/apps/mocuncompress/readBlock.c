/*
NOTICE

The software accompanying this notice (the "Software") is provided to you
free of charge to facilitate your use of the data collected by the Mars
Orbiter Camera (the "MOC Data").  Malin Space Science Systems ("MSSS")
grants to you (either as an individual or entity) a personal,
non-transferable, and non-exclusive right (i) to use and reproduce the
Software solely for the purpose of accessing the MOC Data; (ii) to modify
the source code of the Software as necessary to maintain or adapt the
Software to run on alternate computer platforms; and (iii) to compile, use
and reproduce the modified versions of the Software solely for the purpose
of accessing the MOC Data.  In addition, you may distribute the Software,
including any modifications thereof, solely for use with the MOC Data,
provided that (i) you must include this notice with all copies of the
Software to be distributed; (ii) you may not remove or alter any
proprietary notices contained in the Software; (iii) you may not charge any
third party for the Software; and (iv) you will not export the Software
without the appropriate United States and foreign government licenses.  

You acknowledge that no title to the intellectual property in the Software
is transferred to you.  You further acknowledge that title and full
ownership rights to the Software will remain the exclusive property of MSSS
or its suppliers, and you will not acquire any rights to the Software
except as expressly set forth above.  The Software is provided to you AS
IS.  MSSS MAKES NO WARRANTY, EXPRESS OR IMPLIED, WITH RESPECT TO THE
SOFTWARE, AND SPECIFICALLY DISCLAIMS THE IMPLIED WARRANTIES OF
NON-INFRINGEMENT OF THIRD PARTY RIGHTS, MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE.  SOME JURISDICTIONS DO NOT ALLOW THE EXCLUSION OR
LIMITATION OF INCIDENTAL OR CONSEQUENTIAL DAMAGES, SO SUCH LIMITATIONS OR
EXCLUSIONS MAY NOT APPLY TO YOU.  

Your use or reproduction of the Software constitutes your agreement to the
terms of this Notice.  If you do not agree with the terms of this notice,
promptly return or destroy all copies of the Software in your possession.  

Copyright (C) 1999 Malin Space Science Systems.  All Rights Reserved.
*/
static char *sccsid = "@(#)readBlock.c	1.1 10/04/99";
#if (!defined(NOSCCSID) && (!defined(LINT)))
#endif
/*
* DESCRIPTION
*
* COMMENTARY
*/

#include "fs.h"

#include "readBits.h"
#include "initBlock.h"
#include "readBlock.h"
#include "readCoef.h"
#include "reorder.h"
#include "invFwht16x16.h"
#include "invFdct16x16.h"

void readBlock(transform,spacing,minDC,rangeDC,var,x,y,xSize,image,bitStuff) uint32 transform,spacing; uint16 minDC,rangeDC; uint32 *var; uint32 x,y,xSize; uint8 *image; BITSTRUCT *bitStuff; {
int16 block[256],*scanBlock,*lastCoef;
uint32 i;
uint32 x0,y0;
uint16 dc;
uint16 numZeros;

	dc = readBits(8,bitStuff);

        /* The following line of code was fixed on March 26, 2004
           by Janet Barrett. The uint16 cast was added to get
           consistent results between Linux and Solaris */
	block[0] = (uint16) ((double)dc * rangeDC / 255.0 + minDC);
	var++;

	numZeros = readBits(8,bitStuff);

	scanBlock = &block[255];

	for (i = 0; i < numZeros; i++) {
		*(scanBlock--) = 0;
	};

	lastCoef = &block[255 - numZeros];

	for (scanBlock = &block[1]; scanBlock <= lastCoef; ) {
		*(scanBlock++) = readCoef(encodeTrees[*(var++)],bitStuff) * spacing;
	};

	scanBlock = block;

	reorder(block);

	scanBlock = block;

	switch (transform) {
	case 0:
		invFwht16x16(block,block);
		break;

	case 1:
		invFdct16x16(block,block);
		break;
	};

	scanBlock = block;

	for (y0 = 0; y0 < 16; y0++) {
		for (x0 = 0; x0 < 16; x0++) {
			image[(y + y0) * xSize + (x + x0)] = *(scanBlock++);
		};
	};
}
