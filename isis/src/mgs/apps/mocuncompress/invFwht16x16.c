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
static char *sccsid = "@(#)invFwht16x16.c	1.1 10/04/99";
#if (!defined(NOSCCSID) && (!defined(LINT)))
#endif
/*
* DESCRIPTION
* 	This module calcutes a "sequency" ordered, two dimensional
* 	inverse Walsh-Hadamard transform (WHT) on 16 x 16 blocks of
* 	data.  It is done as two one dimensional transforms (one of the
* 	rows followed by one of the columns).  Each one dimensional
* 	transform is implemented as a 16 point, 4 stage "butterfly".
*
* COMMENTARY
*	These routines have been highly optimized to produce very fast
*	32000 executable code but still use C for coding (thus allowing
*	compilation on other machines).
*/

#include <stdio.h>

#include "fs.h"

#include "invFwht16x16.h"

extern void exit();

/*
* This defines a four input (and output), two stage "butterfly"
* calculation done completely in registers (once the data is read from
* memory.  Four input and two stages was picked to maximize the use of
* the 32000's registers.  Eight of these are required to do a 16 point,
* one dimensional WHT.  The "simple" formulas for this "butterfly" are:
*
*	n0 = i0 + i1
*	n1 = i0 - i1	First stage
*	n2 = i2 + i3
*	n3 = i2 - i3
*
*	o0 = n0 + n2
*	o1 = n1 + n3	Second stage
*	o2 = n0 - n2
*	o3 = n1 - n3
*
* All data (in and out) is assumed to be 16 bit integers.  "in" is the
* base address of the input data array and "ii" is the scaling factor to
* use on the next four indexes into "in" (this allows moving by rows or
* columns through a two dimensional array stored as a one dimensional set
* of numbers).  "i0", "i1", "i2", and "i3" are the unscaled indexes into
* "in".  "out" is the base address of the output data array and "oi" is
* the scaling factor to use on the next four indexes into "out" (this
* allows moving by rows or columns through a two dimensional array stored
* as a one dimensional set of numbers).  "o0", "o1", "o2", and "o3" are
* the unscaled indexes into "out".
*/

#define BUTTERFLY4(in,ii,i0,i1,i2,i3,out,oi,o0,o1,o2,o3)	\
	{							\
		register int32 t0,t1,t2,t3,t4;			\
								\
		/* Load input into registers */			\
		t0 = in[(ii)*(i0)];				\
		t1 = in[(ii)*(i1)];				\
		t2 = in[(ii)*(i2)];				\
		t3 = in[(ii)*(i3)];				\
								\
		/* Do first stage */				\
		t4 = t0;					\
		t4 += t1;					\
		t0 -= t1;					\
								\
		t1 = t2;					\
		t1 += t3;					\
		t2 -= t3;					\
								\
		/* Do second stage */				\
		t3 = t4;					\
		t3 += t1;					\
		t4 -= t1;					\
								\
		t1 = t0;					\
		t1 += t2;					\
		t0 -= t2;					\
								\
		/* Store results from registers */		\
		out[(oi)*(o0)] = t3;				\
		out[(oi)*(o1)] = t1;				\
		out[(oi)*(o2)] = t4;				\
		out[(oi)*(o3)] = t0;				\
	}

static void invFwht16_row(in,out) register int32 *in,*out; {
/*
* This function does a 16 point, one dimensional inverse WHT on 16, 32
* bit integers stored as a vector (as in the rows of a two dimensional
* array) and puts the results in a 32 bit integer vector.  The transform
* is not normalized but is in "sequency" order.
*
* pre:
*	"in" - the 16 inputs stored as 32 bit integers in a vector.
* post:
*	"out" - the 16 outputs stored as 32 bit integers in a vector.
*/
int32 data[32];			/* Temporary storage used between stages */
register int32 *tmp;		/* Register pointer to the temporary storage */

	/* Point at temporary storage */
	tmp = data;

	/* Perform first two stages of 16 point butterfly */
	BUTTERFLY4(in , 1, 0, 1, 2, 3,tmp, 1, 0, 1, 2, 3);
	BUTTERFLY4(in , 1, 4, 5, 6, 7,tmp, 1, 4, 5, 6, 7);
	BUTTERFLY4(in , 1, 8, 9,10,11,tmp, 1, 8, 9,10,11);
	BUTTERFLY4(in , 1,12,13,14,15,tmp, 1,12,13,14,15);

	/*
	* Perform last two stages of 16 point butterfly and store in
	* "sequency" order
	*/
	BUTTERFLY4(tmp, 1, 0, 4, 8,12,out, 1, 0, 3, 1, 2);
	BUTTERFLY4(tmp, 1, 1, 5, 9,13,out, 1,15,12,14,13);
	BUTTERFLY4(tmp, 1, 2, 6,10,14,out, 1, 7, 4, 6, 5);
	BUTTERFLY4(tmp, 1, 3, 7,11,15,out, 1, 8,11, 9,10);
}

