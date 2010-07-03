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
static char *sccsid = "@(#)xmain.c	1.1 10/04/99";

/*
    MOC transform decompressor main routine
    Mike Caplinger, MOC GDS Design Scientist
    SCCS @(#)main.c	1.2 1/5/94

    Adapted from a version by Terry Ligocki with SCCS
    @(#)decompress.c (decompress.c) 1.6
*/

#include <stdio.h>
#include <memory.h>
#include <setjmp.h>
#include <stdlib.h>

#include "fs.h"
#include "types.h"

#include "readBits.h"
#include "readGroups.h"
#include "initBlock.h"
#include "readBlock.h"

extern void exit();

jmp_buf on_error;

uint8 *transform_decomp_main(data, len, height, width, transform, spacing, numLevels)
uint8 *data;
int height, width;
uint32 transform;
uint32 spacing;
uint32 numLevels;
{

uint32 xSize = width, ySize = height;
uint32 level;
uint32 numBlocks;
uint8 *image;
int32 hsize;
int32 header[3];
BITSTRUCT *bitStuff;
uint32 *groups;
uint32 *occ;
uint32 x,y;
uint32 var[256];
int used;

	bitStuff = initBits(data, len);

	if ((image = (uint8 *)malloc((uint32)(xSize * ySize * sizeof(*image)))) == NULL) {
		(void)fprintf(stderr,"Not enough memory for image\n");
		return;
	};

	if ((occ = (uint32 *)malloc((uint32)(numLevels * sizeof(*occ)))) == NULL) {
		(void)fprintf(stderr,"Not enough memory to decoding of image\n");
		return;
	};

	for (level = 0; level < numLevels; level++) {
		occ[level] = 0;
	};

	if(setjmp(on_error)) {
	    goto out;
	}

	numBlocks = (xSize * ySize) >> 8;

	groups = readGroups(numBlocks,bitStuff);

	{
	uint32 block;
	uint32 *scanGroups;

	scanGroups = groups;

	for (block = 0; block < numBlocks; block++) {
		if (*scanGroups >= numLevels) {
			(void)fprintf(stderr,"Group level too large: %d > %d\n",*scanGroups,numLevels-1);
			bzero(image, width*height);
			return image;
		};

		occ[*(scanGroups++)]++;
	};
	};

	initBlock();

	for (level = 0; level < numLevels; level++) {
		if (occ[level] != 0) {
		uint16 minDC,maxDC,rangeDC;
		uint32 *scanGroups,*scanVar;
		uint32 i;

			minDC = readBits(16,bitStuff);
			maxDC = readBits(16,bitStuff);

			rangeDC = maxDC - minDC;

			scanVar = var+1;

			for (i = 1; i < 256; i++) {
				*(scanVar++) = readBits(3,bitStuff);
			};

			scanGroups = groups;

			for (x = 0; x < xSize; x += 16) {
				for (y = 0; y < ySize; y += 16) {
					if (*(scanGroups++) == level) {
						readBlock(transform,spacing,minDC,rangeDC,var,x,y,xSize,image,bitStuff);
					};
				};
			};
		};
	};

	/* note that under some normal circumstances byteCount can get
	   reset to 0 -- this is OK, problems will be indicated by an
	   EOF from readBits above somewhere. */
	used = bitStuff->byteCount;
	if(used != len && used > 0) {
	    fprintf(stderr, "Error: only used %d bytes out of %d\n",
		    used, len);
	}
out:	free(occ);
	freeAllTrees();
	return image;
}
