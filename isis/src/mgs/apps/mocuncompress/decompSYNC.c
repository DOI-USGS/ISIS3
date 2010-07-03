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
static char *sccsid = "@(#)decompSYNC.c	1.1 10/04/99";
#if (!defined(NOSCCSID) && (!defined(LINT)))
#endif

#include "fs.h"
#include "bitsOut.h"
#include "nextValue.h"

#include "decompSYNC.h"

void decompSYNC(curLine,prevLine,size,sync,bitStuff) register uint8 *curLine,*prevLine; register uint32 size; uint16 sync; BITSTRUCT *bitStuff; {
register uint32 count;		/* Pixel counter */
register uint8 *scan;		/* Pixel copying pointer */

	/* Skip sync pattern (assumes alignment) */
	bitStuff->output += 2;

	/* Prepare to copy data */
	scan = bitStuff->output;

	/* Copy all pixels on this line */
	for (count = 0; count < size; count++) {
	register uint8 cur;		/* Current pixel value */

		/* Get current pixel value */
		cur = *(scan++);

		/* Store the current pixel */
		*(curLine++) = cur;

		/* Store as the next pixel's previous pixel */
		*(prevLine++) = cur;
	};

	/* Update bit stream and align */
	bitStuff->output = scan;
	bitStuff->bitCount = 0;
}

#ifdef TEST

#define MAXLINE		2048

uint8 data[2 * MAXLINE + 4];

uint32 trySync(bitStuff) BITSTRUCT *bitStuff; {
uint32 nerror;
uint16 sync;
uint8 prevLine[MAXLINE];
uint8 curLine[MAXLINE];
uint32 i;
uint8 known,actual;

	nerror = 0;

	sync = 0;

	for (i = 0; i < MAXLINE; i++) {
		prevLine[i] = curLine[i] = 0;
	};

	decompSYNC(curLine,prevLine,MAXLINE,sync,bitStuff);

	for (i = 0; i < MAXLINE; i++) {
		known = ~(i+2);
		actual = curLine[i];

		if (known != actual) {
			printf("Sync: Cur pixel:  %4d (%4d %1d), expected: %3d (%02x), got: %3d (%02x)\n",i,bitStuff->data - data,bitStuff->bitCount,known,known,actual,actual);
			nerror += 1;
		};

		actual = prevLine[i];

		if (known != actual) {
			printf("Sync: Prev pixel: %4d (%4d %1d), expected: %3d (%02x), got: %3d (%02x)\n",i,bitStuff->data - data,bitStuff->bitCount,known,known,actual,actual);
			nerror += 1;
		};
	};

	for (i = 0; i < MAXLINE; i++) {
		prevLine[i] = curLine[i] = 0;
	};

	decompSYNC(curLine,prevLine,MAXLINE,sync,bitStuff);

	for (i = 0; i < MAXLINE; i++) {
		known = ~(i+MAXLINE+4);
		actual = curLine[i];

		if (known != actual) {
			printf("Sync: Cur pixel:  %4d (%4d %1d), expected: %3d (%02x), got: %3d (%02x)\n",i,bitStuff->data - data,bitStuff->bitCount,known,known,actual,actual);
			nerror += 1;
		};

		actual = prevLine[i];

		if (known != actual) {
			printf("Sync: Prev pixel: %4d (%4d %1d), expected: %3d (%02x), got: %3d (%02x)\n",i,bitStuff->data - data,bitStuff->bitCount,known,known,actual,actual);
			nerror += 1;
		};
	};

	return(nerror);
}

main() {
uint32 nerror;
BITSTRUCT bitStuff;
uint32 i;

	/* No errors yet */
	nerror = 0;

	printf("Test started\n");

	/* Check decompression */

	for (i = 0; i < 2*MAXLINE + 4; i++) {
		data[i] = ~i;
	};

	bitStuff.data = data;
	bitStuff.bitQueue = 0;
	bitStuff.bitCount = 0;
	nerror = trySync(&bitStuff);

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