static void invFwht16_col(in,out) register int32 *in,*out; {
/*
* This function does a 16 point, one dimensional inverse WHT on 16, 32
* bit integers stored as a vector in every 16th location (as in the
* columns of a two dimensional array stored as a one dimensional array by
* rows) and puts the results out in a similar manner.  The transform is
* not normalized but is in "sequency" order.
*
* pre:
*	"in" - the 16 inputs stored as 32 bit integers in every 16th location.
* post:
*	"out" - the 16 outputs stored as 32 bit integers in every 16th location.
*/
int32 data[16];			/* Temporary storage used between stages */
register int32 *tmp;		/* Register pointer to the temporary storage */

	/* Point at temporary storage */
	tmp = data;

	/* Perform first two stages of 16 point butterfly */
	BUTTERFLY4(in ,16, 0, 1, 2, 3,tmp, 1, 0, 1, 2, 3);
	BUTTERFLY4(in ,16, 4, 5, 6, 7,tmp, 1, 4, 5, 6, 7);
	BUTTERFLY4(in ,16, 8, 9,10,11,tmp, 1, 8, 9,10,11);
	BUTTERFLY4(in ,16,12,13,14,15,tmp, 1,12,13,14,15);

	/*
	* Perform last two stages of 16 point butterfly and store in
	* "sequency" order
	*/
	BUTTERFLY4(tmp, 1, 0, 4, 8,12,out,16, 0, 3, 1, 2);
	BUTTERFLY4(tmp, 1, 1, 5, 9,13,out,16,15,12,14,13);
	BUTTERFLY4(tmp, 1, 2, 6,10,14,out,16, 7, 4, 6, 5);
	BUTTERFLY4(tmp, 1, 3, 7,11,15,out,16, 8,11, 9,10);
}

void invFwht16x16(in,out) register int16 *in,*out; {
/*
* This function does a "sequency" ordered WHT on a 16 x 16 array of data
* (stored as 16 bit integers) stored in 256 contiguous locations.  The
* transform is normalized.  The input is assumed to be 16 bit signed
* integers EXCEPT for the DC entry which is be treated as UNSIGNED.  The
* result is stored in a 16 x 16 array of the same structure.  The output
* is all 8 bit, unsigned integers.
*
* pre:
*	"in" - the 16 x 16 input block data stored as 16 bit integers.
* post:
*	"out" - the 16 x 16 output block data stored as 16 bit integers.
*/
uint32 i;			/* Generic looping variable */
int32 data[256];		/* Temporary storage for transform */

	/* Convert 16 bit integers to 32 bit integers */
	{
	register int16 *scanIn;
	register int32 *scanData;

	scanIn = in;
	scanData = data;

	*(scanData++) = (uint16)(*(scanIn++));

	for (i = 1; i < 256; i++) {
		*(scanData++) = *(scanIn++);
	};
	};

	{
	register int32 *scanData;	/* Current row start in "data" */

	/*
	* Pass each row in "data" array (as a vector of size 16) to the
	* 16 point, 1D inverse WHT and store the results in contiguous
	* 16 point locations in "data".  At completion all rows have been
	* inverse transformed in one dimension.
	*/
	for (i = 0, scanData = data; i < 16; i++, scanData += 16) {
		invFwht16_row(scanData,scanData);
	};
	};

	{
	register int32 *scanData;	/* Current column start in "data" */

	/*
	* Inverse transform each column in the 16 x 16 block stored by rows
	* as a 256 point vector.
	*/
	for (i = 0, scanData = data; i < 16; i++, scanData++) {
		invFwht16_col(scanData,scanData);
	};
	};

	/* Convert 32 bit integers to 16 bit integers */
	{
	register int32 *scanData;
	register int16 *scanOut;

	scanData = data;
	scanOut = out;

	for (i = 0; i < 256; i++) {
	register int32 cur;
		cur = *(scanData++);


		cur >>= 8;

		if (cur < 0) {
			cur = 0;
		};

		if (cur > 255) {
			cur = 255;
		};

		*(scanOut++) = cur;
	};
	};
}
