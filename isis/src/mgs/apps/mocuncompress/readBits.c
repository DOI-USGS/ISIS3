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
static char *sccsid = "@(#)readBits.c	1.1 10/04/99";

/*
    This is a low-level bit stream reader.  It's adapted from a version
    by Terry Ligocki with the SCCS id "@(#)readBits.c (readBits.c) 1.3".

    This version has been modified to read from memory rather than from
    a stdio stream; the old code is retained and can be used by defining
    FILEBITS at the top of readBits.h, but this HAS NOT BEEN TESTED.
    There are known problems with reading a block too far in this mode
    anyway.  Use with caution.

    Mike Caplinger, June 1989.
*/

#include <stdio.h>
#include <setjmp.h>
#include <stdlib.h>

#include "fs.h"

#include "readBits.h"

extern void exit();

extern jmp_buf on_error;

#ifdef FILEBITS
BITSTRUCT *initBits(file) FILE *file; {
BITSTRUCT *bitStuff;

	if ((bitStuff = (BITSTRUCT *)malloc(sizeof(*bitStuff))) == NULL) {
		(void)fprintf(stderr,"Unable to allocate bit structure\n");
		longjmp(on_error, 1);
	};

	bitStuff->bitQueue = 0;
	bitStuff->bitCount = 0;
	bitStuff->file = file;
	bitStuff->byteCount = 0;
	bitStuff->queueSize = 0;

	return(bitStuff);
}
#else
BITSTRUCT *initBits(data, len) uint8 *data; {
BITSTRUCT *bitStuff;

	if ((bitStuff = (BITSTRUCT *)malloc(sizeof(*bitStuff))) == NULL) {
		(void)fprintf(stderr,"Unable to allocate bit structure\n");
		longjmp(on_error, 1);
	};

	bitStuff->bitQueue = 0;
	bitStuff->bitCount = 0;
	bitStuff->byteCount = 0;
	bitStuff->byteQueue = data;
	bitStuff->queueSize = len;

	return(bitStuff);
}
#endif

uint32 readBits(bitCount,bitStuff) register uint8 bitCount; register BITSTRUCT *bitStuff; {
register uint32 bitQueue;
register uint32 bitQueueCount;
register uint32 bits;

	if (bitCount > 24) {
		(void)fprintf(stderr,"Asked for more than 24 bits: %d\n",bitCount);
		exit(1);
	};

	bitQueue      = bitStuff->bitQueue;
	bitQueueCount = bitStuff->bitCount;

	if (bitCount > bitQueueCount) {
	uint32 byteCount; 
	uint8 *byteQueue; 
	uint32 queueSize; 

		byteCount = bitStuff->byteCount;
		byteQueue = bitStuff->byteQueue;
		queueSize = bitStuff->queueSize;

		while (bitQueueCount < 24) {
			if (byteCount == queueSize) {
#ifdef FILEBITS
			        if ((queueSize = fread((char *)byteQueue,sizeof(*byteQueue),MAXQUEUESIZE,bitStuff->file)) == NULL) {
#else
				if (1) {
#endif
					if (bitQueueCount >= bitCount) {
						byteCount = 0;
						break;
					} else {
						(void)fprintf(stderr,"Unable to read %d bits\n",bitCount);
						longjmp(on_error, 1);
					};
				};
				byteCount = 0;
			};
			bitQueue |= byteQueue[byteCount++] << bitQueueCount;
			bitQueueCount += 8;
		};

		bitStuff->byteCount = byteCount;
		bitStuff->queueSize = queueSize;
	};

	bits = bitQueue & (((uint32)0xFFFFFFFF) >> (32 - bitCount));
	bitQueue >>= bitCount;
	bitQueueCount -= bitCount;

	bitStuff->bitQueue = bitQueue;
	bitStuff->bitCount = bitQueueCount;

	return(bits);
}
