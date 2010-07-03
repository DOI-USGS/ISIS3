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
static char *sccsid = "@(#)decompNONE.c	1.1 10/04/99";
#if (!defined(NOSCCSID) && (!defined(LINT)))
#endif

#include "fs.h"
#include "bitsOut.h"
#include "nextValue.h"

#include "decompNONE.h"

void decompNONE(curLine,size,code,left,right,bitStuff) register uint8 *curLine; register uint32 size; uint8 *code,*left,*right; BITSTRUCT *bitStuff; {
register uint32 bitCount;
register uint8 *data;
register uint8 cur;

	data     = bitStuff->output;
	bitCount = bitStuff->bitCount;

	cur = (*data) >> bitCount;

	/* Decode all pixels in this line */
	while (size > 0) {
		/* Store decoded value */
		nextValue(*curLine,code,left,right,cur,bitCount,data);
		curLine++;
		size--;
	};

	bitStuff->output     = data;
	bitStuff->bitCount = bitCount;
}

#ifdef TEST

void initReverse(trans) uint8 *trans; {
/*
* This function generate the bit reversal array "trans" above
*
* pre:
*
* post:
*	The array "tran" contains the bit reversal of each index in that
*	index's location (e.g. trans[0x05] = 0xa0).
*/
register uint32 t;		/* Current index */

	/* Do all 8 bit numbers */
	for (t = 0; t < 256; t++) {
	register uint8 r;	/* Reversed byte */
	register uint8 b;	/* Bit count */
	register uint8 mask;	/* Current bit in index */
	register uint8 bit;	/* Current bit in reversed byte */

		/* Reverse all 8 bits */
		r = 0;
		for (b = 0, mask = 0x1, bit = 0x80; b < 8; b++) {
			/*
			* If the bit in index is set, set the corresponding
			* bit in the reversed byte
			*/
			if (t & mask) {
				r |= bit;
			};

			/* Move bit in index up and bit in reversed byte down */
			mask = mask << 1;
			bit  = bit  >> 1;
		};

		/* Store the reversed byte */
		trans[t] = r;
	};
}

void createIdentTree(code,left,right) uint8 *code,*left,*right; {
uint32 index;

	for (index = 0; index < 127; index++) {
		code[index] = LEFT | RIGHT;
		left[index] = 2*index + 1;
		right[index] = 2*index + 2;
	};

	for (index = 127; index < 255; index++) {
		code[index] = 0;
		left[index]  = ~(2*(index - 127));
		right[index] = ~(2*(index - 127) + 1);
	};
}

#define MAXLINE		2048

uint8 reverse[256];
uint8 data[2 * MAXLINE + 4];

uint32 tryNone(code,left,right,bitStuff) uint8 *code,*left,*right; BITSTRUCT *bitStuff; {
uint32 nerror;
uint8 curLine[MAXLINE];
uint32 i;
uint8 known,actual;

	nerror = 0;

	for (i = 0; i < MAXLINE; i++) {
		curLine[i] = 0;
	};

	decompNONE(curLine,MAXLINE,code,left,right,bitStuff);

	for (i = 0; i < MAXLINE; i++) {
		known = ~reverse[(~i) & 0xFF];
		actual = curLine[i];

		if (known != actual) {
			printf("None: Cur pixel:  %4d (%4d %1d), expected: %3d (%02x), got: %3d (%02x)\n",i,bitStuff->data - data,bitStuff->bitCount,known,known,actual,actual);
			nerror += 1;
		};

	};

	for (i = 0; i < MAXLINE; i++) {
		curLine[i] = 0;
	};

	decompNONE(curLine,MAXLINE,code,left,right,bitStuff);

	for (i = 0; i < MAXLINE; i++) {
		known = ~reverse[(~(i+MAXLINE)) & 0xFF];
		actual = curLine[i];

		if (known != actual) {
			printf("None: Cur pixel:  %4d (%4d %1d), expected: %3d (%02x), got: %3d (%02x)\n",i,bitStuff->data - data,bitStuff->bitCount,known,known,actual,actual);
			nerror += 1;
		};
	};

	return(nerror);
}

main() {
uint32 nerror;
uint8 code[256];
uint8 left[256];
uint8 right[256];
BITSTRUCT bitStuff;
uint32 i;

	/* No errors yet */
	nerror = 0;

	printf("Test started\n");

	initReverse(reverse);

	createIdentTree(code,left,right);

	/* Check decompression */

	for (i = 0; i < 2*MAXLINE + 4; i++) {
		data[i] = ~i;
	};

	bitStuff.data = data;
	bitStuff.bitQueue = 0;
	bitStuff.bitCount = 0;
	nerror = tryNone(code,left,right,&bitStuff);

	/* If no errors print message */
	if (nerror == 0) {
		printf("Test succeeded\n");
	} else {
		printf("Test failed: %d\n",nerror);
	};

	/* Exit with the number of errors */
	exit(nerror);
}
#endif
