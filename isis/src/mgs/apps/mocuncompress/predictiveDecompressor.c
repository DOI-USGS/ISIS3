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
static char *sccsid = "@(#)predictiveDecompressor.c	1.1 10/04/99";
#if (!defined(NOSCCSID) && (!defined(LINT)))
#endif
/*
* DESCRIPTION
*
*	This module produces a single line of decompressed data from
*	data that has been predictively compressed.  The input is a bit
*	stream, a huffman tree, and the previous line.  The output is a
*	line 8 bit pixels.
*
* COMMENTARY
*/

#include "fs.h"
#include "predCompCommon.h"
#include "bitsOut.h"

#include "decompNONE.h"
#include "decompXPRED.h"
#include "decompYPRED.h"
#include "decompXPREDYPRED.h"
#include "decompSYNC.h"

#include "predictiveDecompressor.h"

void predictiveDecompressor(curLine,prevLine,size,type,code,left,right,sync,bitStuff) register uint8 *curLine,*prevLine; register uint32 size; uint8 type; uint8 *code,*left,*right; uint16 sync; BITSTRUCT *bitStuff; {
/*
* This function produces a single line of decompressed data from data
* that has been predictively compressed.  The input is a bit stream, a
* huffman tree, and the previous line.  The output is a line 8 bit
* pixels.
*
* pre:
*	"curLine" - a place for the decoded, decompressed 8 bit output line.
*	"prevLine" - the previous output line (for vertical decompression).
*	"size" - the size of the above two lines of pixels.
*	"type" - the type of predictive compression done to this line (NOTE:
*		the "REQUANT" bit must not be set).
*	"code" - part of the huffman tree in table form (set above).
*	"left" - part of the huffman tree in table form (set above).
*	"right" - part of the huffman tree in table form (set above).
*	"sync" - the sync pattern.
*	"bitStuff" - the bit stream containing the encoded, compressed data.
*
* post:
*	"curLine" - the decoded, decompressed 8 bit output line.
*	"prevLine" - same a "curLine" (used for next call).
*	"bitStuff" - now references the first bit on the next line of encoded,
*		compressed data.
*/

	/* "Decode" the compression options */
	switch (type) {

	/* No "sync" pattern, no prediction, only encoding */
	case NONE:
		decompNONE(curLine,size,code,left,right,bitStuff);
		break;

	/* No "sync" pattern, crosstrack prediction, and encoding */
	case XPRED:
		decompXPRED(curLine,size,code,left,right,bitStuff);
		break;

	/* No "sync" pattern, downtrack prediction, and encoding */
	case YPRED:
		decompYPRED(curLine,prevLine,size,code,left,right,bitStuff);
		break;

	/* No "sync" pattern, 2D prediction, and encoding */
	case XPRED | YPRED:
		decompXPREDYPRED(curLine,prevLine,size,code,left,right,bitStuff);
		break;

	/* "sync" pattern */
	/* REFINE Should check sync pattern and bit alignment endREFINE */
	case SYNC:
	case XPRED | SYNC:
	case YPRED | SYNC:
	case XPRED | YPRED | SYNC:
		decompSYNC(curLine,prevLine,size,sync,bitStuff);
		break;

	default:
		break;
	};
}

#ifdef TEST

main() {
uint32 nerror;

	/* No errors yet */
	nerror = 0;

	printf("Test started\n");

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
