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
static char *sccsid = "@(#)pmain.c	1.2 03/30/00";

/*
* DESCRIPTION
*
* COMMENTARY
*/

#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include <math.h>

#include "fs.h"
#include "predCompCommon.h"
#include "bitsOut.h"

#include "predictiveDecompressor.h"

extern void exit();

#define BUFFERSIZE	4096

/* Huffman tree (in table form) */
uint8 code[256];
uint8 left[256];
uint8 right[256];

int pred_past_eof;

/*
    NOTE: the globals code, left, and right should be set up prior
    to calling this routine.
*/
uint8 *predictive_decomp_main(data, len, height, width,
                              doSync, sync, xpred, ypred, got_height)
uint8 *data;
int len;
uint8 doSync;     /* Use sync pattern */
uint16 sync;      /* Sync pattern to use */
int *got_height;
{
  uint8 *curLine;     /* Current line */
  uint8 *prevLine;    /* Previous line */
  BITSTRUCT bitStuff;   /* Output bit stream structure */
  uint8 compType = 0;   /* Type of compression */
  uint32 decodeSize;    /* Size of huffman tables */
  uint32 y;     /* Looping variable */
  uint32 index;     /* Indexing variable */
  uint8 *result;
  uint8 *lastsync;
  uint16 gotsync;
  extern int errors;
  extern uint8 *findsync(uint8 *p, int len, uint16 sync);


  pred_past_eof = 0;

  /* Allocate space for decompression */
  if ((prevLine = (uint8 *)malloc(width * sizeof(*prevLine))) == NULL) {
    (void)fprintf(stderr,"Unable to get enough memory for line buffers\n");
    exit(1);
  };

  for (y = 0; y < width; y++) {
    prevLine[y] = 0;
  };

  if ((curLine = (uint8 *)malloc(width * sizeof(*curLine))) == NULL) {
    (void)fprintf(stderr,"Unable to get enough memory for line buffers\n");
    exit(1);
  };

  if ((result = (uint8 *) malloc(height*width)) == NULL) {
    fprintf(stderr, "can't get memory for output image\n");
    exit(1);
  }

  if (xpred) compType |= XPRED;
  if (ypred) compType |= YPRED;

  bitStuff.output     = data;
  bitStuff.bitQueue = *(bitStuff.output);
  bitStuff.bitCount = 0;

  lastsync = data;

  for (y = 0; y < height; y++) {
    if ((y % 128 == 0) && (doSync == 1)) {
      if (bitStuff.bitCount != 0) {
        bitStuff.bitCount = 0;
        bitStuff.output++;
      };
      if (((bitStuff.output - data) & 0x1) == 0x1) {
        bitStuff.output++;
      };
      bitStuff.bitQueue = *(bitStuff.output);
      /* check sync pattern:
         NOTE this is kind of a funny place to do this,
         but it's simplest to do it here due to the
         structure of the code. mc, 11/11/98 */
      gotsync = *(bitStuff.output) | (*(bitStuff.output+1)<<8);
      if (gotsync != sync) {
        fprintf(stderr, "lost sync, line %d -- ", y);
        errors += 1;
#define AUTOSYNC
#ifdef AUTOSYNC
        if (!(lastsync = (uint8*)findsync(lastsync,
                                  len-(lastsync-data), sync))) {
#else
        if (1) {
#endif
          *got_height = y;
          if (bitStuff.output-data > len) {
            /* we tried to read beyond the end of
               the data. */
            pred_past_eof = 1;
          }
          fprintf(stderr, "aborting\n");
          return result;
        }
        else {
          bitStuff.output = lastsync;
          //fprintf(stderr, "restart at %#x\n", lastsync);
        }
      }
      else {
        lastsync = bitStuff.output;
      }

      predictiveDecompressor(curLine,prevLine,width,compType | SYNC,code,left,right,sync,&bitStuff);
    }
    else {
      predictiveDecompressor(curLine,prevLine,width,compType,code,left,right,sync,&bitStuff);
    };

    bcopy(curLine, result+y*width, width);
  };

  /* Free the temporary storage */
  free((char *)prevLine);
  free((char *)curLine);

  *got_height = height;
  return result;
}
